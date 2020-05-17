/*
 * Spi.c
 *
 * This file contains Driver Layer Interface implementation for the SPI Driver.
 * SPI Driver provides Driver Layer Interface to do operations on the SPI
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
 *  \file   Spi.c
 *
 *  \brief  IOM implementation for the SPI device
 *
 *  This file implements the IOM for the SPI device for DSP BIOS operating
 *  system
 *
 *  (C) Copyright 2008, Texas Instruments, Inc
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <string.h>
#include <assert.h>
#include <clk.h>
#include <sys.h>
#include <ecm.h>
#include <hwi.h>
#include <c64.h>
#include <iom.h>
#include <tsk.h>
#include "ti/pspiom/spi/Spi.h"
#include "ti/pspiom/gpio/Gpio.h"
#include "ti/pspiom/psc/Psc.h"
#include "SpiLocal.h"

#ifdef Spi_EDMA_ENABLE
    #include <ti/sdo/edma3/drv/edma3_drv.h>
#endif

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
    #define MAX_CHIPSELECT_SPI0     1
    #define MAX_CHIPSELECT_SPI1     1
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
    #define MAX_CHIPSELECT_SPI0     1
    #define MAX_CHIPSELECT_SPI1     1
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
    #define MAX_CHIPSELECT_SPI0     1
    #define MAX_CHIPSELECT_SPI1     1
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
    #define MAX_CHIPSELECT_SPI0     1
    #define MAX_CHIPSELECT_SPI1     1
#endif

const Spi_Params Spi_PARAMS = {
    TRUE,                           /* enablecache     */
    Spi_OpMode_POLLED,              /* opMode          */
    (Uint32)0x2dc6c0,               /* outputClkFreq   */
    0,                              /* loopbackEnabled */
    (Int32)SYS_FOREVER,             /* timeout         */
    {
        0,                          /* intrLevel       */
        Spi_CommMode_MASTER,        /* masterOrSlave   */
        1,                          /* clkInternal     */
        0,                          /* enableHighZ     */
        Spi_PinOpMode_SPISCS_4PIN,  /* pinOpModes      */
        {
            (Uint32)0x0,            /* c2TDelay        */
            (Uint32)0x0,            /* t2CDelay        */
            (Uint32)0x0,            /* t2EDelay        */
            (Uint32)0x0,            /* c2EDelay        */
        },                          /* delay           */
        (Uint32)0x0,                /* waitDelay       */
        (Uint32)0xff,               /* csDefault       */
        {
            {
                (Uint32)0x0,        /* wDelay          */
                (Uint32)0x0,        /* charLength      */
                0,                  /* lsbFirst        */
                0,                  /* phaseIn         */
                0,                  /* oddParity       */
                0,                  /* parityEnable    */
                0,                  /* clkHigh         */
                0,                  /* waitEnable      */
            }, /* [0] */
            {
                (Uint32)0x0,        /* wDelay          */
                (Uint32)0x0,        /* charLength      */
                0,                  /* lsbFirst        */
                0,                  /* phaseIn         */
                0,                  /* oddParity       */
                0,                  /* parityEnable    */
                0,                  /* clkHigh         */
                0,                  /* waitEnable      */
            },  /* [1] */
            {
                (Uint32)0x0,        /* wDelay          */
                (Uint32)0x0,        /* charLength      */
                0,                  /* lsbFirst        */
                0,                  /* phaseIn         */
                0,                  /* oddParity       */
                0,                  /* parityEnable    */
                0,                  /* clkHigh         */
                0,                  /* waitEnable      */
            },  /* [2] */
            {
                (Uint32)0x0,        /* wDelay          */
                (Uint32)0x0,        /* charLength      */
                0,                  /* lsbFirst        */
                0,                  /* phaseIn         */
                0,                  /* oddParity       */
                0,                  /* parityEnable    */
                0,                  /* clkHigh         */
                0,                  /* waitEnable      */
            },  /* [3] */
        },                          /* configDatafmt   */
    },                              /* spiHWCfgData    */
    ((Ptr)((void*)0x0)),            /* edmaHandle      */
    (Uint16)0x0,                    /* HWINumber       */
    FALSE,
    Spi_pllDomain_0
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */
/**
 *  \brief  Array which is part of Spi Module State
 */
static Bool inUse[CSL_SPI_PER_CNT];
/**
 *  \brief  Spi Module State Object
 */
static Spi_Module_State Spi_module = {&inUse[0]};
/**
 *  \brief  Array of Spi instance State objects array
 */
static Spi_Object Spi_Instances[CSL_SPI_PER_CNT];
/**
 *
 * \brief  structure Holding the information specific to an instance.
 */
Spi_HwInfo Spi_deviceInstInfo[CSL_SPI_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int spi_mdBindDev(Ptr *devp, Int devId, Ptr devParams);
static Int spi_mdUnBindDev(Ptr devp);
static Int spi_mdCreateChan(Ptr                 *chanp,
                            Ptr                 devp,
                            String              name,
                            Int                 mode,
                            Ptr                 chanParams,
                            IOM_TiomCallback    cbFxn,
                            Ptr                 cbArg);
static Void spiHandleErrors(Spi_Object  *instHandle,
                            Spi_ChanObj *chanHandle,
                            Uint32       intStatus);
static Int spi_mdDeleteChan(Ptr chanp);
static Int spi_mdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);
static Int spi_mdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg);

IOM_Fxns Spi_IOMFXNS =
{
    &spi_mdBindDev,
    &spi_mdUnBindDev,
    &spi_mdControlChan,
    &spi_mdCreateChan,
    &spi_mdDeleteChan,
    &spi_mdSubmitChan,
};

static Int spiTransfer(Spi_Object       *instHandle,
                       Spi_ChanObj      *chanHandle,
                       Spi_DataParam    *dataparam);

static Int32 spiIoctl(Ptr                     handle,
                     Spi_ioctlCmd             cmd,
                     Ptr                      cmdArg,
                     Ptr                      param);

static Void spiRegisterIntrHandler(Uint32      instNum,
                                   Uint32      intNum,
                                   Spi_isr     initIsr,
                                   Ptr         spiObj);

static Int32 spiSetupConfig(Spi_ChanObj *chanHandle);

static Void spiConfigureOpMode(const Spi_Object *instHandle);

Void spiIntrHandler(Spi_Object *instHandle);

static Void spiUnregisterIntrHandler(Uint32 instNum, Uint32 intNum);

static Bool spiCheckTimeOut(Uint32 startValue,Int32 timeout);

static Void spiCompleteIOInIsr (Spi_Object *instHandle);

static Int32 spiPolledModeTransfer(Spi_ChanObj *chanHandle);

static Void spiCancelAllIo(Spi_Object *instHandle,Spi_ChanObj *chanHandle);
#ifdef BIOS_PWRM_ENABLE
static Int32 SpiRegisterNotification(Spi_Object *instHandle);

static PWRM_NotifyResponse SpiNotifyCallback(PWRM_Event    eventType,
                                             Arg           eventArg1,
                                             Arg           eventArg2,
                                             Arg           clientArg);

static Int32 SpiCalculateConstraints(Spi_Object *instHandle);

static PWRM_NotifyResponse SpiSuspendCurrentIops(Spi_Object  *instHandle,
                                                 PWRM_Event   eventType);
                                                  
static PWRM_NotifyResponse SpiConfigureNewVfSettings(Spi_Object   *instHandle,
                                                     Uns           eventArg1,
                                                     Uns           eventArg2);

static Int32 SpiCalculateClockSettings(Spi_Object *instHandle,
                                       Uint32      setpoint,
                                       Uint32      opFreq,
                                       Uint32     *prescale);

static Int32 SpiConfigureNewClkSettings(Spi_Object *instHandle,
                                        Uint32      prescale);

static Int32 SpiUnregisterNotification(Spi_Object *instHandle);

static Int32 SpiUnregisterConstraints(Spi_Object *instHandle);
#endif
/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */


/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */

/**
 *  \brief  Function called by Bios during instance initialisation
 *
 */
Void Spi_init(Void)
{
    Int i;

    for (i = 0; i < CSL_SPI_PER_CNT; i++)
    {
        /* have to initialize statically                                      */
        Spi_module.inUse[i] = FALSE;
        memset((Void *)&Spi_Instances[i], 0x0, sizeof(Spi_Object));
    }
}

/**
 *  \brief  Function called by Bios during instance initialisation
 *
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID                 in case of error
 */
static Int spi_mdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Int32         status        = IOM_COMPLETED;
    Spi_ChanObj   *chanHandle   = NULL;
    Uint8         count         = 0;
    Spi_Params    *params       = NULL;
    Spi_Object    *instHandle   = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((CSL_SPI_PER_CNT <= devId) || (TRUE == Spi_module.inUse[devId]))
    {
        status = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */
    if (IOM_COMPLETED == status)
    {
        if (devParams == NULL)
        {
            params = (Spi_Params*)&Spi_PARAMS;
        }
        else
        {
            params = (Spi_Params*) devParams;
        }

        instHandle =  &Spi_Instances[devId];
        Spi_module.inUse[devId] = TRUE;

        if (devId == 0)
        {
            Spi_deviceInstInfo[devId].baseAddress =
                (CSL_SpiRegsOvly)CSL_SPI_0_REGS;
            Spi_deviceInstInfo[devId].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_SPIINT0;
            Spi_deviceInstInfo[devId].rxDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_SPI0_RX;
            Spi_deviceInstInfo[devId].txDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_SPI0_TX;
            Spi_deviceInstInfo[devId].inputFrequency =
                (Uint32)CSL_SPI_0_MODULE_FREQ;
            Spi_deviceInstInfo[devId].maxChipSelect =
                (Uint32)MAX_CHIPSELECT_SPI0;
#ifndef BIOS_PWRM_ENABLE
            Spi_deviceInstInfo[devId].pwrmLpscId = (Uint32)CSL_PSC_SPI0;
#else
            Spi_deviceInstInfo[devId].pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_SPI_0;
#endif
            Spi_deviceInstInfo[devId].pscInstance =
                (Uint32)CSL_LPSC_INST_SPI_0;
        }
#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) ||  defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        else if (devId == 1)
        {
            Spi_deviceInstInfo[devId].baseAddress =
                (CSL_SpiRegsOvly)CSL_SPI_1_REGS;
            Spi_deviceInstInfo[devId].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_SPIINT1;
            Spi_deviceInstInfo[devId].rxDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_SPI1_RX;
            Spi_deviceInstInfo[devId].txDmaEventNumber =
                (Uint32)CSL_EDMA3_CHA_SPI1_TX;
            Spi_deviceInstInfo[devId].inputFrequency =
                (Uint32)CSL_SPI_1_MODULE_FREQ;
            Spi_deviceInstInfo[devId].maxChipSelect =
                (Uint32)MAX_CHIPSELECT_SPI1;
#ifndef BIOS_PWRM_ENABLE
            Spi_deviceInstInfo[devId].pwrmLpscId = (Uint32)CSL_PSC_SPI1;
#else
            Spi_deviceInstInfo[devId].pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_SPI_1;
#endif
            Spi_deviceInstInfo[devId].pscInstance =
                (Uint32)CSL_LPSC_INST_SPI_1;
        }
#endif  /* defined(CHIP_C6747) || defined(CHIP_OMAPL137) */

        instHandle->instNum           = (Uint32)devId;
        instHandle->enableCache       = params->enableCache;
        instHandle->opMode            = params->opMode;
        instHandle->hEdma             = NULL;
        instHandle->hwiNumber         = params->hwiNumber;
        instHandle->spiHWconfig       = params->spiHWCfgData;
        instHandle->polledModeTimeout = params->polledModeTimeout;
        instHandle->pscPwrmEnable     = params->pscPwrmEnable;
        instHandle->numOpens          = 0;
        instHandle->devState = Spi_DriverState_CREATED;
        instHandle->csHighPolarity    = 0;
        instHandle->dmaChanAllocated  = 0;
        instHandle->edmaCbCheck       = 0;
        instHandle->currentActiveChannel = NULL;
#ifdef BIOS_PWRM_ENABLE
        memset(&instHandle->pwrmInfo,0x00,sizeof(Spi_pwrmInfo));
#endif
        /* initialize Statistics members                                      */
        instHandle->stats.rxBytes = 0;
        instHandle->stats.txBytes = 0;
        instHandle->stats.pendingPacket = 0;
        instHandle->stats.rxOverrunError = 0;
        instHandle->stats.timeoutError = 0;
        instHandle->stats.bitError = 0;
        instHandle->stats.parityError = 0;
        instHandle->stats.desyncError = 0;

        instHandle->deviceInfo.baseAddress =
            Spi_deviceInstInfo[devId].baseAddress;
        instHandle->deviceInfo.inputFrequency =
            Spi_deviceInstInfo[devId].inputFrequency;
        instHandle->deviceInfo.cpuEventNumber =
            Spi_deviceInstInfo[devId].cpuEventNumber;
        instHandle->deviceInfo.rxDmaEventNumber =
            Spi_deviceInstInfo[devId].rxDmaEventNumber;
        instHandle->deviceInfo.txDmaEventNumber =
            Spi_deviceInstInfo[devId].txDmaEventNumber;
        instHandle->deviceInfo.pscInstance =
            Spi_deviceInstInfo[devId].pscInstance;
        instHandle->deviceInfo.pwrmLpscId =
            Spi_deviceInstInfo[devId].pwrmLpscId;

        for (count = 0; count < Spi_NUM_CHANS; count++)
        {
            chanHandle                   = &instHandle->chanObj[count];
            chanHandle->cbFxn            = NULL;
            chanHandle->cbArg            = NULL;
            chanHandle->mode             = IOM_OUTPUT;
            chanHandle->instHandle       = NULL;
            chanHandle->channelState     = Spi_DriverState_CLOSED;
            chanHandle->busFreq          = params->outputClkFreq;
            chanHandle->loopbackEnabled  = params->loopbackEnabled;
            chanHandle->pendingState     = 0;
            chanHandle->abortAllIo       = 0;
            chanHandle->charLength16Bits = 0;
            chanHandle->currError        = 0;
            chanHandle->txBufFlag        = FALSE;
            chanHandle->rxBufFlag        = FALSE;
            chanHandle->txBufferLen      = 0;
            chanHandle->rxBufferLen      = 0;
            chanHandle->taskPriority     = Spi_MAX_CHAN_PRIORITY;

            /* intialise the transceive buffers                               */
            memset(chanHandle->txTransBuf,0x00,sizeof(chanHandle->txTransBuf));
            memset(chanHandle->rxTransBuf,0x00,sizeof(chanHandle->rxTransBuf));
        }

        *devp = (Ptr)instHandle;

        /* The semaphore needed for the syncronisation of the driver in the   *
         * POLLED mode will be initialised here                               */
        if (Spi_OpMode_POLLED == instHandle->opMode)
        {
            SEM_new(&(instHandle->syncSem),1);
        }

        /* power management not supported for slave mode                      */
        if ((TRUE == instHandle->pscPwrmEnable) &&
            (Spi_CommMode_SLAVE == instHandle->spiHWconfig.masterOrSlave))
        {
            status = IOM_EBADARGS;
        }

        if ((IOM_COMPLETED == status) && (FALSE == instHandle->pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            status = (Int32)PWRM_setDependency(
                        (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);

#else
            /* power on using PSC API                                         */
            status = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->deviceInfo.pscInstance,
                         (Uint32)instHandle->deviceInfo.pwrmLpscId,
                         TRUE);
#endif
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == status) && (TRUE == instHandle->pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            status = SpiRegisterNotification(instHandle);
        }
#endif
    }
    return (status);
}

/**
 *  \brief  Function called by Bios during closing of the instance
 *
 *
 *  \return None
 */
static Int spi_mdUnBindDev(Ptr devp)
{
    Int32        result     = IOM_COMPLETED;
    Spi_Object  *instHandle = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp) || (CSL_SPI_PER_CNT <= ((Spi_Object *)devp)->instNum))
    {
        result = IOM_EBADARGS;
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == result)
    {
        instHandle = (Spi_Object *)devp;

        /* set driver state to deleted                                        */
        instHandle->numOpens = 0;
        instHandle->devState = Spi_DriverState_DELETED;
        Spi_module.inUse[instHandle->instNum] = FALSE;

        if (FALSE == instHandle->pscPwrmEnable)
        {
#ifdef BIOS_PWRM_ENABLE        
            /* power off  using PWRM API                                      */
            result = (Int32)PWRM_releaseDependency(
                        (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
            /* power OFF using PSC API                                        */
            result = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->deviceInfo.pscInstance,
                         (Uint32)instHandle->deviceInfo.pwrmLpscId,
                         FALSE);
#endif
        }
#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == result) && (TRUE == instHandle->pscPwrmEnable))
        {
            /* unregister all the PWRM event notifications                    */
            result = SpiUnregisterNotification(instHandle);
        }
#endif
    }
    return (result);
}

/* ========================================================================== */
/*                              IOM FUNCTIONS                                 */
/* ========================================================================== */

/**
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the SPI device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a SIO_create call.
 *
 * \param     obj          [IN]     Spi driver object
 * \param     name         [IN]     Spi Instance name like Spi0
 * \param     mode         [IN]     channel  mode -> input or output
 * \param     chanParams   [IN]     channel parameters from user
 * \param     cbFxn        [IN]     callback function pointer
 * \param     cbArg        [IN]     callback function Arguments
 *
 * \return    channel handle in case of success
 *            NULL   in case of failure
 *
 */
static Int spi_mdCreateChan (Ptr              *chanp,
                             Ptr              devp,
                             String           name,
                             Int              mode,
                             Ptr              chanParams,
                             IOM_TiomCallback cbFxn,
                             Ptr              cbArg)
{
    Spi_Object          *instHandle  = NULL;
    Spi_ChanObj         *chanHandle  = NULL;
    Spi_ChanParams      *pChanParams = NULL;
    Uint32               key         = 0;
    Uint32               chanCount   = 0;
    Int32                status      = IOM_COMPLETED;
    Int32                retVal      = IOM_COMPLETED;
    TSK_Handle           thisTask    = NULL;
    Bool                 bWhileFalse = TRUE;
    Bool                 pscPwrOn    = FALSE;
    Bool                 isTask      = FALSE;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp) || (NULL == cbFxn) || (NULL == cbArg) ||
        (NULL == chanParams ))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif    
        instHandle = (Spi_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

        if (NULL == instHandle)
        {
            status = IOM_EBADARGS;
        }
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        /* To remove the compiler warnings                                    */
        if (NULL != name)
        {
            name = name;
        }

        do
        {
            /* power on the module                                            */
            status = Spi_localLpscOn(instHandle);

            if (IOM_COMPLETED != status)
            {
                /* Unable to power ON the module,hence break out              */
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }

            bWhileFalse = FALSE;
            for (chanCount = 0; chanCount < Spi_NUM_CHANS; chanCount++)
            {
                if (instHandle->chanObj[chanCount].instHandle == NULL)
                {
                    /* Assignment of channel                                  */
                    chanHandle = &instHandle->chanObj[chanCount];
                    break;
                }
            }

            /* Check if channel is NULL                                       */
            if (NULL == chanHandle)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* update the instance Handle pointer                             */
            chanHandle->instHandle = (Ptr)instHandle;

            /* update the channel mode,callback function and args             */
            chanHandle->mode = mode;
            chanHandle->cbFxn = cbFxn;
            chanHandle->cbArg = cbArg;

            isTask = TSK_isTSK();

            if(TRUE == isTask)
            {
                /* Returns a handle to the currently executing Task object    */
                thisTask = TSK_self();

                if(NULL != thisTask)
                {
                    /* Get task priority of this task                         */
                    chanHandle->taskPriority = TSK_getpri(thisTask);
                }
            }

            pChanParams = (Spi_ChanParams *)chanParams;
            instHandle->hEdma = pChanParams->hEdma;
            chanHandle->hGpio = pChanParams->hGpio;

            if (Spi_CommMode_SLAVE == instHandle->spiHWconfig.masterOrSlave)
            {
                if(FALSE == instHandle->isSlaveChannelOpened)
                {
                    instHandle->isSlaveChannelOpened = TRUE;
                }
                else
                {
                    status = IOM_EBADARGS;
                    break;
                }
            }

            /* Update the hardware if it is first open                        */
            if (0 == instHandle->numOpens)
            {
                key = (Uint32)_disable_interrupts();

                if (FALSE == instHandle->spiHWconfig.intrLevel)
                {
                    (instHandle->deviceInfo.baseAddress)->SPILVL = 0x0000;
                }
                else
                {
                    (instHandle->deviceInfo.baseAddress)->SPILVL = 0x03FFu;
                }

                /* If interrupt mode, register ISR                            */
                if (Spi_OpMode_POLLED != instHandle->opMode)
                {
                    spiRegisterIntrHandler(
                        instHandle->instNum,
                        instHandle->deviceInfo.cpuEventNumber,
                        (Spi_isr)&spiIntrHandler,
                        instHandle);
                }

#ifdef Spi_EDMA_ENABLE
                status = Spi_localEdmaChannel_Request(instHandle);
#endif
#ifdef BIOS_PWRM_ENABLE
                if ((IOM_COMPLETED == status) &&
                    (TRUE == instHandle->pscPwrmEnable))
                {
                    status = SpiCalculateConstraints(instHandle);
                }
#endif
                if (IOM_COMPLETED == status)
                {
                    /* Initialize hardware                                    */
                    status = spiSetupConfig(chanHandle);
                }

                /* restore the interrupts                                     */
                _restore_interrupts(key);
            }

            QUE_new(&(chanHandle->queuePendingList));

        }while(TRUE == bWhileFalse);
    }

    if (TRUE == pscPwrOn)
    {
        /* power off the module                                               */
        retVal = Spi_localLpscOff(instHandle);

        if (IOM_COMPLETED == status)
        {
            status = retVal;
        }
    }

    if (IOM_COMPLETED == status)
    {
        /* Increment open count and return driver handle                      */
        ++instHandle->numOpens;
        chanHandle->channelState = Spi_DriverState_OPENED;
        *chanp = (Ptr)chanHandle;
    }
    else
    {
        /* channel opening failed                                            */
        *chanp = NULL;
    }

    return (status);
}

/**
 *  \brief    This function is called by the application to close a previously
 *            opened channel.it deletes the channel so that it is not available
 *            for further transactions. All the allocated reqources are freed &
 *            the channel will be ready for the "open" operation once again.
 *
 *  \param    instHandle [IN]   Spi driver structure
 *            chanp      [IN]   Handle to the channel.
 *            eb         [OUT]  pointer to the error information block.
 *
 *  \return   None
 */
static Int spi_mdDeleteChan(Ptr chanp)
{
    Spi_Object         *instHandle;
    Spi_ChanObj        *chanHandle = NULL;
    Uint32             key     = 0;
    Int32              status     = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        status = IOM_EBADARGS;
    }
    else if((NULL == ((Spi_ChanObj*)chanp)->instHandle) ||
            (Spi_DriverState_OPENED != ((Spi_ChanObj*)chanp)->channelState))
    {
        status = IOM_EBADARGS;
    }
    else
    {

#endif
        chanHandle = (Spi_ChanObj *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (NULL != chanHandle)
        {
#endif
            instHandle = chanHandle->instHandle;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if (NULL == instHandle)
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
#endif
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        /*  Check for any IO is pending, if yes then cancel io                */
        if (TRUE == chanHandle->pendingState)
        {
            spiIoctl(chanHandle,Spi_IOCTL_CANCEL_PENDING_IO,NULL,NULL);
        }

        /* disable the global interrupts                                      */
        key = (Uint32)_disable_interrupts();

        /* decrement the number of open channels as we are closing the        *
         * channel                                                            */
        --instHandle->numOpens;

        /* enable the global interrupts                                       */
        _restore_interrupts(key);

        if (0 == instHandle->numOpens)
        {
            /* all the channels are closed hence unregister the interrupts    */
            if (instHandle->opMode != Spi_OpMode_POLLED)
            {
                spiUnregisterIntrHandler(
                    instHandle->instNum,
                    instHandle->deviceInfo.cpuEventNumber);
            }

#ifdef Spi_EDMA_ENABLE
            /* Free the Edma Channels  if mode is DMA                         */
            if ((Spi_OpMode_DMAINTERRUPT== instHandle->opMode)
                && (TRUE == instHandle->dmaChanAllocated))
            {
                EDMA3_DRV_freeChannel(
                    instHandle->hEdma,
                    instHandle->deviceInfo.rxDmaEventNumber);

                EDMA3_DRV_freeChannel(
                    instHandle->hEdma,
                    instHandle->deviceInfo.txDmaEventNumber);
            }
            else
            {
                /* Dma channel is not allocated,return error                  */
                status = IOM_EBADARGS;
            }
#endif  /* Spi_EDMA_ENABLE */
            /* Disable the spi enable pin                                     */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~(CSL_SPI_SPIGCR1_ENABLE_MASK));

            /* Put the device in reset mode and quit                          */
            (instHandle->deviceInfo.baseAddress)->SPIGCR0 =
                 CSL_SPI_SPIGCR0_RESETVAL;

#ifdef BIOS_PWRM_ENABLE
            if (TRUE == instHandle->pscPwrmEnable)
            {
                status = SpiUnregisterConstraints(instHandle);
            }
#endif
        }

        chanHandle->instHandle = NULL;

        /* Reset the state variables                                          */
        chanHandle->cbFxn            = NULL;
        chanHandle->cbArg            = NULL;
        chanHandle->mode             = IOM_OUTPUT;
        chanHandle->pendingState     = 0;
        chanHandle->abortAllIo       = 0;
        chanHandle->charLength16Bits = 0;
        chanHandle->currError        = 0;
        chanHandle->txBufFlag        = FALSE;
        chanHandle->rxBufFlag        = FALSE;
        chanHandle->txBufferLen      = 0;
        chanHandle->rxBufferLen      = 0;
        
        /* reset the channel priority                                         */
        chanHandle->taskPriority = Spi_MAX_CHAN_PRIORITY;

        /* Updated the driver state                                           */
        chanHandle->channelState = Spi_DriverState_CLOSED;
    }

    return (status);
}


/**
 *  \brief    Submit a I/O packet to a channel for processing
 *
 *   The GIO layer calls this function to cause the mini-driver
 *   to process the IOM_Packet for read/write operations.
 *
 *  \param   instHandle [IN]  Spi driver structure pointer
 *  \param   chanp      [IN]  Handle to the channel
 *  \param   ioPacket   [IN]  Pointer to packet to be submitted
 *  \Param   eb         [OUT] error block
 *
 *  \return  IOM_COMPLETED, if packet is fully processed
 *           IOM_PENDING,   if packet is not fully processed
 *           IOM_EBADIO      if error in processing
 */
static Int spi_mdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Spi_Object         *instHandle    = NULL;
    Spi_ChanObj        *chanHandle    = NULL;
    Spi_DataParam      *dataparam     = NULL;
    Bool                bWhileFalse   = TRUE;
    Uint32              hwiKey        = 0x00;
    Bool                pscPwrOn      = FALSE;
    Int32               status        = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == chanp) || (NULL == ioPacket))
    {
        status = IOM_EBADARGS;
    }
    else if ((NULL == ioPacket->addr) &&
       ((IOM_ABORT != ioPacket->cmd) && (IOM_FLUSH != ioPacket->cmd)))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif
        chanHandle = (Spi_ChanObj *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (NULL != chanHandle)
        {
#endif
            instHandle = chanHandle->instHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK

            if (NULL == instHandle)
            {
                status = IOM_EBADARGS;
            }
        }

        if ((IOM_ABORT != ioPacket->cmd) && (IOM_FLUSH != ioPacket->cmd))
        {
            dataparam = (Spi_DataParam *)ioPacket->addr;

            if (NULL == dataparam)
            {
                status = IOM_EBADARGS;
            }
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if(IOM_COMPLETED == status)
    {
        do
        {
            bWhileFalse = FALSE;

            if ((ioPacket->cmd == IOM_ABORT) || (ioPacket->cmd == IOM_FLUSH))
            {
                /* This command not handled in the POLLED mode                */
                if (Spi_OpMode_POLLED != instHandle->opMode)
                {
                    /* enter critical section                                 */
                    hwiKey = _disable_interrupts();

                    chanHandle->abortAllIo = TRUE;
                    spiCancelAllIo(instHandle,chanHandle);

                    /* exit crtical section                                   */
                    _restore_interrupts(hwiKey);
                }
                break;
            }

            /* Validate the packet send by the user                           */
            if ((ioPacket->cmd == IOM_READ) || (ioPacket->cmd == IOM_WRITE))
            {
                if ((ioPacket->addr == NULL) || (ioPacket->size <= 0))
                {
                    status = IOM_EBADIO;
                    break;
                }
            }

            dataparam = (Spi_DataParam *)ioPacket->addr;

            /* Validating State of the channel                                */
            if (Spi_DriverState_OPENED  !=  chanHandle->channelState)
            {
                status = IOM_EBADIO;
                break;
            }

            if (NULL == chanHandle->instHandle)
            {
                status = IOM_EBADIO;
                break;
            }

            if ((NULL == dataparam->outBuffer) && (NULL == dataparam->inBuffer))
            {
                status = IOM_EBADIO;
                break;
            }

            if (dataparam->bufLen == 0)
            {
                status = IOM_EBADIO;
                break;
            }

            /* GPIO CS is supported only with CSHOLD */
            if ((Spi_CSHOLD != (dataparam->flags & Spi_CSHOLD))
                && (Spi_GPIO_CS == (dataparam->flags & Spi_GPIO_CS)))
            {
                status = IOM_EBADARGS;
                break;
            }

            /* If GPIO CS is used then do not allow SPICS                     */
            if((Spi_GPIO_CS == (dataparam->flags & Spi_GPIO_CS))
               && (0 != dataparam->chipSelect))
            {
                status  = IOM_EBADARGS;
                break;
            }

#ifdef Spi_EDMA_ENABLE
            /* if the transfer is in DMA mode                                 */
            /* Validate the buffer alignment if mode is DMA                   */
            if (instHandle->opMode == Spi_OpMode_DMAINTERRUPT)
            {
                if ((0 != (((Uint32)dataparam->outBuffer) % 32u))
                    || (0 != (((Uint32)dataparam->inBuffer) % 32u)))
                {
                    status = IOM_EBADARGS;
                    break;
                }
            }
#endif  /* Spi_EDMA_ENABLE */

            /* check for zero '0' timeout                                     */
            if (0 == instHandle->polledModeTimeout)
            {
                status = IOM_EBADIO;
                break;
            }

            /* call the power ON function to switch on the module in PSC      */
            status = Spi_localLpscOn(instHandle);

            if (IOM_COMPLETED != status)
            {
                /* unable to switch ON the Module  hence break out from here  */
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }

            /* protect the "currentActiveChannel"                             */
            hwiKey = _disable_interrupts();

            /* check whether to process the packet or queue it up             */
            if ((NULL == instHandle->currentActiveChannel) &&
                (Spi_DriverState_PWRM_SUSPEND != instHandle->devState))
            {
                instHandle->currentActiveChannel = chanHandle;

                /* critical section ends                                      */
                _restore_interrupts(hwiKey);

                /* There is no current active packet in the driver hence we   *
                 * can load this request directly                             */

                /* Check if the inBuffer or the outBuffer is NULL i.e,not     *
                 * supplied and update respective buffer appropriately with   *
                 * driver buffer                                              */
                if (NULL == dataparam->inBuffer)
                {
                    /* user has not suppiled the IN buffer                    */
                    chanHandle->rxBufFlag = FALSE;

                    /* align the buffer in case of edma mode only             */
                    if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
                    {
                        chanHandle->rxBuffer =
                            (Uint8 *)((((Uint32)&chanHandle->rxTransBuf[0]) +
                                     (0x1F)) & 0xFFFFFFE0u);
                    }
                    else
                    {
                        /* No alignment required here                         */
                        chanHandle->rxBuffer = &chanHandle->rxTransBuf[0];
                    }

                    /* update the details of the out buffer here as it cannot *
                     * be NULL (condition already checked)                    */
                    chanHandle->txBufFlag = TRUE;
                    chanHandle->txBuffer  = dataparam->outBuffer;
                    chanHandle->rxBufferLen = dataparam->bufLen;
                    chanHandle->txBufferLen = dataparam->bufLen;
                }
                else if (NULL == dataparam->outBuffer)
                {
                    /* user has not suppiled the IN buffer                    */
                    chanHandle->txBufFlag = FALSE;

                    /* align the buffer in case of edma mode only             */
                    if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
                    {
                        chanHandle->txBuffer =
                            (Uint8 *)((((Uint32)&chanHandle->txTransBuf[0]) +
                                     (0x1F)) & 0xFFFFFFE0u);
                    }
                    else
                    {
                        /* No alignment required here                         */
                        chanHandle->txBuffer = &chanHandle->txTransBuf[0];
                    }

                    /* update the details of the "inbuffer" here as it cannot *
                     * be NULL (condition already checked)                    */
                    chanHandle->rxBufFlag = TRUE;
                    chanHandle->rxBuffer  = dataparam->inBuffer;
                    chanHandle->rxBufferLen = dataparam->bufLen;
                    chanHandle->txBufferLen = dataparam->bufLen;

                }
                else
                {
                    /* Both the buffers are valid                             */
                    chanHandle->rxBufFlag = TRUE;
                    chanHandle->txBufFlag = TRUE;
                    chanHandle->rxBuffer  = dataparam->inBuffer;
                    chanHandle->txBuffer  = dataparam->outBuffer;
                    chanHandle->rxBufferLen = dataparam->bufLen;
                    chanHandle->txBufferLen = dataparam->bufLen;
                }

                /* No transfer is happening in any of the channels in this    *
                 * instance of the driver.So for sure we can initiate the     *
                 * transfer here itself                                       */
                chanHandle->activeIOP = ioPacket;

                /* Call the Spi_transfer data transfer through hardware       */
                status =  spiTransfer(instHandle,chanHandle,dataparam);

            }
            else
            {
                /* end critical section                                       */
                _restore_interrupts(hwiKey);

                /* that means some IO from some channel is in progress we may *
                 * need to queue the request in pending queue for this channel*
                 * one the control comes to the completion of the current IO, *
                 * the queue will be processed and this IO will be programmed *
                 * In the priority based implementation, please note that only*
                 * the channel that was created from the task with high       *
                 * priority would be processed first                          */
                if (Spi_OpMode_POLLED != chanHandle->channelState)
                {
                     QUE_put(
                        &(chanHandle->queuePendingList),
                        (Ptr)ioPacket);
                        instHandle->stats.pendingPacket++;
    
                     status = IOM_PENDING;
                }
                else
                {
                    /* polled mode does not support queuing of packets        */
                    status = IOM_EBADMODE;
                }
            }
        }while(TRUE == bWhileFalse);
    }

    if (IOM_COMPLETED == status)
    {
        /* if it is polled mode of operation we will power off the module     *
         * after every IOP.Hence power off the module                         */
        if ((Spi_OpMode_POLLED == instHandle->opMode) && (TRUE == pscPwrOn))
        {
            status = Spi_localLpscOff(instHandle);
        }
    }
    else if (IOM_PENDING == status)
    {
        status = IOM_PENDING;
    }
    else
    {
        if (TRUE == pscPwrOn)
        {
            /* switch of the module only if it is already switched on.Dont    *
             * update the status here as already the submit has failed and    *
             * consequently the error status is already set                   */
            Spi_localLpscOff(instHandle);
        }
        status = IOM_EBADIO;
    }
    return status;
}

/**
 *  \brief   This function executes a control command passed by the application
 *
 *   The application's request for a IOCTL to be executed is routed here by the
 *   stream. if the command is supported then the control command is executed.
 *
 *  \param    instHandle [IN]    Spi driver structure
 *  \param    chanp      [IN]    Channel handle
 *  \param    cmd        [IN]    control command given by the application
 *  \param    cmdArgs    [IN]    Optional args required for command execution
 *  \param    eb         [OUT]   error block
 *
 *  \return   None
 */
static Int spi_mdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg)
{
    Spi_ChanObj   *chanHandle  = NULL;
    Int32          status      = IOM_COMPLETED;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        /* invalid params have been detected                                  */
        status = IOM_EBADARGS;
    }
    else
    {
#endif
        chanHandle = (Spi_ChanObj*)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK       
        if (NULL != chanHandle)
        {
            if(Spi_DriverState_OPENED != chanHandle->channelState)
            {
                status = IOM_EBADARGS;
            }
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == status)
    {
        /* call the function to execute the control commands              */
        status = spiIoctl(chanHandle, (Spi_ioctlCmd)cmd,(Ptr) cmdArg, NULL);
    }

    return (status);
}

/* ========================================================================== */
/*                            LOCAL  FUNCTIONS                                */
/* ========================================================================== */

/**
 *  \brief  This to configure the data control bit of SPI data register SPIDAT1.
 *         All four params (CSHold, waitdelay, chip-select, Data format) will
 *         be uploaded to SPIDAT1 before transfer starts.
 *
 *  \param  handle      [IN]   SPI driver object for respective instance.
 *  \param  chipSelect  [IN]   Chip select number
 *  \param  dataFormat  [IN]   Data format register selection
 *  \param  flags       [IN]   Flag for Read/Write of data
 *
 *  \return    Param uploaded value if success
 *            Error code
 */
Uint32 Spi_localControlDataConfig(Spi_ChanObj      *chanHandle,
                                  Uint32            chipSelect,
                                  Spi_DataFormat    dataFormat,
                                  Uint32            gpioPin,
                                  Uint32            flags)
{
    Uint32          spiData1   = 0;
    Uint32          csHold     = FALSE;
    Spi_Object     *instHandle = NULL;
    Gpio_PinCmdArg  userPinCmdArg;

    assert(NULL != chanHandle);

    instHandle = chanHandle->instHandle;
    assert(NULL != instHandle);

    /* only data format0 is supported for the slave mode                      */
    if (Spi_CommMode_SLAVE == instHandle->spiHWconfig.masterOrSlave)
    {
        assert(Spi_DataFormat_0 == dataFormat);
    }

    if (Spi_CSHOLD == (flags & Spi_CSHOLD))
    {
        csHold = TRUE;
    }

    if ((Spi_PinOpMode_SPISCS_4PIN == instHandle->spiHWconfig.pinOpModes)
        || (Spi_PinOpMode_5PIN == instHandle->spiHWconfig.pinOpModes))
    {
        /*Use Chipselect only in case of 4 pin or 5 pin modes                 */
        /*Check if a GPIO is used as a cip select                             */

        if ((Spi_GPIO_CS != (flags & Spi_GPIO_CS)))
        {
            /* Check which pins will be used for SPI chipselect or which function
               will be used as GPIO and set the bit appripriately*/
            if(CSL_SPI_SPIPC0_SCS0FUN0_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN0_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN0_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN0_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN1_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN1_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN1_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN1_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN2_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN2_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN2_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN2_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN3_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN3_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN3_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN3_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN4_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN4_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN4_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN4_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN5_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN5_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN5_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN5_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN6_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN6_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN6_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN6_MASK);
            }

            if(CSL_SPI_SPIPC0_SCS0FUN7_MASK ==
                (CSL_SPI_SPIPC0_SCS0FUN7_MASK & chipSelect))
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 |=
                                                    CSL_SPI_SPIPC0_SCS0FUN7_MASK;
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIPC0 &=
                                                (~CSL_SPI_SPIPC0_SCS0FUN7_MASK);
            }

        }

        /* Enable CS Hold only for 4 pin cs and 5 pin mode                    */
        spiData1 |= (csHold << CSL_SPI_SPIDAT1_CSHOLD_SHIFT);

        if (0 != (chipSelect & instHandle->spiHWconfig.csDefault))
        {
            /* Here we are because default chip select value (during inactive)*
             * is HIGH that means it is active low                            */

           /* Set all CS to default state                                     */
            spiData1 |= (instHandle->spiHWconfig.csDefault <<
                            CSL_SPI_SPIDAT1_CSNR_SHIFT);

            if ((Spi_GPIO_CS != (flags & Spi_GPIO_CS)))
            {
                /* Set the selected CS line                                   */
                spiData1 &= (~(chipSelect << CSL_SPI_SPIDAT1_CSNR_SHIFT));
            }
            else
            {
                /* Deactivate the chip select until the actual transfer is    *
                 * about to start - Here we are for CS active low, so keep it *
                 * high                                                       */
                userPinCmdArg.pin   = gpioPin;
                userPinCmdArg.value = Spi_HIGH;
                Gpio_setPinVal(chanHandle->hGpio,&userPinCmdArg);
            }
        }
        else
        {
            /* Set all CS to default state                                     */
            spiData1 |=  (instHandle->spiHWconfig.csDefault <<
                            CSL_SPI_SPIDAT1_CSNR_SHIFT);

            if ((Spi_GPIO_CS != (flags & Spi_GPIO_CS)))
            {
                /* Set the selected chip select value                         */
                spiData1 |= ((chipSelect) << CSL_SPI_SPIDAT1_CSNR_SHIFT);

            }
            else
            {
                /* Deactivate the chip select until the actual transfer is about
                   to start - Here we are for CS active high, so keep it low
                 */
                userPinCmdArg.pin   = gpioPin;
                userPinCmdArg.value = Spi_LOW;
                Gpio_setPinVal(chanHandle->hGpio,&userPinCmdArg);
            }
        }
    }
    else
    {
        /* Disable the CSHOLD for 3 pin mode                                  */
        spiData1 &= ~(CSL_SPI_SPIDAT1_CSHOLD_MASK);
    }

    /* configure the delay for the transaction                                */
    spiData1 |= (instHandle->spiHWconfig.waitDelay
                    << CSL_SPI_SPIDAT1_WDEL_SHIFT);

    /* program the data format to be used                                     */
    spiData1 |= (((Uint32)dataFormat)<< ((Uint32)CSL_SPI_SPIDAT1_DFSEL_SHIFT));

    /* To make transaction is in progress                                     */
    chanHandle->pendingState = TRUE;

    return spiData1;
}

/**
 *  \brief  Function call for spi data transfer. It transfer data as transcieve
 *          operation.Uses a local driver buffer is the user has not supplied
 *          the buffer.
 *
 *  \param  instHandle  [IN]    SPI Driver handle
 *  \param  chanHandle  [IN]    SPI channel handle
 *  \param  dataparam   [IN]    User data parameters for data Source.
 *
 *  \return            IOM_COMPLETED if success
 *                    Suitable error code
 */
static Int spiTransfer(Spi_Object       *instHandle,
                       Spi_ChanObj      *chanHandle,
                       Spi_DataParam    *dataparam)
{
    Uint32           spidat1     = 0;
    volatile Uint16 *spiDat1Cfg  = NULL;
#ifdef Spi_EDMA_ENABLE
    Int32            status      = IOM_COMPLETED;
#endif    
    Int32            retVal      = IOM_COMPLETED;

    assert((NULL != instHandle) && (NULL != chanHandle) && (NULL != dataparam));

    /* update the channel variables                                           */
    chanHandle->currError     = 0;

    if (instHandle->    \
        spiHWconfig.configDatafmt[dataparam->dataFormat].charLength > 8u)
    {
        chanHandle->charLength16Bits = TRUE;
    }

    /*======================== POLLED MODE====================================*/
    if (Spi_OpMode_POLLED == instHandle->opMode)
    {
        if (TRUE == (SEM_pend(&(instHandle->syncSem),
                         instHandle->polledModeTimeout))) 
        {
            /* Process only if no active IOP is present and the driver is not *
             * in a PWRM suspend state                                        */
            retVal = spiPolledModeTransfer(chanHandle);

#ifdef BIOS_PWRM_ENABLE
            if ((TRUE == instHandle->pscPwrmEnable) && 
                (TRUE == instHandle->pwrmInfo.ioSuspend))
            {
                /* set the current active channel as NULL                     */
                instHandle->currentActiveChannel = NULL;
                
                instHandle->pwrmInfo.ioSuspend = FALSE;
                instHandle->devState = Spi_DriverState_PWRM_SUSPEND;

                /* call the delayed completion function                       */
                (instHandle->pwrmInfo.delayedCompletionFxn  \
                    [instHandle->pwrmInfo.pwrmEvent])();
            }
#endif
            SEM_post(&(instHandle->syncSem));
        }
        else
        {
            retVal = IOM_ETIMEOUT;
        }
    }

    /*======================== INTERRUPT MODE=================================*/
    if (Spi_OpMode_INTERRUPT == instHandle->opMode)
    {
        /* Populate the user sent params to spi object structure              *
         * So device can use all the information when running at  ISR.        */

        /* Consume any stale data                                             */
        while (CSL_SPI_SPIBUF_RXEMPTY_MASK
            != ((instHandle->deviceInfo.baseAddress)->SPIBUF
                & CSL_SPI_SPIBUF_RXEMPTY_MASK))
        {
            (*chanHandle->rxBuffer) =
               (Uint8)(instHandle->deviceInfo.baseAddress)->SPIBUF;
            /* Dont increment the rx buffers as it is just stale data         */
        }

        if (dataparam->bufLen > 0)
        {
            /* load the new configuration values to the SPIDAT1 register      */
            spidat1 = Spi_localControlDataConfig(
                           chanHandle,
                           dataparam->chipSelect,
                           dataparam->dataFormat,
                           dataparam->gpioPinNum,
                           dataparam->flags);

            spiDat1Cfg = (((volatile Uint16 *)
                         (&(instHandle->deviceInfo.baseAddress)->SPIDAT1))+1);

            if (NULL != spiDat1Cfg)
            {
                /* write the upper 16 bits which contain the configurations to*
                 *the SPIDAT1 upper 16bits                                    */
                *spiDat1Cfg = (Uint16)(spidat1 >> 16);
            }

            /* enable all the spi transfers                                   */
            instHandle->deviceInfo.baseAddress->SPIGCR1 |=
                (CSL_SPI_SPIGCR1_ENABLE_MASK);

            /* Enabling receive and error interrupts                          */
            (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
                 (CSL_SPI_SPIINT0_BITERRENA_MASK
                 | CSL_SPI_SPIINT0_TIMEOUTENA_MASK
                 | CSL_SPI_SPIINT0_OVRNINTENA_MASK
                 | CSL_SPI_SPIINT0_DESYNCENA_MASK
                 | CSL_SPI_SPIINT0_PARERRENA_MASK
                 | CSL_SPI_SPIINT0_RXINTENA_MASK
                 | CSL_SPI_SPIINT0_TXINTENA_MASK);
         }/**< if currBuffLen                                                  */
         retVal = IOM_PENDING;
     }

#ifdef Spi_EDMA_ENABLE
    /*======================== DMA INTERRUPT MODE=============================*/
    if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        /* DMA Mode of data transfer                                          */
        /* Mask all interrupts,& populate data to spi object structure        *
         * so driver can extact all information at dma configSpiion           *
         * and dma Rx & Tx callback                                           */

        /* remove any stale data from the RX side                             */
        while (CSL_SPI_SPIBUF_RXEMPTY_MASK !=
            ((instHandle->deviceInfo.baseAddress)->SPIBUF
                & CSL_SPI_SPIBUF_RXEMPTY_MASK ))
        {
            /* Read before Write to remove any stale data                     */
            (*(chanHandle->rxBuffer)) =
                (Uint8)((instHandle->deviceInfo.baseAddress)->SPIBUF);

            /* dont increment the buffer as this is just reading stale data   */
        }

        /* Mask all the interrupts                                            */
        (instHandle->deviceInfo.baseAddress)->SPIINT0 =
            (instHandle->deviceInfo.baseAddress)->SPIINT0
            & (~(Spi_INTERRUPT_MASK));

        /* Call this to complete edma configuration and transfer              */
        status = Spi_localEdmaTransfer(instHandle,dataparam);

        if (IOM_COMPLETED ==  status)
        {
            retVal = IOM_PENDING;
        }
    }
#endif/* EDMA MODE CODE ENDS                                                 */
    return retVal;
}

/**
 *  \brief  function is used to transfer data in polled mode
 *
 *  This function is used to transfer data in polled mode.
 *
 *  \param  chanHandle  [IN]    Channel handle
 *
 *  \return IOM_COMPLETED in case of success
 *          IOM_ERROR in case of failure
 *
 *  \enter  chanHandle  must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 spiPolledModeTransfer(Spi_ChanObj *chanHandle)
{
    Uint32             timeCnt    = 0;
    Uint32             ioCount    = 0;
    Uint32             retStatus  = 0;
    Uint32             spidat1    = 0;
    Spi_Object        *instHandle = NULL;
    Spi_DataParam     *dataParam  = NULL;
    volatile Uint8    *spiDat1    = NULL;
    Int32              status     = IOM_COMPLETED;

    assert(NULL != chanHandle);
    instHandle = chanHandle->instHandle;
    assert(NULL != instHandle);

    /* Polled mode of data transfer Mask all interrupts                       */
    (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
        (~(Spi_INTERRUPT_MASK));

    /* To get current ticks to find out the data transfer timeout             */
    timeCnt = CLK_gethtime();

    dataParam  = (Spi_DataParam *)chanHandle->activeIOP->addr;
    assert(NULL != dataParam);

    spidat1 = Spi_localControlDataConfig(
                    chanHandle,
                    dataParam->chipSelect,
                    dataParam->dataFormat,
                    dataParam->gpioPinNum,
                    dataParam->flags);

    /* enable all the spi transfers                                           */
    instHandle->deviceInfo.baseAddress->SPIGCR1 |=
        (CSL_SPI_SPIGCR1_ENABLE_MASK);

    /* Read if any stale data is present                                      */
    while (CSL_SPI_SPIBUF_RXEMPTY_MASK
        != ((instHandle->deviceInfo.baseAddress)->SPIBUF
                & CSL_SPI_SPIBUF_RXEMPTY_MASK))
    {
        /* Read before Write to remove any stale data                         */
        (*chanHandle->rxBuffer) =
            (Uint8)((instHandle->deviceInfo.baseAddress)->SPIBUF);

        /* dont increment the buffer as this is stale data                    */
    }

    /* Activate CS here -  Irrestpective of CSHOLD is set or not,             *
     * because CSHOLD is only required for deactivating the CS                */
    Spi_localGpioPinToggle(chanHandle,Spi_LOW);

    /* Provide delay between CS active and actual IO as desired by the used   */
    if(Spi_GPIO_CS == (dataParam->flags & Spi_GPIO_CS))
    {
        Spi_localgenericDelay((Int32)dataParam->csToTxDelay);
    }

    /* use the length from the IOP as both the TX and RX operations use the   *
     * same buffer count                                                      */
    while (ioCount < dataParam->bufLen)
    {
        if ((FALSE == spiCheckTimeOut(timeCnt,instHandle->polledModeTimeout)))
        {
            /*  Transmit data pend on TX_FULL flag i.e                        *
             *  TX_FULL == TRUE  i.e  data is already in transmit             *
             *  buffer. TX_FULL == FALSE i.e. No data in transmit             *
             *  buffer, write next data.                                      */
            if (FALSE == ((instHandle->deviceInfo.baseAddress)->SPIBUF
                    & CSL_SPI_SPIBUF_TXFULL_MASK))
            {
                if (instHandle->spiHWconfig.    \
                    configDatafmt[dataParam->dataFormat].charLength <= 8u)
                {
                    (instHandle->deviceInfo.baseAddress)->SPIDAT1 =
                        spidat1 | (*chanHandle->txBuffer);

                    if (TRUE == chanHandle->txBufFlag)
                    {
                        /* increment the buffer                               */
                        chanHandle->txBuffer++;
                    }
                }
                else
                {
                    (instHandle->deviceInfo.baseAddress)->SPIDAT1 =
                        spidat1 | (*((Uint16 *)chanHandle->txBuffer));

                    if (TRUE == chanHandle->txBufFlag)
                    {
                        /* increment the buffer                               */
                        chanHandle->txBuffer += 2;
                    }
                }

                /* decrement the tx word count to be transferred              */
                chanHandle->txBufferLen--;
            }

            /* Read the data until the RX EMPTY flag is reset                 */
            while (CSL_SPI_SPIBUF_RXEMPTY_MASK
                == ((instHandle->deviceInfo.baseAddress)->SPIBUF
                    & CSL_SPI_SPIBUF_RXEMPTY_MASK))
            {
                /* check if the timeout has occured waiting for the rx data   */
                if (TRUE == spiCheckTimeOut(timeCnt,
                                instHandle->polledModeTimeout))
                {
                    status = IOM_ETIMEOUT;
                    instHandle->stats.timeoutError++;
                    break;
                }
            }

            if (instHandle->spiHWconfig.    \
                configDatafmt[dataParam->dataFormat].charLength <= 8u)
            {
                *chanHandle->rxBuffer =
                    (Uint8)(instHandle->deviceInfo.baseAddress)->SPIBUF;

                if (TRUE == chanHandle->rxBufFlag)
                {
                    /* increment the buffer count now                         */
                    chanHandle->rxBuffer++;
                }
            }
            else
            {
                (*((Uint16*)chanHandle->rxBuffer)) =
                    (Uint16)((instHandle->deviceInfo.baseAddress)->SPIBUF);

                if (TRUE == chanHandle->rxBufFlag)
                {
                    /* increment the buffer count now                         */
                    chanHandle->rxBuffer += 2;
                }
            }
            chanHandle->rxBufferLen--;

            /* Check for bit error, desync error,parity error,timeout         *
             * error and receive overflow errors                              */
            retStatus = (instHandle->deviceInfo.baseAddress)->SPIFLG;

            if (0 != (retStatus & (CSL_SPI_SPIFLG_OVRNINTFLG_MASK |
                             CSL_SPI_SPIFLG_PARERRFLG_MASK |
                             CSL_SPI_SPIFLG_TIMEOUTFLG_MASK |
                             CSL_SPI_SPIFLG_BITERRFLG_MASK |
                             CSL_SPI_SPIFLG_DESYNCFLG_MASK)))
            {
                /* Some error has occured check and update the stats values   */
                if (CSL_SPI_SPIFLG_BITERRFLG_MASK
                    ==  (retStatus & CSL_SPI_SPIFLG_BITERRFLG_MASK))
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                        CSL_SPI_SPIFLG_BITERRFLG_MASK;
                    status = Spi_BIT_ERR;
                    instHandle->stats.bitError++;
                }

                if (CSL_SPI_SPIFLG_OVRNINTFLG_MASK
                    == (retStatus & CSL_SPI_SPIFLG_OVRNINTFLG_MASK))
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                        CSL_SPI_SPIFLG_OVRNINTFLG_MASK;

                    status = Spi_RECEIVE_OVERRUN_ERR;
                    instHandle->stats.rxOverrunError++;
                }

                if (CSL_SPI_SPIFLG_PARERRFLG_MASK
                    == (retStatus & CSL_SPI_SPIFLG_PARERRFLG_MASK))
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                        CSL_SPI_SPIFLG_PARERRFLG_MASK;
                    status = Spi_PARITY_ERR;
                    instHandle->stats.parityError++;
                }

                if (CSL_SPI_SPIFLG_TIMEOUTFLG_MASK
                    == (retStatus & CSL_SPI_SPIFLG_TIMEOUTFLG_MASK))
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                        CSL_SPI_SPIFLG_TIMEOUTFLG_MASK;
                    status = Spi_TIMEOUT_ERR;
                    instHandle->stats.timeoutError++;
                }

                if (CSL_SPI_SPIFLG_DESYNCFLG_MASK
                    == (retStatus &  CSL_SPI_SPIFLG_DESYNCFLG_MASK ))
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                        CSL_SPI_SPIFLG_DESYNCFLG_MASK ;
                    status = Spi_DESYNC_ERR ;
                    instHandle->stats.desyncError++;
                }

                /* in case of any error we will break out of the operation    */
                if (IOM_COMPLETED != status)
                {
                    chanHandle->currError = status;
                    break;
                }
            }
        }
        else
        {
            status = IOM_ETIMEOUT;
            instHandle->stats.timeoutError++;
            break;
        }
        ioCount++;
    }

    /* If CSHOLD is set then the CS should be deactivated here, at the end of *
     * request.                                                               */
    if (Spi_CSHOLD == (dataParam->flags & Spi_CSHOLD))
    {
        Spi_localGpioPinToggle(chanHandle,Spi_HIGH);
    }

    if ((dataParam->flags & Spi_CSHOLD_FOR_MULTI_TRANSCEIVE) !=
        Spi_CSHOLD_FOR_MULTI_TRANSCEIVE)
    {
        /* Pointing to the MSByte of SPIDAT1 to reset CSHOLD bit              */
        spiDat1 = ((volatile Uint8*)
                        &((instHandle->deviceInfo.baseAddress)->SPIDAT1)) + 3u;

        assert(NULL != spiDat1);

        *spiDat1 &= ~(Spi_SPIDAT1_CSHOLD_8BIT_MASK);
    }

    /* Updating the transmit and receive stats                                */
    instHandle->stats.txBytes += ioCount;
    instHandle->stats.rxBytes += ioCount;

    chanHandle->activeIOP->status = status;
    if (TRUE == chanHandle->txBufFlag)
    {
        chanHandle->activeIOP->size -= chanHandle->txBufferLen;
    }
    else
    {
        chanHandle->activeIOP->size -= chanHandle->rxBufferLen;
    }

    /* disable all the spi transfers                                          */
    instHandle->deviceInfo.baseAddress->SPIGCR1 &=
        (~CSL_SPI_SPIGCR1_ENABLE_MASK);

    /* There is no transaction for now.It will be set again when there is     *
     * actually transaction to be started                                     */
    chanHandle->pendingState = FALSE;
    instHandle->currentActiveChannel = NULL;

    return status;
}



/**
 *  \brief  IO Control for SPI device. Currently it is used for loopback only
 *
 *  \param  handle      [IN]    Spi Driver instance handle.
 *  \param  cmd         [IN]    Command to do operation.
 *  \param  cmdArg      [IN]    Additional parameters required for the command
 *  \param  param       [IN]    For future reference.
 *  \param  eb          [OUT]   error block
 *
 *  \return None
 *
 */
static Int32 spiIoctl(Ptr            handle,
                      Spi_ioctlCmd   cmd,
                      Ptr            cmdArg,
                      Ptr            param)
{
    Uint32         key         = 0;
    Spi_ChanObj   *chanHandle  = NULL;
    Spi_Object    *instHandle  = NULL;
    Int32          retVal      = IOM_COMPLETED;
    Int32          status      = IOM_COMPLETED;

    assert(NULL != handle);

    chanHandle = (Spi_ChanObj *)handle ;
    assert(NULL != chanHandle);

    instHandle = chanHandle->instHandle;
    assert(NULL != instHandle);

    /* To remove the compiler warnings                                        */
    if(NULL != param)
    {
        param = param;
    }

    /* power ON the module                                                    */
    status = Spi_localLpscOn(instHandle);
    assert(IOM_COMPLETED == status);


    if (Spi_IOCTL_CANCEL_PENDING_IO == cmd)
    {
        /* Start of Critical Section                                          */
        key = (Uint32)_disable_interrupts();

        if (Spi_OpMode_POLLED != instHandle->opMode)
        {
            chanHandle->abortAllIo = TRUE;
            spiCancelAllIo(instHandle,chanHandle);
        }
        else
        {
            status = IOM_EBADMODE;
        }
        /* End of Critical Section                                            */
        _restore_interrupts(key);
    }
    else if (Spi_IOCTL_SET_CS_POLARITY == cmd)
    {
        Spi_CsPolarity *csPol;

        assert(NULL != cmdArg);

        csPol = (Spi_CsPolarity*)cmdArg;

        instHandle->csHighPolarity = csPol->isCsActiveHigh;

        if(TRUE != instHandle->csHighPolarity)
        {
            /* Here it is intended that the CS polarity is active low, thus   *
             * the default value (inactive state) should be high              */
            instHandle->spiHWconfig.csDefault |= csPol->csMask;
        }
        else
        {
            /* Here it is intended that the CS polarity is active high,thus   *
             * the default value (inactive state) should be low               */
            instHandle->spiHWconfig.csDefault &= ~(csPol->csMask);
        }

        /* now set the value to the register                                  */
        (instHandle->deviceInfo.baseAddress)->SPIDEF =
            instHandle->spiHWconfig.csDefault;
    }
    else if (Spi_IOCTL_SET_POLLEDMODETIMEOUT == cmd)
    {
        assert(NULL != cmdArg);

        /* Update the polledModeTimeout value                                 */
        instHandle->polledModeTimeout = *((Int32 *)cmdArg);
    }
    /* Unrecognised Command                                                   */
    else
    {
        status = IOM_EBADARGS;
    }

    /* power OFF the module                                                   */
    retVal = Spi_localLpscOff(instHandle);

    /* if the IOCTL has completed OK then we will update the LPSC status so   *
     * that any error also will be updated                                    */
    if (IOM_COMPLETED == status)
    {
        status = retVal;
    }

    return (status);
}

/**
 *  \brief  Interrupt handler for SPI Device
 *
 *         It will check the following errors like bit error, desync error
 *         parity error, overrun error. Data transfer either read or write
 *         is done on RX INT flag only.
 *
 *  \param  instHandle  [IN]    Pointer to the spi driver object
 *
 *  \return None
 */
Void spiIntrHandler(Spi_Object *instHandle)
{
    volatile Uint32   intStatus  = 0;
    Uint32            spiData1   = 0;
    Spi_ChanObj      *chanHandle = NULL;
    static Bool       csEnabled  = FALSE;

    if (NULL != instHandle)
    {
        spiData1 = (instHandle->deviceInfo.baseAddress)->SPIDAT1
                        & (0xFFFF0000u);

        chanHandle = instHandle->currentActiveChannel;

        if(Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
        {
            intStatus = (instHandle->deviceInfo.baseAddress)->SPIFLG;

            /* only process these if the error interrupt is set               */
            spiHandleErrors(instHandle,chanHandle,intStatus);

        }
        else
        {
            intStatus = ((instHandle->deviceInfo.baseAddress)->SPIFLG
                            & (Spi_INTERRUPT_MASK));
        }

        while ((0 != intStatus)     &&
               (NULL != chanHandle) &&
               (Spi_OpMode_INTERRUPT == instHandle->opMode))
        {
            /* Receive data Handling is done here                             */
            if (CSL_SPI_SPIFLG_RXINTFLG_MASK
                == (intStatus & CSL_SPI_SPIFLG_RXINTFLG_MASK))
            {
                if (0 != chanHandle->rxBufferLen)
                {
                    if (TRUE == chanHandle->abortAllIo)
                    {
                        Spi_localGpioPinToggle(chanHandle,Spi_HIGH);
                        spiCompleteIOInIsr(instHandle);
                    }
                    else
                    {
                        if (TRUE != chanHandle->charLength16Bits)
                        {
                            (*(chanHandle->rxBuffer)) =
                                ((Uint8)((instHandle->deviceInfo.baseAddress)->SPIBUF));

                            if (TRUE == chanHandle->rxBufFlag)
                            {
                                chanHandle->rxBuffer++;
                            }
                        }
                        else
                        {
                            (*((Uint16*)chanHandle->rxBuffer)) =
                                ((Uint16)((instHandle->deviceInfo.baseAddress)->SPIBUF));

                            if (TRUE == chanHandle->rxBufFlag)
                            {
                                chanHandle->rxBuffer += 2;
                            }
                        }

                        /* decrement the buffer length                        */
                        chanHandle->rxBufferLen--;
                        instHandle->stats.rxBytes++;
                    }

                    if(0 == chanHandle->rxBufferLen)
                    {
                        if(TRUE == csEnabled)
                        {
                            Spi_localGpioPinToggle(chanHandle,Spi_HIGH);
                        }
                    }
                }
            }

            /* Transmit data Handling is done here                            */
            if (CSL_SPI_SPIFLG_TXINTFLG_MASK
                == (intStatus & CSL_SPI_SPIFLG_TXINTFLG_MASK))
            {
                if (0 != chanHandle->txBufferLen)
                {
                    if (TRUE == chanHandle->abortAllIo)
                    {
                        Spi_localGpioPinToggle(chanHandle,Spi_HIGH);
                        spiCompleteIOInIsr(instHandle);
                    }
                    else
                    {
                        if (FALSE == csEnabled)
                        {
                            Spi_DataParam *dataPrm;

                            csEnabled = TRUE;
                            Spi_localGpioPinToggle(chanHandle,Spi_LOW);

                            /* Provide delay between CS active and actual IO
                               as desired by the used   */
                           if(NULL != chanHandle->activeIOP)
                            {
                                dataPrm = \
                                    (Spi_DataParam*)chanHandle->activeIOP->addr;
                                if(NULL != dataPrm)
                                {
                                    if(Spi_GPIO_CS == (dataPrm->flags & Spi_GPIO_CS))
                                    {
                                        Spi_localgenericDelay((Int32)dataPrm->csToTxDelay);
                                    }
                                }
                            }

                        }

                        if (TRUE != chanHandle->charLength16Bits)
                        {
                            (instHandle->deviceInfo.baseAddress)->SPIDAT1 =
                                (spiData1 | (*chanHandle->txBuffer));

                            if (TRUE == chanHandle->txBufFlag)
                            {
                                chanHandle->txBuffer++;
                            }
                        }
                        else
                        {
                            (instHandle->deviceInfo.baseAddress)->SPIDAT1 =
                                (spiData1 | (*((Uint16*)chanHandle->txBuffer)));

                            if (TRUE == chanHandle->txBufFlag)
                            {
                                chanHandle->txBuffer += 2;
                            }
                        }

                        /* decrement the buffer length                        */
                        chanHandle->txBufferLen--;
                        instHandle->stats.txBytes++;
                    }
                }
            }

            /* only process these if the error interrupt is set               */
            spiHandleErrors(instHandle,chanHandle,intStatus);

            if (((0 == chanHandle->rxBufferLen)  &&
                 (0 == chanHandle->txBufferLen)) ||
                 (0 != chanHandle->currError))
            {
                if (csEnabled == TRUE)
                {
                    csEnabled = FALSE;
                    Spi_localGpioPinToggle(chanHandle,Spi_HIGH);
                }
                spiCompleteIOInIsr(instHandle);
            }

            /* update the current active channel(in case of new active iop)   */
            chanHandle = instHandle->currentActiveChannel;

            intStatus = ((instHandle->deviceInfo.baseAddress)->SPIFLG
                            & (Spi_INTERRUPT_MASK));
        }
    }
}


/**
 *  \brief  Function to process error interrutps during SPI transfer
 *
 *         It will check the following errors like bit error, desync error
 *         parity error, overrun error.
 *
 *  \param  instHandle  [IN]    Pointer to the spi driver object
 *  \param  chanHandle  [IN]    Pointer to the spi channel object
 *  \param  intStatus   [IN]    Current interrupt status
 *  \return None
 */
Void spiHandleErrors(Spi_Object  *instHandle,
                     Spi_ChanObj *chanHandle,
                     Uint32       intStatus)
{
#ifdef Spi_EDMA_ENABLE
    EDMA3_DRV_PaRAMRegs paramSet    = {0};
    Uint32              bytesRemain = 0;
    Uint32              actualSize  = 0;
    IOM_Packet         *ioPacket    = NULL;
#endif

    if((NULL != instHandle) && (NULL != chanHandle))
    {
        if (0 != (intStatus & (CSL_SPI_SPIFLG_OVRNINTFLG_MASK |
                         CSL_SPI_SPIFLG_PARERRFLG_MASK        |
                         CSL_SPI_SPIFLG_TIMEOUTFLG_MASK       |
                         CSL_SPI_SPIFLG_BITERRFLG_MASK        |
                         CSL_SPI_SPIFLG_DESYNCFLG_MASK)))
        {
#ifdef Spi_EDMA_ENABLE
            if(Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                /* Disable the Tx and Rx  DMA transfers here                  */
                EDMA3_DRV_disableLogicalChannel(
                    instHandle->hEdma,
                    instHandle->deviceInfo.rxDmaEventNumber,
                    EDMA3_DRV_TRIG_MODE_EVENT);

                EDMA3_DRV_disableLogicalChannel(
                    instHandle->hEdma,
                    instHandle->deviceInfo.txDmaEventNumber,
                    EDMA3_DRV_TRIG_MODE_EVENT);

                EDMA3_DRV_setOptField(
                    instHandle->hEdma,
                    instHandle->deviceInfo.rxDmaEventNumber,
                    EDMA3_DRV_OPT_FIELD_TCC,
                    instHandle->deviceInfo.rxDmaEventNumber);

                EDMA3_DRV_setOptField(
                    instHandle->hEdma,
                    instHandle->deviceInfo.txDmaEventNumber,
                    EDMA3_DRV_OPT_FIELD_TCC,
                    instHandle->deviceInfo.txDmaEventNumber);

                EDMA3_DRV_setOptField(
                    instHandle->hEdma,
                    instHandle->deviceInfo.rxDmaEventNumber,
                    EDMA3_DRV_OPT_FIELD_TCC,
                    instHandle->deviceInfo.rxDmaEventNumber);

                (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                    ~(CSL_SPI_SPIINT0_DMAREQEN_MASK);

            }
#endif
            /* check Timeout interrupt                                        */
            if (CSL_SPI_SPIFLG_TIMEOUTFLG_MASK
                == (intStatus & CSL_SPI_SPIFLG_TIMEOUTFLG_MASK))
            {
                chanHandle->currError = Spi_TIMEOUT_ERR;
                (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                    CSL_SPI_SPIFLG_TIMEOUTFLG_MASK;
                instHandle->stats.timeoutError++;
            }

            if (CSL_SPI_SPIFLG_PARERRFLG_MASK
                == (intStatus &  CSL_SPI_SPIFLG_PARERRFLG_MASK))
            {
                /* Parity error interrupt                                     */
                chanHandle->currError = Spi_PARITY_ERR ;
                (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                    CSL_SPI_SPIFLG_PARERRFLG_MASK;
                instHandle->stats.parityError++;
            }

            if (CSL_SPI_SPIFLG_DESYNCFLG_MASK
                == (intStatus &  CSL_SPI_SPIFLG_DESYNCFLG_MASK ))
            {
                /* De-Synchrinization interrupt, holds only in master mode    */
                chanHandle->currError = Spi_DESYNC_ERR ;
                (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                    CSL_SPI_SPIFLG_DESYNCFLG_MASK ;
                instHandle->stats.desyncError++;
            }

            if (CSL_SPI_SPIFLG_BITERRFLG_MASK
                == (intStatus &  CSL_SPI_SPIFLG_BITERRFLG_MASK))
            {
                /* Bit error Interrupt                                        */
                chanHandle->currError = Spi_BIT_ERR ;
                (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                    CSL_SPI_SPIFLG_BITERRFLG_MASK;
                instHandle->stats.bitError++;
            }

            /* Receive Over run interrupt                                     */
            if ((intStatus & CSL_SPI_SPIFLG_OVRNINTFLG_MASK)
                == CSL_SPI_SPIFLG_OVRNINTFLG_MASK)
            {
                chanHandle->currError = Spi_RECEIVE_OVERRUN_ERR;
                (instHandle->deviceInfo.baseAddress)->SPIFLG |=
                    CSL_SPI_SPIFLG_OVRNINTFLG_MASK;
                instHandle->stats.rxOverrunError++;

            }
#ifdef Spi_EDMA_ENABLE
            if(Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                /* check how many bytes were xferred before EDMA was stopped  */
                EDMA3_DRV_getPaRAM (instHandle->hEdma,
                                    instHandle->deviceInfo.rxDmaEventNumber,
                                    &paramSet);

                bytesRemain = (paramSet.aCnt * paramSet.bCnt);

                ioPacket = chanHandle->activeIOP;

                assert(NULL != ioPacket);
                
                /* update the amount of bytes processed                       */
                actualSize = ioPacket->size;

                ioPacket->size -= bytesRemain;

                instHandle->stats.rxBytes  += (actualSize - bytesRemain);

                EDMA3_DRV_getPaRAM (instHandle->hEdma,
                                    instHandle->deviceInfo.txDmaEventNumber,
                                    &paramSet);

                bytesRemain = (paramSet.aCnt * paramSet.bCnt);

                instHandle->stats.txBytes  += (actualSize - bytesRemain);

                /* Update the status for this IOP                             */
                chanHandle->activeIOP->status = chanHandle->currError;

                spi_localCompleteIOedmaCallback(instHandle);
            }
#endif            
        }
    }

    return;
}


/**
 *  \brief   function is used after the completion of ISR
 *
 *  This function is called after Interrupt routine is proccessed out.
 *  This functions ensure whether driver is ready for next operation or not.
 *  Also it gets the next available channel to be processed for data transfer.
 *
 *  \param   instHandle [IN] pointer to the spi driver object
 *
 *  \return  None
 *
 *  \enter   instHandle  must be a valid pointer and should not be null.
 *
 *  \leave   Not Implemented.
 */
static Void spiCompleteIOInIsr (Spi_Object *instHandle)
{
    volatile Uint8      *spiDat1    = NULL;
    Spi_ChanObj         *chanHandle = NULL;
    Spi_DataParam       *dataParam  = NULL;
    IOM_Packet          *ioPacket   = NULL;
#ifdef BIOS_PWRM_ENABLE
    PWRM_Status          status     = PWRM_SOK;
    Uint32               count      = 0x00;
#endif
    Int32                retVal     = IOM_COMPLETED;

    assert(NULL != instHandle);
    chanHandle = instHandle->currentActiveChannel;
    assert(NULL != chanHandle);
    assert(NULL != chanHandle->activeIOP);
    dataParam  = (Spi_DataParam *)chanHandle->activeIOP->addr;
    assert(NULL != dataParam);

    /* Check the Pending State                                                */
    if (TRUE == chanHandle->abortAllIo)
    {
        /* Error of Cancel IO                                                 */
        chanHandle->activeIOP->status = IOM_EABORT;
        chanHandle->charLength16Bits = FALSE;

        /* Disable Receive and transmit interrupts                            */
        (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                   (~(CSL_SPI_SPIINT0_RXINTENA_MASK
                      | CSL_SPI_SPIINT0_TXINTENA_MASK ));
    }
    else
    {
        chanHandle->charLength16Bits = FALSE;
        chanHandle->activeIOP->status = chanHandle->currError;

        /* Disable Receive and transmit interrupts                            */
        (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                   (~(CSL_SPI_SPIINT0_RXINTENA_MASK
                      | CSL_SPI_SPIINT0_TXINTENA_MASK ));
    }

    /* check if CSHOLD needs to be enable after the transmission also.        *
     * If No or if there is any eror disable CSHOLD value, else do nothing    *
     * Also toggle the gpio pin if GPIO_CS is enabled is enabled              */
    if ((dataParam->flags & Spi_CSHOLD_FOR_MULTI_TRANSCEIVE) !=
           Spi_CSHOLD_FOR_MULTI_TRANSCEIVE)
    {
        /* Pointing to the MSByte of SPIDAT1 to reset CSHOLD bit              */
        spiDat1 =
            ((volatile Uint8*)&((instHandle->deviceInfo.baseAddress)->SPIDAT1))+3u;
        assert(NULL != spiDat1);

        *spiDat1 &= ~(Spi_SPIDAT1_CSHOLD_8BIT_MASK);
    }

    /* disable all the spi transfers                                          */
    instHandle->deviceInfo.baseAddress->SPIGCR1 &=
        (~CSL_SPI_SPIGCR1_ENABLE_MASK);

    /* Mask off interrupts                                                    */
    (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
         (~(Spi_INTERRUPT_MASK));

    /* call the application completion callback function registered           *
     * with us during opening of the channel                                  */
    if (NULL != chanHandle->cbFxn)
    {
        if (TRUE == chanHandle->txBufFlag)
        {
            chanHandle->activeIOP->size -= chanHandle->txBufferLen;
        }
        else
        {
            chanHandle->activeIOP->size -= chanHandle->rxBufferLen;
        }

        /* power off the module                                               */
        retVal = Spi_localLpscOff(instHandle);

        if (IOM_COMPLETED == chanHandle->activeIOP->status)
        {
            chanHandle->activeIOP->status = retVal;
        }

        if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, chanHandle->activeIOP);
        }
    }

    /*There is no transaction for now.It will be set again when there is actual*
     *a transaction is to be started                                          */
    chanHandle->pendingState = FALSE;

    chanHandle->activeIOP = NULL;
    chanHandle->currError = 0;

    /* Check if the abort flag is set for this channel. If set, we need to    *
     * remove all (empty) the pending packets from the list and send it back  *
     * to the upper layer. We do it here because, the packets should be dealt *
     * with  in FIFO order                                                    */
    if (TRUE == chanHandle->abortAllIo)
    {
        while (FALSE == QUE_empty(&chanHandle->queuePendingList))
        {
            /* we have atleast one packet                                     */
            ioPacket = (IOM_Packet *)QUE_get(&chanHandle->queuePendingList);

            if (NULL != ioPacket)
            {
                ioPacket->status = IOM_ABORTED;
                ioPacket->size = 0;

                /* power off the module                                       */
                Spi_localLpscOff(instHandle);

                /* Invoke Application callback for this channel               */
                if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
                {
                    (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
                }

                instHandle->stats.pendingPacket--;
            }
        }
        chanHandle->abortAllIo = FALSE;
    }

#ifdef BIOS_PWRM_ENABLE
    if (TRUE != instHandle->pwrmInfo.ioSuspend)
    {
#endif
        Spi_loadPendedIops(instHandle);
#ifdef BIOS_PWRM_ENABLE
    }
    else
    {
        if (TRUE == instHandle->pscPwrmEnable)
        {
            if ((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent))
            {
                /* reduce the dependency count                                */
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
        
            /* set the current active channel as NULL                         */
            instHandle->currentActiveChannel = NULL;

            instHandle->pwrmInfo.ioSuspend = FALSE;
            instHandle->devState = Spi_DriverState_PWRM_SUSPEND;

            /* call the delayed completion function                           */
            (instHandle->pwrmInfo.delayedCompletionFxn  \
                [instHandle->pwrmInfo.pwrmEvent])();
            
            /* No more packets to load.Disable the interrupts                 */
            (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                (~Spi_INTERRUPT_MASK);
        }
    }
#endif
}

/**
 *  \brief  function is used get the next channel
 *
 *  This function is used to get the next highest priority channel. Also it
 *  checks for any pending data in that stream.
 *
 *  \param  instHandle  [IN]    handle to the SPI instance
 *  \param  pChanHandle [OUT]   pointer to the channel handle
 *
 *  \return None
 *
 *  \enter  instHandle  must be a valid pointer and should not be null.
 *          pChanHandle must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
Void Spi_localGetNextChannel(Spi_Object *instHandle,
                             Spi_ChanObj**pChanHandle)
{
    Uint32 counter             = 0;
    Uint32 chanIndexWithMaxPri = Spi_NUM_CHANS;
    Int32 lastFoundMaxPri      = 0;

    assert(NULL != instHandle);

    for (counter = 0; counter < Spi_NUM_CHANS; counter++)
    {
        if((Spi_DriverState_OPENED == instHandle->chanObj[counter].channelState)
            && (lastFoundMaxPri < instHandle->chanObj[counter].taskPriority))
        {
            if(FALSE == QUE_empty(&(instHandle->chanObj[counter].queuePendingList)))
            {
                lastFoundMaxPri = instHandle->chanObj[counter].taskPriority;
                chanIndexWithMaxPri = counter;
            }
        }
    }

    if (Spi_NUM_CHANS != chanIndexWithMaxPri)
    {
        *pChanHandle = &(instHandle->chanObj[chanIndexWithMaxPri]);
    }
    else
    {
        *pChanHandle = NULL;
    }
}


/**
 *  \brief  It will configure the SPI hardware as per user configuration params.
 *         Before setting the Hardware it will put the SPI device in reset and
 *         again out of reset too before setting hardware.
 *         It will set the enable bit of the SPI device.
 *
 *  \param  instHandle   [IN]  pointer to the spi driver object instance
 *  \param  eb           [OUT] error block
 *
 *  \return IOM_COMPLETED if success
 *         else error code in case of failure
 */
static Int32 spiSetupConfig(Spi_ChanObj *chanHandle)
{
    Uint16           sPIPC0     = 0;
    Uint32           prescale   = 0;
    Int32            status     = IOM_COMPLETED;
    Int32            count      = 0;
    Spi_Object      *instHandle = NULL;
#ifdef BIOS_PWRM_ENABLE
    Uns              setpoint   = 0x00;
    PWRM_Domain      domain     = PWRM_CPU;    
    PWRM_Status      retVal     = PWRM_SOK;
#endif

    assert(NULL != chanHandle);

    instHandle = chanHandle->instHandle;

    assert(NULL != instHandle);

    /* clock is enabled in master mode only                                   */
    if (Spi_CommMode_MASTER == instHandle->spiHWconfig.masterOrSlave)
    {
        /* get the information about the current set point                    */
#ifdef BIOS_PWRM_ENABLE
        if (TRUE == instHandle->pscPwrmEnable)
        {
            /* check what clock domain is supported by the device                     */
            if (Spi_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
            {
                domain = PWRM_CPU;
            }
            else
            {
                domain = PWRM_PER;
            }            
            
            retVal = PWRM_getCurrentSetpoint(domain,&setpoint);

            if (PWRM_SOK == retVal)
            {
                status = SpiCalculateClockSettings(
                                instHandle,
                                setpoint,
                                chanHandle->busFreq,
                                &prescale);

                if (IOM_COMPLETED == status)
                {
                    /* check if the prescaler value is within the limits      */
                    if ((2 > prescale) ||  (prescale > 255u ))
                    {
                        status = IOM_EBADARGS;
                    }
                }
            }
        }
        else
        {
#endif        
            /* Prescale value calculation                                     */
            prescale =
                (instHandle->deviceInfo.inputFrequency/ chanHandle->busFreq)-1u;

            /* prescaler is a 8 bit register and can hold maximum 255         */
            if (prescale > 255u )
            {
                status = IOM_EBADARGS;
            }
#ifdef BIOS_PWRM_ENABLE
        }
#endif
    }

    if ((TRUE == instHandle->spiHWconfig.clkInternal) &&
            (Spi_CommMode_SLAVE == instHandle->spiHWconfig.masterOrSlave))
    {
        status = IOM_EBADMODE;
    }

    if (IOM_COMPLETED == status)
    {
        /*Put the module in reset mode                                        */
        (instHandle->deviceInfo.baseAddress)->SPIGCR0 =
            CSL_SPI_SPIGCR0_RESET_IN_RESET;

        /* Bring module out of reset                                          */
        (instHandle->deviceInfo.baseAddress)->SPIGCR0 |=
            CSL_SPI_SPIGCR0_RESET_OUT_OF_RESET;

        /* check if the loopback mode is requested                            */
        if (TRUE == chanHandle->loopbackEnabled)
        {
            /* Enable loopback                                                */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 |=
                CSL_SPI_SPIGCR1_LOOPBACK_MASK;
        }
        else
        {
            /* disable loopback                                               */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~(CSL_SPI_SPIGCR1_LOOPBACK_MASK));
        }

        /* Clock configuration                                                */
        if (FALSE != instHandle->spiHWconfig.clkInternal)
        {
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 |=
                ((Uint32)CSL_SPI_SPIGCR1_CLKMOD_MASK);
        }
        else
        {
            /* configure Clock Externally                                     */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~((Uint32)CSL_SPI_SPIGCR1_CLKMOD_MASK));
        }

        /* Mode of operation of the driver                                    */
        if (Spi_CommMode_MASTER != instHandle->spiHWconfig.masterOrSlave)
        {
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 &=
                (~((Uint32)CSL_SPI_SPIGCR1_MASTER_MASK));
        }
        else
        {
            /* Device in master mode                                          */
            (instHandle->deviceInfo.baseAddress)->SPIGCR1 |=
                ((Uint32)CSL_SPI_SPIGCR1_MASTER_MASK) ;
        }

        /* Select the mode of operation of the SPI device(3pin,4pin,5pin)     */
        switch (instHandle->spiHWconfig.pinOpModes)
        {
            /* 3-pin configuration (DI, DO, CLK pins are used)                */
            case Spi_PinOpMode_3PIN:
                sPIPC0 |=
                    ((CSL_SPI_SPIPC0_SOMIFUN_SPI
                        << CSL_SPI_SPIPC0_SOMIFUN_SHIFT)
                    | (CSL_SPI_SPIPC0_SIMOFUN_SPI
                        << CSL_SPI_SPIPC0_SIMOFUN_SHIFT)
                    | (CSL_SPI_SPIPC0_CLKFUN_SPI
                        << CSL_SPI_SPIPC0_CLKFUN_SHIFT));

                (instHandle->deviceInfo.baseAddress)->SPIPC0 = sPIPC0;

                spiConfigureOpMode(instHandle);
                break ;
            /* 4-pin CS configuration (DI, DO, CLK & CS pins are used)        */
            case Spi_PinOpMode_SPISCS_4PIN:
                sPIPC0 |= ((CSL_SPI_SPIPC0_SOMIFUN_SPI
                               << CSL_SPI_SPIPC0_SOMIFUN_SHIFT)
                          | (CSL_SPI_SPIPC0_SIMOFUN_SPI
                               << CSL_SPI_SPIPC0_SIMOFUN_SHIFT)
                          | (CSL_SPI_SPIPC0_CLKFUN_SPI
                               << CSL_SPI_SPIPC0_CLKFUN_SHIFT));

                (instHandle->deviceInfo.baseAddress)->SPIPC0 = sPIPC0;

                spiConfigureOpMode(instHandle);
                break;
            /* 4-pin CS configuration (DI, DO, CLK & ENA pins are used)       */
            case Spi_PinOpMode_SPIENA_4PIN:
                sPIPC0 |=
                    ((CSL_SPI_SPIPC0_SOMIFUN_SPI
                        << CSL_SPI_SPIPC0_SOMIFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_SIMOFUN_SPI
                        << CSL_SPI_SPIPC0_SIMOFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_CLKFUN_SPI
                        << CSL_SPI_SPIPC0_CLKFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_ENAFUN_SPI
                        << CSL_SPI_SPIPC0_ENAFUN_SHIFT));

                (instHandle->deviceInfo.baseAddress)->SPIPC0 = sPIPC0;

                spiConfigureOpMode(instHandle);
                break ;

           case Spi_PinOpMode_5PIN:
            /**< 5-pin configuration(DI, DO, CLK, CS & ENA pins are used)     */
                sPIPC0 |=
                    ((CSL_SPI_SPIPC0_SOMIFUN_SPI
                        << CSL_SPI_SPIPC0_SOMIFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_SIMOFUN_SPI
                        << CSL_SPI_SPIPC0_SIMOFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_CLKFUN_SPI
                        << CSL_SPI_SPIPC0_CLKFUN_SHIFT)
                    |(CSL_SPI_SPIPC0_ENAFUN_SPI
                        << CSL_SPI_SPIPC0_ENAFUN_SHIFT));

                (instHandle->deviceInfo.baseAddress)->SPIPC0 = sPIPC0;

                spiConfigureOpMode(instHandle);
                break;

            default:
                status = IOM_EBADARGS;
        }

        /* select the data format to use by the SPI device                    */
        (instHandle->deviceInfo.baseAddress)->SPIDAT1 &=
            (~((Uint32)CSL_SPI_SPIDAT1_DFSEL_MASK));

        (instHandle->deviceInfo.baseAddress)->SPIDAT1 |=
            (((Uint32)CSL_SPI_SPIDAT1_DFSEL_FORMAT0)<<
             ((Uint32)CSL_SPI_SPIDAT1_DFSEL_SHIFT));

        /* Program the format registers                                       */
        for (count = 0; count < Spi_DATAFORMAT_VALUE; count++)
        {
            if ((Spi_CommMode_SLAVE == instHandle->spiHWconfig.masterOrSlave)
                && (count > 0) )
            {
                /* slave mode supports only format0                           */
                break;
            }

            /* This value should be loaded only in master mode                */
            if (Spi_CommMode_MASTER == instHandle->spiHWconfig.masterOrSlave)
            {
                /* Program the prescaler                                      */
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                    (~((Uint32)CSL_SPI_SPIFMT_PRESCALE_MASK));

                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                    (prescale << ((Uint32)CSL_SPI_SPIFMT_PRESCALE_SHIFT));
            }

            /* Defines the word length of data format                         */
            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                (~((Uint32)CSL_SPI_SPIFMT_CHARLEN_MASK));

            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                instHandle->spiHWconfig.configDatafmt[count].charLength;


            /* configure the data direction                                   */
            if (FALSE != instHandle->spiHWconfig.configDatafmt[count].lsbFirst)
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                    (((Uint32)CSL_SPI_SPIFMT_SHIFTDIR_LSB)
                    << ((Uint32)CSL_SPI_SPIFMT_SHIFTDIR_SHIFT));
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                    (~((Uint32)CSL_SPI_SPIFMT_SHIFTDIR_MASK));
            }

            /* configure the phase In                                         */
            if (FALSE != instHandle->spiHWconfig.configDatafmt[count].phaseIn)
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                    (((Uint32)CSL_SPI_SPIFMT_PHASE_DELAY)
                    << ((Uint32)CSL_SPI_SPIFMT_PHASE_SHIFT));
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                    (~((Uint32)CSL_SPI_SPIFMT_PHASE_MASK));
            }

            /* configure the polarity                                         */
            if (FALSE != instHandle->spiHWconfig.configDatafmt[count].clkHigh)
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                    (((Uint32)CSL_SPI_SPIFMT_POLARITY_HIGH)
                    << ((Uint32)CSL_SPI_SPIFMT_POLARITY_SHIFT));
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                    (~((Uint32)CSL_SPI_SPIFMT_POLARITY_MASK));
            }


            /* Insert Delay value between two consecutive words               */
            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                (~((Uint32)CSL_SPI_SPIFMT_WDELAY_MASK));

            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                (instHandle->spiHWconfig.configDatafmt[count].wDelay
                << ((Uint32)CSL_SPI_SPIFMT_WDELAY_SHIFT) );

            /* Enable the parity                                              */
            if (FALSE !=
                instHandle->spiHWconfig.configDatafmt[count].parityEnable)
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                    (((Uint32)CSL_SPI_SPIFMT_PARENA_ENABLE)
                    << ((Uint32)CSL_SPI_SPIFMT_PARENA_SHIFT));

                if (FALSE !=
                    instHandle->spiHWconfig.configDatafmt[count].oddParity)
                {
                     /* configure the odd parity                              */
                    (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                        (((Uint32)CSL_SPI_SPIFMT_PARPOL_ODD)
                        << ((Uint32)CSL_SPI_SPIFMT_PARPOL_SHIFT));
                }
                else
                {
                    /* configure the even parity                              */
                    (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                        (~((Uint32)CSL_SPI_SPIFMT_PARPOL_MASK));
                }
            }
            else
            {
                (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                    (~((Uint32)CSL_SPI_SPIFMT_PARENA_MASK));
            }


            /* configure the wait enable                                      */
            if ((Spi_PinOpMode_3PIN != instHandle->spiHWconfig.pinOpModes) &&
                (Spi_CommMode_MASTER == instHandle->spiHWconfig.masterOrSlave))
            {
                if (FALSE !=
                    instHandle->spiHWconfig.configDatafmt[count].waitEnable)
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                        (((Uint32)CSL_SPI_SPIFMT_WAITENA_ENABLE)
                        << ((Uint32)CSL_SPI_SPIFMT_WAITENA_SHIFT));
                }
                else
                {
                    (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                        (~((Uint32)CSL_SPI_SPIFMT_WAITENA_MASK));
                }
            }
        }

        if (FALSE != instHandle->spiHWconfig.enableHighZ)
        {
            (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
                (((Uint32)CSL_SPI_SPIINT0_ENABLEHIGHZ_ENABLE)
                << ((Uint32)CSL_SPI_SPIINT0_ENABLEHIGHZ_SHIFT));
        }
        else
        {
            (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                (~((Uint32)CSL_SPI_SPIINT0_ENABLEHIGHZ_MASK));
        }

        /* Interrupt mapped to level INT1                                     */
        if (FALSE != instHandle->spiHWconfig.intrLevel)
        {
            (instHandle->deviceInfo.baseAddress)->SPILVL  = 0x000003FFu;
        }
        else
        {
            /* Interrupt mapped to level INT0                                 */
            (instHandle->deviceInfo.baseAddress)->SPILVL  = 0x00000000u;
        }

        /* Default chip select pattern used for moment when none of           *
         * device is active                                                   */
        (instHandle->deviceInfo.baseAddress)->SPIDEF =
            instHandle->spiHWconfig.csDefault;

        /* Enable SPI for further transaction                                 */
        (instHandle->deviceInfo.baseAddress)->SPIGCR1 |=
            (((Uint32)CSL_SPI_SPIGCR1_ENABLE_ENABLE)
                << ((Uint32)CSL_SPI_SPIGCR1_ENABLE_SHIFT));

    }/*< if status inside  if status                                          */

    return (status); /* success  or error code                                */
}

/**
 *  \brief  Configure the chipselect pin so it will act as per spi protocol
 *         to select the one of slaves.It will also configure the delay
 *         register of following type of delay.
 *
 *         The Chip-select-active-to-ENA-signal-active-time-out
 *         The Transmit-data-finished-to-ENA-pin-inactive-time-out
 *         The Transmit-end-to-chip-select-inactive-delay
 *         The Chip-select-active-to-transmit-start-delay
 *
 *  \param  Spihandle   [IN]    SPI driver object for respective instance.
 *
 */
static Void spiConfigureOpMode(const Spi_Object *instHandle)
{
    Uint32      dly = 0;

    assert(NULL != instHandle);
    /* Delay register is configured only in master mode, No meaning to        *
     * configure in Slave mode                                                */

    if (Spi_CommMode_MASTER == instHandle->spiHWconfig.masterOrSlave)
    {
        /* Insert the Chip-select-active-to-ENA-signal-active-time-out        */
        /* Insert the Transmit-data-finished-to-ENA-pin-inactive-time-out     */
        /* Insert the Transmit-end-to-chip-select-inactive-delay              */
        /* Insert the Chip-select-active-to-transmit-start-delay              */
        dly |= (instHandle->spiHWconfig.delay.c2EDelay)
                   | (instHandle->spiHWconfig.delay.t2EDelay
                   << CSL_SPI_SPIDELAY_T2EDELAY_SHIFT)
                   | (instHandle->spiHWconfig.delay.t2CDelay
                   << CSL_SPI_SPIDELAY_T2CDELAY_SHIFT)
                   | (instHandle->spiHWconfig.delay.c2TDelay
                   << CSL_SPI_SPIDELAY_C2TDELAY_SHIFT);

        (instHandle->deviceInfo.baseAddress)->SPIDELAY |= dly ;
    }
}


/**
 *  \brief  Register interrupt with BIOS and enable them
 *
 *  \param  instNum      Instance number
 *  \param  intNum       Interrupt number
 *  \param  initIsr      Function Pointer to ISR function
 *  \param  spiObj       Handle to be passed to ISR function
 *
 *  \return void
 */
static Void spiRegisterIntrHandler(Uint32      instNum,
                                   Uint32      intNum,
                                   Spi_isr     initIsr,
                                   Ptr         spiObj)
{
    Spi_Object     *instHandle = NULL;
    ECM_Attrs      ecmattrs    = ECM_ATTRS;

    assert(NULL != initIsr);
    assert(NULL != spiObj);

    instHandle = spiObj;
    assert(NULL != instHandle);

    /* To remove the compiler warning                                         */
    instNum = instNum;

    /* Mapping the event id to the ECM Dispatch of the DSP/BIOS               */
    /* Enable the interrupt later                                             */
    ecmattrs.unmask = 1u;

    /*setup arg such that isr will get 'chan' as arg                          */
    ecmattrs.arg    = (Arg)instHandle;

    /* Plug the ISR with interrupt (event) number                             */
    ECM_dispatchPlug(intNum, (ECM_Fxn)initIsr, &ecmattrs);

    /* Enabling the event                                                     */
    ECM_enableEvent(intNum);

    /* Enabling the HWI_ID                                                    */
    C64_enableIER(1u << instHandle->hwiNumber);
}

/**
 *  \brief  Disable and Un-Register interrupt with BIOS.
 *
 *  \param  instNum   Instance number
 *  \param  intNum    Interrupt number
 *
 *  \return None
 */
static Void spiUnregisterIntrHandler(Uint32 instNum, Uint32 intNum)
{
    /* To remove the compiler warning                                         */
    instNum = instNum;

    /* Disabling the interrupts                                               */
    ECM_disableEvent(intNum);
}

/*
 *  \brief  Aborts all the pending IO in the driver.
 *
 *          This function is used to abort all the pending IO in the driver,
 *          including the current active IO.
 *
 * \param   instHandle  [IN]   Handle to the device instance
 * \param   chanHandle  [IN]   Channel handle
 *
 * \return  NONE
 *
 */
static Void spiCancelAllIo(Spi_Object *instHandle,Spi_ChanObj *chanHandle)
{
#ifdef Spi_EDMA_ENABLE
    EDMA3_DRV_PaRAMRegs paramSet    = {0};
    Uint32              edmaNum     = 0x00;
#endif
    Uint32              bytesRemain = 0x00;
    IOM_Packet         *ioPacket    = NULL;
    Uint32              spidat1     = 0;
    Spi_DataParam      *dataParam   = NULL;
    volatile Uint16    *spiDat1Cfg  = NULL;

    assert((NULL != chanHandle) && (NULL != instHandle));

    /* we need to abort all the IOP's held by the driver and return the same  *
     * to the application                                                     */
#ifdef Spi_EDMA_ENABLE
    if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        /* The driver is running in the DMA mode of operation. Hence first    *
         * stop the DMA engine                                                */
        EDMA3_DRV_disableLogicalChannel(
            instHandle->hEdma,
            instHandle->deviceInfo.rxDmaEventNumber,
            EDMA3_DRV_TRIG_MODE_EVENT);

        EDMA3_DRV_disableLogicalChannel(
            instHandle->hEdma,
            instHandle->deviceInfo.txDmaEventNumber,
            EDMA3_DRV_TRIG_MODE_EVENT);

        if (TRUE == chanHandle->txBufFlag)
        {
            edmaNum = instHandle->deviceInfo.txDmaEventNumber;
        }
        else
        {
            edmaNum = instHandle->deviceInfo.rxDmaEventNumber;
        }

        /* check how many bytes were transferred before the EDMA was stopped  */
        EDMA3_DRV_getPaRAM (instHandle->hEdma, edmaNum, &paramSet);

        bytesRemain = (paramSet.aCnt * paramSet.bCnt);
    }
    else
    {
#endif    
        /* interrupt mode of operation                                        */
        if (TRUE == chanHandle->txBufFlag)
        {
            bytesRemain = chanHandle->txBufferLen;
        }
        else
        {
            bytesRemain = chanHandle->rxBufferLen;
        }
#ifdef Spi_EDMA_ENABLE
    }
#endif
    /* get the current active packet                                          */
    ioPacket = chanHandle->activeIOP;

    if (NULL != ioPacket)
    {
        /* active packet is available we will abort it                        */
        ioPacket->status = IOM_ABORTED;

        chanHandle->rxBufferLen = 0;
        chanHandle->txBufferLen = 0;

        /* update the amount of bytes processed                               */
        ioPacket->size -= bytesRemain;

        instHandle->stats.rxBytes  += ioPacket->size;
        instHandle->stats.txBytes  += ioPacket->size;

        /* power OFF the module here                                          */
        Spi_localLpscOff(instHandle);

        /* callback function is available or not                              */
        if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
        }

        chanHandle->activeIOP = NULL;
        chanHandle->pendingState = FALSE;
        chanHandle->currError = 0;
    }

    /* Empty the pendList queue. Common for DMA/Interrupt Transactions        */
    while (TRUE != QUE_empty(&(chanHandle->queuePendingList)))
    {
        ioPacket = (IOM_Packet *)
                        QUE_get(&(chanHandle->queuePendingList));

        ioPacket->status = IOM_ABORTED;
        instHandle->stats.pendingPacket--;
        ioPacket->size = 0;

        /* power OFF the module here                                          */
        Spi_localLpscOff(instHandle);

        /* callback function is available or not                              */
        if ((NULL != chanHandle->cbFxn) &&
            (NULL != chanHandle->cbArg))
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
        }
    }
    chanHandle->abortAllIo = FALSE;

    chanHandle->activeIOP = NULL;
    chanHandle->pendingState = FALSE;
    chanHandle->currError = 0;

    if((chanHandle == instHandle->currentActiveChannel)
        ||(NULL == instHandle->currentActiveChannel))
    {
        instHandle->currentActiveChannel = NULL;
        /* get the channel with highest priority                              */
        Spi_localGetNextChannel(instHandle, &(instHandle->currentActiveChannel));        
        /* store for local use                                                */
        chanHandle = instHandle->currentActiveChannel;
    }
    else
    {
        chanHandle = NULL;
    }
    
    /* check and load next pending packet                                     */
    if (NULL != chanHandle)
    {
        /* we have atleast one packet                                         */
        ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));
        instHandle->stats.pendingPacket--;

        /* validate and update the iop                                        */
        if (NULL  != ioPacket)
        {
            chanHandle->activeIOP = ioPacket;
            dataParam = (Spi_DataParam *)ioPacket->addr;

            assert(NULL != dataParam);
            
            /* Set the current buffer params correctly                        */
            chanHandle->currError = 0;

            if (instHandle->spiHWconfig.configDatafmt[dataParam->dataFormat].
                charLength > 8u)
            {
                chanHandle->charLength16Bits = TRUE;
            }

            /* Check if the inBuffer or the outBuffer is NULL i.e,not         *
             * supplied and update respective buffer appropriately with       *
             * driver buffer                                                  */
            if (NULL == dataParam->inBuffer)
            {
                /* user has not suppiled the IN buffer                        */
                chanHandle->rxBufFlag = FALSE;

                /* No alignment required here                                 */
                chanHandle->rxBuffer = &chanHandle->rxTransBuf[0];

                /* update the details of the out buffer here as it cannot     *
                 * be NULL (condition already checked)                        */
                chanHandle->txBufFlag = TRUE;
                chanHandle->txBuffer  = dataParam->outBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }
            else if (NULL == dataParam->outBuffer)
            {
                /* user has not suppiled the IN buffer                        */
                chanHandle->txBufFlag = FALSE;

                /* No alignment required here                                 */
                chanHandle->txBuffer = &chanHandle->txTransBuf[0];

                /* update the details of the "inbuffer" here as it cannot     *
                 * be NULL (condition already checked)                        */
                chanHandle->rxBufFlag = TRUE;
                chanHandle->rxBuffer  = dataParam->inBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }
            else
            {
                /* Both the buffers are valid                                 */
                chanHandle->rxBufFlag = TRUE;
                chanHandle->txBufFlag = TRUE;
                chanHandle->rxBuffer  = dataParam->inBuffer;
                chanHandle->txBuffer  = dataParam->outBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }

            /* Consume any stale data                                         */
            while (CSL_SPI_SPIBUF_RXEMPTY_MASK
                != ((instHandle->deviceInfo.baseAddress)->SPIBUF
                & CSL_SPI_SPIBUF_RXEMPTY_MASK))
            {
                (*chanHandle->rxBuffer) =
                    (Uint8)(instHandle->deviceInfo.baseAddress)->SPIBUF;

                /* Dont increment the buffer as we are reading stale data     */
            }

            if ((dataParam->bufLen) > 0)
            {
                /* load the new configuration values to the SPIDAT1 register  */
                spidat1 = Spi_localControlDataConfig(
                               chanHandle,
                               dataParam->chipSelect,
                               dataParam->dataFormat,
                               dataParam->gpioPinNum,
                               dataParam->flags);

                spiDat1Cfg = (((volatile Uint16 *)
                         (&(instHandle->deviceInfo.baseAddress)->SPIDAT1))+1);

                if (NULL != spiDat1Cfg)
                {
                    /* write the upper 16 bits which contain the configs to   *
                     * the SPIDAT1 upper 16bits                               */
                    *spiDat1Cfg = (Uint16)(spidat1 >> 16);
                }

                /* Enabling receive and error interrupts                      */
                (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
                    (CSL_SPI_SPIINT0_BITERRENA_MASK
                    | CSL_SPI_SPIINT0_TIMEOUTENA_MASK
                    | CSL_SPI_SPIINT0_OVRNINTENA_MASK
                    | CSL_SPI_SPIINT0_DESYNCENA_MASK
                    | CSL_SPI_SPIINT0_PARERRENA_MASK
                    | CSL_SPI_SPIINT0_RXINTENA_MASK
                    | CSL_SPI_SPIINT0_TXINTENA_MASK);
            }/**< if currBuffLen                                              */
        }
    }
    else
    {
        /* No more packets to load.Disable the interrupts                     */
        (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                (~Spi_INTERRUPT_MASK);        
    }
}

/**
 *  \brief   checks if a timeout has happened from the given time.
 *
 *  This function checks if the timeout with respect to the start time has
 *  occured. The timeout and the start time is also input to the function.
 *  it checks the current time and compares with the recorded start time value
 *  and then specified timeout. The function returns TRUE in case of time out
 *  else FALSE is returned.
 *
 *  \param   startValue  [IN]   Start value
 *  \param   timeout     [IN]   Timeout value
 *
 *  \return  TRUE   if time out occured.
 *           FALSE  if time out has not occured.
 *
 */
static Bool spiCheckTimeOut(Uint32 startValue,Int32 timeout)
{
    Uint32  checkValue = 0;
    Bool    retVal     = TRUE;

    /* get the current tick value and compare with the start value            */
    checkValue = CLK_gethtime();

    /* check if the current tick count counter has overflowed and suitably    *
     * calculate the elapsed time                                             */
    if (checkValue < startValue)
    {
        checkValue = (((0xFFFFFFFFU) - startValue) + checkValue) + (1U) ;
    }
    else
    {
        checkValue = checkValue - startValue;
    }

    /* if the elapsed time is greater than start tick then set retval to TRUE *
     * to indicate time out else send false                                   */
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
 *  \brief   Function to toggle the SPI gpio pin.
 *
 *  This function toggles a gpio pin which is used by the SPI for communcation
 *  with the slave device which requires that the CS be driven
 *  active-inactive for every transaction of word width.
 *  This functions assumes Spi_LOW for activation and Spi_High for deactivation
 *  Depending upon CS Polarity set in paramters, it is made either zero or one
 *  \param   chanHandle  [IN]   Valid channel handle
 *           toggleState [IN]   the CS assertion or deassertion
 *  \return  None
 */
Void Spi_localGpioPinToggle(Spi_ChanObj *chanHandle,Bool toggleState)
{
    Spi_Object        *instHandle;
    Spi_DataParam     *dataParam;
    Gpio_PinCmdArg     userPinCmdArg;

    if(NULL != chanHandle)
    {
        instHandle = (Spi_Object*)(chanHandle->instHandle);

        if (NULL != instHandle)
        {
            if (Spi_CommMode_MASTER == instHandle->spiHWconfig.masterOrSlave)
            {
                if (NULL != chanHandle->activeIOP)
                {
                    dataParam  = (Spi_DataParam *)(chanHandle->activeIOP->addr);

                    if (NULL != dataParam)
                    {
                        if (Spi_GPIO_CS == (dataParam->flags & Spi_GPIO_CS))
                        {
                            /* The normal polarity is usaully false hence we  *
                             * will assume all states with respect tothis only*/
                            if (FALSE == instHandle->csHighPolarity)
                            {
                                userPinCmdArg.value = toggleState;
                            }
                            else
                            {
                                /* invert the user provioded value            */
                                userPinCmdArg.value = (Uint32)(!toggleState);
                            }

                            userPinCmdArg.pin = dataParam->gpioPinNum;
                            Gpio_setPinVal(chanHandle->hGpio,&userPinCmdArg);
                        }
                    }
                }
            }
        }
    }
}

/**
 *  \brief   Function for generic delay.
 *           This function introduces a delay in a tight while loop.
 *
 *  \note    The loop is not calibrated.
 *  \param   None
 *
 *  \return  None
 */
Void Spi_localgenericDelay(Int32 count)
{
    volatile Uint32 delayLoop1, delayLoop2;

    for (delayLoop1 = 0; delayLoop1 < count; delayLoop1++)
    {
        for (delayLoop2 = 0; delayLoop2 < count; delayLoop2++)
        {
            ;
        }
    }
}

/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/


/**
 *  \brief    Spi_localLpscOn
 *
 *            This function is invoked to power on the SPI peripheral and
 *            configure it according to the operating mode. Note that this
 *            function uses different functions to power ON the module,
 *            depending on the PWRM is enabled in this driver or NOT
 *
 *  \param    instHandle  [IN] Handle to the device instance
 *
 *  \return   IOM_COMPLETED in case of success, an ERROR code in case of error.
 *
 *  \enter    instHandle    must be a valid pointer
 *
 *  \leave    Not Implemented.
 */
Int32 Spi_localLpscOn(Spi_Object *instHandle)
{
    Int32  status = IOM_COMPLETED;

    assert(NULL != instHandle);
    if (TRUE == instHandle->pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power on using bios PWRM API                                       */
        status = (Int32)PWRM_setDependency(
                    (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
        /* power on using the PSC API                                         */
        status = Psc_ModuleClkCtrl(
                    (Psc_DevId)instHandle->deviceInfo.pscInstance,
                    (Uint32)instHandle->deviceInfo.pwrmLpscId,
                    TRUE);
#endif
    }

    return (status);
}

/**
 * \brief    Spi_localLpscOff
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
Int32 Spi_localLpscOff(Spi_Object *instHandle)
{
    Int32  status = IOM_COMPLETED;

    assert(NULL != instHandle);
    if (TRUE == instHandle->pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power off  using PWRM API                                          */
        status = (Int32)PWRM_releaseDependency(
                    (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
        /* power off using PSC API only if it is an instance other than "0"   */
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
Void Spi_loadPendedIops(Spi_Object *instHandle)
{
    Spi_ChanObj         *chanHandle = NULL;
    Spi_DataParam       *dataParam  = NULL;
    IOM_Packet          *ioPacket   = NULL;
    volatile Uint16     *spiDat1Cfg = NULL;
    Uint32               spidat1    = 0;

    assert(NULL != instHandle);
    
    /* get the next channel that can be processed                             */
    Spi_localGetNextChannel(instHandle, &(instHandle->currentActiveChannel));
    
    chanHandle = instHandle->currentActiveChannel;
    
    if ((NULL != chanHandle) &&
         (FALSE == QUE_empty(&(chanHandle->queuePendingList))))
    {
        /* we have atleast one packetto process                               */
        ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));
        instHandle->stats.pendingPacket--;
           
        /* validate and update the iop                                        */
        if (NULL  != ioPacket)
        {
            chanHandle->activeIOP = ioPacket;
            dataParam = (Spi_DataParam *)ioPacket->addr;
            
            assert(NULL != dataParam);

            /* Set the current buffer params correctly                        */
            chanHandle->currError         = 0;

            if (instHandle->spiHWconfig.configDatafmt[dataParam->dataFormat].
                 charLength > 8u)
            {
                chanHandle->charLength16Bits = TRUE;
            }

            /* Check if the inBuffer or the outBuffer is NULL i.e,not supplied*
             * and update respective buffer appropriately with driver buffer  */
            if (NULL == dataParam->inBuffer)
            {
                /* user has not suppiled the IN buffer                        */
                chanHandle->rxBufFlag = FALSE;

                /* No alignment required here                                 */
                chanHandle->rxBuffer = &chanHandle->rxTransBuf[0];

                /* update the details of the out buffer here as it cannot     *
                 * be NULL (condition already checked)                        */
                chanHandle->txBufFlag = TRUE;
                chanHandle->txBuffer  = dataParam->outBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }
            else if (NULL == dataParam->outBuffer)
            {
                /* user has not suppiled the IN buffer                        */
                chanHandle->txBufFlag = FALSE;

                /* No alignment required here                                 */
                chanHandle->txBuffer = &chanHandle->txTransBuf[0];

                /* update the details of the "inbuffer" here as it cannot     *
                 * be NULL (condition already checked)                        */
                chanHandle->rxBufFlag = TRUE;
                chanHandle->rxBuffer  = dataParam->inBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }
            else
            {
                /* Both the buffers are valid                                 */
                chanHandle->rxBufFlag = TRUE;
                chanHandle->txBufFlag = TRUE;
                chanHandle->rxBuffer  = dataParam->inBuffer;
                chanHandle->txBuffer  = dataParam->outBuffer;
                chanHandle->rxBufferLen = dataParam->bufLen;
                chanHandle->txBufferLen = dataParam->bufLen;
            }


            if (Spi_OpMode_INTERRUPT == instHandle->opMode)
            {
                /* Consume any stale data                                     */
                while (CSL_SPI_SPIBUF_RXEMPTY_MASK
                    != ((instHandle->deviceInfo.baseAddress)->SPIBUF
                    & CSL_SPI_SPIBUF_RXEMPTY_MASK))
                {
                    (*chanHandle->rxBuffer) =
                        (Uint8)(instHandle->deviceInfo.baseAddress)->SPIBUF;
                    /* Dont increment the buffer as we are reading stale data */
                }

                if ((dataParam->bufLen) > 0)
                {
                    /* load the new configuration values to the SPIDAT1 reg   */
                    spidat1 = Spi_localControlDataConfig(
                                   chanHandle,
                                   dataParam->chipSelect,
                                   dataParam->dataFormat,
                                   dataParam->gpioPinNum,
                                   dataParam->flags);

                    spiDat1Cfg = (((volatile Uint16 *)
                             (&(instHandle->deviceInfo.baseAddress)->SPIDAT1))+1);

                    if (NULL != spiDat1Cfg)
                    {
                        /* write the upper 16 bits which contain the configs  *
                         * to the SPIDAT1 upper 16bits                        */
                        *spiDat1Cfg = (Uint16)(spidat1 >> 16);
                    }    

                    /* enable all the spi transfers                           */
                    instHandle->deviceInfo.baseAddress->SPIGCR1 |=
                        (CSL_SPI_SPIGCR1_ENABLE_MASK);

                    /* Enabling receive and error interrupts                  */
                    (instHandle->deviceInfo.baseAddress)->SPIINT0 |=
                        (CSL_SPI_SPIINT0_BITERRENA_MASK
                        | CSL_SPI_SPIINT0_TIMEOUTENA_MASK
                        | CSL_SPI_SPIINT0_OVRNINTENA_MASK
                        | CSL_SPI_SPIINT0_DESYNCENA_MASK
                        | CSL_SPI_SPIINT0_PARERRENA_MASK
                        | CSL_SPI_SPIINT0_RXINTENA_MASK
                        | CSL_SPI_SPIINT0_TXINTENA_MASK);
                }
            }
            else if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                /* Call this to complete edma configuration and transfer      */
                Spi_localEdmaTransfer(instHandle,dataParam); 
            }
        } /* NULL != ioPacket */
        else
        {
            if (Spi_OpMode_INTERRUPT == instHandle->opMode)
            {
                /* No more packets to load.Disable the interrupts             */
                (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                    (~Spi_INTERRUPT_MASK);
            }
            else if (Spi_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                (instHandle->deviceInfo.baseAddress)->SPIINT0 &=
                     (~CSL_SPI_SPIINT0_OVRNINTENA_MASK);
            }
        }
    } /* NULL != chanHandle */
    
    /* update the state of the driver                                         */
    instHandle->devState = Spi_DriverState_CREATED;    
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
static Int32 SpiRegisterNotification(Spi_Object *instHandle)
{
    Uint32       eventCnt = 0;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Spi_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL, 
                    &SpiNotifyCallback,
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
static Int32 SpiUnregisterNotification(Spi_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Spi_MAX_PWRM_EVENTS; eventCnt++)
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
static PWRM_NotifyResponse SpiNotifyCallback(PWRM_Event    eventType,
                                             Arg           eventArg1,
                                             Arg           eventArg2,
                                             Arg           clientArg)
{
    Spi_Object          *instHandle = NULL;
    PWRM_NotifyResponse  pwrmStatus = PWRM_NOTIFYDONE;
    PWRM_Status          status     = PWRM_SOK;

    assert(NULL != clientArg);

    instHandle = (Spi_Object *)clientArg;
    assert(NULL != instHandle);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
            pwrmStatus = SpiSuspendCurrentIops(instHandle,eventType);
            break;
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
            /* CPU or PERIPHERAL domain V/F setpoint or a sleep command is    *
             * issued hence we will complete the current IO and suspend all   *
             * the pending IOs                                                */
            if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                     (Spi_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                     (Spi_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {             
                pwrmStatus = SpiSuspendCurrentIops(instHandle,eventType);
            }
            break;
        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
            /* The pending CPU setpoint change has now been made.Hence now we *
             * will recalculate the new settings for this instance and then   *
             * restore the pending state of the driver                        */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                     (Spi_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                      (Spi_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {             
                pwrmStatus = SpiConfigureNewVfSettings(
                                 instHandle,
                                 (Uns)eventArg1,
                                 (Uns)eventArg2);
            }
            
            if (PWRM_NOTIFYDONE == pwrmStatus)
            {
                Spi_loadPendedIops(instHandle);
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
                Spi_loadPendedIops(instHandle);
            }
            else
            {
                pwrmStatus = PWRM_NOTIFYNOTDONE;
            }
            break;
        case PWRM_AWAKEFROMSTANDBY:
            Spi_loadPendedIops(instHandle);
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
static PWRM_NotifyResponse SpiSuspendCurrentIops(Spi_Object  *instHandle,
                                                 PWRM_Event   eventType)
{
    Uint32               hwiKey = 0x00;
    PWRM_NotifyResponse  status = PWRM_NOTIFYDONE; 
    
    assert(NULL != instHandle);

    hwiKey = _disable_interrupts();

    if (NULL == instHandle->currentActiveChannel)
    {
        /* No IOP is pending currently. we can return the status of the       *
         * notify call as sucess                                              */
        instHandle->devState = Spi_DriverState_PWRM_SUSPEND;
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
static PWRM_NotifyResponse SpiConfigureNewVfSettings(Spi_Object   *instHandle,
                                                     Uns           eventArg1,
                                                     Uns           eventArg2)
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
        status = SpiCalculateClockSettings(
                     instHandle,
                     eventArg2,
                     instHandle->chanObj[0].busFreq,
                     &prescale);
        
        if ((IOM_COMPLETED == status)  &&
            ((2 <= prescale) && (prescale <= 255u)))
        {
            /* calcualation of the prescaler value is successful.Hence update *
             * the hardware with the new prescaler values                     */
            status = SpiConfigureNewClkSettings(instHandle,prescale);
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

static Int32 SpiCalculateConstraints(Spi_Object *instHandle)
{
    Spi_ChanObj    *chanHandle      = NULL;
    Uns             numberSetpoints = 0x00;
    Uns             setpoint        = 0x00;
    Uint32          prescale        = 0x00;
    Uint32          mask            = 0x00;
    Int32           status          = IOM_COMPLETED;
    PWRM_Constraint constraint      = PWRM_DISALLOWEDSLEEPSTATE_MASK;
    PWRM_Status     retVal          = PWRM_SOK;
    PWRM_Domain     domain          = PWRM_CPU;

    assert(NULL != instHandle);

    /* get the Handle to any of the channel                                   */
    chanHandle = &instHandle->chanObj[0];
    assert(NULL != chanHandle);
   
    /* check what clock domain is supported by the device                     */
    if (Spi_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
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
            status = SpiCalculateClockSettings(
                          instHandle,
                          setpoint,
                          chanHandle->busFreq,
                          &prescale);

            if (IOM_COMPLETED == status)
            {
                /* successfully calculated now check if the prescaler is OK   */
                if ((2 > prescale) ||  (prescale > 255u ))
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
 * \leave     Not imeplemented
 * 
 * \note      Please note that the calculated prescaler values are not validated
 *            in this function.
 */
static Int32 SpiCalculateClockSettings(Spi_Object *instHandle,
                                       Uint32      setpoint,
                                       Uint32      opFreq,
                                       Uint32     *prescale)
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
    if (Spi_pllDomain_0 != instHandle->pwrmInfo.pllDomain)
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
                (*prescale) = ((((frequency * 1000)/sysclkDiv)/opFreq) - 1u);
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
 * \brief     This function configures the SPI device with the new prescalar
 *            values.
 * 
 * \param     instHandle [IN]  Handle to the driver instance object
 * \param     prescale   [IN]  prescaler value to program.
 * 
 * \return    None
 * 
 * \enter     instHandle is a valid non null pointer
 *            prescalar is a valid value
 * 
 * \leave     Not implemented
 * 
 */
static Int32 SpiConfigureNewClkSettings(Spi_Object *instHandle,
                                        Uint32      prescale)
{
    Uint32  count  = 0;
    Int32   status = IOM_EBADARGS;
    
    assert(NULL != instHandle);

    if (IOM_COMPLETED == Spi_localLpscOn(instHandle))
    {
        for (count = 0; count < Spi_DATAFORMAT_VALUE; count++)
        {
            /* Program the prescaler                                          */
            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] &=
                (~((Uint32)CSL_SPI_SPIFMT_PRESCALE_MASK));
        
            (instHandle->deviceInfo.baseAddress)->SPIFMT[count] |=
                (prescale << ((Uint32)CSL_SPI_SPIFMT_PRESCALE_SHIFT)) ;
        }

        status = Spi_localLpscOff(instHandle); 
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
static Int32 SpiUnregisterConstraints(Spi_Object *instHandle)
{
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;
    Uint32       count  = 0x00;

    assert(NULL != instHandle);
        
    for (count = 0; count < Spi_MAX_PWRM_CONSTRAINTS; count++)
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

