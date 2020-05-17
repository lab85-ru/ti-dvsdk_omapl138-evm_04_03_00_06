/*
 * Mcasp.c
 *
 * This file contains Driver Layer Interface implementation for the McASP Driver.
 * McASP Driver provides Driver Layer Interface to do operations on the McASP
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
 * \file      Mcasp.c
 *
 * \brief     Generic McASP Driver for DSP/BIOS Platform.It uses the EDMA for
 *            data transfer.
 *
 *            This file implements the McASP IOM driver to be used with
 *            DSP-BIOS operating system.
 *
 *            (C) Copyright 2008, Texas Instruments, Inc
 *
 * \author    Platform Support Group
 *
 * \revision
 *             0.2  [25-06-2008]  Added the support for non loop job mode
 *             0.1  [17-06-2008]  Created newly.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <string.h>
#include <assert.h>
#include <iom.h>
#include <hwi.h>
#include <c64.h>
#include <bcache.h>
#include <ecm.h>
#include <que.h>
#include <tsk.h>

#include <ti/sdo/edma3/drv/edma3_drv.h>
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/mcasp/Mcasp.h"
#include "McaspLocal.h"

#include <ti/pspiom/cslr/cslr_mcasp.h>

#ifdef CHIP_DM6437
    #include "ti/pspiom/cslr/soc_DM6437.h"
#endif

#ifdef CHIP_C6747
    #include "ti/pspiom/cslr/soc_C6747.h"
#endif

#ifdef CHIP_OMAPL137
    #include "ti/pspiom/cslr/soc_OMAPL137.h"
#endif

#ifdef CHIP_C6748
    #include "ti/pspiom/cslr/soc_C6748.h"
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
#endif


/* -------- constants -------- */

const struct Mcasp_Params Mcasp_PARAMS = {
    TRUE,  /* enablecache */
    (Uint32)0x0,  /* hwiNumber */
    FALSE,  /* isDataBufferPayloadStructure */
    {
        {
            (Uint32)0x0,  /* pfunc   */
            (Uint32)0x1,  /* pdir    */
            (Uint32)0x0,  /* ctl     */
            (Uint32)0x0,  /* ditCtl  */
            (Uint32)0x0,  /* dlbMode */
            (Uint32)0x0,  /* amute   */
            {
                (Uint32)0x0,  /* [0] */
                (Uint32)0x0,  /* [1] */
                (Uint32)0x0,  /* [2] */
                (Uint32)0x0,  /* [3] */
                (Uint32)0x0,  /* [4] */
                (Uint32)0x0,  /* [5] */
                (Uint32)0x0,  /* [6] */
                (Uint32)0x0,  /* [7] */
                (Uint32)0x0,  /* [8] */
                (Uint32)0x0,  /* [9] */
                (Uint32)0x0,  /* [10] */
                (Uint32)0x0,  /* [11] */
                (Uint32)0x0,  /* [12] */
                (Uint32)0x0,  /* [13] */
                (Uint32)0x0,  /* [14] */
                (Uint32)0x0,  /* [15] */
            },  /* serSetup */
        },  /* glb */
        {
            (Uint32)0xffffffff,  /* mask */
            (Uint32)0x0,  /* fmt */
            (Uint32)0x0,  /* frSyncCtl */
            (Uint32)0x1,  /* tdm */
            (Uint32)0x0,  /* intCtl */
            (Uint32)0x1ff,  /* stat */
            (Uint32)0x0,  /* evtCtl */
            {
                (Uint32)0x3,  /* clkSetupClk */
                (Uint32)0x8046,  /* clkSetupHiClk */
                (Uint32)0x0,  /* clkChk */
            },  /* clk */
        },  /* rx */
        {
            (Uint32)0xffffffff,  /* mask */
            (Uint32)0x0,  /* fmt */
            (Uint32)0x2,  /* frSyncCtl */
            (Uint32)0x1,  /* tdm */
            (Uint32)0x0,  /* intCtl */
            (Uint32)0x1ff,  /* stat */
            (Uint32)0x0,  /* evtCtl */
            {
                (Uint32)0x23,  /* clkSetupClk */
                (Uint32)0x8046,  /* clkSetupHiClk */
                (Uint32)0x0,  /* clkChk */
            },  /* clk */
        },  /* tx */
        (Uint32)0x1,  /* emu */
    },  /* mcaspHwSetup */
    FALSE                 /* pscPwrmEnable  */
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */
/**
 *  \brief  Array which is part of Mcasp Module State
 *
 */
static Bool inUse[CSL_MCASP_PER_CNT];

/*
 * \brief  Array to contain the Isr related information for all instances.
 */
static Mcasp_IsrObject isrInstanceObject[CSL_MCASP_PER_CNT];

/**
 *  \brief  Mcasp Module State Object
 */
static Mcasp_Module_State Mcasp_module = {&inUse[0],&isrInstanceObject[0]};

/**
 *  \brief  Array of Mcasp instance State objects array
 */
static Mcasp_Object Mcasp_Instances[CSL_MCASP_PER_CNT];

/**
 *  \brief  Array for device information per instance
 */
Mcasp_HwInfo Mcasp_deviceInstInfo[CSL_MCASP_PER_CNT];

#ifdef Mcasp_LOOPJOB_ENABLED
/**
 *  \brief  Destination loop buffer per instance
 *
 * \note   Buffer used during the loop job mode for the Transmit section
 */
Mcasp_TempBuffer Mcasp_loopDstBuf[CSL_MCASP_PER_CNT];
/**
 * \brief  Receive loop buffer per instance
 *
 * \note   Buffer used during the loop job mode for the Receive section
 */
Mcasp_TempBuffer Mcasp_loopSrcBuf[CSL_MCASP_PER_CNT];
#endif /* Mcasp_LOOPJOB_ENABLED */
/**
 *  \brief  Mute buffer per instance
 */
Mcasp_TempBuffer Mcasp_muteBuf[CSL_MCASP_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int mcaspMdBindDev(Ptr *devp, Int devId, Ptr devParams);
static Int mcaspMdUnBindDev(Ptr devp);
static Int mcaspMdCreateChan(
                    Ptr                 *chanp,
                    Ptr                 devp,
                    String              name,
                    Int                 mode,
                    Ptr                 chanParams,
                    IOM_TiomCallback    cbFxn,
                    Ptr                 cbArg
                    );
static Int mcaspMdDeleteChan(Ptr chanp);
static Int mcaspMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);
static Int mcaspMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs);


IOM_Fxns Mcasp_IOMFXNS =
{
    &mcaspMdBindDev,
    &mcaspMdUnBindDev,
    &mcaspMdControlChan,
    &mcaspMdCreateChan,
    &mcaspMdDeleteChan,
    &mcaspMdSubmitChan,
};


static Void mcaspInit(Uint32 instNum);

Void mcaspIsr(Arg arg);

Void mcaspIsrInput(Arg chanp);

Void mcaspIsrOutput(Arg chanp);

static Void mcaspUnRegisterInt(Uint16 devid, Uint16 evt,Int mode);

static Void mcaspBitSetGblCtl(Mcasp_Object *instHandle,Uint32 bitMaskVal);

static Void mcaspBitSetGblXCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal);

static Void mcaspBitRemoveGblRCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal);

static Void mcaspBitRemoveGblXCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal);

static Void mcaspMultiSerGlobalRestore(Ptr chanp);

static Void mcaspBitSetGblRCtl(Mcasp_Object *instHandle,Uint32 bitMaskVal);

static Int32 mcaspSubmitReq(Mcasp_ChannelHandle chanHandle,
                            IOM_Packet         *ioPacket);

static Void mcaspRegisterInt(Ptr                   chanp,
                             Uint32                instNum,
                             Uint16                evt,
                             ECM_Fxn intIsr);

static Void mcaspSetSerXmt(Mcasp_HwHandle       hMcasp,
                            Mcasp_SerializerNum  serNum);

static Void mcaspSetSerRcv(Mcasp_HwHandle       hMcasp,
                            Mcasp_SerializerNum  serNum);

static Int32 mcaspSetDitMode(Mcasp_HwHandle hMcasp,
                             Bool           ditFlag);

static Bool mcaspIsRequestedSerzsFree(Mcasp_Object *instHandle,
                                      Ptr           params);

static Int32 mcaspSetDitMode(Mcasp_HwHandle hMcasp,
                             Bool           ditFlag);

static Void mcaspMultiSerInitializationRcv(Mcasp_Object *instHandle,
                                           Ptr           params);

static Void mcaspMultiSerInitializationXmt(Mcasp_Object *instHandle,
                                           Ptr           params);

static Int32 mcaspConfigureXmtChannel(Mcasp_Object *instHandle,
                                      Ptr           params);

static Int32 mcaspConfigureRcvChannel(Mcasp_Object *instHandle,
                                      Ptr           params);

static Int32 mcaspWriteChanStatRam(Mcasp_HwHandle      hMcasp,
                                   Mcasp_ChStatusRam  *const chanStatRam);

static Int32 mcaspWriteUserDataRam(Mcasp_HwHandle      hMcasp,
                                   Mcasp_UserDataRam  *const userDataRam);

static Void mcaspConfigureFifo(Mcasp_HwObj         *hMcasp,
                               Mcasp_ChannelObj    *chanHandle,
                               Bool                 enableHwFifo);

static Int32  mcaspValidateBufferConfig(Mcasp_ChannelHandle  chanHandle,
                                        Mcasp_ChanParams    *const params);

static Void Mcasp_localIsrSwiFxn(Arg arg0,Arg arg1);

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
static Int32 McaspRegisterNotification(Mcasp_Object *instHandle);

static Int32 McaspUnregisterNotification(Mcasp_Object *instHandle);

static Int32 McaspUnregisterConstraints(Mcasp_Object *instHandle);

static PWRM_NotifyResponse McaspNotifyCallback(PWRM_Event    eventType,
                                               Arg           eventArg1,
                                               Arg           eventArg2,
                                               Arg           clientArg);

static PWRM_NotifyResponse McaspSuspendCurrentIops(Mcasp_Object  *instHandle,
                                                   PWRM_Event     eventType);

static Int32 McaspUnregisterConstraints(Mcasp_Object *instHandle);

#endif

#ifndef Mcasp_LOOPJOB_ENABLED
static Void Mcasp_swiTxFifo(Arg arg0,Arg arg1);
#endif
/* ========================================================================== */
/*                            GLOBAL VARIABLES                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */

/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */

/*
 * \brief   Initializes McASP IOM driver's data structures
 *
 *          This function initializes the McASP IOM driver's data structures
 *          including instance objects and channel objects.This function needs
 *          to be called at part of BIOS initialization by setting initFxn for
 *          that particular UDEV instance or by calling this function as part of
 *          user specific initFxn.
 *
 * \param   [IN]  none
 *
 * \return  None
 */
Void Mcasp_init(Void)
{
    Int i;

    for (i = 0; i < CSL_MCASP_PER_CNT; i++)
    {
        /* have to initialize statically */
        Mcasp_module.inUse[i] = FALSE;
        memset((Void *)&Mcasp_Instances[i], 0x0, sizeof(Mcasp_Object));

        Mcasp_module.isrObject[i].isIsrRegistered = FALSE;
        Mcasp_module.isrObject[i].chanEnabled[0] = FALSE;
        Mcasp_module.isrObject[i].chanEnabled[1] = FALSE;
        Mcasp_module.isrObject[i].instHandle = NULL;
        Mcasp_module.isrObject[i].isrSwiTaskHandle = NULL;

        if (i == 0)
        {
#if defined(CHIP_C6747) || defined (CHIP_OMAPL137)
            Mcasp_deviceInstInfo[i].ditSupport = FALSE;
#elif defined(CHIP_C6748) || defined (CHIP_OMAPL138)
            Mcasp_deviceInstInfo[i].ditSupport = TRUE;
#endif
            Mcasp_deviceInstInfo[i].baseAddress =
                (CSL_McaspRegs *)CSL_MCASP_0_CTRL_REGS;
            Mcasp_deviceInstInfo[i].fifoAddress =
                (CSL_AfifoRegs *)CSL_MCASP_0_FIFO_REGS;
            Mcasp_deviceInstInfo[i].dataAddress =
                (CSL_AdataRegs *)CSL_MCASP_0_DATA_REGS;
            Mcasp_deviceInstInfo[i].numSerializers = 16u;
#if defined(CHIP_C6747) || defined (CHIP_OMAPL137)
            Mcasp_deviceInstInfo[i].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_AXRINT;
#elif defined(CHIP_C6748) || defined (CHIP_OMAPL138)
            Mcasp_deviceInstInfo[i].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_MCASP0INT;
#endif
            Mcasp_deviceInstInfo[i].rxDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP0_RX;
            Mcasp_deviceInstInfo[i].txDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP0_TX;
#ifndef BIOS_PWRM_ENABLE
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_PSC_MCASP0;
#else
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_LPSC_NUMBER_MCASP_0;
#endif
            Mcasp_deviceInstInfo[i].pscInstance =
                (Uint32)CSL_LPSC_INST_MCASP_0;
        }
#if defined(CHIP_C6747) || defined (CHIP_OMAPL137)
        else if (i == 1)
        {
            Mcasp_deviceInstInfo[i].ditSupport = FALSE;
            Mcasp_deviceInstInfo[i].baseAddress =
                (CSL_McaspRegs *)CSL_MCASP_1_CTRL_REGS;
            Mcasp_deviceInstInfo[i].fifoAddress =
                (CSL_AfifoRegs *)CSL_MCASP_1_FIFO_REGS;
            Mcasp_deviceInstInfo[i].dataAddress =
                (CSL_AdataRegs *)CSL_MCASP_1_DATA_REGS;
            Mcasp_deviceInstInfo[i].numSerializers = 12u;
            Mcasp_deviceInstInfo[i].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_AXRINT;
            Mcasp_deviceInstInfo[i].rxDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP1_RX;
            Mcasp_deviceInstInfo[i].txDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP1_TX;
#ifndef BIOS_PWRM_ENABLE
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_PSC_MCASP1;
#else
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_LPSC_NUMBER_MCASP_1;
#endif
            Mcasp_deviceInstInfo[i].pscInstance =
                (Uint32)CSL_LPSC_INST_MCASP_1;        
        }
        else if (i == 2)
        {
            Mcasp_deviceInstInfo[i].ditSupport = TRUE;
            Mcasp_deviceInstInfo[i].baseAddress =
                (CSL_McaspRegs *)CSL_MCASP_2_CTRL_REGS;
            Mcasp_deviceInstInfo[i].fifoAddress =
                (CSL_AfifoRegs *)CSL_MCASP_2_FIFO_REGS;
            Mcasp_deviceInstInfo[i].dataAddress =
                (CSL_AdataRegs *)CSL_MCASP_2_DATA_REGS;
            Mcasp_deviceInstInfo[i].numSerializers = 4u;
            Mcasp_deviceInstInfo[i].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_AXRINT;
            Mcasp_deviceInstInfo[i].rxDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP2_RX;
            Mcasp_deviceInstInfo[i].txDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_MCASP2_TX;
#ifndef BIOS_PWRM_ENABLE
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_PSC_MCASP2;
#else
            Mcasp_deviceInstInfo[i].pwrmLpscId = 
                (Uint32)CSL_LPSC_NUMBER_MCASP_2;
#endif
            Mcasp_deviceInstInfo[i].pscInstance =
                (Uint32)CSL_LPSC_INST_MCASP_2;
        }
#endif  /* defined(CHIP_C6747) || defined (CHIP_OMAPL137) */
        else
        {
            /* do nothing                                                     */
        }
    }

    /* intialise the loop job buffers and the mute buffers for all instances  */
#ifdef Mcasp_LOOPJOB_ENABLED
    memset((Void *)Mcasp_loopDstBuf, 0x0,
           sizeof(Mcasp_TempBuffer) * CSL_MCASP_PER_CNT);
    memset((Void *)Mcasp_loopSrcBuf, 0x0,
           sizeof(Mcasp_TempBuffer) * CSL_MCASP_PER_CNT);
#endif /* Mcasp_LOOPJOB_ENABLED */
    memset((Void *)Mcasp_muteBuf, 0x0,
           sizeof(Mcasp_TempBuffer) * CSL_MCASP_PER_CNT);
}

/*
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp        [OUT] pointer to hold the device Handle
 *  \param  devId       [IN]  Instance number of the Mcasp to create
 *  \param  devParams   [IN]  Pointer to the user supplied device parameters
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID         in case of error
 *
 *
 *  \enter  devp        should be non NULL and valid pointer,
 *          devId       should be < CSL_MCASP_PER_CNT
 *          devParams   should be non NULL and valid pointer,
 *
 *  \leave  Not implemented
 */
static Int mcaspMdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Mcasp_Object   *instHandle = NULL;
    Mcasp_Params   *params     = NULL;
    Int32           status     = IOM_COMPLETED;
    Uint32          count      = 0;
    Uint32          key        = 0;
    SWI_Attrs       swiParams  = {0};

    if (NULL == devParams)
    {
        params = (Mcasp_Params*)&Mcasp_PARAMS;
    }
    else
    {
        params = (Mcasp_Params*) devParams;
    }

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (CSL_MCASP_PER_CNT <= devId)
    {
        status = IOM_EBADARGS;
    }

    if (IOM_COMPLETED == status)
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        instHandle =  &Mcasp_Instances[devId];
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (TRUE == Mcasp_module.inUse[devId])
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        /* critical section starts                                            */
        key = (Uint32)_disable_interrupts();

        Mcasp_module.inUse[devId] = TRUE;
        instHandle->devState = Mcasp_DriverState_CREATED;

        /* critical section ends                                              */
        _restore_interrupts(key);

        /* initialise the data structures                                     */
        instHandle->instNum       = (Uint16)devId;
        instHandle->enablecache   = params->enablecache;
        instHandle->hwiNumber     = params->hwiNumber;
        instHandle->pscPwrmEnable = params->pscPwrmEnable;

        /* stop the state machine of RX and TX                                */
        instHandle->stopSmFsXmt = TRUE;
        instHandle->stopSmFsRcv = TRUE;

        instHandle->retryCount = Mcasp_POLLED_RETRYCOUNT;

        instHandle->isDataBufferPayloadStructure =
            params->isDataBufferPayloadStructure;

        /* configure the mcasp hardware info                                  */
        instHandle->mcaspHwSetup = params->mcaspHwSetup;

        /* copy the soc related information                                   */
        instHandle->hwInfo.regs =
            Mcasp_deviceInstInfo[devId].baseAddress;
        instHandle->hwInfo.fifoRegs =
            Mcasp_deviceInstInfo[devId].fifoAddress;
        instHandle->hwInfo.dataAddr =
            Mcasp_deviceInstInfo[devId].dataAddress;
        instHandle->hwInfo.numOfSerializers =
            Mcasp_deviceInstInfo[devId].numSerializers;
        instHandle->hwInfo.edmaTxEventNum =
            Mcasp_deviceInstInfo[devId].txDmaEventNumber;
        instHandle->hwInfo.edmaRxEventNum =
            Mcasp_deviceInstInfo[devId].rxDmaEventNumber;
        instHandle->hwInfo.cpuTxEventNum =
            Mcasp_deviceInstInfo[devId].cpuEventNumber;
        instHandle->hwInfo.cpuRxEventNum =
            Mcasp_deviceInstInfo[devId].cpuEventNumber;
        instHandle->hwInfo.ditStatus =
            Mcasp_deviceInstInfo[devId].ditSupport;
        instHandle->hwInfo.pscInstance =
            Mcasp_deviceInstInfo[devId].pscInstance;
        instHandle->hwInfo.pwrmLpscId =
            Mcasp_deviceInstInfo[devId].pwrmLpscId;

        instHandle->hwInfo.perNo = (Int16)instHandle->instNum;

        /* configure the default values for the transmit channel              */
        instHandle->XmtObj.chanState = Mcasp_DriverState_CLOSED;
        instHandle->XmtObj.mode  = (Uint32)IOM_OUTPUT;
        instHandle->XmtObj.devHandle = NULL;
        instHandle->XmtObj.cbFxn = NULL;
        instHandle->XmtObj.cbArg = NULL;
        QUE_new(&(instHandle->XmtObj.queueReqList));
        QUE_new(&(instHandle->XmtObj.queueFloatingList));
        instHandle->XmtObj.noOfSerAllocated = 0;
        instHandle->XmtObj.channelOpMode = Mcasp_ChanMode_XMT_DIT;
        instHandle->XmtObj.isDmaDriven = TRUE;
        instHandle->XmtObj.dataQueuedOnReset = 0;
        instHandle->XmtObj.intStatus = 0;
        instHandle->XmtObj.dataPacket = NULL;
        instHandle->XmtObj.tempPacket = NULL;
        instHandle->XmtObj.isTempPacketValid = FALSE;
        instHandle->XmtObj.userDataBufferSize = 0;
        instHandle->XmtObj.submitCount = -1;
        instHandle->XmtObj.edmaHandle = NULL;
        instHandle->XmtObj.xferChan = 0;
        instHandle->XmtObj.tcc = 0;
        instHandle->XmtObj.pramTbl[0] = 0;
        instHandle->XmtObj.pramTbl[1] = 0;
        instHandle->XmtObj.pramTblAddr[0] = 0;
        instHandle->XmtObj.pramTblAddr[1] = 0;
        instHandle->XmtObj.nextLinkParamSetToBeUpdated = 0;
        instHandle->XmtObj.loopjobUpdatedinParamset = FALSE;
        instHandle->XmtObj.cpuEventNum = 0;
        instHandle->XmtObj.xferinProgressIntmode = FALSE;
        instHandle->XmtObj.loopJobBuffer = NULL;
        instHandle->XmtObj.loopJobLength = 0;
        instHandle->XmtObj.roundedWordWidth = 0;
        instHandle->XmtObj.currentDataSize =0;
        instHandle->XmtObj.bMuteON = FALSE;
        instHandle->XmtObj.paused = FALSE;
        instHandle->XmtObj.edmaCallback = NULL;
        instHandle->XmtObj.gblErrCbk  = NULL;
        instHandle->XmtObj.enableHwFifo = TRUE;
        instHandle->XmtObj.nextFlag = FALSE;
        instHandle->XmtObj.currentPacketErrorStatus = IOM_COMPLETED;
        instHandle->XmtObj.noOfSlots = 0;
        instHandle->XmtObj.isDataPacked = FALSE;
        instHandle->XmtObj.dataFormat = Mcasp_BufferFormat_1SER_1SLOT;
        instHandle->XmtObj.userLoopJob = FALSE;

        instHandle->RcvObj.chanState = Mcasp_DriverState_CLOSED;
        instHandle->RcvObj.mode   = (Uint32)IOM_INPUT;
        instHandle->RcvObj.devHandle = NULL;
        instHandle->RcvObj.cbFxn  = NULL;
        instHandle->RcvObj.cbArg  = NULL;
        QUE_new(&(instHandle->RcvObj.queueReqList));
        QUE_new(&(instHandle->RcvObj.queueFloatingList));
        instHandle->RcvObj.noOfSerAllocated = 0;
        instHandle->RcvObj.channelOpMode = Mcasp_ChanMode_RCV;
        instHandle->RcvObj.isDmaDriven = TRUE;
        instHandle->RcvObj.dataQueuedOnReset =0;
        instHandle->RcvObj.intStatus = 0;
        instHandle->RcvObj.dataPacket = NULL;
        instHandle->RcvObj.tempPacket = NULL;
        instHandle->RcvObj.isTempPacketValid = FALSE;
        instHandle->RcvObj.userDataBufferSize = 0;
        instHandle->RcvObj.submitCount = -1;
        instHandle->RcvObj.edmaHandle = NULL;
        instHandle->RcvObj.xferChan = 0;
        instHandle->RcvObj.tcc = 0;
        instHandle->RcvObj.pramTbl[0] = 0;
        instHandle->RcvObj.pramTbl[1] = 0;
        instHandle->RcvObj.pramTblAddr[0] = 0;
        instHandle->RcvObj.pramTblAddr[1] = 0;
        instHandle->RcvObj.nextLinkParamSetToBeUpdated = 0;
        instHandle->RcvObj.loopjobUpdatedinParamset = FALSE;
        instHandle->RcvObj.cpuEventNum = 0;
        instHandle->RcvObj.xferinProgressIntmode = FALSE;
        instHandle->RcvObj.loopJobBuffer = NULL;
        instHandle->RcvObj.loopJobLength = 0;
        instHandle->RcvObj.roundedWordWidth = 0;
        instHandle->RcvObj.currentDataSize =0;
        instHandle->RcvObj.bMuteON = FALSE;
        instHandle->RcvObj.paused = FALSE;
        instHandle->RcvObj.edmaCallback = NULL;
        instHandle->RcvObj.gblErrCbk  = NULL;
        instHandle->RcvObj.enableHwFifo = TRUE;
        instHandle->RcvObj.nextFlag = FALSE;
        instHandle->RcvObj.noOfSlots = 0;
        instHandle->RcvObj.isDataPacked = FALSE;
        instHandle->RcvObj.dataFormat = Mcasp_BufferFormat_1SER_1SLOT;
        instHandle->RcvObj.currentPacketErrorStatus = IOM_COMPLETED;
        instHandle->RcvObj.userLoopJob = FALSE;

        for (count = 0; count < Mcasp_NUMSERIALIZERS; count++)
        {
            instHandle->serStatus[count] = Mcasp_SerializerStatus_FREE;
            instHandle->XmtObj.indexOfSersRequested[count] = 0;
            instHandle->RcvObj.indexOfSersRequested[count] = 0;
        }

#ifdef Mcasp_LOOPJOB_ENABLED
        /* driver is compiled in loop Job mode                                */
        instHandle->loopJobMode = TRUE;
        
        if (TRUE ==  instHandle->pscPwrmEnable)
        {
            /* power management not supported for loop job mode               */
            status = IOM_EBADMODE;
        }
#else
        instHandle->loopJobMode = FALSE;
#endif
        if ((IOM_COMPLETED == status) && 
            (FALSE == instHandle->pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            status = (Int32)PWRM_setDependency(
                                (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);
#else
            /* power on using PSC API                                         */
            status = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->hwInfo.pscInstance,
                         (Uint32)instHandle->hwInfo.pwrmLpscId,
                         TRUE);
#endif
        }

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
        if ((IOM_COMPLETED == status) && (TRUE == instHandle->pscPwrmEnable))
        {
            memset(&instHandle->pwrmInfo,0x00,sizeof(Mcasp_pwrmInfo));

            /* register the notify function for the PWRM events               */
            status = McaspRegisterNotification(instHandle);
        }
#endif

        if (IOM_COMPLETED == status)
        {
            /* power on the module                                            */
            status = Mcasp_localLpscOn(instHandle);

            if (IOM_COMPLETED == status)
            {
                Mcasp_localSetupHwInfo(
                    &instHandle->hwInfo,
                    &instHandle->mcaspHwSetup);

                status = Mcasp_localLpscOff(instHandle);
            }
        }

        /* we need to check if any one of the instance has the Swi created    *
         * else we will create the swi here                                   */
        for (count = 0; count < CSL_MCASP_PER_CNT; count++)
        {
            if (NULL != Mcasp_module.isrObject[count].isrSwiTaskHandle)
            {
                break;
            }
        }

        if (count == CSL_MCASP_PER_CNT)
        {
            /* modify the default parameters with the required params         */
            swiParams.priority = Mcasp_SWI_PRIORITY;
            swiParams.fxn = (SWI_Fxn)&Mcasp_localIsrSwiFxn;

            /* create the swi that will be used for ISR handling              */
            SWI_new(&instHandle->isrSwiObject,&swiParams);

            /* copy the swi Handle information to all the module objects      */
            for (count = 0; count < CSL_MCASP_PER_CNT; count++)
            {
                Mcasp_module.isrObject[count].isrSwiTaskHandle = \
                    &instHandle->isrSwiObject;
            }
        }

        if (IOM_COMPLETED == status)
        {
#ifndef Mcasp_LOOPJOB_ENABLED
            instHandle->loopJobMode = FALSE;

            /* create the swi to handle the TX FIFO empty case handling       */
            swiParams.priority = Mcasp_SWI_PRIORITY;
            swiParams.fxn = (SWI_Fxn)&Mcasp_swiTxFifo;
            swiParams.arg0 = (Arg)&instHandle->XmtObj;
            swiParams.arg1 = (Arg)instHandle;

            SWI_new(&instHandle->fifoSwiObject,&swiParams);
#endif
        }

        if (IOM_COMPLETED == status)
        {
             instHandle->devState = Mcasp_DriverState_CREATED;
            *devp = (Ptr)instHandle;
        }
        else
        {
            *devp = NULL;
        }
    }
    return (status);
}

/**
 *  \brief  Closes the McASP device from use.
 *
 *  This function is called when an previously opened channel is to be closed.
 *  it unallocates all the resources allocated during the instance creation
 *  and marks the instance as free for allocation
 *
 *  \param  devp  [IN]  Device handle (returned during device creation)
 *
 *  \return IOM_COMPLETED   if successful
 *          IOM error       if not successful
 *
 *  \enter  instHandle    is not NULL and is a valid pointer,
 *          status        is a valid value.
 *
 *  \leave  Not Implemented
 */
static Int mcaspMdUnBindDev(Ptr devp)
{
    Mcasp_Object       *instHandle = NULL;
    Int32               retVal     = IOM_COMPLETED;
    Uint32              key        = 0;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == devp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        instHandle = (Mcasp_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((CSL_MCASP_PER_CNT <= instHandle->instNum)                  ||
            (Mcasp_DriverState_CLOSED != instHandle->XmtObj.chanState)  ||
            (Mcasp_DriverState_CLOSED != instHandle->RcvObj.chanState))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* Close and free mcasp port handle                                   */
        if (NULL != &(instHandle->hwInfo))
        {
            instHandle->hwInfo.regs    = (Ptr)NULL;
            instHandle->hwInfo.perNo   = (Int16)-1;
            instHandle->hwInfo.numOfSerializers = 0;
            instHandle->hwInfo.ditStatus = FALSE;
        }

        /* critical section starts                                            */
        key = (Uint32)_disable_interrupts();

        /* Mark driver state as deleted and module as not in use              */
        instHandle->devState = Mcasp_DriverState_DELETED;
        Mcasp_module.inUse[instHandle->instNum] = FALSE;

        /* critical section ends                                              */
        _restore_interrupts(key);

        if (FALSE == instHandle->pscPwrmEnable)
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            retVal = (Int32)PWRM_releaseDependency(
                                (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);
#else
            /* power on using PSC API                                         */
            retVal = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->hwInfo.pscInstance,
                         (Uint32)instHandle->hwInfo.pwrmLpscId,
                         FALSE);
#endif
        }

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
        if ((IOM_COMPLETED == retVal) && (TRUE == instHandle->pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            retVal = McaspUnregisterNotification(instHandle);
        }
#endif        
    }
    return (retVal);
}
/* ========================================================================== */
/*                             IOM FUNCTIONS                                  */
/* ========================================================================== */

/**
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the McASP device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a Stream_create call.
 *
 *          Pre-requisites:
 *          1.  Valid chanParams structure
 *              This takes much information pertaining to mcasp channel
 *              configuration such as how many serialisers are used for this
 *              channel what are they communication parameters,clock setting etc
 *          2.  Valid device pointer
 *
 *  \param  instHandle   [IN]   Mcasp driver object
 *  \param  name         [IN]   Mcasp Instance name like MCASP0
 *  \param  mode         [IN]   channel  mode -> input or output
 *  \param  chanParams   [IN]   channel parameters from user
 *  \param  cbFxn        [IN]   callback function pointer
 *  \param  cbArg        [IN]   callback function Arguments
 *
 *  \return channel handle in case of success
 *          NULL   in case of failure
 *
 *  \enter  instHandle   is not NULL and is a valid pointer,
 *          chanp        is not NULL and is a valid pointer,
 *          cmd          is a valid IOCTL command,
 *          cmdArgs      are valid and appropriate for the command passed,
 *
 *  \leave  Not implemented
 */
static Int mcaspMdCreateChan(Ptr             *chanp,
                             Ptr              devp,
                             String           name,
                             Int              mode,
                             Ptr              chanParams,
                             IOM_TiomCallback cbFxn,
                             Ptr              cbArg)
{
    Mcasp_Object           *instHandle  = NULL;
    Mcasp_ChannelHandle     chanHandle  = NULL;
    Mcasp_ChanParams       *chanparam   = NULL;
    Uint32                  key         = 0;
    Uint8                   count       = 0;
    Uint32                  mask        = 0;
    Uint32                  mod         = 0;
    Bool                    falsewhile  = TRUE;
    Bool                    pscPwrOn    = FALSE;
    Int32                   retVal      = IOM_COMPLETED;
#ifdef Mcasp_LOOPJOB_ENABLED
    Uint32                  dlbMode     = 0;
    EDMA3_DRV_TrigMode      trigMode    = EDMA3_DRV_TRIG_MODE_EVENT;
#endif
    Int                     status      = IOM_COMPLETED;

    /* Done to remove compiler warning of unused param                        */
    if (NULL != name)
    {
        name = name;
    }

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (((NULL == chanParams)
        || (NULL == cbFxn)
        || (NULL == cbArg)
        || (NULL == devp))
        || ((IOM_INPUT != mode) && (IOM_OUTPUT != mode)))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanparam = (Mcasp_ChanParams *)chanParams;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == chanparam->edmaHandle) && (TRUE == chanparam->isDmaDriven))
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        instHandle = (Mcasp_Object *)devp;

        do
        {
            falsewhile = FALSE;

            /* power on the module                                            */
            status = Mcasp_localLpscOn(instHandle);

            if (IOM_COMPLETED != status)
            {
                /* Unable to power ON the module,hence break out              */
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }

            /* initialise buffers required by the mcasp for mute and loop job */
            mcaspInit((Uint32)instHandle->instNum);

            /* check the current mode of operation and assign suitable handle */
            if (IOM_INPUT == mode)
            {
                chanHandle = (Mcasp_ChannelHandle)&instHandle->RcvObj;
            }
            else
            {
                chanHandle = (Mcasp_ChannelHandle)&instHandle->XmtObj;
            }

            /* enter critical section and update the status of driver         */
            key = (Uint32)_disable_interrupts();

            /* check if the handle to the channel is valid                    */
            if ((NULL == chanHandle) ||
                (Mcasp_DriverState_OPENED == chanHandle->chanState))
            {
                status = IOM_EBADARGS;

                /* Leave critical section                                     */
                _restore_interrupts(key);
                break;
            }

            /* update the driver status as being opened in case of failure    *
             * status needs to be reverted back                               */
            chanHandle->chanState = Mcasp_DriverState_OPENED;

            /* Leave critical section                                         */
            _restore_interrupts(key);

            /* update the channel operational mode  (i.e TX or RX)            */
            chanHandle->mode = (Uint32)mode;

            /* update the device handle in the channel object structure       */
            chanHandle->devHandle = (Ptr)instHandle;

            /* update the IOM callback function in channel structure          */
            chanHandle->cbFxn = cbFxn;
            chanHandle->cbArg = (Arg)cbArg;

            /* check if requested serialisers are more than device supports.  *
             * This is a first level check, later we check what serialisers   *
             *are remaining and if they match the requested serialisers number*/
            if ((chanparam->noOfSerRequested) >
                (instHandle->hwInfo.numOfSerializers))
            {
                status = IOM_EBADARGS;
                break;
            }

            /* check if  mode (transmit or receive mode) is DIT or TDM        */
            if ((Mcasp_OpMode_TDM != chanparam->channelMode)
                && (Mcasp_OpMode_DIT != chanparam->channelMode))
            {
                status = IOM_EBADARGS;
                break;
            }

            /* validate params for the DIT mode and set an error if incorrect */
            if ((Mcasp_OpMode_DIT == chanparam->channelMode)
                && ((IOM_OUTPUT != mode)
                || (Mcasp_WordLength_32 != chanparam->wordWidth)))
            {
                status = IOM_EBADARGS;
                break;
            }

            if ((Mcasp_OpMode_DIT == chanparam->channelMode)
                && (FALSE == instHandle->isDataBufferPayloadStructure))
            {
                status = IOM_EBADARGS;
                break;
            }

            /* if this instance is not supporting DIT mode then return error  */
            if ((Mcasp_OpMode_DIT == chanparam->channelMode)
                && (FALSE == instHandle->hwInfo.ditStatus))
            {
                status = IOM_EBADARGS;
                break;
            }

            /* check if the cache is to be supported and flush the cache      *
             * if the cache support is enabled                                */
            if (TRUE == instHandle->enablecache)
            {
                BCACHE_wb(Mcasp_muteBuf[instHandle->instNum].scratchBuffer,
                          Mcasp_CACHE_LENGTH,
                          TRUE);
#ifdef Mcasp_LOOPJOB_ENABLED
                BCACHE_wb(Mcasp_loopDstBuf[instHandle->instNum].scratchBuffer,
                          Mcasp_CACHE_LENGTH,
                          TRUE);

                BCACHE_wb(Mcasp_loopSrcBuf[instHandle->instNum].scratchBuffer,
                          Mcasp_CACHE_LENGTH,
                          TRUE);
#endif /* Mcasp_LOOPJOB_ENABLED */
            }

            /* This is used in EDMA mode to populate paramsets in PING PONG   */
            chanHandle->nextLinkParamSetToBeUpdated = 0;

            chanHandle->dataPacket    = NULL;
            chanHandle->currentPacketErrorStatus = IOM_COMPLETED;

            /* Initialize Mute parameter                                      */
            chanHandle->bMuteON = FALSE;

            /* Initialize Pause parameter                                     */
            chanHandle->paused = FALSE;

            /* Check requested serializers are free or not                    */
            if (FALSE == mcaspIsRequestedSerzsFree(instHandle, chanparam))
            {
                /* since serializers cannot be allocated, return error        */
                chanHandle->channelOpMode = Mcasp_ChanMode_FREE;
                status = IOM_EBADARGS;
                break;
            }

            /* Since we have request serialisers as free, we allocate them to *
             * this channel as requested                                      */
            for (count = 0; count < chanparam->noOfSerRequested; count++)
            {
                chanHandle->indexOfSersRequested[count] =
                    chanparam->indexOfSersRequested[count];
            }

            /* update the number of serializers allocated to channel          */
            chanHandle->noOfSerAllocated = chanparam->noOfSerRequested;

            /* Counter that counts outstanding requests of this channel       */
            chanHandle->submitCount = 0;

            /* Global error callback registered to channel                    */
            chanHandle->gblErrCbk = chanparam->gblCbk;

            /* We need to have infomation on what application intends to do.  *
             * for e.g. if application is sending 12bit data and formats it   *
             * in such a way the it in stream it occupies 2 bytes,this members*
             * value should be 16 (Mcasp_WordLength_16). By defaut we assume  *
             * that app has formated the data byte stream for 4 bytes for     *
             * each serialser for each sync event/int from mcasp. Please note *
             * that for burst mode we have 1 sync event/int from mcasp for    *
             * both L and R (in general all slots) where as we have each sync *
             * event /Int from mcasp for L and R (in general all slots)       *
             * seperately                                                     */
            chanHandle->roundedWordWidth = (Uint16)Mcasp_WordLength_32;

            /* we have 3 options for chanHandle->roundedWordWidth variable    *
             * 32,16 & 8 so we start from 32 (bit 5) and check if bit5 , 4,   *
             * or 3 is set if  param->wordWidth is invalid - we assume the    *
             * default of 32bit width.The following logic translates the      *
             * intermediate bit widths to what this driver wants. It assumes  *
             * that always the data streams is aligned to 8 ,16, 32 bits      *
             ( rounded to ceil)                                               */

            if (Mcasp_WordLength_32 > chanparam->wordWidth)
            {
                mask = 0x20u;

                for (count = 0; count < (3u); count++)
                {
                    if (mask == (mask &  chanparam->wordWidth))
                    {
                        chanHandle->roundedWordWidth = (Uint16)mask;

                        if (chanparam->wordWidth > chanHandle->roundedWordWidth)
                        {
                            chanHandle->roundedWordWidth=
                                (Uint16)(chanHandle->roundedWordWidth << 1);
                        }
                        break;
                    }
                    mask= (Uint32)(mask >> 1);
                }
            }

            /* we will overide the above setting if application has enabled   *
             * the packed mode support                                        */
            if (TRUE == chanparam->isDataPacked)
            {

                switch (chanparam->wordWidth)
                {
                    case Mcasp_WordLength_32:
                    case Mcasp_WordLength_24:
                    case Mcasp_WordLength_16:
                    case Mcasp_WordLength_8:
                        chanHandle->roundedWordWidth = chanparam->wordWidth;
                        break;

                    default:
                        status = IOM_EBADARGS;
                        break;
                }
            }

            if (0 == ((0x0000FF80u) & chanparam->mcaspSetup->frSyncCtl))
            {
                /*the driver has been configured for DSP (BURST mode)         */
                if (0 ==  chanparam->noOfChannels)
                {
                    status = IOM_EBADARGS;
                    break;
                }

                chanHandle->roundedWordWidth = (Uint16) \
                    (chanparam->noOfChannels * chanHandle->roundedWordWidth);

                /* rounded wordwidth can not go beyond 32 bits. In burst mode *
                 * multiplication of no of channels and wordwidth can be max. *
                 * 32 bits. Beyond that it is not supported                   */
                if (Mcasp_WordLength_32 < chanHandle->roundedWordWidth)
                {
                    status = IOM_EBADARGS;
                    break;
                }
            }

            if (IOM_COMPLETED
                != mcaspValidateBufferConfig(chanHandle, chanparam))
            {
                status = IOM_EBADARGS;
                break;
            }
            /* Converting word width from bits to bytes                       */
            chanHandle->roundedWordWidth =  \
                (Uint16)(chanHandle->roundedWordWidth >> 3);

            /* Approaches part ways.For interrupt and DMA modes               */
            if (TRUE == chanparam->isDmaDriven)
            {
                /* Store this info in the channel handle too                  */
                chanHandle->isDmaDriven = TRUE;

                /* store the EDMA3 module handle                              */
                chanHandle->edmaHandle = chanparam->edmaHandle;

                if (IOM_INPUT == chanHandle->mode)
                {
                    /* Receive mode */

                    /* Assign the Channel ID and TCC                          */
                    chanHandle->xferChan  = instHandle->hwInfo.edmaRxEventNum;
                    chanHandle->tcc = instHandle->hwInfo.edmaRxEventNum;
                    chanHandle->edmaCallback =  \
                        (EDMA3_RM_TccCallback)&Mcasp_localEdmaCallback;
                    chanHandle->cpuEventNum  = instHandle->hwInfo.cpuRxEventNum;

                }
                else
                {
                    /* Transmit mode */

                    /* Assign the Channel ID and TCC                          */
                    chanHandle->xferChan  = instHandle->hwInfo.edmaTxEventNum;
                    chanHandle->tcc   = instHandle->hwInfo.edmaTxEventNum;
                    chanHandle->edmaCallback =  \
                        (EDMA3_RM_TccCallback)&Mcasp_localEdmaCallback;
                    chanHandle->cpuEventNum  = instHandle->hwInfo.cpuTxEventNum;

                }
#ifdef Mcasp_LOOPJOB_ENABLED
                /* Configure loop job for the user specified buffer if given  */
                if (NULL == chanparam->userLoopJobBuffer)
                {
                    if (IOM_INPUT == chanHandle->mode)
                    {
                        chanHandle->loopJobBuffer = \
                          Mcasp_loopDstBuf[(instHandle->instNum)].scratchBuffer;
                    }
                    else
                    {
                        chanHandle->loopJobBuffer = \
                          Mcasp_loopSrcBuf[(instHandle->instNum)].scratchBuffer;
                    }

                    /* Length of Uint32 per serialiser-this is what we have   *
                     * allocated                                              */
                    if (Mcasp_OpMode_DIT == chanparam->channelMode)
                    {
                        /* we need 2 * 24 bit data per subframe. Hence we will*
                         * program the loop job length as 6 bytes             */
                        chanHandle->loopJobLength = 6u;
                    }
                    else
                    {
                        chanHandle->loopJobLength =
                            (chanHandle->roundedWordWidth
                                * chanHandle->noOfSlots
                                * chanHandle->noOfSerAllocated);
                    }
                    /* user loopJob is not being used                         */
                    chanHandle->userLoopJob = FALSE;
                }
                else
                {
                    /* Apps has preference on the loopjob buffer & lets use it*/
                    chanHandle->loopJobBuffer = chanparam->userLoopJobBuffer;
                    chanHandle->userLoopJobLength = chanparam->userLoopJobLength;

                    /* user loopJob is being used                             */
                    chanHandle->userLoopJob = TRUE;
                    if (chanHandle->roundedWordWidth >
                            chanparam->userLoopJobLength)
                    {
                        /* not enough loopjob buffer has been provided  we    *
                         * should have aleast loopbuffer for 1 sync event     */
                        status = IOM_EBADARGS;
                        break;
                    }
                }
#endif /* Mcasp_LOOPJOB_ENABLED */
                /* configure EDMA for this driver                             */
                if (IOM_COMPLETED !=
                    Mcasp_localSetupEdmaDuringOpen(chanHandle))
                {
                    status = IOM_EBADARGS;
                    break;
                }
            } /* if DMA driven*/
            else
            {
                /* Interrupt driven operation */
                /* Store this info too in the channel handle.                 */
                chanHandle->isDmaDriven = FALSE;

                if (IOM_INPUT == chanHandle->mode)
                {
                    chanHandle->cpuEventNum  = instHandle->hwInfo.cpuRxEventNum;
                }
                else
                {
                    chanHandle->cpuEventNum  = instHandle->hwInfo.cpuTxEventNum;
                }

                /* On being released from reset, the McASP xmt section will   *
                 * trigger two interrupts to fill it's buffers (this has been *
                 * handled in submitreq function.Keep count of packets to     *
                 * service this.                                              */
                chanHandle->dataQueuedOnReset = 0;
                chanHandle->xferinProgressIntmode = FALSE;
            }

            /* configure the FIFO control registers (only if requested)       */
            chanHandle->enableHwFifo = chanparam->enableHwFifo;
            mcaspConfigureFifo(&(instHandle->hwInfo),
                chanHandle,
                chanHandle->enableHwFifo);

            if (IOM_INPUT == mode)
            {
                /* receive channel */
                /* Reconfigure the RCV section                                */
                if (IOM_COMPLETED !=
                    mcaspConfigureRcvChannel(instHandle, chanparam))
                {
                    status = IOM_EBADARGS;
                    break;
                }

                /* Mark REQUESTED serializers as RECEIVE mode.This also       *
                 * configures PDIR pins as INPUT pin                          */
                mcaspMultiSerInitializationRcv(instHandle, chanparam);

                /* Set channel operational mode                               */
                chanHandle->channelOpMode= Mcasp_ChanMode_RCV;

            }
            else
            {
                /* transmit channel */
                /* Re-configure the XMT section                               */
                if (IOM_COMPLETED !=
                    mcaspConfigureXmtChannel(instHandle, chanparam))
                {
                    status = IOM_EBADARGS;
                    break;
                }

                /* Mark REQUESTED serializers as RECEIVE mode. Also configures*
                 * PDIR pins as INPUT pin                                     */
                mcaspMultiSerInitializationXmt(instHandle, chanparam);

                /* Set channel mode                                           */
                if (Mcasp_OpMode_DIT == chanparam->channelMode)
                {
                    /* Channel is to operate in DIT mode                      */
                    chanHandle->channelOpMode= Mcasp_ChanMode_XMT_DIT;
                }
                else
                {
                    /* Channel is to operate in normal TDM mode               */
                    chanHandle->channelOpMode= Mcasp_ChanMode_XMT_TDM;
                }

            }

            /* For burtsmode of operation frame sync error has to ignored     *
             * to indentify that we do the following                          */
            if (IOM_INPUT == chanHandle->mode)
            {
                mod = (Uint32)((instHandle->hwInfo.regs)->AFSRCTL
                           & CSL_MCASP_AFSRCTL_RMOD_MASK);

                /* Clear the framsync error flag from EVTCTLR                 */
                if (0 == mod)
                {
                    (instHandle->hwInfo.regs)->RINTCTL =
                        ((instHandle->hwInfo.regs)->RINTCTL
                            & ~(CSL_MCASP_RINTCTL_RSYNCERR_MASK))
                        | (CSL_MCASP_RINTCTL_RSYNCERR_RESETVAL
                            << CSL_MCASP_RINTCTL_RSYNCERR_SHIFT);
                }
            }
            else
            {
                mod = (Uint32)((instHandle->hwInfo.regs)->AFSXCTL
                           & CSL_MCASP_AFSXCTL_XMOD_MASK) ;

                /* Clear the framsync error flag from EVTCTLX                 */
                if (0 == mod)
                {
                    (instHandle->hwInfo.regs)->XINTCTL =
                        ((instHandle->hwInfo.regs)->XINTCTL
                                & ~(CSL_MCASP_XINTCTL_XSYNCERR_MASK))
                            | (CSL_MCASP_XINTCTL_XSYNCERR_RESETVAL
                                << CSL_MCASP_XINTCTL_XSYNCERR_SHIFT);
                }
            }

            /* Register the interrupts for events for both  DMA/Interrupt     *
             * modes  because in DMA mode also we will get interrupts for     *
             * errors                                                         */
            mcaspRegisterInt(
                    chanHandle,
                    (Uint32)instHandle->instNum,
                    (Uint16)chanHandle->cpuEventNum,
                    (ECM_Fxn)&mcaspIsr);

#ifdef Mcasp_LOOPJOB_ENABLED
            /*Starting DMA after starting clocks and before Serialiser SM FS  */
            if (TRUE == chanHandle->isDmaDriven)
            {
                /* When we get first packet let us start from linkparam set 0 */
                chanHandle->nextLinkParamSetToBeUpdated = 0;

                /* enable the DMA                                             */
                EDMA3_DRV_enableTransfer(
                    chanHandle->edmaHandle,
                    chanHandle->xferChan,
                    trigMode);

                /* if DLB mode is set then Enable both state m/c else rcv
                 * or xmt  state machine only needs to be enabled             */
                dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                            & CSL_MCASP_DLBCTL_DLBEN_MASK)
                            >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

                if (CSL_MCASP_DLBCTL_DLBEN_DISABLE == dlbMode)
                {
                    if (IOM_INPUT == mode)
                    {
                        /* Receive channel */
                        /* activate RX and TX state machine and frame sync    */
                        ECM_enableEvent(chanHandle->cpuEventNum);
                        Mcasp_localActivateSmFsForRcv(instHandle);
                    }
                    else
                    {
                        /* Transmit channel */
                        /* activate RX and TX state machine and frame sync    */
                        ECM_enableEvent(chanHandle->cpuEventNum);
                        Mcasp_localActivateSmFsForXmt(instHandle);
                    }
                }
                else
                {
                    /***        DLB Mode      ****/
                    /* activate RX and TX state machine and frame sync        */
                    ECM_enableEvent(instHandle->hwInfo.cpuRxEventNum);
                    ECM_enableEvent(instHandle->hwInfo.cpuTxEventNum);

                    Mcasp_localActivateSmFsForBoth(instHandle);
                }

            } /* dma driven */

            /* For interrupt mode actual start of mcasp starts in Submit      *
             * function and only when enough data packets are available. Also *
             * note that we have not implemented loopjobbuffer for this mode  */
            /*  update the State m/c stop status                              */
            instHandle->stopSmFsXmt = FALSE;
            instHandle->stopSmFsRcv = FALSE;
#endif  /* Mcasp_LOOPJOB_ENABLED */
        }while(falsewhile);

        if (TRUE == pscPwrOn)
        {
            /* power off the module                                           */
            retVal = Mcasp_localLpscOff(instHandle);
        
            if (IOM_COMPLETED == status)
            {
                status = retVal;
            }
        }
    }

    if (IOM_COMPLETED != status )
    {
        /* enter critical section                                             */
        key = (Uint32)_disable_interrupts();

        /* in case of an error we may not have the reference to channel handle*
         * hence we will explicitly assign it here                            */
        if (NULL != chanHandle)
        {
            /* call the delete function to free all the assigned resources    */
            mcaspMdDeleteChan(chanHandle);
        }

        /* Leave critical section                                             */
        _restore_interrupts(key);

        /* channel opening for transaction has failed                         */
        chanHandle = NULL;
    }
    else
    {
        *chanp = chanHandle;
    }
    return status;
}

/**
 *  \brief   Submit a I/O packet to a channel for processing
 *
 *  The stream layer calls this function to cause the IDriver
 *  to process the IOM_Packet for read/write operations.
 *  This function calls mcaspSubmitReq  function to execute the request.
 *
 *  \param   instHandle [IN]  pointer to mcasp instance object
 *  \param   chanp      [IN]  Handle to the channel
 *  \param   ioPacket   [IN]  Pointer to the packet to be submitted
 *  \Param   eb         [OUT] error block
 *
 *  \return  IOM_COMPLETED, if packet is fully processed
 *           IOM_PENDING,   if packet is not fully processed
 *           IOM_ERROR      if error in processing
 *
 *  \enter  instHandle    is not NULL and is a valid pointer,
 *          chanp         is not NULL and is a valid pointer,
 *          ioPacket      is not NULL and is a valid pointer,
 *
 *  \leave  Not implemented
 */
static Int mcaspMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Mcasp_ChannelHandle    chanHandle  = NULL;
    Mcasp_Object          *instHandle  = NULL;
    Int32                  status      = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == chanp) || (NULL == ioPacket))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

        chanHandle = (Mcasp_ChannelHandle)chanp;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == chanHandle) ||
             (Mcasp_DriverState_OPENED != chanHandle->chanState) ||
             (((IOM_READ == ioPacket->cmd) || (IOM_WRITE == ioPacket->cmd)) &&
            ((0 == ioPacket->size) || (NULL == ioPacket->addr))))
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    /* validate all the input parameters and set error if not OK              */
    if (IOM_COMPLETED == status)
    {
        /* if the command is for the ABORT or FLUSH                           */
        if ((IOM_ABORT == ioPacket->cmd) || (IOM_FLUSH == ioPacket->cmd))
        {
            status = Mcasp_localAbortRequests(chanHandle, NULL);
        }
        else if ((IOM_WRITE == ioPacket->cmd) || (IOM_READ == ioPacket->cmd))
        {
            instHandle = (Mcasp_Object *)chanHandle->devHandle;
            assert(NULL != instHandle);

            /* switch on the device in the PSC                                */
            status = Mcasp_localLpscOn(instHandle);

            if (IOM_COMPLETED == status)
            {
                /* submit the IO request to the driver                        */
                status = mcaspSubmitReq((Ptr)chanHandle,ioPacket);

                if (IOM_PENDING != status)
                {
                    status = Mcasp_localLpscOff(instHandle);
                }
            }
        }
        else
        {
            /* error: command not found                                       */
            status = IOM_EBADARGS;
        }
    }
    return status;
}

/**
 *  \brief   This function is called by the application to close a previously
 *           opened channel.it deletes the channel so that it is not available
 *           for further transactions. All the allocated resources are freed
 *           and the channel will be ready for the "open" operation once again.
 *
 *  \param   chanp      [IN]   Handle to the channel.
 *
 *  \return  None
 *
 *  \enter   instHandle    is not NULL and is a valid pointer,
 *           chanp         is not NULL and is a valid pointer,
 *
 *  \leave   Not implemented
 */
static Int mcaspMdDeleteChan(Ptr chanp)
{
    Mcasp_Object        *instHandle  = NULL;
    Mcasp_ChannelHandle  chanHandle  = NULL;
    Int32                status      = IOM_COMPLETED;
    Uint8                linkCnt     = 0;
    Uint32               hwiKey      = 0;
    Bool                 pscPwrOn    = FALSE;
    Int32                retVal      = IOM_COMPLETED;
    
/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Mcasp_ChannelHandle)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (NULL == chanHandle->devHandle)
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    /* check if the input parameters are valid                                */
    if (IOM_COMPLETED == status)
    {
        instHandle = chanHandle->devHandle;
        assert(NULL != instHandle);

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
        if (TRUE == instHandle->pscPwrmEnable)
        {
            status = McaspUnregisterConstraints(instHandle);
        }
#endif
        /* DMA driven transaction channel                                     */
        if (TRUE == chanHandle->isDmaDriven)
        {
            /* Unregister the interrupts registered                           */
            mcaspUnRegisterInt(
                instHandle->instNum,
                (Uint16)chanHandle->cpuEventNum,
                (Int)chanHandle->mode);

            status = Mcasp_localLpscOn(instHandle);

            if (IOM_COMPLETED == status)
            {
                pscPwrOn = TRUE;
            }

            if (IOM_COMPLETED == status)
            {
                /* Stop the xmt/rcv state machine.                            */
                if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
                {
                    /* Reset state m/c and frame sync of receive section      */
                    Mcasp_localDeactivateSmFsForRcv(instHandle);
                }
                else
                {
                    /* Reset state m/c and frame sync of transmit section     */
                    Mcasp_localDeactivateSmFsForXmt(instHandle);
                }
            }

            /* Disable the current transfer to make sure that there is        *
             * no running EDMA transfer taking place                          */
            if (IOM_COMPLETED == status)
            {
                status = EDMA3_DRV_disableTransfer(
                            chanHandle->edmaHandle,
                            chanHandle->xferChan,
                            EDMA3_DRV_TRIG_MODE_EVENT);
            }

            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADMODE;
                /* we will continue inspite of the error here                 */
            }

            status = EDMA3_DRV_freeChannel(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan);

            /* Free the allocated Edma channels                               */
            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADMODE;
                /* we will continue inspite of the error here                 */
            }

            /* free the EDMA PaRAM entries used for Pingpong buffering        */
            for (linkCnt = 0; linkCnt < Mcasp_MAXLINKCNT; linkCnt++)
            {
                EDMA3_DRV_freeChannel(
                    chanHandle->edmaHandle,
                    chanHandle->pramTbl[linkCnt]);
            }
        } /* DMA Driven flag is TRUE if loop */

        /* restore global serializer state.                                   */
        mcaspMultiSerGlobalRestore(chanHandle);

        if (IOM_OUTPUT== chanHandle->mode)
        {
            (instHandle->hwInfo.regs)->XGBLCTL = 0;
        }
        else
        {
            (instHandle->hwInfo.regs)->RGBLCTL = 0;
        }

        /* Set the Muted state to FALSE                                       */
        chanHandle->bMuteON = FALSE;

        /* Set the Pause state to FALSE                                       */
        chanHandle->paused = FALSE;

        /* start of critical section                                          */
        hwiKey = (Uint32)_disable_interrupts();

        /* Mark channel as ready to be allocated again.                       */
        chanHandle->chanState = Mcasp_DriverState_CLOSED;

        /* Leave critical section                                             */
        _restore_interrupts(hwiKey);

        chanHandle->noOfSerAllocated = 0;
        chanHandle->channelOpMode  = Mcasp_ChanMode_FREE;

        if (TRUE == pscPwrOn)
        {
            retVal = Mcasp_localLpscOff(instHandle);

            if (IOM_COMPLETED == status)
            {
                status = retVal;
            }
        }
    }
    return (status);
}


/**
 *  \brief    This function allows the control commands to be sent to the Mcasp
 *            driver. it internally calls the mcaspSubmitIoctl Function to
 *            perform the desired operation.
 *
 *  \param    instHandle [IN]    pointer to the Mcasp instance object
 *  \param    chanp      [IN]    Channel handle
 *  \param    cmd        [IN]    control command given by the application
 *  \param    cmdArgs    [IN]    Optional arguments required for the command
 *  \param    eb         [OUT]   error block
 *
 *  \return   None
 *
 *  \enter   instHandle   is not NULL and is a valid pointer,
 *           chanp        is not NULL and is a valid pointer,
 *           cmd          is a valid IOCTL command,
 *           cmdArgs      are valid and appropriate for the command passed,
 *
 *  \leave   Not implemented
 *
 */
static Int mcaspMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs)
{
    Mcasp_ChannelHandle  chanHandle = NULL;
    Mcasp_Object        *instHandle = NULL;
    Int32                status     = IOM_COMPLETED;
    Int32                retVal     = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL != chanp)
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Mcasp_ChannelHandle)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (Mcasp_DriverState_OPENED != chanHandle->chanState)
        {
            status = IOM_EBADARGS;
        }
        else if (NULL == chanHandle->devHandle)
        {
            status = IOM_EBADARGS;
        }
    }
    else
    {
        status = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    /* check if the channel handle passed is valid                            */
    if (IOM_COMPLETED == status)
    {
        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        status = Mcasp_localLpscOn(instHandle);

        if (IOM_COMPLETED == status)
        {
            /* channel status is OK Hence the IOCTL can be executed           */
            status = Mcasp_localSubmitIoctl(chanHandle,cmd,(Ptr)cmdArgs,NULL);

            retVal = Mcasp_localLpscOff(instHandle);

            if (IOM_COMPLETED == status)
            {
                status = retVal;
            }
        }
    }

    return (status);
}

/* ========================================================================== */
/*                              LOCAL FUNCTIONS                               */
/* ========================================================================== */

/**
 *  \brief  This function configures the  McASP instance registers as per the
 *          values passed in the hardware setup structure.
 *
 *  \param   hMcasp     [IN]    Handle to the McASP instance
 *  \param   myHwSetup  [IN]    Pointer to hardware setup structure
 *                             the query passed
 *
 *  \return  None
 *
 *  \enter   hMcasp      is a valid non null pointer
 *          myHwSetup   is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
Void Mcasp_localSetupHwInfo(Mcasp_HwHandle        hMcasp,
                             const Mcasp_HwSetup  *myHwSetup)
{
    Int16   serNum = 0;
    volatile Uint32 *pSrCtl = NULL;

    assert((NULL != hMcasp) && (NULL != myHwSetup) && (NULL != hMcasp->regs));

    /* Reset McASP to default values by setting GBLCTL = 0                    */
    (hMcasp->regs)->GBLCTL = 0x0000;

    /* Configure the RMASK register                                           */
    (hMcasp->regs)->RMASK =  (Uint32)myHwSetup->rx.mask;

    /* Configure RFMT                                                         */
    (hMcasp->regs)->RFMT  =  myHwSetup->rx.fmt;

    /* Configure AFSRCTL                                                      */
    (hMcasp->regs)->AFSRCTL =  myHwSetup->rx.frSyncCtl;

    /* Configure ACLKRCTL                                                     */
    (hMcasp->regs)->ACLKRCTL = myHwSetup->rx.clk.clkSetupClk;

    /* Configure AHCLKRCTL                                                    */
    (hMcasp->regs)->AHCLKRCTL =  myHwSetup->rx.clk.clkSetupHiClk;

    /* Configure RTDM                                                         */
    (hMcasp->regs)->RTDM =  myHwSetup->rx.tdm;

    /* Configure RINTCTL                                                      */
    (hMcasp->regs)->RINTCTL = myHwSetup->rx.intCtl;

    /* Configure RCLKCHK                                                      */
    (hMcasp->regs)->RCLKCHK =  myHwSetup->rx.clk.clkChk;

    /* Configure XMASK                                                        */
    (hMcasp->regs)->XMASK =  myHwSetup->tx.mask;

    /* Configure XFMT                                                         */
    (hMcasp->regs)->XFMT  =  myHwSetup->tx.fmt;

    /* Configure AFSXCTL                                                      */
    (hMcasp->regs)->AFSXCTL = myHwSetup->tx.frSyncCtl;

    /* Configure ACLKXCTL                                                     */
    (hMcasp->regs)->ACLKXCTL = myHwSetup->tx.clk.clkSetupClk;

    /* Configure AHCLKXCTL                                                    */
    (hMcasp->regs)->AHCLKXCTL = myHwSetup->tx.clk.clkSetupHiClk;

    /* Configure XTDM                                                         */
    (hMcasp->regs)->XTDM  = myHwSetup->tx.tdm;

    /* Configure XINTCTL                                                      */
    (hMcasp->regs)->XINTCTL = myHwSetup->tx.intCtl;

    /* Configure XCLKCHK                                                      */
    (hMcasp->regs)->XCLKCHK   = myHwSetup->tx.clk.clkChk;

    while (serNum < hMcasp->numOfSerializers)
    {
        pSrCtl = ((volatile Uint32 *)&((hMcasp->regs)->SRCTL0) + serNum);

        if (NULL != pSrCtl)
        {
            (*pSrCtl) =  (Uint32) myHwSetup->glb.serSetup[serNum];
            serNum++;
        }
        else
        {
            break;
        }
    }

    /* Configure pin function register                                        */
    (hMcasp->regs)->PFUNC = myHwSetup->glb.pfunc;

    /* Configure pin direction register                                       */
    (hMcasp->regs)->PDIR =  myHwSetup->glb.pdir;

    /* Configure DITCTL                                                       */
    if (TRUE == hMcasp->ditStatus)
    {
        (hMcasp->regs)->DITCTL =  myHwSetup->glb.ditCtl;
    }

    /* Configure DLBCTL                                                       */
    (hMcasp->regs)->DLBCTL =  myHwSetup->glb.dlbMode;

    /* Configure AMUTE                                                        */
    (hMcasp->regs)->AMUTE  =  myHwSetup->glb.amute;

    /* Configure RSTAT and XSTAT                                              */
    (hMcasp->regs)->RSTAT =  myHwSetup->rx.stat;

    (hMcasp->regs)->XSTAT =  myHwSetup->tx.stat;

    /* Configure REVTCTL and XEVTCTL                                          */
    (hMcasp->regs)->REVTCTL =  myHwSetup->rx.evtCtl;
    (hMcasp->regs)->XEVTCTL =  myHwSetup->tx.evtCtl;

    /* Initialize the global control register                                 */
    (hMcasp->regs)->GBLCTL =  myHwSetup->glb.ctl;

}


/**
 *  \brief   Aborts all the request queued up with the current channel
 *
 *  \param   chanHandle   [IN]   Handle to the mcasp channel
 *
 *  \return  None
 *
 *  \enter   chanHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
Int32 Mcasp_localAbortReset(Mcasp_ChannelHandle chanHandle)
{
    Mcasp_Object        *instHandle = NULL;
    IOM_Packet          *ioPacket   = NULL;
    Uint32               hwiKey     = 0;
    Bool                 falsewhile = FALSE;
    Uint32               localcCnt  = 0;
    Uint32               localbCnt  = 0;
    Bool                 firstReq   = TRUE;
    Int32                status     = IOM_COMPLETED;

    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    do
    {
        falsewhile = FALSE;

        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        assert(NULL != instHandle);

        /* This function services both transmit and receive channels. Define  *
         * critical section to avoid corruption of local variables.           *
         * CRITICAL SECTION: begins!                                          */
        hwiKey = (Uint32)_disable_interrupts();

        /* stop the xmt/rcv state machine.                                    */
        if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
        {
            /* Reset state m/c and frame sync of receive section              */
            Mcasp_localDeactivateSmFsForRcv(instHandle);
        }
        else
        {
            /* Reset state m/c and frame sync of transmit section             */
            Mcasp_localDeactivateSmFsForXmt(instHandle);
        }

        /* Disable EDMA  transfer                                             */
        EDMA3_DRV_disableTransfer(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_TRIG_MODE_EVENT);

        /* Clear the error bits of McASP due to stopping of peripheral.       */
        if (IOM_INPUT == chanHandle->mode)
        {
            (instHandle->hwInfo.regs)->RSTAT =
                ((instHandle->hwInfo.regs)->RSTAT
                    & ~(CSL_MCASP_RSTAT_RDMAERR_MASK))
                | (CSL_MCASP_RSTAT_RDMAERR_NO_ERROR
                    << CSL_MCASP_RSTAT_RDMAERR_SHIFT);

            (instHandle->hwInfo.regs)->RSTAT =
                ((instHandle->hwInfo.regs)->RSTAT
                    & ~(CSL_MCASP_RSTAT_ROVRN_MASK))
                | (CSL_MCASP_RSTAT_ROVRN_NO << CSL_MCASP_RSTAT_ROVRN_SHIFT);

            (instHandle->hwInfo.regs)->RSTAT =
                ((instHandle->hwInfo.regs)->RSTAT
                    & ~(CSL_MCASP_RSTAT_RSYNCERR_MASK))
                | (CSL_MCASP_RSTAT_RSYNCERR_NO
                    << CSL_MCASP_RSTAT_RSYNCERR_SHIFT);
        }
        else
        {
            (instHandle->hwInfo.regs)->XSTAT =
                ((instHandle->hwInfo.regs)->XSTAT
                    & ~(CSL_MCASP_XSTAT_XDMAERR_MASK))
                | (CSL_MCASP_XSTAT_XDMAERR_NOERROR
                    << CSL_MCASP_XSTAT_XDMAERR_SHIFT);

            (instHandle->hwInfo.regs)->XSTAT =
                ((instHandle->hwInfo.regs)->XSTAT
                    & ~(CSL_MCASP_XSTAT_XUNDRN_MASK))
                | (CSL_MCASP_XSTAT_XUNDRN_NO << CSL_MCASP_XSTAT_XUNDRN_SHIFT);

            (instHandle->hwInfo.regs)->XSTAT =
                ((instHandle->hwInfo.regs)->XSTAT
                    & ~(CSL_MCASP_XSTAT_XSYNCERR_MASK))
                | (CSL_MCASP_XSTAT_XSYNCERR_NO
                    << CSL_MCASP_XSTAT_XSYNCERR_SHIFT);
        }

        /* Get the current value of cCnt for the actual data transfered       */
        EDMA3_DRV_getPaRAMField(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_PARAM_FIELD_CCNT,
            &localcCnt);

        EDMA3_DRV_getPaRAMField(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_PARAM_FIELD_BCNT,
            &localbCnt);

        firstReq = TRUE;

        if (TRUE == chanHandle->isDmaDriven)
        {
            /* Empty the floating queue.                                      */
            while (TRUE != QUE_empty(&(chanHandle->queueFloatingList)))
            {
                ioPacket = (IOM_Packet *)  \
                                QUE_get(&(chanHandle->queueFloatingList));

                if (NULL == ioPacket)
                {
                    status = IOM_EBADIO;
                    /* Ideally control should not come here                   */
                    break;
                }

                /* set packet status equal to IOM_ABORTED                     */
                ioPacket->status = IOM_ABORTED;


                /* Actual data transfer will be equal to size provided by     *
                 * application i.e.userDataBufferSize reduced by current      *
                 * value of cCnt * aCnt * bCnt                                */
                if (TRUE == firstReq )
                {
                    ioPacket->size =
                        (Uint32)(ioPacket->size -
                        (localcCnt * chanHandle->roundedWordWidth *localbCnt));
                    firstReq = FALSE;
                }

                if (NULL != ioPacket)
                {
                    if (NULL !=  chanHandle->cbFxn)
                    {
                        /*  Application callback                              */
                        (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
                    }
                }
                /* decrement the submit count as request is aborted           */
                chanHandle->submitCount--;
            }
        }

        /* Empty the pendQUE queue.                                           */
        while (TRUE != QUE_empty(&(chanHandle->queueReqList)))
        {
            ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queueReqList));

            if (NULL != ioPacket)
            {
                /* set packet status equal to IOM_ABORTED                     */
                ioPacket->status = IOM_ABORTED;

                if (NULL !=  chanHandle->cbFxn)
                {
                    /*  Application callback                                  */
                    (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
                }
            }
            /* decrement the submit count as request is aborted */
            chanHandle->submitCount--;
        }

        /* As EDMA is disabled it might have thrown an error and set error    *
         * bit. Clear the error bit to enable the transfer again              */
        EDMA3_DRV_clearErrorBits(
            chanHandle->edmaHandle,
            chanHandle->xferChan);

        /* Configure the EDMA channel and EDMA param tables with intialization*
         * configuration as they are configured at the create time.           */
        chanHandle->nextLinkParamSetToBeUpdated = 0;
        chanHandle->loopjobUpdatedinParamset = TRUE;

        Mcasp_localEdmaChanPaRAMsetup(chanHandle);

#ifdef Mcasp_LOOPJOB_ENABLED
        /* Enable EDMA transfer, state machine and fram sync                  */
        EDMA3_DRV_enableTransfer(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_TRIG_MODE_EVENT);

        if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
        {
            /* Reset state m/c and frame sync of receive section              */
            Mcasp_localActivateSmFsForRcv(instHandle);
        }
        else
        {
            /* Reset state m/c and frame sync of transmit section             */
            Mcasp_localActivateSmFsForXmt(instHandle);
        }
#endif
        /* CRITICAL SECTION: ends!                                            */
        _restore_interrupts(hwiKey);

    }while(falsewhile);

    return (status);
}

/**
 *  \brief   This function activates the receive section state machine and
 *           the corresponding frame sync.
 *
 *  \param   instHandle  [IN]   Handle to the Mcasp instance object
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
Void Mcasp_localActivateSmFsForRcv(Mcasp_Object *instHandle)
{
    assert(NULL != instHandle);

    (instHandle->hwInfo.regs)->RSTAT = (CSL_MCASP_RSTAT_RERR_MASK
                                       | CSL_MCASP_RSTAT_RDMAERR_MASK
                                       | CSL_MCASP_RSTAT_RSTAFRM_MASK
                                       | CSL_MCASP_RSTAT_RDATA_MASK
                                       | CSL_MCASP_RSTAT_RLAST_MASK
                                       | CSL_MCASP_RSTAT_RTDMSLOT_MASK
                                       | CSL_MCASP_RSTAT_RCKFAIL_MASK
                                       | CSL_MCASP_RSTAT_RSYNCERR_MASK
                                       | CSL_MCASP_RSTAT_ROVRN_MASK);

    /*serializer*/
    mcaspBitSetGblRCtl(instHandle, CSL_MCASP_RGBLCTL_RSRCLR_MASK);

    /*state m/c*/
    mcaspBitSetGblRCtl(instHandle, CSL_MCASP_RGBLCTL_RSMRST_MASK);

    /*frame sync*/
    mcaspBitSetGblRCtl(instHandle, CSL_MCASP_RGBLCTL_RFRST_MASK);

}

/**
 *  \brief   This function activates the transmit section state machine and
 *           the corresponding frame sync.
 *
 *  \param   instHandle [IN]  pointer to the Mcasp instance object
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
Void Mcasp_localActivateSmFsForXmt(Mcasp_Object *instHandle)
{
    assert(NULL != instHandle);

    (instHandle->hwInfo.regs)->XSTAT = (CSL_MCASP_XSTAT_XERR_MASK
                                       | CSL_MCASP_XSTAT_XDMAERR_MASK
                                       | CSL_MCASP_XSTAT_XSTAFRM_MASK
                                       | CSL_MCASP_XSTAT_XDATA_MASK
                                       | CSL_MCASP_XSTAT_XLAST_MASK
                                       | CSL_MCASP_XSTAT_XTDMSLOT_MASK
                                       | CSL_MCASP_XSTAT_XCKFAIL_MASK
                                       | CSL_MCASP_XSTAT_XSYNCERR_MASK
                                       | CSL_MCASP_XSTAT_XUNDRN_MASK);

    /*Start the Serializer, State M/C and Frame Sync Generator                */
    mcaspBitSetGblXCtl(instHandle, CSL_MCASP_XGBLCTL_XSRCLR_MASK);
    mcaspBitSetGblXCtl(instHandle, CSL_MCASP_XGBLCTL_XSMRST_MASK);
    mcaspBitSetGblXCtl(instHandle, CSL_MCASP_XGBLCTL_XFRST_MASK);

}

/**
 *  \brief   This function activates the state machine and the corresponding
 *           frame sync for both receive and transmit sections.
 *
 *  \param   instHandle [IN]  pointer to the Mcasp Instance object
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
Void Mcasp_localActivateSmFsForBoth(Mcasp_Object *instHandle)
{
    assert((NULL != instHandle));

    /* Clear the interrupts                                                   */
    (instHandle->hwInfo.regs)->XSTAT = ( CSL_MCASP_XSTAT_XERR_MASK
                                       | CSL_MCASP_XSTAT_XDMAERR_MASK
                                       | CSL_MCASP_XSTAT_XSTAFRM_MASK
                                       | CSL_MCASP_XSTAT_XDATA_MASK
                                       | CSL_MCASP_XSTAT_XLAST_MASK
                                       | CSL_MCASP_XSTAT_XTDMSLOT_MASK
                                       | CSL_MCASP_XSTAT_XCKFAIL_MASK
                                       | CSL_MCASP_XSTAT_XSYNCERR_MASK
                                       | CSL_MCASP_XSTAT_XUNDRN_MASK);

    (instHandle->hwInfo.regs)->RSTAT = ( CSL_MCASP_RSTAT_RERR_MASK
                                       | CSL_MCASP_RSTAT_RDMAERR_MASK
                                       | CSL_MCASP_RSTAT_RSTAFRM_MASK
                                       | CSL_MCASP_RSTAT_RDATA_MASK
                                       | CSL_MCASP_RSTAT_RLAST_MASK
                                       | CSL_MCASP_RSTAT_RTDMSLOT_MASK
                                       | CSL_MCASP_RSTAT_RCKFAIL_MASK
                                       | CSL_MCASP_RSTAT_RSYNCERR_MASK
                                       | CSL_MCASP_RSTAT_ROVRN_MASK);

    /*Start the Serializer, State M/C and Frame Sync Generator*/
    mcaspBitSetGblCtl(
        instHandle,
        (CSL_MCASP_XGBLCTL_XSRCLR_MASK | CSL_MCASP_RGBLCTL_RSRCLR_MASK));
    mcaspBitSetGblCtl(
        instHandle,
        (CSL_MCASP_XGBLCTL_XSMRST_MASK | CSL_MCASP_RGBLCTL_RSMRST_MASK));
    mcaspBitSetGblCtl(
        instHandle,
        (CSL_MCASP_XGBLCTL_XFRST_MASK  | CSL_MCASP_RGBLCTL_RFRST_MASK ));

}



/**
 *  \brief  Resets the state m/c of the McASP xmt/rcv unit. In case of interrupt
 *          driven transactions, this also resets data queued for servicing the
 *          MCASP xmt/rcv unit when it is taken out of reset.
 *
 *  \param  chanHandle  [IN]  Handle to the mcasp channel to reset
 *  \param  eb          [OUT] pointer to the error information block
 *
 *  \return  Nothing
 *
 *  \enter   chanHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localSmReset(Mcasp_ChannelHandle chanHandle)
{
    Mcasp_Object   *instHandle = NULL;

    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
    {
        /* Reset state m/c and frame sync of receive section                  */
        Mcasp_localDeactivateSmFsForRcv(instHandle);
    }
    else
    {
        /* Reset state m/c and frame sync of transmit section                 */
        Mcasp_localDeactivateSmFsForXmt(instHandle);
    }

    if (FALSE == chanHandle->isDmaDriven)
    {
        /* Interrupt driven transactions */

        /* When iohandle state m/c is reactivated, it will require two        *
         * requests to service it. Reset count to ensure this.                */
        chanHandle->dataQueuedOnReset = 0;
        chanHandle->xferinProgressIntmode = FALSE;
    }
    else
    {
        EDMA3_DRV_disableTransfer(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_TRIG_MODE_EVENT);
    }
}

/**
 *  \brief  This function activates the state machine of the Mcasp transmit and
 *          receive sections and enables the edma transfer.
 *
 *  \param  chanHandle  [IN]  Handle to the Mcasp Channel
 *  \param  eb          [OUT] pointer to the error information block
 *
 *  \return  None
 *
 *  \enter   hanHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localSmSet(Mcasp_ChannelHandle chanHandle)
{
    Mcasp_Object *instHandle = NULL;

    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    instHandle = (Mcasp_Object *)chanHandle->devHandle;

    if (FALSE == chanHandle->isDmaDriven)
    {
        /* Interrupt driven transactions */

        /* When channel state m/c is reactivated, it will require two         *
         * requests to service it. Reset count to ensure this.                */
        chanHandle->dataQueuedOnReset = 0;
        chanHandle->xferinProgressIntmode = FALSE;
    }
    else
    {
        /* Start the DMA chanHandle operation.                                */
        EDMA3_DRV_enableTransfer(
            chanHandle->edmaHandle,
            chanHandle->xferChan,
            EDMA3_DRV_TRIG_MODE_EVENT);
    }

    if (Mcasp_ChanMode_RCV == chanHandle->channelOpMode)
    {
        /* Reset state m/c and frame sync of receive section                  */
        Mcasp_localActivateSmFsForRcv(instHandle);
    }
    else
    {
        /* Reset state m/c and frame sync of transmit section                 */
        Mcasp_localActivateSmFsForXmt(instHandle);
    }

}

/**
 *  \brief   This function configures format, frame sync, and other parameters
 *           related to the xmt section. Also configures the xmt clk section.
 *
 *  \param   instHandle  [IN]  Pointer to the Mcasp driver object
 *  \param   xmtData     [IN]  data for configuring the TX section
 *  \param   eb          [OUT] pointer to the error information block.
 *
 *  \return  IOM_COMPLETED  - Successful completion of read
 *          IOM_EBADARGS   - Invalid parameters
 *
 *
 *  \enter   instHandle  is a valid non null pointer
 *          xmtData     is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Int32 Mcasp_localConfigXmtSection(Mcasp_Object       *instHandle,
                                  Mcasp_HwSetupData  *const xmtData)
{
    Int32   status     = IOM_COMPLETED;
    Uint8   xCnt       = 0x00;
    Uint32  xMin       = 0x00;
    Uint32  xMax       = 0x00;
    Uint32  retryCount = 5u;
    Uint32  tempVal    = 0x00;

    assert((NULL != instHandle)
           && (NULL != xmtData)
           && (NULL != (instHandle->hwInfo.regs)));

    /* Configure TXMASK register                                              */
    (instHandle->hwInfo.regs)->XMASK = xmtData->mask;

    /* Reset the XSMRST bit in GBLCTL register                                */
    mcaspBitRemoveGblXCtl(instHandle,~(CSL_MCASP_XGBLCTL_XSMRST_MASK));

    /* Configure TXFMT register                                               */
    (instHandle->hwInfo.regs)->XFMT = xmtData->fmt;

    /* Reset the XFRST register in GBLCTL                                     */
    mcaspBitRemoveGblXCtl(instHandle,~(CSL_MCASP_XGBLCTL_XFRST_MASK));

    /* Configure TXFMCTL register                                             */
    (instHandle->hwInfo.regs)->AFSXCTL = xmtData->frSyncCtl;

    if (CSL_MCASP_AFSXCTL_FSXM_INTERNAL ==
        (((instHandle->hwInfo.regs)->AFSXCTL & CSL_MCASP_AFSXCTL_FSXM_MASK)
        >> CSL_MCASP_AFSXCTL_FSXM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~ (CSL_MCASP_PDIR_AFSX_MASK))
            | (CSL_MCASP_PDIR_AFSX_OUTPUT << CSL_MCASP_PDIR_AFSX_SHIFT);
    }

    mcaspBitRemoveGblXCtl(
        instHandle,
        ~(CSL_MCASP_XGBLCTL_XHCLKRST_MASK
          | CSL_MCASP_XGBLCTL_XCLKRST_MASK
          | CSL_MCASP_XGBLCTL_XSRCLR_MASK));


    /* Configure XTDM register                                                */
    (instHandle->hwInfo.regs)->XTDM =  xmtData->tdm;

    /* Configure XCLKCHK register                                             */
    (instHandle->hwInfo.regs)->XCLKCHK   =  xmtData->clk.clkChk;

    /* Sequence of start: starting hclk first                                 */
    mcaspBitSetGblXCtl(instHandle,CSL_MCASP_XGBLCTL_XHCLKRST_MASK);

    /* start ACLKX only if internal clock is used                             */
    if (CSL_MCASP_ACLKXCTL_CLKXM_INTERNAL ==
        (((instHandle->hwInfo.regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_CLKXM_MASK)
        >> CSL_MCASP_ACLKXCTL_CLKXM_SHIFT))
    {
        mcaspBitSetGblXCtl(instHandle,CSL_MCASP_XGBLCTL_XCLKRST_MASK);
    }

    /* Configure ACLKXCTL register                                            */
    (instHandle->hwInfo.regs)->ACLKXCTL = xmtData->clk.clkSetupClk;

    if (CSL_MCASP_ACLKXCTL_CLKXM_INTERNAL ==
       (((instHandle->hwInfo.regs)->ACLKXCTL & CSL_MCASP_ACLKXCTL_CLKXM_MASK)
        >> CSL_MCASP_ACLKXCTL_CLKXM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~(CSL_MCASP_PDIR_ACLKX_MASK))
            | (CSL_MCASP_PDIR_ACLKX_OUTPUT << CSL_MCASP_PDIR_ACLKX_SHIFT);

    }

    /* Configure AHCLKXCTL register                                           */
    (instHandle->hwInfo.regs)->AHCLKXCTL = xmtData->clk.clkSetupHiClk;

    if (CSL_MCASP_AHCLKXCTL_HCLKXM_INTERNAL ==
       (((instHandle->hwInfo.regs)->AHCLKXCTL & CSL_MCASP_AHCLKXCTL_HCLKXM_MASK)
       >> CSL_MCASP_AHCLKXCTL_HCLKXM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~(CSL_MCASP_PDIR_AHCLKX_MASK))
            | (CSL_MCASP_PDIR_AHCLKX_OUTPUT << CSL_MCASP_PDIR_AHCLKX_SHIFT);
    }

    /* Configure XEVTCTL register                                             */
    if (Mcasp_TX_CLK_FAIL == (Mcasp_TX_CLK_FAIL & xmtData->intCtl))
    {
        /* now read the counter value and set val +1 as max and val -1 as min */
        xCnt = (((instHandle->hwInfo.regs)->XCLKCHK
                   & CSL_MCASP_XCLKCHK_XCNT_MASK) >>
                   CSL_MCASP_XCLKCHK_XCNT_SHIFT);


        xMax = xCnt + 3u;
        xMin = xCnt - 3u;

        if (xCnt >= 0xFD)
        {
            xMax = 255u;
        }

        if (xCnt <= 3u)
        {
            xMin = 0u;
        }

        /* now write the value to the register                                */
        (instHandle->hwInfo.regs)->XCLKCHK &= ~(CSL_MCASP_XCLKCHK_XMAX_MASK |
                                                 CSL_MCASP_XCLKCHK_XMIN_MASK);

        (instHandle->hwInfo.regs)->XCLKCHK |=
            ((xMax  << CSL_MCASP_XCLKCHK_XMAX_SHIFT) |
             (xMin  << CSL_MCASP_XCLKCHK_XMIN_SHIFT));

        /* check if the clock has stablised without any errors                */
        while (retryCount-- > 0)
        {
            /* write to the XCLK FAIL register to clear the failure bit       */
            tempVal = (instHandle->hwInfo.regs)->XSTAT;

            (instHandle->hwInfo.regs)->XSTAT = tempVal;

            TSK_sleep(10);

            /* verify if the clock failure is detected. if not detected then  *
             * we can retry else we can continue with the intialisation       */
            if (Mcasp_TX_CLK_FAIL !=
                ((instHandle->hwInfo.regs)->XSTAT & Mcasp_TX_CLK_FAIL))
            {
                break;
            }
        }

        if (0x00 == retryCount)
        {
            /* setting the clock failure interrupt failed                     */
            status = IOM_EBADARGS;
        }

        (instHandle->hwInfo.regs)->XINTCTL |= Mcasp_TX_CLK_FAIL;

        /* set the AMUTE register RCLK bit                                    */
        (instHandle->hwInfo.regs)->AMUTE |= CSL_MCASP_AMUTE_XCKFAIL_ENABLE <<
                                                CSL_MCASP_AMUTE_XCKFAIL_SHIFT;
    }

    /* Configure XINTCTL register                                             */
    (instHandle->hwInfo.regs)->XINTCTL =  xmtData->intCtl;

#ifndef Mcasp_LOOPJOB_ENABLED
    /* copy the user supplied interrupt enable mask                           */
    instHandle->XmtObj.userIntValue = xmtData->intCtl;
#endif /* Mcasp_LOOPJOB_ENABLED  */

    /* Configure XEVTCTL register                                             */
    (instHandle->hwInfo.regs)->XEVTCTL =  xmtData->evtCtl;

    return (status);
}


/**
 *  \brief   This function configures the format, frame sync, and other params
 *           related to the rcv section. Also configures the rcv clk section.
 *
 *  \param   hMcasp      [IN]     Handle to the McASP instance
 *  \param   rcvData     [IN]     Receive related parameters
 *  \return
 *           IOM_COMPLETED   - Successful completion of read
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 *  \enter   instHandle  is a valid non null pointer
 *           rcvData     is a valid non null pointer
 *
 *  \leave   Not implemented
 */

Int32 Mcasp_localConfigRcvSection(Mcasp_Object       *instHandle,
                                  Mcasp_HwSetupData  *const rcvData)
{
    Int32   status     = IOM_COMPLETED;
    Uint8   rCnt       = 0x00;
    Uint32  rMin       = 0x00;
    Uint32  rMax       = 0x00;
    Uint32  retryCount = 5u;
    Uint32  tempVal    = 0x00;

    assert((NULL != instHandle)
          && (NULL != rcvData)
          && (NULL != instHandle->hwInfo.regs));

    /* Configure RMASK register                                               */
    (instHandle->hwInfo.regs)->RMASK = rcvData->mask;

    /* Reset the RSMRST bit in GBLCTL register                                */
    mcaspBitRemoveGblRCtl(instHandle,~(CSL_MCASP_RGBLCTL_RSMRST_MASK));

    /* Configure RFMT register                                                */
    (instHandle->hwInfo.regs)->RFMT = rcvData->fmt;

    mcaspBitRemoveGblRCtl(instHandle,~(CSL_MCASP_RGBLCTL_RFRST_MASK));

    /* Configure RXFMCTL register                                             */
    (instHandle->hwInfo.regs)->AFSRCTL =  rcvData->frSyncCtl;

    if (CSL_MCASP_AFSRCTL_FSRM_INTERNAL ==
       (((instHandle->hwInfo.regs)->AFSRCTL & CSL_MCASP_AFSRCTL_FSRM_MASK)
       >> CSL_MCASP_AFSRCTL_FSRM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~(CSL_MCASP_PDIR_AFSR_MASK))
            | (CSL_MCASP_PDIR_AFSR_OUTPUT << CSL_MCASP_PDIR_AFSR_SHIFT);
    }

    /* Reset RHCLKRST, RCLKRST, RSRCLR in GBLCTL                              */
    mcaspBitRemoveGblRCtl(
        instHandle,
        ~(CSL_MCASP_RGBLCTL_RHCLKRST_MASK
          | CSL_MCASP_RGBLCTL_RCLKRST_MASK
          | CSL_MCASP_RGBLCTL_RSRCLR_MASK));

    /* Configure it completely before starting the clocks                     */
    /* Configure RTDM register                                                */
    (instHandle->hwInfo.regs)->RTDM =  rcvData->tdm;

    /* Configure RXCLKCHK register                                            */
    (instHandle->hwInfo.regs)->RCLKCHK   =  rcvData->clk.clkChk;

    /* Sequence of start: starting hclk first                                 */
    /* start AHCLKR                                                           */
    mcaspBitSetGblRCtl(instHandle,CSL_MCASP_RGBLCTL_RHCLKRST_MASK);

    /* start ACLKR only if internal clock is used*/
    if (CSL_MCASP_ACLKRCTL_CLKRM_INTERNAL ==
        (((instHandle->hwInfo.regs)->ACLKRCTL & CSL_MCASP_ACLKRCTL_CLKRM_MASK)
        >> CSL_MCASP_ACLKRCTL_CLKRM_SHIFT))
    {
        mcaspBitSetGblRCtl(instHandle,CSL_MCASP_RGBLCTL_RCLKRST_MASK);
    }

    /* Configure ACLKRCTL register                                            */
    (instHandle->hwInfo.regs)->ACLKRCTL  = rcvData->clk.clkSetupClk;

    if (CSL_MCASP_ACLKRCTL_CLKRM_INTERNAL ==
       (((instHandle->hwInfo.regs)->ACLKRCTL & CSL_MCASP_ACLKRCTL_CLKRM_MASK)
       >> CSL_MCASP_ACLKRCTL_CLKRM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~(CSL_MCASP_PDIR_ACLKR_MASK))
            | (CSL_MCASP_PDIR_ACLKR_OUTPUT << CSL_MCASP_PDIR_ACLKR_SHIFT);
    }

    /* Configure AHCLKRCTL register */
    (instHandle->hwInfo.regs)->AHCLKRCTL = rcvData->clk.clkSetupHiClk;

    if (CSL_MCASP_AHCLKRCTL_HCLKRM_INTERNAL ==
       (((instHandle->hwInfo.regs)->AHCLKRCTL & CSL_MCASP_AHCLKRCTL_HCLKRM_MASK)
       >> CSL_MCASP_AHCLKRCTL_HCLKRM_SHIFT))
    {
        (instHandle->hwInfo.regs)->PDIR =
            ((instHandle->hwInfo.regs)->PDIR & ~(CSL_MCASP_PDIR_AHCLKR_MASK))
            | (CSL_MCASP_PDIR_AHCLKR_OUTPUT << CSL_MCASP_PDIR_AHCLKR_SHIFT);
    }

    /* Configure REVTCTL register                                             */
    if (Mcasp_RX_CLK_FAIL == (Mcasp_RX_CLK_FAIL & rcvData->intCtl))
    {
        /* now read the counter value and set val +1 as max and val -1 as min */
        rCnt = (((instHandle->hwInfo.regs)->RCLKCHK
                   & CSL_MCASP_RCLKCHK_RCNT_MASK) >>
                   CSL_MCASP_RCLKCHK_RCNT_SHIFT);

        rMax = rCnt + 3u;
        rMin = rCnt - 3u;

        if (rCnt >= 0xFD)
        {
            rMax = 255u;
        }

        if (rCnt <= 3u)
        {
            rMin = 0u;
        }

        /* now write the value to the register                                */
        (instHandle->hwInfo.regs)->RCLKCHK &= ~(CSL_MCASP_RCLKCHK_RMAX_MASK |
                                                 CSL_MCASP_RCLKCHK_RMIN_MASK);

        (instHandle->hwInfo.regs)->RCLKCHK |=
            ((rMax  << CSL_MCASP_RCLKCHK_RMAX_SHIFT ) |
             (rMin  << CSL_MCASP_RCLKCHK_RMIN_SHIFT ));

        /* check if the clock has stablised without any errors                */
        while (retryCount-- > 0)
        {
            /* write to the RCLK FAIL register to clear the failure bit       */
            tempVal = (instHandle->hwInfo.regs)->RSTAT;

            (instHandle->hwInfo.regs)->RSTAT = tempVal;

            /* wait for the reading to be taken                               */
            TSK_sleep(10);

            /* verify if the clock failure is detected. if detected then      *
             * we can retry else we can continue with the intialisation       */
            if (Mcasp_RX_CLK_FAIL !=
                ((instHandle->hwInfo.regs)->RSTAT & Mcasp_RX_CLK_FAIL))
            {
                break;
            }
        }

        if (0x00 == retryCount)
        {
            /* setting the clock failure interrupt failed                     */
            status = IOM_EBADARGS;
        }


        (instHandle->hwInfo.regs)->RINTCTL |= Mcasp_RX_CLK_FAIL;

        /* set the AMUTE register RCLK bit                                    */
        (instHandle->hwInfo.regs)->AMUTE |= CSL_MCASP_AMUTE_RCKFAIL_ENABLE <<
                                                CSL_MCASP_AMUTE_RCKFAIL_SHIFT;
    }

    (instHandle->hwInfo.regs)->RINTCTL =  rcvData->intCtl;

#ifndef Mcasp_LOOPJOB_ENABLED
    /* copy the user supplied interrupt enable mask                           */
    instHandle->RcvObj.userIntValue = rcvData->intCtl;
#endif /* Mcasp_LOOPJOB_ENABLED  */

    (instHandle->hwInfo.regs)->REVTCTL =  rcvData->evtCtl;
    return (status);
}


/**
 *  \brief  This function resets the bits related to transmit in XGBLCTL.
 *
 *  \param  hMcasp  [IN] Pointer to the Mcasp Hardware info structure
 *
 *  \return
 *          IOM_COMPLETED   - Successful completion
 *          IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 *  \enter  hMcasp      is a valid non null pointer
 *
 *  \leave  Not implemented
 */
Int32 Mcasp_localResetXmt(Mcasp_HwHandle  hMcasp)
{
    Int32   status = IOM_COMPLETED;

    assert((NULL != hMcasp) && (NULL != hMcasp->regs));

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XCLKRST_MASK))
        | (CSL_MCASP_XGBLCTL_XCLKRST_RESET
            << CSL_MCASP_XGBLCTL_XCLKRST_SHIFT);

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XHCLKRST_MASK))
        | (CSL_MCASP_XGBLCTL_XHCLKRST_RESET
            << CSL_MCASP_XGBLCTL_XHCLKRST_SHIFT);

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XSRCLR_MASK))
        |(CSL_MCASP_XGBLCTL_XSRCLR_CLEAR << CSL_MCASP_XGBLCTL_XSRCLR_SHIFT);

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XSMRST_MASK))
        |(CSL_MCASP_XGBLCTL_XSMRST_RESET << CSL_MCASP_XGBLCTL_XSMRST_SHIFT);

    (hMcasp->regs)->XGBLCTL =
        ((hMcasp->regs)->XGBLCTL & ~(CSL_MCASP_XGBLCTL_XFRST_MASK))
        | (CSL_MCASP_XGBLCTL_XFRST_RESET << CSL_MCASP_XGBLCTL_XFRST_SHIFT);

    return (status);

}

/**
 *  \brief   This function resets the bits related to receive in RGBLCTL.
 *
 *  \param   hMcasp  [IN] Pointer to the Mcasp Hardware info structure
 *
 *  \return
 *           IOM_COMPLETED   - Successful completion
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 *  \enter   hMcasp      is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Int32 Mcasp_localResetRcv(Mcasp_HwHandle  hMcasp)
{
    Int32  status = IOM_COMPLETED;

    assert((NULL != hMcasp) && (NULL != hMcasp->regs));

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RCLKRST_MASK))
        | (CSL_MCASP_RGBLCTL_RCLKRST_RESET
            << CSL_MCASP_RGBLCTL_RCLKRST_SHIFT);

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RHCLKRST_MASK))
        | (CSL_MCASP_RGBLCTL_RHCLKRST_RESET
            << CSL_MCASP_RGBLCTL_RHCLKRST_SHIFT);

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RSRCLR_MASK))
        |(CSL_MCASP_RGBLCTL_RSRCLR_CLEAR << CSL_MCASP_RGBLCTL_RSRCLR_SHIFT);

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RSMRST_MASK))
        |(CSL_MCASP_RGBLCTL_RSMRST_RESET << CSL_MCASP_RGBLCTL_RSMRST_SHIFT);

    (hMcasp->regs)->RGBLCTL =
        ((hMcasp->regs)->RGBLCTL & ~(CSL_MCASP_RGBLCTL_RFRST_MASK))
        | (CSL_MCASP_RGBLCTL_RFRST_RESET << CSL_MCASP_RGBLCTL_RFRST_SHIFT);

    return (status);

}


/**
 *  \brief  For the transmit operation, the eDMA channel's destination
 *          port is tied to the MCASP DAT port. In case of receive, the eDMA
 *          channel's source port is tied to the MCASP DAT port. The source
 *          address for transmit eDMA channel and the destination address for
 *          the receive eDMA channel are set here.
 *
 *  \param  chanp      [IN]    Pointer to channel
 *  \param  ioPacket   [IN]    Pointer to request to be processed
 *
 *  \return IOM_COMPLETED, if the address is set correctly
 *          ERROR ID otherwise
 *
 *  \enter  chanp       is a valid non null pointer
 *          ioPacket    is a valid non null pointer
 *
 *  \leave  Not implemented
 */
Int32 Mcasp_localEdmaProcessPkt(Ptr chanp, IOM_Packet *ioPacket)
{
    Mcasp_Object           *instHandle  = NULL;
    Mcasp_ChannelHandle     chanHandle  = NULL;
    Mcasp_PktAddrPayload   *payLoad     = NULL;
    Int32                   status      = IOM_COMPLETED;
    Bool                    falsewhile  = TRUE;

    /* Validate pointers and parameters                                       */
    assert((NULL != chanp) && (NULL != ioPacket));

    chanHandle = (Mcasp_ChannelHandle)chanp;

    assert(NULL != chanHandle->devHandle);

    do
    {
        falsewhile = FALSE;

        instHandle = (Mcasp_Object *)chanHandle->devHandle;

        if ((Mcasp_ChanMode_RCV != chanHandle->channelOpMode)
            && (Mcasp_ChanMode_XMT_DIT == chanHandle->channelOpMode))
        {
            /* The data should be a payload structure                         */
            payLoad = (Mcasp_PktAddrPayload *)ioPacket->addr;
            assert(NULL != payLoad);

            if (TRUE == payLoad->writeDitParams)
            {
                /* DIT Mode: for DIT mode as of now each packet (IOP)         *
                 * carries/should carry 1 super frame = 192 frames = 384 sub  *
                 * frames of data max                                         */
                status = mcaspWriteChanStatRam(
                            &(instHandle->hwInfo),
                            (Mcasp_ChStatusRam *)payLoad->chStat);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_ABORTED;
                    break;
                }
                status = mcaspWriteUserDataRam(
                            &(instHandle->hwInfo),
                            (Mcasp_UserDataRam *)payLoad->userData);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_ABORTED;
                    break;
                }
            }
        }

        /* we are going to update the paramset from EDMA callback context.    *
         * That means the one of the param sets are having audio data         *
         * just completed data request and one having currently loaded (active*
         * ) data request Current index is pointing only latest paramset      *
         * (that is active)  so move the index                                */
        Mcasp_localGetNextIndex(&chanHandle->nextLinkParamSetToBeUpdated);

        /* Link the packet                                                    */
        status = Mcasp_localUpdtDtPktToLnkPrms(chanHandle,ioPacket);

    }while(falsewhile);

    return (status);
}

/**
 *  \brief   This function completes the current pending request and then
 *           invokes the application registered callback.
 *
 *  \param   chanHandle   [IN]  Handle to the channel
 *  \param   eb           [IN]  pointer to the error block
 *
 *  \return  None
 *
 *  \enter   chanHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localCompleteCurrentIo(Mcasp_ChannelHandle chanHandle)
{
    IOM_Packet    *ioPacket     = NULL;
#ifndef Mcasp_LOOPJOB_ENABLED
    Mcasp_Object  *instHandle   = NULL;
#endif

    assert(NULL != chanHandle);

#ifndef Mcasp_LOOPJOB_ENABLED
    assert(NULL != chanHandle->devHandle);
    instHandle = (Mcasp_Object *)chanHandle->devHandle;
#endif

    if (TRUE == chanHandle->isTempPacketValid)
    {
        ioPacket = chanHandle->tempPacket;
        chanHandle->isTempPacketValid = FALSE;
    }
    else
    {
        ioPacket = chanHandle->dataPacket;
    }

#ifndef Mcasp_LOOPJOB_ENABLED
    /* reduce the dependency count                                            */
    Mcasp_localLpscOff(instHandle);
#endif

    if (NULL != ioPacket)
    {
        /* call the completion callback function registered with us           *
         * during channel creation                                            */
        if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
        {
            /*  Application callback                                          */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
        }
    }
    chanHandle->dataPacket = NULL;
    chanHandle->userDataBufferSize = 0;
}

/**
 *  \brief   This function aborts all the pending requests in the queue
 *
 *  \param   chanHandle   [IN]  Handle to the channel
 *  \param   eb           [IN]  pointer to the error block
 *
 *  \return  None
 *
 *  \enter   chanHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Int32 Mcasp_localCancelAndAbortAllIo(Mcasp_ChannelHandle chanHandle)
{
    Uint32               dlbMode    = 0u;
    Mcasp_Object        *instHandle = NULL;
    Int32                status = IOM_COMPLETED;

    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    chanHandle->submitCount--;

    chanHandle->dataPacket->status = IOM_ABORTED;

    Mcasp_localCompleteCurrentIo(chanHandle);

    instHandle = (Mcasp_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* While aborting care should be taken about DLB mode otherwise           *
     * aborting packets in only one direction will make the packet in         *
     * other direction not to return                                          */
    dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                 & CSL_MCASP_DLBCTL_DLBEN_MASK)
              >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

    if (CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
    {
        status = Mcasp_localAbortReset((Mcasp_ChannelHandle)&(instHandle->RcvObj));
        if (IOM_COMPLETED == status)
        {
            status = Mcasp_localAbortReset((Mcasp_ChannelHandle)&(instHandle->XmtObj));
        }
    }
    else
    {
        status = Mcasp_localAbortReset(chanHandle);
    }

    return (status);
}

/**
 *  \brief   This function sets the GBLCTL bit and wait and checks that bit is
 *           set or not
 *
 *  \param   instHandle  [IN]  pointer to the mcasp instance object
 *  \param   bitMaskVal  [IN]  mask bit value
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *           bitMaskVal  is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspBitSetGblCtl(Mcasp_Object *instHandle,Uint32 bitMaskVal)
{
    Uint32 timeout = Mcasp_GBLCTL_TIMEOUT;
    Bool   isTask  = FALSE;

    assert(NULL != instHandle);

    /* If already this bit is set then don't set again                        */
    if (((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) != bitMaskVal)
    {
        (instHandle->hwInfo.regs)->GBLCTL =
            ((instHandle->hwInfo.regs)->GBLCTL | bitMaskVal);

        while ((((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) !=
            bitMaskVal) && (timeout > 0))
        {
            /* reduce the timout count value                                  */
            timeout--;

            isTask = TSK_isTSK();

            /* wait for 1 tick only if called from a task context             */
            if (TRUE == isTask)
            {
                TSK_sleep(1);
            }
        }
        assert(0 != timeout);
    }
}

/*
 *  \brief Initializes McASP driver's data structures
 *
 *  This function initializes the McASP driver's internal data structures
 *  used by the driver for the mute and loop job. These data structures are
 *  aligned to the proper cache line boundary here.
 *
 *  \param  instNum [IN]  instance num of the data structure to initialise
 *
 *  \return Nothing
 *
 *  \enter   instNUm     is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspInit(Uint32 instNum)
{
    if (instNum < CSL_MCASP_PER_CNT)
    {
#ifdef Mcasp_LOOPJOB_ENABLED
        /* initialise the mcasp device driver internal buffers                */
        memset(&Mcasp_loopDstBuf[instNum],0x00u, sizeof(Mcasp_TempBuffer));
        memset(&Mcasp_loopSrcBuf[instNum],0x00u, sizeof(Mcasp_TempBuffer));

        /* align the buffers to the cache line size                           */
        Mcasp_loopSrcBuf[instNum].scratchBuffer = (Uint32 *)
            (((Uint32)Mcasp_loopSrcBuf[instNum].scratchBuf + 0x7F) & ~0x7F);

        Mcasp_loopDstBuf[instNum].scratchBuffer = (Uint32 *)
            (((Uint32)Mcasp_loopDstBuf[instNum].scratchBuf + 0x7F) & ~0x7F);
#endif /* Mcasp_LOOPJOB_ENABLED */
        Mcasp_muteBuf[instNum].scratchBuffer = (Uint32 *)
            (((Uint32)Mcasp_muteBuf[instNum].scratchBuf + 0x7F) & ~0x7F);
    }
    /* initialisation of driver data structures is completed                  */
    return;
}

/**
 *  \brief   Writes a packet of data to the output channel of the driver
 *           This function is called by the IDriver submit function. This
 *           function is called with handle to the o/p channel and the pointer
 *           to ioRequest structure which takes a set of information need for
 *           the IO.
 *
 *  \param   chanHandle  [IN]   Handle to the McASP channel
 *  \param   ioPacket    [IN]   io request information structure
 *
 *  \return IOM_COMPLETED or Error code
 *
 *  \enter   chanHandle  is a valid non null pointer
 *           ioPacket    is a valid non null pointer
 *
 *  \leave   Not implemented
 */
static Int32 mcaspSubmitReq(Mcasp_ChannelHandle chanHandle,
                            IOM_Packet         *ioPacket)
{
    Mcasp_Object           *instHandle  = NULL;
    Mcasp_PktAddrPayload   *payLoad     = NULL;
    Int32                   status      = IOM_COMPLETED;
    Uint32                  hwiKey      = 0;
    Bool                    falsewhile  = TRUE;
    Ptr                     pktAddr     = NULL;
#ifdef Mcasp_LOOPJOB_ENABLED
    Bool                    chanStopped = FALSE;
#endif

    assert((NULL != chanHandle) && (NULL != ioPacket));

    assert(NULL != chanHandle->devHandle);

    /* check for the valid packet size                                        */
    assert((0 != ioPacket->size)
        && (Mcasp_MAX_PACKET_SIZE >= 
            ioPacket->size/chanHandle->noOfSerAllocated));

    do
    {
        falsewhile = FALSE;
        instHandle = (Mcasp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        if (TRUE == instHandle->isDataBufferPayloadStructure)
        {
            payLoad = (Mcasp_PktAddrPayload *)ioPacket->addr;
            assert(NULL != payLoad);

            pktAddr = payLoad->addr;
        }
        else
        {
            pktAddr = ioPacket->addr;
        }

        assert(NULL != pktAddr);

        /* maintain cache coherency                                           */
        if (IOM_INPUT == chanHandle->mode)
        {
            /* Cleans and invalidate the D Cache in MVA way                   */
            if (TRUE == instHandle->enablecache)
            {
                BCACHE_inv(pktAddr,ioPacket->size,TRUE);
            }
#ifdef Mcasp_LOOPJOB_ENABLED
            /* check if the state machine is stopped. if the state machine is *
             * stopped then we will load the packets in to the pending queue  */
            if (TRUE == instHandle->stopSmFsRcv)
            {
                chanStopped = TRUE;
            }
#endif
        }
        else
        {
            /* clean the D Cache in MVA way                                   */
            if (TRUE == instHandle->enablecache)
            {
                BCACHE_wb(pktAddr,ioPacket->size,TRUE);
            }
#ifdef Mcasp_LOOPJOB_ENABLED
            /* check if the state machine is stopped. if the state machine is *
             * stopped then we will load the packets in to the pending queue  */
            if (TRUE == instHandle->stopSmFsXmt)
            {
                chanStopped = TRUE;
            }
#endif
        }

        /*  Enter critical section to protect the submit count                */
        hwiKey = (Uint32)_disable_interrupts();

#ifdef Mcasp_LOOPJOB_ENABLED
        if ((Mcasp_MAXLINKCNT <= chanHandle->submitCount) ||
            (TRUE == chanStopped) ||
#else
        /* We now have 3 packets loaded in the EDMA                           */
        if (((Mcasp_MAXLINKCNT + 1) <= chanHandle->submitCount) ||
#endif
            (TRUE == chanHandle->paused) ||
            (Mcasp_DriverState_PWRM_SUSPEND == instHandle->devState))
        {
            /* Already enough requests are in active queue.Hence load in to   *
             * the pending queue                                              */
            chanHandle->submitCount++;
            QUE_put(&(chanHandle->queueReqList),(Ptr)ioPacket);

            /* restore the interrupts                                         */
            _restore_interrupts(hwiKey);
        }
        else
        {
            /* Process this packet immediately as active queue is empty       */
            chanHandle->submitCount++;
            QUE_put(&(chanHandle->queueFloatingList),(Ptr)ioPacket);

            /* restore the interrupts                                         */
            _restore_interrupts(hwiKey);

            /* load this request in to the EDMA paramsets                     */
            Mcasp_localLoadPktToEdma(chanHandle,ioPacket);
        }
        /* return the status as pending as the packet will complete later     */
        status = IOM_PENDING;
    }while(falsewhile);
    return status;
}

/**
 * \fn      static Void mcaspIsrSwiFxn()
 *
 * \brief   Swi thread which handles the Mcasp module interrupts
 *
 * \param   arg0  [IN]  The pointer to the ISR object holds instance pointer.
 * \param   arg1  [IN]  unused argument
 *
 * \return  None
 *
 * \enter   arg0    is a valid non NULL pointer.
 *
 * \leave   Not implemented
 */
static Void Mcasp_localIsrSwiFxn(Arg arg0,Arg arg1)
{
    Uint32            count      = 0;
    Mcasp_Object     *instHandle = NULL;

    /* To remove the compiler warning                                         */
    arg0 = arg0;
    arg1 = arg1;

    /* An interrupt has occured and it could be because of any one of the     *
     * mcasp instances and also either a TX or RX section interrupt.Hence we  *
     * will check all the instances to find which interrupt has occured. Also *
     * note that here we are only intrested in logging the interrupts for the *
     * error conditions only                                                  */

    /* we are only checking the instances which are currently active          */
    for (count = 0; count < CSL_MCASP_PER_CNT; count++)
    {
        /* check if the instance has registered for interrupts                */
        if ((TRUE == Mcasp_module.isrObject[count].isIsrRegistered)
            && (NULL != Mcasp_module.isrObject[count].instHandle))
        {
            instHandle = Mcasp_module.isrObject[count].instHandle;
            assert(NULL != instHandle);

            /* check if the RX channel has registered the interrupts          */
            if (TRUE == Mcasp_module.isrObject[count].chanEnabled[0])
            {
                /* check if atleast one error has occured                     */
                if (Mcasp_RSTAT_ERROR ==
                    (Mcasp_RSTAT_ERROR & ((instHandle->hwInfo.regs)->RSTAT)))
                {
                    mcaspIsrInput((Arg)&(instHandle->RcvObj));
                }
            }

            /* check if the TX channel has registered the interrupts          */
            if (TRUE == Mcasp_module.isrObject[count].chanEnabled[1])
            {
                if (Mcasp_XSTAT_ERROR ==
                    (Mcasp_XSTAT_ERROR & ((instHandle->hwInfo.regs)->XSTAT)))

                {
                    mcaspIsrOutput((Arg)&(instHandle->XmtObj));
                }
            }
        }
    }
}


/**
 * \fn      static Void mcaspIsr()
 *
 * \brief   This function is the interrupt service routine for the mcasp driver.
 *
 * \param   arg  [IN]  The instance number of the first instance to register
 *                     the ISR.
 *
 * \return  None
 *
 * \enter   arg     is a valid non null pointer
 *
 * \leave   Not implemented
 */
Void mcaspIsr(Arg arg)
{
    /* we have got the interrupt for the Mcasp. we need to check which        *
     * instance has generated this interrupt and which section TX or RX. We   *
     * defer the procesing to the Swi thread for the mcasp driver             */

    /* Trigger the SWI thread to handle the interrupts                        *
     * Note : The "arg" here is the instance number of the instance which has *
     * first registered the isr. please note that any valid instance is OK as *
     * all the instances have the same SWI task handle.                       */
    SWI_post((SWI_Handle)arg);
}


/**
 *  \brief   This function is a ISR function.it will be called when Mcasp
 *           receive interrupt is triggered.
 *
 *  \param   chanp   [IN]  Handle to the mcasp channel
 *
 *  \return  None
 *
 *  \enter   chanp       is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void mcaspIsrInput(Arg chanp)
{
    Mcasp_Object         *instHandle = NULL;
    Mcasp_ChannelHandle   chanHandle = NULL;
    Bool                  falsewhile = TRUE;
    Int32                 status     = IOM_COMPLETED;
    Uint32                rmod       = 0;
    Uint32                dlbMode    = 0;
    EDMA3_DRV_PaRAMRegs   pramTbl    = {0};

    do
    {
        assert(NULL != chanp);

        falsewhile  = FALSE;

        chanHandle = (Mcasp_ChannelHandle)chanp;
        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        instHandle = (Mcasp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* Interrupt handling starts                                          */

        /* The driver is in DMA mode and it is enough if we handle errors     */

        /* Read interrupt-status register.                                    */
        chanHandle->intStatus = (Uint32)(instHandle->hwInfo.regs)->RSTAT;

        /*Interrupt status word to be written back to reset status bits       */
        (instHandle->hwInfo.regs)->RSTAT = chanHandle->intStatus;

        /* for bursmode of operation frame sync error has to ignored to       *
         * indentify that we do the following                                 */
        rmod = (Uint32)((instHandle->hwInfo.regs)->AFSRCTL
                    & CSL_MCASP_AFSRCTL_RMOD_MASK) ;

        do
        {
            falsewhile  = FALSE;

            if ((Mcasp_DMAERR == (Mcasp_DMAERR & chanHandle->intStatus)))
            {
                status = IOM_EBADIO;
                break;
            }

            if ((Mcasp_RCV_OVERRUN ==
                (Mcasp_RCV_OVERRUN & chanHandle->intStatus)))
            {
                status = IOM_EBADIO;
                break;
            }

            if (Mcasp_RX_CLK_FAIL ==
                (Mcasp_RX_CLK_FAIL & (instHandle->hwInfo.regs)->RINTCTL))
            {

                if (Mcasp_RX_CLK_FAIL ==
                    (Mcasp_RX_CLK_FAIL & chanHandle->intStatus))
                {
                    status = IOM_EBADIO;
                    break;
                }
            }
            /* no sync error check for burst mode                             */
            if (0 != rmod)
            {
                if ((Mcasp_SYNCERR ==
                    (Mcasp_SYNCERR & chanHandle->intStatus)))
                {
                    status = IOM_ETIMEOUT;
                    break;
                }
            }
        }while(falsewhile);

        if (IOM_COMPLETED != status)
        {
            /* call the application registered callback if any                */
            if (NULL != (chanHandle->gblErrCbk))
            {
                (chanHandle->gblErrCbk)(status);
            }

            /* For sync error we call the application callback and we         *
             * abort all other pending IOPs also                              */
            if (IOM_ETIMEOUT == status)
            {
                /* While aborting care should be taken about DLB mode         *
                 * otherwise aborting packets in only one direction will      *
                 * make the packet in other direction not to return           */
                dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                              & CSL_MCASP_DLBCTL_DLBEN_MASK)
                          >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

                if (CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
                {
                    status = Mcasp_localAbortReset(&(instHandle->RcvObj));

                    if (IOM_COMPLETED == status)
                    {
                        status = Mcasp_localAbortReset(&(instHandle->XmtObj));
                    }
                }
                else
                {
                    status = Mcasp_localAbortReset(chanHandle);
                }
                break;
            }

            /* if the error is a non- framesync error - we just send error    *
             * status for the current packet in the callback function and     *
             * continue with other packets for xfer                           */
            if (IOM_ETIMEOUT != status)
            {
                /* Disable Serializer, State machine and Frame sync of RCV    *
                 * Section. Also Disable EDMA Transfer.                       */
                Mcasp_localSmReset(chanHandle);

                /* Initiate dummy transfer (or reduce the c-count to 1) in    *
                 * EDMA-current transfer channel param This would load the    *
                 * next link paramset and give a edma callback where we       *
                 * do futher steps needed to close the current packet and     *
                 * load next pending packet etc                               */

                /* get the param table information of transfer channel        */
                EDMA3_DRV_getPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->xferChan,
                    &pramTbl);

                if (0 != pramTbl.cCnt)
                {
                    pramTbl.cCnt = 0;

                    EDMA3_DRV_setPaRAM(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        &pramTbl);
                }

                chanHandle->currentPacketErrorStatus = status;

                /* Enable Serializer, State machine and Frame sync of RCV     *
                 * Section.Also Enable EDMA Transfer.                         */
                Mcasp_localSmSet(chanHandle);
            }/*for errors except sync errors*/
        }/*dma transfer for all errors*/
    }while(falsewhile);
}

/**
 *  \brief   This function is the ISR function. This is called when Mcasp0 XMT
 *           interrupt is triggered
 *
 *  \param   chanp  [IN]  pointer.The pointer to the channel handle.
 *
 *  \return  None
 *
 *  \enter   chanp       is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void mcaspIsrOutput(Arg chanp)
{
    Mcasp_Object           *instHandle = NULL;
    Mcasp_ChannelHandle     chanHandle = NULL;
    Bool                    falsewhile = TRUE;
    Int32                   status     = IOM_COMPLETED;
    Uint32                  xmod       = 0;
    Uint32                  dlbMode    = 0;
    EDMA3_DRV_PaRAMRegs     pramTbl    = {0};

    assert(NULL != chanp);

    do
    {
        falsewhile = FALSE;

        chanHandle = (Mcasp_ChannelHandle)chanp;
        assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

        instHandle = (Mcasp_Object *)chanHandle->devHandle;
        assert(NULL != instHandle);

        /* Interrupt handling starts                                          */

        /*The driver is in DMA mode and we will handle only errors here       */
        status = IOM_COMPLETED;

        /* Read interrupt-status register.                                    */
        chanHandle->intStatus = (Uint32)(instHandle->hwInfo.regs)->XSTAT;

        /*Interrupt status word to be written back to reset status bits       */
        (instHandle->hwInfo.regs)->XSTAT = chanHandle->intStatus;

        /* For burst mode of operation frame sync error has to ignored        *
         * to indentify that we do the following                              */
        xmod = (Uint32)((instHandle->hwInfo.regs)->AFSXCTL
                    & CSL_MCASP_AFSXCTL_XMOD_MASK);
        do
        {
            falsewhile  = FALSE;

            if (((Mcasp_DMAERR) == (Mcasp_DMAERR & chanHandle->intStatus)))
            {
                status = IOM_EBADIO;
                break;
            }

            if (((Mcasp_XMT_UNDERRUN) ==
                (Mcasp_XMT_UNDERRUN & chanHandle->intStatus)))
            {
                status = IOM_EBADIO;
                break;
            }

            /* check the mcasp clock failure interrupt only if it is enabled  *
             * in interrupt control register                                  */
            if (Mcasp_TX_CLK_FAIL ==
                (Mcasp_TX_CLK_FAIL & (instHandle->hwInfo.regs)->XINTCTL))
            {
                if (Mcasp_TX_CLK_FAIL ==
                    (Mcasp_TX_CLK_FAIL & chanHandle->intStatus))
                {
                    status = IOM_EBADIO;
                    break;
                }
            }

            /* no sync error check for burst mode                             */
            if (0 != xmod)
            {
                if ((Mcasp_SYNCERR ==
                    (Mcasp_SYNCERR & chanHandle->intStatus)))
                {
                    status = IOM_ETIMEOUT;
                    break;
                }
            }
        }while(falsewhile);

        if (IOM_COMPLETED != status)
        {
            /* call the application registered callback if any                */
            if (NULL != (chanHandle->gblErrCbk))
            {
                (chanHandle->gblErrCbk)(status);
            }

            /* at this point we will decide whether to continue other         *
             * pending IOPs  or abort - it depends on error                   */
            /* For sync error we call the application callback and we         *
             * abort all other pending IOPs also                              */
            if (IOM_ETIMEOUT == status)
            {
                /* While aborting care should be taken about DLB mode         *
                 * otherwise aborting packets in only one direction will      *
                 * make the packet in other direction not to return           */
                dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                              & CSL_MCASP_DLBCTL_DLBEN_MASK)
                          >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

                if (CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
                {
                    status = Mcasp_localAbortReset(&(instHandle->RcvObj));
                    if (IOM_COMPLETED == status)
                    {
                        Mcasp_localAbortReset(&(instHandle->XmtObj));
                    }
                }
                else
                {
                    status = Mcasp_localAbortReset(chanHandle);
                }
                break;
            }

            /* if the error is a non- framesync error - we just send error    *
             * status for the current packet in the callback function and     *
             * continue with other packets for xfer                           */
            if (IOM_ETIMEOUT != status)
            {
                /* Disable Serializer, State machine and Frame sync of RCV    *
                 * Section.Also Disable EDMA Transfer.                        */
                Mcasp_localSmReset(chanHandle);

                /* Initiate dummy transfer (or reduce the c-count to 1) in    *
                 * EDMA-current transfer channel param This would load the    *
                 * next link paramset and give a edma callback where we       *
                 * do futher steps needed to close the current packet and     *
                 * load next pending packet etc                               */

                /* get the param table information of transfer channel        */
                EDMA3_DRV_getPaRAM(
                    chanHandle->edmaHandle,
                    chanHandle->xferChan,
                    &pramTbl);

                if (0 != pramTbl.cCnt)
                {
                    pramTbl.cCnt = 0;

                    EDMA3_DRV_setPaRAM(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        &pramTbl);
                }
                chanHandle->currentPacketErrorStatus = status;

                /* Enable Serializer, State machine and Frame sync of RCV     *
                 * Section. Also Enable EDMA Transfer.                        */
                Mcasp_localSmSet(chanHandle);
            }/*for errors except sync errors*/
        }/*dma transfer for all errors*/
    }while(falsewhile);
}


/**
 *  \brief    This function sets the GBLCTLR bit and wait and check that bit is
 *            set or not
 *
 *  \param    instHandle  [IN]  pointer to the mcasp instance object
 *  \param    bitMaskVal  [IN]  mask bit value
 *
 *  \return   None
 *
 *  \enter   hMcasp      is a valid non null pointer
 *           bitMaskVal  is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspBitSetGblRCtl(Mcasp_Object *instHandle,Uint32 bitMaskVal)
{
    Uint32 timeout = Mcasp_GBLCTL_TIMEOUT;
    Bool   isTask  = FALSE;

    assert(NULL != instHandle);

    /* If already this bit is set then don't set again                        */
    if (((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) != bitMaskVal)
    {
        if (FALSE == instHandle->RcvObj.isDmaDriven)
        {
            ECM_disableEvent(instHandle->RcvObj.cpuEventNum);
        }

        (instHandle->hwInfo.regs)->RGBLCTL =
             ((instHandle->hwInfo.regs)->RGBLCTL | bitMaskVal);

        while ((((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) !=
            bitMaskVal) && (timeout > 0u))
        {
            /* decrement the timeout count                                    */
            timeout--;

            isTask = TSK_isTSK();

            /* wait for 1 tick only if called from a task context             */
            if (TRUE == isTask)
            {
                TSK_sleep(1);
            }
        }

        assert(0 != timeout);

        if (FALSE == instHandle->RcvObj.isDmaDriven)
        {
            /* Leave critical section */
            ECM_enableEvent(instHandle->RcvObj.cpuEventNum);
        }
    }
}

/**
 *  \brief   This function removes the GBLCTLR bit and wait and checks that
 *           bit is set or not
 *
 *  \param   instHandle  [IN]  pointer to the mcasp instance object
 *  \param   bitMaskVal  [IN]  mask bit value
 *
 *  \return  None
 *
 *  \enter   hMcasp      is a valid non null pointer
 *           bitMaskVal  is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspBitRemoveGblRCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal)
{
    Uint32   temp_bitMaskVal = 0u;
    Uint32   timeout         = Mcasp_GBLCTL_TIMEOUT;
    Bool     isTask          = FALSE;

    temp_bitMaskVal = (0xFFFFFFFFu & (Uint32)bitMaskVal);

    assert(NULL != instHandle);

    /* If already this bit is set then don't set again                        */
    if (((instHandle->hwInfo.regs)->GBLCTL & ~temp_bitMaskVal)
        == (0x0000FFFFu & ~temp_bitMaskVal))
    {
        (instHandle->hwInfo.regs)->RGBLCTL =
            ((instHandle->hwInfo.regs)->RGBLCTL
            & (0xFFFF0000u | temp_bitMaskVal));

        while ((((instHandle->hwInfo.regs)->GBLCTL & ~temp_bitMaskVal)
            == (0x0000FFFFu & ~temp_bitMaskVal)) && (timeout > 0u))
        {
            /* decrement the timeout count                                    */
            timeout--;

            isTask = TSK_isTSK();

            /* wait for 1 tick only if called from a task context             */
            if (TRUE == isTask)
            {
                TSK_sleep(1);
            }

        }

        assert(0 != timeout);
    }
}

/**
 *  \brief   This function sets the GBLCTLX bit and wait and check that bit is
 *           set or not
 *
 *  \param   instHandle  [IN]  pointer to the mcasp instance object
 *  \param   bitMaskVal  [IN]  mask bit value
 *
 *  \return  None
 *
 *  \enter   hMcasp      is a valid non null pointer
 *           bitMaskVal  is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspBitSetGblXCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal)
{
    Uint32 timeout = Mcasp_GBLCTL_TIMEOUT;
    Bool   isTask  = FALSE;

    assert(NULL != instHandle);

    /* If already this bit is set then don't set again                        */
    if (((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) != bitMaskVal)
    {
        if (FALSE == instHandle->XmtObj.isDmaDriven)
        {
            ECM_disableEvent(instHandle->XmtObj.cpuEventNum);
        }

        (instHandle->hwInfo.regs)->XGBLCTL =
            ((instHandle->hwInfo.regs)->XGBLCTL | bitMaskVal);

        while ((((instHandle->hwInfo.regs)->GBLCTL & bitMaskVal) !=
            bitMaskVal) && (timeout > 0))
        {
            /* reduce the retry count value                                   */
            timeout--;

            isTask = TSK_isTSK();

            /* wait for 1 tick only if called from a task context             */
            if (TRUE == isTask)
            {
                TSK_sleep(1);
            }
        }

        assert(0 != timeout);

        if (FALSE == instHandle->XmtObj.isDmaDriven)
        {
            /* Leave critical section                                         */
            ECM_enableEvent(instHandle->XmtObj.cpuEventNum);
        }
    }
}

/**
 *  \brief  This function removes the GBLCTLX bit and wait and checks that bit is
 *          set or not.
 *
 *  \param  instHandle  [IN]  pointer to the mcasp instance object
 *  \param  bitMaskVal  [IN]  mask bit value
 *
 *  \return None
 *
 *  \enter   hMcasp      is a valid non null pointer
 *           bitMaskVal  is a valid value
 *
 *  \leave   Not implemented
 */
static Void mcaspBitRemoveGblXCtl(Mcasp_Object *instHandle, Uint32 bitMaskVal)
{
    volatile Uint32 temp_bitMaskVal = 0u;
    Uint32          timeout         = Mcasp_GBLCTL_TIMEOUT;
    Bool            isTask          = FALSE;

    temp_bitMaskVal = (0xFFFFFFFFu & (Uint32)bitMaskVal);

    assert(NULL != instHandle);

    /* If already this bit is set then don't set again                        */
    if (((instHandle->hwInfo.regs)->GBLCTL & ~temp_bitMaskVal) ==
        (0x0000FFFFu & ~temp_bitMaskVal))
    {
        (instHandle->hwInfo.regs)->XGBLCTL =
            ((instHandle->hwInfo.regs)->XGBLCTL
            & (0xFFFF0000u | temp_bitMaskVal));

        while ((((instHandle->hwInfo.regs)->GBLCTL & ~temp_bitMaskVal) ==
            (0x0000FFFFu & ~temp_bitMaskVal)) && (timeout > 0))
        {
            /* reduce the retry count value                                   */
            timeout--;

            isTask = TSK_isTSK();

            /* wait for 1 tick only if called from a task context             */
            if (TRUE == isTask)
            {
                TSK_sleep(1);
            }

        }
        assert(0 != timeout);
    }
}

/*
 *  This function configures the required serializer to transmit mode.
 *
 * \param    hMcasp   [IN]  pointer to the Mcasp Hardware information structure
 * \param    serNum   [IN]  serializer number to configure
 * \param    eb       [OUT] pointer to error block
 *
 * \return
 *           IOM_COMPLETED   - Successful completion
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 * \enter   hMcasp      is a valid non null pointer
 *          serNum      is a valid value
 * \leave   Not implemented
 */
static Void mcaspSetSerXmt(Mcasp_HwHandle       hMcasp,
                           Mcasp_SerializerNum  serNum)
{
    volatile Uint32  *pSrcCtl = NULL;

    assert(NULL != hMcasp);

    assert(serNum < hMcasp->numOfSerializers);

    /* we need not validate the address because it is already done in *
     * xs file                                                        */
    pSrcCtl = (((volatile Uint32*)&((hMcasp->regs)->SRCTL0)) + (Uint32)(serNum));

    if (NULL != pSrcCtl)
    {
        *pSrcCtl = ((*pSrcCtl) & ~(CSL_MCASP_SRCTL0_SRMOD_MASK))
                   | (CSL_MCASP_SRCTL0_SRMOD_XMT
                        << CSL_MCASP_SRCTL0_SRMOD_SHIFT);
    }

    (hMcasp->regs)->PDIR |= (1u << serNum);
}

/*
 *  This function configures the required serializer to receive mode.
 *
 * \param    hMcasp   [IN]  pointer to the Mcasp Hardware information structure
 * \param    serNum   [IN]  serializer number to configure
 * \param    eb       [OUT] pointer to error block
 *
 * \return  None
 *
 * \enter   hMcasp      is a valid non null pointer
 *          ditFlag     is a valid value
 * \leave   Not implemented
 */

static Void mcaspSetSerRcv(Mcasp_HwHandle       hMcasp,
                           Mcasp_SerializerNum  serNum)
{
    volatile Uint32 *pSrcCtl = NULL;

    assert(NULL != hMcasp);
    assert(serNum < hMcasp->numOfSerializers);

    pSrcCtl = (((volatile Uint32 *)&((hMcasp->regs)->SRCTL0)) +
                    (Uint32)(serNum));

    if (NULL != pSrcCtl)
    {
        *pSrcCtl = ((*pSrcCtl) & ~(CSL_MCASP_SRCTL0_SRMOD_MASK))
                   | (CSL_MCASP_SRCTL0_SRMOD_RCV << CSL_MCASP_SRCTL0_SRMOD_SHIFT);
    }

    (hMcasp->regs)->PDIR &= (~(1u << serNum));
}

/**
 *  \brief    This function enables/disables the DIT mode.
 *
 *  \param    hMcasp      [IN]     Handle to the McASP hardware info structure
 *  \param    ditFlag     [IN]     Value to loaded into the DITCTL register
 *  \param    eb      [OUT] Pointer to the error block
 *
 *  \return
 *           IOM_COMPLETED   - Successful completion
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 *  \enter   hMcasp      is a valid non null pointer
 *           ditFlag     is a valid value
 *  \leave   Not implemented
 */

static Int32 mcaspSetDitMode(Mcasp_HwHandle hMcasp,
                             Bool           ditFlag)
{
    Int32  status = IOM_COMPLETED;

    assert(NULL != hMcasp);

    if (TRUE == hMcasp->ditStatus)
    {
        /* Reset XSMRST and XFRST in GBLCTL */
        (hMcasp->regs)->GBLCTL =
            ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_XSMRST_MASK))
            | (CSL_MCASP_GBLCTL_XSMRST_RESET << CSL_MCASP_GBLCTL_XSMRST_SHIFT);

        (hMcasp->regs)->GBLCTL =
            ((hMcasp->regs)->GBLCTL & ~(CSL_MCASP_GBLCTL_XFRST_MASK))
            | (CSL_MCASP_GBLCTL_XFRST_RESET << CSL_MCASP_GBLCTL_XFRST_SHIFT);

        /* Configure DITEN bit of DITCTL    */
        (hMcasp->regs)->DITCTL =
            ((hMcasp->regs)->DITCTL & ~(CSL_MCASP_DITCTL_DITEN_MASK))
            | (ditFlag << CSL_MCASP_DITCTL_DITEN_SHIFT);
    }

    return (status);
}

/**
 *  \brief    This function registers the interrupt handler for the given
 *            channel.
 *
 *  \param    chanp        [IN]    Pointer to channel object
 *  \param    instNum      [IN]    instance number
 *  \param    evt          [IN]    event number
 *  \param    intIsr       [IN]    function pointer
 *  \param    eb           [OUT]   pointer to error block
 *
 *  \return   If success return IOM_COMPLETED else error code is
 *            returned
 *
 *  \enter   chanp       is a valid non null pointer
 *           instNum     is a valid value
 *           evt         is a valid value
 *           intIsr      is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
static Void mcaspRegisterInt(Ptr      chanp,
                             Uint32   instNum,
                             Uint16   evt,
                             ECM_Fxn  intIsr)
{
    Mcasp_ChannelHandle  chanHandle    = NULL;
    static Uns           hwiKey        = 0;
    Mcasp_Object        *instHandle    = NULL;
    ECM_Attrs            attrs         = ECM_ATTRS;

    assert((NULL != chanp) && (NULL != intIsr));

    /* get the handle to the channel to register the interrupt                */
    chanHandle = (Mcasp_ChannelHandle)chanp;

    assert((NULL != chanHandle) && (NULL != chanHandle->devHandle));

    instHandle = (Mcasp_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    hwiKey = (Uint32)_disable_interrupts();

    /* set the variable to indicate that this channel has registered          *
     * interrupts                                                             */
    Mcasp_module.isrObject[instNum].chanEnabled[chanHandle->mode - 1] = TRUE;

    /* update the information of the instance which is registering the ISR    */
    if (FALSE == Mcasp_module.isrObject[instNum].isIsrRegistered)
    {
        Mcasp_module.isrObject[instNum].isIsrRegistered = TRUE;
        Mcasp_module.isrObject[instNum].instHandle = instHandle;

        attrs.arg = (Arg)Mcasp_module.isrObject[instNum].isrSwiTaskHandle;
        attrs.unmask = 1u;

        /* Plug the ISR with interrupt (event) number                         */
        ECM_dispatchPlug(evt,(ECM_Fxn)intIsr,&attrs);

        /* Enabling the the interrupt                                         */
        C64_enableIER(1 << instHandle->hwiNumber);
    }
    else
    {
        /* interrupt handler is already plugged in for this instance          */
    }

    /* Enabling the interrupts                                                */
    _restore_interrupts(hwiKey);
}

/**
 *  \brief    unregister interrupt handler for the given channel.
 *
 *  \param    devid        [IN]    instance number
 *  \param    evt          [IN]    interrupt number
 *  \param    mode         [IN]    input or output
 *
 *  \return   Nothing
 *
 *  \enter   Not implemented
 *
 *  \leave   Not implemented
 */
static Void mcaspUnRegisterInt(Uint16 devid, Uint16 evt,Int mode)
{
    Uint32        hwiKey        = 0;
    Uint32        count         = 0;
    Bool          unRegisterInt = TRUE;

    /* disable the interrupts before unregistering the interrupts             */
    hwiKey = (Uint32)_disable_interrupts();

    /* we will unregister the interrupt only if all the instance's TX and RX  *
     * interrupt handlers have been unregistered                              */

    /* indicate that the channel has unregisterd the interrupts               */
    Mcasp_module.isrObject[devid].chanEnabled[mode-1] = FALSE;

    /* check if the current instance has unregistered both of its interrupt   *
     * handler if yes the remove the reference to this instance from the      *
     * "Mcasp_isrInstanceObject"                                              */
    if ((FALSE == Mcasp_module.isrObject[devid].chanEnabled[0])
        && (FALSE == Mcasp_module.isrObject[devid].chanEnabled[1]))
    {
        Mcasp_module.isrObject[devid].isIsrRegistered = FALSE;
        Mcasp_module.isrObject[devid].instHandle = NULL;
    }

    /* check if All the instances have unregistered their interrupts          */
    for (count = 0; count < CSL_MCASP_PER_CNT; count++)
    {
        if (TRUE
            == Mcasp_module.isrObject[count].isIsrRegistered)
        {
            unRegisterInt = FALSE;
            break;
        }
    }

    if (TRUE == unRegisterInt)
    {
        /* None of the instances have interrupts registered.Hence we can      *
         * Disable the event now.                                             */
        ECM_disableEvent(evt);
    }

    /* Enabling the interrupts                                                */
    _restore_interrupts(hwiKey);
}

/**
 *  \brief   This function Deactivates the receive section state machine and
 *           the corresponding frame sync.
 *
 *  \param   instHandle  [IN]  pointer to the Mcasp instance object
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localDeactivateSmFsForRcv(Mcasp_Object *instHandle)
{
    assert(NULL != instHandle);

    (instHandle->hwInfo.regs)->RSTAT = ((CSL_MCASP_RSTAT_RERR_MASK
                                       | CSL_MCASP_RSTAT_RDMAERR_MASK
                                       | CSL_MCASP_RSTAT_RSTAFRM_MASK
                                       | CSL_MCASP_RSTAT_RDATA_MASK
                                       | CSL_MCASP_RSTAT_RLAST_MASK
                                       | CSL_MCASP_RSTAT_RTDMSLOT_MASK
                                       | CSL_MCASP_RSTAT_RCKFAIL_MASK
                                       | CSL_MCASP_RSTAT_RSYNCERR_MASK
                                       | CSL_MCASP_RSTAT_ROVRN_MASK));

    /*frame sync*/
    mcaspBitRemoveGblRCtl(instHandle, ~CSL_MCASP_RGBLCTL_RFRST_MASK);

    /*state m/c*/
    mcaspBitRemoveGblRCtl(instHandle, ~CSL_MCASP_RGBLCTL_RSMRST_MASK);

    /*serializer*/
    mcaspBitRemoveGblRCtl(instHandle, ~CSL_MCASP_RGBLCTL_RSRCLR_MASK);
}

/**
 *  \brief   This function deactivates the transmit section state machine and
 *           the corresponding frame sync.
 *
 *  \param   instHandle [IN]  Pointer to the mcasp instance object
 *
 *  \return  None
 *
 *  \enter   instHandle  is a valid non null pointer
 *
 *  \leave   Not implemented
 */
Void Mcasp_localDeactivateSmFsForXmt(Mcasp_Object *instHandle)
{
    assert(NULL != instHandle);

    (instHandle->hwInfo.regs)->XSTAT = ((CSL_MCASP_XSTAT_XERR_MASK
                                         |CSL_MCASP_XSTAT_XDMAERR_MASK
                                         |CSL_MCASP_XSTAT_XSTAFRM_MASK
                                         |CSL_MCASP_XSTAT_XDATA_MASK
                                         |CSL_MCASP_XSTAT_XLAST_MASK
                                         |CSL_MCASP_XSTAT_XTDMSLOT_MASK
                                         |CSL_MCASP_XSTAT_XCKFAIL_MASK
                                         |CSL_MCASP_XSTAT_XSYNCERR_MASK
                                         |CSL_MCASP_XSTAT_XUNDRN_MASK));

    /*Start the Serializer, State M/C and Frame Sync Generator*/
    mcaspBitRemoveGblXCtl(instHandle, ~CSL_MCASP_XGBLCTL_XFRST_MASK);
    mcaspBitRemoveGblXCtl(instHandle, ~CSL_MCASP_XGBLCTL_XSMRST_MASK);
    mcaspBitRemoveGblXCtl(instHandle, ~CSL_MCASP_XGBLCTL_XSRCLR_MASK);

}

/**
 *  \brief  This function checks if the requested serialisers are free or not
 *
 *  \param   instHandle  [IN]  pointer to the Mcasp instance object
 *  \param   chanParams  [IN]  Device parameter structure that supplies
 *                            data to initialize the McASP
 *  \param  eb           [OUT] pointer to the error information block
 *
 *  \return TRUE if all serializers are free (Mcasp_SerializerStatus_FREE)
 *          FALSE if any one of the requested serializer is not free
 *
 *  \enter   instHandle  is a valid non null pointer
 *           params      is a valid non null pointer
 *
 *  \leave   Not implemented
 */
static Bool mcaspIsRequestedSerzsFree(Mcasp_Object *instHandle,
                                      Ptr           params)
{
    Mcasp_ChanParams  *chanParams = NULL;
    Uint32             count      = 0 ;
    Bool               status     = TRUE;
    Bool               falsewhile = TRUE;

    assert((NULL != instHandle) && (NULL != params));

    do
    {
        falsewhile  = FALSE;

        chanParams = (Mcasp_ChanParams *)params;

        for (count = 0; count < chanParams->noOfSerRequested; count++)
        {
            /* Get serializer number requested by user                        */
            if (Mcasp_SerializerStatus_FREE !=
               (instHandle->serStatus[chanParams->indexOfSersRequested[count]]))
            {
                status = FALSE;
                break;
            }
        }
    }while(falsewhile);

    return (status);
}

/**
 *  \brief   This function is used to configure the requested serializers in the
 *           receive mode.
 *
 *  \param    instHandle  [IN]   pointer to the Mcasp instance object
 *  \param    params      [IN]   channel parameters given by user
 *  \param    eb          [OUT]  pointer to the Error infromation block
 *
 *  \return   None
 *
 *  \enter   instHandle  is a valid non null pointer
 *           params      is a valid non null pointer
 *
 *  \leave   Not implemented
 */
static Void mcaspMultiSerInitializationRcv(Mcasp_Object *instHandle,
                                           Ptr           params)
{
    Mcasp_ChanParams     *chanParams = NULL;
    Uint32               count       = 0;
    Uint32               serNum      = 0;
    Mcasp_SerializerNum  serializerToConfigure = (Mcasp_SerializerNum)0;

    assert((NULL != instHandle) && (NULL != params));

    chanParams = (Mcasp_ChanParams *)params;

    for (count = 0; count < chanParams->noOfSerRequested; count++)
    {
        /* Get serializer number requested by user                            */
        serNum = (Uint32)chanParams->indexOfSersRequested[count] ;

        if (Mcasp_SerializerStatus_FREE == instHandle->serStatus[serNum])
        {
            /* Configure requested serialzer as receive mode                  */
            instHandle->serStatus[serNum] = Mcasp_SerializerStatus_RCV;
            serializerToConfigure = (Mcasp_SerializerNum)serNum;

            mcaspSetSerRcv(&(instHandle->hwInfo),serializerToConfigure);
        }
    }
}


/**
 *  \brief   This function configures the requested serializers in the transmit
 *           mode
 *
 *  \param    instHandle  [IN]   pointer to the Mcasp instance object
 *  \param    params      [IN]   channel parameters given by user
 *  \param    eb          [OUT]  pointer to the Error infromation block
 *
 *  \return   None
 *
 *  \enter   instHandle  is a valid non null pointer
 *          params      is a valid non null pointer
 *
 *  \leave   Not implemented
 */

static Void mcaspMultiSerInitializationXmt(Mcasp_Object *instHandle,
                                           Ptr           params)
{
    Mcasp_ChanParams      *chanParams  = NULL;
    Uint32                 count       = 0;
    Uint32                 serNum      = 0;
    Mcasp_SerializerNum    serializerToConfigure = (Mcasp_SerializerNum)0;

    assert((NULL != instHandle) && (NULL != params));

    chanParams = (Mcasp_ChanParams *)params;

    for (count = 0; count < chanParams->noOfSerRequested; count++)
    {
        /* Get serializer number requested by user                            */
        serNum = (Uint32)chanParams->indexOfSersRequested[count] ;

        if (Mcasp_SerializerStatus_FREE == instHandle->serStatus[serNum])
        {
            /* Configure requested serialzer as transmit mode                 */
            instHandle->serStatus[serNum] = Mcasp_SerializerStatus_XMT;

            serializerToConfigure = (Mcasp_SerializerNum)serNum;

            mcaspSetSerXmt(&(instHandle->hwInfo),serializerToConfigure);
        }
    }

}


/**
 *  \brief   This function restore global serializer state as available
 *           for further use.
 *
 *  \param   chanp  [IN]   Channel handle
 *
 *  \return  None
 *
 *  \enter   chanp       is a valid non null pointer
 *
 *  \leave   Not implemented
 */
static Void mcaspMultiSerGlobalRestore(Ptr chanp)
{
    Mcasp_ChannelHandle     chanHandle  = NULL;
    Mcasp_Object           *instHandle  = NULL;
    Uint32                  serNum      = 0;

    assert(NULL != chanp);

    chanHandle = (Mcasp_ChannelHandle)chanp;

    assert(NULL != chanHandle->devHandle);

    instHandle = (Mcasp_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    for (serNum=0; serNum < chanHandle->noOfSerAllocated; serNum++)
    {
        instHandle->serStatus[chanHandle->indexOfSersRequested[serNum]] =
            Mcasp_SerializerStatus_FREE;
    }
}


/**
 *  \brief   This function configures the Mcasp transmit sections
 *
 *  \param   instHandle  [IN]   pointer to the Mcasp instance object
 *  \param   params      [IN]   transmit channel parameters
 *  \param   eb          [OUT]  pointer to error block
 *
 *  \return  IOM_COMPLETED If configuration is success
 *           Error code in case of failure.
 *
 *  \enter   instHandle  is a valid non null pointer
 *          params      is a valid non null pointer
 *
 *  \leave   Not implemented
 */
static Int32 mcaspConfigureXmtChannel(Mcasp_Object *instHandle,
                                      Ptr           params)
{
    Mcasp_ChanParams  *chanParams = NULL;
    Int32              status     = IOM_COMPLETED;
    Bool               falsewhile = TRUE;

    assert((NULL != instHandle) && (NULL != params));

    do
    {
        falsewhile  = FALSE;

        chanParams = (Mcasp_ChanParams *)params;

        /* before touching McASP registers, place transmit section in reset   */
        status = Mcasp_localResetXmt(&(instHandle->hwInfo));

        if (IOM_COMPLETED != status)
        {
            break;
        }

        /* Configure the transmit section of the device: Start transmit high  *
         * clock and clock                                                    */
        status = Mcasp_localConfigXmtSection(
                    instHandle,
                    chanParams->mcaspSetup);

        if (IOM_COMPLETED != status)
        {
            break;
        }

        /* Set DITEN bit if DIT mode else set to I2S mode.                    */
        if (Mcasp_OpMode_DIT == chanParams->channelMode)
        {
            /* check hardware supports dit mode or not                        */
            if (TRUE == instHandle->hwInfo.ditStatus)
            {
                status = mcaspSetDitMode(
                            &(instHandle->hwInfo),
                            (Bool)chanParams->channelMode);

                if (IOM_COMPLETED != status)
                {
                    break;
                }
            }
        }

    }while(falsewhile);

    return(status);
}

/**
 *  \brief   This function configures the receive section of the mcasp device.
 *
 *  \param   instHandle  [IN]  pointer to the Mcasp instance object
 *  \param   params      [IN]  receive channel parameters
 *  \param   eb          [OUT] pointer to error block
 *
 *  \return  IOM_COMPLETED in case of success
 *           Error code in case of failure
 *
 *  \enter   instHandle  is a valid non null pointer
 *          params      is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */

static Int32 mcaspConfigureRcvChannel(Mcasp_Object *instHandle,
                                      Ptr           params)
{
    Mcasp_ChanParams    *chanParams  = NULL;
    Int32                status      = IOM_COMPLETED ;
    Bool                 falsewhile  = TRUE;

    assert((NULL != instHandle) && (NULL != params));

    do
    {
        falsewhile  = FALSE;

        chanParams = (Mcasp_ChanParams *)params;

        /* before touching McASP registers, place receive section in reset    */
        status = Mcasp_localResetRcv(&(instHandle->hwInfo));

        if (IOM_COMPLETED != status)
        {
            break;
        }

        /* Configure the receive section of the device: start Receive high    *
         * clock and clock                                                    */
        status = Mcasp_localConfigRcvSection(
                    instHandle,
                    chanParams->mcaspSetup);

        if (IOM_COMPLETED != status)
        {
            break;
        }

    }while(falsewhile);

    return (status);
}


/**
 *  \brief   This function writes to the Channel status RAM (DITCSRA/B0-5)
 *
 *  \param   hMcasp      [IN]     Handle to the McASP instance
 *  \param   chanStatRam [IN]     status to be written
 *  \param   eb          [OUT]    pointer to the error information block
 *
 *  \return
 *           IOM_COMPLETED   - Successful completion of read
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 *  \enter   hMcasp      is a valid non null pointer
 *          chanStatRam is a valid non null pointer
 *
 *  \leave   Not implemented
 */

static Int32 mcaspWriteChanStatRam(Mcasp_HwHandle      hMcasp,
                                   Mcasp_ChStatusRam  *const chanStatRam)
{
    Int32   status = IOM_COMPLETED;

    assert((NULL != hMcasp) && (NULL != chanStatRam));

    if (TRUE == hMcasp->ditStatus)
    {
        /* Configure the DIT left channel status registers                    */
        (hMcasp->regs)->DITCSRA0 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_0];
        (hMcasp->regs)->DITCSRA1 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_1];
        (hMcasp->regs)->DITCSRA2 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_2];
        (hMcasp->regs)->DITCSRA3 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_3];
        (hMcasp->regs)->DITCSRA4 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_4];
        (hMcasp->regs)->DITCSRA5 =
            chanStatRam->chStatusLeft[Mcasp_DITRegIndex_5];

        /* Configure the DIT right channel status registers                   */
        (hMcasp->regs)->DITCSRB0 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_0];
        (hMcasp->regs)->DITCSRB1 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_1];
        (hMcasp->regs)->DITCSRB2 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_2];
        (hMcasp->regs)->DITCSRB3 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_3];
        (hMcasp->regs)->DITCSRB4 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_4];
        (hMcasp->regs)->DITCSRB5 =
            chanStatRam->chStatusRight[Mcasp_DITRegIndex_5];
    }
    else
    {
        status = IOM_EBADARGS;
    }

    return (status);
}

/**
 *  \brief   This function writes to the User Data RAM (DITUDRA/B0-5)
 *
 *  \param   hMcasp      [IN] pointer to the Mcasp H/W information structure
 *  \param   userDataRam [IN] data to be written
 *
 *  \return
 *           IOM_COMPLETED   - Successful completion of write
 *           IOM_EBADARGS    - Invalid parameters or Invalid handle
 *
 *  \enter   hMcasp      is a valid non null pointer
 *          userDataRam is a valid non null pointer
 *
 *  \leave   Not implemented
 *
 */
static Int32 mcaspWriteUserDataRam(Mcasp_HwHandle      hMcasp,
                                   Mcasp_UserDataRam  *const userDataRam)
{
    Int32   status = IOM_COMPLETED;

    assert((NULL != hMcasp) && (NULL != userDataRam));

    if (TRUE == hMcasp->ditStatus)
    {
        /* Configure the DIT left user data registers                         */
        (hMcasp->regs)->DITUDRA0 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_0];
        (hMcasp->regs)->DITUDRA1 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_1];
        (hMcasp->regs)->DITUDRA2 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_2];
        (hMcasp->regs)->DITUDRA3 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_3];
        (hMcasp->regs)->DITUDRA4 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_4];
        (hMcasp->regs)->DITUDRA5 =
            userDataRam->userDataLeft[Mcasp_DITRegIndex_5];

        /* Configure the DIT right user data registers                        */
        (hMcasp->regs)->DITUDRB0 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_0];
        (hMcasp->regs)->DITUDRB1 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_1];
        (hMcasp->regs)->DITUDRB2 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_2];
        (hMcasp->regs)->DITUDRB3 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_3];
        (hMcasp->regs)->DITUDRB4 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_4];
        (hMcasp->regs)->DITUDRB5 =
            userDataRam->userDataRight[Mcasp_DITRegIndex_5];
    }
    else
    {
        status = IOM_EBADARGS;
    }
    return (status);
}

/**
 *
 * \brief   This function configures the Hardware FIFO
 *
 * \param   hMcasp       [IN] pointer to the Mcasp Hardware information struct
 * \param   chanHandle   [IN] handle to the channel
 * \param   enableHwFifo [IN] hardware FIFO to be enabled or disabled
 *
 * \return  None
 *
 * \enter   hMcasp      is a valid non null pointer
 *          chanHandle  is a valid non null pointer
 *
 * \leave   Not implemented
 *
 */
static Void mcaspConfigureFifo(Mcasp_HwObj         *hMcasp,
                               Mcasp_ChannelObj    *chanHandle,
                               Bool                 enableHwFifo)

{
    /* validate the input parameters                                          */
    assert((NULL != hMcasp) && (NULL != hMcasp->fifoRegs)
            && (NULL != chanHandle));

    /* check if the HW FIFO usage is requested by the user for this channel   */
    if (TRUE == enableHwFifo)
    {
        if (IOM_INPUT == chanHandle->mode)
        {
            /* configure the receive channel                                  */
            /* Step 1 : configure the "WNUMDMA" and "WNUMEVT" bits before     *
             * enabling the FIFO                                              */
            hMcasp->fifoRegs->RFIFOCTL =
                ((chanHandle->noOfSerAllocated) <<
                     CSL_AFIFO_RFIFOCTL_RNUMEVT_SHIFT)
                | (chanHandle->noOfSerAllocated);

            /* enable the FIFO now by setting the "WENA" bit                  */
            hMcasp->fifoRegs->RFIFOCTL |=
                (CSL_AFIFO_RFIFOCTL_RENA_ENABLED
                    << CSL_AFIFO_RFIFOCTL_RENA_SHIFT);
        }
        else
        {
            /* configure the transmit channel                                 */
            /* Step 1 : configure the "WNUMDMA" and "WNUMEVT" bits before     *
             * enabling the FIFO                                              */
            hMcasp->fifoRegs->WFIFOCTL =
                ((chanHandle->noOfSerAllocated) <<
                     CSL_AFIFO_WFIFOCTL_WNUMEVT_SHIFT)
                | (chanHandle->noOfSerAllocated);

            /* enable the FIFO now by setting the "WENA" bit                  */
            hMcasp->fifoRegs->WFIFOCTL |=
                (CSL_AFIFO_WFIFOCTL_WENA_ENABLED
                    << CSL_AFIFO_WFIFOCTL_WENA_SHIFT);
        }
    }
    else
    {
        /* FIFO needs to be disabled                                          */
        if (IOM_INPUT == chanHandle->mode)
        {
            /* disable the FIFO now by resetting the "WENA" bit               */
            hMcasp->fifoRegs->RFIFOCTL =
                (CSL_AFIFO_RFIFOCTL_RENA_DISABLED
                    << CSL_AFIFO_RFIFOCTL_RENA_SHIFT);
        }
        else
        {
            /* disable the FIFO now by resetting the "WENA" bit               */
            hMcasp->fifoRegs->WFIFOCTL =
                (CSL_AFIFO_WFIFOCTL_WENA_DISABLED
                    << CSL_AFIFO_WFIFOCTL_WENA_SHIFT);
        }
    }
}


/**
 *
 * \brief This function validates the application specified buffer formats and
 *          updates the channel handle with the choosen buffer format.
 *
 * Side effect: This function also update the channel environment variable
 *              "noOfSlots" that would be used to compute the EDMA 3 indices.
 *
 * \param   chanHandle    [IN]    Pointer to channel handle
 * \param   params        [IN]    user supplied channel configurations.
 *
 * \return  DriverTypes_COMPLETED on sucess else an error code is returned
 *
 * \enter   chanHandle     is a valid non null pointer
 *          params         is a valid non null pointer
 *
 * \leave   Not implemented
 */

static Int32  mcaspValidateBufferConfig(Mcasp_ChannelHandle  chanHandle,
                                        Mcasp_ChanParams    *const params)
{
    Uint32  tempVar   = 0x0;
    Int32   status    = IOM_COMPLETED;

    assert((NULL != chanHandle) && (params != NULL));

    /* get the number of slots to be configured                               */
    tempVar = ((0x0000FF80u) & params->mcaspSetup->frSyncCtl);
    tempVar = tempVar >> 7;

    if (0x01 < params->noOfSerRequested)
    {
        /* multi serializer configuration                                     */
        if (0x01 < tempVar)
        {
            /* multi serialiser multi slot configuration                      */
            if ((Mcasp_BufferFormat_MULTISER_MULTISLOT_SEMI_INTERLEAVED_1
                != params->dataFormat) &&
                (Mcasp_BufferFormat_MULTISER_MULTISLOT_SEMI_INTERLEAVED_2
                != params->dataFormat))
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            /* Multi serialiser single slot configuration                     */
            if ((Mcasp_BufferFormat_MULTISER_1SLOT_SER_INTERLEAVED
                != params->dataFormat) &&
                (Mcasp_BufferFormat_MULTISER_1SLOT_SER_NON_INTERLEAVED
                != params->dataFormat))
            {
                status = IOM_EBADARGS;
            }
        }
    }
    else
    {
        /* single serialiser configuration                                    */
        if (0x01 < tempVar)
        {
            /* single serializer multi slot configuration                     */
            if ((Mcasp_BufferFormat_1SER_MULTISLOT_INTERLEAVED
                != params->dataFormat) &&
                (Mcasp_BufferFormat_1SER_MULTISLOT_NON_INTERLEAVED
                != params->dataFormat))
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            /* single slot configuration                                      */
            if (Mcasp_BufferFormat_1SER_1SLOT != params->dataFormat)
            {
                status = IOM_EBADARGS;
            }
        }
    }

    /* update the number of slots in to the channel structure                 */
    chanHandle->noOfSlots = (Uint16)params->noOfChannels;

    if (IOM_COMPLETED == status)
    {
        /* copy the data format to the channel structure                      */
        chanHandle->dataFormat = params->dataFormat;
    }
    return status;
}

/*
 *  \brief  This function toggles the index of the edma params
 *
 *  \param  index [IN]  pointer to current index
 *
 *  \return None
 *
 *  \enter  Not implemented
 *
 *  \leave  Not implemented
 */
Void Mcasp_localGetNextIndex(Uint32 *index)
{
    assert(NULL != index);
    *index = (((*index) + 1u) & 0x01u);
}

/**
 *  \brief  This function loads the packets to the actual EDMA paramset.
 *
 *  \param  chanHandle [IN]  Handle to channel.
 *  \param  ioPacket   [IN]  pointer to the ioPacket
 *
 *  \return None
 *
 *  \enter  Not implemented
 *
 *  \leave  Not implemented
 */

Void Mcasp_localLoadPktToEdma(Mcasp_ChannelObj *chanHandle,IOM_Packet *ioPacket)
{
    Mcasp_Object           *instHandle  = NULL;
    Mcasp_PktAddrPayload   *payLoad     = NULL;
    Int32                   status      = IOM_COMPLETED;
    Bool                    falseWhile  = TRUE;
    Uint32                  hwiKey      = 0;
#ifndef Mcasp_LOOPJOB_ENABLED
    Uint32                  dlbMode     = 0;
    EDMA3_DRV_TrigMode      trigMode    = EDMA3_DRV_TRIG_MODE_EVENT;
#endif

    assert(NULL != chanHandle);
    assert(NULL != ioPacket);    

    instHandle = (Mcasp_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    do
    {
        falseWhile = FALSE;

        if (TRUE == instHandle->isDataBufferPayloadStructure)
        {
            payLoad = (Mcasp_PktAddrPayload *)ioPacket->addr;
            assert(NULL != payLoad);
        }

        /* For a DIT  request,  channel has to be programmed with its contents*
         * (DIT RAM registers also if this is an xmt channel in DIT mode).    */
        if (Mcasp_ChanMode_RCV != chanHandle->channelOpMode)
        {
            if ((Mcasp_ChanMode_XMT_DIT == chanHandle->channelOpMode)
                && (TRUE == instHandle->isDataBufferPayloadStructure)
                && (TRUE == payLoad->writeDitParams))
            {
                /* DIT Mode: Write to the Channel Status RAM and User Data RAM*/
                status = mcaspWriteChanStatRam(
                              &(instHandle->hwInfo),
                              payLoad->chStat);

                status |= mcaspWriteUserDataRam(
                               &(instHandle->hwInfo),
                               payLoad->userData);

                if (IOM_COMPLETED != status)
                {
                    status = IOM_ABORTED;
                    break;
                }
            }
        }

        chanHandle->currentDataSize = (Uint16)ioPacket->size;
        chanHandle->userDataBufferSize = (Uint32)ioPacket->size;

        /* for DMA mode we will not use chanHandle->dataPacket to hold        *
         * current packet - we use floatingqueue                              */
        chanHandle->dataPacket = NULL;

        /*  Enter critical section to protect the submit count                */
        hwiKey = (Uint32)_disable_interrupts();

#ifdef Mcasp_LOOPJOB_ENABLED
        if (2u == chanHandle->submitCount)
#else
        /* The second and the third packet will go the link paramsets         */
        if (2u <= chanHandle->submitCount)
#endif
        {
            chanHandle->loopjobUpdatedinParamset = FALSE;
            Mcasp_localGetNextIndex(&chanHandle->nextLinkParamSetToBeUpdated);
        }

        status = Mcasp_localUpdtDtPktToLnkPrms(chanHandle,ioPacket);

        if (IOM_COMPLETED != status)
        {
            status = IOM_EBADIO;

            /*  Enter critical section to protect the submit count            */
            _restore_interrupts(hwiKey);
            break;
        }

        if (1u == chanHandle->submitCount)
        {
            /* First packet is being submitted in case of loop job        *
             * mode this will ne loaded in to the link paramset else      *
             * in case of the non loop job this will go in to the main    *
             * channel                                                    */
#ifdef Mcasp_LOOPJOB_ENABLED
            /* if at all this is the very first packet, then one param    *
             * set has loop job loaded , self linked and active with      *
             * the main xfer channel param. other param set is ready      *
             * loaded (just now and has link paramater set as the one     *
             * having loopjob (this is to ensure that if at all we are    *
             * not getting any more packets loopjob be will taken         *
             * over). Now we have to link the floating / newly loaded     *
             * param set to xfer channel.                                 */
            status = EDMA3_DRV_linkChannel(
                        chanHandle->edmaHandle,
                        chanHandle->xferChan,
                        chanHandle->pramTbl \
                        [chanHandle->nextLinkParamSetToBeUpdated]);

            if (IOM_COMPLETED != status)
            {
                status = IOM_EBADIO;
                /*  exit critical section to protect the submit count     */
                _restore_interrupts(hwiKey);
                break;
            }
#endif
            _restore_interrupts(hwiKey);

#ifndef Mcasp_LOOPJOB_ENABLED
            chanHandle->nextLinkParamSetToBeUpdated = 0;

            /* configure the FIFO for the specific channel                    */
            if (TRUE == chanHandle->enableHwFifo)
            {
                /* disable and enable the FIFO to clear any data in the FIFO  */
                mcaspConfigureFifo(&instHandle->hwInfo,chanHandle,FALSE);
                mcaspConfigureFifo(&instHandle->hwInfo,chanHandle,TRUE);
            }

            /* This is the first packet. we would have update the main params *
             * with the ioPacket. we will now just start the transfer so that *
             * the packet is transferred                                      */
            EDMA3_DRV_enableTransfer(
                chanHandle->edmaHandle,
                chanHandle->xferChan,
                trigMode);

            /*  update the State m/c stop status                              */
            instHandle->stopSmFsXmt = FALSE;
            instHandle->stopSmFsRcv = FALSE;
        
            /* if DLB mode is set then Enable both state m/c else rcv or xmt  *
             * state machine only needs to be enabled                         */
            dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                        & CSL_MCASP_DLBCTL_DLBEN_MASK)
                        >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);

            if (CSL_MCASP_DLBCTL_DLBEN_DISABLE == dlbMode)
            {
                if (IOM_INPUT == chanHandle->mode)
                {
                    /* Receive channel */
                    /* activate RX and TX state machine and frame sync*/
                    ECM_enableEvent(chanHandle->cpuEventNum);
                    Mcasp_localActivateSmFsForRcv(instHandle);
                }
                else
                {
                    /* Transmit channel */
                    /* activate RX and TX state machine and frame sync*/
                    ECM_enableEvent(chanHandle->cpuEventNum);
                    Mcasp_localActivateSmFsForXmt(instHandle);
                }
            }
            else
            {
                /***        DLB Mode      ****/
                /* activate RX and TX state machine and frame sync    */
                ECM_enableEvent(instHandle->hwInfo.cpuRxEventNum);
                ECM_enableEvent(instHandle->hwInfo.cpuTxEventNum);
                Mcasp_localActivateSmFsForBoth(instHandle);
            }
#endif
        }
    } while (falseWhile);
}


#ifndef Mcasp_LOOPJOB_ENABLED
/*
 *  \brief  This function waits for the FIFO to be emptied so that the Framesync
 *          can be stopped.here after the FIFO is empty.
 *
 *  \param  arg0   [IN]  Handle to the TX channel
 *  \param  arg1   [IN]  unused
 *
 *  \return None
 *
 *  \enter  Not implemented
 *
 *  \leave  Not implemented
 */
Void Mcasp_swiTxFifo(Arg arg0,Arg arg1)
{
    Mcasp_ChannelObj  *chanHandle = NULL;
    Mcasp_Object      *instHandle = NULL;
    Uint32             timeOut    = 0x00;
    Uint32             dlbMode    = 0x00;

    chanHandle = (Mcasp_ChannelObj *)arg0;
    instHandle = (Mcasp_Object *)arg1;

    assert(NULL != chanHandle);
    assert(NULL != instHandle);

    /* update the timeout value from the instance handle                      */
    timeOut = instHandle->retryCount;

    dlbMode = (((instHandle->hwInfo.regs)->DLBCTL
                 & CSL_MCASP_DLBCTL_DLBEN_MASK)
                  >> CSL_MCASP_DLBCTL_DLBEN_SHIFT);


    /* we have come here means that the Mcasp has got an callback but it      *
     * did have any more packet to load Hence here we will wait for the       *
     * FIFO to become empty (if FIFO is enabled) else wait for the TX to      *
     * become empty.then we will disable the TX section                       */
    if (TRUE == chanHandle->enableHwFifo)
    {
        while ((0 != (instHandle->hwInfo.fifoRegs->WFIFOSTS &
                       CSL_AFIFO_WFIFOSTS_WLVL_MASK))
                && (0 != timeOut))
        {
            /* reduce the timeout count and check if the FIFO is empty        */
            timeOut--;
        }
    }

    /* reinitialize the retry count                                           */
    timeOut = instHandle->retryCount;
    
    while ((CSL_MCASP_XSTAT_XUNDRN_MASK !=
        (instHandle->hwInfo.regs->XSTAT & CSL_MCASP_XSTAT_XUNDRN_MASK))
         && (0 != timeOut))
    {
        /* reduce the retry count and check if the TX has completed           *
         * transmitting all the bytes                                         */
        timeOut--;
    }

    /* Now we can disable the frame sync                                      */
    if( CSL_MCASP_DLBCTL_DLBEN_ENABLE == dlbMode)
    {
        Mcasp_localDeactivateSmFsForRcv(instHandle);
        Mcasp_localDeactivateSmFsForXmt(instHandle);
    }
    else
    {
        /* Reset state m/c & frame sync of TX section                         */
        Mcasp_localDeactivateSmFsForXmt(instHandle);
    }

    if (CSL_MCASP_XINTCTL_XUNDRN_MASK ==
        (CSL_MCASP_XINTCTL_XUNDRN_MASK & chanHandle->userIntValue))
    {
        /* clear the underrun error                                           */
        instHandle->hwInfo.regs->XSTAT |= CSL_MCASP_XSTAT_XUNDRN_MASK;

        /* enable the underrun error                                          */
        instHandle->hwInfo.regs->XINTCTL |= (CSL_MCASP_XSTAT_XUNDRN_MASK);
    }

    /* clear the error bits in the EDMA(as this is the last packet)           */
    EDMA3_DRV_clearErrorBits(
        chanHandle->edmaHandle,
        chanHandle->xferChan);

    /* complete the IOP now and call the callback to the stream               */
    chanHandle->tempPacket = QUE_get(&(chanHandle->queueFloatingList));

    /* Decrement the submit count for the IOpackets                           */
    chanHandle->submitCount--;

    chanHandle->isTempPacketValid = TRUE;
    chanHandle->tempPacket->status = chanHandle->currentPacketErrorStatus;

    Mcasp_localCompleteCurrentIo(chanHandle);

#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
    /* check if the driver is waiting to go to sleep or process DVFS event    *
     * (only if the application has requested PWRM support)                   */
    if (TRUE == instHandle->pscPwrmEnable)
    {
        Mcasp_localHandlePwrmEvent(instHandle,chanHandle);
    }
#endif /* #ifdef BIOS_PWRM_ENABLE  */
}


/**
 * \brief     This function restores the driver to the original state that is it
 *            resumes the normal operation of the driver by picking the IOPs
 *            from the pending queue and putting it to the active IOP.
 * 
 * \param     instHandle [IN] Handle to the device instance object
 * 
 * \enter     instHandle is a valid non NULL pointer
 * 
 * \leave     Not impplemented
 */
Int32 Mcasp_loadPendedIops(Mcasp_ChannelObj *chanHandle)
{
    IOM_Packet           *ioPacket        = NULL;
    Int32                 status          = IOM_COMPLETED;
    Uint32                count           = 0x00;
    Uint32                hwiKey          = 0x00;
    Uint32                tempSubmitCount = 0x00;

    assert(NULL != chanHandle);

    /* reset the submit count to indicate we are loading the first packet     */
    tempSubmitCount = (Uint32)chanHandle->submitCount;
    chanHandle->submitCount = 0x00;

    for (count = 0; count < (Mcasp_MAXLINKCNT + 1); count++)
    {
        /* start the critical section                                         */
        hwiKey = (Uint32)_disable_interrupts();

        if (TRUE == QUE_empty(&chanHandle->queueReqList))
        {
            break;
        }
        else
        {
            chanHandle->submitCount++;

            /* get the packet out of the pending queue and load it to the     *
             * active Queue                                                   */
            ioPacket = QUE_get(&(chanHandle->queueReqList));

            /* put the packet in to the active queue                          */
            QUE_put(&(chanHandle->queueFloatingList),(Ptr)ioPacket);
    
            /* load the packet to the edma                                    */
            Mcasp_localLoadPktToEdma(chanHandle,ioPacket);
        }
    }

    /* restore the submit count                                               */
    chanHandle->submitCount = (Int32)tempSubmitCount;
    _restore_interrupts(hwiKey);

    return status;
}


#endif /* Mcasp_LOOPJOB_ENABLED */

/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/

/**
 *  \brief    Mcasp_localLpscOn
 *
 *            This function is invoked to power on the Mcasp peripheral and
 *            configure it according to the operating mode. Note that this
 *            function uses different functions to power ON the module,
 *            depending on whether PWRM is enabled in this driver or NOT
 *
 *  \param    instHandle  [IN] Handle to the device instance
 *
 *  \return   IOM_COMPLETED in case of success, an ERROR code in case of error.
 *
 *  \enter    instHandle    must be a valid pointer
 *
 *  \leave    Not Implemented.
 */
Int32 Mcasp_localLpscOn(Mcasp_Object *instHandle)
{
    Int32  status = IOM_COMPLETED;

    assert(NULL != instHandle);
    if (TRUE == instHandle->pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power on using bios PWRM API                                       */
        status = (Int32)PWRM_setDependency(
                    (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);
#else
        /* power on using the PSC API                                         */
        status = Psc_ModuleClkCtrl(
                    (Psc_DevId)instHandle->hwInfo.pscInstance,
                    (Uint32)instHandle->hwInfo.pwrmLpscId,
                    TRUE);
#endif
    }
    return (status);
}

/**
 * \brief    Mcasp_localLpscOff
 *
 *           This function is invoked to power off the module.
 *
 * \param    instHandle  [IN] Handle to the device instance
 *
 * \return   IOM_COMPLETED in case of success, an ERROR code in case of error.
 *
 * \enter    instHandle must be a valid channel object
 *           No pending IO
 *
 * \leave    Not Implemented.
 */
Int32 Mcasp_localLpscOff(Mcasp_Object *instHandle)
{
    Int32  status = IOM_COMPLETED;

    assert(NULL != instHandle);

    if (TRUE == instHandle->pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power off  using PWRM API                                          */
        status = (Int32)PWRM_releaseDependency(
                            (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);
#else
        /* power off using PSC API only if it is an instance other than "0"   */
        status = Psc_ModuleClkCtrl(
                    (Psc_DevId)instHandle->hwInfo.pscInstance,
                    (Uint32)instHandle->hwInfo.pwrmLpscId,
                    FALSE);
#endif
    }
    return (status);
}


#if defined(BIOS_PWRM_ENABLE) && !defined(Mcasp_LOOPJOB_ENABLED)
/**
 * \brief     This function registers with the BIOS PWRM module for all the 
 *            possible power management events.
 *
 * \param     instHandle [IN] Handle to the driver instance object
 *
 * \return    None
 *
 * \enter     instHandle is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 McaspRegisterNotification(Mcasp_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Mcasp_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL, 
                    &McaspNotifyCallback,
                    (Arg)instHandle, 
                    &(instHandle->pwrmInfo.notifyHandle[eventCnt]), 
                    &(instHandle->pwrmInfo.delayedCompletionFxn[eventCnt]));

        if (PWRM_SOK != retVal)
        {
            status = IOM_EBADARGS;
            break;
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
 * \return    None
 *
 * \enter     eventType is a valid event
 *
 * \leave     Not implemented
 */
static PWRM_NotifyResponse McaspNotifyCallback(PWRM_Event    eventType,
                                               Arg           eventArg1,
                                               Arg           eventArg2,
                                               Arg           clientArg)
{
    Mcasp_Object          *instHandle = NULL;
    PWRM_NotifyResponse    pwrmStatus = PWRM_NOTIFYDONE;
    PWRM_Status            status     = PWRM_SOK;

    assert(NULL != clientArg);

    eventArg1 = eventArg1;
    eventArg2 = eventArg2;

    instHandle = (Mcasp_Object *)clientArg;
    assert(NULL != instHandle);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
            pwrmStatus = McaspSuspendCurrentIops(instHandle,eventType);
            break;
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
            /* No DVFS changes required hence return done                     */
            pwrmStatus = PWRM_NOTIFYDONE;
            break;
        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
            /* No DVFS changes required hence return done                     */
            pwrmStatus = PWRM_NOTIFYDONE;
            break;
        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
            /* restore the dependency count                                   */
            while (instHandle->pwrmInfo.dependencyCount > 0)
            {
                status = PWRM_setDependency(
                             (PWRM_Resource)instHandle->hwInfo.pwrmLpscId);

                if (PWRM_SOK != status)
                {
                    break;
                }
                instHandle->pwrmInfo.dependencyCount--;
            }
         
            if (PWRM_SOK == status)
            {
                Mcasp_loadPendedIops(&instHandle->XmtObj);
                Mcasp_loadPendedIops(&instHandle->RcvObj);
                instHandle->devState = Mcasp_DriverState_CREATED;                
            }
            else
            {
                pwrmStatus = PWRM_NOTIFYNOTDONE;
            }
            break;
        case PWRM_AWAKEFROMSTANDBY:
            Mcasp_loadPendedIops(&instHandle->XmtObj);
            Mcasp_loadPendedIops(&instHandle->RcvObj);

            instHandle->devState = Mcasp_DriverState_CREATED;
            break;
        default:
            break;
    }
    return (pwrmStatus);
}

/**
 * \brief    This function configures the driver so that the current IOP can be
 *           completed and then supends all the remaining IOPs.
 *
 * \param    instHandle [IN] Handle to the device instance object
 * \param    eventType  [IN] event which has caused this notification call to 
 *                           driver
 *
 * \return   PWRM_NOTIFYDONE    if no IOPs are pending
 *           PWRM_NOTIFYNOTDONE if IOPs are pending in the driver.
 */
static PWRM_NotifyResponse McaspSuspendCurrentIops(Mcasp_Object  *instHandle,
                                                   PWRM_Event     eventType)
{
    Uint32               hwiKey = 0x00;
    PWRM_NotifyResponse  status = PWRM_NOTIFYDONE; 

    assert(NULL != instHandle);

    hwiKey = (Uint32)_disable_interrupts();

    if ((TRUE == QUE_empty(&instHandle->XmtObj.queueFloatingList)) &&
        (TRUE == QUE_empty(&instHandle->RcvObj.queueFloatingList)))
    {
        /* No IOP is pending currently. we can return the status of the       *
         * notify call as sucess                                              */
        instHandle->devState = Mcasp_DriverState_PWRM_SUSPEND;
        status = PWRM_NOTIFYDONE;
    }
    else
    {
        /* There are some IOPs pending,hence we will complete processing      *
         * of this request in the ISR context(task in case of polled mode)    *
         * by calling the delayed completion function                         */
        status = PWRM_NOTIFYNOTDONE;
        instHandle->pwrmInfo.ioSuspend = TRUE;
        instHandle->pwrmInfo.pwrmEvent = eventType;
    }

    _restore_interrupts(hwiKey);
    return (status);
}


/**
 * \brief     This function unregisters all the power management events 
 *            which were previously registered by the driver instance.
 *
 * \param     instHandle [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADARGS  in case of error
 *
 * \enter     instHandle is a NON null valid pointer
 *
 * \leave     Not implemented
 */
static Int32 McaspUnregisterNotification(Mcasp_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Mcasp_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        if (NULL != (instHandle->pwrmInfo.notifyHandle[eventCnt]))
        {
            retVal = PWRM_unregisterNotify(
                        (instHandle->pwrmInfo.notifyHandle[eventCnt]));

            if (PWRM_SOK != retVal)
            {
                status = IOM_EBADARGS;
                break;
            }
            else
            {
                instHandle->pwrmInfo.notifyHandle[eventCnt] = 0x00;
            }
        }
    }
    return (status);
}

/**
 * \brief     This function unregisters all the constraints for the device 
 *            instance.
 * 
 * \param     instHandle [IN]  Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADMODE in case of failure
 * 
 * \enter     instHandle is a valid non null pointer
 *
 * \leave     Not implemented
 */
static Int32 McaspUnregisterConstraints(Mcasp_Object *instHandle)
{
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;
    Uint32       count  = 0x00;

    assert(NULL != instHandle);
    
    for (count = 0; count < Mcasp_MAX_PWRM_CONSTRAINTS; count++)
    {
        if (NULL != instHandle->pwrmInfo.constraintHandle[count])
        {
            retVal = PWRM_unregisterConstraint(
                         instHandle->pwrmInfo.constraintHandle[count]);

            if (PWRM_SOK != retVal)
            {
                status = IOM_EBADMODE;
                break;
            }
            else
            {
                instHandle->pwrmInfo.constraintHandle[count] = 0x00;
            }
        }
    }
    return (status);
}

#endif
/* ========================================================================== */
/*                             END OF FILE                                    */
/* ========================================================================== */

