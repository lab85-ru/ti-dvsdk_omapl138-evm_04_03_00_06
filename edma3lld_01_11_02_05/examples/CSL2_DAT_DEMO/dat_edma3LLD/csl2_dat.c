/*
 * csl2_dat.c
 *
 * Implements the CSL DAT2 API using EDMA3 Driver and Resource Manager Package
 * (EDMA3LLD). Behaviour mimics DAT2 on EDMA2 hardware.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <stdio.h>
#include <csl_dat.h>

/*
 * Includes for accessing EDMA3 DRV and RM layer
 */
#include <ti/sdo/edma3/drv/edma3_drv.h>


/*
 * Include for setting up the EDMA3 LLD
 */
#include <csl2_dat_edma3lld.h>


/*
 * extern declarations
 */

/*
 * OS dependent functions, that must be implemented by user of CSL DAT adapter
 * These functions mark the entry and exit to critical sections of the code
 */
extern void _dat_critical_section_enter();
extern void _dat_critical_section_exit();

/*
 * Flag to indicate if EDMA3 LLD was initialized before calling the DAT APIs
 */
extern int DAT_EDMA3LLD_initCalled;

/*
 * typedef declarations
 */

typedef struct DAT_EDMA3LLD_ChannelDesc {

    /* Holds the param number allocated */
    Uint32 paramNo;

    /* Holds the tcc number allocated */
    Uint32 tccNo;

}  DAT_EDMA3LLD_ChannelDesc;

/*
 * static variable definitions
 */

/*
 * Max size array to hold the allocated resources for DAT
 */
static DAT_EDMA3LLD_ChannelDesc
        DAT_allocatedChannels[DAT_EDMA3LLD_HW_MAXPHYCHANNELS];
/*
 * Flag to indicate that the DAT instance has been opened
 */
static Uint32 DAT_EDMA3LLD_openFlag = 0;

/*
 * 64 bit internal Transfer completion register, used to hold status
 * of the channel
 * It has a bit set to one whenever a Transfer is submitted, and cleared
 * when it completes
 */
static volatile Uint32 TransferCompleteH = 0x0;
static volatile Uint32 TransferCompleteL = 0x0;

/*
 * Set to the last allocated bit index number in the TransferComplete fxn
 */
static Uint32 lastAllocatedIndex = DAT_INVALID_ID;

/*
 * static function declarations
 */

/*
 * Call back function declaration for transfer completion
 */
static void _transferComplete(Uint32 tcc, EDMA3_RM_TccStatus status,
        void *param);

/*
 * Obtain the next free channel from available channels
 */
static inline Uint32 _getFreeChannel(Uint32 *tccNum);

/*
 * Function to setup and enable DAT transfer for given channel number tcc number
 */
static inline Uint32 _setupTransferOptions(Uint32 chNum, Uint32 tccNum);

/*
 * global variable definitions
 */

/*
 * Holds the EDMA3 driver handle
 */
EDMA3_DRV_Handle DAT_EDMA3LLD_hEdma = NULL;

/*
 * number of EDMA3 channels allocated for the DAT module
 */
int DAT_EDMA3LLD_numAllocatedChannels = 0;


/*
 * global function definitions
 */


/*
 *  ======== DAT_open =========
 * Opens the DAT module, called before all other DAT APIs
 * Ignore all paramters of DAT_open, open any channel,
 * all channels are opened with equal priority
 * All channels can be used in 2D mode etc, flags are not applicable
 */
int DAT_open(int chNum, int priority, Uint32 flags) {

    int i = 0;
    int j = 0;
    Uint32 chaNum = DAT_INVALID_ID;
    Uint32 tccNum = DAT_INVALID_ID;

    chNum = chNum;
    priority = priority;
    flags = flags;

    /*
     * Ensure _initCalled is called before DAT_open
     * Also ensure DAT_open is called only once
     */
    _dat_critical_section_enter();

    if (DAT_EDMA3LLD_initCalled == 0) {
        _dat_critical_section_exit();
        return 0;
    }

    if (1 == DAT_EDMA3LLD_openFlag) {
        _dat_critical_section_exit();
        return 0;
    }
    DAT_EDMA3LLD_openFlag = 1;
    _dat_critical_section_exit();

    /*
     * Request default number of channels and Tccs from the EDMA3 DRV package
     */
    for(i=0; i < DAT_EDMA3LLD_numAllocatedChannels; i++) {

        chaNum = EDMA3_DRV_DMA_CHANNEL_ANY;
        tccNum = EDMA3_DRV_TCC_ANY;

        /*
         * EDMA3 DRV call to request for channel and tcc resources
         */
        if (EDMA3_DRV_SOK != EDMA3_DRV_requestChannel(DAT_EDMA3LLD_hEdma,
                &chaNum, &tccNum,
                (EDMA3_RM_EventQueue)DAT_EDMA3LLD_HW_EVT_QUEUE_ID,
                (EDMA3_RM_TccCallback)&_transferComplete, NULL)) {

            /*
             * Error requesting channels, Clean up all channels requested so far
             */
            for(j = i-1; j >=0; j--) {
                EDMA3_DRV_freeChannel(DAT_EDMA3LLD_hEdma,
                    DAT_allocatedChannels[j].paramNo);
                DAT_allocatedChannels[i].paramNo = DAT_INVALID_ID;
                DAT_allocatedChannels[i].tccNo = DAT_INVALID_ID;
            }

            if (EDMA3_DRV_SOK != EDMA3_DRV_close(DAT_EDMA3LLD_hEdma, NULL)){
                printf("Error closing DRV instance \n");
            }
            else {
                if (EDMA3_DRV_SOK != EDMA3_DRV_delete(DAT_EDMA3LLD_HW_INST_ID,
                    NULL)){
                    printf("Error deleting EDMA3 DRV\n");
                }
            }

            DAT_EDMA3LLD_openFlag = 0;
            return 0;
        }

        /*
         * Store the allocated Channels in an array
         */
        DAT_allocatedChannels[i].paramNo = chaNum;
        DAT_allocatedChannels[i].tccNo = tccNum;
    }

    return 1;
}

/*
 *  ======== DAT_close =========
 * Close the DAT module
 */
void DAT_close() {
    int i = 0;

    /*
     * Ensure DAT_open was called
     */
    _dat_critical_section_enter();
    if (DAT_EDMA3LLD_openFlag == 0)
    {
        _dat_critical_section_exit();
        return;
    }
    DAT_EDMA3LLD_openFlag = 0;
    _dat_critical_section_exit();

    /*
     * Wait for all pending transfers to complete
     */
    DAT_wait(DAT_XFRID_WAITALL);

    /*
     * Free all requested channels
     */
    for(i=0; i < DAT_EDMA3LLD_numAllocatedChannels; i++) {
        EDMA3_DRV_freeChannel(DAT_EDMA3LLD_hEdma,
                DAT_allocatedChannels[i].paramNo);
        DAT_allocatedChannels[i].paramNo = DAT_INVALID_ID;
        DAT_allocatedChannels[i].tccNo = DAT_INVALID_ID;
    }

}

/*
 *  ======== DAT_copy =========
 * One dimensional copy from source to destination of byteCnt bytes
 */
Uint32 DAT_copy(void *src, void *dst, Uint16 byteCnt ) {
    Uint32 chNum = 0;
    Uint32 tccNum = 0;
    /*
     * An alternate way to setup the params
     * EDMA3_DRV_PaRAMRegs param;
     * const EDMA3_DRV_PaRAMRegs *newPaRAM = &param;
     */

    /*
     * Obtain a free channel
     * This call spins till a free channel is obtained
     */
    chNum = _getFreeChannel(&tccNum);

    /*
     * Set up Transfer Paramters for this channel
     */
    EDMA3_DRV_setTransferParams(DAT_EDMA3LLD_hEdma, chNum, byteCnt, 1, 1, 0,
            EDMA3_DRV_SYNC_AB);
    EDMA3_DRV_setDestParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int)dst,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);
    EDMA3_DRV_setSrcParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int)src,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);

    /*
     * To set up all the parameters in a single call, can use
     * EDMA3_DRV_PARAMRegs structure, and populate it as indicated

     param.srcAddr = (Uint32)src;
     param.aCnt = (unsigned short) byteCnt;
     param.bCnt = 1;
     param.destAddr = (Uint32)dst;
     param.srcBIdx = (short)0;
     param.destBIdx = (short)0;
     param.linkAddr = DAT_NULL_LINK;
     param.bCntReload = 0x0 ;
     param.srcCIdx = 0x0 ;
     param.destCIdx = 0x0;
     param.cCnt = 0x1;

     param.opt = DAT_OPT_TCC(DAT_OPT_DEFAULT, tccNum);

     if (EDMA3_DRV_SOK  != EDMA3_DRV_setPaRAM(DAT_EDMA3LLD_hEdma, chNum,
         newPaRAM)) {
         printf("Error setting up transfer \n");
         HWI_enable();
         return INVALID_ID;
     }
     */

     return _setupTransferOptions(chNum, tccNum);
}


/*
 *  ======== DAT_copy2d =========
 * 2-dimensional copy from src to dst of lineCnt lines each of length lineLen
 * bytes. The pitch for the second dimension is linePitch bytes
 */
Uint32 DAT_copy2d(Uint32 type, void *src, void *dst, Uint16 lineLen,
        Uint16 lineCnt, Uint16 linePitch) {
    Uint32 chNum = 0;
    Uint32 tccNum = 0;

    /*
     * Obtain a free channel
     */
    chNum = _getFreeChannel(&tccNum);

    /*
     * Set up Transfer Paramters for this channel
     */
    EDMA3_DRV_setSrcParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int)src,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);
    EDMA3_DRV_setDestParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int)dst,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);
    EDMA3_DRV_setTransferParams(DAT_EDMA3LLD_hEdma, chNum, lineLen, lineCnt,
            1, 0, EDMA3_DRV_SYNC_AB);

    /*
     * Depending on type of transfer set the src and dest BIdx
     * Different types of transfers differ only in the Src and Dst BIdx-es
     */
    switch (type) {
        case DAT_1D2D:
            EDMA3_DRV_setSrcIndex(DAT_EDMA3LLD_hEdma, chNum, lineLen, 0);
            EDMA3_DRV_setDestIndex(DAT_EDMA3LLD_hEdma, chNum, linePitch, 0);
            break;
        case DAT_2D1D:
            EDMA3_DRV_setSrcIndex(DAT_EDMA3LLD_hEdma, chNum, linePitch, 0);
            EDMA3_DRV_setDestIndex(DAT_EDMA3LLD_hEdma, chNum, lineLen, 0);
            break;
        case DAT_2D2D:
            EDMA3_DRV_setSrcIndex(DAT_EDMA3LLD_hEdma, chNum, linePitch, 0);
            EDMA3_DRV_setDestIndex(DAT_EDMA3LLD_hEdma, chNum, linePitch, 0);
            break;
    }

    return _setupTransferOptions(chNum, tccNum);
}


/*
 *  ======== DAT_fill =========
 * Fills up dst with byteCnt bytes of the pattern pointed to be 'value'
 */
Uint32 DAT_fill(void *dst, Uint16 byteCnt, Uint32 *value) {
    Uint32 chNum = 0;
    Uint32 tccNum = 0;

    /*
     * Obtain a free channel
     */
    chNum = _getFreeChannel(&tccNum);

    /*
     * Set up Transfer Paramters for this channel
     * For DAT_fill, the SrcIndex is set to zero since src address never changes
     */
    EDMA3_DRV_setSrcParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int )value,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);
    EDMA3_DRV_setDestParams(DAT_EDMA3LLD_hEdma, chNum, (unsigned int)dst,
            EDMA3_DRV_ADDR_MODE_INCR,(EDMA3_DRV_FifoWidth)0);
    EDMA3_DRV_setTransferParams(DAT_EDMA3LLD_hEdma, chNum, 8, byteCnt>>3 ,1,0,
            EDMA3_DRV_SYNC_AB);
    EDMA3_DRV_setSrcIndex(DAT_EDMA3LLD_hEdma, chNum, 0, 0);
    EDMA3_DRV_setDestIndex(DAT_EDMA3LLD_hEdma, chNum, 8, 0);

    return _setupTransferOptions(chNum, tccNum);
}



/*
 *  ======== DAT_wait =========
 * Wait for the transfer identified by waitId, to complete
 */
void DAT_wait(Uint32 waitId) {

    /*
     * If both the registers are zero, we're done !!
     */
    if (0x0 == (TransferCompleteL | TransferCompleteH)) {
        return;
    }

    /*
     * Check if we need to wait for all transfers or just this one ?
     */
    if (DAT_XFRID_WAITALL == waitId) {

        /* If for all, then use both the TransferComplete register for checking
         * if transfer has completed
         */
        while (1) {
            if (0x0 == (TransferCompleteH | TransferCompleteL) ) {
                return;
            }
        }
    }

    /*
     * Use the waitId to check for transfers having completed
     * Check for that bit in the completion register in a loop
     */
    if (waitId < 32) {

        while(1) {
            if (0x0 == GET_REGISTER32_BIT(TransferCompleteL, waitId)) {
                return;
            }
        }
    }
    else {

        waitId = waitId -32;
        while(1) {
            if (0x0 == GET_REGISTER32_BIT(TransferCompleteH,waitId)) {
                return;
            }
        }
    }
}

/*
 *  ======== DAT_busy =========
 * Check the busy status of transfer identified by waitId
 */
int DAT_busy(Uint32 waitId) {
    /*
     * Check if the particular transfer has completed by returning the status
     * from the itnernal completion register
     */
    if (waitId < 32) {
        return GET_REGISTER32_BIT(TransferCompleteL,waitId);
    }
    else {
        return GET_REGISTER32_BIT(TransferCompleteH, waitId-32);
    }
}

/*
 * static function definitions
 */


/*
 *  ======== _getFreeChannel =========
 * Used to obtain the next available channel to set up a new transfer
 * Function spins till a channel becomes available
 */
static inline Uint32 _getFreeChannel(Uint32 *tccNum) {
    Uint32 chNum,index ;

    /*
     * Start looking for available channels from the index after the one
     * that was allocated previously
     */
    index = (lastAllocatedIndex + 1)%(DAT_EDMA3LLD_numAllocatedChannels);

    /*
     * Spins till a free bit in TransferComplete is found
     */
    while (1) {
        *tccNum = DAT_allocatedChannels[index].tccNo;

        if (*tccNum < 32) {
            if((GET_REGISTER32_BIT(TransferCompleteL,*tccNum)) == 0) {
                chNum = DAT_allocatedChannels[index].paramNo;
                lastAllocatedIndex = index;
                return chNum;
            }
        }
        else {
            if((GET_REGISTER32_BIT(TransferCompleteH,*tccNum - 32)) == 0) {
                chNum = DAT_allocatedChannels[index].paramNo;
                lastAllocatedIndex = index;
                return chNum;
            }
        }

        /*
         * Increment index
         */
        index = (index + 1)%(DAT_EDMA3LLD_numAllocatedChannels);
    }
}

/*
 *  ======== _transferComplete =========
 * Callback function for transfer completion on a particular channel
 */
static void _transferComplete(Uint32 tccNum, EDMA3_RM_TccStatus status,
        void *param) {

    int i;
	(void)param;

    /*
     * Check for errors, print error message, clear error bits and return
     */
    if (EDMA3_RM_XFER_COMPLETE != status)
    {
        printf("Error during transfer, clearing Missed Event Register\n");
        for (i = 0; i < DAT_EDMA3LLD_numAllocatedChannels ;i++)
        {
            if (tccNum == DAT_allocatedChannels[i].tccNo)
            {
                    EDMA3_DRV_clearErrorBits(DAT_EDMA3LLD_hEdma, DAT_allocatedChannels[i].paramNo);
                    return;
            }
        }
    }

    /*
     * Mark zero in bit position tccNum
     */
    if (tccNum < 32) {
        CLEAR_REGISTER32_BIT(TransferCompleteL, tccNum );
    }
    else {
        CLEAR_REGISTER32_BIT(TransferCompleteH, tccNum -32 );
    }
}

/*
 *  ======== _setupTransferOptions =========
 * Function to set up the OPT for given chNum and tccNum
 */
inline Uint32 _setupTransferOptions(Uint32 chNum, Uint32 tccNum) {
    /*
     * Set up OPT for this transfer
     */
    EDMA3_DRV_setOptField(DAT_EDMA3LLD_hEdma, chNum, EDMA3_DRV_OPT_FIELD_SAM,
            0x0);
    EDMA3_DRV_setOptField(DAT_EDMA3LLD_hEdma, chNum, EDMA3_DRV_OPT_FIELD_DAM,
            0x0);
    EDMA3_DRV_setOptField(DAT_EDMA3LLD_hEdma, chNum, EDMA3_DRV_OPT_FIELD_STATIC,
            0x1);
    EDMA3_DRV_setOptField(DAT_EDMA3LLD_hEdma, chNum,
            EDMA3_DRV_OPT_FIELD_TCCMODE, 0x0);
    EDMA3_DRV_setOptField (DAT_EDMA3LLD_hEdma, chNum,
            EDMA3_DRV_OPT_FIELD_TCINTEN, 0x1);

    /*
     * Set a 1 bit in the TransferComplete register corresponding to the tcc
     */
    if (tccNum < 32) {
        SET_REGISTER32_BIT(TransferCompleteL,tccNum);
    }
    else {
        SET_REGISTER32_BIT(TransferCompleteH,tccNum - 32);
    }

    /*
     * Clear error bits before starting transfer
     */
    EDMA3_DRV_clearErrorBits(DAT_EDMA3LLD_hEdma, chNum);

    /*
     * Enable transfer
     */
    if (EDMA3_DRV_SOK != EDMA3_DRV_enableTransfer(DAT_EDMA3LLD_hEdma,chNum,
            EDMA3_DRV_TRIG_MODE_MANUAL)) {

        printf("Error enabling transfer \n");
        if (tccNum < 32) {
            CLEAR_REGISTER32_BIT(TransferCompleteL,tccNum);
        }
        else {
            CLEAR_REGISTER32_BIT(TransferCompleteH,tccNum - 32);
        }

        return DAT_INVALID_ID;
    }

    return tccNum;
}

