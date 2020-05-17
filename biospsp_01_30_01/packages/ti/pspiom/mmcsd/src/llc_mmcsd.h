/*
 * llc_mmcsd.h
 *
 * This file contains MMCSD low level controller functionality macros and inter-
 * face definitions.
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

/**
 * \file llc_mmcsd.h
 *
 * \brief Controller specific header file
 *
 * This file controller specific function
 *
 * (C) Copyright 2008, Texas Instruments, Inc
 *
 */

#ifndef _LLC_MMCSD_H_
#define _LLC_MMCSD_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <ti/sdo/edma3/drv/edma3_drv.h>

#include <ti/pspiom/cslr/cslr_mmcsd.h>

#include <ti/pspiom/mmcsd/psp_mmcsd.h>
#include "dda_mmcsdCfg.h"

#ifdef __cplusplus
extern "C"
{
#endif
/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
/* Debugging macros. It may be required to include stdio.h, if prints are not
 coming properly*/
#define MMCSD_DRV_DEBUG(x)
/**< To debug MMCSD make this define equal to printf x                        */
#define MMCSD_ERR_DEBUG(x)
/**< To debug ERR on MMCSD make this define equal to printf x                 */
#define MMCSD_INIT_DEBUG(x)
/**< To debug the INIT seq on MMCSD make this define equal to printf x        */

/*#define MMCSD_DEBUG*/
/*#define SCR_HIGH_SPEED_DEBUG*/
/*#define CMD_DEBUG*/
/**< Enable these macros for getting debug information.                       */

#define EDMA_LINKING_ALLOWED            FALSE
/**< It tells whether linking will be performed or not for EDMA               */
#define EDMA_MAX_LOGICAL_CHA_ALLOWED    2U
/**< No of channel logical channels allowed for transfer including main channel
for mmcsd                                                                     */
#define LLC_MMCSD_RESPONSE_TIMEOUT      0x3FFFFU
/**< Response timeout                                                         */
#define LLC_MMCSD_DATA_RW_TIMEOUT       0xFFFFU
/**< Data Read Write timeout                                                  */

#define LLC_MMCSD_EVENT_ERROR           (LLC_MMCSD_EVENT_ERROR_CMDCRC |     \
                                         LLC_MMCSD_EVENT_ERROR_DATACRC |    \
                                         LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT | \
                                         LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)
/**<  MMCSD Event Error                                                       */

#define LLC_MMCSD_RESPONSE_R1_ERROR_MASK    0xFEFF0000u
/**<  MMCSD response 1 error mask                                             */

#define LLC_MMCSD_RESPONSE_R6_ERROR_MASK    0x0000E000u
/**<  MMCSD response 6 error mask                                             */

/* ========================================================================== */
/*                      MMCSD LLC DATA TYPES                                  */
/* ========================================================================== */

/**\brief LLC object handle                                                   */
typedef Ptr LLC_MmcsdHandle;

/**\brief MMCSD line characteristics                                          */
typedef enum
{
    LLC_MMCSD_LINECHR_PUSHPULL  = 0,
    LLC_MMCSD_LINECHR_OPENDRAIN = 1
}LLC_MmcsdLineChr;

/**\brief MMCSD response type                                                 */
typedef enum
{
    LLC_MMCSD_RESPTYPE_NO   = 0,
    LLC_MMCSD_RESPTYPE_R1   = 1,
    LLC_MMCSD_RESPTYPE_R2   = 2,
    LLC_MMCSD_RESPTYPE_R3   = 3,
    LLC_MMCSD_RESPTYPE_R4   = 4,
    LLC_MMCSD_RESPTYPE_R5   = 5,
    LLC_MMCSD_RESPTYPE_R6   = 6
}LLC_MmcsdRespType;

/**\brief MMCSD response Busy                                                 */
typedef enum
{
    LLC_MMCSD_RESPBUSY_NO   = 0,
    LLC_MMCSD_RESPBUSY_YES  = 1
}LLC_MmcsdRespBusy;

/**\brief MMCSD command type                                                  */
typedef enum
{
    LLC_MMCSD_CMDTYPE_BC    = 0,
    LLC_MMCSD_CMDTYPE_BCR   = 1,
    LLC_MMCSD_CMDTYPE_AC    = 2,
    LLC_MMCSD_CMDTYPE_ADTC  = 3
}LLC_MmcsdCmdType;

/**\brief MMCSD command direction                                             */
typedef enum
{
    LLC_MMCSD_DIR_WRITE     = 0,
    LLC_MMCSD_DIR_READ      = 1,
    LLC_MMCSD_DIR_DONTCARE  = 2
}LLC_MmcsdDir;

/**\brief MMCSD event type*/
typedef enum
{

    LLC_MMCSD_EVENT_EOFCMD              = (1 << 2),
    /**< for commands with response, an end of
     * Command + Response; for commands without response,
     * an end of Command                                                      */
    LLC_MMCSD_EVENT_READ                = (1 << 10),
    /**< data available with controller for reading                           */

    LLC_MMCSD_EVENT_WRITE               = (1 << 9),
    /**< data required by controller for writing                              */

    LLC_MMCSD_EVENT_ERROR_CMDCRC        = (1 << 7),
    /**< Error detected in the CRC during commannd
     * - response phase                                                       */

    LLC_MMCSD_EVENT_ERROR_DATACRC       = ((1 << 6)|(1 << 5)),
    /**< Error detected in the CRC during data transfer                       */

    LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT    = (1 << 4),
    /**< Timeout detected during commannd - response phase                    */

    LLC_MMCSD_EVENT_ERROR_DATATIMEOUT   = (1 << 3),
    /**< Timeout detected during data transfer                                */

    LLC_MMCSD_EVENT_CARD_EXITBUSY       = (1 << 1),
    /**< Card has exited busy state                                           */

    LLC_MMCSD_EVENT_BLOCK_XFERRED       = 1
    /**< block transfer done                                                  */
}LLC_MmcsdEvent;

/**\brief MMC/SD command type*/
typedef enum
{
    LLC_MMCSD_GO_IDLE_STATE = 0,
    LLC_MMCSD_MMC_SEND_OP_COND = 1,
    LLC_MMCSD_ALL_SEND_CID = 2,
    LLC_MMCSD_SEND_RELATIVE_ADDR = 3,
    LLC_MMCSD_MMC_SET_RELATIVE_ADDR = 3,
    LLC_MMCSD_SET_DSR = 4,
    LLC_MMCSD_SELECT_DESELECT_CARD = 7,
    LLC_MMCSD_SEND_IF_COND = 8,
    LLC_MMCSD_SEND_CSD = 9,
    LLC_MMCSD_SEND_CID = 10,
    LLC_MMCSD_MMC_READ_DAT_UNTIL_STOP = 11,
    LLC_MMCSD_STOP_TRANSMISSION = 12,
    LLC_MMCSD_SEND_STATUS = 13,
    LLC_MMCSD_GO_INACTIVE_STATE = 15,
    LLC_MMCSD_SET_BLOCKLEN = 16,
    LLC_MMCSD_READ_SINGLE_BLOCK = 17,
    LLC_MMCSD_READ_MULTIPLE_BLOCK = 18,
    LLC_MMCSD_MMC_WRITE_DAT_UNTIL_STOP = 20,
    LLC_MMCSD_MMC_SET_BLOCK_COUNT = 23,
    LLC_MMCSD_WRITE_BLOCK = 24,
    LLC_MMCSD_WRITE_MULTIPLE_BLOCK = 25,
    LLC_MMCSD_MMC_PROGRAM_CID = 26,
    LLC_MMCSD_PROGRAM_CSD = 27,
    LLC_MMCSD_SET_WRITE_PROT = 28,
    LLC_MMCSD_CLR_WRITE_PROT = 29,
    LLC_MMCSD_SEND_WRITE_PROT = 30,
    LLC_MMCSD_ERASE_WR_BLK_START = 32,
    LLC_MMCSD_ERASE_WR_BLK_END = 33,
    LLC_MMCSD_MMC_ERASE_GROUP_START = 35,
    LLC_MMCSD_MMC_ERASE_GROUP_END = 36,
    LLC_MMCSD_ERASE= 38,
    LLC_MMCSD_MMC_FAST_IO = 39,
    LLC_MMCSD_MMC_GO_IRQ_STATE = 40,
    LLC_MMCSD_LOCK_UNLOCK = 42,
    LLC_MMCSD_APP_CMD = 55,
    LLC_MMCSD_GEN_CMD = 56,
    LLC_MMCSD_ACMD_SET_BUS_WIDTH = 6,
    LLC_MMCSD_ACMD_SD_STATUS = 13,
    LLC_MMCSD_ACMD_SEND_NUM_WR_BLOCKS = 22,
    LLC_MMCSD_ACMD_SET_WR_BLK_ERASE_COUNT = 23,
    LLC_MMCSD_ACMD_SD_SEND_OP_COND = 41,
    LLC_MMCSD_ACMD_SET_CLR_CARD_DETECT = 42,
    LLC_MMCSD_ACMD_SEND_SCR = 51,
    LLC_MMCSD_SWITCH_FUNC = 6
}LLC_MmcsdCmdName;

/**
 * \brief MMCSD LLC EDMA info structure
 *
 */
typedef struct
{
    Uint8                   cntChannel;
    /**< Number of logical channel allocated                                  */
    Uint32                  channelNum[EDMA_MAX_LOGICAL_CHA_ALLOWED];
    /**< Logical channel numbers                                              */
    Uint32                  mmcTxCh;
    /**< Tx channel numbers                                                   */
    Uint32                  mmcRxCh;
    /**< Rx channel numbers                                                   */
    Uint32                  edmaOPTRead;
    /**< EDMA OPT field value for Read                                        */
    Uint32                  edmaOPTWrite;
    /**< EDMA OPT field value for Write                                       */
}LLC_edmaChMmcsd;

/**
 * \brief MMCSD LLC Driver structure
 *
 */
typedef struct
{
     Uint32                     instId;
    /**< Instance number                                                      */
    CSL_MmcsdRegsOvly           regs;
    /**< Register overlay                                                     */
     Uint32                     intrNum;
    /**< Interrupt number                                                     */
    LLC_edmaChMmcsd             edmaChDetails;
    /**< Details of EDMA channel allocated                                    */
    Uint32                      pwrmLpscID;
    /**< LPSC id of MMCSD                                                     */
    Uint32                      pscInstance;
    /**< PSC instance of MMCSD                                                */
    Bool                        isPwrAware;
    /**< Is the driver power management aware or not                          */
}LLC_MmcsdObj;

/**
 * \brief LLC Driver Command
 *
 * The following object different command parameters needed during command
 * transfer.
 */
typedef struct
{
    LLC_MmcsdCmdName        idx;
    /**< name of the command                                                  */
    LLC_MmcsdRespType       respType;
    /**< response expected for the command                                    */
    LLC_MmcsdRespBusy       busy;
    /**< response expected for the command is of type busy                    */
    LLC_MmcsdCmdType        type;
    /**< protocol type of the command                                         */
    LLC_MmcsdDir            dir;
    /**< direction for the data if accompanying the command                   */
    LLC_MmcsdLineChr        lineChr;
    /**< line characteristics of the command                                  */
    PSP_MmcsdOpMode         opMode;
    /**< dma requirement for data transfer of the command                     */
}LLC_MmcsdCmd;


/** @brief Structure having all info needed to be given to controller for DMA.
 *  It includes its direction of xfer, address of buffer, number of words to
 *  xfer and frame length                                                     */
typedef struct
{
    LLC_MmcsdDir            dir;
    /**< direction : controller writes to or deads from card                  */
    Ptr                     address;
    /**< address of buffer to which transfer is needed                        */
    Uint32                  numWords;
    /**< number of words to transfer                                          */
    Uint16                  frameLength;
    /**< number of words to transfer per controller request                   */
}LLC_MmcsdDmaSetup;

/* Initilize the LLC Controller                                               */
Void LLC_mmcsdInitController(LLC_MmcsdObj* const mmcsdObj);

/* Initilize the LLC                                                          */
Int32 LLC_mmcsdInit(LLC_MmcsdObj* mmcsdObj, Uint32 instId);

/* send a command to the card                                                 */
Void LLC_mmcsdSendCmd(LLC_MmcsdHandle          hLlc,
                      LLC_MmcsdCmd*  const     cmd,
                      Uint32                   argument);

/* configures the FIFO                                                        */
Int32 LLC_mmcsdConfigureFIFO(LLC_MmcsdHandle    hLlc,
                             LLC_MmcsdDir       cmd,
                             Uint16             fifoSize);

/**< configures the Interrupt depending upon read/write operation */
Int32 LLC_mmcsdConfigureInterrupt(LLC_MmcsdHandle hLlc, LLC_MmcsdDir cmd);

/**< get response from the card                                               */
Int32 LLC_mmcsdGetResp(LLC_MmcsdHandle         hLlc,
                       LLC_MmcsdRespType       respType,
                       Uint16*    const        response);

/**< read from controller                                                     */
Int32 LLC_mmcsdRead(LLC_MmcsdHandle hLlc, Uint16 numWords, Ptr buf);

/**< write to controller                                                      */
Int32 LLC_mmcsdWrite(LLC_MmcsdHandle hLlc, Uint16 numWords, Ptr buf);

/**< set the MMCSD interrupt mode                                             */
Void LLC_mmcsdSetInterruptMode(LLC_MmcsdHandle         hLlc,
                               Uint16                  newIntMode,
                               Uint16*                 oldIntMode);

/**< fetch & clear controller event status                                    */
Int32 LLC_mmcsdGetEventStatus(LLC_MmcsdHandle hLlc, Uint16* status);

/**< setup the DMA                                                            */
Int32 LLC_mmcsdDmaSetupXfer(LLC_MmcsdHandle             hLlc,
                            LLC_MmcsdDmaSetup* const    dmaSetup,
                            Ptr                         hEdma,
                            Uint16                      fifoSizeInBytes,
                            PSP_MmcsdEdmaEventQueue     eventQ);

/**< DMA start                                                                */
Int32 LLC_mmcsdDmaStart(Uint32 channelNo, Ptr hEdma);

/**< DMA stop                                                                 */
Int32 LLC_mmcsdDmaStop(Uint32 channelNo, Ptr hEdma);

/**< Free the DMA channel                                                     */
Void LLC_mmcsdDmaCleanChannel(Uint32 channelNo, Ptr hEdma);

/**< Free the dma channel*/
Int32 LLC_mmcsdDmaFreeChannel(LLC_MmcsdHandle hLlc, Ptr hEdma);

/**< change the frequency as specified                                        */
Void LLC_mmcsdSetFrequency(LLC_MmcsdHandle hLlc, Ptr freq, Bool isDivBy4Reqd);

/**< in addition to sending the num block info to the card, the
 * controller also needs a separate notification of the count                 */
Void LLC_mmcsdSetNumBlocks(LLC_MmcsdHandle hLlc, Uint32 numBlocks);

/**< the controller needs a notification of the count - only
 * once unlike numBlocks                                                      */
Void LLC_mmcsdSetBlockLen(LLC_MmcsdHandle hLlc, Uint32 blockLen);

/* notification to the controller about the bus width                         */
Void LLC_mmcsdSetBusWidth(LLC_MmcsdHandle hLlc, Bool busWidthIs4Bit);

/* send the initialization sequence                                           */
Void LLC_mmcsdSendInitSeq(LLC_MmcsdHandle hLlc);

#ifdef __cplusplus
}
#endif

#endif /* _LLC_MMCSD_H_ */
/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
