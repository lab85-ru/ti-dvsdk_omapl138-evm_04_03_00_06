/*
 * ddc_mmcsd.c
 *
 * This file contains MMCSD core functionality implementation.
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


/** \file   ddc_mmcsd.c
 *
 *  \brief  DDC Initialization for  the MMMCSD
 *
 *  This file implements the DDC Initialzation functionality
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
#include <hwi.h>
#include <tsk.h>
#include <ecm.h>
#include <c64.h>
#include <bcache.h>
#include <string.h>

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#include <ti/pspiom/psc/Psc.h>

#include "ddc_mmcsd.h"

#ifdef MMCSD_INSTRUMENTATION_ENABLED
#include    <ti/sdo/pspdrivers/pal_os/bios/psp_log.h>
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
/* ========================================================================== */
/*                             MACRO DEFINITIONS                              */
/* ========================================================================== */
#define DDC_MMC_CARD_RCA                        7U
/**< RCA value for MMCSD                                                      */
#define RCA_SHIFT_MASK                          16U
/**< RCA shift value for MMCSD                                                */
#define SD_HIGH_CAPACITY_ECHO_PATTERN           0xAAU
/**< High Capacity echo pattern                                               */
#define MAX_SECTORS_PER_EDMA_CHANNEL            4000U
/**< Max number of sectors per EDMA channel                                   */
#define MMCSD_DEFAULT_BLOCK_LENGTH              512U
/**< Default block length for MMCSD                                           */
#define PERMANENT_WRITE_PROTECT_MASK            0x20U
/**< Permanent write protect mask value                                       */
#define TEMPERORY_WRITE_PROTECT_MASK            0x10U
/**< Temperory write protect mask value                                       */
#define MMCSD_ARGUMENT_STUFF_BITS               0x0U
/**< MMCSD argument bits                                                      */
#define SD_STATUS_READ                          TRUE
/**< SD staus read or not                                                     */
#define MAX_RESP_SIZE_BYTES                     8U
/**< maximum response size in bytes in controller                             */
#define ONE_BLOCK_REQUEST                       1U
/**< One block request                                                        */
#define DEFAULT_DSR_VALUE                       0x404U
/**< Default DSR value                                                        */

#ifdef MMCSD_DEBUG
DDC_MmcsdDeviceState state;
Uint32 dataTransfered = 0;
Uint32 BusyHit = 0;
Bool Entered = 0;
Uint32 transtcnt = 0;
Uint32 readRequest = 0;
Uint32 writeRequest = 0;
#endif

/**\brief MMCSD DDC objects*/
DDC_MmcsdObj gMmcsdDdcObj[PSP_MMCSD_NUM_INSTANCES];

/**\brief MMCSD hardware config*/
extern DDC_MmcsdDrvrHwCfg gMmcsdDrvrHwCfg;

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
/**\brief MMCSD instance 0 DMA init or not*/
static Uint8 isInst0DmaInitialize = 0;
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
/**\brief MMCSD instance 1 DMA init or not*/
static Uint8 isInst1DmaInitialize = 0;
#endif

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Void mmcsdConfigPolledMode(DDC_MmcsdDeviceHandle devDDC);

static Void mmcsdConfigDmaMode(DDC_MmcsdDeviceHandle devDDC);

static Uint8 mmcsdCalculateCRC7(Uint8 crc, Uint8 *pc, Uint32 len);

static Int32 mmcsdTranStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdGetStatus(DDC_MmcsdDeviceHandle devDDC,
                            Uint16* pStatus,
                            Uint16 errStatus,
                            Uint16 reqdStatus);

static Int32 mmcsdGetStopStatusInError(DDC_MmcsdDeviceHandle devDDC,
                                       Uint16* const pStatus,
                                       Uint16 errStatus,
                                       Uint16 reqdStatus);

static Int32 mmcsdSendStopCmd(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdWritePolledMode(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdReadPolledMode(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdReadStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdReadStopStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Bool mmcsdFinishIopProcessingAndIsIopPending(
                DDC_MmcsdDeviceHandle devDDC,
                Int32* result);

static Int32 mmcsdWriteStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdWriteStopStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdSendStatusStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdFinishCheckStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdFinishBusyStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status);

static Int32 mmcsdCheckResponse(LLC_MmcsdHandle         hLlc,
                                LLC_MmcsdRespType   respType,
                                Uint16* const       resp);

static Bool mmcsdGetStatusFromCard(DDC_MmcsdDeviceHandle devDDC,
                                   Uint16* resp,
                                   PSP_MmcsdCardType card);

static Void mmcsdCheckHighSpeedCard(DDC_MmcsdDeviceHandle devDDC, Uint16* resp);

static Int32 mmcsdReadSCR(DDC_MmcsdDeviceHandle devDDC, Uint16* resp);

static Int32 mmcsdReadSdStatus(DDC_MmcsdDeviceHandle devDDC, Uint16* resp);

static Void mmcsdRestoreInt(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode);

static Int32 mmcsdSendInterfaceInfo(DDC_MmcsdDeviceHandle devDDC,
                                    Uint16 mmcIntMode);

static Int32 mmcsdSendCID(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode);

static Int32 mmcsdSendCSD(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode);

static Int32 mmcsdDisconnectReg(DDC_MmcsdDeviceHandle devDDC,
                                Uint16 mmcIntMode);

static Uint32 mmcsdGetCapacity(DDC_MmcsdDeviceHandle devDDC,
                               PSP_MmcsdCardType card);

static Int32 mmcsdInitSdCard(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdInitMmcCard(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdCheckCard(DDC_MmcsdDeviceHandle devDDC);

static Int32 mmcsdSendCmdPolledMode(LLC_MmcsdHandle         hLlc,
                                    LLC_MmcsdCmd*   const   cmd,
                                    Uint32                  arg,
                                    Uint16*                 resp,
                                    Uint16                  reqdStatus,
                                    Uint16                  errStatus,
                                    Uint16*                 retStatus);

static Int32 mmcsdSubmitIoRequestSingle(DDC_MmcsdDeviceHandle devDDC,
                                        DDC_MmcsdIoPacket* const iop);

static Int32 mmcsdCalculateFreqForCard(DDC_MmcsdHandle DDC,
                                       Uint32 initReqFreq,
                                       Uint32 mmcReqFreq,
                                       Uint32 sdReqFreq,
                                       Uint32 sdhighReqFreq);

static Void mmcsdHisrHandler(DDC_MmcsdObj *hDdc);

static Void mmcsdIsrHandler(DDC_MmcsdObj *hDdc);

static Void mmcsdCheckFinishIOP(DDC_MmcsdObj *hDdc);

static Void mmcsdDmaIsrHandler(Uint32 curr_lch,
                               EDMA3_RM_TccStatus ch_status,
                               Ptr data);

static Void mmcsdFxnIoComplete(DDC_MmcsdHandle hDDC,
                               DDC_MmcsdIoPacket * const iop);

static Int32 mmcsdControlCb(DDC_MmcsdDeviceHandle devDDC,
                            Int32 cmd,
                            Ptr cmdArg);

static Void mmcsdSetPolledMode(DDC_MmcsdObj* const DDC);

static Int32 mmcsdSetDmaMode(DDC_MmcsdObj* const DDC);

static Int32 mmcsdUnSetDmaMode(DDC_MmcsdObj* const DDC);

static Int32 mmcsdFreeIop(DDC_MmcsdHandle hDDC, DDC_MmcsdIoPacket * const iop);

static Int32 mmcsdIo(DDC_MmcsdChannelHandle hChannel,
                     Ptr            MedHandle,
                     Uint8          cmd,
                     Uint32         address,
                     Uint16* const  bufPtr,
                     Uint32         bufSz,
                     Uint32         timeout);

static Int32 mmcsdSyncDelay(DDC_MmcsdChannelObj* pChannel,
                            DDC_MmcsdIoPacket* iop,
                            Uint32          timeout);

static Int32 mmcsdAsyncDelay(DDC_MmcsdChannelObj* pChannel,
                             DDC_MmcsdIoPacket* iop,
                             Uint32          timeout);

static Int32 mmcsdSyncCallback(DDC_MmcsdChannelObj* pChannel,
                               Int32           count,
                               Ptr         mediaHandle);

static Int32 mmcsdAsyncCallback(DDC_MmcsdChannelObj* const pChannel,
                                Int32           count,
                                Ptr         mediaHandle);

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
static Void mmcsdIsrHandler0(Void);
static Void mmcsdTask0(Void);
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
static Void mmcsdIsrHandler1(Void);
static Void mmcsdTask1(Void);
#endif

static Void mmcsdRegisterIntrHandler(Uint32 deviceId,
                                     Uint32 intrNum,
                                     Uint16 hwiNo);

static Int32 mmcsdLpscOff(DDC_MmcsdObj* const DDC);

static Int32 mmcsdLpscOn(DDC_MmcsdObj* const DDC);

#ifdef BIOS_PWRM_ENABLE
static Int32 mmcsdRegisterNotification(DDC_MmcsdObj* const DDC);

static PWRM_NotifyResponse mmcsdNotifyCallback(PWRM_Event    eventType,
                                               Arg           eventArg1,
                                               Arg           eventArg2,
                                               Arg           clientArg);

static PWRM_NotifyResponse mmcsdConfigureNewVfSettings(DDC_MmcsdObj* const DDC,
                                                       Uns           eventArg1,
                                                       Uns           eventArg2);

static Int32 mmcsdUnregisterNotification(DDC_MmcsdObj* const DDC);
#endif

/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */

/**\brief configure polled mode*/
static Void mmcsdConfigPolledMode(DDC_MmcsdDeviceHandle devDDC)
{
    assert(NULL != devDDC);

    devDDC->opMode = PSP_MMCSD_OPMODE_POLLED;

    LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, NULL);
}

/**\brief configure DMA mode*/
static Void mmcsdConfigDmaMode(DDC_MmcsdDeviceHandle devDDC)
{
    assert(NULL != devDDC);

    devDDC->opMode = PSP_MMCSD_OPMODE_DMAINTERRUPT;
    LLC_mmcsdSetInterruptMode(devDDC->hLlc,
                                      (LLC_MMCSD_EVENT_EOFCMD |
                                       LLC_MMCSD_EVENT_ERROR_CMDCRC |
                                       LLC_MMCSD_EVENT_ERROR_DATACRC |
                                       LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                                       LLC_MMCSD_EVENT_ERROR_DATATIMEOUT |
                                       LLC_MMCSD_EVENT_CARD_EXITBUSY |
                                       LLC_MMCSD_EVENT_BLOCK_XFERRED),
                                      NULL);

}

/**\brief Calculate 7 bit CRC*/
static Uint8 mmcsdCalculateCRC7(Uint8 crc, Uint8 *pc, Uint32 len)
{
    Uint32 i;
    Uint8  ibit;
    Uint8  c;

    assert(NULL != pc);

    for (i = 0U; i < len; i++)
    {
        c = *pc;
        for (ibit = 0U; ibit < 8U; ibit++)
        {
            crc <<= 1U;
            if (((c ^ crc) & 0x80U) != 0)
            {
                crc ^= 0x09U;
            }
            c <<= 1U;
        }
        crc &= 0x7FU;
        pc++;
    }
    return crc;
}

/**\brief Transmit state function*/
static Int32 mmcsdTranStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    DDC_MmcsdIoPacket*  iop;
    Uint32              blkSize;
    Uint16              rwThreshold;
    LLC_MmcsdCmd        cmd;
    Uint32              address;
    Uint32              maxSectorsPerTxRx = 0;
    Int32               result = IOM_COMPLETED;
    LLC_MmcsdDmaSetup   dmaSetup;
    Bool                flag = TRUE;
    Bool                isEdmaLink = EDMA_LINKING_ALLOWED;

#ifdef MMCSD_DEBUG
    state = DDC_MMCSD_DEVICESTATE_TRAN;
    dataTransfered = 0;
    transtcnt++;
#endif
    assert(NULL != devDDC);
    status = status;        /* to remove compilation warning */
    do
    {
        flag = FALSE;
        iop = QUE_head(devDDC->listHeadNode);
        if (NULL == iop)
        {
            result = IOM_EBADMODE;
            break;
        }

        blkSize = devDDC->blkSize;
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;

        /*addressing changes for a high capacity card*/
        if (TRUE == devDDC->isHighCapacitySDCard)
        {
            address = iop->blockAddress + (iop->xferActual / devDDC->blkSize);
        }
        else
        {
            address = iop->blockAddress + iop->xferActual;
        }
        /* if cancel pending, then take the packet & finish it's processing
         * without touching the hardware; if sync pending & command is of type
         * 'read' then do the same */
        if ((((devDDC->flags & DDC_MMCSD_FLAG_CANCELPENDING) != 0) ||
            ((devDDC->flags & DDC_MMCSD_FLAG_DEVICE_ERROR) != 0)) ||
            (((devDDC->flags & DDC_MMCSD_FLAG_SYNCPENDING) != 0) &&
             (DDC_MMCSD_IOCMD_READ == iop->cmd)))
        {

            /* if cancel is set; then finish off the processing & call
             * mmcsdFxnIoComplete */
            if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
            {
                if (DDC_MMCSD_IOCMD_READ == iop->cmd)
                {
                    LLC_mmcsdDmaStop(
                        ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcRxCh,
                        devDDC->hEdma);
                    LLC_mmcsdDmaCleanChannel(
                        ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcRxCh,
                        devDDC->hEdma);
                }
                else
                {
                    LLC_mmcsdDmaStop(
                        ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcTxCh,
                        devDDC->hEdma);
                    LLC_mmcsdDmaCleanChannel(
                        ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcTxCh,
                        devDDC->hEdma);
                }
            }
            iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
            if (TRUE == (mmcsdFinishIopProcessingAndIsIopPending(devDDC,
                             &result)))
            {
                result = (devDDC->fsm[devDDC->devState])(devDDC, 0);
            }
            break;
        }

        iop->result = IOM_PENDING;
        cmd.busy = LLC_MMCSD_RESPBUSY_NO;
        cmd.lineChr = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode = devDDC->opMode;
        cmd.type = LLC_MMCSD_CMDTYPE_ADTC;
        cmd.respType = LLC_MMCSD_RESPTYPE_R1;

        LLC_mmcsdSetBlockLen(devDDC->hLlc, blkSize);
        /*This is feature of driver which is by default set to FALSE. Klocwork
        throws error at this as "Code is unreachable"*/
        if (TRUE == isEdmaLink)
        {
            maxSectorsPerTxRx = MAX_SECTORS_PER_EDMA_CHANNEL *
                                             EDMA_MAX_LOGICAL_CHA_ALLOWED;
        }
        else
        {
            maxSectorsPerTxRx = MAX_SECTORS_PER_EDMA_CHANNEL;
        }

        if (((iop->xferRequest / devDDC->blkSize) > maxSectorsPerTxRx) &&
            (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode))
        {
            if ((iop->xferRequest - iop->xferActual) >=
               (maxSectorsPerTxRx * devDDC->blkSize))
            {
                devDDC->chunkSize = maxSectorsPerTxRx * devDDC->blkSize;
            }
            else
            {
                devDDC->chunkSize = iop->xferRequest - iop->xferActual;
            }
        }
        else
        {
            devDDC->chunkSize = iop->xferRequest;
        }
        LLC_mmcsdSetNumBlocks(devDDC->hLlc,
            devDDC->chunkSize / devDDC->blkSize);

        switch (iop->cmd)
        {
            case DDC_MMCSD_IOCMD_READ:
                if ((MMCSD_DEFAULT_BLOCK_LENGTH == iop->xferRequest) ||
                    ((TRUE == devDDC->isHighCapacitySDCard) &&
                    (ONE_BLOCK_REQUEST == iop->xferRequest)))
                {
                    cmd.idx = LLC_MMCSD_READ_SINGLE_BLOCK;
                }
                else
                {
                    cmd.idx = LLC_MMCSD_READ_MULTIPLE_BLOCK;
                }
                cmd.dir = LLC_MMCSD_DIR_READ;
                devDDC->devState = DDC_MMCSD_DEVICESTATE_READ;
                /*Configure the FIFO*/
                LLC_mmcsdConfigureFIFO(devDDC->hLlc,
                    LLC_MMCSD_DIR_READ,
                    rwThreshold);
                break;

            case DDC_MMCSD_IOCMD_WRITE:
                if ((MMCSD_DEFAULT_BLOCK_LENGTH == iop->xferRequest) ||
                    ((TRUE == devDDC->isHighCapacitySDCard) &&
                    (ONE_BLOCK_REQUEST == iop->xferRequest)))
                {
                    cmd.idx = LLC_MMCSD_WRITE_BLOCK;
                }
                else
                {
                    cmd.idx = LLC_MMCSD_WRITE_MULTIPLE_BLOCK;
                }
                cmd.dir = LLC_MMCSD_DIR_WRITE;
                cmd.busy = LLC_MMCSD_RESPBUSY_NO;
                devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE;
                /*Configure the FIFO*/
                LLC_mmcsdConfigureFIFO(devDDC->hLlc,
                    LLC_MMCSD_DIR_WRITE,
                    rwThreshold);

                if (PSP_MMCSD_OPMODE_POLLED == devDDC->opMode)
                {
                    LLC_mmcsdWrite(devDDC->hLlc,
                        rwThreshold,
                        (Uint32*)(iop->buf) + \
                            (iop->xferActual / FIFO_WIDTH_IN_BYTES));
                    iop->xferActual += rwThreshold;
                }
                break;
            default:
                /*print error*/
                break;
        }

        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
        {
            if (0 == (iop->xferActual))
            {
                /*if write, clean the cache*/
                if (LLC_MMCSD_DIR_WRITE == cmd.dir)
                {
                    BCACHE_wb((Ptr)(iop->buf), iop->xferRequest, TRUE);
                }
                else
                {
                    BCACHE_wbInv((Ptr)(iop->buf), iop->xferRequest, TRUE);
                }
            }
#ifdef MMCSD_INSTRUMENTATION_ENABLED
        if (DDC_MMCSD_IOCMD_WRITE == iop->cmd){
            PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                        PSP_DVT_eDATA_SND_START,
                        PSP_DVT_dCHANNEL_ID_INST,
                        PSP_DVT_dPACKET_ID,
                        PSP_DVT_dNONE
                        ),
                CHANNELID_INSTANCEID(
                    iop->cmd,
                    0),
                iop->buf
                );
        }
        else if (DDC_MMCSD_IOCMD_READ == iop->cmd){
            PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                        PSP_DVT_eRCV_START,
                        PSP_DVT_dCHANNEL_ID_INST,
                        PSP_DVT_dPACKET_ID,
                        PSP_DVT_dNONE
                       ),
                CHANNELID_INSTANCEID(
                    iop->cmd,
                    0),
                iop->buf
                );
        }
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
            if (DDC_MMCSD_IOCMD_READ == iop->cmd)
            {
                LLC_mmcsdDmaStop(
                    ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcRxCh,
                    devDDC->hEdma);
            }
            else
            {
                LLC_mmcsdDmaStop(
                    ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcTxCh,
                    devDDC->hEdma);
            }
            devDDC->actualChunkXfered = 0;
            /* setup dma for transfer */
            dmaSetup.dir = cmd.dir;
            dmaSetup.address = (Ptr)(((Uint32*)(iop->buf)) + \
                                   ((iop->xferActual) / FIFO_WIDTH_IN_BYTES));
            dmaSetup.numWords = ((devDDC->chunkSize) / FIFO_WIDTH_IN_BYTES);
            dmaSetup.frameLength = (rwThreshold / FIFO_WIDTH_IN_BYTES);

            LLC_mmcsdDmaSetupXfer(devDDC->hLlc,
                &dmaSetup,
                devDDC->hEdma,
                rwThreshold,
                devDDC->eventQ);

            if (DDC_MMCSD_IOCMD_READ == iop->cmd)
            {
                LLC_mmcsdDmaStart(
                    ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcRxCh,
                    devDDC->hEdma);
            }
            else
            {
                LLC_mmcsdDmaStart(
                    ((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcTxCh,
                    devDDC->hEdma);
            }
        }

        LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, address);
        if (PSP_MMCSD_OPMODE_POLLED == devDDC->opMode)
        {
            devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
            if (DDC_MMCSD_IOCMD_WRITE == iop->cmd)
            {
                result = mmcsdWritePolledMode(devDDC);
            }
            else
            {
                result = mmcsdReadPolledMode(devDDC);
            }
        }

        if ((PSP_MMCSD_OPMODE_POLLED == devDDC->opMode) &&
            (result != IOM_COMPLETED))
        {
            devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
            /*remove the packet from pending queue*/
            iop = (DDC_MmcsdIoPacket*)QUE_get(devDDC->listHeadNode);
            mmcsdFxnIoComplete(iop->hDDC, iop);
            SEM_post(devDDC->semAlignDrvrThreads);
        }
    } while(flag);
    return result;
}

/**\brief get status from the card*/
static Int32 mmcsdGetStatus(DDC_MmcsdDeviceHandle   devDDC,
                            Uint16*                 pStatus,
                            Uint16                  errStatus,
                            Uint16                  reqdStatus)
{
    Uint16      gatingStatus = (errStatus |reqdStatus);
    Uint32      cnt = MMCSD_TIME_OUT_COUNT;
    Int32 status = IOM_COMPLETED;

    assert((NULL != devDDC) && (NULL != pStatus));

    if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
    {
        /* Some cards require more time to generate the event*/
        cnt = 0xFFFFU;
    }

    *pStatus = 0;

    while ((0 == ((*pStatus) & gatingStatus)) && (cnt != 0))
    {
        LLC_mmcsdGetEventStatus(devDDC->hLlc, pStatus);
        cnt--;
    }

    if (((((*pStatus) & errStatus) != 0) ||
        (0 == ((*pStatus) & reqdStatus))) ||
        (0 == cnt))
    {
        status = PSP_MMCSD_RESULT_ERROR_FAILURE;
    }

    return status;
}

/**\brief get status during error*/
static Int32 mmcsdGetStopStatusInError(DDC_MmcsdDeviceHandle    devDDC,
                                       Uint16* const            pStatus,
                                       Uint16                   errStatus,
                                       Uint16                   reqdStatus)
{
    Uint16 gatingStatus = (errStatus | reqdStatus);
    DDC_MmcsdIoPacket* iop;
    Uint32 temp[MMCSD_MAX_WORDS_IN_FIFO];
    Uint16 rwThreshold;
    Int32 status = IOM_COMPLETED;

    assert((NULL != devDDC) && (NULL != pStatus));

    iop = QUE_head(devDDC->listHeadNode);
    if (NULL == iop)
    {
        status = IOM_EBADMODE;
    }
    if (IOM_COMPLETED == status)
    {
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;
        while ((0 == ((*pStatus) & gatingStatus)))
        {
            if (DDC_MMCSD_IOCMD_WRITE == iop->cmd)
            {
                LLC_mmcsdWrite(devDDC->hLlc, rwThreshold, temp);
            }
            else
            {
                LLC_mmcsdRead(devDDC->hLlc, rwThreshold, temp);
            }
            LLC_mmcsdGetEventStatus(devDDC->hLlc, pStatus);
        }

        if ((((*pStatus) & errStatus) != 0) || (0 == ((*pStatus) & reqdStatus)))
        {
            status = PSP_MMCSD_RESULT_ERROR_FAILURE;
        }
    }
    return status;
}

/**\brief send stop command*/
static Int32 mmcsdSendStopCmd(DDC_MmcsdDeviceHandle devDDC)
{
    Int32 result = IOM_COMPLETED;
    DDC_MmcsdIoPacket* iop;
    Uint16 reqdStatus = LLC_MMCSD_EVENT_EOFCMD;
    Uint16 errStatus = LLC_MMCSD_EVENT_ERROR;
    Uint16 status = 0;
    LLC_MmcsdCmd cmd;

    assert(NULL != devDDC);

    iop = QUE_head(devDDC->listHeadNode);
    if (NULL == iop)
    {
        result = IOM_EBADMODE;
    }

    if (IOM_COMPLETED == result)
    {
        iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
        /*report error using Callback*/
        mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);

        /*send stop transmission command*/
        cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_YES;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;
        LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
        status = 0;
        if (DDC_MMCSD_IOCMD_READ == iop->cmd)
        {
            reqdStatus = LLC_MMCSD_EVENT_EOFCMD;
        }
        else
        {
            reqdStatus = LLC_MMCSD_EVENT_CARD_EXITBUSY;
        }

        result = mmcsdGetStopStatusInError(devDDC,
                     &status,
                     errStatus,
                     reqdStatus);
        if (result != IOM_COMPLETED)
        {
            iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                          LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
                devDDC->flags |= DDC_MMCSD_FLAG_DEVICE_STOPPED;
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            }
        }
        else
        {
            devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
            iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
        }
    }
    return result;
}



/**\brief write mmcsd in polled mode*/
static Int32 mmcsdWritePolledMode(DDC_MmcsdDeviceHandle devDDC)
{
    Int32               result = IOM_COMPLETED;
    DDC_MmcsdIoPacket*  iop;
    Uint16              rwThreshold;
    Uint16              reqdStatus  = LLC_MMCSD_EVENT_EOFCMD;
    Uint16              errStatus   = LLC_MMCSD_EVENT_ERROR;
    Uint16              status      = 0;
    Bool                isIopPending = 0;
    LLC_MmcsdCmd        cmd;
    Bool                isErrorEncountered = FALSE;
    Uint32              rca;
    Uint16              resp[MAX_RESP_SIZE_BYTES];
    Bool                flag = TRUE;

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        iop = QUE_head(devDDC->listHeadNode);
        if (NULL == iop)
        {
            result = IOM_EBADMODE;
            break;
        }

        rwThreshold = devDDC->drvrHwCfg->rwThreshold;

        /*Sent a write command earlier and now check its status/error
        encountered.*/
        result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        if (result != IOM_COMPLETED)
        {
            result = mmcsdSendStopCmd(devDDC);
            break;
        }

        /*MMCSD data transmit register is not ready                           */
        if (0 == (LLC_MMCSD_EVENT_WRITE & status))
        {
            /* Klocwork says "Code is unreachable" but the status is read from
              mmcsd register and its value can be anything. seems to be an error
              in klocwork */
            reqdStatus = LLC_MMCSD_EVENT_WRITE;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
                result = mmcsdSendStopCmd(devDDC);
                break;
            }
        }

        /*MMCSD data transmit register is ready */
        if (0 != (LLC_MMCSD_EVENT_WRITE & status))
        {
            reqdStatus = LLC_MMCSD_EVENT_WRITE;
            /*loop till requested data is not actually transfered*/
            while (iop->xferActual != iop->xferRequest)
            {
                LLC_mmcsdWrite(devDDC->hLlc,
                               rwThreshold,
                               (Uint32*)(iop->buf) + \
                                   (iop->xferActual / FIFO_WIDTH_IN_BYTES));

                iop->xferActual += rwThreshold;
                status = 0;
                result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
                /* error encountered while transfer*/
                if (result != IOM_COMPLETED)
                {
                    result = mmcsdSendStopCmd(devDDC);
                    break;
                }
            }
        }

        /*data is not fully transmitted*/
        if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
        {
            reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
                iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
        }

        TSK_sleep(100U);

        if ((MMCSD_DEFAULT_BLOCK_LENGTH == iop->xferRequest) ||
            ((TRUE == devDDC->isHighCapacitySDCard) &&
            (ONE_BLOCK_REQUEST == iop->xferRequest)))
        {
            /*send status*/
            cmd.idx         = LLC_MMCSD_SEND_STATUS;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;

            rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType) ?
                    ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                    ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

            LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
            status = 0;
            LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);

            if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
            {
                result = mmcsdCheckResponse(devDDC->hLlc,
                             LLC_MMCSD_RESPTYPE_R1,
                             resp);

                /*Check whether card is ready or not*/
                if ((resp[6] & 0x0100U) != 0)
                {
                    LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
                }
                else
                {
                    /*send stop transmission command*/
                    cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
                    cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                    cmd.busy        = LLC_MMCSD_RESPBUSY_YES;
                    cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                    cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                    cmd.opMode      = devDDC->opMode;
                    LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
                    status = 0;
                    reqdStatus = LLC_MMCSD_EVENT_CARD_EXITBUSY;
                    result = mmcsdGetStopStatusInError(devDDC,
                                 &status,
                                 errStatus,
                                 reqdStatus);
                    if (result != IOM_COMPLETED)
                    {
                        iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                        mmcsdControlCb(devDDC,
                            DDC_MMCSD_HWEVENT_ERROR_STATUS,
                            &status);
                        if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                                      LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
                        {
                            devDDC->flags |= DDC_MMCSD_FLAG_DEVICE_STOPPED;
                            devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
                        }
                    }
                    else
                    {
                        devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                        iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                    }
                    break;
                }
            }
        }
        else
        {
            /*send stop transmission command*/
            cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_YES;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;
            LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
            status = 0;
            reqdStatus = LLC_MMCSD_EVENT_CARD_EXITBUSY;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        }

        TSK_sleep(100U);
        if (result != IOM_COMPLETED)
        {
            iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            isErrorEncountered = TRUE;
            mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                          LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
                devDDC->flags |= DDC_MMCSD_FLAG_DEVICE_STOPPED;
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            }
        }
        else
        {
            devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
            if (FALSE == isErrorEncountered)
            {
                isIopPending = mmcsdFinishIopProcessingAndIsIopPending(devDDC,
                                   &result);
            }

            if (TRUE == isIopPending)
            {
                result = (devDDC->fsm[devDDC->devState])(devDDC, 0);
            }
            else
            {
                if (TRUE == isErrorEncountered)
                {
                    iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                    result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                }
                else
                {
                    result = IOM_COMPLETED;
                    iop->result = IOM_COMPLETED;
                }
            }
        }
    } while(flag);

    return result;
}

/**\brief read mmcsd in polled mode*/
static Int32 mmcsdReadPolledMode(DDC_MmcsdDeviceHandle devDDC)
{
    Int32               result = IOM_COMPLETED;
    DDC_MmcsdIoPacket*  iop;
    Uint16              rwThreshold;
    Uint16              reqdStatus = LLC_MMCSD_EVENT_EOFCMD;
    Uint16              errStatus = LLC_MMCSD_EVENT_ERROR;
    Uint16              status = 0;
    Bool                isIopPending = 0;
    LLC_MmcsdCmd        cmd;
    Bool                isErrorEncountered = FALSE;
    Uint32              rca;
    Bool                flag = TRUE;

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        iop = QUE_head(devDDC->listHeadNode);
        if (NULL == iop)
        {
            result = IOM_EBADMODE;
            break;
        }

        rwThreshold = devDDC->drvrHwCfg->rwThreshold;
        /*Sent a read command earlier and now check its status/error
        encountered. */
        result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        if (result != IOM_COMPLETED)
        {
            result = mmcsdSendStopCmd(devDDC);
            break;
        }

        /*MMCSD data receive register is not ready                            */
        if (0 == (LLC_MMCSD_EVENT_READ & status))
        {
            /*Klocwork says "Code is unreachable" but the status is read from
              mmcsd register and its value can be anything. seems to be an error
              in klocwork*/
            reqdStatus = LLC_MMCSD_EVENT_READ;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
                result = mmcsdSendStopCmd(devDDC);
                break;
            }
        }

        /*MMCSD data receive register is ready                                */
        if ((LLC_MMCSD_EVENT_READ & status) != 0)
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            /*loop till requested data is not actually received               */
            while (iop->xferActual != iop->xferRequest)
            {
                LLC_mmcsdRead(devDDC->hLlc,
                              rwThreshold,
                              (Uint32*)(iop->buf) + \
                                  (iop->xferActual / FIFO_WIDTH_IN_BYTES));

                iop->xferActual += rwThreshold;

                if (iop->xferActual != (iop->xferRequest/*- rwThreshold*/))
                {
                    status = 0;
                    result = mmcsdGetStatus(devDDC,
                                 &status,
                                 errStatus,
                                 reqdStatus);
                    /* error encountered while receive*/
                    if (result != IOM_COMPLETED)
                    {
                        result = mmcsdSendStopCmd(devDDC);
                        break;
                    }
                }
            }
        }

        if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
        {
            reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
                iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
        }

        if ((MMCSD_DEFAULT_BLOCK_LENGTH == iop->xferRequest) ||
            ((TRUE == devDDC->isHighCapacitySDCard) &&
            (ONE_BLOCK_REQUEST == iop->xferRequest)))
        {
            /*send status*/
            cmd.idx         = LLC_MMCSD_SEND_STATUS;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;

            rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType) ?
                    ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                    ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

            LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
        }
        else
        {
            cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_YES;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;
            LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
        }
        status = 0;
        reqdStatus = LLC_MMCSD_EVENT_EOFCMD;
        result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        if (result != IOM_COMPLETED)
        {
            iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            isErrorEncountered = TRUE;
            mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                          LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
                devDDC->flags |= DDC_MMCSD_FLAG_DEVICE_STOPPED;
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            }

        }
        else
        {
            devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
            if (FALSE == isErrorEncountered)
            {
                isIopPending = mmcsdFinishIopProcessingAndIsIopPending(devDDC,
                                   &result);
            }

            if (TRUE == isIopPending)
            {
                result = (devDDC->fsm[devDDC->devState])(devDDC, 0);
            }
            else
            {
                if (TRUE == isErrorEncountered)
                {
                    iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                    result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                }
                else
                {
                    result = IOM_COMPLETED;
                    iop->result = IOM_COMPLETED;
                }
            }
        }
    }while (flag);
    return result;
}

/**\brief Device is reading in DMA mode*/
static Int32 mmcsdReadStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    Int32 result = IOM_COMPLETED;
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    LLC_MmcsdCmd cmd;
    Uint32 rca;
    DDC_MmcsdIoPacket* iop;
    Bool flag = TRUE;

#ifdef MMCSD_DEBUG
    state = DDC_MMCSD_DEVICESTATE_READ;
#endif

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        /*Check whether end of command is received for the read command*/
        if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
        {
            /* check the response to make sure all is OK */
            result = mmcsdCheckResponse(devDDC->hLlc,
                         LLC_MMCSD_RESPTYPE_R1,
                         resp);
            if (IOM_COMPLETED != result)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_RESPONSE, NULL);
                /* In the case of error, STOP TRANSMISSION Cmd is required and
                   so switch to DDC_MMCSD_DEVICESTATE_WRITE_STOP*/
                devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                /*send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
                result = IOM_COMPLETED;
                break;
            }
        }

        /*Check whether block transfer event is received for the read command*/
        if ((status & LLC_MMCSD_EVENT_BLOCK_XFERRED) != 0)
        {
            iop = QUE_head(devDDC->listHeadNode);
            if (NULL == iop)
            {
                result = IOM_EBADMODE;
                break;
            }

            devDDC->devState = DDC_MMCSD_DEVICESTATE_READ_STOP;
            iop->xferActual = iop->xferActual + devDDC->chunkSize;
            devDDC->actualChunkXfered = devDDC->chunkSize;

#ifdef MMCSD_DEBUG
            dataTransfered = iop->xferActual;
#endif

            if (MMCSD_DEFAULT_BLOCK_LENGTH == iop->xferRequest)
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_FINISH_BUSY;
                /*send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
                result = IOM_COMPLETED;
            }
            else/* transfer request for more than 1 block in dma mode*/
            {
                mmcsdReadStopStFxn(devDDC,status);
            }
            break;
        }
        else /*Waiting for interrupt LLC_MMCSD_EVENT_BLOCK_XFERRED and so ret.*/
        {
            result = IOM_COMPLETED;
            break;
        }
    } while(flag);
    return result;
}

/**\brief Send stop cmd in dma mode*/
static Int32 mmcsdReadStopStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    LLC_MmcsdCmd cmd;

#ifdef MMCSD_DEBUG
    state = DDC_MMCSD_DEVICESTATE_READ_STOP;
#endif

    assert(NULL != devDDC);
    status = status;        /* to remove compilation warning */

    /*send stop transmission*/
    cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
    cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
    cmd.type        = LLC_MMCSD_CMDTYPE_AC;
    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
    cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
    cmd.opMode      = devDDC->opMode;

    LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
    devDDC->devState = DDC_MMCSD_DEVICESTATE_STATUS;
    return IOM_COMPLETED;
}

/**\brief check if the top of list iop processing is done & removes it
 * from pending list. Releases the driver-threads-align-semaphore.
 * returns TRUE if more iops pending & FALSE otherwise */
static Bool mmcsdFinishIopProcessingAndIsIopPending(
                DDC_MmcsdDeviceHandle         devDDC,
                Int32*             result)
{
    Uint32 intState;
    DDC_MmcsdIoPacket* iop;
    Bool status = TRUE;
    Bool flag = TRUE;
    Int32 pwrResult;

    assert((NULL != devDDC) && (NULL != result));

    do
    {
        flag = FALSE;
        *result = IOM_COMPLETED;
        /*remove the packet from pending queue*/
        iop = (DDC_MmcsdIoPacket*)QUE_get(devDDC->listHeadNode);
        if (NULL == iop)
        {
            status = FALSE;
            break;
        }

        /* If the driver is power aware disable the PSC */
        if (TRUE == iop->hDDC->oLlc.isPwrAware)
        {
            /* Disable LPSC */
            pwrResult = mmcsdLpscOff(iop->hDDC);
            if (IOM_COMPLETED != pwrResult)
            {
                status = FALSE;
                break;
            }
        }

        mmcsdFxnIoComplete(iop->hDDC, iop);

        if ((devDDC->flags & DDC_MMCSD_FLAG_DEVICE_ERROR) != 0)
        {
            devDDC->flags &= ~DDC_MMCSD_FLAG_DEVICE_ERROR;
        }

        devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
        intState = HWI_disable();

        if ((QUE_empty(devDDC->listHeadNode)) == TRUE)
        {
            if ((devDDC->flags & DDC_MMCSD_FLAG_DEVICE_STOPPED) != 0)
            {
                devDDC->flags &= ~DDC_MMCSD_FLAG_CANCELPENDING;
                devDDC->devState = DDC_MMCSD_DEVICESTATE_START;
            }

            if ((devDDC->flags & DDC_MMCSD_FLAG_SYNCPENDING) != 0)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_SYNC_DONE, NULL);
            }

            if ((devDDC->flags & DDC_MMCSD_FLAG_CANCELPENDING) != 0)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_CANCEL_DONE, NULL);
            }

            devDDC->flags &= ~(DDC_MMCSD_FLAG_SYNCPENDING |
                            DDC_MMCSD_FLAG_CANCELPENDING);

            SEM_post(devDDC->semAlignDrvrThreads);
            HWI_restore(intState);
            status = FALSE;
            break;
        }

        HWI_restore(intState);
    } while(flag);
    return status;
}

/**\brief Driver is writing in DMA mode                                       */
static Int32 mmcsdWriteStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    DDC_MmcsdIoPacket* iop;
    LLC_MmcsdCmd cmd;
    Uint32 rca;
    Int32 result = IOM_COMPLETED;
    Bool flag = TRUE;

#ifdef MMCSD_DEBUG
    printf("mmcsdWriteStFxn in start:%l\n",status);
    state = DDC_MMCSD_DEVICESTATE_WRITE;
#endif

    assert(NULL != devDDC);
    do
    {
        flag = FALSE;
        /*Check whether end of command is received for the write command*/
        if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
        {
            /* check the response to make sure all is OK */
            result = mmcsdCheckResponse(devDDC->hLlc,
                         LLC_MMCSD_RESPTYPE_R1,
                         resp);
            if (IOM_COMPLETED != result)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_RESPONSE, NULL);
                devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                /*send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
                result = IOM_COMPLETED;
                break;
            }
        }

        /*Check whether block transfer event is received for the write command*/
        if ((status & LLC_MMCSD_EVENT_BLOCK_XFERRED) != 0)
        {
            iop = QUE_head(devDDC->listHeadNode);
            if (NULL == iop)
            {
                result = IOM_EBADMODE;
                break;
            }

            devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
            iop->xferActual = iop->xferActual + devDDC->chunkSize;
            devDDC->actualChunkXfered = devDDC->chunkSize;

#ifdef MMCSD_DEBUG
            dataTransfered= iop->xferActual;
#endif

            if (iop->xferRequest > MMCSD_DEFAULT_BLOCK_LENGTH)
            {
                mmcsdWriteStopStFxn(devDDC, status);
            }
            else/* transfer request for 1 block in dma mode*/
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_FINISH_BUSY;
                /*Send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
            }
            result = IOM_COMPLETED;
            break;
        }
        else /*Waiting for interrupt LLC_MMCSD_EVENT_BLOCK_XFERRED and so ret.*/
        {
            result = IOM_COMPLETED;
            break;
        }
    } while(flag);
    return result;
}

/**\brief Send stop command during write*/
static Int32 mmcsdWriteStopStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    LLC_MmcsdCmd        cmd;
    Uint32              temp[MMCSD_MAX_WORDS_IN_FIFO];
    DDC_MmcsdIoPacket*  iop;
    Uint16              rwThreshold;
    Int32               retVal = IOM_COMPLETED;

#ifdef MMCSD_DEBUG
    printf("mmcsdWriteStopStFxn in start:%l\n",status);
    state = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
#endif

    assert(NULL != devDDC);

    iop = QUE_head(devDDC->listHeadNode);
    if (NULL == iop)
    {
        retVal = IOM_EBADMODE;
    }
    else
    {
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;

        /*STOP_TRANSMISSION*/
        cmd.idx         = LLC_MMCSD_STOP_TRANSMISSION;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, 0U);
        if ((iop->xferRequest != iop->xferActual) ||
            ((devDDC->actualChunkXfered != devDDC->chunkSize)&&
            (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)))
        {
            if ((status & (LLC_MMCSD_EVENT_ERROR_DATACRC |
                           LLC_MMCSD_EVENT_ERROR_CMDCRC)) != 0)
            {
                if (DDC_MMCSD_IOCMD_WRITE == iop->cmd)
                {
                    LLC_mmcsdWrite(devDDC->hLlc, rwThreshold, (Uint32*)temp);
                }
                else
                {
                    LLC_mmcsdRead(devDDC->hLlc, rwThreshold, (Uint32*)temp);
                }
            }
        }
        devDDC->devState = DDC_MMCSD_DEVICESTATE_STATUS;
    }
    return retVal;
}

/**\brief Send status cmd to the card*/
static Int32 mmcsdSendStatusStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    LLC_MmcsdCmd        cmd;
    Uint32              rca;
    Int32               result = IOM_COMPLETED;

#ifdef MMCSD_DEBUG
    printf("mmcsdSendStatusStFxn in start:%l\n",status);
    state = DDC_MMCSD_DEVICESTATE_STATUS;
#endif

    assert(NULL != devDDC);

    if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
    {
        /*Send Status*/
        cmd.idx         = LLC_MMCSD_SEND_STATUS;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_YES;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType) ?
                ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

        LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
        devDDC->devState = DDC_MMCSD_DEVICESTATE_FINISH_CHECK;
    }
    else
    {
        /*Command completion interrupt has not come yet*/
    }
    return result;
}


/**\brief Check status*/
static Int32 mmcsdFinishCheckStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    Int32 result = IOM_COMPLETED;
    DDC_MmcsdIoPacket*  iop;
    Uint16              resp[MAX_RESP_SIZE_BYTES];
    LLC_MmcsdCmd        cmd;
    Uint32              rca;
    Bool                flag = TRUE;
#ifdef MMCSD_DEBUG
    printf("mmcsdFinishCheckStFxn in start:%l\n",status);
    state = DDC_MMCSD_DEVICESTATE_FINISH_CHECK;
#endif

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        iop = QUE_head(devDDC->listHeadNode);
        if (NULL == iop)
        {
            result = IOM_EBADMODE;
            break;
        }

        if ((status & LLC_MMCSD_EVENT_CARD_EXITBUSY) != 0)
        {
            result = mmcsdCheckResponse(devDDC->hLlc,
                         LLC_MMCSD_RESPTYPE_R1,
                         resp);

            /*Check whether card is ready or not*/
            if ((resp[6] & 0x0100U) != 0)
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                /* Clear status register--it's required because it's set because
                of mmcsdCheckResponse call */

                LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
                /* if current transfer is done then check if more iops present;
                 * if not, release sem & return otherwise start processing the
                 * next iop
                 */
                if ((iop->xferRequest == iop->xferActual) ||
                    ((devDDC->actualChunkXfered == devDDC->chunkSize)&&
                    (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)))
                {
                    iop->result = IOM_COMPLETED;
                }
                else
                {
                    iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                }
                if ((iop->xferRequest == iop->xferActual) ||
                    (iop->result != IOM_COMPLETED))
                {
                    if (FALSE == mmcsdFinishIopProcessingAndIsIopPending(devDDC,
                                     &result))
                    {
                        result = IOM_COMPLETED;
                        break;
                    }
                }
                result = (devDDC->fsm[devDDC->devState])(devDDC, 0);
                break;
            }
            else
            {
#ifdef MMCSD_DEBUG
                BusyHit++;
                printf("mmcsdFinishCheckStFxn- Card state is not ready");
                for (;;)
                    ;
#endif
                /*send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                    ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                    ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
                devDDC->devState = DDC_MMCSD_DEVICESTATE_FINISH_BUSY;
                result = IOM_COMPLETED;
                break;
            }
        }
        else
        {
            /*Command completion interrupt has not come yet*/
            result = IOM_COMPLETED;
            break;
        }
    }while (flag);
    return result;
}

/**\brief Finish busy state*/
static Int32 mmcsdFinishBusyStFxn(DDC_MmcsdDeviceHandle devDDC, Uint16 status)
{
    Int32               result = IOM_COMPLETED;
    DDC_MmcsdIoPacket*  iop;
    Uint16              resp[MAX_RESP_SIZE_BYTES];
    LLC_MmcsdCmd        cmd;
    Uint32              rca;
    Bool                flag = TRUE;
#ifdef MMCSD_DEBUG
    printf("mmcsdFinishBusyStFxn in start:%l\n",status);
    state = DDC_MMCSD_DEVICESTATE_FINISH_BUSY;
#endif

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        iop = QUE_head(devDDC->listHeadNode);
        if (NULL == iop)
        {
            result = IOM_EBADMODE;
            break;
        }

        if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
        {
            result = mmcsdCheckResponse(devDDC->hLlc,
                         LLC_MMCSD_RESPTYPE_R1,
                         resp);

            /*Check whether card is ready or not*/
            if ((resp[6] & 0x0100U) != 0)
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                /* Clear status register--it's required because it's set because
                of mmcsdCheckResponse call */
                LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);

                /* if current transfer is done then check if more iops present;
                 * if not, release sem & return otherwise start processing the
                 * next iop
                 */
                if ((iop->xferRequest == iop->xferActual) ||
                    ((devDDC->actualChunkXfered == devDDC->chunkSize)&&
                    (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)))
                {
                    iop->result = IOM_COMPLETED;
                }
                else
                {
                    iop->result = PSP_MMCSD_RESULT_ERROR_CANCELLED;
                }
                if ((iop->xferRequest == iop->xferActual) ||
                    (iop->result != IOM_COMPLETED))
                {
                    if (FALSE == mmcsdFinishIopProcessingAndIsIopPending(devDDC,
                                     &result))
                    {
                        result = IOM_COMPLETED;
                        break;
                    }
                }
                result = (devDDC->fsm[devDDC->devState])(devDDC, 0);
            }
            else /*Modified for Movie Trim issue*/
            {
                /*send status*/
                cmd.idx         = LLC_MMCSD_SEND_STATUS;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmd.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                    ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                    ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmd, rca);
                devDDC->devState = DDC_MMCSD_DEVICESTATE_FINISH_BUSY;
                result = IOM_COMPLETED;
                break;
            }
        }
        else
        {
            /*Command completion interruot has not come yet*/
            result = IOM_COMPLETED;
            break;
        }
    } while(flag);
    return result;
}

/**\brief Check response from the card. The response will interpreted as:
     * response[0] - R2[15:0]
     * response[1] - R2[31:16]
     * response[2] - R2[47:32]
     * response[3] - R2[63:48]
     * response[4] - R2[79:64]
     * response[5] - R2[95:80]
     * response[6] - R2[111:96] or R1/R1B/R3/R4/R5/R6[23:8]
     * response[7] - R2[127:112] or R1/R1B/R3/R4/R5/R6[39:24]
*/
static Int32 mmcsdCheckResponse(LLC_MmcsdHandle         hLlc,
                                LLC_MmcsdRespType       respType,
                                Uint16*   const         resp)
{
    Int32       result = IOM_COMPLETED;
    Uint32      mask;

    assert((NULL != hLlc) && (NULL != resp));

    switch (respType)
    {
        case LLC_MMCSD_RESPTYPE_NO:
            break;

        case LLC_MMCSD_RESPTYPE_R1:
            mask = LLC_MMCSD_RESPONSE_R1_ERROR_MASK;
            break;

        case LLC_MMCSD_RESPTYPE_R6:
            mask = LLC_MMCSD_RESPONSE_R6_ERROR_MASK;
            break;

        default: /* R2 - R5 responses */
            result = LLC_mmcsdGetResp(hLlc, respType, resp);
            break;
    }

    if ((LLC_MMCSD_RESPTYPE_R1 == respType) ||
        (LLC_MMCSD_RESPTYPE_R6 == respType))
    {
        result = LLC_mmcsdGetResp(hLlc, respType, resp);
        if (0 == ((Uint32)(((((Uint32)resp[7]) << 16) | resp[6])) & mask))
        {
            result = IOM_COMPLETED;
        }
        else
        {
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
        }
    }
    return result;
}

/**\brief get status of the card*/
static Bool mmcsdGetStatusFromCard(DDC_MmcsdDeviceHandle devDDC,
                                   Uint16* resp,
                                   PSP_MmcsdCardType card)
{
    Int32 result;
    LLC_MmcsdCmd cmd;
    Uint32 rca = 0;
    Bool status = TRUE;
    Bool statusFlag = FALSE;

    assert((NULL != devDDC) && (NULL != resp));

    if (PSP_MMCSD_CARDTYPE_MMC == card)
    {
        rca = devDDC->cardRegs.mmc.rca;
        rca <<= RCA_SHIFT_MASK;
    }
    else if (PSP_MMCSD_CARDTYPE_SD == card)
    {
        rca = devDDC->cardRegs.sd.rca;
        rca <<= RCA_SHIFT_MASK;
    }
    else /*print error*/
    {
        MMCSD_DRV_DEBUG(("\r\n mmcsdGetStatusFromCard() invalid card type "));
    }
    statusFlag = TRUE;
    while (statusFlag)
    {
        statusFlag = TRUE;
        /*send status*/
        cmd.idx         = LLC_MMCSD_SEND_STATUS;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            status = FALSE;
            break;
        }

        if (0x800 == (resp[6] & ((1 << 12) - (1 << 9))))
        {
            /*current state of device is tran state*/
            break;
        }
        TSK_sleep(1U);
    }
    return status;
}

/**\brief check whether the card is high speed or not*/
static Void mmcsdCheckHighSpeedCard(DDC_MmcsdDeviceHandle devDDC, Uint16* resp)
{
    LLC_MmcsdObj* llc;
    LLC_MmcsdCmd cmd;
    Int32 result;
    Uint16 errStatus = LLC_MMCSD_EVENT_ERROR;
    Uint16 reqdStatus;
    Uint16 status;
    Uint32 readBuf[16] = {0};/*Klocwork throws error here bec of bug in tool*/
    Uint8 switchFucSt[64] = {0};/*Klocwork throws err. here bec of bug in tool*/
    Uint32 dataCnt = 0, tempDataCnt = 0;
    Uint16 rwThreshold;
    Bool flag = TRUE;

    assert((NULL != devDDC) && (NULL != resp));
    llc = (LLC_MmcsdObj*)devDDC->hLlc;
    assert(NULL != llc);

    do
    {
        flag = FALSE;
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;
        LLC_mmcsdSetBlockLen(devDDC->hLlc, 64U);
        LLC_mmcsdSetNumBlocks(devDDC->hLlc, ONE_BLOCK_REQUEST);
        /*Configure the FIFO for read*/
        LLC_mmcsdConfigureFIFO(llc, LLC_MMCSD_DIR_READ, rwThreshold);

        /*SWITCH_FUNC*/
        cmd.idx         = LLC_MMCSD_SWITCH_FUNC;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_ADTC;
        cmd.dir         = LLC_MMCSD_DIR_READ;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = PSP_MMCSD_OPMODE_POLLED;

        /* Read switch function status in Mode 0 to find out which all
        switch function is supported*/
        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     0x00FFFF01U,/*Mode = Check function*/
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if ((IOM_COMPLETED == result) && (0 == (status & LLC_MMCSD_EVENT_READ)))
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
#ifdef SCR_HIGH_SPEED_DEBUG
                printf("\r\nMMCSD Failed at waiting for Ready after CMD6");
#endif
                mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
                break;
            }
        }
        else
        {
            MMCSD_INIT_DEBUG(("\r\n MMCSD : Failed at CMD6"));
            mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
            break;
        }
        /* Wait for switch function to switch the mode*/
        /*Read the data from FIFO*/
        dataCnt = 0;
        tempDataCnt = 0;
        if ((LLC_MMCSD_EVENT_READ & status) != 0)
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            while (tempDataCnt != MMCSD_SWITCH_CMD_DATA_SIZE_BYTE)
            {

                LLC_mmcsdRead(devDDC->hLlc,
                              rwThreshold,
                              (Ptr)(readBuf + dataCnt));

                dataCnt += (rwThreshold / FIFO_WIDTH_IN_BYTES);
                tempDataCnt += rwThreshold;
                if (tempDataCnt < MMCSD_SWITCH_CMD_DATA_SIZE_BYTE)
                {
                    status = 0;
                    result = mmcsdGetStatus(devDDC,
                                 &status,
                                 errStatus,
                                 reqdStatus);
                    if (result != IOM_COMPLETED)
                    {
#ifdef SCR_HIGH_SPEED_DEBUG
                        printf("\r\nMMCSD : Failed at waiting for 2nd Ready "
                               " after CMD6");
#endif
                        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                    }
                }
            }
        }
        if (PSP_MMCSD_RESULT_ERROR_FAILURE == result)
        {
            break;
        }
        /*Klocwork throws "Buffer overflow, array index of 'readBuf' may be
        outside the bounds" at this line which is not valid in this case*/
        memcpy((Ptr)switchFucSt, (Ptr)readBuf, MMCSD_SWITCH_CMD_DATA_SIZE_BYTE);

        if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
        {
            reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        }
        if (result != IOM_COMPLETED)
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Failed at waiting for blockXfered after CMD6");
#endif
            mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
            break;
        }

        TSK_sleep(1U);

        if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD)))
        {
            mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
            break;
        }
        else
        {
            result = IOM_COMPLETED;
        }

        /*check whether switch is possible or not for high speed */
        if (((switchFucSt[13] & 0x1U) != 0) &&
            (!(0xF == (switchFucSt[16] & 0x0FU))))
        {
            cmd.idx         = LLC_MMCSD_SWITCH_FUNC;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_ADTC;
            cmd.dir         = LLC_MMCSD_DIR_READ;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = PSP_MMCSD_OPMODE_POLLED;

            /* Read switch function status in Mode 1 to switch to high speed*/
            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         0x80FFFF01,/*Mode = Switch function*/
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         &status);
            if ((IOM_COMPLETED == result) &&
                (0 == (status & LLC_MMCSD_EVENT_READ)))
            {
                reqdStatus = LLC_MMCSD_EVENT_READ;
                result = mmcsdGetStatus(devDDC, &status, errStatus,reqdStatus);
                if (result != IOM_COMPLETED)
                {
#ifdef SCR_HIGH_SPEED_DEBUG
                    printf("\r\nMMCSD : Failed at waiting for Ready ready after"
                           " CMD6");
#endif
                    mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
                    break;
                }
            }
            else
            {
                MMCSD_INIT_DEBUG(("\r\n MMCSD : Failed at CMD6"));
                mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
                break;
            }
            /* Wait for switch function to switch the mode*/
            /*Read the data from FIFO*/
            dataCnt = 0;
            tempDataCnt = 0;
            if ((LLC_MMCSD_EVENT_READ & status) != 0)
            {
                reqdStatus = LLC_MMCSD_EVENT_READ;
                while (tempDataCnt != MMCSD_SWITCH_CMD_DATA_SIZE_BYTE)
                {

                    LLC_mmcsdRead(devDDC->hLlc,
                                  rwThreshold,
                                  (Ptr)(readBuf + dataCnt));

                    dataCnt += (rwThreshold / FIFO_WIDTH_IN_BYTES);
                    tempDataCnt += rwThreshold;
                    if (tempDataCnt < MMCSD_SWITCH_CMD_DATA_SIZE_BYTE)
                    {
                        status = 0;
                        result = mmcsdGetStatus(devDDC,
                                     &status,
                                     errStatus,
                                     reqdStatus);
                        if (result != IOM_COMPLETED)
                        {
#ifdef SCR_HIGH_SPEED_DEBUG
                            printf("\r\nMMCSD : Failed at waiting for 2nd Ready"
                                   " ready after CMD6");
#endif
                            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                        }
                    }
                }
            }

            if (PSP_MMCSD_RESULT_ERROR_FAILURE == result)
            {
                break;
            }

            memcpy((Ptr)switchFucSt, (Ptr)readBuf, MMCSD_SWITCH_CMD_DATA_SIZE_BYTE);

            if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
            {
                reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
                result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            }

            if (result != IOM_COMPLETED)
            {
#ifdef SCR_HIGH_SPEED_DEBUG
                printf("\r\nMMCSD : Failed at waiting for blockXfered after CMD6");
#endif
                mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
                break;
            }

            TSK_sleep(1U);

            if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD)))
            {
                mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
                break;
            }
            else
            {
                result = IOM_COMPLETED;
            }
        }
        else
        {
            /*card does not support high speed mode */
            mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
            break;
        }

        if (((IOM_COMPLETED == result) &&
             (!((switchFucSt[16] & 0x0FU) == 0xFU))) &&
            ((switchFucSt[16] & 0x1U) != 0))
        {
            /* Switch to high frequency*/
            MMCSD_INIT_DEBUG(("\r\n MMCSD: SD Card is high speed card"));
            LLC_mmcsdSetFrequency(devDDC->hLlc,
                &(devDDC->hDrvrClock.sdPushPullHighSpeed),
                FALSE);
            devDDC->isHighSpeedSDCard = TRUE;
        }
        else
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Switching to high speed failed");
#endif
            devDDC->isHighSpeedSDCard = FALSE;
        }

        mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD);
    } while(flag);
    return;
}


/**\brief read SD configration register*/
static Int32 mmcsdReadSCR(DDC_MmcsdDeviceHandle devDDC, Uint16* resp)
{
    LLC_MmcsdObj* llc;
    LLC_MmcsdCmd cmd;
    Int32 result;
    Uint16 errStatus = LLC_MMCSD_EVENT_ERROR;
    Uint16 reqdStatus;
    Uint16 status;
    Uint32 i, rca;
    Uint32 readBuf[2];
    Uint16 rwThreshold;
    Bool flag = TRUE;

    assert((NULL != devDDC) && (NULL != resp));
    llc = (LLC_MmcsdObj*)devDDC->hLlc;
    assert(NULL != llc);

    do
    {
        flag = FALSE;
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;
        /*Read the SCR register*/
        LLC_mmcsdSetBlockLen(devDDC->hLlc, MMCSD_SCR_SIZE_BYTE);
        LLC_mmcsdSetNumBlocks(devDDC->hLlc, ONE_BLOCK_REQUEST);
        /*Configure the FIFO for read*/
        LLC_mmcsdConfigureFIFO(llc, LLC_MMCSD_DIR_READ, rwThreshold);

        /* Read SCR register of SD card*/
        cmd.idx         = LLC_MMCSD_APP_CMD;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;
        rca = devDDC->cardRegs.sd.rca;
        rca <<= RCA_SHIFT_MASK;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);

        if (IOM_COMPLETED != result)
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Failed at CMD55 issed b4 ACMD51");
#endif
            break;
        }
        /*Send SCR(SD configration register)*/
        cmd.idx         = LLC_MMCSD_ACMD_SEND_SCR;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_ADTC;
        cmd.dir         = LLC_MMCSD_DIR_READ;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = PSP_MMCSD_OPMODE_DMAINTERRUPT;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     0U,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if ((IOM_COMPLETED == result) && (0 == (status & LLC_MMCSD_EVENT_READ)))
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            result = mmcsdGetStatus(devDDC, &status, errStatus,reqdStatus);
            if (result != IOM_COMPLETED)
            {
#ifdef SCR_HIGH_SPEED_DEBUG
                printf("\r\nMMCSD : Failed at waiting for Read after ACMD51");
#endif
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
        }

        for (i = 0; i < 2U; i++)
        {
            readBuf[i] = llc->regs->MMCDRR;
        }
        /*Klocwork throws "Buffer overflow, array index of 'readBuf' may be
        outside the bounds" at this line which is not valid in this case*/
        memcpy((Ptr)devDDC->cardRegs.sd.scr,
                      (Ptr)readBuf, MMCSD_SCR_SIZE_BYTE);

        if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
        {
            reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        }

        if (result != IOM_COMPLETED)
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Failed at waiting for BlokXfered after ACMD51");
#endif
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }
    } while(flag);
    return result;
}

#ifdef SD_STATUS_READ
/**\brief read SD status value*/
static Int32 mmcsdReadSdStatus(DDC_MmcsdDeviceHandle devDDC, Uint16* resp)
{
    LLC_MmcsdObj* llc;
    LLC_MmcsdCmd cmd;
    Int32 result;
    Uint16 errStatus = LLC_MMCSD_EVENT_ERROR;
    Uint16 reqdStatus;
    Uint16 status;
    Uint32 dataCnt = 0, rca;
    Uint8 sdStatusBuf[MMCSD_SD_STATUS_SIZE_BYTE];
    Uint16 rwThreshold;
    Bool flag = TRUE;

    assert((NULL != devDDC) && (NULL != resp));
    llc = (LLC_MmcsdObj*)devDDC->hLlc;
    assert(NULL != llc);

    do
    {
        flag = FALSE;
        rwThreshold = devDDC->drvrHwCfg->rwThreshold;
        /*Read the SD Status register*/
        LLC_mmcsdSetBlockLen(devDDC->hLlc, MMCSD_SD_STATUS_SIZE_BYTE);
        LLC_mmcsdSetNumBlocks(devDDC->hLlc, ONE_BLOCK_REQUEST);
        /*Configure the FIFO for read*/
        LLC_mmcsdConfigureFIFO(llc, LLC_MMCSD_DIR_READ, rwThreshold);

        /* Read SD status register of SD card*/
        cmd.idx         = LLC_MMCSD_APP_CMD;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;
        rca = devDDC->cardRegs.sd.rca;
        rca <<= RCA_SHIFT_MASK;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Failed at CMD55 issed b4 ACMD51");
#endif
            break;
        }
        /*SD_STATUS*/
        cmd.idx         = LLC_MMCSD_ACMD_SD_STATUS;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_ADTC;
        cmd.dir         = LLC_MMCSD_DIR_READ;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = PSP_MMCSD_OPMODE_DMAINTERRUPT;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     MMCSD_ARGUMENT_STUFF_BITS,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if ((IOM_COMPLETED == result) && (0 == (status & LLC_MMCSD_EVENT_READ)))
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
            if (result != IOM_COMPLETED)
            {
#ifdef SCR_HIGH_SPEED_DEBUG
                printf("\r\nMMCSD : Failed at waiting for Read after ACMD13");
#endif
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
        }

        dataCnt = 0;
        if ((LLC_MMCSD_EVENT_READ & status) != 0)
        {
            reqdStatus = LLC_MMCSD_EVENT_READ;
            while (dataCnt != MMCSD_SD_STATUS_SIZE_BYTE)
            {

                LLC_mmcsdRead(devDDC->hLlc,
                    rwThreshold,
                    (Ptr)(sdStatusBuf + dataCnt));

                dataCnt += rwThreshold;
                if (dataCnt < MMCSD_SD_STATUS_SIZE_BYTE)
                {
                    status = 0;
                    result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
                    if (result != IOM_COMPLETED)
                    {
                        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                        break;
                    }
                }
            }
            if (PSP_MMCSD_RESULT_ERROR_FAILURE == result)
            {
                break;
            }
        }

        if (0 == (status & LLC_MMCSD_EVENT_BLOCK_XFERRED))
        {
            reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
            result = mmcsdGetStatus(devDDC, &status, errStatus, reqdStatus);
        }

        if (result != IOM_COMPLETED)
        {
#ifdef SCR_HIGH_SPEED_DEBUG
            printf("\r\nMMCSD : Failed at waiting for BlokXfered after ACMD13");
#endif
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }
    } while(flag);
    return result;
}
#endif

/**\brief restore interrupts*/
static Void mmcsdRestoreInt(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode)
{
    Uint16 status;

    assert(NULL != devDDC);

    /*Clear the status register before enabling the dma interrupt*/
    /* restore DMA mode after initialization */
    /*Clear the status register before enabling the interrupt to avoid dummy
    interrupt*/
    LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
    /* restore mmcsd level interrupts */
    LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
    if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
    {
        mmcsdConfigDmaMode(devDDC);
    }
}

/**\brief Send card interface information*/
static Int32 mmcsdSendInterfaceInfo(DDC_MmcsdDeviceHandle devDDC,
                                    Uint16 mmcIntMode)
{

    LLC_MmcsdCmd cmd;
    Int32 result = IOM_COMPLETED;
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    Uint16 status;
    Uint32 cnt = DDC_MMCSD_NUM_OF_TRIES;
    Bool flag = TRUE;

    assert(NULL != devDDC);
    do
    {
        flag = FALSE;

        /*Send interface information*/
        cmd.idx         = LLC_MMCSD_SEND_IF_COND;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = PSP_MMCSD_OPMODE_POLLED;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     /*VHS = 0x1 i.e. 0001b mean 2.7-3.6V*/
                     (0x100U | SD_HIGH_CAPACITY_ECHO_PATTERN),
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if (IOM_COMPLETED != result)
        {
            /* card not responded to cmd 8 i.e. card is either
            Ver2.00 or later SD Memory Card(voltage mismatch) or
            Ver1.X SD Memory Card or not SD Memory Card*/
            /*Send cmd 0 for high speed card else it will not work*/
            cmd.idx         = LLC_MMCSD_GO_IDLE_STATE;
            cmd.respType    = LLC_MMCSD_RESPTYPE_NO;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_BC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
            cmd.opMode      = devDDC->opMode;

            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         MMCSD_ARGUMENT_STUFF_BITS,
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         NULL);
            if (IOM_COMPLETED != result)
            {
                mmcsdRestoreInt(devDDC, mmcIntMode);
                break;
            }
            while (0 != cnt)
            {
                cmd.idx         = LLC_MMCSD_APP_CMD;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                cmd.opMode      = devDDC->opMode;

                result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                             &cmd,
                             MMCSD_ARGUMENT_STUFF_BITS,
                             resp,
                             LLC_MMCSD_EVENT_EOFCMD,
                             LLC_MMCSD_EVENT_ERROR,
                             &status);
                if (IOM_COMPLETED != result)
                {
                    mmcsdRestoreInt(devDDC, mmcIntMode);
                    break;
                }
                /*Send OCR*/
                cmd.idx         = LLC_MMCSD_ACMD_SD_SEND_OP_COND;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                cmd.opMode      = devDDC->opMode;
                /*Send ACMD 41 with HCS =0*/
                result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                             &cmd,
                             (0x0U | devDDC->drvrHwCfg->ocr),
                             resp,
                             LLC_MMCSD_EVENT_EOFCMD,
                             LLC_MMCSD_EVENT_ERROR,
                             &status);
                if (IOM_COMPLETED != result)
                {
                    mmcsdRestoreInt(devDDC, mmcIntMode);
                    break;
                }

                if ((resp[7] & 0x8000U) != 0)
                {
                    /* SD card powered up */
                    devDDC->cardRegs.sd.ocr = \
                        (Uint32)(((Uint32)resp[7] << 16) | resp[6]);
                    devDDC->isHighCapacitySDCard = FALSE;
                    break;
                }
                /*since busy bit is 0 give some time to card for initialisation*/
                /*TSK_sleep(100);*/
                cnt--;
            } /* while (0 != cnt) */
            if (0 == cnt)
            {
                MMCSD_INIT_DEBUG(("\r\n Cnt expired"));
                mmcsdRestoreInt(devDDC, mmcIntMode);
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
            if (IOM_COMPLETED != result)
            {
                break;
            }
        }
        else
        {
            /*Response to command 8 means Ver2.00 or later SD Memory Card*/
            while (0 != cnt)
            {
                cmd.idx         = LLC_MMCSD_APP_CMD;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                cmd.opMode      = devDDC->opMode;

                result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                             &cmd,
                             MMCSD_ARGUMENT_STUFF_BITS,
                             resp,
                             LLC_MMCSD_EVENT_EOFCMD,
                             LLC_MMCSD_EVENT_ERROR,
                             &status);
                if (IOM_COMPLETED != result)
                {
                    mmcsdRestoreInt(devDDC, mmcIntMode);
                    break;
                }
                /*send ocr*/
                cmd.idx         = LLC_MMCSD_ACMD_SD_SEND_OP_COND;
                cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
                cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
                cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                cmd.opMode      = devDDC->opMode;
                /* send ACMD 41 with HCS = 1*/
                result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                             &cmd,
                             (0x40000000U | devDDC->drvrHwCfg->ocr),
                             resp,
                             LLC_MMCSD_EVENT_EOFCMD,
                             LLC_MMCSD_EVENT_ERROR,
                             &status);
                if (IOM_COMPLETED != result)
                {
                    mmcsdRestoreInt(devDDC, mmcIntMode);
                    break;
                }
                /*check busy bit in ocr to find init complete or not*/
                if ((resp[7] & 0x8000U) != 0)
                {
                    /* SD card powered up */
                    devDDC->cardRegs.sd.ocr = \
                        (Uint32)(((Uint32)resp[7] << 16) | resp[6]);
                    /*Check card is high capacity or standard capacity card*/
                    if ((resp[7] & 0x4000U) != 0)
                    {
                        devDDC->isHighCapacitySDCard = TRUE;
                        MMCSD_INIT_DEBUG(("\r\n MMCSD: High capacity SD card"));
                    }
                    else
                    {
                        devDDC->isHighCapacitySDCard = FALSE;
                    }
                    break;
                }
                /*since busy bit is 0 give some time to card for initialisation*/
                /*TSK_sleep(100);*/
                cnt--;
            } /* while (0 != cnt) */

            if (0 == cnt)
            {
                MMCSD_INIT_DEBUG(("\r\n Cnt expired"));
                mmcsdRestoreInt(devDDC, mmcIntMode);
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
            if (IOM_COMPLETED != result)
            {
                break;
            }
        }
    } while(flag);
    return result;
}

/**\brief Send card identification register*/
static Int32 mmcsdSendCID(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode)
{
    LLC_MmcsdCmd cmd;
    Uint32 i;
    Int32 result = IOM_COMPLETED;
    Uint16 resp[MAX_RESP_SIZE_BYTES];

    assert(NULL != devDDC);

    /* send CID*/
    cmd.idx         = LLC_MMCSD_ALL_SEND_CID;
    cmd.respType    = LLC_MMCSD_RESPTYPE_R2;
    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
    cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
    cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
    cmd.opMode      = devDDC->opMode;

    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                 &cmd,
                 MMCSD_ARGUMENT_STUFF_BITS,
                 resp,
                 LLC_MMCSD_EVENT_EOFCMD,
                 LLC_MMCSD_EVENT_ERROR,
                 NULL);
    if (IOM_COMPLETED != result)
    {
        mmcsdRestoreInt(devDDC, mmcIntMode);
    }
    else
    {
        for (i = 0; i < 8U; i++)
        {
            devDDC->cardRegs.sd.cid[2U * i] = resp[i] & 0xFF;
            devDDC->cardRegs.sd.cid[(2U * i) + 1U] = ((resp[i] & 0xFF00) >> 8);
        }
    }
    return result;

}

/**\brief Send Card Specific Data*/
static Int32 mmcsdSendCSD(DDC_MmcsdDeviceHandle devDDC, Uint16 mmcIntMode)
{
    LLC_MmcsdCmd cmd;
    Uint32 i;
    Int32 result = IOM_COMPLETED;
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    Uint32 rca;
    Uint16 status;

    assert(NULL != devDDC);

    /*send CSD*/
    cmd.idx         = LLC_MMCSD_SEND_CSD;
    cmd.respType    = LLC_MMCSD_RESPTYPE_R2;
    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
    cmd.type        = LLC_MMCSD_CMDTYPE_AC;
    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
    cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
    cmd.opMode      = devDDC->opMode;
    rca = devDDC->cardRegs.sd.rca;
    rca <<= RCA_SHIFT_MASK;

    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                 &cmd,
                 rca,
                 resp,
                 LLC_MMCSD_EVENT_EOFCMD,
                 LLC_MMCSD_EVENT_ERROR,
                 &status);
    if (IOM_COMPLETED != result)
    {
        mmcsdRestoreInt(devDDC, mmcIntMode);
    }
    else
    {
        for (i = 0; i < 8U; i++)
        {
            devDDC->cardRegs.sd.csd[2U * i] = resp[i] & 0xFF;
            devDDC->cardRegs.sd.csd[(2U * i) + 1U] = ((resp[i] & 0xFF00) >> 8);
        }
    }
    return result;
}

/**\brief Disconnect the register on SD card befor transfer*/
static Int32 mmcsdDisconnectReg(DDC_MmcsdDeviceHandle devDDC,
                                Uint16 mmcIntMode)
{
    LLC_MmcsdCmd cmd;
    Int32 result = IOM_COMPLETED;
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    Uint32 rca;

    assert(NULL != devDDC);

    cmd.idx         = LLC_MMCSD_APP_CMD;
    cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
    cmd.type        = LLC_MMCSD_CMDTYPE_AC;
    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
    cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
    cmd.opMode      = devDDC->opMode;
    rca = devDDC->cardRegs.sd.rca;
    rca <<= RCA_SHIFT_MASK;

    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                 &cmd,
                 rca,
                 resp,
                 LLC_MMCSD_EVENT_EOFCMD,
                 LLC_MMCSD_EVENT_ERROR,
                 NULL);
    if (IOM_COMPLETED != result)
    {
        mmcsdRestoreInt(devDDC, mmcIntMode);
    }
    else
    {
        /* ACMD 42 Required to disconnect 50Kohm registor in the case of SD card
        before data transfer*/
        cmd.idx         = LLC_MMCSD_ACMD_SET_CLR_CARD_DETECT;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     MMCSD_ARGUMENT_STUFF_BITS,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
        }
    }
    return result;
}

/**\brief get the capacity of the card*/
static Uint32 mmcsdGetCapacity(DDC_MmcsdDeviceHandle devDDC,
                               PSP_MmcsdCardType card)
{
    Uint32 cSizeMult, cSize, temp1, temp2, temp3, mult, blocknr, blocklen;
    Uint32 cardCapacity = 0U;

    assert(NULL != devDDC);

    if (PSP_MMCSD_CARDTYPE_SD == card)
    {
        /*Calculate card capacity*/
        if ((devDDC->cardRegs.sd.csd[15] & 0xC0U) != 0)
        {
            /*CSD structure 2.0*/
            temp1 = (devDDC->cardRegs.sd.csd[6]);
            temp2 = (devDDC->cardRegs.sd.csd[7]);
            temp3 = (devDDC->cardRegs.sd.csd[8] & 0x3FU);
            cSize = (temp3<<16) | (temp2 << 8) | temp1;
            cardCapacity = (cSize + 1U) * 512U;
        }
        else
        {
            /*CSD structure 1.0*/
            temp1 = (devDDC->cardRegs.sd.csd[5] & 0x80U) >> 7;
            temp2 = (devDDC->cardRegs.sd.csd[6] & 0x3U);
            cSizeMult = (temp2 << 1) | temp1;
            mult = 1 << (cSizeMult + 2U);
            temp1 = (devDDC->cardRegs.sd.csd[7] & 0xC0U) >> 6;
            temp2 = (devDDC->cardRegs.sd.csd[8] & 0xFFU);
            temp3 = (devDDC->cardRegs.sd.csd[9] & 0x3U);
            cSize = (temp3 << 10) | (temp2 << 2) | temp1;
            blocknr = (cSize + 1U) * mult;
            blocklen=(1 << (devDDC->cardRegs.sd.csd[10] & 0xFU));
            cardCapacity = blocknr * blocklen;
        }
    }
    else if (PSP_MMCSD_CARDTYPE_MMC == card)/*Card is mmc card*/
    {
        /*Calculate card capacity*/
        temp1 = (devDDC->cardRegs.mmc.csd[5] & 0x80) >> 7;
        temp2 = (devDDC->cardRegs.mmc.csd[6] & 0x3);
        cSizeMult = (temp2 << 1) | temp1;
        mult = 1 << (cSizeMult + 2U);
        temp1 = (devDDC->cardRegs.mmc.csd[7] & 0xC0)>>6;
        temp2 = (devDDC->cardRegs.mmc.csd[8] & 0xFF);
        temp3 = (devDDC->cardRegs.mmc.csd[9] & 0x3);
        cSize = (temp3 << 10) | (temp2 << 2) | temp1;
        blocknr = (cSize + 1U) * mult;
        blocklen=(1 << (devDDC->cardRegs.mmc.csd[10] & 0xF));
        cardCapacity = blocknr * blocklen;
    }
    else/*print error*/
    {
        MMCSD_DRV_DEBUG(("\r\n Invalid card type"));
    }

    return cardCapacity;
}


/**\brief Initialise the SD card*/
static Int32 mmcsdInitSdCard(DDC_MmcsdDeviceHandle devDDC)
{
    LLC_MmcsdCmd cmd;
    Uint16 status,i;
    Uint16 resp[MAX_RESP_SIZE_BYTES];
    Uint16 mmcIntMode;
    Bool isSCRReadPass = FALSE;
    Int32 result = IOM_COMPLETED;
    Uint32 cardCapacity, rca;
    Bool flag = TRUE;

    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        devDDC->cardRegs.sd.ocr = 0;
        for (i = 0; i < MMCSD_CID_SIZE_BYTE; i++)
        {
            devDDC->cardRegs.sd.cid[i] = 0;
        }
        for (i = 0; i < MMCSD_CSD_SIZE_BYTE; i++)
        {
            devDDC->cardRegs.sd.csd[i] = 0;
        }
        devDDC->cardRegs.sd.rca = 0;
        devDDC->cardRegs.sd.dsr = 0;
        for (i = 0; i < MMCSD_SCR_SIZE_BYTE; i++)
        {
            devDDC->cardRegs.sd.scr[i] = 0;
        }

        /* The card is present & is of type SD. Initialize it now*/
        MMCSD_INIT_DEBUG(("\r\n mmcsdInitSdCard() start "));

        /* work in polled mode, so no mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, &mmcIntMode);

        /* turn off DMA mode if enabled during initialization */
        LLC_mmcsdSetFrequency(devDDC->hLlc,
            &(devDDC->hDrvrClock.openDrain),
            devDDC->hDrvrClock.isOpenDrainDivBy4Reqd);

        LLC_mmcsdSendInitSeq(devDDC->hLlc);
        status = 0;
        while (0 == (status & (LLC_MMCSD_EVENT_EOFCMD | LLC_MMCSD_EVENT_ERROR)))
        {
            LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
        }

        if ((0 == (status & LLC_MMCSD_EVENT_EOFCMD)) ||
            ((LLC_MMCSD_EVENT_ERROR & status) != 0))
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }

        /*GO_IDLE_STATE*/
        cmd.idx         = LLC_MMCSD_GO_IDLE_STATE;
        cmd.respType    = LLC_MMCSD_RESPTYPE_NO;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_BC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     MMCSD_ARGUMENT_STUFF_BITS,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        /*Send interface information*/
        result = mmcsdSendInterfaceInfo(devDDC, mmcIntMode);
        if (IOM_COMPLETED != result)
        {
            break;
        }

        /* send CID*/
        result = mmcsdSendCID(devDDC, mmcIntMode);
        if (IOM_COMPLETED != result)
        {
            break;
        }

        /*send RCA*/
        cmd.idx         = LLC_MMCSD_SEND_RELATIVE_ADDR;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R6;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     MMCSD_ARGUMENT_STUFF_BITS,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        devDDC->cardRegs.sd.rca = resp[7];

        /*send CSD*/
        result = mmcsdSendCSD(devDDC, mmcIntMode);
        if (IOM_COMPLETED != result)
        {
            break;
        }

        LLC_mmcsdSetFrequency(devDDC->hLlc,
            &(devDDC->hDrvrClock.sdPushPull),
            FALSE);


        if ((devDDC->cardRegs.sd.csd[1] & PERMANENT_WRITE_PROTECT_MASK) != 0)
        {
            /*Card is permanet write protected*/
            devDDC->permanentWP = TRUE;
        }
        else
        {
            devDDC->permanentWP = FALSE;
        }

        if ((devDDC->cardRegs.sd.csd[1] & TEMPERORY_WRITE_PROTECT_MASK) != 0)
        {
            /*Card is temp write protected*/
            devDDC->tempWP = TRUE;
        }
        else
        {
            devDDC->tempWP = FALSE;
        }

        /* the default value, this is never updated */
        devDDC->cardRegs.sd.dsr = DEFAULT_DSR_VALUE;
        /*select card and put in transfer state*/
        cmd.idx         = LLC_MMCSD_SELECT_DESELECT_CARD;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;
        rca = devDDC->cardRegs.sd.rca;
        rca <<= RCA_SHIFT_MASK;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD)))
        {
            MMCSD_INIT_DEBUG(("\r\n mmcsdInitSdCard : GetStatusFromCard() failed"));
            mmcsdRestoreInt(devDDC, mmcIntMode);
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }

        /* ACMD 42 Required to disconnect 50Kohm registor in the case of SD card
        before data transfer*/
        result = mmcsdDisconnectReg(devDDC, mmcIntMode);
        if (IOM_COMPLETED != result)
        {
            break;
        }

        /*Read the SCR register*/
        result = mmcsdReadSCR(devDDC, resp);
        if (result != IOM_COMPLETED)
        {
            isSCRReadPass = FALSE;
            MMCSD_INIT_DEBUG(("\r\n MMCSD : SCR read regiter failure"));
        }
        else
        {
            isSCRReadPass = TRUE;
        }

        TSK_sleep(1U);

        if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD)))
        {
            MMCSD_INIT_DEBUG(("\r\n mmcsdInitSdCard : GetStatusFromCard() failed"));
            mmcsdRestoreInt(devDDC, mmcIntMode);
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }

#ifdef SD_STATUS_READ
        /*Read the SD Status register*/
        result = mmcsdReadSdStatus(devDDC, resp);
        if (result != IOM_COMPLETED)
        {
            MMCSD_INIT_DEBUG(("\r\n MMCSD : SD status read register failure"));
        }
        else
        {
            MMCSD_INIT_DEBUG(("\r\n SD status read register success"));
        }

        TSK_sleep(1U);

        if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_SD)))
        {
            MMCSD_INIT_DEBUG(("\r\n mmcsdInitSdCard : GetStatusFromCard() failed"));
            mmcsdRestoreInt(devDDC, mmcIntMode);
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }
#endif /*SD_STATUS_READ*/

        /* Support for high speed card. Check whether card supports High speed
         * functionality or not and accordingly switch the frequency
         * The host shall check the "SD_SPEC" field in the SCR register to identify
         * what version of the spec the card complies with before using CMD6. It is
         * mandatory for an SD memory card of Version 1.10 and higher to support
         * CMD6. */
        if ((((devDDC->cardRegs.sd.scr[0] & 0x01) != 0) &&
             (TRUE == isSCRReadPass)) ||
            (((devDDC->cardRegs.sd.scr[0] & 0x02) != 0) && (TRUE == isSCRReadPass)))
        {
            mmcsdCheckHighSpeedCard(devDDC, resp);
        }

        /* It is observed that there is small delay required for some of card
        otherwise ACMD6 has failed*/
        /*TSK_sleep(100);*/

        /*SCR_READ*/
        /*check whether card supports 4 bit mode or not*/
        if ((TRUE == isSCRReadPass) && ((devDDC->cardRegs.sd.scr[1] & 0x01) != 0))
        {
            /* Required to configure SD card in 4 bit mode of operation*/
            cmd.idx         = LLC_MMCSD_APP_CMD;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;
            rca = devDDC->cardRegs.sd.rca;
            rca <<= RCA_SHIFT_MASK;

            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         rca,
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         NULL);
            if (IOM_COMPLETED != result)
            {
                MMCSD_INIT_DEBUG(("\r\n MMCSD : Failed at CMD55 issued b4 ACMD6 to"
                                  " switch to 4bit mode"));
                mmcsdRestoreInt(devDDC, mmcIntMode);
                break;
            }
            /* Required to configure SD card in 4 bit mode of operation*/
            /*Set bus width*/
            cmd.idx         = LLC_MMCSD_ACMD_SET_BUS_WIDTH;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;
            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         0x2U,
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         &status);
            if (IOM_COMPLETED != result)
            {
                MMCSD_INIT_DEBUG(("\r\n MMCSD : Failed at ACMD6 to switch to 4bit"
                                  " mode"));
            }
            else
            {
                LLC_mmcsdSetBusWidth(devDDC->hLlc, TRUE);
            }
        }
        else
        {
            MMCSD_INIT_DEBUG(("\r\n MMCSD : This card does not support 4 bit mode"
                              " and so no switching to 4 bit mode"));
        }

        cardCapacity = mmcsdGetCapacity(devDDC, PSP_MMCSD_CARDTYPE_SD);

        /*Set the block size to 512 bytes*/
        devDDC->blkSize = MMCSD_DEFAULT_BLOCK_LENGTH;

        if (TRUE == devDDC->isHighCapacitySDCard)
        {
            /*For high speed card card capacity informed in KByte*/
            devDDC->sectorCnt = cardCapacity * 2U;
        }
        else
        {
            devDDC->sectorCnt = cardCapacity / devDDC->blkSize;
        }

        /* Added to Support for 2GB SD Card*/
        /*SET_BLOCKLEN*/
        cmd.idx         = LLC_MMCSD_SET_BLOCKLEN;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     (Uint32)devDDC->blkSize,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;

        /*Clear the status register before enabling the dma interrupt restore DMA
          mode after initialization. Clear the status register before enabling the
          interrupt to avoid dummy interrupt*/
        LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
        /* restore mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
        {
            mmcsdConfigDmaMode(devDDC);
        }
    } while(flag);
    return result;
}

/**\brief initialise MMC Card*/
static Int32 mmcsdInitMmcCard(DDC_MmcsdDeviceHandle devDDC)
{
    LLC_MmcsdCmd        cmd;
    Uint16              status;
    Uint32              i;
    Uint16              resp[MAX_RESP_SIZE_BYTES];
    Uint16              mmcIntMode;
    Uint32              cnt = DDC_MMCSD_NUM_OF_TRIES;
    Int32               result = IOM_COMPLETED;
    Uint32              cardCapacity, rca;
    Bool                flag = TRUE;
    assert(NULL != devDDC);

    do
    {
        flag = FALSE;
        devDDC->cardRegs.mmc.ocr = 0;
        for (i = 0; i < MMCSD_CID_SIZE_BYTE; i++)
        {
            devDDC->cardRegs.mmc.cid[i] = 0;
        }
        for (i = 0; i < MMCSD_CSD_SIZE_BYTE; i++)
        {
            devDDC->cardRegs.mmc.csd[i] = 0;
        }
        devDDC->cardRegs.mmc.rca = 0;
        devDDC->cardRegs.mmc.dsr = 0;

        MMCSD_INIT_DEBUG(("\r\n mmcsdInitMmcCard() start "));

        /* work in polled mode, so no mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, &mmcIntMode);

        /* turn off DMA mode if enabled during initialization */
        LLC_mmcsdSetFrequency(devDDC->hLlc,
            &(devDDC->hDrvrClock.openDrain),
            devDDC->hDrvrClock.isOpenDrainDivBy4Reqd);

        if (0 == devDDC->cardRegs.mmc.ocr)
        {
            /* send ocr*/
            cmd.idx         = LLC_MMCSD_MMC_SEND_OP_COND;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
            cmd.opMode      = devDDC->opMode;

            while (0 != cnt)
            {
                result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                             &cmd,
                             (0x80000000 | devDDC->drvrHwCfg->ocr),
                             resp,
                             LLC_MMCSD_EVENT_EOFCMD,
                             LLC_MMCSD_EVENT_ERROR,
                             &status);
                if (IOM_COMPLETED != result)
                {
                    mmcsdRestoreInt(devDDC, mmcIntMode);
                    break;
                }

                if ((resp[7] & 0x8000) != 0)
                {
                    devDDC->cardRegs.mmc.ocr = \
                        (Uint32)(((Uint32)resp[7] << 16) | resp[6]);
                    break;
                }
                cnt--;
            }

            if (0 == cnt)
            {
                MMCSD_INIT_DEBUG(("\r\n Cnt expired"));
                mmcsdRestoreInt(devDDC, mmcIntMode);
                result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                break;
            }
            if (IOM_COMPLETED != result)
            {
                break;
            }
        }

        /*send CID*/
        cmd.idx         = LLC_MMCSD_ALL_SEND_CID;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R2;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     MMCSD_ARGUMENT_STUFF_BITS,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        for (i = 0; i < 8U; i++)
        {
            devDDC->cardRegs.mmc.cid[2U * i] = resp[i] & 0xFF;
            devDDC->cardRegs.mmc.cid[(2U * i) + 1U] = ((resp[i] & 0xFF00) >> 8);
        }
        /*set RCA*/
        cmd.idx         = LLC_MMCSD_MMC_SET_RELATIVE_ADDR;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     (DDC_MMC_CARD_RCA << RCA_SHIFT_MASK),
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     &status);

        devDDC->cardRegs.mmc.rca = DDC_MMC_CARD_RCA;
        /*send CSD*/
        cmd.idx         = LLC_MMCSD_SEND_CSD;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R2;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
        cmd.opMode      = devDDC->opMode;
        rca = devDDC->cardRegs.mmc.rca;
        rca <<= RCA_SHIFT_MASK;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        for (i = 0; i < 8U; i++)
        {
            devDDC->cardRegs.mmc.csd[2U * i] = resp[i] & 0xFF;
            devDDC->cardRegs.mmc.csd[(2U * i) + 1U] = ((resp[i] & 0xFF00) >> 8);
        }

        LLC_mmcsdSetFrequency(devDDC->hLlc,
            &(devDDC->hDrvrClock.mmcPushPull),
            FALSE);

        if ((devDDC->cardRegs.mmc.csd[1] & PERMANENT_WRITE_PROTECT_MASK) != 0)
        {
            /*Card is permanet write protected*/
            devDDC->permanentWP = TRUE;
        }
        else
        {
            devDDC->permanentWP = FALSE;
        }

        if ((devDDC->cardRegs.mmc.csd[1] & TEMPERORY_WRITE_PROTECT_MASK) != 0)
        {
            /*Card is temp write protected*/
            devDDC->tempWP = TRUE;
        }
        else
        {
            devDDC->tempWP = FALSE;
        }

        /* the default value, this is never updated */
        devDDC->cardRegs.mmc.dsr = DEFAULT_DSR_VALUE;

        /*select card*/
        cmd.idx         = LLC_MMCSD_SELECT_DESELECT_CARD;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;
        rca = devDDC->cardRegs.mmc.rca;
        rca <<= RCA_SHIFT_MASK;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     rca,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        if (FALSE == (mmcsdGetStatusFromCard(devDDC, resp, PSP_MMCSD_CARDTYPE_MMC)))
        {
            MMCSD_INIT_DEBUG(("\r\n mmcsdInitMmcCard : GetStatusFromCard() fails"));
            mmcsdRestoreInt(devDDC, mmcIntMode);
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
            break;
        }

        /* Set the block length*/
        devDDC->blkSize = MMCSD_DEFAULT_BLOCK_LENGTH;
        cmd.idx         = LLC_MMCSD_SET_BLOCKLEN;
        cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
        cmd.type        = LLC_MMCSD_CMDTYPE_AC;
        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
        cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
        cmd.opMode      = devDDC->opMode;

        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                     &cmd,
                     (Uint32)devDDC->blkSize,
                     resp,
                     LLC_MMCSD_EVENT_EOFCMD,
                     LLC_MMCSD_EVENT_ERROR,
                     NULL);
        if (IOM_COMPLETED != result)
        {
            mmcsdRestoreInt(devDDC, mmcIntMode);
            break;
        }

        cardCapacity = mmcsdGetCapacity(devDDC, PSP_MMCSD_CARDTYPE_MMC);
        devDDC->sectorCnt = cardCapacity / devDDC->blkSize;
        devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;

        /*Clear the status register before enabling the dma interrupt restore DMA
          mode after initialization. Clear the status register before enabling the
          interrupt to avoid dummy interrupt*/
        LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
        /* restore mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
        {
            mmcsdConfigDmaMode(devDDC);
        }
    } while(flag);
    return result;
}

/**\brief Check whether CARD is present or not*/
static Int32 mmcsdCheckCard(DDC_MmcsdDeviceHandle devDDC)
{
    Int32           result = IOM_COMPLETED;
    Uint16          status;
    Uint16          mmcIntMode;
    LLC_MmcsdCmd    cmd;
    Uint16          resp[MAX_RESP_SIZE_BYTES];
    Int32           i;
    LLC_MmcsdObj*   llc;
    Uint32          rca = 0;
    Bool            flag = TRUE;
    assert(NULL != devDDC);
    llc = (LLC_MmcsdObj*)devDDC->hLlc;
    assert(NULL != llc);

    MMCSD_DRV_DEBUG(("\r\n mmcsdCheckCard() start "));

    do
    {
        flag = FALSE;
        if (TRUE != SEM_pend(devDDC->semAlignDrvrThreads, SYS_FOREVER))
        {
            /* the sem is taken up implies that the card being
            * transacted with. So return the card as being present */
            break;
        }

        if ((devDDC->flags & DDC_MMCSD_FLAG_DEVICE_STOPPED) != 0)
        {
            devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            LLC_mmcsdInitController(llc);
            devDDC->permanentWP = FALSE;
            devDDC->tempWP = FALSE;
        }

        /* restore mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, &mmcIntMode);

        if (PSP_MMCSD_CARDTYPE_NONE != devDDC->cardRegs.cardType)
        {
            /* if card is recorded as being inserted, then send CMD13 to check */
            /*send status*/
            cmd.idx         = LLC_MMCSD_SEND_STATUS;
            cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_AC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
            cmd.opMode      = devDDC->opMode;
            if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
            {
                rca = devDDC->cardRegs.mmc.rca;
                rca <<= RCA_SHIFT_MASK;
            }
            if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
            {
                rca = devDDC->cardRegs.sd.rca;
                rca <<= RCA_SHIFT_MASK;
            }

            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         rca,
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         &status);
            if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
            {
                /* card exists & is initialized */
                result = IOM_COMPLETED;
            }
            else if (((status & LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT) != 0)||
                    ((status & LLC_MMCSD_EVENT_ERROR_DATATIMEOUT) != 0))
            {
                /* existing card is removed */
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
                devDDC->flags |= DDC_MMCSD_FLAG_DEVICE_STOPPED;
                result = IOM_COMPLETED;
            }
            else
            {
                /*Clear the status register before enabling the interrupt to
                avoid dummy interrupt*/
                LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
                /* restore mmcsd level interrupts */
                LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
                SEM_post(devDDC->semAlignDrvrThreads);
                break;
            }
        }
        else
        {
            LLC_mmcsdSetFrequency(devDDC->hLlc,
                &(devDDC->hDrvrClock.openDrain),
                devDDC->hDrvrClock.isOpenDrainDivBy4Reqd);

            LLC_mmcsdSendInitSeq(devDDC->hLlc);

            status = 0;
            while (0 == (status & (LLC_MMCSD_EVENT_EOFCMD | LLC_MMCSD_EVENT_ERROR)))
            {
                LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
            }

            devDDC->cardRegs.mmc.ocr = 0;
            devDDC->cardRegs.sd.ocr = 0;
            devDDC->isHighCapacitySDCard = FALSE;
            devDDC->isHighSpeedSDCard = FALSE;
            /*go idle state*/
            cmd.idx         = LLC_MMCSD_GO_IDLE_STATE;
            cmd.respType    = LLC_MMCSD_RESPTYPE_NO;
            cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
            cmd.type        = LLC_MMCSD_CMDTYPE_BC;
            cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
            cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
            cmd.opMode      = devDDC->opMode;

            result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                         &cmd,
                         MMCSD_ARGUMENT_STUFF_BITS,
                         resp,
                         LLC_MMCSD_EVENT_EOFCMD,
                         LLC_MMCSD_EVENT_ERROR,
                         &status);
            if (IOM_COMPLETED != result)
            {
                /*Clear the status register before enabling the interrupt to
                avoid dummy interrupt*/
                LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
                /* restore mmcsd level interrupts */
                LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
                SEM_post(devDDC->semAlignDrvrThreads);
                break;
            }

            for (i = 0; i < 3; i++)
            {
                if (0 == i)
                {
                    /*send interface condition. only SD card respond*/
                    cmd.idx         = LLC_MMCSD_SEND_IF_COND;
                    cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
                    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                    cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
                    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                    cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                    cmd.opMode      = PSP_MMCSD_OPMODE_POLLED;

                    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                                 &cmd,
                                 (0x100U | SD_HIGH_CAPACITY_ECHO_PATTERN),
                                 resp,
                                 LLC_MMCSD_EVENT_EOFCMD,
                                 LLC_MMCSD_EVENT_ERROR,
                                 &status);
                    MMCSD_DRV_DEBUG(("\r\n cmd 8 sent result = %l status"
                                     " = %l", result, status));
                }
                else if (1 == i)
                {
                    /*ACMD only sd card respond*/
                    cmd.idx         = LLC_MMCSD_APP_CMD;
                    cmd.respType    = LLC_MMCSD_RESPTYPE_R1;
                    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                    cmd.type        = LLC_MMCSD_CMDTYPE_AC;
                    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                    cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                    cmd.opMode      = devDDC->opMode;

                    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                                 &cmd,
                                 0x0U,/*default rca*/
                                 resp,
                                 LLC_MMCSD_EVENT_EOFCMD,
                                 LLC_MMCSD_EVENT_ERROR,
                                 &status);
                    MMCSD_DRV_DEBUG(("\r\n cmd 55 sent result = %l status"
                                     " = %l", result, status));
                }
                else/*i==2*/
                {
                    /*send ocr. only mmc card respond*/
                    cmd.idx         = LLC_MMCSD_MMC_SEND_OP_COND;
                    cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
                    cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                    cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
                    cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                    cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                    cmd.opMode      = devDDC->opMode;

                    result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                                 &cmd,
                                 (0x80000000 | devDDC->drvrHwCfg->ocr),
                                 resp,
                                 LLC_MMCSD_EVENT_EOFCMD,
                                 LLC_MMCSD_EVENT_ERROR,
                                 &status);
                    MMCSD_DRV_DEBUG(("\r\n cmd 1 sent result = %l status"
                                     " = %l", result, status));
                }

                if ((status & LLC_MMCSD_EVENT_EOFCMD) != 0)
                {
                    MMCSD_DRV_DEBUG(("\r\n status is LLC_MMCSD_EVENT_EOFCMD"));
                    if (1 == i)
                    {
                        /* sd card found */
                        cmd.idx         = LLC_MMCSD_ACMD_SD_SEND_OP_COND;
                        cmd.respType    = LLC_MMCSD_RESPTYPE_R3;
                        cmd.busy        = LLC_MMCSD_RESPBUSY_NO;
                        cmd.type        = LLC_MMCSD_CMDTYPE_BCR;
                        cmd.dir         = LLC_MMCSD_DIR_DONTCARE;
                        cmd.lineChr     = LLC_MMCSD_LINECHR_OPENDRAIN;
                        cmd.opMode      = devDDC->opMode;

                        result = mmcsdSendCmdPolledMode(devDDC->hLlc,
                                     &cmd,
                                     (0x80000000 | devDDC->drvrHwCfg->ocr),
                                     resp,
                                     LLC_MMCSD_EVENT_EOFCMD,
                                     LLC_MMCSD_EVENT_ERROR,
                                     &status);
                        MMCSD_DRV_DEBUG(("\r\n cmd 41 sent result = %l status"
                                         " = %l", result, status));
                        if (IOM_COMPLETED != result)
                        {
                            break;
                        }

                        if (((Uint32)(((Uint32)resp[7] << 16) | resp[6]) &
                            (devDDC->drvrHwCfg->ocr)) != (devDDC->drvrHwCfg->ocr))
                        {
                            result = PSP_MMCSD_RESULT_ERROR_VOLTAGE_MISMATCH;
                            MMCSD_DRV_DEBUG(("\r\n Err ERROR VOLTAGE MISMATCH"));
                        }
                        else
                        {
                            devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_SD;
                            result = IOM_COMPLETED;
                            MMCSD_DRV_DEBUG(("\r\n SD Card found"));
                        }
                    }
                    else if (2 == i)
                    {
                        /* mmc card found */
                        if (((Uint32)(((Uint32)resp[7] << 16) | resp[6]) &
                            (devDDC->drvrHwCfg->ocr)) != (devDDC->drvrHwCfg->ocr))
                        {
                            result = PSP_MMCSD_RESULT_ERROR_VOLTAGE_MISMATCH;
                            MMCSD_DRV_DEBUG(("\r\n VOLTAGE MISMATCH with i = 2"));
                        }
                        else
                        {
                            devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_MMC;
                            if ((resp[7] & 0x8000) != 0)
                            {
                                devDDC->cardRegs.mmc.ocr = \
                                    (Uint32)(((Uint32)resp[7] << 16) | resp[6]);
                            }
                            result = IOM_COMPLETED;
                            MMCSD_DRV_DEBUG(("\r\n MMC card found"));
                        }
                    }
                    else
                    {/*i==0*/
                        /*sd High capacity found*/
                        if (SD_HIGH_CAPACITY_ECHO_PATTERN == (resp[6] & 0xFF))
                        {
                            devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_SD;
                            result = IOM_COMPLETED;
                            MMCSD_DRV_DEBUG(("\r\n sd card found"));
                        }
                        else
                        {
                            result = PSP_MMCSD_RESULT_ERROR_VOLTAGE_MISMATCH;
                            MMCSD_DRV_DEBUG(("\r\n VOLTAGE MISMATCH with i = 0"));
                        }
                    }
                    break;
                }
                else if ((status & LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT) != 0)
                {
                    if ((0 == i) || (1 == i))
                    {
                        /* SD and High capacity SD card not present; so check for
                        MMC */
                        MMCSD_DRV_DEBUG(("\r\n continue with i = 1 and 0"));
                    }
                    else
                    {/*i==2*/
                        /* neither SD nor MMC is present */
                        MMCSD_DRV_DEBUG(("\r\n error with i=2"));
                    }
                }
                else
                {
                    /* neither timeout nor successful end of command; so error */
                    result = PSP_MMCSD_RESULT_ERROR_FAILURE;
                    MMCSD_DRV_DEBUG(("\r\n fail"));
                }
            }
        }

        /*Clear the status register before enabling the interrupt to avoid dummy
        interrupt*/
        LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);
        /* restore mmcsd level interrupts */
        LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntMode, NULL);
        SEM_post(devDDC->semAlignDrvrThreads);
    } while(flag);
    return result;
}

/**\brief send command in polled mode. retStatus can be NULL if status is not
 required*/
static Int32 mmcsdSendCmdPolledMode(LLC_MmcsdHandle         hLlc,
                                    LLC_MmcsdCmd* const     cmd,
                                    Uint32                  arg,
                                    Uint16*                 resp,
                                    Uint16                  reqdStatus,
                                    Uint16                  errStatus,
                                    Uint16*                 retStatus)
{
    Uint16      lStatus;
    Uint32      cnt = MMCSD_TIME_OUT_COUNT;
    Uint16      gatingStatus = (errStatus | reqdStatus);
    Int32       result = IOM_COMPLETED;

    assert(((NULL != hLlc) && (NULL != cmd)) && (NULL != resp));

    LLC_mmcsdSendCmd(hLlc, cmd, arg);

    lStatus = 0;
    while ((0 == (lStatus & gatingStatus)) && (cnt !=0))
    {
        LLC_mmcsdGetEventStatus(hLlc, &lStatus);
        cnt--;
    }

    if (retStatus != NULL)
    {
        *retStatus = lStatus;
    }

    if ((((lStatus & errStatus) != 0) ||
        (0 == (lStatus & reqdStatus))) ||
        (0 == cnt))
    {
        result = PSP_MMCSD_RESULT_ERROR_FAILURE;
    }
    else
    {
        result = mmcsdCheckResponse(hLlc, cmd->respType, resp);
        if (IOM_COMPLETED != result)
        {
            result = PSP_MMCSD_RESULT_ERROR_FAILURE;
        }
    }

    return result;
}

/**
 * \brief Submit an Asynchronous IO request to the Communication Stream
 *
 * This function is used to submit an asynchronous IO request to the driver.
 * This function returns immediately after enqueuing the request on the devDDC.
 * The IO request packet will be serviced in a peripheral specific manner by
 * the driver, with separate TX and RX queues. As packets are filled or
 * emptied, they shall be placed on a completion list, unless the DDA has
 * programmed the driver to call the completion callback with the completion
 * of every IO request (through a call to the ioctl
 * DDC_MMCSD_IOCTL_SET_CALLBACK). The Async notification strategy can either
 * be autonomous or polled. If the callback for notfying IoComplete has been
 * configured, the async notification strategy is assumed to be autonomous,
 * else, it is assumed to be polled. When the strategy is polled, all IO
 * that gets completed is queued up internally and is supplied to the calling
 * context when a call to DDC_StreamGetIoCompletionList is issued.
 *
 * \param   devDDC           devDDC Handle
 * \param   iop              Handle to the IO request packet.
 *
 * \return IOM_COMPLETED or Error code
 */
static Int32 mmcsdSubmitIoRequestSingle(DDC_MmcsdDeviceHandle devDDC,
                                        DDC_MmcsdIoPacket* const iop)
{
    Uint32 intState;
    Bool isListEmpty;
    Uint32 address;
    Uint32 sectorNr;
    Int32 status = IOM_PENDING;
    Bool flag = TRUE;
    Int32 pwrResult;

    assert((NULL != devDDC) && (NULL != iop));

    do
    {
        flag = FALSE;
        address = iop->blockAddress / devDDC->blkSize;
        sectorNr = iop->xferRequest / devDDC->blkSize;

        if ((devDDC->flags & (DDC_MMCSD_FLAG_SYNCPENDING |
                         DDC_MMCSD_FLAG_CANCELPENDING |
                         DDC_MMCSD_FLAG_DEVICE_STOPPED)) != 0)
        {
            status = IOM_EBADMODE; /*ERROR to submit now!*/
            break;
        }

        if (DDC_PWRM_SUSPEND == devDDC->state)
        {
            status = IOM_EBADMODE;
            break;
        }

        if (((devDDC->sectorCnt < ((address+sectorNr) - 1U)) || (0 == sectorNr)) ||
           (NULL == iop->buf))
        {
            status = IOM_EBADARGS;
            break;
        }

        if ((DDC_MMCSD_IOCMD_WRITE == iop->cmd) &&
           ((TRUE == devDDC->permanentWP) || (TRUE == devDDC->tempWP)))
        {
            status = IOM_EBADMODE; /*ERROR to submit now!*/
            break;
        }

        intState = HWI_disable();
        isListEmpty = QUE_empty(devDDC->listHeadNode);
        /*submit the packet to a pending queue*/

        QUE_put(devDDC->listHeadNode, iop);

        status = iop->result = IOM_PENDING;

        if (FALSE == isListEmpty)
        {
            HWI_restore(intState);
            break;
        }

        HWI_restore(intState);
        SEM_pend(devDDC->semAlignDrvrThreads, SYS_FOREVER);

        /* If the driver is power aware enable the PSC */
        if (TRUE == iop->hDDC->oLlc.isPwrAware)
        {
            /* Enable LPSC as registers are touched */
            pwrResult = mmcsdLpscOn(iop->hDDC);
            if (IOM_COMPLETED != pwrResult)
            {
                status = pwrResult;
                break;
            }
        }

        if (devDDC->devState != DDC_MMCSD_DEVICESTATE_START)
        {
            (devDDC->fsm[devDDC->devState])(devDDC, 0);
            status = iop->result;
        }
    } while(flag);
    return status;
}

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
/**\brief MMCSD Task for instance 0*/
static Void mmcsdTask0(Void)
{
    Uint16 status;
    DDC_MmcsdDeviceHandle devDDC = &(gMmcsdDdcObj[MMCSD_INST_ID0].ddcDevObj);
    Bool taskFlag = FALSE;

    assert((NULL != devDDC) && (NULL != devDDC->hLlc));/*Invalid parameters*/
    taskFlag = TRUE;
    while (taskFlag)
    {
        taskFlag = TRUE;
        /* take sync sema which will be released only by mmcsd isr*/
        if (TRUE != SEM_pend(devDDC->syncSem, SYS_FOREVER))
        {
            MMCSD_ERR_DEBUG(("\r\nMMCSD devDDC->syncSem Take failed \r\n"));
        }

        if (devDDC->taskRun == FALSE)
        {
            break;
        }

        LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);

        if (((0 == status) ||/*No error*/
            ((devDDC->flags &
             (DDC_MMCSD_FLAG_DEVICE_STOPPED)) != 0)) ||/*Device stopped*/
            ((QUE_empty(devDDC->listHeadNode)) != 0))/*List empty*/
        {
#ifdef MMCSD_DEBUG
            printf("\n\rNothing happened");
#endif
        }
        else
        {
            /* Timeout happened*/
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                          LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
#ifdef MMCSD_DEBUG
                printf("\n\rMMCSD-Timeout error and status is %l\r\n",status);
#endif

                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    LLC_mmcsdSetBusWidth(devDDC->hLlc, FALSE);
                }

                devDDC->flags |= (DDC_MMCSD_FLAG_DEVICE_STOPPED |
                               DDC_MMCSD_FLAG_CANCELPENDING);

                devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            }
            else
            {
                /* check for CRC errors */
                /*Don't check for CRC error in write_on state some of card
                (SanDisk,Lexar,Nexon are generating this error because of
                problem with card*/
                if (((status & (LLC_MMCSD_EVENT_ERROR_DATACRC)) != 0) ||
                    ((((status & LLC_MMCSD_EVENT_ERROR_CMDCRC) != 0) &&
                   (((devDDC->devState != DDC_MMCSD_DEVICESTATE_WRITE_STOP) &&
                    (devDDC->devState !=
                     DDC_MMCSD_DEVICESTATE_READ_STOP)) != 0)) != 0))
                {
#ifdef MMCSD_DEBUG
                    if (status & (LLC_MMCSD_EVENT_ERROR_DATACRC))
                    {
                        printf("\n\rMMCSD-Data CRC error. status is %l",status);
                    }
                    else
                    {
                        printf("\n\rMMCSD-CMD CRC error. status is %l",status);
                    }
#endif
                    mmcsdControlCb(devDDC,
                        DDC_MMCSD_HWEVENT_ERROR_STATUS,
                        &status);
                    devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                }
            }

            (devDDC->fsm[devDDC->devState])(devDDC, status);
            if (TRUE == devDDC->isDmaError)
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                (devDDC->fsm[devDDC->devState])(devDDC,status);
                devDDC->isDmaError = FALSE;
            }
            mmcsdCheckFinishIOP(&gMmcsdDdcObj[devDDC->instId]);
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
              LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);
            }
        }
    }
}
#endif


#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
/**\brief MMCSD Task for instance 1*/
static Void mmcsdTask1(Void)
{
    Uint16 status;
    DDC_MmcsdDeviceHandle devDDC = &(gMmcsdDdcObj[MMCSD_INST_ID1].ddcDevObj);
    Bool taskFlag = FALSE;

    assert((NULL != devDDC) && (NULL != devDDC->hLlc));/*Invalid parameters*/
    taskFlag = TRUE;
    while (taskFlag)
    {
        taskFlag = TRUE;
        /* take sync sema which will be released only by mmcsd isr*/
        if (TRUE != SEM_pend(devDDC->syncSem, SYS_FOREVER))
        {
            MMCSD_ERR_DEBUG(("\r\nMMCSD devDDC->syncSem Take failed \r\n"));
        }

        if (devDDC->taskRun == FALSE)
        {
            break;
        }

        LLC_mmcsdGetEventStatus(devDDC->hLlc, &status);

        if (((0 == status) ||/*No error*/
            ((devDDC->flags &
             (DDC_MMCSD_FLAG_DEVICE_STOPPED)) != 0)) ||/*Device stopped*/
            ((QUE_empty(devDDC->listHeadNode)) != 0))/*List empty*/
        {
#ifdef MMCSD_DEBUG
            printf("\n\rNothing happened");
#endif
        }
        else
        {
            /* Timeout happened*/
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
                          LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
#ifdef MMCSD_DEBUG
                printf("\n\rMMCSD-Timeout error and status is %l\r\n",status);
#endif

                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    LLC_mmcsdSetBusWidth(devDDC->hLlc, FALSE);
                }

                devDDC->flags |= (DDC_MMCSD_FLAG_DEVICE_STOPPED |
                               DDC_MMCSD_FLAG_CANCELPENDING);

                devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;
            }
            else
            {
                /* check for CRC errors */
                /*Don't check for CRC error in write_on state some of card
                (SanDisk,Lexar,Nexon are generating this error because of
                problem with card*/
                if (((status & (LLC_MMCSD_EVENT_ERROR_DATACRC)) != 0) ||
                    ((((status & LLC_MMCSD_EVENT_ERROR_CMDCRC) != 0) &&
                   (((devDDC->devState != DDC_MMCSD_DEVICESTATE_WRITE_STOP) &&
                    (devDDC->devState !=
                     DDC_MMCSD_DEVICESTATE_READ_STOP)) != 0)) != 0))
                {
#ifdef MMCSD_DEBUG
                    if (status & (LLC_MMCSD_EVENT_ERROR_DATACRC))
                    {
                        printf("\n\rMMCSD-Data CRC error. status is %l",status);
                    }
                    else
                    {
                        printf("\n\rMMCSD-CMD CRC error. status is %l",status);
                    }
#endif
                    mmcsdControlCb(devDDC,
                        DDC_MMCSD_HWEVENT_ERROR_STATUS,
                        &status);
                    devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                }
            }

            (devDDC->fsm[devDDC->devState])(devDDC, status);
            if (TRUE == devDDC->isDmaError)
            {
                devDDC->devState = DDC_MMCSD_DEVICESTATE_WRITE_STOP;
                (devDDC->fsm[devDDC->devState])(devDDC,status);
                devDDC->isDmaError = FALSE;
            }
            mmcsdCheckFinishIOP(&gMmcsdDdcObj[devDDC->instId]);
            if ((status & (LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT |
              LLC_MMCSD_EVENT_ERROR_DATATIMEOUT)) != 0)
            {
                mmcsdControlCb(devDDC, DDC_MMCSD_HWEVENT_ERROR_STATUS, &status);
            }
        }
    }
}
#endif


/**\brief This function calculates the clock divider values for the card*/
static Int32 mmcsdCalculateFreqForCard(DDC_MmcsdHandle DDC,
                                       Uint32 initReqFreq,
                                       Uint32 mmcReqFreq,
                                       Uint32 sdReqFreq,
                                       Uint32 sdhighReqFreq)
{
    Uint32 mmcFreq, sdFreq, sdHighFreq, cpuArmClk = 0;
    Int32 result = IOM_COMPLETED;

    assert(NULL != DDC);

    if ((((initReqFreq > MMCSD_INIT_FREQ_MAX) ||
       (mmcReqFreq > MMC_CLOCK)) ||
       (sdReqFreq > SD_CLOCK)) ||
       (sdhighReqFreq > SD_HIGH_SPEED_CLOCK))
    {
        result = IOM_EBADARGS;
    }
    else
    {
        DDC->ddcDevObj.hDrvrClock.isOpenDrainDivBy4Reqd = FALSE;
        cpuArmClk = DDC->cpuArmClock;
        DDC->ddcDevObj.hDrvrClock.openDrain = ((Uint32)cpuArmClk /
                                               (2U * initReqFreq)) - 1U;
        if (DDC->ddcDevObj.hDrvrClock.openDrain > 0xFFU)
        {
            DDC->ddcDevObj.hDrvrClock.isOpenDrainDivBy4Reqd = TRUE;
            DDC->ddcDevObj.hDrvrClock.openDrain = ((Uint32)cpuArmClk /
                                       (4U * initReqFreq)) - 1U;
            if (DDC->ddcDevObj.hDrvrClock.openDrain > 0xFFU)
            {
                DDC->ddcDevObj.hDrvrClock.openDrain = 0xFFU;
            }
        }

        if (cpuArmClk > (2U * sdReqFreq))
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPull = ((Uint32)cpuArmClk /
                                                    (2U * sdReqFreq)) - 1U;
        }
        else
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPull = 0;
        }

        if (cpuArmClk > (2U * mmcReqFreq))
        {
            DDC->ddcDevObj.hDrvrClock.mmcPushPull = ((Uint32)cpuArmClk /
                                                     (2U * mmcReqFreq)) - 1U;
        }
        else
        {
            DDC->ddcDevObj.hDrvrClock.mmcPushPull = 0;
        }

        if (cpuArmClk > (2U * sdhighReqFreq))
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed = ((Uint32)cpuArmClk /
                                                    (2U * sdhighReqFreq)) - 1U;
        }
        else
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed = 0;
        }

        mmcFreq = (Uint32)cpuArmClk /
                      (2U * (DDC->ddcDevObj.hDrvrClock.mmcPushPull + 1U));
        sdFreq = (Uint32)cpuArmClk /
                     (2U * (DDC->ddcDevObj.hDrvrClock.sdPushPull + 1U));
        sdHighFreq = (Uint32)cpuArmClk / (2U *
                         (DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed + 1U));

        if (mmcFreq > mmcReqFreq)
        {
            DDC->ddcDevObj.hDrvrClock.mmcPushPull =
                DDC->ddcDevObj.hDrvrClock.mmcPushPull + 1U;
        }

        if (sdFreq > sdReqFreq)
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPull =
                DDC->ddcDevObj.hDrvrClock.sdPushPull + 1U;
        }

        if (sdHighFreq > sdhighReqFreq)
        {
            DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed =
                DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed + 1U;
        }
    }

    if ((((DDC->ddcDevObj.hDrvrClock.mmcPushPull > 0xFFU) ||
       (DDC->ddcDevObj.hDrvrClock.sdPushPull > 0xFFU)) ||
       (DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed > 0xFFU)) ||
       (DDC->ddcDevObj.hDrvrClock.openDrain > 0xFFU))
    {
        MMCSD_ERR_DEBUG(("\r\nMMCSD - mmcsdCalculateFreqForCard - freq value"
                         " overflow"));
    }

    return result;
}

/**\brief This is called when there is some more data in the list. Using this
API tell the number of data actually transferred by the driver to the upper
layer*/
static Void mmcsdHisrHandler(DDC_MmcsdObj *hDdc)
{
    DDC_MmcsdIoPacket*iop;
    Int32   count;
    Bool hisrFlag = FALSE;

    assert(NULL != hDdc);
    hisrFlag = TRUE;
    while (hisrFlag)
    {
        hisrFlag = TRUE;
        iop = (DDC_MmcsdIoPacket*)QUE_get(hDdc->iopReclaimableListHead);
        if ((NULL == iop) || ((Ptr)(hDdc->iopReclaimableListHead) == (Ptr)iop))
        {
            break;
        }

        if (IOM_COMPLETED == iop->result)
        {
            if (hDdc->blkSize != 0)
            {
                count = (Int32)(iop->xferActual / hDdc->blkSize);
            }
            else
            {
                count = (Int32)(IOM_EBADMODE);
            }
        }
        else
        {
            count = 0;
        }
#ifdef MMCSD_INSTRUMENTATION_ENABLED
        if (DDC_MMCSD_IOCMD_WRITE == iop->cmd){
            PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                        PSP_DVT_eDATA_SND_END,
                        PSP_DVT_dCHANNEL_ID_INST,
                        PSP_DVT_dPACKET_ID,
                        PSP_DVT_dSIZE_BYTES
                        ),
                CHANNELID_INSTANCEID(
                    iop->cmd,
                    0),
                iop->buf,
                count
                );
        }
        else if (DDC_MMCSD_IOCMD_READ == iop->cmd){
            PSP_LOG_EVENT(
                &DVTEvent_Log,
                "MMCSD",
                PSP_DVT_DESC(
                        PSP_DVT_eRCV_END,
                        PSP_DVT_dCHANNEL_ID_INST,
                        PSP_DVT_dPACKET_ID,
                        PSP_DVT_dSIZE_BYTES
                       ),
                CHANNELID_INSTANCEID(
                    iop->cmd,
                    0),
                iop->buf,
                count
                );
        }

#endif /* MMCSD_INSTRUMENTATION_ENABLED */
        hDdc->channelObj.ddcCallback(&(hDdc->channelObj),
                                     count,
                                     iop->mediaHandle);
        mmcsdFreeIop(hDdc,iop);
    }

}



/**
 * \brief MMCSD Interrupt interrupt handler function
 *
 * DDC registers its ISR with the OS.Typically a ISR returns the status of
 * interrupt (TX, RX, Error etc). DDC ISR can choose to process the respective
 * interrupts in ISR context (e.g Error interrupt) or just trigger the thread
 * processing packets. This flexible mechanism allows packet processing
 * priority to be controlled in its own thread to balance the overall system.
 * Alternatively, the DDC can choose to register this function directly with
 * the OS.
 *
 * \param   hDdc       DDC Handle
 *
 */
static Void mmcsdIsrHandler(DDC_MmcsdObj *hDdc)
{
    DDC_MmcsdDeviceHandle devDDC;
    if(NULL != hDdc)
    {
        devDDC = &(hDdc->ddcDevObj);
        if(NULL != devDDC)
        {
            if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
            {
                SEM_post(devDDC->syncSem);
            }

            if (FALSE == (QUE_empty(hDdc->iopReclaimableListHead)))
            {
                mmcsdHisrHandler(hDdc);
            }
        }
    }
}

/**\brief check iop function*/
static Void mmcsdCheckFinishIOP(DDC_MmcsdObj *hDdc)
{
    assert(NULL != hDdc);
    if (FALSE == (QUE_empty(hDdc->iopReclaimableListHead)))
    {
        mmcsdHisrHandler(hDdc);
    }
}

#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
/**\brief MMCSD instance 0 ISR*/
static Void mmcsdIsrHandler0(Void)
{
    mmcsdIsrHandler(&gMmcsdDdcObj[MMCSD_INST_ID0]);
}
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
/**\brief MMCSD instance 1 ISR*/
static Void mmcsdIsrHandler1(Void)
{
    mmcsdIsrHandler(&gMmcsdDdcObj[MMCSD_INST_ID1]);
}
#endif

/**\brief MMCSD DMA ISR handler function*/
static Void mmcsdDmaIsrHandler(Uint32 curr_lch,
                               EDMA3_RM_TccStatus ch_status,
                               Ptr data)
{
    DDC_MmcsdObj* hDdc;
    DDC_MmcsdDeviceHandle devDDC;
    Uint32 intState = 0;

    if (data != NULL)
    {
        hDdc = (DDC_MmcsdObj*)data;
        devDDC = &(hDdc->ddcDevObj);
        if (NULL == devDDC)
        {
            MMCSD_ERR_DEBUG(("\r\nddcMmcsdDmaIsrHandler : devDDC is NULL"));
            return;
        }

        intState = HWI_disable();
        devDDC->isDmaError = FALSE;
        HWI_restore(intState);

        if (((devDDC->flags & (DDC_MMCSD_FLAG_DEVICE_STOPPED)) != 0) ||
            (TRUE == QUE_empty(devDDC->listHeadNode)))
        {
#ifdef MMCSD_DEBUG
            printf("\n\rMMCSD-Error from DMA ISR");
#endif
        }

        /* check for event drop or channel timeout errors */
        if (((LLC_MmcsdObj *)devDDC->hLlc)->edmaChDetails.mmcTxCh == curr_lch)
        {
            if (((EDMA3_RM_E_CC_DMA_EVT_MISS == ch_status) ||
                (EDMA3_RM_E_CC_QDMA_EVT_MISS == ch_status)) ||
                (EDMA3_RM_E_CC_TCC == ch_status))
            {
#ifdef MMCSD_DEBUG
                printf("\n\rMMCSD-Error from DMA ISR while write");
#endif
                if (devDDC->devState == DDC_MMCSD_DEVICESTATE_WRITE)
                {
                    intState = HWI_disable();
                    devDDC->isDmaError = TRUE;
                    HWI_restore(intState);
                }
            }
        }
        else
        {
            if (((EDMA3_RM_E_CC_DMA_EVT_MISS == ch_status) ||
               (EDMA3_RM_E_CC_QDMA_EVT_MISS == ch_status)) ||
               (EDMA3_RM_E_CC_TCC == ch_status))
            {
#ifdef MMCSD_DEBUG
                printf("\n\rMMCSD-Error from DMA ISR while read");
#endif
                if (DDC_MMCSD_DEVICESTATE_READ == devDDC->devState)
                {
                    intState = HWI_disable();
                    devDDC->isDmaError = TRUE;
                    HWI_restore(intState);
                }
            }
        }

        LLC_mmcsdDmaStop(curr_lch, devDDC->hEdma);
        /*Free all channels used for linking purpose*/
        LLC_mmcsdDmaFreeChannel(devDDC->hLlc, devDDC->hEdma);
        if (TRUE == devDDC->isDmaError)
        {
            LLC_mmcsdDmaCleanChannel(curr_lch, devDDC->hEdma);
        }
#ifdef MMCSD_INSTRUMENTATION_ENABLED
        PSP_LOG_EVENT(
            &DVTEvent_Log,
            "MMCSD",
            PSP_DVT_DESC(
                    PSP_DVT_eFUNC_END,
                    PSP_DVT_dNONE
                    ),
            );
#endif /* MMCSD_INSTRUMENTATION_ENABLED */
        if (FALSE == (QUE_empty(hDdc->iopReclaimableListHead)))
        {
            mmcsdHisrHandler(data);
        }
    }
}



/**
 * \brief Asynchronous IO complete
 *
 * This function notifies that an IO request packet completed its IO
 * and should be reclaimed. The IOP may have completed processing nornally in
 * which case iop::result shall be IOM_COMPLETED. This field shall contain an
 * error code if the processing of an IOP did not complete normally.
 * iop::xferActual shall contain the number of bytes transferred. This must
 * be examined irrespective of iop::result. Further, if iop::timeout had been
 * specified then iop::xferActual will reflect the number of bytes that could
 * be transferred without blocking the calling thread of control.
 *
 * \param   hDDC            [IN]    DDC Handle
 * \param   iop             [IN]    Pointer to the IOP that just completed.
 *
 * \return IOM_COMPLETED or Error code
 */
static Void mmcsdFxnIoComplete(DDC_MmcsdHandle hDDC,
                               DDC_MmcsdIoPacket * const iop)
{
    assert((hDDC != NULL) && (iop != NULL));

    QUE_put(hDDC->iopReclaimableListHead, iop);
}


/**
 *  \brief Control (or notification) function
 *
 *  It can be used by the to report asynchronous events, errors, messages
 *  etc to the upper layer. It can also be used to get/set parameters on app.
 *  The nature of operations/usage is device specific and is left to the
 *  specifics of the hardware and driver implementation.
 *
 *  \param  devDDC  DDC Device Handle
 *  \param  cmd     Operation to be performed, typically an enum gets passed
 *  \param  cmdArg  Provides additonal information related to the operation.
                    Can be NULL.
 *  \return IOM_COMPLETED or Error code
 */
static Int32 mmcsdControlCb(DDC_MmcsdDeviceHandle devDDC, Int32 cmd, Ptr cmdArg)
{
    PSP_MmcsdHwEvent hwEvent = PSP_MMCSD_HWEVENT_CARD_REMOVED;
    assert(devDDC != NULL);
    switch (cmd)
    {
        case DDC_MMCSD_HWEVENT_ERROR_STATUS:
            if (NULL != cmdArg)
            {
                switch (*(Uint16*)cmdArg & LLC_MMCSD_EVENT_ERROR)
                {
                    case LLC_MMCSD_EVENT_ERROR_CMDCRC:
                        hwEvent = PSP_MMCSD_HWEVENT_COMMAND_CORRUPTED;
                        break;

                    case DDC_MMCSD_READ_DATA_CRC_ERROR:
                        hwEvent = PSP_MMCSD_HWEVENT_DATA_CORRUPTED;
                        break;

                    case DDC_MMCSD_WRITE_DATA_CRC_ERROR:
                        hwEvent = PSP_MMCSD_HWEVENT_DATA_CORRUPTED;
                        break;

                    case LLC_MMCSD_EVENT_ERROR_CMDTIMEOUT:
                    case LLC_MMCSD_EVENT_ERROR_DATATIMEOUT:
                        hwEvent = PSP_MMCSD_HWEVENT_CARD_REMOVED;
                        break;

                    default:
                        /*print error*/
                        break;
                }
            }
            if (devDDC->hwEventNotification != NULL)
            {
                devDDC->hwEventNotification(hwEvent, NULL);
            }
            break;

        case DDC_MMCSD_HWEVENT_ERROR_RESPONSE:
            hwEvent = PSP_MMCSD_HWEVENT_ERROR_CARD_STATUS;
            if (devDDC->hwEventNotification != NULL)
            {
                devDDC->hwEventNotification(hwEvent, NULL);
            }
            break;

        case DDC_MMCSD_HWEVENT_SYNC_DONE:
        case DDC_MMCSD_HWEVENT_CANCEL_DONE:
        default:
            /*print error*/
            break;
    }
    return IOM_COMPLETED;
}


/**\brief Set MMCSD for polled mode*/
static Void mmcsdSetPolledMode(DDC_MmcsdObj* const DDC)
{
    assert(NULL != DDC);

    DDC->ddcDevObj.opMode = PSP_MMCSD_OPMODE_POLLED;
}

/**\brief Set mmcsd for DMA mode*/
static Int32 mmcsdSetDmaMode(DDC_MmcsdObj* const DDC)
{
    Int32 result = IOM_COMPLETED;
    Uint32 tcc;
    /* Setup for Channel 1*/
    Uint32 ch1Id;

    assert(NULL != DDC);

    switch (DDC->instanceId)
    {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        case MMCSD_INST_ID0:
        {
            if (0 == isInst0DmaInitialize)
            {
                tcc = EDMA3_DRV_TCC_ANY;
                ch1Id = DDC->oLlc.edmaChDetails.mmcTxCh;
                result = EDMA3_DRV_requestChannel(DDC->ddcDevObj.hEdma,
                             &ch1Id,
                             &tcc,
                             (EDMA3_RM_EventQueue)DDC->ddcDevObj.eventQ,
                             &mmcsdDmaIsrHandler,
                             &gMmcsdDdcObj[DDC->instanceId]);

                if (result != IOM_COMPLETED)
                {
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for TX of MMCSD has failed");
#endif
                    break;
                }
                else
                {
                    DDC->oLlc.edmaChDetails.mmcTxCh = ch1Id;
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for TX of MMCSD "
                           "channel=%l,tcc=%l",ch1Id,tcc);
#endif
                }

                ch1Id = DDC->oLlc.edmaChDetails.mmcRxCh;
                tcc = EDMA3_DRV_TCC_ANY;
                result = EDMA3_DRV_requestChannel(DDC->ddcDevObj.hEdma,
                             &ch1Id,
                             &tcc,
                             (EDMA3_RM_EventQueue)DDC->ddcDevObj.eventQ,
                             &mmcsdDmaIsrHandler,
                             &gMmcsdDdcObj[DDC->instanceId]);

                if (result!= IOM_COMPLETED)
                {
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for RX of MMCSD has failed");
#endif
                    break;
                }
                else
                {
                    DDC->oLlc.edmaChDetails.mmcRxCh = ch1Id;
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for RX of MMCSD "
                           "channel=%l,tcc=%l",ch1Id,tcc);
#endif
                }
                isInst0DmaInitialize = 1U;
            }
        }
        break;
#endif
#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        case MMCSD_INST_ID1:
        {
            if (0 == isInst1DmaInitialize)
            {
                tcc = EDMA3_DRV_TCC_ANY;
                ch1Id = DDC->oLlc.edmaChDetails.mmcTxCh;
                result = EDMA3_DRV_requestChannel(DDC->ddcDevObj.hEdma,
                             &ch1Id,
                             &tcc,
                             (EDMA3_RM_EventQueue)DDC->ddcDevObj.eventQ,
                             &mmcsdDmaIsrHandler,
                             &gMmcsdDdcObj[DDC->instanceId]);

                if (result != IOM_COMPLETED)
                {
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for TX of MMCSD has failed");
#endif
                    break;
                }
                else
                {
                    DDC->oLlc.edmaChDetails.mmcTxCh = ch1Id;
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for TX of MMCSD "
                           "channel=%l,tcc=%l",ch1Id,tcc);
#endif
                }

                ch1Id = DDC->oLlc.edmaChDetails.mmcRxCh;
                tcc = EDMA3_DRV_TCC_ANY;
                result = EDMA3_DRV_requestChannel(DDC->ddcDevObj.hEdma,
                             &ch1Id,
                             &tcc,
                             (EDMA3_RM_EventQueue)DDC->ddcDevObj.eventQ,
                             &mmcsdDmaIsrHandler,
                             &gMmcsdDdcObj[DDC->instanceId]);

                if (result!= IOM_COMPLETED)
                {
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for RX of MMCSD has failed");
#endif
                    break;
                }
                else
                {
                    DDC->oLlc.edmaChDetails.mmcRxCh = ch1Id;
#ifdef MMCSD_DEBUG
                    printf("\r\nMMCSD EDMA channel for RX of MMCSD "
                           "channel=%l,tcc=%l",ch1Id,tcc);
#endif
                }
                isInst1DmaInitialize = 1U;
            }
        }
        break;
#endif
        default:
            break;
    }
    DDC->ddcDevObj.opMode = PSP_MMCSD_OPMODE_DMAINTERRUPT;

    return result;
}

/**\brief Remove the DMA mode*/
static Int32 mmcsdUnSetDmaMode(DDC_MmcsdObj* const DDC)
{
    Int32 result = IOM_COMPLETED;

    assert(NULL != DDC);

    switch (DDC->instanceId)
    {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        case MMCSD_INST_ID0:
            isInst0DmaInitialize = 0;
            break;
#endif
#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        case MMCSD_INST_ID1:
            isInst1DmaInitialize = 0;
            break;
#endif
        default:
            break;
    }
    result = EDMA3_DRV_freeChannel(DDC->ddcDevObj.hEdma,
                 DDC->oLlc.edmaChDetails.mmcTxCh);
    if (result!= IOM_COMPLETED)
    {
#ifdef MMCSD_DEBUG
        printf("\r\n!!MMCSD EDMA Tx channel Free failed");
#endif
    }
    result = EDMA3_DRV_freeChannel(DDC->ddcDevObj.hEdma,
                 DDC->oLlc.edmaChDetails.mmcRxCh);
    if (result!= IOM_COMPLETED)
    {
#ifdef MMCSD_DEBUG
        printf("\r\n!!MMCSD EDMA Rx channel Free failed");
#endif
    }
    return result;
}


/**\brief mmcsdFreeIop : move the iop got from the reclaimable pool to free
list */
static Int32 mmcsdFreeIop(DDC_MmcsdHandle hDDC,
                          DDC_MmcsdIoPacket* const iop)
{
    Int32 result = IOM_COMPLETED;

    assert((hDDC != NULL) && (iop != NULL));

    QUE_put(hDDC->iopFreeListHead, iop);

    return result;
}

/**\brief Input output function*/
static Int32 mmcsdIo(DDC_MmcsdChannelHandle hChannel,
                     Ptr            MedHandle,
                     Uint8          cmd,
                     Uint32         address,
                     Uint16* const  bufPtr,
                     Uint32         bufSz,
                     Uint32         timeout)
{
    DDC_MmcsdChannelHandle channel;
    DDC_MmcsdIoPacket *iop;
    Int32 lResult = IOM_COMPLETED;
    Uint32 count = 0;
    DDC_MmcsdDeviceHandle devDDC;
    Bool ioFlag = FALSE;

    assert(((NULL != hChannel) && (NULL != MedHandle)) && (NULL != bufPtr));

    channel = hChannel;
    assert(NULL != channel);

    devDDC = &(channel->hDDC->ddcDevObj);
    assert(NULL != devDDC);

    /*Take out an iop from the free list pool and make and packet*/
    iop = (DDC_MmcsdIoPacket*)QUE_get(channel->hDDC->iopFreeListHead);
    if (NULL == iop)
    {
        lResult = IOM_EALLOC;
    }

    if (IOM_COMPLETED == lResult)
    {
        iop->mediaHandle = MedHandle;
        if (FALSE == devDDC->isHighCapacitySDCard)
        {
            iop->blockAddress = address * channel->hDDC->blkSize;
        }
        else
        {
            iop->blockAddress = address;
        }

        iop->xferActual = 0;
        iop->cmd = cmd;
        iop->xferRequest = bufSz * channel->hDDC->blkSize;
        iop->buf = bufPtr;
        iop->timeout = timeout;
        iop->hDDC = channel->hDDC;

        lResult = mmcsdSubmitIoRequestSingle(devDDC, iop);
        if ((IOM_EBADMODE == lResult) || (IOM_EBADARGS == lResult))
        {
            channel->ddcCallback(channel, 0, iop->mediaHandle);
            mmcsdFreeIop(channel->hDDC, iop);
        }
        else if (IOM_PENDING != lResult)
        {
            ioFlag = TRUE;
            while (ioFlag)
            {
                ioFlag = TRUE;
                iop = QUE_get(channel->hDDC->iopReclaimableListHead);

                if ((Ptr)(channel->hDDC->iopReclaimableListHead) == (Ptr)iop)
                {
                    break;
                }

                if (IOM_COMPLETED == lResult)
                {
                    count = (iop->xferActual / channel->hDDC->blkSize);
                }
                else
                {
                    count = 0;
                }

                channel->ddcCallback(channel, count, iop->mediaHandle);
                mmcsdFreeIop(channel->hDDC, iop);
            }
        }
        else
        {
            /*print error*/
        }
    }
    return lResult;

}


/**\brief This function is used to implement timeout for sync function which is
not supported right now */
static Int32 mmcsdSyncDelay(DDC_MmcsdChannelObj* pChannel,
                            DDC_MmcsdIoPacket* iop,
                            Uint32          timeout)
{
    pChannel = pChannel;        /* to remove compilation warning */
    iop = iop;                  /* to remove compilation warning */
    timeout = timeout;          /* to remove compilation warning */
    return IOM_COMPLETED;
}

/**\brief This function is used to implement timeout for async function which is
not supported right now */
static Int32 mmcsdAsyncDelay(DDC_MmcsdChannelObj* pChannel,
                             DDC_MmcsdIoPacket* iop,
                             Uint32          timeout)
{
    pChannel = pChannel;        /* to remove compilation warning */
    iop = iop;                  /* to remove compilation warning */
    timeout = timeout;          /* to remove compilation warning */
    return IOM_COMPLETED;
}

/**\brief When driver is operating in sync mode,this function is used to
call user callback fxn if user has provided same */
static Int32 mmcsdSyncCallback(DDC_MmcsdChannelObj* pChannel,
                               Int32           count,
                               Ptr         mediaHandle)
{
    pChannel = pChannel;        /* to remove compilation warning */
    count = count;                  /* to remove compilation warning */
    mediaHandle = mediaHandle;          /* to remove compilation warning */
    return IOM_COMPLETED;
}

/**\brief When driver is operating in async mode,this function is used to
call user callback fxn if user has provided same */
static Int32 mmcsdAsyncCallback(DDC_MmcsdChannelObj* const pChannel,
                                Int32           count,
                                Ptr         mediaHandle)
{
    Int32 result = IOM_COMPLETED;

    assert((NULL != pChannel) && (NULL != mediaHandle));

    pChannel->appCallback(mediaHandle, count);

    return result;
}


/**\brief Register interrrupt with BIOS and enable it */
static Void mmcsdRegisterIntrHandler(Uint32 deviceId, Uint32 intrNum, Uint16 hwiNo)
{
    ECM_Attrs   ecmattrs    = ECM_ATTRS;
    Uint32      cookie      = 0;

    cookie = HWI_disable();
    /*Enable the interrupt later*/
    ecmattrs.unmask = 0u;
    /*setup arg such that isr will get 'chan' as arg*/
    ecmattrs.arg    = 0;

    switch (deviceId)
    {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
        case MMCSD_INST_ID0:
            /* Plug the MMCSD interrupt handler with interrupt (event) number */
            ECM_dispatchPlug(intrNum, (ECM_Fxn)&mmcsdIsrHandler0, &ecmattrs);
            /* Enabling the event                            */
            ECM_enableEvent(intrNum);
            break;
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
        case MMCSD_INST_ID1:
            /* Plug the MMCSD interrupt handler with interrupt (event) number */
            ECM_dispatchPlug(intrNum, (ECM_Fxn)&mmcsdIsrHandler1, &ecmattrs);
            /* Enabling the event                            */
            ECM_enableEvent(intrNum);
            break;
#endif
        default:
            MMCSD_ERR_DEBUG(("\r\nError: Wrong instance"));
    }
    /*Enabling the HWI_ID where corresponding ECM group is configured */
    C64_enableIER(1 << hwiNo);

    /* Enabling the interrupts */
    HWI_restore(cookie);
}

/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/

/**
 *  \brief    mmcsdLpscOn
 *
 *   This function is invoked to power on the peripheral. Note that this
 *   function uses different functions to power ON the module, depending on
 *   the PWRM is enabled in this driver or NOT
 *
 *  \param    DDC    [IN]    handle to the ddc
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  DDC    must be a valid ddc object
 *          The driver should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 mmcsdLpscOn(DDC_MmcsdObj* const DDC)
{
    Int32 lpscOnStatus;
    assert(NULL != DDC);

#ifdef BIOS_PWRM_ENABLE
    /* power on using PWRM API*/
    lpscOnStatus = PWRM_setDependency((PWRM_Resource)DDC->oLlc.pwrmLpscID);
#else
    /*power on using PSC API*/
    lpscOnStatus = Psc_ModuleClkCtrl((Psc_DevId)DDC->oLlc.pscInstance,
                       (Uint32)DDC->oLlc.pwrmLpscID,
                       TRUE);
#endif
    return lpscOnStatus;
}



/**
 *  \brief    mmcsdLpscOff
 *
 *   This function is invoked to power off the peripheral
 *   handler and interrupt cause respectively.
 *
 *  \param    DDC    [IN]    handle to the ddc
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  DDC    must be a valid ddc object
 *          The driver should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 mmcsdLpscOff(DDC_MmcsdObj* const DDC)
{
    Uint16 status;
    Int32 lpscOffStatus;
    assert(NULL != DDC);

    /* Wait and clear any unwanted event from the status register so that
    next time when the device is powered on it should not get any
    undesired event*/
    do
    {
        LLC_mmcsdGetEventStatus(&(DDC->oLlc), &status);
    }while (status != 0);

#ifdef BIOS_PWRM_ENABLE
    /* power off  using PWRM API*/
    lpscOffStatus = PWRM_releaseDependency((PWRM_Resource)DDC->oLlc.pwrmLpscID);
#else
     /*power off using PSC API*/
    lpscOffStatus = Psc_ModuleClkCtrl((Psc_DevId)DDC->oLlc.pscInstance,
                        (Uint32)DDC->oLlc.pwrmLpscID,
                        FALSE);
#endif
    return lpscOffStatus;
}


#ifdef BIOS_PWRM_ENABLE
/**
 * \brief     This function registers with the BIOS PWRM module for all the
 *            possible power management events.
 *
 * \param     DDC [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADARGS  in case of error
 *
 * \enter     DDC is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 mmcsdRegisterNotification(DDC_MmcsdObj* const DDC)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != DDC);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < DDC_MMCSD_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    mmcsdNotifyCallback,
                    (Arg)DDC,
                    &(DDC->ddcDevObj.pwrmInfo.notifyHandle[eventCnt]),
                    &(DDC->ddcDevObj.pwrmInfo.delayedCompletionFxn[eventCnt]));

        if (PWRM_SOK != retVal)
        {
            status = IOM_EBADARGS;
            break;
        }
    }
    return (status);
}

/**
 * \brief     This function unregisters all the power management events
 *            which were previously registered by the driver instance.
 *
 * \param     DDC [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADARGS  in case of error
 *
 * \enter     DDC is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 mmcsdUnregisterNotification(DDC_MmcsdObj* const DDC)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != DDC);

    for (eventCnt = 0; eventCnt < DDC_MMCSD_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        if (NULL != (DDC->ddcDevObj.pwrmInfo.notifyHandle[eventCnt]))
        {
            retVal = PWRM_unregisterNotify(
                        (DDC->ddcDevObj.pwrmInfo.notifyHandle[eventCnt]));

            if (PWRM_SOK != retVal)
            {
                status = IOM_EBADARGS;
                break;
            }
            else
            {
                DDC->ddcDevObj.pwrmInfo.notifyHandle[eventCnt] = 0x00;
            }
        }
    }
    return (status);
}

/**
 * \brief     This function is the notify function called by the BIOS whenever
 *            a power event occurs in the system.This function handles the
 *            power events and calls the appropriate functions to process the
 *            same.
 *
 * \param     eventType [IN] power event type
 * \param     eventArg1 [IN] event-specific argument
 * \param     eventArg2 [IN] event-specific argument
 * \param     clientArg [IN] argument passed by the PWRM module(usually
 *                          (supplied during the registration of the notifyFxn)
 *
 * \return    IOM_COMPLETED if success else error code
 *
 * \enter     eventType is a valid event
 *
 * \leave     Not implemented
 */
static PWRM_NotifyResponse mmcsdNotifyCallback(PWRM_Event    eventType,
                                               Arg           eventArg1,
                                               Arg           eventArg2,
                                               Arg           clientArg)
{
    DDC_MmcsdObj          *hDDC = NULL;
    PWRM_NotifyResponse  pwrmStatus = PWRM_NOTIFYDONE;
    Bool        isIopPending;

    assert(NULL != clientArg);
    hDDC = (DDC_MmcsdObj *)clientArg;
    assert(NULL != hDDC);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
        /* The mmcsd driver shall support PWRM events only when channel is  *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            isIopPending = QUE_empty(hDDC->ddcDevObj.listHeadNode) ? FALSE : TRUE;
            if (TRUE == isIopPending)
            {
                pwrmStatus = PWRM_NOTIFYERROR;
            }
            else
            {
                hDDC->ddcDevObj.prevDevstate = hDDC->ddcDevObj.state;
                hDDC->ddcDevObj.state = DDC_PWRM_SUSPEND;
            }
            break;

        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
        /* The mmcsd driver shall support PWRM events only when channel is  *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                 (PSP_MMCSD_PLL_DOMAIN_0 == hDDC->ddcDevObj.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                 (PSP_MMCSD_PLL_DOMAIN_1 == hDDC->ddcDevObj.pllDomain)))
            {
                isIopPending = QUE_empty(hDDC->ddcDevObj.listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    pwrmStatus = PWRM_NOTIFYERROR;
                }
                else
                {
                    hDDC->ddcDevObj.prevDevstate = hDDC->ddcDevObj.state;
                    hDDC->ddcDevObj.state = DDC_PWRM_SUSPEND;
                }
            }
            break;

        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
             /* The driver shall now reconfigure the clock settings based     *
              * on the new set point of the system                            */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                 (PSP_MMCSD_PLL_DOMAIN_0 == hDDC->ddcDevObj.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                (PSP_MMCSD_PLL_DOMAIN_1 == hDDC->ddcDevObj.pllDomain)))
            {
                pwrmStatus = mmcsdConfigureNewVfSettings(hDDC,
                                                         eventArg1,
                                                         eventArg2);
                hDDC->ddcDevObj.state = hDDC->ddcDevObj.prevDevstate;
            }
            break;

        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
        case PWRM_AWAKEFROMSTANDBY:
            /* There is nothing for the mmcsd driver to be done here. Just   *
             * check the channel status. The mmcsd will be enabled, during   *
             * first IOP submit                                              */
            isIopPending = QUE_empty(hDDC->ddcDevObj.listHeadNode) ? FALSE : TRUE;
            if (TRUE == isIopPending)
            {
                pwrmStatus = PWRM_NOTIFYERROR;
            }
            else
            {
                hDDC->ddcDevObj.state = hDDC->ddcDevObj.prevDevstate;
            }
            break;

        default:
            break;
    }
    return (pwrmStatus);
}

/**
 * \brief      This function calculates clock settings for the new DVFS set
 *             point.
 *
 * \param      DDC [IN] Handle to the device instance object
 * \param      eventArg1  [IN] previous set point.
 * \param      eventArg2  [IN] current set point
 *
 * \return     PWRM_SOK incase that the prescaler values for the new set point
 *             are calculated successfully else an appropriate error code is
 *             returned
 *
 * \enter      DDC is a valid non NULL pointer
 *
 * \leave      Not imeplemented
 */
static PWRM_NotifyResponse mmcsdConfigureNewVfSettings(DDC_MmcsdObj* const DDC,
                                                       Uns           eventArg1,
                                                       Uns           eventArg2)
{
    PWRM_NotifyResponse  retVal = PWRM_NOTIFYDONE;
    Int32                status = IOM_COMPLETED;
    PWRM_Status          pwrmStat = PWRM_SOK;
    PWRM_Domain          domain = PWRM_CPU;
    Uns                  frequency = 0x00;
    Uns                  voltage = 0x00;
    Uint32               sysclkDiv = 0x00;

    /* This function will calculate the settings for the new set point and    *
     * also indicate if this is a valid setpoint                              */
    assert(NULL != DDC);

    /* check if both the set points are not the same                          */
    if (eventArg1 != eventArg2)
    {
        /* The DVFS set point change has happened successfully.calculate the  *
         * new divider values                                                 */

        /* check what clock domain is supported by the device                 */
        if (PSP_MMCSD_PLL_DOMAIN_0 != DDC->ddcDevObj.pllDomain)
        {
            domain = PWRM_PER;
        }

        /* calculate the sysclk divisor value                                 */
        pwrmStat = PWRM_getSetpointInfo(domain,
                     eventArg1,
                     &frequency,
                     &voltage);

        if (PWRM_SOK == pwrmStat)
        {
            /* calculate the sysclk divisor value                             */
            sysclkDiv = ((frequency * 1000) / DDC->cpuArmClock);

            /* get the information about the current set point                */
            pwrmStat = PWRM_getSetpointInfo(domain,
                         eventArg2,
                         &frequency,
                         &voltage);

            if (PWRM_SOK == pwrmStat)
            {
                DDC->cpuArmClock = (frequency * 1000) / sysclkDiv;
                /* calculate the new divider values values                    */
                status = mmcsdCalculateFreqForCard(DDC,
                             MMCSD_INIT_CLOCK,
                             MMC_CLOCK,
                             SD_CLOCK,
                             SD_HIGH_SPEED_CLOCK);
                /*
                 * At 1.0 V the MAX IO speed is 25MHz. But when the module
                 * clock is 50Mhz, even if divider is put as 0 the IO clock
                 * can be max 25MHz. which is the IP limitation. So there is
                 * not need to set SD_HIGH_SPEED_CLOCK = 25MHz.
                 */
            }
        }

        /* If the driver is power aware enable the PSC */
        if ((IOM_COMPLETED == status) && (TRUE == DDC->oLlc.isPwrAware))
        {
            status = mmcsdLpscOn(DDC);
        }

        if ((IOM_COMPLETED == status) && (PWRM_SOK == pwrmStat))
        {
            /* calcualation of the divider value is successful.Hence update *
             * the hardware with the new divider values                     */
            if (PSP_MMCSD_CARDTYPE_SD == DDC->ddcDevObj.cardRegs.cardType)
            {
                if (TRUE == DDC->ddcDevObj.isHighSpeedSDCard)
                {
                    LLC_mmcsdSetFrequency(DDC->ddcDevObj.hLlc,
                        &(DDC->ddcDevObj.hDrvrClock.sdPushPullHighSpeed),
                        FALSE);
                }
                else
                {
                    LLC_mmcsdSetFrequency(DDC->ddcDevObj.hLlc,
                        &(DDC->ddcDevObj.hDrvrClock.sdPushPull),
                        FALSE);
                }
            }
            else if (PSP_MMCSD_CARDTYPE_MMC == DDC->ddcDevObj.cardRegs.cardType)
            {
                    LLC_mmcsdSetFrequency(DDC->ddcDevObj.hLlc,
                        &(DDC->ddcDevObj.hDrvrClock.mmcPushPull),
                        FALSE);
            }
            else
            {
                    LLC_mmcsdSetFrequency(DDC->ddcDevObj.hLlc,
                        &(DDC->ddcDevObj.hDrvrClock.openDrain),
                        FALSE);
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }

        /* If the driver is power aware disable the PSC */
        if ((IOM_COMPLETED == status) && (TRUE == DDC->oLlc.isPwrAware))
        {
            status = mmcsdLpscOff(DDC);
        }
    }

    if (IOM_COMPLETED != status)
    {
        retVal = PWRM_NOTIFYERROR;
    }

    return (retVal);
}

#endif

/*============================================================================*/
/*                             PWRM FUNCTIONS END                             */
/*============================================================================*/

/**\brief MMCSD open channel*/
Int32 PSP_mmcsdOpenChannel(PSP_MmcsdHandle hDDC,
                           PSP_MmcsdChannelHandle* const hChannel,
                           PSP_MmcsdChannelConfig* const chnlConfig)
{
    DDC_MmcsdObj* DDC;
    DDC_MmcsdDeviceHandle devDDC;
    Int32 status = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (((NULL == hDDC) || (NULL == hChannel)) || (NULL == chnlConfig))
    {
        status = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    DDC = (DDC_MmcsdObj*)hDDC;
    devDDC = &(DDC->ddcDevObj);

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == status)
    {
        if (NULL == devDDC)
        {
            status = IOM_EBADARGS;
        }

        if (devDDC->state != DDC_OPENED)
        {
            status = IOM_EBADMODE;
        }

        if (TRUE == DDC->channelObj.isAssigned)
        {
            status = IOM_EBADMODE;
        }
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        DDC->channelObj.isAssigned = TRUE;
        devDDC->devState = DDC_MMCSD_DEVICESTATE_START;
        devDDC->blkSize = 0;
        devDDC->cardRegs.cardType = PSP_MMCSD_CARDTYPE_NONE;

        devDDC->listHeadNode = QUE_create(NULL);
        if (NULL == devDDC->listHeadNode)
        {
            status = IOM_EALLOC;
        }
    }

    if (IOM_COMPLETED == status)
    {
        if (chnlConfig->appCallback != NULL)
        {
            /* async mode */
            DDC->channelObj.ddcCallback = &mmcsdAsyncCallback;
            DDC->channelObj.ddcDelay = &mmcsdAsyncDelay;
        }
        else
        {
            /* sync mode */
            DDC->channelObj.ddcCallback = &mmcsdSyncCallback;
            DDC->channelObj.ddcDelay = &mmcsdSyncDelay;
        }

        DDC->channelObj.appCallback = chnlConfig->appCallback;
        DDC->channelObj.hDDC = DDC;

        *hChannel = &(DDC->channelObj);
    }
    return status;
}

/**\brief MMCSD close channel*/
Int32 PSP_mmcsdCloseChannel(PSP_MmcsdChannelHandle hChannel)
{
    DDC_MmcsdChannelHandle  channel;
    DDC_MmcsdDeviceHandle devDDC;
    Uint32 intState;
    Int32 status = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == hChannel)
    {
        status = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    channel = (DDC_MmcsdChannelHandle)hChannel;
    devDDC = &(channel->hDDC->ddcDevObj);

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == status)
    {
        if (NULL == devDDC)
        {
            status = IOM_EBADARGS;
        }

        if ((DDC_OPENED != devDDC->state) || (channel->isAssigned != TRUE))
        {
            status = IOM_EBADMODE;
        }
    }
#endif
/* End parameter checking                                                     */
    if (IOM_COMPLETED == status)
    {
        intState = HWI_disable();
        if (FALSE == QUE_empty(devDDC->listHeadNode))
        {
            /* Wait for all the IOPs to be completed */
            HWI_restore(intState);
            status = IOM_EBADMODE;
        }
    }
    if (IOM_COMPLETED == status)
    {
        QUE_delete(devDDC->listHeadNode);
        HWI_restore(intState);

        channel->appCallback = NULL;
        channel->isAssigned = FALSE;
    }
    return status;
}


/**\brief This fuction initialise the mmcsd driver*/
Int32 PSP_mmcsdInitialize(Uint32 instNum,
                          PSP_MmcsdHandle* const hDDC,
                          PSP_MmcsdConfig* const pConfig,
                          Uint32          moduleInputClk)
{
    DDC_MmcsdObj*               DDC;
    DDC_MmcsdDeviceHandle       hDevDDC;
    Int32                       status;
    Int32                       i;
    SEM_Attrs                   temp;
    TSK_Attrs                   attrs;
    Bool                        flag = TRUE;

    do
    {
        /* Initialize the local variables as required                         */
        status = IOM_EBADARGS;
        flag = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == hDDC) || (NULL == pConfig))
        {
            break;
        }

        if (!((PSP_MMCSD_OPMODE_DMAINTERRUPT == pConfig->opMode) ||
              (PSP_MMCSD_OPMODE_POLLED == pConfig->opMode)))
        {
            status = IOM_ENOTIMPL;
            break;
        }

        if (instNum >= PSP_MMCSD_NUM_INSTANCES)
        {
            status = IOM_ENOTIMPL;
            break;
        }

        if ((NULL == pConfig->hEdma) &&
            (PSP_MMCSD_OPMODE_POLLED != pConfig->opMode))
        {
            break;
        }

        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == pConfig->opMode)
        {
            if ((PSP_MMCSD_EDMA3_EVENTQ_0 != pConfig->eventQ) &&
               (pConfig->eventQ != PSP_MMCSD_EDMA3_EVENTQ_1))
            {
                break;
            }
        }
#endif
/* End parameter checking                                                     */

        memset(&gMmcsdDdcObj[instNum], 0, sizeof(DDC_MmcsdObj));
        DDC = &gMmcsdDdcObj[instNum];
        *hDDC = &gMmcsdDdcObj[instNum];

        DDC->instanceId = instNum;
        DDC->cpuArmClock = moduleInputClk;

        /* mmcsd initialization starts here */
        hDevDDC = &(DDC->ddcDevObj);
        if (NULL == hDevDDC)
        {
            break;
        }

        if (((DDC_CREATED == hDevDDC->state) ||
            (DDC_INITIALIZED == hDevDDC->state)) ||
            (DDC_DEINITIALIZED == hDevDDC->state))
        {
            status = IOM_EBADMODE;
            break;
        }

        hDevDDC->instId = DDC->instanceId;
        hDevDDC->versionId = 0x0101ABCDU;
        hDevDDC->state = DDC_CREATED;

        temp.name = (String)"Mmcsd_Align_Sema";
        hDevDDC->semAlignDrvrThreads = SEM_create(1U, &temp);
        if (NULL == hDevDDC->semAlignDrvrThreads)
        {
            status = IOM_EALLOC;
            break;
        }

        temp.name = (String)"Mmcsd_Sync_Sema";
        hDevDDC->syncSem = SEM_create(0U, &temp);
        if (NULL == hDevDDC->syncSem)
        {
            status = IOM_EALLOC;
            break;
        }

        /* create a list of packets to be used henceforth */
        DDC->iopFreeListHead = QUE_create(NULL);

        /* no reclaimable packets right now! */
        DDC->iopReclaimableListHead = QUE_create(NULL);

        if ((NULL == DDC->iopFreeListHead) || (NULL == DDC->iopReclaimableListHead))
        {
            status = IOM_EALLOC;
            break;
        }

        for (i = 0; i < DDC_MMCSD_IO_DEPTH; i++)
        {
            QUE_put(DDC->iopFreeListHead,(Ptr)(&(DDC->iopPool[i])));
        }

        mmcsdCalculateFreqForCard(DDC,
            MMCSD_INIT_CLOCK,
            MMC_CLOCK,
            SD_CLOCK,
            SD_HIGH_SPEED_CLOCK);

        hDevDDC->drvrHwCfg = &gMmcsdDrvrHwCfg;
        hDevDDC->hLlc = &(DDC->oLlc);
        DDC->channelObj.isAssigned = FALSE;
        hDevDDC->hEdma = pConfig->hEdma;
        hDevDDC->eventQ = pConfig->eventQ;
        hDevDDC->opMode = pConfig->opMode;
        DDC->hwiNumber = pConfig->hwiNumber;
        DDC->oLlc.isPwrAware = pConfig->pscPwrmEnable;
        hDevDDC->pllDomain = pConfig->pllDomain;

        if (((DDC_CREATED != hDevDDC->state) &&
            (DDC_DELETED != hDevDDC->state)) &&
            (DDC_DEINITIALIZED != hDevDDC->state))
        {
            status = IOM_EBADMODE;
            break;
        }

        hDevDDC->flags = DDC_MMCSD_FLAG_DEVICE_STOPPED;

        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_TRAN]           = &mmcsdTranStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_READ]           = &mmcsdReadStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_READ_STOP]      = &mmcsdReadStopStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_WRITE]          = &mmcsdWriteStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_WRITE_STOP]     = &mmcsdWriteStopStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_STATUS]         = &mmcsdSendStatusStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_FINISH_CHECK]   = &mmcsdFinishCheckStFxn;
        hDevDDC->fsm[DDC_MMCSD_DEVICESTATE_FINISH_BUSY]    = &mmcsdFinishBusyStFxn;

        /*Added for LLC Init*/
        LLC_mmcsdInit(hDevDDC->hLlc, hDevDDC->instId);

        if (FALSE == DDC->oLlc.isPwrAware)
        {
            /*
             * The first thing to do when power management is not desired is to
             * turn on the controller using the PSC or PWRM API
             */
             status = mmcsdLpscOn(DDC);
            if (IOM_COMPLETED != status)
            {
                break;
            }
        }

        hDevDDC->state = DDC_INITIALIZED;

        hDevDDC->hwEventNotification = NULL;
        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == hDevDDC->opMode)
        {
#if(PSP_MMCSD_NUM_INSTANCES > 0x0)
            /* Create MMCSD Driver task */
            if (MMCSD_INST_ID0 == instNum)
            {
                attrs.priority = PSP_MMCSD_DEV_TASKPRIO;
                attrs.stack = NULL;
                attrs.stacksize = PSP_MMCSD_DEV_STACK_SIZE_BYTE;
                attrs.stackseg = 0;
                attrs.environ = NULL;
                attrs.name = "Mmcsd0_Task";
                attrs.initstackflag = 1U;
                attrs.exitflag = 1U;

                /*Create MMCSD Driver task for instance 0*/
                DDC->taskHandle = TSK_create((Fxn)&mmcsdTask0, &attrs, NULL);
                if (NULL == DDC->taskHandle)
                {
                    MMCSD_ERR_DEBUG(("\r\nError: Cannot create mmcsd task0"));
                    status = IOM_EALLOC;
                    break;
                }
                hDevDDC->taskRun = TRUE;
            }
#endif

#if(PSP_MMCSD_NUM_INSTANCES > 0x1)
            /* Create MMCSD Driver task */
            if (MMCSD_INST_ID1 == instNum)
            {
                attrs.priority = PSP_MMCSD_DEV_TASKPRIO;
                attrs.stack = NULL;
                attrs.stacksize = PSP_MMCSD_DEV_STACK_SIZE_BYTE;
                attrs.stackseg = 0;
                attrs.environ = NULL;
                attrs.name = "Mmcsd1_Task";
                attrs.initstackflag = 1U;
                attrs.exitflag = 1U;

                /*Create MMCSD Driver task for instance 1*/
                DDC->taskHandle = TSK_create((Fxn)&mmcsdTask1, &attrs, NULL);
                if (NULL == DDC->taskHandle)
                {
                    MMCSD_ERR_DEBUG(("\r\nError: Cannot create mmcsd task1"));
                    status = IOM_EALLOC;
                    break;
                }
                hDevDDC->taskRun = TRUE;
            }
#endif

            mmcsdRegisterIntrHandler(hDevDDC->instId,
                DDC->oLlc.intrNum,
                DDC->hwiNumber);
            status = mmcsdSetDmaMode(DDC);
            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADMODE;
                break;
            }
            mmcsdConfigDmaMode(hDevDDC);
        }
        else
        {
            status = IOM_COMPLETED;
            mmcsdSetPolledMode(DDC);
            mmcsdConfigPolledMode(hDevDDC);
        }

        /* Check for valid entry conditions */
        if ((hDevDDC->state != DDC_INITIALIZED) && (hDevDDC->state != DDC_CLOSED))
        {
            status = IOM_EBADMODE;
            break;
        }

#ifdef BIOS_PWRM_ENABLE
        memset(&hDevDDC->pwrmInfo, 0x00, sizeof(DDC_MmcsdPwrmInfo));
        if ((IOM_COMPLETED == status) && (TRUE == DDC->oLlc.isPwrAware))
        {
            /* register the notify function for the PWRM events               */
            status = mmcsdRegisterNotification(DDC);
        }
#endif

        hDevDDC->state = DDC_OPENED;
    }while (flag);

    return status;
}

/**\brief This function terminates or deinit mmcsd driver*/
Int32 PSP_mmcsdTerminate(PSP_MmcsdHandle hDDC)
{
    DDC_MmcsdObj* DDC;
    DDC_MmcsdDeviceHandle devDDC;
    Int32 sts = IOM_COMPLETED;
    Uint32          cookie      = 0;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == hDDC)
    {
        sts = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    DDC = (DDC_MmcsdObj*)hDDC;
    devDDC = &(DDC->ddcDevObj);

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == sts)
    {
        if (NULL == devDDC)
        {
            sts = IOM_EBADARGS;
        }

        if (devDDC->state != DDC_OPENED)
        {
            sts = IOM_EBADMODE;
        }
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == sts)
    {
        devDDC->state = DDC_CLOSED;

        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
        {
            cookie = HWI_disable();
            /* Disable the event*/
            ECM_disableEvent(DDC->oLlc.intrNum);
            HWI_restore(cookie);
            mmcsdUnSetDmaMode(DDC);
        }

        /* Check for the correct entry condition */
        if ((devDDC->state != DDC_INITIALIZED) && (devDDC->state != DDC_CLOSED))
        {
            sts = IOM_EBADMODE;
        }
    }
    if (IOM_COMPLETED == sts)
    {
        devDDC->state = DDC_DELETED;

        SEM_delete(devDDC->semAlignDrvrThreads);
        if (PSP_MMCSD_OPMODE_DMAINTERRUPT == devDDC->opMode)
        {
            devDDC->taskRun = FALSE;
            SEM_post(devDDC->syncSem);
            SEM_delete(devDDC->syncSem);
            TSK_delete(DDC->taskHandle);
        }
        else
        {
            SEM_delete(devDDC->syncSem);
        }
        QUE_delete(DDC->iopFreeListHead);
        QUE_delete(DDC->iopReclaimableListHead);
        /* If the driver is not power aware disable the PSC */
        if (FALSE == DDC->oLlc.isPwrAware)
        {
            /*
             * The last thing to do when power management is not desired is to
             * turn off the controller using the PSC or PWRM API
             */
            sts = mmcsdLpscOff(DDC);
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == sts) && (TRUE == DDC->oLlc.isPwrAware))
        {
            /* unregister all the PWRM event notifications                    */
            sts = mmcsdUnregisterNotification(DDC);
        }
#endif

    }
    return sts;
}


/**\brief MMCSD read function*/
Int32 PSP_mmcsdInput(PSP_MmcsdChannelHandle hChannel,
                     Ptr         mediaHandle,
                     Uint32      address,
                     Uint16*     bufPtr,
                     Uint32      bufSz,
                     Uint32      timeout)
{
#ifdef MMCSD_DEBUG
    readRequest++;
#endif
    /* Validate the input paramaters                                      */
    assert((((NULL != hChannel) && (NULL != mediaHandle)) &&
            (NULL != bufPtr)) && (0 != bufSz));
    return mmcsdIo((DDC_MmcsdChannelHandle)hChannel,
               mediaHandle,
               DDC_MMCSD_IOCMD_READ,
               address,
               bufPtr,
               bufSz,
               timeout);
}

/**\brief MMCSD write channel*/
Int32 PSP_mmcsdOutput(PSP_MmcsdChannelHandle hChannel,
                      Ptr  mediaHandle,
                      Uint32  address,
                      Uint16* bufPtr,
                      Uint32  bufSz,
                      Uint32  timeout)
{
#ifdef MMCSD_DEBUG
    writeRequest++;
#endif
    /* Validate the input paramaters                                      */
    assert((((NULL != hChannel) && (NULL != mediaHandle)) &&
            (NULL != bufPtr)) && (0 != bufSz));
    return mmcsdIo((DDC_MmcsdChannelHandle)hChannel,
               mediaHandle,
               DDC_MMCSD_IOCMD_WRITE,
               address,
               bufPtr,
               bufSz,
               timeout);
}


/**
 *  \brief MMCSD Control operations (ioctl)
 *
 * This API provides the capability for control operations to be performed on
 * the device driver core. Typically the "ioctl" style set/get functionality
 * is used to configure/program the parameters in devDDC or get status or read
 * devDDC parameters. This mechanism can also be used by the upper layer driver
 * software to configure a parameter in the device driver core or change a
 * parameter during run time. The implementation details and capabilities of the
 * function are left to the specifics of the hardware device.
 * Typically, this function will have the option to get/set all the config
 * parameters via a structure (apart from capability to get/set one parameter.
 * This helps in getting/setting all the configuration parameters all at once
 * if required. arg and param can be NULL for some of the IOCTLs.
 *
 */
Int32 PSP_mmcsdStatus(PSP_MmcsdChannelHandle hChannel,
                      PSP_MmcsdIoctl     cmd,
                      Ptr const arg,
                      Ptr param)
{
    DDC_MmcsdChannelHandle  channel;
    Int32                   retVal = IOM_COMPLETED;
    DDC_MmcsdDeviceHandle   devDDC;
    Int32                   pwrResult;       /* Result of power APIs        */
    Bool                    pscPwrOn = FALSE;/* Pwr off or not              */

    assert(NULL != hChannel);
    assert(cmd < PSP_MMCSD_IOCTL_MAX_IOCTL);

    channel = (DDC_MmcsdChannelHandle)hChannel;
    devDDC = &(channel->hDDC->ddcDevObj);

    assert(NULL != devDDC);
    assert(DDC_OPENED == devDDC->state);
    /* If the driver is power aware enable the PSC */
    if (TRUE == channel->hDDC->oLlc.isPwrAware)
    {
        pwrResult = mmcsdLpscOn(channel->hDDC);
        if (IOM_COMPLETED != pwrResult)
        {
            retVal = pwrResult;
        }
        else
        {
            pscPwrOn = TRUE;
        }
    }


    if (IOM_COMPLETED == retVal)
    {
        switch (cmd)
        {
            /* Implement temporary write protect*/
            case PSP_MMCSD_IOCTL_SET_TEMPORARY_WP:
            {
                Bool            isIopPending;
                Uint8           i, j;
                Uint8           cardCSD[MMCSD_CSD_SIZE_BYTE];
                Uint16          mmcIntOldMode;
                LLC_MmcsdCmd    cmdTempWP;
                Uint16          resp[MAX_RESP_SIZE_BYTES];
                Uint16          statusTempWP = 0;
                Uint16          errStatus = LLC_MMCSD_EVENT_ERROR;
                Uint16          reqdStatus = LLC_MMCSD_EVENT_BLOCK_XFERRED;
                Uint8           crc = 0;
                Uint8           crcValue = 0;
                Uint32          rca;
                Bool            currentState = FALSE;

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.mmc.csd[1] &
                         TEMPERORY_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is temp write protected*/
                        currentState = TRUE;
                    }
                    else
                    {
                        currentState = FALSE;
                    }
                }
                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.sd.csd[1] &
                         TEMPERORY_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is temp write protected*/
                        currentState = TRUE;
                    }
                    else
                    {
                        currentState = FALSE;
                    }
                }
                assert(NULL != arg);
                if (currentState == (*(Bool*)arg))
                {
                    retVal = IOM_COMPLETED;
                    break;
                }

                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    j = 15U;
                    for (i = 0; i < 16U; i++)
                    {
                        /*Reverse the byte sequence as expected by card*/
                        cardCSD[i] = devDDC->cardRegs.mmc.csd[j];
                        j--;
                    }
                }
                else if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    j = 15U;
                    for (i = 0; i < 16U; i++)
                    {
                        /*Reverse the byte sequence as expected by card*/
                        cardCSD[i] = devDDC->cardRegs.sd.csd[j];
                        j--;
                    }
                }
                else
                {
                    retVal = IOM_EBADARGS;
                    break;
                }

                if (TRUE == (*(Bool*)arg))
                {
                    /*Set the temporary write protection*/
                    cardCSD[14] = cardCSD[14] | TEMPERORY_WRITE_PROTECT_MASK;
                }
                else
                {
                    /* clear the temporary clear protection*/
                    cardCSD[14] = cardCSD[14] & (~TEMPERORY_WRITE_PROTECT_MASK);
                }

                crc = 0;
                crcValue = mmcsdCalculateCRC7(crc, cardCSD, 15U);
                /*Populate the newly calculated CR value*/
                cardCSD[15] = (Uint8)((crcValue << 1) | 0x1);

                if (TRUE != SEM_pend(devDDC->semAlignDrvrThreads, SYS_FOREVER))
                {
                    retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                    SEM_post(devDDC->semAlignDrvrThreads);
                    break;
                }

                /* work in polled mode, so no mmcsd level interrupts */
                LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, &mmcIntOldMode);
                LLC_mmcsdSetBlockLen(devDDC->hLlc, MMCSD_CSD_SIZE_BYTE);
                LLC_mmcsdSetNumBlocks(devDDC->hLlc, ONE_BLOCK_REQUEST);

                /*Configure the FIFO*/
                LLC_mmcsdConfigureFIFO(devDDC->hLlc,
                    LLC_MMCSD_DIR_WRITE,
                    MMCSD_FIFO_SIZE_32_BYTES);/*FIFO 256bit*/
                retVal = 0;
                if (IOM_COMPLETED == retVal)
                {
                    cmdTempWP.busy        = LLC_MMCSD_RESPBUSY_NO;
                    cmdTempWP.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                    cmdTempWP.opMode      = PSP_MMCSD_OPMODE_DMAINTERRUPT;
                    cmdTempWP.type        = LLC_MMCSD_CMDTYPE_ADTC;
                    cmdTempWP.respType    = LLC_MMCSD_RESPTYPE_R1;
                    cmdTempWP.idx         = LLC_MMCSD_PROGRAM_CSD;
                    cmdTempWP.dir         = LLC_MMCSD_DIR_WRITE;

                    LLC_mmcsdSendCmd(devDDC->hLlc, &cmdTempWP, 0U);

                    reqdStatus = LLC_MMCSD_EVENT_WRITE;
                    retVal = mmcsdGetStatus(devDDC,
                                 &statusTempWP,
                                 errStatus,
                                 reqdStatus);
                    if (retVal != IOM_COMPLETED)
                    {
                        retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                        LLC_mmcsdSetInterruptMode(devDDC->hLlc,
                            mmcIntOldMode,
                            NULL);
                        SEM_post(devDDC->semAlignDrvrThreads);
                        break;
                    }
                }
                statusTempWP = 0;/* Reset the status */

                LLC_mmcsdWrite(devDDC->hLlc, MMCSD_CSD_SIZE_BYTE, cardCSD);
                if (0 == (statusTempWP & LLC_MMCSD_EVENT_BLOCK_XFERRED))
                {
                    reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
                    retVal = mmcsdGetStatus(devDDC,
                                 &statusTempWP,
                                 errStatus,
                                 reqdStatus);
                }

                cmdTempWP.idx         = LLC_MMCSD_SEND_STATUS;
                cmdTempWP.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmdTempWP.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmdTempWP.type        = LLC_MMCSD_CMDTYPE_AC;
                cmdTempWP.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmdTempWP.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmdTempWP.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmdTempWP, rca);
                retVal = LLC_mmcsdGetResp(devDDC->hLlc,
                             LLC_MMCSD_RESPTYPE_R1,
                             resp);
                if (0 == ((Uint32)(((((Uint32)resp[7]) << 16) | resp[6])) &
                            LLC_MMCSD_RESPONSE_R1_ERROR_MASK))
                {
                    retVal =  IOM_COMPLETED;
                }
                else
                {
                    retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                }

                LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntOldMode, NULL);
                SEM_post(devDDC->semAlignDrvrThreads);

                if (IOM_COMPLETED == retVal)
                {
                    if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                    {
                        devDDC->cardRegs.mmc.csd[1] = cardCSD[14];
                        if ((devDDC->cardRegs.mmc.csd[1] &
                             TEMPERORY_WRITE_PROTECT_MASK) != 0)
                        {
                            /*Card is temp write protected*/
                            devDDC->tempWP = TRUE;
                        }
                        else
                        {
                            devDDC->tempWP = FALSE;
                        }
                    }

                    if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                    {
                        devDDC->cardRegs.sd.csd[1] = cardCSD[14];
                        if ((devDDC->cardRegs.sd.csd[1] &
                             TEMPERORY_WRITE_PROTECT_MASK) != 0)
                        {
                            /*Card is temp write protected*/
                            devDDC->tempWP = TRUE;
                        }
                        else
                        {
                            devDDC->tempWP = FALSE;
                        }
                    }
                }
            }
            break;

            case PSP_MMCSD_IOCTL_GET_TEMPORARY_WP:
            {
                Bool isIopPending;
                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.mmc.csd[1] &
                         TEMPERORY_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is temp write protected*/
                        *(Bool*)arg = TRUE;
                    }
                    else
                    {
                        *(Bool*)arg = FALSE;
                    }
                }
                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.sd.csd[1] &
                         TEMPERORY_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is temp write protected*/
                        *(Bool*)arg = TRUE;
                    }
                    else
                    {
                        *(Bool*)arg = FALSE;
                    }
                }
            }
            break;

            /*This will make the card permanent read only*/
            case PSP_MMCSD_IOCTL_SET_PERMANENT_WP:
            {
                Bool            isIopPending;
                /*Set the permanent write protection*/
                Uint8           i,j;
                Uint8           cardCSD[MMCSD_CSD_SIZE_BYTE];
                Uint16          mmcIntOldMode;
                LLC_MmcsdCmd    cmdPermWP;
                Uint16          resp[MAX_RESP_SIZE_BYTES];
                Uint16          statusPermWP = 0;
                Uint16          errStatus = LLC_MMCSD_EVENT_ERROR;
                Uint16          reqdStatus = LLC_MMCSD_EVENT_BLOCK_XFERRED;
                Uint8           crc = 0;
                Uint8           crcValue = 0;
                Uint32          rca;
                Bool            currentState = FALSE;

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.mmc.csd[1] &
                         PERMANENT_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is permanent write protected*/
                        currentState = TRUE;
                    }
                    else
                    {
                        currentState = FALSE;
                    }
                }

                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.sd.csd[1] &
                         PERMANENT_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is permanent write protected*/
                        currentState = TRUE;
                    }
                    else
                    {
                        currentState = FALSE;
                    }
                }

                if (currentState == (*(Bool*)arg))
                {
                    retVal = IOM_COMPLETED;
                    break;
                }

                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    j = 15U;
                    for (i = 0; i < 16U; i++)
                    {
                        /*Reverse the byte sequence as expected by card*/
                        cardCSD[i] = devDDC->cardRegs.mmc.csd[j];
                        j--;
                    }
                }
                else if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    j = 15U;
                    for (i = 0; i < 16U; i++)
                    {
                        /*Reverse the byte sequence as expected by card*/
                        cardCSD[i] = devDDC->cardRegs.sd.csd[j];
                        j--;
                    }
                }
                else
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                if (TRUE == (*(Bool*)arg))
                {
                    /*Set the Permaent write protection*/
                    cardCSD[14] = cardCSD[14] | PERMANENT_WRITE_PROTECT_MASK;
                }
                else
                {
                    /* clear the Permanent clear protection which will generate
                    error effectively*/
                    cardCSD[14] = cardCSD[14] & (~PERMANENT_WRITE_PROTECT_MASK);
                }

                crc = 0;
                crcValue = mmcsdCalculateCRC7(crc, cardCSD, 15U);
                /*Populate the newly calculated CRC value*/
                cardCSD[15] = (Uint8)((crcValue << 1) | 0x1);

                if (TRUE != SEM_pend(devDDC->semAlignDrvrThreads, SYS_FOREVER))
                {
                    retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                    SEM_post(devDDC->semAlignDrvrThreads);
                    break;
                }

                /* work in polled mode, so no mmcsd level interrupts */
                LLC_mmcsdSetInterruptMode(devDDC->hLlc, 0U, &mmcIntOldMode);
                LLC_mmcsdSetBlockLen(devDDC->hLlc, MMCSD_CSD_SIZE_BYTE);
                LLC_mmcsdSetNumBlocks(devDDC->hLlc, ONE_BLOCK_REQUEST);

                /*Configure the FIFO*/
                LLC_mmcsdConfigureFIFO(devDDC->hLlc,
                    LLC_MMCSD_DIR_WRITE,
                    MMCSD_FIFO_SIZE_32_BYTES);/*FIFO 256bit*/
                retVal = 0;

                if (IOM_COMPLETED == retVal)
                {
                    /*Program CSD*/
                    cmdPermWP.idx         = LLC_MMCSD_PROGRAM_CSD;
                    cmdPermWP.busy        = LLC_MMCSD_RESPBUSY_NO;
                    cmdPermWP.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                    cmdPermWP.opMode      = PSP_MMCSD_OPMODE_DMAINTERRUPT;
                    cmdPermWP.type        = LLC_MMCSD_CMDTYPE_ADTC;
                    cmdPermWP.respType    = LLC_MMCSD_RESPTYPE_R1;
                    cmdPermWP.dir         = LLC_MMCSD_DIR_WRITE;

                    LLC_mmcsdSendCmd(devDDC->hLlc, &cmdPermWP, 0U);

                    reqdStatus = LLC_MMCSD_EVENT_WRITE;
                    retVal = mmcsdGetStatus(devDDC,
                                 &statusPermWP,
                                 errStatus,
                                 reqdStatus);
                    if (retVal != IOM_COMPLETED)
                    {
                        retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                        LLC_mmcsdSetInterruptMode(devDDC->hLlc,
                            mmcIntOldMode,
                            NULL);
                        SEM_post(devDDC->semAlignDrvrThreads);
                        break;
                    }
                }
                LLC_mmcsdWrite(devDDC->hLlc, MMCSD_CSD_SIZE_BYTE, cardCSD);
                if (0 == (statusPermWP & LLC_MMCSD_EVENT_BLOCK_XFERRED))
                {
                    reqdStatus = (Uint16)LLC_MMCSD_EVENT_BLOCK_XFERRED;
                    retVal = mmcsdGetStatus(devDDC,
                                 &statusPermWP,
                                 errStatus,
                                 reqdStatus);
                }
                /*send Status*/
                cmdPermWP.idx         = LLC_MMCSD_SEND_STATUS;
                cmdPermWP.respType    = LLC_MMCSD_RESPTYPE_R1;
                cmdPermWP.busy        = LLC_MMCSD_RESPBUSY_NO;
                cmdPermWP.type        = LLC_MMCSD_CMDTYPE_AC;
                cmdPermWP.dir         = LLC_MMCSD_DIR_DONTCARE;
                cmdPermWP.lineChr     = LLC_MMCSD_LINECHR_PUSHPULL;
                cmdPermWP.opMode      = devDDC->opMode;

                rca = (Uint32)((PSP_MMCSD_CARDTYPE_MMC == \
                          devDDC->cardRegs.cardType) ?
                        ((Uint32)devDDC->cardRegs.mmc.rca << RCA_SHIFT_MASK) :
                        ((Uint32)devDDC->cardRegs.sd.rca << RCA_SHIFT_MASK));

                LLC_mmcsdSendCmd(devDDC->hLlc, &cmdPermWP, rca);
                retVal = mmcsdCheckResponse(devDDC->hLlc,
                             LLC_MMCSD_RESPTYPE_R1,
                             resp);
                if (IOM_COMPLETED != retVal)
                {
                    retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                }

                LLC_mmcsdSetInterruptMode(devDDC->hLlc, mmcIntOldMode, NULL);
                SEM_post(devDDC->semAlignDrvrThreads);

                if (IOM_COMPLETED == retVal)
                {
                    if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                    {
                        devDDC->cardRegs.mmc.csd[1] = cardCSD[14];
                        if ((devDDC->cardRegs.mmc.csd[1] &
                             PERMANENT_WRITE_PROTECT_MASK) != 0)
                        {
                            /*Card is permanent write protected*/
                            devDDC->permanentWP = TRUE;
                        }
                        else
                        {
                            devDDC->permanentWP = FALSE;
                        }
                    }

                    if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                    {
                        devDDC->cardRegs.sd.csd[1] = cardCSD[14];
                        if ((devDDC->cardRegs.sd.csd[1] &
                             PERMANENT_WRITE_PROTECT_MASK) != 0)
                        {
                            /*Card is permanent write protected*/
                            devDDC->permanentWP = TRUE;
                        }
                        else
                        {
                            devDDC->permanentWP = FALSE;
                        }
                    }
                }
            }
            break;

            case PSP_MMCSD_IOCTL_GET_PERMANENT_WP:
            {
                Bool isIopPending;
                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.mmc.csd[1] &
                         PERMANENT_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is permanent write protected*/
                        *(Bool*)arg = TRUE;
                    }
                    else
                    {
                        *(Bool*)arg = FALSE;
                    }
                }

                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    if ((devDDC->cardRegs.sd.csd[1] &
                         PERMANENT_WRITE_PROTECT_MASK) != 0)
                    {
                        /*Card is permanent write protected*/
                        *(Bool*)arg = TRUE;
                    }
                    else
                    {
                        *(Bool*)arg = FALSE;
                    }
                }
            }
            break;

            case PSP_MMCSD_IOCTL_GET_CARD_SIZE:
            {
                *(Uint32*)arg = mmcsdGetCapacity(devDDC,
                                    devDDC->cardRegs.cardType);
            }
            break;

            case PSP_MMCSD_IOCTL_START:
            {
                mmcsdCalculateFreqForCard(channel->hDDC,
                    MMCSD_INIT_CLOCK,
                    MMC_CLOCK,
                    SD_CLOCK,
                    SD_HIGH_SPEED_CLOCK);
                /* take sync sema which will be released only by mmcsd isr*/
                if (TRUE != SEM_pend(devDDC->semAlignDrvrThreads, SYS_FOREVER))
                {
                    MMCSD_ERR_DEBUG(("\r\n semAlignDrvrThreads Take failed "));
                }

                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    retVal = mmcsdInitMmcCard(devDDC);
                }
                else if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    retVal = mmcsdInitSdCard(devDDC);
                }
                else
                {
                    retVal = PSP_MMCSD_RESULT_ERROR_FAILURE;
                }
                if (IOM_COMPLETED == retVal)
                {
                    devDDC->devState = DDC_MMCSD_DEVICESTATE_TRAN;
                    devDDC->flags &= ~DDC_MMCSD_FLAG_DEVICE_STOPPED;
                }

                SEM_post(devDDC->semAlignDrvrThreads);
                if (IOM_COMPLETED != retVal)
                {
                    break;
                }
                channel->hDDC->blkSize = devDDC->blkSize;
            }
            break;

            case PSP_MMCSD_IOCTL_CHECK_CARD:
            {
                retVal = mmcsdCheckCard(devDDC);
                (*(PSP_MmcsdCardType*)arg) = devDDC->cardRegs.cardType;
            }
            break;

            case PSP_MMCSD_IOCTL_GET_CARDREGS:
            {
                (*(PSP_MmcsdCardRegs*)arg) = devDDC->cardRegs;
            }
            break;

            case PSP_MMCSD_IOCTL_GET_BLOCKSIZE:
            {
                if (channel->hDDC->blkSize > 0)
                {
                    *(Uint32*)arg = channel->hDDC->blkSize;
                }
                else
                {
                    retVal = IOM_EBADMODE;
                }
            }
            break;

            case PSP_MMCSD_IOCTL_GET_OPMODE:
            {
                (*(PSP_MmcsdOpMode*)arg) = (PSP_MmcsdOpMode)(devDDC->opMode);
            }
            break;

            case PSP_MMCSD_IOCTL_SET_CALLBACK:
            {
                PSP_MmcsdAppCallback *cb = (PSP_MmcsdAppCallback *)arg;
                if (NULL == channel->appCallback)
                {
                    retVal = IOM_EBADARGS;
                    break;
                }
                channel->appCallback = *cb;
            }
            break;

            case PSP_MMCSD_IOCTL_SET_HWEVENT_NOTIFICATION:
            {
                Uint32  tmp = 0;
                tmp = (Uint32) arg;
                devDDC->hwEventNotification = \
                    ((PSP_MmcsdHwEventNotification)tmp);
            }
            break;

            case PSP_MMCSD_IOCTL_GET_HWEVENT_NOTIFICATION:
            {
                *(PSP_MmcsdHwEventNotification*)arg = \
                    devDDC->hwEventNotification;
            }
            break;

            case PSP_MMCSD_IOCTL_CHECK_HIGH_CAPACITY_CARD:
            {
                *(Bool*)arg = devDDC->isHighCapacitySDCard;
            }
            break;

            case PSP_MMCSD_IOCTL_GET_TOTAL_SECTORS:
            {
                *(Uint32*)arg = devDDC->sectorCnt;
            }
            break;

            case PSP_MMCSD_IOCTL_SET_EVENTQ:
            {
                Bool isIopPending;
                Uint32 temp;
                if ((PSP_MMCSD_OPMODE_DMAINTERRUPT !=
                    (PSP_MmcsdOpMode)(devDDC->opMode)))
                {
                    retVal = IOM_EBADARGS;
                    break;
                }
                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }

                retVal = mmcsdUnSetDmaMode(channel->hDDC);
                if (IOM_COMPLETED != retVal)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }
                temp = *(Uint32*)arg;
                devDDC->eventQ = (PSP_MmcsdEdmaEventQueue)temp;
                retVal = mmcsdSetDmaMode(channel->hDDC);
                if (IOM_COMPLETED != retVal)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }
            }
            break;

            case PSP_MMCSD_IOCTL_SET_CARD_FREQUENCY:
            {
                PSP_CardFrequency cardFreq = *(PSP_CardFrequency*)arg;
                Bool        isIopPending;
                isIopPending = QUE_empty(devDDC->listHeadNode) ? FALSE : TRUE;
                if (TRUE == isIopPending)
                {
                    retVal = IOM_EBADMODE;
                    break;
                }
                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    retVal = mmcsdCalculateFreqForCard(channel->hDDC,
                                 MMCSD_INIT_CLOCK,
                                 cardFreq,
                                 SD_CLOCK,
                                 SD_HIGH_SPEED_CLOCK);
                    if (IOM_COMPLETED == retVal)
                    {
                        LLC_mmcsdSetFrequency(devDDC->hLlc,
                            &(devDDC->hDrvrClock.mmcPushPull),
                            FALSE);
                    }
                }
                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    retVal = mmcsdCalculateFreqForCard(channel->hDDC,
                                 MMCSD_INIT_CLOCK,
                                 MMC_CLOCK,
                                 cardFreq,
                                 cardFreq);
                    if (IOM_COMPLETED == retVal)
                    {
                        LLC_mmcsdSetFrequency(devDDC->hLlc,
                            &(devDDC->hDrvrClock.sdPushPull),
                            FALSE);
                    }
                }
            }
            break;

            case PSP_MMCSD_IOCTL_GET_CARD_VENDOR:
                if (PSP_MMCSD_CARDTYPE_SD == devDDC->cardRegs.cardType)
                {
                    *(Uint32*)arg = (Uint32)(devDDC->cardRegs.sd.cid[13])<<16;
                    *(Uint32*)arg |= ((Uint32)(devDDC->cardRegs.sd.cid[14])<<8);
                    *(Uint32*)arg |= devDDC->cardRegs.sd.cid[15];
                }
                if (PSP_MMCSD_CARDTYPE_MMC == devDDC->cardRegs.cardType)
                {
                    *(Uint32*)arg = (Uint32)(devDDC->cardRegs.mmc.cid[13])<<16;
                    *(Uint32*)arg |= ((Uint32)(devDDC->cardRegs.mmc.cid[14])<<8);
                    *(Uint32*)arg |= devDDC->cardRegs.mmc.cid[15];
                }
                break;

            case PSP_MMCSD_IOCTL_GET_CONTROLLER_REG:
            {
                LLC_MmcsdObj* llc;
                Uint32 offset;
                llc = (LLC_MmcsdObj*)devDDC->hLlc;
                if (NULL == llc)
                {
                    retVal = IOM_EBADARGS;
                    break;
                }

                offset  = *(Uint32*)arg;
                if (((offset % 4U) != 0) || (offset > 0x74U))
                {
                    retVal = IOM_EBADARGS;
                    break;
                }
                if (NULL != param)
                {
                    *(Uint32*)param = \
                        *((volatile Uint32 *)((Uint8*)llc->regs + offset));
                }
                else
                {
                    retVal = IOM_EBADARGS;
                }
            }
            break;

            case PSP_MMCSD_IOCTL_SET_CONTROLLER_REG:
            {
                LLC_MmcsdObj* llc;
                Uint32 offset;
                llc = (LLC_MmcsdObj*)devDDC->hLlc;
                if (NULL == llc)
                {
                    retVal = IOM_EBADARGS;
                    break;
                }

                offset  = *(Uint32*)arg;
                if (((offset % 4U) != 0) || (offset > 0x74U))
                {
                    retVal = IOM_EBADARGS;
                    break;
                }
                if (NULL != param)
                {
                    *((volatile Uint32 *)((Uint8*)llc->regs + offset)) = \
                        *(Uint32*)param;
                }
                else
                {
                    retVal = IOM_EBADARGS;
                }
            }
            break;

            /*Klocwork throws "Code is unreachable" at this line which is
            not valid in this case*/
            default:
                retVal = IOM_ENOTIMPL;
                break;
        }
    }

    /* If the driver is power aware disable the PSC */
    if ((TRUE == channel->hDDC->oLlc.isPwrAware) && (TRUE == pscPwrOn))
    {
        pwrResult = mmcsdLpscOff(channel->hDDC);
        if (IOM_COMPLETED != pwrResult)
        {
            retVal = pwrResult;
        }
    }

    return retVal;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
