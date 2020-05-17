/*
 * llc_mmcsd.c
 *
 * This file contains MMCSD low level controller functionality implementation.
 * MMCSD Driver provides Driver Layer Interface to do operations on the MMCSD
 * peripheral like device initialization, channel creation, control commands for
 * peripheral specific operations etc
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

/** \file   llc_mmcsd.c
 *
 *  \brief  MMCSD controller layer implementation
 *
 *  This file implements the DDA functionality
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <assert.h>
#include <iom.h>

#include "llc_mmcsd.h"

/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
/**Transfer controller interrupt enable for EDMA*/
#define TCINTEN (1U << 20U)
/**Max C count value for EDMA*/
#define MAX_C_CNT 65535U

/* ========================================================================== */
/*                        MMCSD LLC GLOBAL FUNCTION DEFINTIONS                */
/* ========================================================================== */

/**\brief Initialise MMCSD controller                                         */
Void LLC_mmcsdInitController(LLC_MmcsdObj* const mmcsdObj)
{
    assert(NULL != mmcsdObj);

    /*CMD line portion is disabled and in reset state*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_CMDRST,
        CSL_MMCSD_MMCCTL_CMDRST_DISABLE);
    /*DAT line portion is disabled and in reset state*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_DATRST,
        CSL_MMCSD_MMCCTL_DATRST_DISABLE);
    /*set endian*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_PERMDX,
        CSL_MMCSD_MMCCTL_PERMDX_RESETVAL);
    /*set endian*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_PERMDR,
        CSL_MMCSD_MMCCTL_PERMDR_RESETVAL);
    /*DAT3 disabled*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_DATEG,
        CSL_MMCSD_MMCCTL_DATEG_RESETVAL);
    /*bus width*/
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_WIDTH0,
        CSL_MMCSD_MMCCTL_WIDTH0_RESETVAL);
    /*Set Response timeout*/
    mmcsdObj->regs->MMCTOR = LLC_MMCSD_RESPONSE_TIMEOUT;
    /*Set Data Read Timeout*/
    mmcsdObj->regs->MMCTOD = LLC_MMCSD_DATA_RW_TIMEOUT;
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_CMDRST,
        CSL_MMCSD_MMCCTL_CMDRST_ENABLE);
    CSL_FINS(mmcsdObj->regs->MMCCTL,
        MMCSD_MMCCTL_DATRST,
        CSL_MMCSD_MMCCTL_DATRST_ENABLE);
    /*Enable the clock*/
    CSL_FINS(mmcsdObj->regs->MMCCLK,
        MMCSD_MMCCLK_CLKEN,
        CSL_MMCSD_MMCCLK_CLKEN_ENABLE);

}

/**\brief Initilize the LLC                                                   */
Int32 LLC_mmcsdInit(LLC_MmcsdObj* mmcsdObj, Uint32 instId)
{
    assert(NULL != mmcsdObj);

    mmcsdObj->instId = instId;
    mmcsdObj->edmaChDetails.edmaOPTWrite = 0;
    mmcsdObj->edmaChDetails.edmaOPTRead = 0;

    /* Set the base address and interrupt number */
    switch (mmcsdObj->instId)
    {
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137)
        case MMCSD_INST_ID0:
            mmcsdObj->intrNum = CSL_INTC_EVENTID_MMCSDINT0;
            mmcsdObj->regs = (CSL_MmcsdRegsOvly)CSL_MMCSD_0_REGS;
            mmcsdObj->edmaChDetails.mmcTxCh = CSL_EDMA3_CHA_MMCSD_TX;
            mmcsdObj->edmaChDetails.mmcRxCh = CSL_EDMA3_CHA_MMCSD_RX;
            mmcsdObj->pscInstance = (Uint32)CSL_LPSC_INST_MMCSD_0;
    #ifndef BIOS_PWRM_ENABLE
            mmcsdObj->pwrmLpscID = (Uint32)CSL_PSC_MMCSD0;
    #else
            mmcsdObj->pwrmLpscID = (Uint32)CSL_LPSC_NUMBER_MMCSD_0;
    #endif
            break;
#endif
#if defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        case MMCSD_INST_ID0:
            mmcsdObj->intrNum = CSL_INTC_EVENTID_MMCSD0_INT0;
            mmcsdObj->regs = (CSL_MmcsdRegsOvly)CSL_MMCSD_0_REGS;
            mmcsdObj->edmaChDetails.mmcTxCh = CSL_EDMA3_CHA_MMCSD0_TX;
            mmcsdObj->edmaChDetails.mmcRxCh = CSL_EDMA3_CHA_MMCSD0_RX;
            mmcsdObj->pscInstance = (Uint32)CSL_LPSC_INST_MMCSD_0;
    #ifndef BIOS_PWRM_ENABLE
            mmcsdObj->pwrmLpscID = (Uint32)CSL_PSC_MMCSD0;
    #else
            mmcsdObj->pwrmLpscID = (Uint32)CSL_LPSC_NUMBER_MMCSD_0;
    #endif
            break;

        case MMCSD_INST_ID1:
            mmcsdObj->intrNum = CSL_INTC_EVENTID_MMCSD1_INT0;
            mmcsdObj->regs = (CSL_MmcsdRegsOvly)CSL_MMCSD_1_REGS;
            mmcsdObj->edmaChDetails.mmcTxCh = CSL_EDMA3_CHA_MMCSD1_TX;
            mmcsdObj->edmaChDetails.mmcRxCh = CSL_EDMA3_CHA_MMCSD1_RX;
            mmcsdObj->pscInstance = (Uint32)CSL_LPSC_INST_MMCSD_1;
    #ifndef BIOS_PWRM_ENABLE
            mmcsdObj->pwrmLpscID = (Uint32)CSL_PSC_MMCSD1;
    #else
            mmcsdObj->pwrmLpscID = (Uint32)CSL_LPSC_NUMBER_MMCSD_1;
    #endif
            break;
#endif
        default:
            /*Pass a Large number outside interrut number range */
            mmcsdObj->intrNum = 0xFFFFU;
            mmcsdObj->regs = (CSL_MmcsdRegsOvly)0;
    }
    LLC_mmcsdInitController(mmcsdObj);
    return IOM_COMPLETED;
}

/**\brief send a command to the card                                          */
Void LLC_mmcsdSendCmd(LLC_MmcsdHandle hLlc,
                      LLC_MmcsdCmd* const cmd,
                      Uint32 argument)
{

    LLC_MmcsdObj*   llc;
    Uint32 cmdType = 0;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == hLlc) || (NULL == cmd))
    {
        return;
    }
#endif
    llc = (LLC_MmcsdObj*)hLlc;

    cmdType |= CSL_FMKT(MMCSD_MMCCMD_DCLR, NO);

    /*Set command Busy or not*/
    cmdType |= CSL_FMK(MMCSD_MMCCMD_BSYEXP, cmd->busy);

    /*Set command index*/
    cmdType |= CSL_FMK(MMCSD_MMCCMD_CMD, cmd->idx);

    /*Setting initialize clock*/
    if (LLC_MMCSD_GO_IDLE_STATE == cmd->idx)
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_INITCK,INIT);
    }
    /*Set for generating DMA Xfer event*/
    if ((PSP_MMCSD_OPMODE_DMAINTERRUPT == cmd->opMode) &&
        ((LLC_MMCSD_WRITE_BLOCK == cmd->idx) ||
         (LLC_MMCSD_WRITE_MULTIPLE_BLOCK == cmd->idx) ||
         (LLC_MMCSD_PROGRAM_CSD == cmd->idx) ||
         (LLC_MMCSD_ACMD_SEND_SCR == cmd->idx)))
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_DMATRIG, YES);
    }
    else
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_DMATRIG, NO);
    }

    /*Setting whether command involves data transfer or not*/
    if (LLC_MMCSD_CMDTYPE_ADTC == cmd->type)
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_WDATX, DATA);
    }
    else
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_WDATX, NO);
    }

    /*Setting whether stream or block transfer*/
    if ((LLC_MMCSD_CMDTYPE_ADTC == cmd->type) &&
        ((LLC_MMCSD_MMC_WRITE_DAT_UNTIL_STOP == cmd->idx) ||
         (LLC_MMCSD_MMC_READ_DAT_UNTIL_STOP == cmd->idx)))
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_STRMTP, STREAM);
    }
    else
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_STRMTP, NO);
    }

     /*Setting whether data read or write */
    if (LLC_MMCSD_DIR_READ == cmd->dir)
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_DTRW, READ);
    }
    else if (LLC_MMCSD_DIR_WRITE == cmd->dir)
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_DTRW, WRITE);
    }
    else
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_DTRW, NO);
    }

    /*Setting response type */
    switch (cmd->respType)
    {
        case LLC_MMCSD_RESPTYPE_NO:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, NORSP);
            break;

        case LLC_MMCSD_RESPTYPE_R1:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R1);
            break;

        case LLC_MMCSD_RESPTYPE_R2:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R2);
            break;

        case LLC_MMCSD_RESPTYPE_R3:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R3);
            break;

        case LLC_MMCSD_RESPTYPE_R4:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R4);
            break;

        case LLC_MMCSD_RESPTYPE_R5:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R5);
            break;

        case  LLC_MMCSD_RESPTYPE_R6:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, R6);
            break;
        /*Klocwork throws "Code is unreachable" at this line which is not valid
        in this case*/
        default:
            cmdType |= CSL_FMKT(MMCSD_MMCCMD_RSPFMT, NORSP);
    }

     /*Setting push pull or open drain */
    if (LLC_MMCSD_LINECHR_PUSHPULL == cmd->lineChr)
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_PPLEN, PP);
    }
    else
    {
        cmdType |= CSL_FMKT(MMCSD_MMCCMD_PPLEN, OD);
    }

#ifdef CMD_DEBUG
    printf("\r\n Cmd No = %l  Cmd Reg = 0x%x  Cmd Arg Reg = "
           "0x%x", cmd->idx, cmdType, argument);
#endif

    llc->regs->MMCARGHL = argument;
    llc->regs->MMCCMD = cmdType;
}


/**\brief Configure FIFO for MMCSD                                            */
Int32 LLC_mmcsdConfigureFIFO(LLC_MmcsdHandle    hLlc,
                             LLC_MmcsdDir       cmd,
                             Uint16             fifoSize)
{
    LLC_MmcsdObj*   llc;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;

    switch (cmd)
    {
        case LLC_MMCSD_DIR_WRITE:
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFORST,
                CSL_MMCSD_MMCFIFOCTL_FIFORST_RESET);
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFORST,
                CSL_MMCSD_MMCFIFOCTL_FIFORST_RESETVAL);
            /*write to FIFO*/
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFODIR,
                CSL_MMCSD_MMCFIFOCTL_FIFODIR_WRITE);
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_ACCWD,
                CSL_MMCSD_MMCFIFOCTL_ACCWD_4BYTES);
            break;

        case LLC_MMCSD_DIR_READ:
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFORST,
                CSL_MMCSD_MMCFIFOCTL_FIFORST_RESET);
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFORST,
                CSL_MMCSD_MMCFIFOCTL_FIFORST_RESETVAL);
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFODIR,
                CSL_MMCSD_MMCFIFOCTL_FIFODIR_READ);
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_ACCWD,
                CSL_MMCSD_MMCFIFOCTL_ACCWD_4BYTES);
            break;

        default:
            /*print error*/
            break;
    }

    switch (fifoSize)
    {
        case MMCSD_FIFO_SIZE_32_BYTES:
            /*FIFO 256bit*/
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFOLEV,
                CSL_MMCSD_MMCFIFOCTL_FIFOLEV_256BIT);
            break;

        case MMCSD_FIFO_SIZE_64_BYTES:
            CSL_FINS(llc->regs->MMCFIFOCTL,
                MMCSD_MMCFIFOCTL_FIFOLEV,
                CSL_MMCSD_MMCFIFOCTL_FIFOLEV_512BIT);
            break;

        default:
            /*print error*/
            break;
    }
    return IOM_COMPLETED;
}

/**\brief Configure interrupts for MMCSD                                      */
Int32 LLC_mmcsdConfigureInterrupt(LLC_MmcsdHandle hLlc, LLC_MmcsdDir cmd)
{
    LLC_MmcsdObj*   llc;
    Int32 result = IOM_COMPLETED;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;
    switch (cmd)
    {
        case LLC_MMCSD_DIR_WRITE:
            CSL_FINS(llc->regs->MMCIM,
                MMCSD_MMCIM_EDRRDY,
                CSL_MMCSD_MMCIM_EDRRDY_PROHIBIT);
            CSL_FINS(llc->regs->MMCIM,
                MMCSD_MMCIM_EDXRDY,
                CSL_MMCSD_MMCIM_EDXRDY_PERMIT);
            break;

        case LLC_MMCSD_DIR_READ:
            CSL_FINS(llc->regs->MMCIM,
                MMCSD_MMCIM_EDXRDY,
                CSL_MMCSD_MMCIM_EDXRDY_PROHIBIT);
            CSL_FINS(llc->regs->MMCIM,
                MMCSD_MMCIM_EDRRDY,
                CSL_MMCSD_MMCIM_EDRRDY_PERMIT);
            break;

        default:
            /*print error*/
            break;
    }

    return result;
}


/**\brief get response from the card                                          */
Int32 LLC_mmcsdGetResp(LLC_MmcsdHandle     hLlc,
                       LLC_MmcsdRespType   respType,
                       Uint16*   const     response)
{
    LLC_MmcsdObj* llc;
    Int32 result = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == hLlc) || (NULL == response))
    {
        result = IOM_EBADARGS;
    }
#endif

    llc = (LLC_MmcsdObj*)hLlc;
    if(IOM_COMPLETED == result)
    {
        if (LLC_MMCSD_RESPTYPE_NO != respType)
        {
            response[6] = (Uint16)(llc->regs->MMCRSP67 &
                                   CSL_MMCSD_MMCRSP67_MMCRSP6_MASK);
            response[7] = (Uint16)((llc->regs->MMCRSP67 >>
                                    CSL_MMCSD_MMCRSP67_MMCRSP7_SHIFT) &
                                    CSL_MMCSD_MMCRSP67_MMCRSP6_MASK);
            if (LLC_MMCSD_RESPTYPE_R2 == respType)
            {
                response[0] = (Uint16)(llc->regs->MMCRSP01 &
                                       CSL_MMCSD_MMCRSP01_MMCRSP0_MASK);
                response[1] = (Uint16)((llc->regs->MMCRSP01 >>
                                        CSL_MMCSD_MMCRSP01_MMCRSP1_SHIFT) &
                                        CSL_MMCSD_MMCRSP01_MMCRSP0_MASK);
                response[2] = (Uint16)(llc->regs->MMCRSP23 &
                                       CSL_MMCSD_MMCRSP23_MMCRSP2_MASK);
                response[3] = (Uint16)((llc->regs->MMCRSP23 >>
                                        CSL_MMCSD_MMCRSP23_MMCRSP3_SHIFT) &
                                        CSL_MMCSD_MMCRSP23_MMCRSP2_MASK);
                response[4] = (Uint16)(llc->regs->MMCRSP45 &
                                       CSL_MMCSD_MMCRSP45_MMCRSP4_MASK);
                response[5] = (Uint16)((llc->regs->MMCRSP45 >>
                                        CSL_MMCSD_MMCRSP45_MMCRSP5_SHIFT) &
                                        CSL_MMCSD_MMCRSP45_MMCRSP4_MASK);
            }
        }
    }
    return result;
}

/**\brief read from controller                                                */
Int32 LLC_mmcsdRead(LLC_MmcsdHandle hLlc, Uint16 numBytes, Ptr buf)
{
    LLC_MmcsdObj*   llc;
    Int32 i;
    Uint32* readBuf;
    Int32 result = IOM_COMPLETED;

    assert((NULL != hLlc) && (NULL != buf));

    llc = (LLC_MmcsdObj*)hLlc;
    readBuf = (Uint32 *) buf;
    for (i = 0; i < (numBytes / FIFO_WIDTH_IN_BYTES); i++)
    {
        *readBuf=llc->regs->MMCDRR;
        readBuf++;
    }

    return result;
}

/**\brief write to controller                                                 */
Int32 LLC_mmcsdWrite(LLC_MmcsdHandle hLlc, Uint16 numBytes, Ptr buf)
{
    LLC_MmcsdObj* llc;
    Int32 i;
    Uint32* writeBuf;
    Int32 result = IOM_COMPLETED;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == hLlc) || (NULL == buf))
    {
        result = IOM_EBADARGS;
    }
#endif
    llc = (LLC_MmcsdObj*)hLlc;
    writeBuf = (Uint32*) buf;
    for (i = 0; i < (numBytes / FIFO_WIDTH_IN_BYTES); i++)
    {
        llc->regs->MMCDXR = *writeBuf;
        writeBuf++;
    }
    return result;
}

/**\brief set the new MMCSD interrupt mask & get the old one back. oldIntMode
 can be NULL when not required                                                */
Void LLC_mmcsdSetInterruptMode(LLC_MmcsdHandle hLlc,
                               Uint16          newIntMode,
                               Uint16*         oldIntMode)
{
    LLC_MmcsdObj* llc;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;
    if (oldIntMode != NULL)
    {
        *oldIntMode = (Uint16)llc->regs->MMCIM;
    }
    llc->regs->MMCIM = newIntMode;
}

/**\brief fetch & clear controller event status                               */
Int32 LLC_mmcsdGetEventStatus(LLC_MmcsdHandle hLlc, Uint16* status)
{
    LLC_MmcsdObj*   llc;
    Int32 result = IOM_COMPLETED;

    assert((NULL != hLlc) && (NULL != status));

    llc = (LLC_MmcsdObj*)hLlc;
    *status = (Uint16)llc->regs->MMCST0;

    return result;
}

/**\brief setup the DMA                                                       */
Int32 LLC_mmcsdDmaSetupXfer(LLC_MmcsdHandle             hLlc,
                            LLC_MmcsdDmaSetup* const    dmaSetup,
                            Ptr                         hEdma,
                            Uint16                      fifoSizeInBytes,
                            PSP_MmcsdEdmaEventQueue     eventQ)
{
    LLC_MmcsdObj*       llc;
    Uint32 channelNumber;
    Uint8 i, j;
    Uint16 aCnt, bCnt, cCnt;
    Uint32 srcPort, dstPort, tempcCnt;
    EDMA3_DRV_AddrMode modeSrc, modeDst;
    EDMA3_DRV_FifoWidth fifoWidthSrc, fifoWidthDst;
    Uint16 srcBidx, dstBidx;
    Uint16 srcCidx, dstCidx;
    Uint16 bCntRld;/*It's not cared as used for A sync mode*/
    EDMA3_DRV_SyncType syncMode;
    EDMA3_DRV_PaRAMRegs temp;
    Uint32 edmaChanNum;
    Uint32 numWordsInFifo;
    Uint32 tcc = 0;
    Int32 result;

    assert(((NULL != hLlc) && (NULL != hEdma)) && (NULL != dmaSetup));

    llc = (LLC_MmcsdObj*)hLlc;

    numWordsInFifo = (dmaSetup->numWords) /
                     (fifoSizeInBytes / FIFO_WIDTH_IN_BYTES);

    if (LLC_MMCSD_DIR_WRITE == dmaSetup->dir)
    {
        /*AB Sync Transfer*/
        /* Acnt =32, Bcnt= , Cnt=1*/
        channelNumber = llc->edmaChDetails.mmcTxCh;/*Write*/
        aCnt = FIFO_WIDTH_IN_BYTES;
        bCnt = fifoSizeInBytes / aCnt;
        if (numWordsInFifo > MAX_C_CNT)
        {
            tempcCnt = MAX_C_CNT;
            cCnt = (Uint16)tempcCnt;
        }
        else
        {
            cCnt = (Uint16)numWordsInFifo;
            tempcCnt = cCnt;
        }

        srcPort = (Uint32)(dmaSetup->address);
        modeSrc = EDMA3_DRV_ADDR_MODE_INCR;
        fifoWidthSrc = EDMA3_DRV_W8BIT; /* It's not cared as modeDsr is INCR*/
        srcBidx = aCnt;
        srcCidx = aCnt * bCnt;

        dstPort = (Uint32)&(llc->regs->MMCDXR);
        modeDst = EDMA3_DRV_ADDR_MODE_INCR;
        fifoWidthDst = EDMA3_DRV_W8BIT;/* It's not cared as modeDsr is INCR*/
        dstBidx = 0;
        dstCidx = 0;
        bCntRld = bCnt;/*It's not cared as used for A sync mode*/
        syncMode = EDMA3_DRV_SYNC_AB;
    }
    else
    {
        channelNumber = llc->edmaChDetails.mmcRxCh; /*Read*/
        aCnt = FIFO_WIDTH_IN_BYTES;
        bCnt = fifoSizeInBytes / aCnt;
        if (numWordsInFifo > MAX_C_CNT)
        {
            tempcCnt = MAX_C_CNT;
            cCnt = (Uint16)tempcCnt;
        }
        else
        {
            cCnt = (Uint16)numWordsInFifo;
            tempcCnt = cCnt;
        }

        srcPort = (Uint32)&(llc->regs->MMCDRR);
        modeSrc = EDMA3_DRV_ADDR_MODE_INCR;
        fifoWidthSrc = EDMA3_DRV_W8BIT;
        srcBidx = 0;
        srcCidx = 0;

        dstPort = (Uint32)(dmaSetup->address);
        modeDst = EDMA3_DRV_ADDR_MODE_INCR;
        fifoWidthDst = EDMA3_DRV_W8BIT; /* It's not cared as modeDsr is INCR*/
        dstBidx = aCnt;
        dstCidx = aCnt * bCnt;
        bCntRld = bCnt;/*It's not cared as used for A sync mode*/
        syncMode = EDMA3_DRV_SYNC_AB;
    }

    EDMA3_DRV_setSrcParams(hEdma,
        channelNumber,
        srcPort,
        modeSrc,
        fifoWidthSrc);

    EDMA3_DRV_setDestParams(hEdma,
        channelNumber,
        dstPort,
        modeDst,
        fifoWidthDst);

    EDMA3_DRV_setSrcIndex(hEdma, channelNumber, srcBidx, srcCidx);
    EDMA3_DRV_setDestIndex(hEdma, channelNumber, dstBidx, dstCidx);

    EDMA3_DRV_setTransferParams(hEdma,
        channelNumber,
        aCnt,
        bCnt,
        cCnt,
        bCntRld,
        syncMode);

    llc->edmaChDetails.cntChannel = 0;

    EDMA3_DRV_getPaRAM(hEdma, channelNumber, &temp);

    if (LLC_MMCSD_DIR_WRITE == dmaSetup->dir)
    {
        if (0 == llc->edmaChDetails.edmaOPTWrite)
        {
            llc->edmaChDetails.edmaOPTWrite = temp.opt;
        }
        else
        {
            temp.opt = llc->edmaChDetails.edmaOPTWrite;
            EDMA3_DRV_setPaRAM(hEdma, channelNumber, &temp);
        }
    }

    if (LLC_MMCSD_DIR_READ == dmaSetup->dir)
    {
        if (0 == llc->edmaChDetails.edmaOPTRead)
        {
            llc->edmaChDetails.edmaOPTRead = temp.opt;
        }
        else
        {
            temp.opt = llc->edmaChDetails.edmaOPTRead;
            EDMA3_DRV_setPaRAM(hEdma, channelNumber, &temp);
        }
    }

    EDMA3_DRV_getPaRAM(hEdma, channelNumber, &temp);
    temp.opt |= TCINTEN;
    EDMA3_DRV_setPaRAM(hEdma, channelNumber, &temp);

    if (numWordsInFifo > MAX_C_CNT) /* Linking will be performed*/
    {
        EDMA3_DRV_setOptField(hEdma,
            channelNumber,
            EDMA3_DRV_OPT_FIELD_TCINTEN,
            0);

        for (i = 0; i < EDMA_MAX_LOGICAL_CHA_ALLOWED; i++)
        {
            if (i != 0)
            {
                j = i - 1U;
                EDMA3_DRV_setOptField(hEdma,
                    llc->edmaChDetails.channelNum[j],
                    EDMA3_DRV_OPT_FIELD_TCINTEN,
                    0);
            }

            llc->edmaChDetails.cntChannel++;
            /* Setup for Channel 2 */
            tcc = channelNumber;
            edmaChanNum = EDMA3_DRV_LINK_CHANNEL;
            result = EDMA3_DRV_requestChannel(hEdma,
                         &edmaChanNum,
                         &tcc,
                         (EDMA3_RM_EventQueue)eventQ,
                         NULL,
                         NULL);

            if (IOM_COMPLETED == result)
            {
                MMCSD_DRV_DEBUG(("\r\n MMCSD: Link channel allocated with "
                                 "ChanNum = %d, tcc = %d", edmaChanNum, tcc));
            }
            else
            {
                MMCSD_ERR_DEBUG(("\r\n MMCSD: LINK channel allocation failed"));
            }

            llc->edmaChDetails.channelNum[i] = edmaChanNum;
            cCnt = temp.cCnt & 0x0000FFFFU;

            if (LLC_MMCSD_DIR_WRITE == dmaSetup->dir)
            {
                temp.srcAddr = temp.srcAddr + (aCnt * bCnt * cCnt);
            }
            else
            {
                temp.destAddr = temp.destAddr + (aCnt * bCnt * cCnt);
            }

            temp.opt |= TCINTEN;

            if ((numWordsInFifo - tempcCnt) > MAX_C_CNT)
            {
                temp.cCnt = (temp.cCnt & 0xFFFF0000U) | MAX_C_CNT;
                cCnt = temp.cCnt & 0x0000FFFFU;
                tempcCnt = tempcCnt + cCnt;
            }
            else
            {
                temp.cCnt = (temp.cCnt & 0xFFFF0000U) |
                            (Uint16)(numWordsInFifo - tempcCnt);
                cCnt = temp.cCnt & 0x0000FFFF;
                tempcCnt = tempcCnt + cCnt;
            }
            EDMA3_DRV_setPaRAM(hEdma, edmaChanNum, &temp);

            if (i != 0)
            {
                j = i - 1U;
                EDMA3_DRV_linkChannel(hEdma,
                    llc->edmaChDetails.channelNum[j],
                    edmaChanNum);
            }
            if (tempcCnt == numWordsInFifo)
            {
                break;
            }
        }
        EDMA3_DRV_linkChannel(hEdma,
            channelNumber,
            llc->edmaChDetails.channelNum[0]);
    }
    return IOM_COMPLETED;
}

/**\brief DMA start                                                           */
Int32 LLC_mmcsdDmaStart(Uint32 channelNo, Ptr hEdma)
{
    Int32 result = IOM_COMPLETED;

    assert(NULL != hEdma);

    result = EDMA3_DRV_enableTransfer(hEdma,
                 channelNo,
                 EDMA3_DRV_TRIG_MODE_EVENT);

    return result;
}

/**\brief DMA stop                                                            */
Int32 LLC_mmcsdDmaStop(Uint32 channelNo, Ptr hEdma)
{
    Int32 result = IOM_COMPLETED;

    assert(NULL != hEdma);

    result = EDMA3_DRV_disableTransfer(hEdma,
                 channelNo,
                 EDMA3_DRV_TRIG_MODE_EVENT);

    return result;
}

/**\brief Free the DMA channel                                                */
Void LLC_mmcsdDmaCleanChannel(Uint32 channelNo, Ptr hEdma)
{
    assert(NULL != hEdma);

    EDMA3_DRV_clearErrorBits(hEdma, channelNo);
}

/**\brief Free the DMA channel                                                */
Int32 LLC_mmcsdDmaFreeChannel(LLC_MmcsdHandle hLlc, Ptr hEdma)
{
    LLC_MmcsdObj*       llc;
    Uint8 i;
    Int32 result = IOM_COMPLETED;

    assert((NULL != hLlc) && (NULL != hEdma));

    llc = (LLC_MmcsdObj*)hLlc;

    if (llc->edmaChDetails.cntChannel != 0)
    {
        for (i = 0; i < llc->edmaChDetails.cntChannel; i++)
        {
            EDMA3_DRV_freeChannel(hEdma, llc->edmaChDetails.channelNum[i]);
        }
    }

    return result;
}

/**\brief Set frequency for mmcasd controller*/
Void LLC_mmcsdSetFrequency(LLC_MmcsdHandle hLlc, Ptr freq, Bool isDivBy4Reqd)
{
    LLC_MmcsdObj*       llc;

    assert((NULL != hLlc) && (NULL != freq));
    llc = (LLC_MmcsdObj*)hLlc;
    /*Disable the clock*/
    CSL_FINS(llc->regs->MMCCLK,
        MMCSD_MMCCLK_CLKEN,
        CSL_MMCSD_MMCCLK_CLKEN_DISABLE);
    if (TRUE == isDivBy4Reqd)
    {
        /*Enable the clock to be divided by 4*/
        CSL_FINS(llc->regs->MMCCLK,
            MMCSD_MMCCLK_DIV4,
            CSL_MMCSD_MMCCLK_DIV4_ENABLE);
    }
    else
    {
        /*Enable the clock to be divided by 2*/
        CSL_FINS(llc->regs->MMCCLK,
            MMCSD_MMCCLK_DIV4,
            CSL_MMCSD_MMCCLK_DIV4_DISABLE);
    }

    CSL_FINS(llc->regs->MMCCLK, MMCSD_MMCCLK_CLKRT, *((Uint32*)freq));
    /*Enable the clock*/
    CSL_FINS(llc->regs->MMCCLK,
        MMCSD_MMCCLK_CLKEN,
        CSL_MMCSD_MMCCLK_CLKEN_ENABLE);
}


/**\brief Set number of blocks for MMCSD controller*/
Void LLC_mmcsdSetNumBlocks(LLC_MmcsdHandle hLlc, Uint32 numBlocks)
{
    LLC_MmcsdObj*       llc;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;
    CSL_FINS(llc->regs->MMCNBLK, MMCSD_MMCNBLK_NBLK, numBlocks);
}

/**\brief Set MMCSD block length*/
Void LLC_mmcsdSetBlockLen(LLC_MmcsdHandle hLlc, Uint32 blockLen)
{
    LLC_MmcsdObj*       llc;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;
    CSL_FINS(llc->regs->MMCBLEN, MMCSD_MMCBLEN_BLEN, blockLen);
}

/**\brief set bus width for MMCSD*/
Void LLC_mmcsdSetBusWidth(LLC_MmcsdHandle hLlc, Bool busWidthIs4Bit)
{
    LLC_MmcsdObj*       llc;
    Uint32 Buswidth = (Uint32)busWidthIs4Bit;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;
    CSL_FINS(llc->regs->MMCCTL, MMCSD_MMCCTL_WIDTH0, Buswidth);
}

/**\brief send init sequence for MMCSD*/
Void LLC_mmcsdSendInitSeq(LLC_MmcsdHandle hLlc)
{
    LLC_MmcsdObj*       llc;

    assert(NULL != hLlc);

    llc = (LLC_MmcsdObj*)hLlc;

    llc->regs->MMCCMD = CSL_FMKT(MMCSD_MMCCMD_DMATRIG, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_DCLR, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_STRMTP, RESETVAL)|
                            CSL_FMKT(MMCSD_MMCCMD_WDATX, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_DTRW, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_BSYEXP, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_RSPFMT, RESETVAL) |
                            CSL_FMKT(MMCSD_MMCCMD_INITCK, INIT) |
                            CSL_FMKT(MMCSD_MMCCMD_PPLEN, RESETVAL) |
                            CSL_FMK(MMCSD_MMCCMD_CMD, 0);
}
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
