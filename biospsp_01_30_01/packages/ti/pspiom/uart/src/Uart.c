/*
 * Uart.c
 *
 * This file contains Driver Layer Interface implementation for the UART Driver.
 * UART Driver provides Driver Layer Interface to do operations on the UART
 * peripheral like device initialization, channel creation, control commands for
 * peripheral specific operations etc
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
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
 *  \file   Uart.c
 *
 *  \brief  IOM implementation for Uart device.
 *
 *   This file contains the driver implementation for the Uart device.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <string.h>
#include <assert.h>
#include <que.h>
#include <iom.h>
#include <swi.h>
#include <tsk.h>
#include <hwi.h>
#include <c64.h>
#include <bcache.h>
#include <ecm.h>
#include <clk.h>

#ifdef Uart_EDMA_ENABLE
    #include <ti/sdo/edma3/drv/edma3_drv.h>
#endif

#include "ti/pspiom/cslr/cslr_uart.h"
#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/uart/Uart.h"
#include "UartLocal.h"

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif

/* -------- constants -------- */
const struct Uart_Params Uart_PARAMS = {
    TRUE,                   /* cacheEnable                    */
    TRUE,                   /* fifoEnable                     */
    Uart_OpMode_INTERRUPT,  /* opMode                         */
    FALSE,                  /* loopbackEnabled                */
    Uart_BaudRate_115_2K,   /* baudRate                       */
    Uart_NumStopBits_1,     /* stopBits                       */
    Uart_CharLen_8,         /* charLen                        */
    Uart_Parity_NONE,       /* parity                         */
    Uart_RxTrigLvl_14,      /* rxThreshold                    */
    {                       /* fc                             */
        Uart_FcType_NONE,
        Uart_FcParam_NONE
    },
    0,                      /* edmaRxTC                       */
    0,                      /* edmaTxTC                       */
    0,                      /* hwiNumber                      */
    0xffffffff,             /* polledModeTimeout              */
    1,                      /* softTxFifoThreshold            */
    FALSE,                  /* PSC control disabled           */
    Uart_pllDomain_0        /* PLL domain used by the driver  */
};

/**
 *  \brief  Default Channel parameters for Rx
 */
const Uart_ChanParams Uart_defaultRxChanParams = {NULL};
/**
 *  \brief  Default Channel parameters for Tx
 */
const Uart_ChanParams Uart_defaultTxChanParams = {NULL};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */
/**
 *  \brief  Array which is part of Uart Module State
 */
static Bool inUse[CSL_UART_PER_CNT];
/**
 *  \brief  Uart Module State Object
 */
static Uart_Module_State Uart_module = {&inUse[0]};
/**
 *  \brief  Array of Uart instance State objects array
 */
static Uart_Object Uart_Instances[CSL_UART_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int uartMdBindDev(Ptr *devp, Int devId, Ptr devParams);

static Int uartMdUnBindDev(Ptr devp);

static Int uartMdCreateChan(Ptr                *chanp,
                            Ptr                 devp,
                            String              name,
                            Int                 mode,
                            Ptr                 chanParams,
                            IOM_TiomCallback    cbFxn,
                            Ptr                 cbArg);

static Int uartMdDeleteChan(Ptr chanp);

static Int uartMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);

static Int uartMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg);

static Int32 uartBaudRateChangeIoctl(Uart_Object *instHandle,Ptr cmdArgs);

Void  uartIsr(Arg arg);

static Void  uartTxSwiHandler(Arg arg0, Arg unUsedArg);

static Void  uartRxSwiHandler(Arg arg0, Arg unUsedArg);

static Void uartRegisterInterrupt(Ptr                   inDevp,
                                  Uint32                hwiNumber,
                                  Uint16                evt,
                                  Uart_pspUartIsr       intIsr);

static Void uartConfigure(Uart_Object           *instHandle,
                          const Uart_DevParams  *openParams);

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
static Int32 uartValidateParams(const Uart_Params *openParams);
#endif

static Int32 uartHwSetup(Uart_Object          *instHandle,
                         const Uart_DevParams *currentParams);


static Int32 uartSubmitIoReq(Uart_ChanObj *chanHandle, IOM_Packet *ioPacket);

static Void uartCancelCurrentIo(Uart_ChanObj *chanHandle, IOM_Packet *ioPacket);

static Void uartEnableHwFlowControl(Uart_Object             *instHandle,
                                    const Uart_FlowControl  *pFlowCtrl);

static Uint32 uartXfer(Uart_ChanObj *chanHandle,
                       Uint8        *buffer,
                       Uint32        numBytes);

static Int32 uartSubmitIoReqPolled(Uart_ChanObj *chanHandle,
                                   IOM_Packet   *ioPacket);

static Int32 uartSubmitIoReqIntDma(Uart_ChanObj *chanHandle,
                                   IOM_Packet   *ioPacket,
                                   Uint32       key);

static Void uartCancelAllIo(Uart_ChanObj *chanHandle);

static inline Void uartUnRegisterInterrupt(Uint32 hwiNumber, Uint16 evt);

static inline Void uartIntrEnable(Uart_Object *instHandle, Uint32 bitmask);

static inline Void uartIntrDisable(Uart_Object *instHandle, Uint32 bitmask);

static Void uartIntrHandler(Uart_ChanObj *chanHandle);

static Uint8 uartReadByte(Uart_Object *instHandle);

static inline Void uartWriteByte(Uart_Object* instHandle, Char data);

#ifndef BIOS_PWRM_ENABLE
static Void uartSetBaudRate(Uart_Object *instHandle, Uint32 baudRate);
#endif

static inline Uint32 uartGetLineStatus(Uart_Object *instHandle);

static inline Uint32 uartGetIntrStatus(Uart_Object *instHandle);

static Bool uartOsCheckTimeOut(Uint32 startValue, Uint32 timeout);

static Int32 uartHandleRxError(Uart_ChanObj *chanHandle);

#ifdef BIOS_PWRM_ENABLE
static Int32 UartRegisterNotification(Uart_Object *instHandle);

static Int32 UartUnregisterNotification(Uart_Object *instHandle);

static PWRM_NotifyResponse UartNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg);

static PWRM_NotifyResponse UartSuspendCurrentIops(Uart_Object  *instHandle,
                                                  PWRM_Event    eventType);

static Int32 UartCalculateConstraints(Uart_Object *instHandle);

static Int32 UartUnregisterConstraints(Uart_Object *instHandle);

static Int32 UartCalculateClockSettings(Uart_Object   *instHandle,
                                        Uint32         setpoint,
                                        Uart_BaudRate  opFreq,
                                        Uint32        *prescale);

static Int32 UartConfigureNewClkSettings(Uart_Object *instHandle,
                                         Uint32       prescale);

static PWRM_NotifyResponse UartConfigureNewVfSettings(Uart_Object   *instHandle,
                                                      Uns            eventArg1,
                                                      Uns            eventArg2);
#endif
/* ========================================================================== */
/*                            GLOBAL VARIBALES                                */
/* ========================================================================== */

IOM_Fxns Uart_IOMFXNS =
{
    &uartMdBindDev,
    &uartMdUnBindDev,
    &uartMdControlChan,
    &uartMdCreateChan,
    &uartMdDeleteChan,
    &uartMdSubmitChan
};

/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */

/**
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \return None
 */
Void Uart_init(Void)
{
    Int i;

    for (i = 0; i < CSL_UART_PER_CNT; i++)
    {
        /* have to initialize statically */
        Uart_module.inUse[i] = FALSE;
        memset((Void *)&Uart_Instances[i], 0x0, sizeof(Uart_Object));
    }
}

/**
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp       [OUT]  pointer to Uart driver object
 *  \param  devId      [IN]   device Id
 *  \param  devParams  [IN]   device Parameters
 *
 *  \return IOM_COMPLETED   if success
 *          Error ID        in case of error
 *
 *  \enter  *devp           must be a valid pointer and should not be null.
 *          devParams       must be a valid pointer and should not be null.
 *          devId           must be a valid device number (<CSL_UART_PER_CNT)
 *
 *  \leave  Not Implemented.
 */
static Int uartMdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Int32         retVal     = IOM_COMPLETED;
    SWI_Attrs     swiAttrs   = {0};
    Uart_Params  *params     = NULL;
    Uart_Object  *instHandle = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devParams) || (CSL_UART_PER_CNT  <= devId))
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        params = (Uart_Params *)devParams;
        instHandle =  &Uart_Instances[devId];
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((TRUE == Uart_module.inUse[devId]) ||
            (Uart_DriverState_DELETED != instHandle->devState)  ||
            (0 == params->hwiNumber) ||
            (IOM_COMPLETED != uartValidateParams(params)))
        {
            /* Driver not in deleted (initial) state or in use                */
            retVal = IOM_EBADMODE;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* set the status of the module as in use                             */
        Uart_module.inUse[devId] = TRUE;

        /* initialise the soc specific information                            */
        if (0 == devId)
        {
            instHandle->deviceInfo.baseAddress = (Ptr)CSL_UART_0_REGS;
            instHandle->deviceInfo.cpuEventNumber
                = (Uint32)CSL_INTC_EVENTID_UARTINT0;
            instHandle->deviceInfo.rxDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART0_RX;
            instHandle->deviceInfo.txDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART0_TX;
            instHandle->deviceInfo.inputFrequency = CSL_UART_0_MODULE_FREQ;
#ifndef BIOS_PWRM_ENABLE
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_PSC_UART0;
#else
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_UART_0;
#endif
            instHandle->deviceInfo.pscInstance = (Uint32)CSL_LPSC_INST_UART_0;
        }
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        else if (1 == devId)
        {
            instHandle->deviceInfo.baseAddress = (Ptr)CSL_UART_1_REGS;
            instHandle->deviceInfo.cpuEventNumber
                = (Uint32)CSL_INTC_EVENTID_UARTINT1;
            instHandle->deviceInfo.rxDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART1_RX;
            instHandle->deviceInfo.txDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART1_TX;
            instHandle->deviceInfo.inputFrequency = CSL_UART_1_MODULE_FREQ;
#ifndef BIOS_PWRM_ENABLE
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_PSC_UART1;
#else
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_UART_1;
#endif
            instHandle->deviceInfo.pscInstance = (Uint32)CSL_LPSC_INST_UART_1;
        }
#endif /* defined(CHIP_C6747) || ... */
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        else
        {
            instHandle->deviceInfo.baseAddress = (Ptr)CSL_UART_2_REGS;
            instHandle->deviceInfo.cpuEventNumber
                = (Uint32)CSL_INTC_EVENTID_UARTINT2;
            instHandle->deviceInfo.rxDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART2_RX;
            instHandle->deviceInfo.txDmaEventNumber
                = (Uint32)CSL_EDMA3_CHA_UART2_TX;
            instHandle->deviceInfo.inputFrequency = CSL_UART_2_MODULE_FREQ;
#ifndef BIOS_PWRM_ENABLE
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_PSC_UART2;
#else
            instHandle->deviceInfo.pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_UART_2;
#endif
            instHandle->deviceInfo.pscInstance = (Uint32)CSL_LPSC_INST_UART_2;
        }
#endif  /* defined(CHIP_C6747) || defined(CHIP_OMAPL137) */
        /* Initialize Tx Channel members                                      */
        instHandle->xmtChanObj.status = Uart_DriverState_CLOSED;
        instHandle->xmtChanObj.mode = Uart_OUTPUT;
        instHandle->xmtChanObj.cbFxn = NULL;
        instHandle->xmtChanObj.cbArg = NULL;
        QUE_new(&(instHandle->xmtChanObj.queuePendingList));
        instHandle->xmtChanObj.activeIOP = NULL;
        instHandle->xmtChanObj.bytesRemaining = 0;
        instHandle->xmtChanObj.chunkSize = 0;
        instHandle->xmtChanObj.devHandle = NULL;
        instHandle->xmtChanObj.errors = 0;
        instHandle->xmtChanObj.hEdma = Uart_defaultTxChanParams.hEdma;
        instHandle->xmtChanObj.edmaTcc =
        instHandle->deviceInfo.txDmaEventNumber;
        instHandle->xmtChanObj.edmaChId =
        instHandle->deviceInfo.txDmaEventNumber;
        instHandle->xmtChanObj.edmaTC   = params->edmaTxTC;
        instHandle->xmtChanObj.optFlag   = FALSE;

        instHandle->xmtChanObj.gblErrCbkFxn = NULL;
        instHandle->xmtChanObj.gblErrCbkArg = NULL;

        /* Initailize Rx Channel members                                      */
        instHandle->rcvChanObj.status = Uart_DriverState_CLOSED;
        instHandle->rcvChanObj.mode = Uart_INPUT;
        instHandle->rcvChanObj.cbFxn = NULL;
        instHandle->rcvChanObj.cbArg = NULL;
        QUE_new(&(instHandle->rcvChanObj.queuePendingList));
        instHandle->rcvChanObj.activeIOP = NULL;
        instHandle->rcvChanObj.bytesRemaining = 0;
        instHandle->rcvChanObj.chunkSize = 0;
        instHandle->rcvChanObj.devHandle = NULL;
        instHandle->rcvChanObj.errors = 0;
        instHandle->rcvChanObj.hEdma = Uart_defaultTxChanParams.hEdma;
        instHandle->rcvChanObj.edmaTcc =
        instHandle->deviceInfo.rxDmaEventNumber;
        instHandle->rcvChanObj.edmaChId =
        instHandle->deviceInfo.rxDmaEventNumber;
        instHandle->rcvChanObj.edmaTC   = params->edmaRxTC;
        instHandle->rcvChanObj.optFlag  = FALSE;

        instHandle->rcvChanObj.gblErrCbkFxn = NULL;
        instHandle->rcvChanObj.gblErrCbkArg = NULL;

        /*Inintialize Statistics members                                      */
        instHandle->stats.rxBytes = 0;
        instHandle->stats.txBytes = 0;
        instHandle->stats.overrun = 0;
        instHandle->stats.rxTimeout = 0;
        instHandle->stats.rxFramingError = 0;
        instHandle->stats.rxBreakError = 0;
        instHandle->stats.rxParityError = 0;

        instHandle->devParams.fifoEnable = params->fifoEnable;
        instHandle->devParams.loopbackEnabled = params->loopbackEnabled;
        instHandle->devParams.baudRate = params->baudRate;
        instHandle->devParams.stopBits = params->stopBits;
        instHandle->devParams.charLen = params->charLen;
        instHandle->devParams.parity = params->parity;
        instHandle->devParams.rxThreshold = params->rxThreshold;
        instHandle->devParams.fc.fcType = params->fc.fcType;
        instHandle->devParams.fc.fcParam = params->fc.fcParam;
        instHandle->devParams.fifoEnable     = params->fifoEnable;
        instHandle->polledModeTimeout = params->polledModeTimeout;
        instHandle->devParams.softTxFifoThreshold = params->softTxFifoThreshold;
        instHandle->devParams.pscPwrmEnable = params->pscPwrmEnable;
        
        instHandle->opMode = params->opMode;
        instHandle->devState = Uart_DriverState_DELETED;
        instHandle->hwiNumber = params->hwiNumber;
        instHandle->enableCache = params->enableCache;
        instHandle->instNum = (Uint32)devId;
        instHandle->txTskletHandle = NULL;
        instHandle->rxTskletHandle = NULL;
        instHandle->uartConfigured = FALSE;
#ifdef BIOS_PWRM_ENABLE
        instHandle->pwrmInfo.pllDomain = params->pllDomain;
        memset(&instHandle->pwrmInfo,0x00,sizeof(Uart_pwrmInfo));
#endif
        if ((Uart_OpMode_INTERRUPT == instHandle->opMode) ||
            (Uart_OpMode_DMAINTERRUPT == instHandle->opMode))
        {
            /* Creating Tasklet(SWI)to schedule TX empty interrupt cause. Becz*
             * TX empty handler requires a  wait for the transmitter to become*
             * empty in order to write 16 bytes into FIFO. This delay in Uart *
             * HWI handler context shall cause interrupt latencies for other  *
             * HWIs which are having less priority than Uart HWI.Thus         *
             * scheduling this TX processing in Tasklet(SWI) context for      *
             * better system performance.                                     */
            swiAttrs = SWI_ATTRS;

            /* modify the default parameters with the required params         */
            swiAttrs.priority = Uart_TASKLET_PRIORITY;
            swiAttrs.arg0     = (Arg)&instHandle->xmtChanObj;
            swiAttrs.fxn      = &uartTxSwiHandler;

            /* create the swi that will be used for buffering TX data*/
            instHandle->txTskletHandle = (Ptr)SWI_create(&swiAttrs);

            /* modify the default parameters with the required params         */
            swiAttrs.arg0     = (Arg)&instHandle->rcvChanObj;
            swiAttrs.fxn      = &uartRxSwiHandler;

            /* create the swi that will be used for buffering RX data*/
            instHandle->rxTskletHandle = (Ptr)SWI_create(&swiAttrs);

            if ((NULL == instHandle->txTskletHandle)||
                (NULL == instHandle->rxTskletHandle))
            {
                retVal = IOM_EBADARGS;
            }
        }
        else if (Uart_OpMode_POLLED == instHandle->opMode)
        {
            /* polled mode of operation                                       */
            SEM_new(&(instHandle->syncSem),1);
        }
        else
        {
            /* do nothing                                                     */
        }

        if ((IOM_COMPLETED == retVal)
            && (FALSE == instHandle->devParams.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            retVal = (Int32)PWRM_setDependency(
                          (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
            /* power on using PSC API                                         */
            retVal = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->deviceInfo.pscInstance,
                         (Uint32)instHandle->deviceInfo.pwrmLpscId,
                         TRUE);
#endif
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == retVal) &&
            (TRUE == instHandle->devParams.pscPwrmEnable))
        {
            /* unregister all the PWRM event notifications                    */
            retVal = UartRegisterNotification(instHandle);
        }
#endif
        if (IOM_COMPLETED == retVal)
        {
            instHandle->devState = Uart_DriverState_CREATED;
            *devp = (Ptr)instHandle;
        }
    }
    return retVal;
}

/*
 *  \brief  Function called by Bios during closing of the instance
 *
 *  \param  devp       [IN]  pointer to Uart driver object
 *
 *  \return success code
 *
 *  \enter  devp       must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int uartMdUnBindDev(Ptr devp)
{
    Uart_Object    *instHandle  = NULL;
    Uint32          waitCnt     = Uart_TRANSMITEMPTY_TRIALCOUNT;
    Int32           retVal      = IOM_COMPLETED;
    Uint32          tempReg     = 0x00;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == devp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        instHandle = (Uart_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle)                                        ||
            (CSL_UART_PER_CNT <= instHandle->instNum)                   ||
            (Uart_DriverState_CREATED != instHandle->devState)          ||
            (Uart_DriverState_CLOSED != instHandle->xmtChanObj.status)  ||
            (Uart_DriverState_CLOSED != instHandle->rcvChanObj.status))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* Unregister interrupts                                              */
        uartUnRegisterInterrupt(
            (Uint32)instHandle->hwiNumber,
            (Uint16)instHandle->deviceInfo.cpuEventNumber);

        /* Deleting Tasklet handler                                           */
        if (NULL != instHandle->txTskletHandle)
        {
            SWI_delete(instHandle->txTskletHandle);
        }

        if (NULL != instHandle->rxTskletHandle)
        {
            SWI_delete(instHandle->rxTskletHandle);
        }

        tempReg = (instHandle->deviceInfo.baseAddress)->LSR;

        retVal = Uart_localLpscOn(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            /* Waits till the transmitter is empty                            */
            while ((0 == (tempReg & CSL_UART_LSR_TEMT_MASK))
                   && (waitCnt != 0))
            {
                tempReg = uartGetLineStatus(instHandle);
                waitCnt--;
            }

            retVal = Uart_localLpscOff(instHandle);
        }

        if ((IOM_COMPLETED == retVal)
            && (FALSE == instHandle->devParams.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power off  using PWRM API                                      */
            retVal = (Int32)PWRM_releaseDependency(
                        (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
            /* power on using PSC API                                         */
            retVal = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->deviceInfo.pscInstance,
                         (Uint32)instHandle->deviceInfo.pwrmLpscId,
                         FALSE);
#endif
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == retVal) &&
            (TRUE == instHandle->devParams.pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            retVal = UartUnregisterNotification(instHandle);
        }
#endif
        instHandle->devState = Uart_DriverState_DELETED;
        Uart_module.inUse[instHandle->instNum] = FALSE;
    }
    return (retVal);
}

/* ========================================================================== */
/*                           IOM FUNCTIONS                                    */
/* ========================================================================== */

/**
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the Uart device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a SIO_create call.
 *  \param     chanp        [OUT]    channel object pointer
 *  \param     devp         [IN]     pointer to uart instance
 *  \param     name         [IN]     Uart Instance name like Uart0
 *  \param     mode         [IN]     channel  mode -> input or output
 *  \param     chanParams   [IN]     channel parameters from user
 *  \param     cbFxn        [IN]     callback function pointer
 *  \param     cbArg        [IN]     callback function Arguments
 *
 *  \return    0                in case of success
 *             error code       in case of failure
 *
 *  \enter     chanp      must be a valid pointer and should not be null.
 *             devp       must be a valid pointer and should not be null.
 *             name       must be a valid name.
 *             mode       must be a valid mode, either input or output.
 *             chanParams must be a valid pointer and should not be null.
 *             cbFxn      must be a valid pointer and should not be null.
 *             cbArg      must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int uartMdCreateChan (Ptr             *chanp,
                             Ptr              devp,
                             String           name,
                             Int              mode,
                             Ptr              chanParams,
                             IOM_TiomCallback cbFxn,
                             Ptr              cbArg)
{
    Uart_Object            *instHandle  = NULL;
    Uart_ChanObj           *chanHandle  = NULL;
    Bool                    bFalseWhile = TRUE;
    Uint32                  key         = 0;
    Bool                    pscPwrOn    = FALSE;
    Int32                   retVal      = IOM_COMPLETED;
    Int32                   status      = IOM_COMPLETED;
#ifdef Uart_EDMA_ENABLE
    Uint32                  value       = 0;
    Uart_ChanParams        *chanparams  = NULL;
#endif

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == cbFxn)||
        (NULL == cbArg) ||
        ((IOM_INPUT != mode) && (IOM_OUTPUT != mode))||
        (NULL == devp) ||
        (NULL == chanp))
    {
         retVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        do
        {
            bFalseWhile = FALSE;
            instHandle  = (Uart_Object *)devp;

            /* To remove the compiler warnings                                */
            if (NULL != name)
            {
                name = name;
            }

#ifdef Uart_EDMA_ENABLE
            if (NULL != chanParams)
            {
                chanparams = (Uart_ChanParams *)chanParams;
            }
#else
            else
            {
                chanParams = chanParams;
            }
#endif/* Uart_EDMA_ENABLE */
            /* check the current mode of operation and assign suitable handle */
            if (IOM_INPUT == mode)
            {
                chanHandle = (Uart_ChanObj*)&instHandle->rcvChanObj;
            }
            else
            {
                chanHandle = (Uart_ChanObj*)&instHandle->xmtChanObj;
            }

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            /* check if the channel was already opened.if already open then   *
             * flag error and bail out                                        */
            if ((NULL == chanHandle) ||
                (Uart_DriverState_OPENED == chanHandle->status))
            {
                retVal = IOM_EBADARGS;
                break;
            }

#ifdef Uart_EDMA_ENABLE
            if ( ((Uart_OpMode_DMAINTERRUPT == instHandle->opMode)   &&
                     ((NULL == chanparams) || (NULL == chanparams->hEdma))))
            {
                retVal = IOM_EBADARGS;
                break;
            }
#endif  /* Uart_EDMA_ENABLE */

#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

            /* enter critical section                                         */
            key = (Uint32)_disable_interrupts();

            /* Mark channel status as allocated.                              */
            chanHandle->status = Uart_DriverState_OPENED;

            /* exit critical section                                          */
            _restore_interrupts(key);

            chanHandle->mode      = (Uart_IoMode)(mode);
            chanHandle->devHandle = instHandle;

            /*Assign the respective callback function                         */
            chanHandle->cbFxn = cbFxn;
            chanHandle->cbArg = (Ptr)cbArg;

            retVal = Uart_localLpscOn(instHandle);

            if (IOM_COMPLETED == retVal)
            {
                pscPwrOn = TRUE;
            }
            else
            {
                break;
            }

            /* enter critical section                                         */
            key = (Uint32)_disable_interrupts();

            if (FALSE == instHandle->uartConfigured)
            {
                instHandle->uartConfigured = TRUE;

                /* exit critical section                                      */
                _restore_interrupts(key);

#ifdef BIOS_PWRM_ENABLE
                if ((IOM_COMPLETED == status) &&
                    (TRUE == instHandle->devParams.pscPwrmEnable))
                {
                    status = UartCalculateConstraints(instHandle);
                }
#endif
                /* Configure Uart hw for communication parameters only once   *
                 * and for the second channels create call - we need to skip  *
                 * this function call                                         */
                uartConfigure(instHandle, &instHandle->devParams);

                if ((Uart_OpMode_INTERRUPT == instHandle->opMode) ||
                    (Uart_OpMode_DMAINTERRUPT == instHandle->opMode))
                {
                    /* Register interrupts                                    */
                    uartRegisterInterrupt(
                        (Ptr)instHandle,
                        (Uint32)instHandle->hwiNumber,
                        (Uint16)instHandle->deviceInfo.cpuEventNumber,
                        &uartIsr);
                }
            }
            else
            {
                /* exit critical section                                      */
                _restore_interrupts(key);
            }

#ifdef Uart_EDMA_ENABLE
            if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode )
            {
                chanHandle->hEdma = chanparams->hEdma;

                retVal = EDMA3_DRV_requestChannel(
                             (EDMA3_DRV_Handle)chanHandle->hEdma,
                             &chanHandle->edmaChId,
                             &chanHandle->edmaTcc,
                             (EDMA3_RM_EventQueue)chanHandle->edmaTC,
                             &Uart_localIsrEdma,
                             (Ptr)chanHandle);

                if (IOM_COMPLETED != retVal)
                {
                     break;
                }

                switch (instHandle->devParams.rxThreshold)
                {
                    case Uart_RxTrigLvl_1:
                         value |= CSL_UART_FCR_RXFIFTL_CHAR1;
                         /* Receive FIFO trigger level 1 byte                 */
                         break;

                    case Uart_RxTrigLvl_4:
                         value |= CSL_UART_FCR_RXFIFTL_CHAR4
                                     << CSL_UART_FCR_RXFIFTL_SHIFT;
                         /* Receive FIFO trigger level 4 byte                 */
                         break;

                    case Uart_RxTrigLvl_8:
                         value |= CSL_UART_FCR_RXFIFTL_CHAR8
                                     << CSL_UART_FCR_RXFIFTL_SHIFT;
                         /* Receive FIFO trigger level 8 byte                 */
                         break;

                    case Uart_RxTrigLvl_14:
                    default:
                         value |= CSL_UART_FCR_RXFIFTL_CHAR14
                                     << CSL_UART_FCR_RXFIFTL_SHIFT;
                         /* Receive FIFO trigger level 14 byte                */
                         break;
                }

                /* Enable DMA Request                                         */
                value |= CSL_UART_FCR_FIFOEN_MASK;
                value |= CSL_UART_FCR_TXCLR_MASK;
                value |= CSL_UART_FCR_RXCLR_MASK;

                /* Write into FCR                                             */
                (instHandle->deviceInfo.baseAddress)->FCR = value;

                value |= CSL_UART_FCR_DMAMODE1_MASK;
                /* Write into FCR                                             */
                (instHandle->deviceInfo.baseAddress)->FCR = value;

            }/* End of if opmode dma loop                                     */
#endif  /* Uart_EDMA_ENABLE */


            chanHandle->activeBuf    = NULL;
            chanHandle->activeIOP    = NULL;
            chanHandle->bytesRemaining = 0;
            /* If fifo is enabled then transfer chunk could be FIFO size else *
             * only one byte can be trasferred per interrupt                  */
            if(TRUE == instHandle->devParams.fifoEnable)
            {
                chanHandle->chunkSize = (Uint32)Uart_FIFO_SIZE;
            }
            else
            {
                chanHandle->chunkSize = 1u;
            }

            if ((Uart_OpMode_POLLED != instHandle->opMode)
                 && (Uart_INPUT == chanHandle->mode))
            {
                 /* Enabling Line Status Interrupt if receive channel is
                  * created, in order to process Line status error interrupts.*/
                uartIntrEnable(chanHandle->devHandle, (Uint32)Uart_Intr_RLS);
            }
        }while(bFalseWhile);
    }

    if (TRUE == pscPwrOn)
    {
        status = Uart_localLpscOff(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            retVal = status;
        }
    }

    if (IOM_COMPLETED != retVal)
    {
        *chanp = NULL;
    }
    else
    {
        *chanp = (Ptr)chanHandle;
    }

    return retVal;
}

/**
 *  \brief    This function is called by the application to close a previously
 *            opened channel.it deletes the channel so that it is not available
 *            for further transactions. All the allocated resources are freed &
 *            the channel will be ready for the "open" operation once again.
 *
 *  \param    chanp      [IN]   Handle to the channel.
 *
 *  \return   status
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int uartMdDeleteChan(Ptr chanp)
{
    Uart_Object      *instHandle = NULL;
    Uart_ChanObj     *chanHandle = NULL;
    Uint32            key        = 0;
    Int32             retVal     = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Uart_ChanObj *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((Uart_DriverState_OPENED != chanHandle->status))
        {
            retVal = IOM_EBADARGS;
        }
        else
        {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
            instHandle = (Uart_Object *)chanHandle->devHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if (NULL == instHandle)
            {
                retVal = IOM_EBADARGS;
            }
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        if (TRUE == QUE_empty(&(chanHandle->queuePendingList)))
        {
#ifdef Uart_EDMA_ENABLE
            /*  Close EDMA Channel                                            */
            if ((Uart_OpMode_DMAINTERRUPT == instHandle->opMode ))
            {
                EDMA3_DRV_freeChannel(
                    chanHandle->hEdma,
                    chanHandle->edmaChId);
            }
#endif  /* Uart_EDMA_ENABLE */
            key = (Uint32)_disable_interrupts();

            /* Free this channel resource                                     */
            chanHandle->status = Uart_DriverState_CLOSED;

            if (IOM_COMPLETED == Uart_localLpscOn(instHandle))
            {
                /* Disable Uart interrupt causes                              */
                if (Uart_INPUT == chanHandle->mode )
                {
                    uartIntrDisable(instHandle,
                        (Uint32)(Uart_Intr_RLS | Uart_Intr_RHR));
                }
                else
                {
                    uartIntrDisable(instHandle, (Uint32)Uart_Intr_THR);
                }
                retVal = Uart_localLpscOff(instHandle);
            }

            /* Reset the error callback pointer to NULL                       */
            chanHandle->gblErrCbkFxn = NULL;
            chanHandle->gblErrCbkArg = NULL;

            _restore_interrupts(key);
        }
        else /* Some IOPs are pending                                         */
        {
            retVal = IOM_EBADMODE;
        }
        /* Updates the channel port Void                                      */
        chanHandle->devHandle = NULL;
    }

#ifdef BIOS_PWRM_ENABLE
    if (IOM_COMPLETED == retVal)
    {
        /* if both the channels are deleted then unregister all the contraints*/
        if ((Uart_DriverState_CLOSED == instHandle->xmtChanObj.status) &&
            (Uart_DriverState_CLOSED == instHandle->rcvChanObj.status))
        {
            retVal = UartUnregisterConstraints(instHandle);
        }
    }
#endif
    return (retVal);
}


/**
 *  \brief Submit a I/O packet to a channel for processing
 *
 *   The GIO layer calls this function to cause the mini-driver
 *   to process the IOM_Packet for read/write operations.
 *
 *  \param   chanp      [IN]  Handle to the channel
 *  \param   ioPacket   [IN]  Pointer to packet to be submitted
 *
 *  \return  IOM_COMPLETED, if packet is fully processed
 *           IOM_PENDING,   if packet is not fully processed
 *           negative error in case of error
 *
 *  \enter   chanp      must be a valid pointer and should not be null.
 *           ioPacket   must be a valid pointer and should not be null.

 *
 *  \leave  Not Implemented.
 */
static Int uartMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Uart_ChanObj       *chanHandle = NULL;
    Uart_Object        *instHandle = NULL;
    Bool                pscPwrOn   = FALSE;
    Int32               retVal     = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* The read and write command should give a proper buffer                 *
     * hence check if the buffer pointer is not null                          */
    if ((NULL == chanp) || (NULL == ioPacket) ||
        (((IOM_ABORT != ioPacket->cmd) && (IOM_FLUSH != ioPacket->cmd))
         && ((NULL == ioPacket->addr) || (0 == ioPacket->size))))
    {
         retVal = IOM_EBADARGS;
    }
    else
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        chanHandle = (Uart_ChanObj *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((Uart_DriverState_OPENED != chanHandle->status) ||
            (NULL == chanHandle->devHandle))
        {
            retVal = IOM_EBADARGS;
        }
        else
        {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
            instHandle = (Uart_Object *)chanHandle->devHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if (NULL == instHandle)
            {
                retVal = IOM_EBADARGS;
            }
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* check the command and process it stream will take care that        *
         * proper command is passed hence no need to check it once again      */
        if ((IOM_READ == ioPacket->cmd) || (IOM_WRITE == ioPacket->cmd))
        {
            /* request will be posted for processing. Now depending on        *
             * The mode of operation we need to either Wait for               *
             * processing to complete or the callback function to be          *
             * called                                                         */
            retVal = Uart_localLpscOn(instHandle);

            if (IOM_COMPLETED == retVal)
            {
                pscPwrOn = TRUE;
                retVal = uartSubmitIoReq((Ptr)chanHandle,ioPacket);
            }
        }
        else if ((Uart_OpMode_POLLED != instHandle->opMode) &&
            ((IOM_ABORT == ioPacket->cmd) || (IOM_FLUSH == ioPacket->cmd)))
        {
            /* we will try and abort all the packets except if the driver is  *
             * in the POLLED mode as in polled mode we will be executing in   *
             * task context and till it return control to application, an     *
             * abort call cannot come to us                                   */
            uartCancelAllIo(chanHandle);
        }
        else
        {
            /* unknown command has been passed to the driver hence set the    *
             * status of the command as error                                 */
            retVal = IOM_EBADARGS;
        }
    }

    /* update the status of the IO packet here                                */
    if (IOM_COMPLETED == retVal)
    {
        if ((Uart_OpMode_POLLED == instHandle->opMode) && (TRUE == pscPwrOn))
        {
            retVal = Uart_localLpscOff(instHandle);
        }
    }
    else if (IOM_PENDING == retVal)
    {
        retVal = IOM_PENDING;
    }
    else
    {
        if (TRUE == pscPwrOn)
        {
            /* The submit did not succeed hence power off the module.         */
            Uart_localLpscOff(instHandle);
        }
        retVal = IOM_EBADIO;
    }
    return retVal;
}


/**
 *  \brief   This function executes a control command passed by the application
 *
 *   The application's request for a IOCTL to be executed is routed here by the
 *   stream. if the command is supported then the control command is executed.
 *
 *  \param    chanp      [IN]    Channel handle
 *  \param    cmd        [IN]    control command given by the application
 *  \param    cmdArgs    [IN]    Optional argss required for command execution
 *
 *  \return   None
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *            cmd        must be a valid IOCTL command.
 *            cmdArg     must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int uartMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg)
{
    Uart_Object*        instHandle    = NULL;
    Uart_ChanObj       *chanHandle    = NULL;
    Uint32              tempReg       = 0;
    Uint32              lineStatus    = 0;
    Int32               retVal        = IOM_COMPLETED;
    Uart_NumStopBits    stopBits      = Uart_NumStopBits_1;
    Uart_CharLen        charLen       = Uart_CharLen_8;
    Uint32              hwiKey        = 0;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
         retVal = IOM_EBADARGS;
    }
    else if((cmd != Uart_IOCTL_RESET_TX_FIFO)     &&  /* These command do not*/
            (cmd != Uart_IOCTL_RESET_RX_FIFO)     &&   /* cmdARg. Hence this  */
            (cmd != Uart_IOCTL_CANCEL_CURRENT_IO) &&   /* check               */
            (cmd != Uart_IOCTL_CANCEL_CURRENT_IO) &&
            (cmd != Uart_IOCTL_CLEAR_STATS)       &&
            (cmd != Uart_IOCTL_FLUSH_ALL_REQUEST))
    {
        if(NULL == cmdArg)
        {
            retVal = IOM_EBADARGS;
        }
    }

    if(IOM_COMPLETED == retVal)
    {
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        /* get the handle to the channel to send the ICOTL to                 */
        chanHandle = (Uart_ChanObj*)chanp;
        instHandle = (Uart_Object *)chanHandle->devHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle)    ||
            (Uart_DriverState_OPENED != chanHandle->status))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == retVal)
    {
        /* Get the Line status of Uart. Uart ioctl commands to change baud    *
         * rate, stop bit, data bit, parity, flow control and other clock     *
         * related paramters should not be allowed when IO is on progress     *
         * i.e. TX FIFO is not empty                                          */
        lineStatus = uartGetLineStatus(instHandle);

        if (Uart_IOCTL_SET_BAUD == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((CSL_UART_LSR_TEMT_MASK | CSL_UART_LSR_THRE_MASK) ==
               (lineStatus & (CSL_UART_LSR_TEMT_MASK | CSL_UART_LSR_THRE_MASK)))
            {
                retVal = uartBaudRateChangeIoctl(instHandle,cmdArg);
            }
            else
            {
                /* IO operation is going. Baud rate change should not         *
                 * be entertained.                                            */
                retVal = IOM_EBADMODE;
            }
        }
        else if (Uart_IOCTL_SET_STOPBITS == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)) ==
                    (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK))
            {
                stopBits = *((Uart_NumStopBits *)cmdArg);

                /* Perform error checking on stop bits                        */
                switch (stopBits)
                {
                    case Uart_NumStopBits_1:
                    case Uart_NumStopBits_1_5:
                    case Uart_NumStopBits_2:
                        /* Setting the stop bits                              */
                        tempReg = (instHandle->deviceInfo.baseAddress)->LCR;
                        tempReg &= (Uint32)(~CSL_UART_LCR_STB_MASK);
                        tempReg |= (*(Uint32 *)&stopBits);
                        (instHandle->deviceInfo.baseAddress)->LCR = tempReg;
                        break;
                    default:
                        retVal = IOM_EBADARGS;
                        break;
                }
            }
            else
            {
                /* IO operation is going. Stop bit change should not be       *
                 * entertained.                                               */
                retVal = IOM_EBADMODE;
            }
        }

        else if (Uart_IOCTL_SET_DATABITS == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)) ==
                    (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK) )
            {
                charLen = *((Uart_CharLen *)cmdArg);

                /* Perform error checking on character bits                   */
                switch (charLen)
                {
                    case Uart_CharLen_5:
                    case Uart_CharLen_6:
                    case Uart_CharLen_7:
                    case Uart_CharLen_8:
                        /* Setting the character length                       */
                        tempReg = (instHandle->deviceInfo.baseAddress)->LCR;
                        tempReg &= (~CSL_UART_LCR_WLS_MASK);
                        tempReg |= (*(Uint32 *)&charLen);
                        (instHandle->deviceInfo.baseAddress)->LCR = tempReg;
                        break;
                    default:
                        retVal = IOM_EBADARGS;
                        break;
                }
            }
            else
            {
                /* IO operation is going. Stop bit change should not be       *
                 * entertained.                                               */
                retVal = IOM_EBADMODE;
            }
        }

        else if (Uart_IOCTL_SET_PARITY == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)) ==
                    (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK))
            {
                Uart_Parity  parity = Uart_Parity_NONE;

                parity = *((Uart_Parity *)cmdArg);

                /* Perform error checking on parity bits                      */
                switch (parity)
                {
                    case Uart_Parity_ODD:
                    case Uart_Parity_EVEN:
                    case Uart_Parity_NONE:
                        /* Setting the parity                                 */
                        tempReg = (instHandle->deviceInfo.baseAddress)->LCR;
                        tempReg &= (~(CSL_UART_LCR_EPS_MASK|CSL_UART_LCR_PEN_MASK));
                        tempReg |= (*(Uint32 *)&parity);
                        (instHandle->deviceInfo.baseAddress)->LCR = tempReg;
                        break;
                    default:
                        retVal = (IOM_EBADARGS);
                        break;
                }
            }
            else
            {
                /* IO operation is going. Parity change should not be         *
                 * entertained.                                               */
                retVal = IOM_EBADMODE;
            }
        }

        else if (Uart_IOCTL_SET_FLOWCONTROL == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)) ==
                    (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK))
            {
                Uart_FlowControl     *fc = NULL;
                fc = (Uart_FlowControl *)cmdArg;

                switch (fc->fcType)
                {
                    case Uart_FcType_HW:
                        /* Enable HW flow control                             */
                        uartEnableHwFlowControl(instHandle,fc);
                        break;

                    case Uart_FcType_NONE:
                        /* Disable AFE bit                                    */
                        (instHandle->deviceInfo.baseAddress)->MCR &=
                            ~(CSL_UART_MCR_AFE_MASK);

                        (instHandle->deviceInfo.baseAddress)->MCR |=
                            (CSL_UART_MCR_AFE_DISABLE <<
                                CSL_UART_MCR_AFE_SHIFT);
                        break;

                    case Uart_FcType_SW:
                    default:
                        retVal = IOM_ENOTIMPL;
                    break;
                }
            }
            else
            {
                /* IO operation is going. Parity change should not be         *
                 * entertained.                                               */
                retVal = IOM_EBADMODE;
            }
        }

        /* set the trigger level for the Rx FIFO                              */
        else if (Uart_IOCTL_SET_TRIGGER_LEVEL == cmd)
        {
            /* If TX FIFO is empty, then process this command                 */
            if ((CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK) ==
                    (lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)))
            {
                Uint32            value    = 0;
                Uart_RxTrigLvl    trigLevel;

                /* Validate params and FIFO Enable                            */
                if (TRUE == instHandle->devParams.fifoEnable)
                {
                    trigLevel = *((Uart_RxTrigLvl *)cmdArg);

                    switch (trigLevel)
                    {
                        case Uart_RxTrigLvl_1:
                             value |= CSL_UART_FCR_RXFIFTL_CHAR1;
                             /* Receive FIFO trigger level 1 byte             */
                             break;

                        case Uart_RxTrigLvl_4:
                             value |= CSL_UART_FCR_RXFIFTL_CHAR4 <<
                                                 CSL_UART_FCR_RXFIFTL_SHIFT;
                             /* Receive FIFO trigger level 4 byte             */
                             break;

                        case Uart_RxTrigLvl_8:
                             value |= CSL_UART_FCR_RXFIFTL_CHAR8 <<
                                                 CSL_UART_FCR_RXFIFTL_SHIFT;
                             /* Receive FIFO trigger level 8 byte             */
                             break;

                        case Uart_RxTrigLvl_14:
                             value |= CSL_UART_FCR_RXFIFTL_CHAR14 <<
                                                 CSL_UART_FCR_RXFIFTL_SHIFT;
                             /* Receive FIFO trigger level 14 byte            */
                             break;
                        default:
                             retVal = IOM_EBADARGS;
                             break;
                    }

                    if (0 != value)/* If valid trigger level passed           */
                    {
                        if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
                        {
                            /*  Set DMAMODE bit                               */
                            value |= CSL_UART_FCR_DMAMODE1_MASK;
                        }

                        /* Set FIFO enable bit                                */
                        value |= CSL_UART_FCR_FIFOEN_MASK;

                        /* Write into FCR                                     */
                        (instHandle->deviceInfo.baseAddress)->FCR = value;
                    }
                }
                else
                {
                    retVal = IOM_EBADMODE;
                }
            }
            else
            {
                /* IO operation is going. Parity change should not be         *
                 * entertained.                                               */
                retVal = IOM_EBADMODE;
            }
        }
        /* reset the TX FIFO                                                  */
        else if (Uart_IOCTL_RESET_TX_FIFO == cmd)
        {
            /* check if FIFO is supported and then reset the TX  FIFO         */
            if ((TRUE == instHandle->devParams.fifoEnable) &&
                 (Uart_OUTPUT == chanHandle->mode))
            {
                Uint32   value    = 0;

                /* Set TXFIFO Clear Bit                                       */
                value |= CSL_UART_FCR_TXCLR_MASK;

                if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode )
                {
                    /*  Set DMAMODE bit                                       */
                    value |= CSL_UART_FCR_DMAMODE1_MASK;
                }
                /* Set FIFO enable bit                                        */
                value |= CSL_UART_FCR_FIFOEN_MASK;

                /* Write into FCR                                             */
                (instHandle->deviceInfo.baseAddress)->FCR = value;
            }
            else
            {
                retVal = IOM_EBADMODE;
            }
        }

        /* reset the receive section FIFO                                     */
        else if (cmd == Uart_IOCTL_RESET_RX_FIFO)
        {
            /* check if FIFO is supported and then reset RX FIFO              */
            if ((TRUE == instHandle->devParams.fifoEnable) &&
                ((Uart_INPUT == chanHandle->mode)))
            {
                Uint32   value = 0;

                /* Set RXFIFO Clear Bit                                       */
                value |= CSL_UART_FCR_RXCLR_MASK;

                if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
                {
                   /*  Set DMAMODE bit                                        */
                   value |= CSL_UART_FCR_DMAMODE1_MASK;
                }
                /* Set FIFO enable bit                                        */
                value |= CSL_UART_FCR_FIFOEN_MASK;
                /* Write into FCR                                             */
                (instHandle->deviceInfo.baseAddress)->FCR = value;
            }
            else
            {
                retVal = IOM_EBADMODE;
            }
        }

        /* Cancel the current IO on the channel                               */
        else if (Uart_IOCTL_CANCEL_CURRENT_IO == cmd)
        {
            uartCancelCurrentIo(chanHandle, NULL);
        }

        /* copy the statistics for the channel                                */
        else if (Uart_IOCTL_GET_STATS == cmd)
        {
            *(Uart_Stats *)cmdArg = instHandle->stats;
        }

        /* clear the data statistics stored for the channel                   */
        else if (cmd == Uart_IOCTL_CLEAR_STATS)
        {
            memset(&instHandle->stats,0x00,sizeof(Uart_Stats));
        }

        else if (Uart_IOCTL_SET_POLLEDMODETIMEOUT == cmd)
        {
            /* Update the polledModeTimeout value                             */
            instHandle->polledModeTimeout = *((Uint32 *)cmdArg);
        }
        else if(Uart_IOCTL_SET_ERROR_CALLBACK ==cmd)
        {
            hwiKey = _disable_interrupts();

            /* To register a callback funtion for error notification          */
            chanHandle->gblErrCbkFxn = ((Uart_GblErrCbk*)cmdArg)->gblErrCbkFxn;
            chanHandle->gblErrCbkArg = ((Uart_GblErrCbk*)cmdArg)->gblErrCbkArg;

            _restore_interrupts(hwiKey);
        }

/*=======================Uart_IOCTL_FLUSH_ALL_REQUEST=========================*/
/*========================DriverTypes_CHAN_ABORT==============================*/
        else if (Uart_IOCTL_FLUSH_ALL_REQUEST == cmd)
        {
            uartCancelAllIo((Ptr)chanHandle);
        }

        else
        {
            /* unrecognised IOCTL is passed,hence status not implemented      */
            retVal = IOM_ENOTIMPL;
        }
    }

    return (retVal);
}

/* ========================================================================== */
/*                            LOCAL  FUNCTIONS                                */
/* ========================================================================== */

/**
 * \brief    This function implements the baud rate change IOCTL.
 *
 * \param    instHandle   [IN]    Pointer to the device Object
 * \param    cmdArgs      [IN]    Argument for the command.
 *
 * \return   IOM_COMPLETED                if success
 *           IOM_EBADARGS,IOM_EBADMODE    if failure
 *
 * \enter    instHandle must be valid pointer non NULL pointer
 *
 * \leave    Not Implemented.
 */
static Int32 uartBaudRateChangeIoctl(Uart_Object *instHandle,Ptr cmdArgs)
{
    Uart_BaudRate baudRate    = Uart_BaudRate_115_2K;
#ifdef BIOS_PWRM_ENABLE    
    PWRM_Domain   domain      = PWRM_CPU;
    Uint32        setpoint    = 0x00;
    Uint32        prescale    = 0x00;
#endif
    Int32         status      = IOM_COMPLETED;
    
    assert(NULL != cmdArgs);
    assert(NULL != instHandle);
    
    /* Perform error checking on baud rate                                    */
    baudRate = *((Uart_BaudRate *)cmdArgs);

    switch(baudRate)
    {
        case Uart_BaudRate_2_4K:
        case Uart_BaudRate_4_8K:
        case Uart_BaudRate_9_6K:
        case Uart_BaudRate_19_2K:
        case Uart_BaudRate_38_4K:
        case Uart_BaudRate_57_6K:
        case Uart_BaudRate_115_2K:
            /* Note: Tested only upto this baud rate                          */
        default:
#ifdef BIOS_PWRM_ENABLE
            /* update the baudrate in to the instHandle                       */
            instHandle->devParams.baudRate = baudRate;

            /* unregister the old constraints                                 */
            status = UartUnregisterConstraints(instHandle);
            
            /* calculate the contraints for this new baudrate                 */
            status |= UartCalculateConstraints(instHandle);

            if (IOM_COMPLETED == status)
            {
                /* check what clock domain is supported by the device         */
                if (Uart_pllDomain_0 != instHandle->pwrmInfo.pllDomain)
                {
                    domain = PWRM_PER;
                }

                /* get the information regarding the active set point         */
                if (PWRM_SOK == PWRM_getCurrentSetpoint(domain,&setpoint))
                {
                    status = UartCalculateClockSettings(
                                instHandle,
                                setpoint,
                                instHandle->devParams.baudRate,
                                &prescale);
                }
                else
                {
                    status = IOM_EBADMODE;
                }

                /* check if the calculation of new prescaler value for the new*
                 * baud rate is successful also check if the prescaler is     *
                 * within the permissible limits                              */
                if ((IOM_COMPLETED == status)  &&
                     ((1 <= prescale) && (prescale <= 0xFFFFu)))
                {
                    /* calculation of the prescaler value is successful.Hence *
                     * update the hardware with the new prescaler values      */
                    status = UartConfigureNewClkSettings(instHandle,prescale);
                }
            }
#else
            /* update the baudrate in to the instHandle                       */
            instHandle->devParams.baudRate = baudRate;

            uartSetBaudRate(instHandle, *(Uint32 *)cmdArgs);
#endif
            break;
    }

    return (status);
}


/**
 *  \brief  static Void uartRegisterInterrupt(Ptr inDevp, Uint32 hwiNumber, Uint16 evt, Uart_pspUartIsr intIsr)
 *
 *   This function is invoked in order for to register Uart interrupts
 *   with the requested interrupt handler with the OS (BIOS) and enables
 *   the interrupt.
 *
 *  \param    inDevp       [IN]    Pointer to the device
 *  \param    hwiNumber    [IN]    interrupt number
 *  \param    evt          [IN]    event number
 *  \param    intIsr       [IN]    ISR function pointer
 *
 *  \return
 *            IOM_COMPLETED   if success
 *            IOM_EBADARGS    if failure
 *
 *  \enter    inDevp    must be valid pointer to Uart Obj & should not be null
 *            hwiNumber must be a valid hardware interrupt number
 *            evt       must be a valid event number
 *            intIsr    must be a valid pointer to ISR function and should
 *                      not be  null.
 *
 *  \leave    Not Implemented.
 */
static Void uartRegisterInterrupt(Ptr                   inDevp,
                                  Uint32                hwiNumber,
                                  Uint16                evt,
                                  Uart_pspUartIsr       intIsr)
{
    static Uint32       key         = 0;
    Uart_Object        *instHandle  = NULL ;
    ECM_Attrs           ecmattrs    = ECM_ATTRS;

    assert((NULL != intIsr) && (NULL != inDevp));

    instHandle = (Uart_Object *)inDevp;

    assert(NULL != instHandle);

    key = (Uint32)_disable_interrupts();

    /*Enable the interrupt later                                              */
    ecmattrs.unmask = 0u;

    /*setup arg such that isr will get 'chan' as arg                          */
    ecmattrs.arg    = (Arg)instHandle;

    /* Plug the ISR with interrupt (event) number                             */
    ECM_dispatchPlug(evt, (ECM_Fxn)intIsr, &ecmattrs);

    /* Enabling the event                                                     */
    ECM_enableEvent(evt);

    /* Enabling the HWI_ID where corresponding ECM group is configured        */
    C64_enableIER(1 << hwiNumber);

    /* Enabling the interrupts                                                */
    _restore_interrupts(key);
}

/**
 *  \brief    uartUnRegisterInterrupt
 *
 *   This function is invoked to unregister and disable interrupt
 *   handler and interrupt cause respectively.
 *
 *  \param    hwiNumber     [IN]    interrupt number
 *  \param    evt           [IN]    event number
 *
 *  \return   IOM_COMPLETED
 *
 *  \enter  intNum    must be a valid hardware interrupt number
 *          evt       must be a valid event number
 *
 *  \leave  Not Implemented.
 */
static inline Void uartUnRegisterInterrupt(Uint32 hwiNumber, Uint16 evt)
{
    /* done to remove compiler warning                                        */
    hwiNumber= hwiNumber;

    /* Disable the event                                                      */
    ECM_disableEvent(evt);
}


/**
 *  \brief   This function enables the interrupts for the Uart device
 *
 *  \param   instHandle  [IN]     Handle to the Uart driver Object
 *  \param   bitmask     [IN]     Bitmask for the register to enable interrupts
 *
 * \return  None
 *
 * \enter   instHandle   must be a valid pointer and should not be null.
 *          bitmask      must be a valid bitmask value.
 *
 * \leave  Not Implemented.
 */
static inline Void uartIntrEnable(Uart_Object  *instHandle, Uint32 bitmask)
{
    Uint32 temp = 0;

    assert(NULL != instHandle);

    temp = (instHandle->deviceInfo.baseAddress)->IER;
    temp |= (bitmask & 0xFFu);

    (instHandle->deviceInfo.baseAddress)->IER = temp;
}


/**
 * \brief   This function dsiables the interrupts for the Uart device
 *
 * \param   instHandle  [IN]     Handle to the Uart driver object
 * \param   bitmask     [IN]     Bitmask for the register to disable interrupts
 *
 * \return  None
 *
 * \enter   instHandle   must be a valid pointer and should not be null.
 *          bitmask      must be a valid bitmask value.
 *
 * \leave  Not Implemented.
 */
static inline Void uartIntrDisable(Uart_Object *instHandle, Uint32 bitmask)
{
    Uint32 temp = 0;

    assert(NULL != instHandle);

    temp  = (instHandle->deviceInfo.baseAddress)->IER;
    temp &= (~bitmask);

    (instHandle->deviceInfo.baseAddress)->IER = temp & 0xFFu;
}


/**
 *  \brief   Perform read/write transfer
 *
 *           This function is called from ISR and Task context to do transfer
 *
 *  \param   chanHandle     [IN]     Handle to the channel
 *
 *  \return  If successful, returns IOM_COMPLETED else error code
 *
 *  \enter   chanHandle  must be a valid pointer and should not be null.
 *
 *  \leave   Not Implemented.
 *
 */
static Void uartIntrHandler(Uart_ChanObj *chanHandle)
{
    Uint32           xfer       = 0;
    IOM_Packet      *ioPacket   = NULL;
    Uart_Object     *instHandle = NULL;
#ifdef BIOS_PWRM_ENABLE
    PWRM_Status      status     = PWRM_SOK;
    Uint32           count      = 0x00;
#endif

    assert(NULL != chanHandle);
    instHandle = (Uart_Object*)(chanHandle->devHandle);
    assert(NULL != instHandle);

    /* If this is called from SWI(Tasklet) context                            */
    if (Uart_OpMode_INTERRUPT == instHandle->opMode)
    {
        ECM_disableEvent(instHandle->deviceInfo.cpuEventNumber);
    }

    ioPacket = chanHandle->activeIOP;

    if (NULL  != ioPacket)
    {
        xfer = uartXfer(chanHandle,
                        chanHandle->activeBuf,
                        chanHandle->bytesRemaining);

        chanHandle->bytesRemaining    -= xfer;
        chanHandle->activeBuf         += xfer;

        if (Uart_INPUT == chanHandle->mode)
        {
            instHandle->stats.rxBytes  += xfer;
            /* Re-enable the interrupt here, because it was disabled in ISR   *
             * before posting the SWI. This interrupt will be aprropriately   *
             * disabled below                                                 */
            uartIntrEnable(instHandle, (Uint32)Uart_Intr_RHR);
        }
        else
        {
            instHandle->stats.txBytes  += xfer;
            /* Re-enable the interrupt here, because it was disabled in ISR   *
             * before posting the SWI. This interrupt will be aprropriately   *
             * disabled below                                                 */
            uartIntrEnable(instHandle, (Uint32)Uart_Intr_THR);
        }

        if (0 == chanHandle->bytesRemaining)
        {
            if (Uart_INPUT == chanHandle->mode)
            {
                uartIntrDisable(instHandle, (Uint32)Uart_Intr_RHR);
            }
            else
            {
                uartIntrDisable(instHandle, (Uint32)Uart_Intr_THR);
            }

            /* Get next IOP from RX pending list - do this before             *
             * notifyCompletion is called as it will manipulate the           *
             * next and prev pointers of iop disturbing the pending list      */
            if (0 != chanHandle->errors)
            {
                ioPacket->status = IOM_EBADIO;
            }
            else
            {
                ioPacket->status = IOM_COMPLETED;
            }

            /* Perform the operation to complete the IO                       */
            Uart_localCompleteCurrentIO(chanHandle);

#ifdef BIOS_PWRM_ENABLE
            if (FALSE == instHandle->pwrmInfo.ioSuspend)
            {
#endif
                Uart_loadPendedIops(chanHandle);
#ifdef BIOS_PWRM_ENABLE
            }
            else
            {
                if (TRUE == instHandle->devParams.pscPwrmEnable)
                {
                    /* set the current active IOP as NULL                     */
                    chanHandle->activeIOP = NULL;
                    instHandle->devState = Uart_DriverState_PWRM_SUSPEND;

                    if ((NULL == instHandle->rcvChanObj.activeIOP) &&
                        (NULL == instHandle->xmtChanObj.activeIOP))
                    {
                        /* if both the channels are inactive then reset the   *
                         * io suspend flag                                    */
                        instHandle->pwrmInfo.ioSuspend = FALSE;

                        if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                            (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
                        {
                            /* reduce the dependency count                        */
                            status = PWRM_getDependencyCount(
                                        (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId,
                                         &count);
                        
                            instHandle->pwrmInfo.dependencyCount = count;
                        
                            if (PWRM_SOK == status)
                            {
                                while (count > 0)
                                {
                                    status =  PWRM_releaseDependency(
                                        (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
                        
                                    if (PWRM_SOK != status)
                                    {
                                        break;
                                    }
                                    count--;
                                }
                            }
                        }                        
                        /* call the delayed completion function               */
                        (instHandle->pwrmInfo.delayedCompletionFxn  \
                            [instHandle->pwrmInfo.pwrmEvent])();
                    }
               }
           }
#endif
        }/* for bytesRemaining equals to Zero                                 */
    }/* iop NULL condition                                                    */

    /* If this is called from SWI(Tasklet) context                            */
    if (Uart_OpMode_INTERRUPT == instHandle->opMode)
    {
        ECM_enableEvent(instHandle->deviceInfo.cpuEventNumber);
    }
}

#ifndef BIOS_PWRM_ENABLE
/**
 *  \brief   interface for configuring the Uart controller for a specific baud
 *           rate of operation
 *
 *  \param   instHandle  [IN]   Uart driver structure
 *  \param   baudRate    [IN]   The baud rate to be configured.
 *
 *  \return  None
 *
 *  \enter   instHandle  must be a valid pointer and should not be null.
 *           baudRate    must be a valid baudRate value.
 *
 *  \leave   Not Implemented.
 */
static Void uartSetBaudRate(Uart_Object *instHandle, Uint32 baudRate)
{
    Uint32 value = 0;

    assert(NULL != instHandle);

    value = (Uint32)((instHandle->deviceInfo.inputFrequency) /
                 (Uint32)((Uint32)Uart_BAUD_MULTIPLIER * (Uint32)baudRate));

    (instHandle->deviceInfo.baseAddress)->DLL = (Uint8)(value & 0xFF);
    (instHandle->deviceInfo.baseAddress)->DLH =  \
                                             (Uint8)((value & 0xFF00) >> 8);
}
#endif
/**
 *  \brief   Set the baud rate at which the Uart must operate in.
 *
 *  \param   instHandle [IN]    Handle to the Uart driver object
 *  \param   pFlowCtrl  [IN]    flow control params
 *
 *  \return  None
 *
 *  \enter   instHandle  must be a valid pointer and should not be null.
 *           pFlowCtrl   must be a valid pointer and should not be null.
 *
 *  \leave   Not Implemented.
 */
static Void uartEnableHwFlowControl(Uart_Object             *instHandle,
                                    const Uart_FlowControl  *pFlowCtrl)
{
    Uint32 mcr = 0;

    assert(((NULL != instHandle) && (NULL != pFlowCtrl)));
    mcr = (instHandle->deviceInfo.baseAddress)->MCR;

    /* Enable Auto Flow control                                               */
    mcr &= ~(CSL_UART_MCR_AFE_MASK);
    mcr |= (CSL_UART_MCR_AFE_ENABLE << CSL_UART_MCR_AFE_SHIFT);

    /* If only Auto CTS flow control                                          */
    if (Uart_FcParam_AUTO_CTS_ONLY == pFlowCtrl->fcParam )
    {
        mcr &= ~(CSL_UART_MCR_RTS_MASK);
        mcr |= (CSL_UART_MCR_RTS_DISABLE << CSL_UART_MCR_RTS_SHIFT);
    }
    else /* Auto RTS and auto CTS (Default)                                   */
    {
        mcr &= ~(CSL_UART_MCR_RTS_MASK);
        mcr |= (CSL_UART_MCR_RTS_ENABLE << CSL_UART_MCR_RTS_SHIFT);
    }
    /* Write into MCR register                                                */
    (instHandle->deviceInfo.baseAddress)->MCR = mcr;
}

/**
 *  \brief Configure Uart device HW
 *
 *   This function is used to set up the hardware for the current settings
 *  "param" points to the hardware config and other open params.
 *
 *  \param  instHandle    [IN]   Handle to the Uart driver object
 *  \param  openParams    [IN]   Parameters passed by the user
 *
 *  \return IOM_COMPLETED in case of sucess or
 *          E_badArgs in case of error
 *
 *  \enter  instHandle must be a valid pointer and should not be null.
 *          openParams must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void uartConfigure(Uart_Object           *instHandle,
                          const Uart_DevParams  *openParams)
{
    Uint32 tempReg = 0;
    volatile Uint32 timeout = 1000u;

    assert((NULL != instHandle) && (NULL != openParams));

    /* Reset Uart and setup hardware params                                   */
    (instHandle->deviceInfo.baseAddress)->PWREMU_MGMT = Uart_TXRX_DISABLE;

    /* wait for some time for uart to get disabled                            */
    while (0 != timeout)
    {
        timeout--;
    }

    /* clear the interrupt enable register and the status register            */
    (instHandle->deviceInfo.baseAddress)->IER = 0x00;

    /* read the status register to clear the pending events                   */
    uartGetLineStatus(instHandle);

    /* Putting Transmitter/Receiver in unreset state.                         *
     * Emulation FREE mode is set                                             */
    (instHandle->deviceInfo.baseAddress)->PWREMU_MGMT = Uart_TXRX_ENABLE;
    instHandle->devParams.fifoEnable  = openParams->fifoEnable;

    uartHwSetup(instHandle,&instHandle->devParams);

    /* Disable all interrupts                                                 */
    uartIntrDisable(instHandle,0xFFu);

    /* If the set parameters set the HW Flowcontrol                           */
    if (Uart_FcType_HW == openParams->fc.fcType )
    {
        /* Enable HW flow control                                             */
        uartEnableHwFlowControl(instHandle, &openParams->fc);
    }

    /* Enable Loopback                                                        */
    if ((Bool)TRUE == openParams->loopbackEnabled)
    {
        tempReg = (instHandle->deviceInfo.baseAddress)->MCR;
        tempReg &= (~CSL_UART_MCR_LOOP_MASK);
        tempReg |= CSL_UART_MCR_LOOP_MASK;
        (instHandle->deviceInfo.baseAddress)->MCR = tempReg;
     }
}

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
/**
 *  \brief    Validating Parameters passed to the Uart
 *
 *   This function is invoked in order to statically validate the
 *   various parameters passed to the Uart like baud Rate,Stop Bits,etc.
 *
 *  \param    instHandle   [IN]    Pointer to the Uart driver object
 *  \param    openParams   [IN]    Parameters passed to be vaildated
 *
 *  \return   IOM_COMPLETED if success
 *
 *  \enter    instHandle must be a valid pointer and should not be null.
 *            openParams must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 uartValidateParams(const Uart_Params *openParams)
{
    Int32  retVal = IOM_COMPLETED;

    assert(NULL != openParams);

    /* operational mode verification                                          */
    switch (openParams->opMode)
    {
        case Uart_OpMode_POLLED:
        case Uart_OpMode_INTERRUPT:
        case Uart_OpMode_DMAINTERRUPT:
             break;
        default:
             retVal = IOM_EBADARGS;
             break;
    }

    /* Get the stop bits from user configuration                              */
    switch (openParams->stopBits)
    {
        case Uart_NumStopBits_1:
        case Uart_NumStopBits_1_5:
        case Uart_NumStopBits_2:
        break;

        default:
            retVal = IOM_EBADARGS;
        break;
    }

    /* Get the char len from user configuration                               */
    switch (openParams->charLen)
    {
        case Uart_CharLen_5:
        case Uart_CharLen_6:
        case Uart_CharLen_7:
        case Uart_CharLen_8:
            break;

        default:
            retVal = IOM_EBADARGS;
            break;
    }

    /* Get the parity from user configuration                                 */
    switch (openParams->parity)
    {
         case Uart_Parity_ODD:
         case Uart_Parity_EVEN:
         case Uart_Parity_NONE:
             break;

         default:
            retVal = IOM_EBADARGS;
            break;
    }

    /* Update baud rate                                                       */
    switch (openParams->baudRate)
    {
        case Uart_BaudRate_2_4K:
        case Uart_BaudRate_4_8K:
        case Uart_BaudRate_9_6K:
        case Uart_BaudRate_19_2K:
        case Uart_BaudRate_38_4K:
        case Uart_BaudRate_57_6K:
        case Uart_BaudRate_115_2K:
        /* Note: Tested upto this baudrate */
        default:
            retVal = IOM_COMPLETED;
            break;
    }

    /* Update receive threshold                                               */
    switch (openParams->rxThreshold)
    {
        case Uart_RxTrigLvl_1 :
        case Uart_RxTrigLvl_4 :
        case Uart_RxTrigLvl_8 :
        case Uart_RxTrigLvl_14:
            break;
        default:
            retVal = IOM_EBADARGS;
            break;
    }

    return retVal;
}
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

/**
 *  \brief   Initialize the Uart device by setting the hardware registers
 *
 *  \param   instHandle      [IN]     Handle to the Uart driver object
 *  \param   currentParams   [IN]     Pointer for the Hardware configuration
 *
 *  \return  Void
 *
 *  \enter   instHandle    must be a valid pointer and should not be null.
 *           currentParams must be a valid pointer and should not be null.
 *
 *  \leave   Not Implemented.
 */
static Int32 uartHwSetup(Uart_Object            *instHandle,
                         const Uart_DevParams   *currentParams)
{
    Uint32           regVal     = 0x00;
#ifdef BIOS_PWRM_ENABLE
    Uns              setpoint   = 0x00;
    PWRM_Domain      domain     = PWRM_CPU;
#endif
    Int32            retVal     = IOM_COMPLETED;    
    
    assert((NULL != instHandle) && (NULL != currentParams));

#ifdef BIOS_PWRM_ENABLE
    if (TRUE == instHandle->devParams.pscPwrmEnable)
    {
        /* check what clock domain is supported by the device                 */
        if (Uart_pllDomain_0 != instHandle->pwrmInfo.pllDomain)
        {
            domain = PWRM_PER;
        }

        retVal = (Int32) PWRM_getCurrentSetpoint(domain,&setpoint);

        if (PWRM_SOK == retVal)
        {
            retVal = UartCalculateClockSettings(
                            instHandle,
                            setpoint,
                            currentParams->baudRate,
                            &regVal);
        }
    }
    else
    {
#endif
        /* get the divisor                                                    */
        regVal = (Uint32)(instHandle->deviceInfo.inputFrequency /
                 ((Uint32)Uart_BAUD_MULTIPLIER * (Uint32)currentParams->baudRate));

#ifdef BIOS_PWRM_ENABLE
    }
#endif

    (instHandle->deviceInfo.baseAddress)->DLL = (Uint8)(regVal & 0xFF);
    (instHandle->deviceInfo.baseAddress)->DLH = \
        (Uint8)((regVal & 0xFF00) >> 8u);

    /* Set the Fifo control - FIFO Disable not supported                      */
    /* Clear FIFO counters  */
    (instHandle->deviceInfo.baseAddress)->FCR = 0;

    regVal = 0;

    if (TRUE == currentParams->fifoEnable)
    {
        regVal = (CSL_UART_FCR_FIFOEN_ENABLE);

        switch (currentParams->rxThreshold)
        {
            case Uart_RxTrigLvl_1:
                 regVal |= CSL_UART_FCR_RXFIFTL_CHAR1;
                 /* Receive FIFO trigger level 1 byte   */
                 break;

            case Uart_RxTrigLvl_4:
                 regVal |= CSL_UART_FCR_RXFIFTL_CHAR4
                               << CSL_UART_FCR_RXFIFTL_SHIFT;
                 /* Receive FIFO trigger level 4 byte  */
                 break;

            case Uart_RxTrigLvl_8:
                 regVal |= CSL_UART_FCR_RXFIFTL_CHAR8
                               << CSL_UART_FCR_RXFIFTL_SHIFT;
                 /* Receive FIFO trigger level 8 byte  */
                 break;

            case Uart_RxTrigLvl_14:
            default:
                 regVal |= CSL_UART_FCR_RXFIFTL_CHAR14
                               << CSL_UART_FCR_RXFIFTL_SHIFT;
                 /* Receive FIFO trigger level 14 byte */
                 break;

        }
        regVal |= CSL_UART_FCR_FIFOEN_MASK;
        regVal |= CSL_UART_FCR_TXCLR_MASK;
        regVal |= CSL_UART_FCR_RXCLR_MASK;

    }

    (instHandle->deviceInfo.baseAddress)->FCR = regVal;

#ifdef Uart_EDMA_ENABLE
    if(Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        regVal |= CSL_UART_FCR_DMAMODE1_MASK;
        (instHandle->deviceInfo.baseAddress)->FCR = regVal;
    }
#endif  /* Uart_EDMA_ENABLE */

    /* Set other parameters based upon configuration                          */
    /* modem control register not programmed                                  */
    (instHandle->deviceInfo.baseAddress)->MCR = 0;

    regVal = (((Uint32) currentParams->charLen)
                | ((Uint32) currentParams->stopBits)
                | ((Uint32) currentParams->parity));

    (instHandle->deviceInfo.baseAddress)->LCR = regVal;

    (instHandle->deviceInfo.baseAddress)->PWREMU_MGMT |= 0x01u;

    return (retVal);
}


/**
 *  \brief   This function writes a byte to the Uart device
 *
 *  \param   instHandle  [IN]     Handle to the Uart driver Object
 *
 *  \return  Data read is returned
 *
 *  \enter   instHandle must be a valid pointer and should not be null.
 *
 *  \leave   Not Implemented.
 */
static Uint8 uartReadByte(Uart_Object *instHandle)
{
    Uint32           rbr       = 0;
    volatile Uint32  waitCount = 0xFFFFFFu;

    assert(NULL != instHandle);

    /* Read and throw Erroneous bytes from RxFIFO                             */
    while ((0 != (((instHandle->deviceInfo.baseAddress)->LSR)
        & (CSL_UART_LSR_BI_MASK | CSL_UART_LSR_FE_MASK | CSL_UART_LSR_PE_MASK)))
        && (0 != waitCount))
    {
        rbr = (instHandle->deviceInfo.baseAddress)->RBR;
        instHandle->stats.rxFramingError++;
        instHandle->rcvChanObj.errors++;
        waitCount--;
    }
    /* Read non-erroneous byte from RxFIFO                                    */
    rbr = (instHandle->deviceInfo.baseAddress)->RBR;

    return (Uint8)rbr;
}


/**
 *  \brief   This function writes a byte to the Uart device
 *
 *  \param   instHandle  [IN]     Handle to the Uart driver Object
 *  \param   data        [IN]     data to be written to the device
 *
 *  \return  None
 *
 *  \enter   instHandle must be a valid pointer and should not be null.
 *          data       must be a valid data value.
 *
 *  \leave   Not Implemented.
 */
static inline Void uartWriteByte(Uart_Object* instHandle, Char data)
{
    assert(NULL != instHandle);
    (instHandle->deviceInfo.baseAddress)->THR = (Uint32)data;
}


/**
 *  \brief    Getting the line status for errors or data ready
 *
 *  \param    instHandle [IN]  pointer to Uart driver object
 *
 *  \return   value of the lins status register
 *
 *  \enter    instHandle must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
static inline Uint32 uartGetLineStatus(Uart_Object *instHandle)
{
    Uint32           regVal     = 0;

    assert(NULL != instHandle);
    regVal = (instHandle->deviceInfo.baseAddress)->LSR;
    return regVal;
}

/**
 *  \brief    This function returns the interrupt staus register value
 *
 *  \param    instHandle [IN]   pointer to Uart driver object
 *
 *  \return   value of the IIR register
 *
 *  \enter    instHandle must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
static inline Uint32 uartGetIntrStatus(Uart_Object *instHandle)
{
    Uint32            regVal   = 0;

    assert(NULL != instHandle);
    regVal   = ((instHandle->deviceInfo.baseAddress)->IIR & 0xFu);

    return regVal;
}


/**
 *  \brief   checks if a timeout has happened from the given time.
 *
 *  This function checks if the timeout with respect to the start time has
 *  occured. The timeout and the start time is also input to the function.
 *
 *  \param   startValue  [IN]   Start value
 *  \param   timeout     [IN]   Timeout value
 *
 *  \return  TRUE   if time out occured.
 *          FALSE  if time out has not occured.
 *
 *  \enter   startValue  must be a valid start value
 *          timeout     must be a valid timeout value
 *
 *  \leave   Not Implemented.
 *
 */
static Bool uartOsCheckTimeOut(Uint32 startValue,Uint32 timeout)
{
    Uint32  checkValue = 0;
    Bool    retVal     = TRUE;

    /* get the current tick value and compare with the start value            */
    checkValue = CLK_gethtime();

    if (checkValue < startValue)
    {
        checkValue = (((0xFFFFFFFFU) - startValue) + checkValue) + (1U) ;
    }
    else
    {
        checkValue = checkValue - startValue;
    }

    /* if the difference between the current tick and start tick is greater   *
     * than start tick then set retval to TRUE to indicate time out           */
    if (checkValue < timeout)
    {
        retVal  =   FALSE;
    }
    else
    {
        retVal  =   TRUE;
    }
    return  retVal;
}

/**
 *  \brief   Read/Write operation are performed in polled mode
 *
 *  This function performs the read/write operation as requested by the
 *  application in the submitted io packet. it submits the packets in the
 *  POLLED mode.
 *
 *  \param  chanHandle  [IN]  Handle to the Channel
 *  \param  ioPacket    [IN]  Packet passed by the stream layer
 *
 *  \return If successful, returns IOM_COMPLETED else Error id
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          ioPacket     must be a valid pointer and should not be null.

 *
 *  \leave  Not Implemented.
 */
static Int32 uartSubmitIoReqPolled(Uart_ChanObj        *chanHandle,
                                   IOM_Packet          *ioPacket)
{
    Int32          retVal         = IOM_COMPLETED;
    Uint32         lineStatus     = 0;
    Uint32         currentTick    = 0;
    Bool           timeoutElapsed = FALSE;
    Uart_Object   *instHandle     = NULL;
    Bool           isTask         = FALSE;

    assert((NULL != chanHandle) && (NULL != ioPacket));

    /* get the handle the instance                                            */
    instHandle = (Uart_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* This submit does not allow a second request (from different            *
     * task ) when we are in polled mode                                      */
    if (NULL != chanHandle->activeIOP)
    {
        /* already an active iop is present hence cannot queue another        *
         * request in polled mode                                             */
        ioPacket->status = IOM_EBADIO;
        ioPacket->size  = 0;
        retVal          = IOM_EBADIO;
    }
    else
    {

        /* No active iop hence can queue our request                          */
        chanHandle->errors         = 0 ;
        chanHandle->activeIOP      = ioPacket;
        chanHandle->activeBuf      = ioPacket->addr;
        chanHandle->bytesRemaining = ioPacket->size;
        retVal = IOM_EBADIO;

        /* Update current tick value to perform timeout operation             */
        currentTick = CLK_gethtime();
        while ((FALSE == timeoutElapsed)
                   && (0 != chanHandle->bytesRemaining))
        {
            uartIntrHandler(chanHandle);
            /* Check whether timeout happened or not                          */
            timeoutElapsed = (Bool)uartOsCheckTimeOut(
                                        currentTick,
                                        instHandle->polledModeTimeout);

            isTask = TSK_isTSK();

            /* sleep only if in a task context and not in SWI or HWI          */
            if (TRUE == isTask)
            {
                TSK_sleep((Uart_TPOLL_MSECS));
            }
        }

        if ((0 == chanHandle->bytesRemaining)
            && (0 == chanHandle->errors))
        {
            do
            {
                lineStatus = uartGetLineStatus(instHandle);
            }while((CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK) !=
                    (lineStatus & (CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK)));

            retVal = IOM_COMPLETED;
        }
    }

    if (TRUE == timeoutElapsed)
    {
        ioPacket->status = retVal;
        ioPacket->size = \
            ioPacket->size - chanHandle->bytesRemaining;

        /* Perform the operation to complete the IO                           */
        Uart_localCompleteCurrentIO(chanHandle);
    }

    return retVal;
}

/**
 *  \brief   Read/Write operation are performed in polled mode
 *
 *  This function performs the read/write operation as requested by the
 *  application in the submitted io packet. It submits the packets in the
 *  interrupt or DMA mode without buffering enabled.
 *
 *  \param  chanHandle  [IN]  Handle to the Channel
 *  \param  ioPacket    [IN]  Packet passed by the stream layer
 *  \param  key      [IN]  value required for restoring interrupts
 *  \param  eb          [OUT] pointer to the error block
 *
 *  \return If successful, returns IOM_COMPLETED else Error id
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          ioPacket     must be a valid pointer and should not be null.
 *          key       must be a valid key value.
 *
 *  \leave  Not Implemented.
 */
static Int32 uartSubmitIoReqIntDma(Uart_ChanObj        *chanHandle,
                                   IOM_Packet          *ioPacket,
                                   Uint32               key)
{
    Int32          retVal         = IOM_COMPLETED;
    Uart_Object   *instHandle     = NULL;

    assert((NULL != chanHandle) && (NULL != ioPacket));

    /* get the handle the instance                                            */
    instHandle = (Uart_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* To remove the compiler warning                                         */
    key = key;

    /* If not in polled mode,then it may be in INT or DMA mode.               *
     * Disable the interrupts.Check if any IOP is active, if yes              *
     * check if the new IOP has a timeout value. If not then post             *
     * an error. else enque the iop into the list and return back.Also check  *
     * if the driver is in power saving state                                 */
    if ((NULL != chanHandle->activeIOP) ||
        (Uart_DriverState_PWRM_SUSPEND == instHandle->devState))
    {
        /* Queue it and update the return value                               */
        QUE_put(&(chanHandle->queuePendingList),(Ptr)ioPacket);
        ioPacket->status = IOM_PENDING;
        retVal = IOM_PENDING;
    }
    else
    {
        /* No other IOP is active, We can process this IOP, set               *
         * it as active IOP                                                   */
        chanHandle->activeIOP      = ioPacket;
        chanHandle->activeBuf      = ioPacket->addr;
        chanHandle->bytesRemaining = ioPacket->size;
        chanHandle->errors         = 0 ;

#ifdef Uart_EDMA_ENABLE
        if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
        {
            /* If DMA mode then start the DMA                                 */
            retVal = Uart_localStartEdmaTransfer(
                        chanHandle,
                        (Uint32)ioPacket->cmd);

            if  (IOM_COMPLETED == retVal)
            {
                retVal = IOM_PENDING;
            }
        }
#endif  /* Uart_EDMA_ENABLE */
        retVal  = IOM_PENDING;

        /* enable interrupt                                                   */
        if (Uart_OpMode_DMAINTERRUPT != instHandle->opMode)
        {
            if (Uart_INPUT == chanHandle->mode)
            {
                uartIntrEnable(chanHandle->devHandle,(Uint32)Uart_Intr_RHR);
            }
            else
            {
                uartIntrEnable(chanHandle->devHandle,(Uint32)Uart_Intr_THR);
            }
        }
        else
        {
            /* In case of DMA mode enable line status interrupt only to handle*
             * RX errors                                                      */
            uartIntrEnable(chanHandle->devHandle,CSL_UART_IER_ELSI_MASK);
        }
    }/*end of buffer mode check                                               */

    return retVal;
}

/**
 *  \brief   Read/Write operation are performed
 *
 *  This function performs the read/write operation as requested by the
 *  application in the submitted io packet. It can operate in POLLED and
 *  INTERRUPT and DMA modes.
 *
 *  \param  chanHandle  [IN]  Handle to the Channel
 *  \param  ioPacket    [IN]  Packet passed by the stream layer
 *
 *  \return If successful, returns IOM_COMPLETED else IOM_EBADIO
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          ioPacket     must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 uartSubmitIoReq(Uart_ChanObj        *chanHandle,
                             IOM_Packet          *ioPacket)
{
    Uint32        key         = 0;
    Int32         retVal      = IOM_COMPLETED;
    Uart_Object  *instHandle  = NULL;

    /* Validating all the input parameters inputs                             */
    assert((NULL != chanHandle) && (NULL != ioPacket));

    /* get the handle the instance                                            */
    instHandle = (Uart_Object *)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* If polled mode is set with the active iop as the received iop call     *
     * the function "uartRxIntrHandler" where the control will block          *
     * till it gets the requested number of bytes.                            */
    if (Uart_OpMode_POLLED == instHandle->opMode)
    {
        if (Uart_DriverState_PWRM_SUSPEND != instHandle->devState)
        {
            /* we will protect this function with a semaphore because in case *
             * of multiple tasks submitting the IO requests we will be running*
             * in to race conditions hence in polled mode we will only allow  *
             * only one task to complete the request in polled mode. and the  *
             * other task will be pending on the semaphore.(until the first   *
             * task has completed the IO).                                    */
            if (TRUE == SEM_pend(&(instHandle->syncSem),
                            instHandle->polledModeTimeout))
            {
                retVal = uartSubmitIoReqPolled(chanHandle,ioPacket);

#ifdef BIOS_PWRM_ENABLE
                if ((TRUE == instHandle->devParams.pscPwrmEnable) &&
                    (TRUE == instHandle->pwrmInfo.ioSuspend))
                {
                    /* set the current active IOP as NULL                     */
                    chanHandle->activeIOP = NULL;

                    /* set the driver state as suspended to prevent any new   *
                     * IOP from getting loaded.                               */
                    instHandle->devState = Uart_DriverState_PWRM_SUSPEND;

                    if ((NULL == instHandle->rcvChanObj.activeIOP) &&
                        (NULL == instHandle->xmtChanObj.activeIOP))
                    {
                        /* if both the channels are inactive then reset the   *
                         * io suspend flag                                    */
                        instHandle->pwrmInfo.ioSuspend = FALSE;

                        /* call the delayed completion function               */
                        (instHandle->pwrmInfo.delayedCompletionFxn  \
                            [instHandle->pwrmInfo.pwrmEvent])();
                    }
                }
#endif
                /* we have completed the IO processing hence post  semaphore  */
                SEM_post(&(instHandle->syncSem));
            }
            else
            {
                retVal = IOM_ETIMEOUT;
            }
        }
        else
        {
            /* Driver is in power saving mode and cannot accept new IOs       */
            retVal = IOM_EBADMODE;
        }
    }
    else
    {
        /* interrupt mode of operation                                        */
        /* disable the interrupts                                             */
        key = (Uint32)_disable_interrupts();
        retVal = uartSubmitIoReqIntDma(chanHandle,ioPacket,key);
        _restore_interrupts(key);
    }
    return retVal;
}

/**
 *  \brief   Cancels the current ioo transaction on the channel
 *
 *   Cancel IO function - cancels the current request only.
 *   Used for canceling the request incase of an Timeout
 *
 *  \param  chanHandle  [IN]    Handle to the Channel
 *  \param  ioPacket    [IN]    Stream IO Packet to be completed
 *
 *  \return None
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          ioPacket     must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
static Void uartCancelCurrentIo(Uart_ChanObj       *chanHandle,
                                IOM_Packet         *ioPacket)
{
    Uint32       key        = 0;
    Uart_Object *instHandle = NULL;

    assert(NULL != chanHandle);

    assert(NULL != chanHandle->devHandle);
    instHandle = chanHandle->devHandle;

    /* Protect from Uart interrupt and task switching so that                 *
     * the process of cancellation is not interrupted                         */
    key = (Uint32)_disable_interrupts();

    /* if iop passed is null the update iop with active iop which is to       *
     * be cancelled                                                           */
    if (NULL == ioPacket)
    {
        ioPacket = chanHandle->activeIOP;
    }

    if (chanHandle->activeIOP == ioPacket)
    {
        /* Current IOP                                                        */
        if (NULL != chanHandle->activeIOP)
        {
            chanHandle->bytesRemaining = 0;

            if (Uart_INPUT == chanHandle->mode)
            {
                ((Uart_Object *)chanHandle->devHandle)->stats.rxBytes
                    += ioPacket->size;
            }
            else
            {
                ((Uart_Object *)chanHandle->devHandle)->stats.txBytes
                    += ioPacket->size;
            }

            ioPacket->status = IOM_ABORTED;
            Uart_localCompleteCurrentIO(chanHandle);

            if (FALSE == QUE_empty(&(chanHandle->queuePendingList)))
            {
                /* The last Active IOP got completed, so need                 *
                 * to modify this pointer                                     */
                ioPacket = (IOM_Packet *)
                                     QUE_get(&(chanHandle->queuePendingList));

                chanHandle->activeIOP      = ioPacket;
                chanHandle->activeBuf      = ioPacket->addr;
                chanHandle->bytesRemaining = ioPacket->size;
                chanHandle->errors = 0;
            }
            else
            {
                chanHandle->activeIOP  = NULL;

                if (IOM_COMPLETED == Uart_localLpscOn(instHandle))
                {
                    if (Uart_INPUT == chanHandle->mode)
                    {
                        uartIntrDisable(chanHandle->devHandle,
                            (Uint32)Uart_Intr_RHR);
                    }
                    else
                    {
                        uartIntrDisable(chanHandle->devHandle,
                            (Uint32)Uart_Intr_THR);
                    }

                    Uart_localLpscOff(instHandle);
                }
            }
        }
    }
    else
    {
        QUE_remove((Ptr)ioPacket);
    }
    _restore_interrupts(key);
}


/**
 *  \brief Cancels all iops in the tx/rx channel
 *
 *  This function cancels all the request pending with the Channel.
 *
 *  \param  chanHandle   [IN]   Handle to the Channel Object
 *
 *  \return None
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void uartCancelAllIo(Uart_ChanObj *chanHandle)
{
    Uint32               key        = 0;
    IOM_Packet          *ioPacket   = NULL;
    Uart_Object         *instHandle = NULL;

    /* validate params                                                        */
    assert(NULL != chanHandle);

    instHandle = chanHandle->devHandle;
    assert(NULL != instHandle);

    key = (Uint32)_disable_interrupts();
#ifdef Uart_EDMA_ENABLE
    if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        /* we will disable the edma transfer because a transfer might be in   *
         * progress                                                           */
        EDMA3_DRV_disableTransfer(
            chanHandle->hEdma,
            chanHandle->edmaChId,
            EDMA3_DRV_TRIG_MODE_EVENT);
    }
#endif
    ioPacket = chanHandle->activeIOP;

    /* if there is an active iop we will abort it.it is OK if the active IOP  *
     * is NULL because it means the driver has just finished the IOP or has No*
     * IOPs at ALL. in case it has just finished the IOP pending que could    *
     * still have some more IOPs and those will be aborted                    */
    if (NULL != ioPacket)
    {
        /* active packet is available we will abort it                        */
        ioPacket->status = IOM_ABORTED;
        chanHandle->bytesRemaining = 0;

        if (Uart_INPUT == chanHandle->mode)
        {
            instHandle->stats.rxBytes  += ioPacket->size;
        }
        else
        {
            instHandle->stats.txBytes  += ioPacket->size;
        }

        /* switch OFF the module in the PSC                                   */
        Uart_localLpscOff(instHandle);

        /* callback function is available or not                              */
        if ((NULL != chanHandle->cbFxn) &&
            (Uart_OpMode_POLLED != instHandle->opMode))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
        }

        chanHandle->activeIOP = NULL;
    }

    /* Empty the pendList queue. Common for DMA/Interrupt Transactions        */
    while (TRUE != QUE_empty(&(chanHandle->queuePendingList)))
    {
        ioPacket = (IOM_Packet *)
                        QUE_get(&(chanHandle->queuePendingList));

        ioPacket->status = IOM_ABORTED;
        ioPacket->size = 0;

        /* switch OFF the module in the PSC                                   */
        Uart_localLpscOff(instHandle);

        /* callback function is available or not                              */
        if ((NULL != chanHandle->cbFxn) &&
            (Uart_OpMode_POLLED != instHandle->opMode))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
        }
    }
    _restore_interrupts(key);
}


/**
 *  \brief   This function completes the processing of the current active IOP.
 *          it calls the call back function registered with it so that the
 *          IOP processing is completed.
 *
 *  \param   chanHandle   [IN]  Handle to the channel
 *
 *  \return  None
 */
Void Uart_localCompleteCurrentIO(Uart_ChanObj *chanHandle)
{
    Uart_Object        *instHandle  = NULL;
    Int32               retVal      = IOM_COMPLETED;
    Uint32              hwiKey      = 0;
#ifdef Uart_EDMA_ENABLE
    Uint32              bytesRemain = 0;
    EDMA3_DRV_PaRAMRegs paramSet    = {0,0,0,0,0,0,0,0,0,0,0,0};
#endif
    hwiKey = _disable_interrupts();
    assert(NULL != chanHandle);
    instHandle =  (Uart_Object*)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* call the application completion callback function registered           *
     * with us during opening of the channel                                  */
    if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg) &&
        (Uart_OpMode_POLLED != instHandle->opMode))
    {
        /* Update the packet status                                           */
        if (0 != chanHandle->errors)
        {
            /* Priority is for the appropriate status set before calling this *
             * completion function for the IOP                                */
            if(IOM_COMPLETED == chanHandle->activeIOP->status)
            {
                chanHandle->activeIOP->status = IOM_EBADIO;
            }
        }
        else
        {
            chanHandle->activeIOP->status = IOM_COMPLETED;
        }

        /* Update the size                                                    */
        if (Uart_OpMode_INTERRUPT == instHandle->opMode)
        {
            chanHandle->activeIOP->size -= chanHandle->bytesRemaining;
        }
#ifdef Uart_EDMA_ENABLE
        else if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
        {
            /* Get the PaRAM set for default parameters                       */
            EDMA3_DRV_getPaRAM(chanHandle->hEdma,
                               chanHandle->edmaChId,
                               &paramSet);
            /* calculate the amount of bytes remaining                        */
            bytesRemain = (paramSet.aCnt * paramSet.bCnt * paramSet.cCnt);
            chanHandle->activeIOP->size -= bytesRemain;
        }
#endif
        else
        {
            /* Do nothing - poll mode is handled in transfer function itself  */
        }

        /* switch OFF the module in the PSC                                   */
        retVal = Uart_localLpscOff(instHandle);

        /* when setting the status of the IOP,priority is given to the error  *
         * generated by the IOP. if the IOP has completed successfully, then  *
         * the priority will be for the error (if any) generated by the PSC   *
         * while switching off the module                                     */
        if (IOM_COMPLETED == chanHandle->activeIOP->status)
        {
            chanHandle->activeIOP->status = retVal;
        }

        /* Invoke Application callback for this channel                       */
        (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,chanHandle->activeIOP);
    }

    chanHandle->activeIOP = NULL;

    _restore_interrupts(hwiKey);
}

/**
 *  \brief  Receiver Errors
 *
 *  This function finds errors on the Receiver buffer.
 *
 *  \param  chanHandle     [IN]   Handle of the Channel Object
 *
 *  \return If successful, returns IOM_COMPLETED
 *          or IOM_EBADARGS in case of error
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 uartHandleRxError(Uart_ChanObj  *chanHandle)
{
    Uint32        status       = 0;
    Uint32        iteration    = 0;
    Int32         retVal       = IOM_COMPLETED;
    Uint32        tempByte     = 0;
    Uint32        delay        = 0xFFFFu;
    Uart_Object  *instHandle   = NULL;
#ifdef BIOS_PWRM_ENABLE
    Uns           count       = 0x00;
#endif
    assert(NULL != chanHandle);
    instHandle =  (Uart_Object*)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* check for the errors and if there is any error update the stats        */
    status = (instHandle->deviceInfo.baseAddress)->LSR;

    /* Notify the upper layer about the errors, if notification is registered */
    if (NULL != chanHandle->gblErrCbkFxn)
    {
        (*chanHandle->gblErrCbkFxn)((Uint32)chanHandle->gblErrCbkArg,
                                    status,
                                    NULL);
    }
#ifdef Uart_EDMA_ENABLE
    if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        
       /* Disable the EDMA                                                    */
       EDMA3_DRV_disableLogicalChannel(
            chanHandle->hEdma,
            chanHandle->edmaChId,
            EDMA3_DRV_TRIG_MODE_EVENT);
    }
#endif
    if (CSL_UART_LSR_OE_MASK == (status & CSL_UART_LSR_OE_MASK))
    {
        instHandle->stats.overrun++;
        chanHandle->errors++;
    }        

    if (CSL_UART_LSR_RXFIFOE_MASK == (status & CSL_UART_LSR_RXFIFOE_MASK))
    {
        if(CSL_UART_LSR_FE_MASK == (status & CSL_UART_LSR_FE_MASK))
        {
            instHandle->stats.rxFramingError++ ;
            chanHandle->errors++;
        }

        if (CSL_UART_LSR_PE_MASK == (status & CSL_UART_LSR_PE_MASK))
        {
            instHandle->stats.rxParityError++ ;
            chanHandle->errors++;
        }

        if (CSL_UART_LSR_BI_MASK == (status & CSL_UART_LSR_BI_MASK))
        {
            instHandle->stats.rxBreakError++ ;
            chanHandle->errors++;
        }
    }
    
    /* Clearing Receive Errors(FE,BI,PE)by reading erroneous data from RX FIFO*/
    /* Iteration count: Worst case = FIFO size                                */
    if (TRUE == instHandle->devParams.fifoEnable)
    {
        /* Iteration count:  FIFO size(16)  if FIFO enabled               */
        iteration = Uart_FIFO_SIZE;
    }    
    else
    {
        iteration = 1;
    }
    
    do
    {
        /* Read and throw error byte                                          */
        /* Till Line status int is pending                                    */
        tempByte = (Uint8)((instHandle->deviceInfo.baseAddress)->RBR);
        /* To remove compiler "unused" warnings                               */
        tempByte = tempByte;
        
        iteration--;
        while( (instHandle->deviceInfo.baseAddress->LSR
                & CSL_UART_LSR_DR_MASK) == 0)
        {
            if (delay == 0)
            {
                break;
            }
            delay--;
        }

        status = (instHandle->deviceInfo.baseAddress)->LSR;
        status &= (CSL_UART_LSR_BI_MASK |
                   CSL_UART_LSR_FE_MASK |
                   CSL_UART_LSR_PE_MASK |
                   CSL_UART_LSR_OE_MASK |
                   CSL_UART_LSR_RXFIFOE_MASK);
    } while( ( status != 0 ) && (iteration != 0) );

    if(NULL != chanHandle->activeIOP)
    {
        Uart_localCompleteCurrentIO(chanHandle);
    }
#ifdef BIOS_PWRM_ENABLE    
    if(TRUE == instHandle->pwrmInfo.ioSuspend)
    {
#endif
        Uart_loadPendedIops(chanHandle);
#ifdef BIOS_PWRM_ENABLE
    }
    else
    {
        if ((TRUE == instHandle->devParams.pscPwrmEnable) &&
             (TRUE == instHandle->pwrmInfo.ioSuspend))
        {
            /* set the current active IOP as NULL                 */
            chanHandle->activeIOP = NULL;
            instHandle->devState = Uart_DriverState_PWRM_SUSPEND;
        
            if ((NULL == instHandle->rcvChanObj.activeIOP) &&
                (NULL == instHandle->xmtChanObj.activeIOP))
            {
                /* if both the channels are inactive then reset   *
                 * the io suspend flag                            */
                instHandle->pwrmInfo.ioSuspend = FALSE;
        
                if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                    (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
                {
                    /* reduce the dependency count                    */
                    status = PWRM_getDependencyCount(
                                (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId,
                                 &count);
                
                    instHandle->pwrmInfo.dependencyCount = count;
                
                    if (PWRM_SOK == status)
                    {
                        while (count > 0)
                        {
                            status =  PWRM_releaseDependency(
                                      (PWRM_Resource)
                                      instHandle->deviceInfo.pwrmLpscId);
                
                            if (PWRM_SOK != status)
                            {
                                break;
                            }
                            count--;
                        }
                    }
                }                            
                /* call the delayed completion function           */
                (instHandle->pwrmInfo.delayedCompletionFxn  \
                    [instHandle->pwrmInfo.pwrmEvent])();
            }
        }
    }
#endif                    
    return retVal;
}


/**
 *  \brief   checks if the data is ready
 *
 *  \param   chanHandle   [IN]     Handle to the channel Object
 *
 *  \return  TRUE  if data is ready
 *          FALSE  if not ready
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Bool uartStatusIsDataReady(Uart_ChanObj *chanHandle)
{
    Uint32        status     = 0;
    Bool          retVal     = FALSE;
    Uart_Object  *instHandle = NULL;

    assert(NULL != chanHandle);
    instHandle =  (Uart_Object*)chanHandle->devHandle;
    assert(NULL != instHandle);

    status = (instHandle->deviceInfo.baseAddress)->LSR;

    /* Added for error checks                                                 */
    if (CSL_UART_LSR_RXFIFOE_MASK == (status & CSL_UART_LSR_RXFIFOE_MASK))
    {
        uartHandleRxError(chanHandle);
    }

    if (CSL_UART_LSR_DR_MASK == (status & CSL_UART_LSR_DR_MASK))
    {
        retVal = TRUE;
    }

    return retVal;
}

/**
 *  \brief  Write into TX FIFO
 *
 *  This function writes into TX FIFO from the iop buffer
 *
 *  \param  chanHandle    [IN]             Handle of the Channel Object
 *  \param  buffer        [IN]             Buffer for the operation
 *  \param  nwrite        [IN]             Number of bytes left to be written
 *
 *  \return Nunber of bytes written
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          buffer       must be a valid pointer and should not be null.
 *          nwrite       must be a valid nwrite value.
 *
 *  \leave  Not Implemented.
 */
static Uint32 uartFifoWrite(Uart_ChanObj      *chanHandle,
                            const Uint8       *buffer,
                            Uint32             nwrite)
{
    Uint32            size          = nwrite;
    Uint32            lineStatus    = 0;
    Uint32            tempChunksize = 0;
    Uart_Object      *instHandle    = NULL;
    Int32             maxTrialCount = Uart_TRANSMITEMPTY_TRIALCOUNT;

    assert((NULL != chanHandle) && (NULL != buffer));
    instHandle =  (Uart_Object*)chanHandle->devHandle;
    assert(NULL != instHandle);

    /* Load the chunk size(fifo)                                              */
    tempChunksize = chanHandle->chunkSize;

    /* Before we could write 'chunksize' no. of bytes, we should have         *
     * 'chunk size' no. of free buffers, (in this case max 16 bytes)          *
     * hence we check for shiftregister empty (ensure the whole FIFO          *
     * is empty) to write 16bytes                                             */

    do
    {
        lineStatus = (instHandle->deviceInfo.baseAddress)->LSR;
        maxTrialCount--;

    }while (((CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK) !=
                ((CSL_UART_LSR_TEMT_MASK|CSL_UART_LSR_THRE_MASK) & lineStatus))
            && (0 < maxTrialCount));

    if (0 != maxTrialCount)
    {
        while ( ((tempChunksize != 0)&&(nwrite != 0)) != 0)
        {
            /* Writing to the H/w                                             */
            uartWriteByte(chanHandle->devHandle,(Char)(*buffer));
            buffer++;
            nwrite--;
            tempChunksize--;
        }
    }

    /* Returns the size actually written                                      */
    return (size - nwrite);
}


/**
 *  \brief  Read from RX FIFO
 *
 *  This function reads from the RX FIFO into the iop buffer
 *
 *  \param  chanHandle  [IN]    Handle of the Channel Object
 *  \param  buffer      [IN]    Buffer for the operation
 *  \param  bufSize     [IN]    Number of bytes left to be read
 *
 *  \return Number of bytes read
 *
 *  \enter  chanHandle   must be a valid pointer and should not be null.
 *          buffer       must be a valid pointer and should not be null.
 *          bufSize      must be a valid buffer size value.
 *
 *  \leave  Not Implemented.
 */
static Uint32 uartFifoRead(Uart_ChanObj     *chanHandle,
                            Uint8            *buffer,
                            Uint32            bufSize)
{
    Uint32       size      = bufSize;
    Bool         isRxReady = FALSE;

    assert((NULL != chanHandle) && (NULL != buffer));

    isRxReady = uartStatusIsDataReady(chanHandle);

    while ((TRUE == isRxReady) && (0 != bufSize))
    {
        /* once the H/w is ready  reading from the H/w                        */
        *buffer = (Uint8)uartReadByte(chanHandle->devHandle);
        buffer++;
        --bufSize;

        isRxReady = uartStatusIsDataReady(chanHandle);
    }

    return (size - bufSize);
}


/**
 *  \brief    This function reads or writes the data depending on the
 *            channel type.
 *
 *  \param    chanHandle        [IN]    Handle to the Channel
 *  \param    buffer            [IN]    Buffer for the transfer
 *  \param    numBytes          [IN]    No. of bytes to be transferred
 *
 *  \return   No of bytes processed.
 *
 *  \enter    chanHandle   must be a valid pointer and should not be null.
 *            buffer       must be a valid pointer and should not be null.
 *            numBytes     must be a valid number of bytes value.

 *
 *  \leave  Not Implemented.
 */
static Uint32 uartXfer(Uart_ChanObj         *chanHandle,
                       Uint8                *buffer,
                       Uint32                numBytes)
{
    Uint32 xferedBytes = 0;

    assert((NULL != chanHandle) && (NULL != buffer));

    /* Depending on the mode call its respective functions                    */
    if (Uart_INPUT == chanHandle->mode )
    {
        /* receive channel hence read the data                                */
        xferedBytes = uartFifoRead(chanHandle, buffer, numBytes);
    }
    else
    {
        /* transmit channel hence write the data                              */
        xferedBytes = uartFifoWrite(chanHandle, buffer, numBytes);
    }

    /* return the number of bytes processed(read or written)                  */
    return (xferedBytes);
}


/**
 *  \brief Uart ISR
 *
 *  This function is the ISR for the entire Uart device. This routine further
 *  processes the exact cause of interrupt by reading IIR
 *
 *  \param  arg  [IN]    Handle to the Uart driver object
 *
 *  \return If successful, returns IOM_COMPLETED or IOM error codes
 *
 *  \enter  arg   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
Void uartIsr(Arg arg)
{
    volatile Uint32   status      = 0x01u;
    Uint32            intrCnt     = 0;
    Uart_Object      *instHandle  = NULL;
    Uart_ChanObj     *chanHandle  = NULL;
    Uint32            xfer        = 0;
#ifdef BIOS_PWRM_ENABLE
    Uns               count       = 0x00;
#else
    IOM_Packet       *ioPacket    = NULL;
#endif

    assert(NULL != arg);

    instHandle = (Uart_Object*)arg;
    status = (Uint32)uartGetIntrStatus(instHandle);
    
    while ((CSL_UART_IIR_IPEND_PEND == (status & CSL_UART_IIR_IPEND_NO_PEND))
          && ((intrCnt) < (Uart_MAX_ISR_LOOP)))
    {
        intrCnt++;

        /*  check for any line error while transfer                           */
        if (CSL_UART_IIR_INTID_RLS ==
                (CSL_UART_IIR_INTID_RLS & (status >> CSL_UART_IIR_INTID_SHIFT)))
        {
            /* call this function when ther is any line error                 */
            uartHandleRxError(&(instHandle->rcvChanObj));
        }
        else if(Uart_OpMode_INTERRUPT == instHandle->opMode)
        {
            /* check whether any character timeout happened                   */
            if( CSL_UART_IIR_INTID_RDA ==
                (CSL_UART_IIR_INTID_RDA & (status >> CSL_UART_IIR_INTID_SHIFT)))
            {
                if(CSL_UART_IIR_INTID_CTI ==
                    (CSL_UART_IIR_INTID_CTI & (status >> CSL_UART_IIR_INTID_SHIFT)))
                {
                    chanHandle = &(instHandle->rcvChanObj);
                    /* if timeout error then update stats                     */
                    instHandle->stats.rxTimeout++;

                    /* Increment the error count                              */
                    chanHandle->errors++;

                    /* Update the IOP with proper status                      */
                    chanHandle->activeIOP->status = IOM_ETIMEOUT;

                    /* Transfer the remanant bytes if any are remaining in the*
                     * FIFO                                                   */
                    xfer = uartFifoRead(chanHandle,
                            chanHandle->activeBuf,
                            chanHandle->bytesRemaining);

                    chanHandle->bytesRemaining    -= xfer;
                    chanHandle->activeBuf         += xfer;
                    instHandle->stats.rxBytes     += xfer;

                    /* Disable the RX interrupts before completing. If only   *
                     * there is a packet for processing it shall be re-enabled*/
                    uartIntrDisable(instHandle,
                        (Uint32)(Uart_Intr_RLS | Uart_Intr_RHR));

                    /* Perform the operation to complete the IO               */
                    Uart_localCompleteCurrentIO(chanHandle);
#ifdef BIOS_PWRM_ENABLE
                    if ((TRUE == instHandle->devParams.pscPwrmEnable) &&
                         (TRUE == instHandle->pwrmInfo.ioSuspend))
                    {
                        /* set the current active IOP as NULL                 */
                        chanHandle->activeIOP = NULL;
                        instHandle->devState = Uart_DriverState_PWRM_SUSPEND;

                        if ((NULL == instHandle->rcvChanObj.activeIOP) &&
                            (NULL == instHandle->xmtChanObj.activeIOP))
                        {
                            /* if both the channels are inactive then reset   *
                             * the io suspend flag                            */
                            instHandle->pwrmInfo.ioSuspend = FALSE;

                            if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                                (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
                            {
                                /* reduce the dependency count                    */
                                status = PWRM_getDependencyCount(
                                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId,
                                             &count);
                            
                                instHandle->pwrmInfo.dependencyCount = count;
                            
                                if (PWRM_SOK == status)
                                {
                                    while (count > 0)
                                    {
                                        status =  PWRM_releaseDependency(
                                                  (PWRM_Resource)
                                                  instHandle->deviceInfo.pwrmLpscId);
                            
                                        if (PWRM_SOK != status)
                                        {
                                            break;
                                        }
                                        count--;
                                    }
                                }
                            }                            
                            /* call the delayed completion function           */
                            (instHandle->pwrmInfo.delayedCompletionFxn  \
                                [instHandle->pwrmInfo.pwrmEvent])();
                        }
                    }
#else
                    /* Now that we have just completed the current IOP        *
                     * we proceed to check if there are still packets         *
                     * pending in pending queue                               */
                    if (FALSE == QUE_empty(&(chanHandle->queuePendingList)))
                    {
                        /* we have atleast one packet                                         */
                        ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));

                        /* validate and update the iop                                        */
                        if (NULL  != ioPacket)
                        {
                            chanHandle->activeIOP      = ioPacket;
                            chanHandle->activeBuf      = ioPacket->addr;
                            chanHandle->bytesRemaining = ioPacket->size;
                            chanHandle->errors = 0;
                        }
                        uartIntrEnable(instHandle,
                           (Uint32)(Uart_Intr_RLS | Uart_Intr_RHR));
                    }
                    else
                    {
                        chanHandle->activeIOP = NULL;
                    }
#endif
                }
                else
                {
                    /* Disable the receive interrupt until current interrupt  *
                     * is processed. Re-enabled again in uartIntrHandler()    */
                    uartIntrDisable(instHandle,(Uint32)(Uart_Intr_RHR));

                    SWI_post(instHandle->rxTskletHandle);
                }
            }
            else
            {
                /* check whether Tx register is empty or not                  */
                if (CSL_UART_IIR_INTID_THRE ==
                    (CSL_UART_IIR_INTID_THRE & (status >> CSL_UART_IIR_INTID_SHIFT)))
                {
                    /* Disable the transmit interrupt until current interrupt *
                     * is processed. Re-enabled again in uartIntrHandler()    */
                    uartIntrDisable(instHandle,(Uint32)(Uart_Intr_THR));
                    SWI_post(instHandle->txTskletHandle);
                }
                else
                {
                    /* check modem status                                     */
                    if (CSL_UART_IIR_INTID_MODSTAT ==
                        (CSL_UART_IIR_INTID_MODSTAT & (status >> CSL_UART_IIR_INTID_SHIFT)))
                    {
                        /* Read the MSR to clear Modem Status Int cause       */
                        (instHandle->deviceInfo.baseAddress)->SCR =
                            (instHandle->deviceInfo.baseAddress)->MSR;
                    }
                }
            }
        }
        else
        {
            /* Do nothing                                                     */
        }
        status = uartGetIntrStatus(instHandle);
    }
}

/**
 *  \brief  The entry function for Tx Swi Handler
 *
 *  \param  arg0       [IN]  Handle of the Channel Object
 *  \param  unUsedArg  [IN]  Handle to unused arguments
 *
 *  \return None
 *
 *  \enter  arg0        must be a valid pointer and should not be null.
 *          unUsedArg   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void uartTxSwiHandler(Arg arg0, Arg unUsedArg)
{
    Uart_ChanObj *chanHandle = NULL;

    assert(NULL != arg0);

    chanHandle = (Uart_ChanObj*)arg0;

    /* To remove the compiler warning                                         */
    unUsedArg = unUsedArg;
    uartIntrHandler(chanHandle);
}

/**
 *  \brief  The entry function for Rx Swi Handler
 *
 *  \param  arg0       [IN]  Handle of the Channel Object
 *  \param  unUsedArg  [IN]  Handle to unused arguments
 *
 *  \return None
 *
 *  \enter  arg0        must be a valid pointer and should not be null.
 *          unUsedArg   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Void uartRxSwiHandler(Arg arg0, Arg unUsedArg)
{
    Uart_ChanObj *chanHandle = NULL;

    assert(NULL != arg0);

    /* To remove the compiler warning                                         */
    unUsedArg = unUsedArg;

    chanHandle = (Uart_ChanObj*)arg0;

    uartIntrHandler(chanHandle);
}

/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/

/**
 * \brief    Uart_localLpscOn
 *           This function is invoked to power on the UART peripheral in the
 *           PSC.
 *
 * \note     Note that this function uses different functions to power ON the
 *           module, depending on whether the PWRM is enabled in this driver
 *           or NOT
 *
 * \param    chanHandle    [IN]    handle to the IO channel
 *
 * \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 * \enter    chanHandle must be a valid channel object
 *           The driver/channel should be in opened state.
 *
 * \leave    Not Implemented.
 */
Int32 Uart_localLpscOn(Uart_Object *instHandle)
{
    Int32 status = IOM_COMPLETED;

    assert(NULL != instHandle);

    if (TRUE == instHandle->devParams.pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power on using PWRM API                                            */
        status = (Int32)PWRM_setDependency(
                     (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
        /* power on using PSC API                                             */
        status = Psc_ModuleClkCtrl(
                     (Psc_DevId)instHandle->deviceInfo.pscInstance,
                     (Uint32)instHandle->deviceInfo.pwrmLpscId,
                     TRUE);
#endif
    }

    return (status);
}

/**
 * \brief    Uart_localLpscOff
 *           This function is invoked to power off the peripheral in the PSC.
 *
 * \param    chanHandle    [IN]    channel handle
 *
 * \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 * \enter    chanHandle must be a valid channel object
 *           No pending IO
 *
 * \leave    Not Implemented.
 */
Int32 Uart_localLpscOff(Uart_Object *instHandle)
{
    Uint32          waitCnt = Uart_TRANSMITEMPTY_TRIALCOUNT;
    Uint32          tempReg = 0;
    Int32           status  = IOM_COMPLETED;

    assert(NULL != instHandle);

    if (TRUE == instHandle->devParams.pscPwrmEnable)
    {
        /* Check if all bits have been pushed out of the Uart shift register  */
        tempReg = (instHandle->deviceInfo.baseAddress)->LSR;

        /* Waits till the transmitter is empty                                */
        while ((0 == (tempReg & CSL_UART_LSR_TEMT_MASK))
               && (waitCnt != 0))
        {
            tempReg = uartGetLineStatus(instHandle);
            waitCnt--;
        }

#ifdef BIOS_PWRM_ENABLE
        /* power off  using PWRM API                                          */
        status = (Int32)PWRM_releaseDependency(
                    (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
        /* power off using PSC API                                            */
        status = Psc_ModuleClkCtrl(
                    (Psc_DevId)instHandle->deviceInfo.pscInstance,
                    (Uint32)instHandle->deviceInfo.pwrmLpscId,
                    FALSE);
#endif
    }

    return (status);
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
Void Uart_loadPendedIops(Uart_ChanObj  *chanHandle)
{
    Uart_Object          *instHandle = NULL;
    IOM_Packet           *ioPacket   = NULL;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->devHandle);

    instHandle = (Uart_Object *)chanHandle->devHandle;

    if (FALSE == QUE_empty(&(chanHandle->queuePendingList)))
    {
        /* we have atleast one packet                                         */
        ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));

        /* validate and update the iop                                        */
        if (NULL  != ioPacket)
        {
            chanHandle->activeIOP      = ioPacket;
            chanHandle->activeBuf      = ioPacket->addr;
            chanHandle->bytesRemaining = ioPacket->size;
            chanHandle->errors = 0;

            if (Uart_OpMode_INTERRUPT == instHandle->opMode)
            {
                if (Uart_INPUT == chanHandle->mode)
                {
                    uartIntrEnable(instHandle,
                        (Uint32)(Uart_Intr_RHR | Uart_Intr_RLS));
                }
                else
                {
                    uartIntrEnable(instHandle, (Uint32)Uart_Intr_THR);
                }
            }
    
#ifdef Uart_EDMA_ENABLE
            if (Uart_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                Uart_localStartEdmaTransfer(chanHandle,ioPacket->cmd);
            }
        }
#endif
    }
    else
    {
        chanHandle->activeIOP = NULL;
    }
}


#ifdef BIOS_PWRM_ENABLE
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
static Int32 UartRegisterNotification(Uart_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Uart_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    UartNotifyCallback,
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
static Int32 UartUnregisterNotification(Uart_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Uart_MAX_PWRM_EVENTS; eventCnt++)
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
static PWRM_NotifyResponse UartNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg)
{
    Uart_Object          *instHandle = NULL;
    PWRM_NotifyResponse   pwrmStatus = PWRM_NOTIFYDONE;
    PWRM_Status           status     = PWRM_SOK;

    assert(NULL != clientArg);

    instHandle = (Uart_Object *)clientArg;
    assert(NULL != instHandle);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
            pwrmStatus = UartSuspendCurrentIops(instHandle,eventType);
            break;
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
            /* CPU or PERIPHERAL domain V/F setpoint or a sleep command is    *
             * issued hence we will complete the current IO and suspend all   *
             * the pending IOs                                                */
            if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                     (Uart_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                     (Uart_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                pwrmStatus = UartSuspendCurrentIops(instHandle,eventType);
            }
            break;
        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
            /* The pending CPU setpoint change has now been made.Hence now we *
             * will recalculate the new settings for this instance and then   *
             * restore the pending state of the driver                        */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                     (Uart_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                      (Uart_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                pwrmStatus = UartConfigureNewVfSettings(
                                 instHandle,
                                 eventArg1,
                                 eventArg2);
            }

            if (PWRM_NOTIFYDONE == pwrmStatus)
            {
                /* Resume both the channels                                   */
                uartConfigure(instHandle,&instHandle->devParams);
                Uart_loadPendedIops(&instHandle->xmtChanObj);
                Uart_loadPendedIops(&instHandle->rcvChanObj);
                /* update the state of the driver                             */
                instHandle->devState = Uart_DriverState_CREATED;
            }
            break;
        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
            /* restore the dependency count                                   */
            while (instHandle->pwrmInfo.dependencyCount > 0)
            {
                status = PWRM_setDependency(
                             (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);

                if (PWRM_SOK != status)
                {
                    break;
                }
                instHandle->pwrmInfo.dependencyCount--;
            }

            if (PWRM_SOK == status)
            {
                /* Resume both the channels                                   */
                uartConfigure(instHandle,&instHandle->devParams);
                Uart_loadPendedIops(&instHandle->xmtChanObj);
                Uart_loadPendedIops(&instHandle->rcvChanObj);
                /* update the state of the driver                             */
                instHandle->devState = Uart_DriverState_CREATED;
            }
            else
            {
                pwrmStatus = PWRM_NOTIFYNOTDONE;
            }
            break;
        case PWRM_AWAKEFROMSTANDBY:
            /* Resume both the channels                                       */
            uartConfigure(instHandle,&instHandle->devParams);
            Uart_loadPendedIops(&instHandle->xmtChanObj);
            Uart_loadPendedIops(&instHandle->rcvChanObj);
            /* update the state of the driver                                 */
            instHandle->devState = Uart_DriverState_CREATED;
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
 * \return   PWRM_NOTIFYDONE    if no IOPs are pending else
 *           PWRM_NOTIFYNOTDONE if IOPs are pending in the driver.
 */
static PWRM_NotifyResponse UartSuspendCurrentIops(Uart_Object  *instHandle,
                                                  PWRM_Event    eventType)
{
    Uint32               hwiKey = 0x00;
    PWRM_NotifyResponse  status = PWRM_NOTIFYDONE;
    
    assert(NULL != instHandle);
    
    hwiKey = _disable_interrupts();

    /* check if both of the channels have any active IOP                      */
    if ((NULL == instHandle->rcvChanObj.activeIOP) &&
        (NULL == instHandle->xmtChanObj.activeIOP))
    {
        /* No IOP is pending currently. we can return the status of the       *
         * notify call as sucess                                              */
        instHandle->devState = Uart_DriverState_PWRM_SUSPEND;
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
 * \brief     This function calculates the constraints for a device at all
 *            the setpoints supported for the SOC. it then registers the
 *            appropriate contraints for each of the setpoints not supported.
 *
 * \param     instHandle [IN] Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADMODE  in case of error
 *
 * \enter     instHandle is a NON null valid pointer
 */
static Int32 UartCalculateConstraints(Uart_Object *instHandle)
{
    Uns             numberSetpoints = 0x00;
    Uns             setpoint        = 0x00;
    Uint32          prescale        = 0x00;
    Uint32          mask            = 0x00;
    Int32           status          = IOM_COMPLETED;
    PWRM_Constraint constraint      = PWRM_DISALLOWEDSLEEPSTATE_MASK;
    PWRM_Status     retVal          = PWRM_SOK;
    PWRM_Domain     domain          = PWRM_CPU;

    assert(NULL != instHandle);

    /* check what clock domain is supported by the device                     */
    if (Uart_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
    {
        domain = PWRM_CPU;
        constraint = PWRM_DISALLOWED_CPU_SETPOINT_MASK;
    }
    else
    {
        domain = PWRM_PER;
        constraint = PWRM_DISALLOWED_PER_SETPOINT_MASK;
    }

    /* get the number of supported setpoint info for this instance            */
    retVal = PWRM_getNumSetpoints(domain, &numberSetpoints);

    if (PWRM_SOK == retVal)
    {
        /* calculate the contraints for the required bus frequency            */
        for (setpoint = 0x00; setpoint < numberSetpoints; setpoint++)
        {
            status = UartCalculateClockSettings(
                          instHandle,
                          setpoint,
                          instHandle->devParams.baudRate,
                          &prescale);

            if (IOM_COMPLETED == status)
            {
                if ((1 > prescale) || (prescale > 0xFFFFu))
                {
                    /* register the constraint for this setpoint              */
                    mask |= (1 << setpoint);
                }
            }
            else
            {
                retVal = PWRM_EINVALIDVALUE;
                break;
            }
        }

        /* check if this instance has already registered some constraint      *
         * if so then unregister those before registering the new             *
         * contraints(for CPU or PER event's unsupported DVFS only)           */
        if (PWRM_SOK == retVal)
        {
            if (NULL != instHandle->pwrmInfo.constraintHandle[constraint - 1])
            {
                retVal = PWRM_unregisterConstraint(
                              instHandle->pwrmInfo.constraintHandle[constraint - 1]);
            }

            /* register the new set of constraints (only if any contraints    *
             * are present)                                                   */
            if ((PWRM_SOK == retVal) && (0 != mask))
            {
                /* store the value of the constraint mask being registered    */
                instHandle->pwrmInfo.constraintMask = mask;

                retVal = PWRM_registerConstraint(
                             constraint,
                             mask,
                             &instHandle->pwrmInfo.constraintHandle[constraint - 1]);
            }
        }
    }

    if (PWRM_SOK != retVal)
    {
        /* failed in calculating the constraints                              */
        status = IOM_EBADMODE;
    }
    return (status);
}

/**
 * \brief      This function calculates clock settings for the new DVFS set
 *             point.
 *
 * \param      instHandle [IN] Handle to the device instance object
 * \param      eventArg1  [IN] previous set point.
 * \param      eventArg2  [IN] current set point
 *
 * \return     PWRM_SOK incase that the prescaler values for the new set point
 *             are calculated successfully else an appropriate error code is
 *             returned
 *
 * \enter      instHandle is a valid non NULL pointer
 *
 * \leave      Not imeplemented
 */
static PWRM_NotifyResponse UartConfigureNewVfSettings(Uart_Object   *instHandle,
                                                      Uns            eventArg1,
                                                      Uns            eventArg2)
{
    Uint32               prescale = 0x00;
    PWRM_NotifyResponse  retVal   = PWRM_NOTIFYDONE;
    Int32                status   = IOM_COMPLETED;

    /* This function will calculate the settings for the new set point and    *
     * also indicate if this is a valid setpoint                              */
    assert(NULL != instHandle);

    /* check if both the set points are not the same                          */
    if (eventArg1 != eventArg2)
    {
        /* The DVFS set point change has happened successfully.calculate the  *
         * new prescaler value                                                */
        status = UartCalculateClockSettings(
                     instHandle,
                     eventArg2,
                     instHandle->devParams.baudRate,
                     &prescale);

        if ((IOM_COMPLETED == status)  &&
            ((1 <= prescale) && (prescale <= 0xFFFFu)))
        {
            /* calculation of the prescaler value is successful.Hence update  *
             * the hardware with the new prescaler values                     */
            status = UartConfigureNewClkSettings(instHandle,prescale);
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }

    if (IOM_COMPLETED != status)
    {
        retVal = PWRM_NOTIFYERROR;
    }

    return (retVal);
}


/**
 * \brief     This function calculates the required prescaler values
 *            for the required ouput frequency for a given setpoint.
 *
 * \param     instHandle [IN]  Handle to the driver instance object
 * \param     setpoint   [IN]  setpoint for which the prescaler is calculated.
 * \param     opFreq     [IN]  desired module IO frequency
 * \param     prescale   [OUT] calculated prescaler value.
 *
 * \return    IOM_COMPLETED if the calculation is completed.
 *            IOM_EBADMODE  if the calcualtion failed for some reason.
 *
 * \enter     instHandle is a valid non null pointer
 *            prescalar is a non null pointer.
 *
 * \leave     Not implemented
 *
 * \note      Please note that the calculated prescaler values are not validated
 *            in this function.
 */
static Int32 UartCalculateClockSettings(Uart_Object   *instHandle,
                                        Uint32         setpoint,
                                        Uart_BaudRate  opFreq,
                                        Uint32        *prescale)
{
    PWRM_Status     retVal          = PWRM_SOK;
    PWRM_Domain     domain          = PWRM_CPU;
    Uns             numberSetpoints = 0x00;
    Uns             frequency       = 0x00;
    Uns             voltage         = 0x00;
    Uint32          sysclkDiv       = 0x00;
    Int32           status          = IOM_COMPLETED;

    assert((NULL != instHandle) && (NULL != prescale));

    /* check what clock domain is supported by the device                     */
    if (Uart_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
    {
        domain = PWRM_CPU;
    }
    else
    {
        domain = PWRM_PER;
    }

    /* get the number of supported setpoint info for this instance            */
    retVal = PWRM_getNumSetpoints(domain, &numberSetpoints);

    if (PWRM_SOK == retVal)
    {
        /* calculate the sysclk divisor value                                 */
        retVal = PWRM_getSetpointInfo(
                     domain,
                     (numberSetpoints - 1u),
                     &frequency,
                     &voltage);

        if (PWRM_SOK == retVal)
        {
            /* calculate the sysclk divisor value                             */
            sysclkDiv = ((frequency * 1000)
                             / instHandle->deviceInfo.inputFrequency);

            /* get the information about the current set point                */
            retVal = PWRM_getSetpointInfo(
                         domain,
                         setpoint,
                         &frequency,
                         &voltage);

            if (PWRM_SOK == retVal)
            {
                /* calculate the new prescaler values                         */
                (*prescale) = (((frequency * 1000)/sysclkDiv)/
                            ((Uint32)Uart_BAUD_MULTIPLIER * (Uint32)opFreq));
            }
        }
    }

    if (PWRM_SOK != retVal)
    {
        status = IOM_EBADMODE;
    }
    return (status);
}

/**
 * \brief     This function configures the Uart device with the new prescalar
 *            values.
 *
 * \param     instHandle [IN]  Handle to the driver instance object
 * \param     prescale   [IN]  prescaler value to program.
 *
 * \return    IOM_COMPLETED  if sucessful
 *            IOM_EBADARGS   in case of error
 *
 * \enter     instHandle is a valid non null pointer
 *            prescalar is a valid value
 *
 * \leave     Not implemented
 *
 */
static Int32 UartConfigureNewClkSettings(Uart_Object *instHandle,
                                         Uint32       prescale)
{
    Int32    status = IOM_EBADARGS;

    assert(NULL != instHandle);

    /* switch on the module in the PSC                                        */
    if (IOM_COMPLETED == Uart_localLpscOn(instHandle))
    {
        /* update the new prescaler settings for the new baud rate            */
        (instHandle->deviceInfo.baseAddress)->DLL = (Uint8)(prescale & 0xFF);
        (instHandle->deviceInfo.baseAddress)->DLH =  \
                                              (Uint8)((prescale & 0xFF00) >> 8);

        /* switch off the module in PSC                                       */
        status = Uart_localLpscOff(instHandle);
    }

    return (status);
}


/**
 * \brief     This function unregisters all the constraints for the given device
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
static Int32 UartUnregisterConstraints(Uart_Object *instHandle)
{
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;
    Uint32       count  = 0x00;

    assert(NULL != instHandle);
    
    for (count = 0; count < Uart_MAX_PWRM_CONSTRAINTS; count++)
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
/*                              END OF FILE                                   */
/* ========================================================================== */
