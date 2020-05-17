/*
 * I2c.c
 *
 * This file contains Driver Layer Interface implementation for the I2C Driver.
 * I2C Driver provides Driver Layer Interface to do operations on the I2C
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
 *  \file   I2c.c
 *
 *  \brief  I2C driver implementation
 *
 *  This file contains the IOM driver implementation for the I2c device
 *
 *  \author     PSG
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <string.h>
#include <assert.h>
#include <que.h>
#include <iom.h>
#include <c64.h>
#include <hwi.h>
#include <clk.h>
#include <tsk.h>
#include <sem.h>
#include <ecm.h>

#ifdef I2c_EDMA_ENABLE
    #include <ti/sdo/edma3/drv/edma3_drv.h>
#endif

#include "ti/pspiom/i2c/I2c.h"
#include "ti/pspiom/psc/Psc.h"
#include "I2cLocal.h"

#ifdef CHIP_C6747
    #include <ti/pspiom/cslr/soc_C6747.h>
#endif

#ifdef CHIP_OMAPL137
    #include <ti/pspiom/cslr/soc_OMAPL137.h>
#endif

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif

/* -------- constants -------- */
const struct I2c_Params I2c_PARAMS =
{
    TRUE,              /* enablecache     */
    I2c_OpMode_POLLED, /* mode            */
    0x10u,             /* ownAddr         */
    8,                 /* numBits         */
    200000,            /* busFreq         */
    0,                 /* addressing      */
    0,                 /* loopBackEnabled */
    0,                 /* edma3EventQueue */
    0,                 /* hwiNumber       */
    0xffffffff,        /* timeout         */
    FALSE,             /* pscPwrmEnable   */
    I2c_pllDomain_NONE /*ASYNC2 DOMAIN    */
};

const I2c_ChanParams I2c_defaultRxIoParams = {NULL};
const I2c_ChanParams I2c_defaultTxIoParams = {NULL};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */

static Bool inUse[CSL_I2C_PER_CNT];

static I2c_Module_State I2c_module = {&inUse[0]};

static I2c_Object I2c_Instances[CSL_I2C_PER_CNT];

static I2c_HwInfo I2c_deviceInstInfo[CSL_I2C_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

static Int i2cCalculateClockSettings(const I2c_ChanObj *chanHandle,
                                     Uint32            *psc,
                                     Uint32            *clock);

static Int i2cTransfer(I2c_Object    *instHandle,
                       I2c_ChanObj   *chanHandle,
                       I2c_DataParam *dataParam,
                       Ptr           param);

static Int i2cWaitForBB(const  I2c_Object *instHandle,
                        Uint32             waitCount,
                        Uint32             sleepAllowed);

static Void i2cRegisterIntrHandler(Uint32     instNum,
                                   Uint32     intNum,
                                   I2c_isr    isrFunc,
                                   Ptr        i2cObj);

static Void i2cCancelIO(I2c_Object *instHandle,Int retCode);

static void i2cUnregisterIntrHandler(Uint32 instNum, Uint32 intNum);

static Bool i2cCheckTimeOut(Uint32 startValue,Uint32 timeout);

static Void i2cIntrHandler(I2c_Object *instHandle);

static Void i2cCompleteIOInIsr(I2c_Object *instHandle);

static Int32 i2cPolledModeTransfer(I2c_ChanObj *chanHandle);

static Int i2cMdBindDev(Ptr *devp, Int devId, Ptr devParams);

static Int i2cMdUnBindDev(Ptr devp);

static Int i2cMdCreateChan(Ptr                *chanp,
                           Ptr                 devp,
                           String              name,
                           Int                 mode,
                           Ptr                 chanParams,
                           IOM_TiomCallback    cbFxn,
                           Ptr                 cbArg);

static Int i2cMdDeleteChan(Ptr chanp);

static Int i2cSoftReset(I2c_ChanObj   *chanHandle);

static Int i2cMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);

static Int i2cMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs);

#ifdef BIOS_PWRM_ENABLE
static Int32 I2cRegisterNotification(I2c_Object *instHandle);

static PWRM_NotifyResponse I2cNotifyCallback(PWRM_Event    eventType,
                                             Arg           eventArg1,
                                             Arg           eventArg2,
                                             Arg           clientArg);

static Int32 I2cCalculateConstraints(I2c_Object *instHandle);

static PWRM_NotifyResponse I2cSuspendCurrentIops(I2c_Object  *instHandle,
                                                 PWRM_Event   eventType);
                                                  
static PWRM_NotifyResponse I2cConfigureNewVfSettings(I2c_Object   *instHandle,
                                                     Uns           eventArg1,
                                                     Uns           eventArg2);

static Int32 I2cCalculateClkSettings(I2c_Object *instHandle,
                                     Uint32      setpoint,
                                     Uint32      opFreq,
                                     Uint32     *prescale,
                                     Uint32     *clkDiv);

static Int32 I2cConfigureNewClkSettings(I2c_Object *instHandle,
                                        Uint32      prescale,
                                        Uint32      clkDiv);

static Int32 I2cUnregisterNotification(I2c_Object *instHandle);

static Int32 I2cUnregisterConstraints(I2c_Object *instHandle);

static Void I2c_saveContext(I2c_Object *instHandle);

static Void I2c_restoreContext(I2c_Object *instHandle);
#endif

const IOM_Fxns I2c_IOMFXNS =
{
    &i2cMdBindDev,
    &i2cMdUnBindDev,
    &i2cMdControlChan,
    &i2cMdCreateChan,
    &i2cMdDeleteChan,
    &i2cMdSubmitChan,
};

/* ========================================================================== */
/*                          FUNCTION DEFINTIONS                               */
/* ========================================================================== */



/* ========================================================================== */
/*                            MODULE FUNCTIONS                                */
/* ========================================================================== */

/*
 *  \brief  Function called by BIOS during instance initialisation
 *
 */
Void I2c_init(Void)
{
    Int i;

    for (i = 0; i < CSL_I2C_PER_CNT; i++)
    {
        /* have to initialize statically */
        I2c_module.inUse[i] = FALSE;
        memset((Void *)&I2c_Instances[i], 0x0, sizeof(I2c_Object));
    }
}

/*
 *  \fn     static Int i2cMdBindDev
 *
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp       [OUT]      pointer to Uart driver object
 *  \param  devId      [IN]       device Id
 *  \param  devParams  [IN]       device Parameters
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID         in case of error
 *
 *  \enter  *devp           must be a valid pointer and should not be null.
 *          devParams       must be a valid pointer and should not be null.
 *          devId           must be a valid device number (<CSL_UART_PER_CNT)
 *
 *  \leave  Not Implemented.
 */
static Int i2cMdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Uint32        key        = 0;
    Int           status     = IOM_COMPLETED;
    Uint32        count      = 0;
    I2c_ChanObj  *chanHandle = NULL;
    I2c_Params   *params     = NULL;
    I2c_Object   *instHandle = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* check if the current instance is within the limit supported by the H/W */
    if ((NULL == devParams) || (CSL_I2C_PER_CNT <= devId) || (0 > devId))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle =  &I2c_Instances[devId];
        params = (I2c_Params*)devParams;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle) || (NULL == params))
        {
            status = IOM_EBADARGS;
        }
    }

#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == status)
    {
        key = (Uint32)_disable_interrupts();

        if (TRUE == I2c_module.inUse[devId])
        {
            _restore_interrupts(key);
            status = IOM_EINUSE;
        }
        else
        {
            I2c_module.inUse[devId] = TRUE;
            _restore_interrupts(key);
        }

#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        if (IOM_COMPLETED == status)
        {
            if (0 == devId)
            {
                /* I2c0 instance  */
                I2c_deviceInstInfo[0].baseAddress      =
                    (CSL_I2cRegsOvly)CSL_I2C_0_REGS;
                I2c_deviceInstInfo[0].cpuEventNumber   =
                    (Uint32)CSL_INTC_EVENTID_I2CINT0;
                I2c_deviceInstInfo[0].rxDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C0_RX;
                I2c_deviceInstInfo[0].txDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C0_TX;
                I2c_deviceInstInfo[0].txDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C0_TX;
                I2c_deviceInstInfo[0].inputFrequency   =
                    (Uint32)CSL_I2C_0_MODULE_FREQ;
                /* always power on module(no psc control)                     */
                I2c_deviceInstInfo[0].pwrmLpscId       = 0;
                I2c_deviceInstInfo[0].pscInstance      = 0;
            }
            else if (1 == devId)
            {
                /* I2c1 instance  */
                I2c_deviceInstInfo[1].baseAddress      =
                    (CSL_I2cRegsOvly)CSL_I2C_1_REGS;
                I2c_deviceInstInfo[1].cpuEventNumber   =
                    (Uint32)CSL_INTC_EVENTID_I2CINT1;
                I2c_deviceInstInfo[1].rxDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C1_RX;
                I2c_deviceInstInfo[1].txDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C1_TX;
                I2c_deviceInstInfo[1].txDmaEventNumber =
                    (Uint32)CSL_EDMA3_CHA_I2C1_TX;
                I2c_deviceInstInfo[1].inputFrequency   =
                    (Uint32)CSL_I2C_1_MODULE_FREQ;

#ifdef BIOS_PWRM_ENABLE
                I2c_deviceInstInfo[1].pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_I2C_1;
#else
                I2c_deviceInstInfo[1].pwrmLpscId = (Uint32)CSL_PSC_I2C1;
#endif
                I2c_deviceInstInfo[1].pscInstance = (Uint32)CSL_LPSC_INST_I2C_1;
            }
        }
#endif  /* defined(CHIP_C6747) || defined(CHIP_OMAPL137) */

        if (IOM_COMPLETED == status)
        {
            /* set the current number of opens to zero                        */
            instHandle->numOpens = 0;

            /* Update the State of the driver                                 */
            instHandle->devStatus = I2c_DriverState_CREATED;

            /* update the instance related information given by application   */
            instHandle->instNum = (Uint32)devId;
            instHandle->enableCache = params->enableCache;
            instHandle->opMode    = params->opMode;
            instHandle->edma3EventQueue = params->edma3EventQueue;
            instHandle->hwiNumber = params->hwiNumber;
            instHandle->polledModeTimeout  = params->polledModeTimeout;
            instHandle->pscPwrmEnable = params->pscPwrmEnable;
            instHandle->pwrmInfo.pllDomain = params->pllDomain;

            /* initialise the instance related misc information               */
            instHandle->hwEventCallback    = NULL;
            instHandle->hEdma              = NULL;
            instHandle->dmaChaAllocated    = 0;
            instHandle->currentActiveChannel = NULL;

            /*Inintialize Statistics members                                  */
            instHandle->stats.rxBytes = 0;
            instHandle->stats.txBytes = 0;
            instHandle->stats.pendingPacket = 0;
            instHandle->stats.overFlowError = 0;
            instHandle->stats.timeoutError = 0;
            instHandle->stats.arbLossError = 0;
            instHandle->stats.nackError = 0;

            /* update the soc specific information for this instances         */
            instHandle->deviceInfo.baseAddress =
                I2c_deviceInstInfo[devId].baseAddress;
            instHandle->deviceInfo.cpuEventNumber =
                I2c_deviceInstInfo[devId].cpuEventNumber;
            instHandle->deviceInfo.txDmaEventNumber =
                I2c_deviceInstInfo[devId].txDmaEventNumber;
            instHandle->deviceInfo.rxDmaEventNumber =
                I2c_deviceInstInfo[devId].rxDmaEventNumber;
            instHandle->deviceInfo.inputFrequency =
                I2c_deviceInstInfo[devId].inputFrequency;
            instHandle->deviceInfo.pscInstance =
                I2c_deviceInstInfo[devId].pscInstance;
            instHandle->deviceInfo.pwrmLpscId =
                I2c_deviceInstInfo[devId].pwrmLpscId;

            /* update the channel related information                         */
            for (count=0; count < I2c_NUM_CHANS; count++)
            {
                chanHandle = &instHandle->chans[count];

                chanHandle->cbFxn = NULL;
                chanHandle->cbArg = NULL;
                chanHandle->instHandle = NULL;
                chanHandle->mode = (Uint32)IOM_OUTPUT;
                chanHandle->ownAddr = params->ownAddr;
                chanHandle->numBits = params->numBits;
                chanHandle->busFreq = params->busFreq;
                chanHandle->addressing = params->addressing;
                chanHandle->loopbackEnabled = params->loopbackEnabled;
                chanHandle->pendingState    = 0;
                chanHandle->abortAllIo      = 0;
                chanHandle->currError       = 0;
                chanHandle->currFlags       = 0;
                chanHandle->currBuffer      = NULL;
                chanHandle->currBufferLen   = 0;
                chanHandle->taskPriority    = I2c_MAX_CHAN_PRIORITY;
            }

            if (I2c_OpMode_POLLED == instHandle->opMode)
            {
                SEM_new(&instHandle->syncSem,1);
            }
        }

        if ((IOM_COMPLETED == status) && (FALSE == instHandle->pscPwrmEnable))
        {
            if (0 != devId)
            {        
#ifdef BIOS_PWRM_ENABLE
                /* power on using bios PWRM API                               */
                status = (Int32)PWRM_setDependency(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
                /* power on using PSC API                                     */
                status = Psc_ModuleClkCtrl(
                             (Psc_DevId)instHandle->deviceInfo.pscInstance,
                             (Uint32)instHandle->deviceInfo.pwrmLpscId,
                             TRUE);
#endif
            }
        }
#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == status) && (TRUE == instHandle->pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            status = I2cRegisterNotification(instHandle);
        }
#endif
    }

    if (IOM_COMPLETED == status)
    {
        *devp = instHandle;
    }
    else
    {
        *devp = NULL;
    }

    return (status);
}

/**
 *  \fn     static Int i2cMdUnBindDev
 *
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
static Int i2cMdUnBindDev(Ptr devp)
{
    I2c_Object *instHandle   = NULL;
    Int32       result       = IOM_COMPLETED;
    Int         chanCount    = 0;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* Check if this is a valid handle                                        */
    if (NULL != devp)
    {
#endif
        /* Validate instance ID                                               */
        instHandle = (I2c_Object *)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle) || (CSL_I2C_PER_CNT <= instHandle->instNum))
        {
            result = IOM_EBADARGS;
        }
    }
    else
    {
        result = IOM_EBADARGS;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == result)
    {
        /* Check if all the channels are closed                               */
        for (chanCount = 0; chanCount < I2c_NUM_CHANS ; chanCount++)
        {
            /* If any Channel is in Use then come out of for loop             */
            if (I2c_DriverState_OPENED ==
                instHandle->chans[chanCount].channelState)
            {
                result = IOM_EBADARGS;
                break;
            }
        }

        if (IOM_COMPLETED == result)
        {
            if ((FALSE == instHandle->pscPwrmEnable)
                && (0 != instHandle->instNum))
            {
#ifdef BIOS_PWRM_ENABLE
                /* power off  using PWRM API                                  */
                result = (Int32)PWRM_releaseDependency(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
                /* power OFF using PSC API                                    */
                result = Psc_ModuleClkCtrl(
                             (Psc_DevId)instHandle->deviceInfo.pscInstance,
                             (Uint32)instHandle->deviceInfo.pwrmLpscId,
                             FALSE);
#endif
            }
#ifdef BIOS_PWRM_ENABLE
            if ((IOM_COMPLETED == result) && (TRUE == instHandle->pscPwrmEnable))
            {
                /* unregister all the PWRM event notifications                */
                result = I2cUnregisterNotification(instHandle);
            }
#endif
            /* channel delete()                                               */
            instHandle->numOpens  = 0u;

            /* set state to deleted                                           */
            instHandle->devStatus  = I2c_DriverState_DELETED;

            /* set the module state to not in use                             */
            I2c_module.inUse[instHandle->instNum] = FALSE;
        }
    }
    return result;
}

/* ========================================================================== */
/*                           IOM FUNCTIONS                                    */
/* ========================================================================== */

/*
 *  \fn     static Int i2cMdCreateChan
 *
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the I2c device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a SIO_create call.
 * \param     chanp        [OUT]    channel object pointer
 * \param     devp         [IN]     I2c driver object
 * \param     name         [IN]     I2c Instance name like I2c0
 * \param     mode         [IN]     channel  mode -> input or output
 * \param     chanParams   [IN]     channel parameters from user
 * \param     cbFxn        [IN]     callback function pointer
 * \param     cbArg        [IN]     callback function Arguments
 *
 *  \return   IOM_COMPLETED    in case of success
 *            error code       in case of failure
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *            devp       must be a valid pointer and should not be null.
 *            name       must be a valid name.
 *            mode       must be a valid mode, either input or output.
 *            chanParams must be a valid pointer and should not be null.
 *            cbFxn      must be a valid pointer and should not be null.
 *            cbArg      must be a valid pointer and should not be null.
 *
 * \leave  Not Implemented.
 */
static Int i2cMdCreateChan(Ptr                *chanp,
                           Ptr                 devp,
                           String              name,
                           Int                 mode,
                           Ptr                 chanParams,
                           IOM_TiomCallback    cbFxn,
                           Ptr                 cbArg)
{
    I2c_Object       *instHandle  = NULL;
    I2c_ChanObj      *chanHandle  = NULL;
    I2c_ChanParams   *chanPrms    = NULL;
    Uint32            psc         = 0;
    Uint32            clk         = 0;
    Uint32            key         = 0;
    Uint32            chanCount   = 0;
    Int32             status      = IOM_COMPLETED;
    Int32             retVal      = IOM_COMPLETED;
    TSK_Handle        thisTask    = NULL;
    Bool              isTask      = FALSE;
    Bool              pscPwrOn    = FALSE;
    Bool              bFalseWhile = TRUE;
#ifdef BIOS_PWRM_ENABLE
    Uns               setpoint    = 0x00;
    PWRM_Domain       domain      = PWRM_CPU;    
#endif

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /*validate the input parameters                                           */
    if ((NULL == chanp) || (NULL == devp) || (NULL == cbFxn) || (NULL == cbArg)
        || ((IOM_INPUT != mode)  && (IOM_OUTPUT != mode) && (IOM_INOUT != mode))
        || (NULL == chanParams))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle = (I2c_Object *)devp;
        chanPrms   = (I2c_ChanParams *)chanParams;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if ((NULL == instHandle) || (NULL == chanPrms))
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == status)
    {
        do
        {
            bFalseWhile = FALSE;

            /* To remove the compiler warning                                 */
            if(name != name)
            {
                name = name;
            }

            /* The channel is being created, hence we will power on the I2C   *
             * module so as to allow the configuration of the device          */
            status = I2c_localLpscOn(instHandle);

            if (IOM_COMPLETED != status)
            {
                /* unable to switch ON the module,Hence break from here       */
                break;
            }
            else
            {
                /* Psc is powered ON                                          */
                pscPwrOn = TRUE;
            }

            for (chanCount = 0 ; chanCount < I2c_NUM_CHANS ; chanCount++)
            {
                if (NULL == instHandle->chans[chanCount].instHandle)
                {
                    /* Assignment of channel                                  */
                    chanHandle = &instHandle->chans[chanCount];
                    break;
                }
            }

            /* Check if channel is NULL                                       */
            if (NULL == chanHandle)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* Assign the Mode and Handle                                     */
            chanHandle->mode = (Uint32)mode;
            chanHandle->instHandle = (Ptr)instHandle;

            isTask = TSK_isTSK();

            if (TRUE == isTask)
            {
                /* Returns a handle to the currently executing Task object    */
                thisTask = TSK_self();

                if (NULL == thisTask)
                {
                    status = IOM_EBADARGS;
                    break;
                }

                /* Get the priority of the task                               */
                chanHandle->taskPriority = TSK_getpri(thisTask);
            }

            /* Updating the callback function parameter and argument          */
            chanHandle->cbFxn = cbFxn;
            chanHandle->cbArg = cbArg;

            chanHandle->masterOrSlave = chanPrms->masterOrSlave;

            key = (Uint32)_disable_interrupts();

            /* power management not supported for slave mode                  */
            if ((TRUE == instHandle->pscPwrmEnable) &&
                (I2c_CommMode_SLAVE == chanHandle->masterOrSlave))
            {
                status = IOM_EBADARGS;
                break;
            }
            
            /* Init hardware if first open                                    */
            if (NULL == instHandle->numOpens)
            {
                /* Put the module in reset                                    */
                (instHandle->deviceInfo.baseAddress)->ICMDR = 0;

#ifdef BIOS_PWRM_ENABLE
                if ((IOM_COMPLETED == status) &&
                    (TRUE == instHandle->pscPwrmEnable))
                {
                    status = I2cCalculateConstraints(instHandle);

                    if (IOM_COMPLETED != status)
                    {
                        /* End of critical section in case of error           */
                        _restore_interrupts(key);
                        break;                    
                    }
                }
#endif

#ifdef BIOS_PWRM_ENABLE
                if (TRUE == instHandle->pscPwrmEnable)
                {
                    /* check what clock domain is supported by the device     */
                    if (I2c_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
                    {
                        domain = PWRM_CPU;
                    }
                    else
                    {
                        domain = PWRM_PER;
                    }            
                
                    /* get the current setpoint info                          */
                    if (PWRM_SOK != PWRM_getCurrentSetpoint(domain,&setpoint))
                    {
                        status = IOM_EBADMODE;
                        break;
                    }

                    if (PWRM_SOK == retVal)
                    {
                        /* calculate the clk settings for the currentsetpoint */
                        status = I2cCalculateClkSettings(
                                      instHandle,
                                      setpoint,
                                      chanHandle->busFreq,
                                      &psc,
                                      &clk);

                        if ((IOM_COMPLETED != status) ||
                           (psc > 255) || (clk > 255))
                        {
                            status = IOM_ETIMEOUTUNREC;
                            break;
                        }
                    }
                }
                else
                {
#endif
                    /* Calculate the Clock according to Prescalar             */
                    if (0 != i2cCalculateClockSettings(chanHandle,&psc,&clk))
                    {
                        /* Clock setting has failed hence set appropriate err */
                        status = IOM_ETIMEOUTUNREC;
                        /* End of critical section in case of error           */
                        _restore_interrupts(key);
                        break;
                    }
#ifdef BIOS_PWRM_ENABLE                    
                }
#endif
                /* Load Prescalar value into Register                         */
                (instHandle->deviceInfo.baseAddress)->ICPSC = psc;

                /* Load Clock Register into Register                          */
                (instHandle->deviceInfo.baseAddress)->ICCLKH = clk/2u;
                (instHandle->deviceInfo.baseAddress)->ICCLKL =
                    (clk - (instHandle->deviceInfo.baseAddress)->ICCLKH);

                /* Set own address and bits/byte                              */
                (instHandle->deviceInfo.baseAddress)->ICOAR =
                    chanHandle->ownAddr;

                (instHandle->deviceInfo.baseAddress)->ICMDR =
                    (chanHandle->numBits & I2c_NUMBITS_MASK)
                    | CSL_I2C_ICMDR_MST_MASK;

                /* Enable/Disable DLB                                         */
                if (TRUE == chanHandle->loopbackEnabled)
                {
                    (instHandle->deviceInfo.baseAddress)->ICMDR |=
                        CSL_I2C_ICMDR_DLB_MASK;
                }
                else
                {
                    (instHandle->deviceInfo.baseAddress)->ICMDR &=
                        ~(CSL_I2C_ICMDR_DLB_MASK);
                }

                /* Enable/Disable 7/10 bit Addressing mode                    */
                if (TRUE == chanHandle->addressing)
                {
                    (instHandle->deviceInfo.baseAddress)->ICMDR |=
                        CSL_I2C_ICMDR_XA_MASK;
                }
                else
                {
                    (instHandle->deviceInfo.baseAddress)->ICMDR &=
                        (~(CSL_I2C_ICMDR_XA_MASK));
                }

                /* If interrupt mode, register ISR                            */
                if (I2c_OpMode_POLLED !=instHandle->opMode)
                {
                    /* Call this function to Register the Handler             */
                    i2cRegisterIntrHandler(
                        instHandle->instNum,
                        instHandle->deviceInfo.cpuEventNumber,
                        (I2c_isr)&i2cIntrHandler,
                        instHandle);
                }

#ifdef I2c_EDMA_ENABLE
                if (I2c_OpMode_DMAINTERRUPT == instHandle->opMode)
                {
                    if (NULL != chanPrms->hEdma)
                    {
                        instHandle->hEdma = chanPrms->hEdma;
                        /* request the edma channels                          */
                        I2c_localEdmaChanRequest(instHandle);

                        if (FALSE == instHandle->dmaChaAllocated)
                        {
                            status = IOM_EBADARGS;
                            /* End of critical section in case of error       */
                            _restore_interrupts(key);
                            break;
                        }
                    }
                    else
                    {
                        status = IOM_EBADARGS;
                        /* End of critical section in case of error           */
                        _restore_interrupts(key);
                        break;
                    }
                }
#endif
                /* Bring module out of reset                                  */
                (instHandle->deviceInfo.baseAddress)->ICMDR |=
                    CSL_I2C_ICMDR_IRS_MASK;

                /* Set the backward compatibility mode off - this is          *
                 * critical to get interrupts correctly                       */
                (instHandle->deviceInfo.baseAddress)->ICEMDR &=
                    (~CSL_I2C_ICEMDR_BCM_MASK);

            } /* First open initialization                                    */

            /* End of critical section                                        */
            _restore_interrupts(key);

            QUE_new(&(chanHandle->queuePendingList));
        }while (bFalseWhile);
    }

    /* check if the Module was powered ON in the PSC.if so then we will power *
     * it off now                                                             */
    if (TRUE == pscPwrOn)
    {
        /* power of the module in the PSC                                     */
        retVal = I2c_localLpscOff(instHandle);

        if (IOM_COMPLETED == status)
        {
            status = retVal;
        }
    }

    if (IOM_COMPLETED != status)
    {
        *chanp = NULL;
    }
    else
    {
        /* Increment the channel count and update the driver handle           */
        ++instHandle->numOpens;

        /* Updated the driver state                                           */
        chanHandle->channelState = I2c_DriverState_OPENED;

        *chanp = (Ptr)chanHandle;
    }

    return (status);
}

/**
 *  \brief    This function is called by the application to close a previously
 *            opened channel.it deletes the channel so that it is not available
 *            for further transactions. All the allocated reqources are freed &
 *            the channel will be ready for the "open" operation once again.
 *
 *  \param    chanp      [IN]   Handle to the channel.
 *
 *  \return   status
 *
 *            chanp      must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int i2cMdDeleteChan(Ptr chanp)
{
    I2c_Object    *instHandle  = NULL;
    I2c_ChanObj   *chanHandle  = NULL;
    Int32          result      = IOM_COMPLETED;
    Uint32         key         = 0;
    Int            chanCount   = 0;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL != chanp)
    {
        /* Check state of handle                                              */
        if ((NULL == ((I2c_ChanObj *)chanp)->instHandle)
            || (I2c_DriverState_OPENED != ((I2c_ChanObj *)chanp)->channelState))
        {
            result = IOM_EBADMODE;
        }
        else
        {
#endif
            instHandle = ((I2c_ChanObj *)chanp)->instHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if (NULL == instHandle)
            {
                result = IOM_EBADARGS;
            }
        }
    }
    else
    {
        result = IOM_EBADARGS;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == result)
    {
        chanHandle = (I2c_ChanObj *)chanp;

        /* Start of Critical Section                                          */
        key = (Uint32)_disable_interrupts();

        /* Decrease the Number of Opens                                       */
        --instHandle->numOpens;

        /* Exit of Critical Section                                           */
        _restore_interrupts(key);

        if (0 == instHandle->numOpens)
        {
            /* If interrupt mode, unregister ISR                              */
            if (I2c_OpMode_POLLED!= instHandle->opMode)
            {
                /* Call this function to unregister the ISR                   */
                i2cUnregisterIntrHandler(
                    instHandle->instNum,
                    instHandle->deviceInfo.cpuEventNumber);
            }

#ifdef I2c_EDMA_ENABLE
            /* Free the Edma Channels                                         */
            if (I2c_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                /* This will verify whether channel is allocated or not       */
                if (TRUE == instHandle->dmaChaAllocated )
                {
                    EDMA3_DRV_freeChannel(
                        instHandle->hEdma,
                        instHandle->deviceInfo.rxDmaEventNumber);

                    EDMA3_DRV_freeChannel(
                        instHandle->hEdma,
                        instHandle->deviceInfo.txDmaEventNumber);
                }
            }
#endif /* I2c_EDMA_ENABLE */

#ifdef BIOS_PWRM_ENABLE
            if (TRUE == instHandle->pscPwrmEnable)
            {
                result = I2cUnregisterConstraints(instHandle);
            }
#endif
        }

        chanHandle->instHandle = NULL;
        /* Reset the state variables                                          */
        chanHandle->cbFxn = NULL;
        chanHandle->cbArg = NULL;
        chanHandle->mode = (Uint32)IOM_OUTPUT;
        chanHandle->pendingState       = 0;
        chanHandle->abortAllIo         = 0;
        chanHandle->currError          = 0;
        chanHandle->currFlags          = 0;
        chanHandle->currBuffer         = NULL;
        chanHandle->currBufferLen      = 0;
        chanHandle->taskPriority       = I2c_MAX_CHAN_PRIORITY;

        /* Updated the driver state                                           */
        chanHandle->channelState = I2c_DriverState_CLOSED;

        /* Check if all the channels are closed                               */
        for (chanCount = 0; chanCount < I2c_NUM_CHANS ; chanCount++)
        {
            /* If any Channel is in Use then come out of for loop             */
            if (instHandle->chans[chanCount].instHandle != NULL)
            {
                break;
            }
        }

        /* If all channels are closed then update the state to closed         */
        if (chanCount == I2c_NUM_CHANS)
        {
            instHandle->devStatus = I2c_DriverState_CLOSED;
        }
    }

    return result;
}

/**
 *  \brief    Submit a I/O packet to a channel for processing
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
static Int i2cMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    I2c_Object         *instHandle    = NULL;
    I2c_ChanObj        *chanHandle    = NULL;
    Int32               status        = IOM_COMPLETED;
    I2c_DataParam      *dataParam     = NULL;
    volatile Uint32     delay         = I2c_POLLED_TIMEOUT;
    Uint32              key           = 0;
    Bool                pscPwrOn      = FALSE;
    Bool                bFalseWhile   = TRUE;

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL != chanp) && (NULL != ioPacket))
    {
        /* ok to check chanp struct members now because we know it's not NULL */
        if (NULL == ((I2c_ChanObj *)chanp)->instHandle)
        {
            status = IOM_EBADIO;
        }
        else
        {
#endif
            dataParam = (I2c_DataParam *)ioPacket->addr;
            instHandle = ((I2c_ChanObj *)chanp)->instHandle;
            chanHandle = (I2c_ChanObj *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if ((NULL == instHandle) || (NULL == chanHandle))
            {
                status = IOM_EBADARGS;
            }
            else
            {
                if ((NULL == dataParam) &&
                   (((IOM_READ == ioPacket->cmd) ||
                    (IOM_WRITE == ioPacket->cmd))))
                {
                    status = IOM_EBADARGS;
                }
            }
        }
    }
    else
    {
        status = IOM_EBADIO;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == status)
    {
        do
        {
            bFalseWhile   = FALSE;

            if (I2c_DriverState_OPENED != chanHandle->channelState)
            {
                 status = IOM_EBADMODE;
                 break;
            }

            /* Do not allow to address self                                   */
            if(dataParam->slaveAddr == chanHandle->ownAddr)
            {
                status = IOM_EBADARGS;
                break;
            }

            /* validating the channel mode and the command                    */
            if ((IOM_WRITE == ioPacket->cmd)
                && (IOM_INPUT != chanHandle->mode))
            {
                /* Update the flag and give Write Command                     */
                dataParam->flags |= I2c_WRITE;
            }

            /* validating the channel mode and the command                    */
            if ((IOM_READ == ioPacket->cmd)
                && (IOM_OUTPUT != chanHandle->mode))
            {
                /* Update the flag and give Read Command                      */
                dataParam->flags |= I2c_READ;
            }

            if ((IOM_ABORT == ioPacket->cmd) || (IOM_FLUSH == ioPacket->cmd))
            {
                /* Start of Critical Section                                  */
                key = (Uint32)_disable_interrupts();
                /* Just set the abort flag to true. The actual abortion of all*
                 * the requests will take place after the current IO has      *
                 * completed from the interrupt handler.This is becoz, abruptly*
                 * halting the current request may cause undefined behaviour  *
                 * on the bus                                                 */ 
                if (I2c_OpMode_POLLED != instHandle->opMode)
                {
                    /* to cancel io                                           */
                    chanHandle->abortAllIo = TRUE;
                }

                /* End of Critical Section                                    */
                _restore_interrupts(key);
                status = IOM_COMPLETED;
                break;
            }

            if(I2c_CommMode_MASTER == chanHandle->masterOrSlave)
            {
                dataParam->flags |= I2c_MASTER;
            }
            else
            {
                dataParam->flags &= ~I2c_MASTER;
            }

            /* Return error if aplication sets stop bit during repeat mode
             * and bufLen > 0                                                 */
            if ((dataParam->flags & I2c_REPEAT) == 0)
            {
                /* Check input parameters                                     */
                if (((NULL == dataParam->buffer)
                    || (dataParam->bufLen > I2c_BUFFER_MAXLENGTH))
                    || (NULL == dataParam->bufLen))
                {
                      status = IOM_EBADIO;
                      break;
                }

                if (((dataParam->flags & I2c_READ) == 0)
                    && ((dataParam->flags & I2c_WRITE)== 0))
                {
                    status = IOM_EBADARGS;
                    break;
                }
            }


            if ((dataParam->flags & I2c_REPEAT) != 0)
            {
                if ((dataParam->bufLen > 0)
                    && ((dataParam->flags & I2c_STOP) != 0))
                {
                    status = IOM_EBADIO;
                    break;
                }
            }

            if (instHandle->polledModeTimeout == 0)
            {
                status = IOM_EBADIO;
                break;
            }

#ifdef I2c_EDMA_ENABLE
            /* Validate the buffer alignment if mode is DMA                   */
            if ((I2c_OpMode_DMAINTERRUPT == instHandle->opMode)
                && (0 !=((Uint32)dataParam->buffer) % 32u))
            {
                status = IOM_EBADARGS;
                break;
            }
#endif /* I2c_EDMA_ENABLE */

            /* call the power ON function to switch ON the module in PSC      */
            status = I2c_localLpscOn(instHandle);

            if (IOM_COMPLETED != status)
            {
                /* unable to power On the module. Hence break out from here   */
                break;
            }
            else
            {
                pscPwrOn = TRUE;
            }

            /* disable the interrupts                                         */
            key = _disable_interrupts();

            /* check if we need to queue this io request or we should initiate*
             * IO here itself of this channel                                 */
            if ((NULL == instHandle->currentActiveChannel) &&
                (I2c_DriverState_PWRM_SUSPEND != instHandle->devStatus))
            {
                /* no transfer is happening in any of the channels in this    *
                 * instance of the driver. So for sure we can initiate the    *
                 * transfer here itself                                       */
                chanHandle->activeIOP = ioPacket;
                instHandle->currentActiveChannel = chanHandle;

                /* restore interrupts                                         */
                _restore_interrupts(key);

                /* call I2c transfer function                                 */
                status = i2cTransfer(instHandle, chanHandle, dataParam, NULL);
            }
            else
            {
                /* restore interrupts                                         */
                _restore_interrupts(key);
            
                /* that means some IO from some channel is in progress we may *
                 * need to queue the request in pending queue for this channel*
                 * one the control comes to the completion of the current IO, *
                 * the queue will be processed and this IO will be programmed *
                 * In the priority based implementation, please note that only*
                 * the channel that was created from the task with high       *
                 * priority wouldbe processed first                           */
                if (I2c_OpMode_POLLED != instHandle->opMode)
                {
                    QUE_put(&(chanHandle->queuePendingList),(Ptr)ioPacket);
                    instHandle->stats.pendingPacket++;
                    status = IOM_PENDING;
                }
                else
                {
                    status = IOM_EBADMODE;
                }
            }
        } while (bFalseWhile);
    }

    /* update the status of the IO packet here                                */
    if (IOM_COMPLETED == status)
    {
        if ((I2c_OpMode_POLLED == instHandle->opMode) && (TRUE == pscPwrOn))
        {
            status = I2c_localLpscOff(instHandle);
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
            /* The submit did not succeed hence power off the module.         */
            I2c_localLpscOff(instHandle);
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
 *  \param    chanp      [IN]    Channel handle
 *  \param    cmd        [IN]    control command given by the application
 *  \param    cmdArgs    [IN]    Optional args required for command execution
 *
 *  \return   None
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *            cmd        must be a valid IOCTL command.
 *            cmdArgs    must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int i2cMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs)
{
    I2c_Object      *instHandle  = NULL;
    Int32            result      = IOM_COMPLETED;
    Int32            retCode     = IOM_COMPLETED;
    Bool             nack        = FALSE;
    I2c_ChanObj     *chanHandle  = NULL;
    Uint32           key         = 0;
    Uint32           bitrate     = 0;
    Uint32           psc         = 0;
    Uint32           clk         = 0;
    Uint32           count       = 0x00;
#ifdef BIOS_PWRM_ENABLE
    Uns              setpoint    = 0x00;
    PWRM_Domain      domain      = PWRM_CPU;
    PWRM_Status      pwrmStatus  = PWRM_SOK;
#endif

#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* check if the input parameters are OK                                   */
    if (NULL == chanp)
    {
        result = IOM_EBADARGS;
    }
    else
    {
        /* ok to check chanp struct members now because we know it's not NULL */
        if (NULL == ((I2c_ChanObj *)chanp)->instHandle)
        {
            result = IOM_EBADIO;
        }
        else
        {
#endif
            instHandle = ((I2c_ChanObj *)chanp)->instHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if (NULL == instHandle)
            {
                result = IOM_EBADARGS;
            }
        }
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    /* power ON the module as some of the IOCTLs may require to configure     *
     * the hardware                                                           */
    if (IOM_COMPLETED == result)
    {
        result =  I2c_localLpscOn(instHandle);
    }

    if (IOM_COMPLETED == result)
    {
        chanHandle = (I2c_ChanObj *)chanp;

        /* Check for the channel state                                        */
        if (chanHandle->channelState != I2c_DriverState_OPENED)
        {
            result = IOM_EBADARGS;
        }
        else
        {
            if (I2c_IOCTL_SET_BIT_RATE == cmd)
            {
                assert(NULL != cmdArgs);

                if (chanHandle->pendingState == TRUE)
                {
                    result = IOM_EBADMODE;
                }
                else
                {
                    key = (Uint32)_disable_interrupts();
                    bitrate = (*((Uint32*)cmdArgs));

                    for (count = 0; count < I2c_NUM_CHANS; count++)
                    {
                        instHandle->chans[count].busFreq = bitrate;
                    }
#ifdef BIOS_PWRM_ENABLE
                    /* calcualte the contraints for this new bitrate          */
                    retCode = I2cCalculateConstraints(instHandle);

                    if (IOM_COMPLETED == retCode)
                    {
                        /* check what clock domain is supported by the device */
                        if (I2c_pllDomain_0 != instHandle->pwrmInfo.pllDomain)
                        {
                            domain = PWRM_PER;
                        } 

                        pwrmStatus = PWRM_getCurrentSetpoint(domain,&setpoint);
                  
                        if (PWRM_SOK == pwrmStatus)
                        {
                            retCode = I2cCalculateClkSettings(
                                         instHandle,
                                         setpoint,
                                         chanHandle->busFreq,
                                         &psc,
                                         &clk);
                        }
                        else
                        {
                            retCode = IOM_EBADMODE;
                        }
                    }
#else
                        /* Calculate the Bitrate and load in Register         */
                        retCode = i2cCalculateClockSettings(
                                      chanHandle,
                                      &psc,
                                      &clk);
#endif
                    if (retCode != 0)
                    {
                        _restore_interrupts(key);
                        result = (Int32) retCode;
                    }
                    else
                    {
                        /* Put the module in reset                            */
                        (instHandle->deviceInfo.baseAddress)->ICMDR =
                            CSL_I2C_ICMDR_IRS_RESETVAL;

                        /* Load the Prescalar value in Register               */
                        (instHandle->deviceInfo.baseAddress)->ICPSC = psc;
                        (instHandle->deviceInfo.baseAddress)->ICCLKH = clk/2u;
                        (instHandle->deviceInfo.baseAddress)->ICCLKL =
                            clk - (instHandle->deviceInfo.baseAddress)->ICCLKH;

                        /* Set own address and bits/byte                      */
                        (instHandle->deviceInfo.baseAddress)->ICOAR =
                            chanHandle->ownAddr;
                        (instHandle->deviceInfo.baseAddress)->ICMDR =
                            chanHandle->numBits & I2c_NUMBITS_MASK;

                        /* Bring module out of reset                          */
                        (instHandle->deviceInfo.baseAddress)->ICMDR |=
                            CSL_I2C_ICMDR_IRS_MASK;

                        /* Set the backward compatibility mode off -
                         * this is critical to get interrupts correctly       */
                        (instHandle->deviceInfo.baseAddress)->ICEMDR &=
                            (~CSL_I2C_ICEMDR_BCM_MASK);

                        _restore_interrupts(key);
                    }
                } /* retVAL                                                   */
            }

            else if (I2c_IOCTL_GET_BIT_RATE == cmd)
            {
                assert(NULL != cmdArgs);

                *((Uint32 *)cmdArgs) = chanHandle->busFreq ;
            }
            else if (I2c_IOCTL_CANCEL_PENDING_IO == cmd)
            {
                if (I2c_OpMode_POLLED != instHandle->opMode)
                {
                    key = _disable_interrupts();
                /* Just set the abort flag to true. The actual abortion of all*
                 * the requests will take place after the current IO has      *
                 * completed from the interrupt handler.This is becoz, abruptly*
                 * halting the current request may cause undefined behaviour  *
                 * on the bus                                                 */ 
                    /* to cancel io                                           */
                    chanHandle->abortAllIo = TRUE;

                    _restore_interrupts(key);
                }
            }
            else if (I2c_IOCTL_BIT_COUNT == cmd)
            {
                assert(NULL != cmdArgs);

                /* reset bitcount before updating                             */
                (instHandle->deviceInfo.baseAddress)->ICMDR &=
                    ~I2c_NUMBITS_MASK;

                /* Set the bit count value                                    */
                (instHandle->deviceInfo.baseAddress)->ICMDR |=
                    (*((Uint32*)cmdArgs) & I2c_NUMBITS_MASK);
            }
            else if (I2c_IOCTL_NACK == cmd)
            {
                assert(NULL != cmdArgs);
                nack = (*((Bool*)cmdArgs));

                if (nack == 1)
                {
                    /* set the nack mode, MSB of ICMDR                    */
                    (instHandle->deviceInfo.baseAddress)->ICMDR =
                        (instHandle->deviceInfo.baseAddress)->ICMDR
                            | CSL_I2C_ICMDR_NACKMOD_MASK;
                }
                else
                {
                    /* disable the nack mode,                             */
                    (instHandle->deviceInfo.baseAddress)->ICMDR =
                        (instHandle->deviceInfo.baseAddress)->ICMDR
                            & ~(CSL_I2C_ICMDR_NACKMOD_MASK);
                }
            }
            else if (I2c_IOCTL_SET_OWN_ADDR == cmd)
            {
                assert(NULL != cmdArgs);

                if (chanHandle->pendingState == TRUE)
                {
                    result = IOM_EBADMODE;
                }
                else
                {
                    chanHandle->ownAddr = *((Uint32 *)cmdArgs);
                    (instHandle->deviceInfo.baseAddress)->ICOAR  =
                        chanHandle->ownAddr;
                }
            }
            else if (I2c_IOCTL_GET_OWN_ADDR == cmd)
            {
                assert(NULL != cmdArgs);

                *((Uint32 *)cmdArgs) = chanHandle->ownAddr;
            }
            else if (I2c_IOCTL_SET_POLLEDMODETIMEOUT == cmd)
            {
                assert(NULL != cmdArgs);

                /* Update the polledModeTimeout value                         */
                instHandle->polledModeTimeout = *((Uint32 *)cmdArgs);
            }
            else if (I2c_IOCTL_DEVICE_SOFT_RESET == cmd)
            {
                /* Perform a soft reset on the I2C module                     */
                key = (Uint32)_disable_interrupts();

                result = i2cSoftReset(chanHandle);

                _restore_interrupts(key);
            }
            else
            {
                result = IOM_EBADMODE;
            }
        }

        /* power of the module                                                */
        retCode = I2c_localLpscOff(instHandle);

        /* We will update the state of the module power off only if the IOCTL *
         * execution is successful.if it is a failure then we need not update *
         * the status of the module switch OFF. but in case of IOCTL success  *
         * and module switch OFF. we will return the status of the module     *
         * switch off so that in case of an failure the application can be    *
         * notified of the same                                               */
        if (IOM_COMPLETED == result)
        {
            result = retCode;
        }
    }
    return (result);
}

/* ========================================================================== */
/*                            LOCAL  FUNCTIONS                                */
/* ========================================================================== */

/*
 *  \brief  function used to transfer the data
 *
 *  This is the function in which a dual data transfer happens.Function
 *  validates input parameters,set the parameter values in to the registers
 *  and starts transmission.
 *
 *  \param  instHandle  [IN]    I2C Driver Instance Handle
 *  \param  chanHandle  [IN]    Channel handle
 *  \param  dataParam   [IN]    Pointer to the Data Source
 *  \param  param       [IN]    Extra paramter for future usage
 *
 * \return  Success  Number of bytes transferred.
 *          Error id in case of error
 *
 *  \enter  instHandle must be a valid pointer and should not be null.
 *          chanHandle must be a valid pointer and should not be null.
 *          dataParam  must be a valid pointer and should not be null.
 *          param      must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
static Int i2cTransfer(I2c_Object    *instHandle,
                       I2c_ChanObj   *chanHandle,
                       I2c_DataParam *dataParam,
                       Ptr            param)
{
    Int32              retVal     = IOM_COMPLETED;
    Int32              retCode    = IOM_COMPLETED;
    Uint32             flags      = 0;

    assert(((NULL != instHandle) &&(NULL != chanHandle) &&(NULL != dataParam)));

    chanHandle->tempICMDRValue = 0;
    flags = dataParam->flags;

    /* To remove the compiler warnings                                        */
    if(param != param)
    {
        param = param;
    }

    /* Check user has not set both read and write flag at same time           */
    if (((flags & I2c_READ) != 0) && ((flags & I2c_WRITE) != 0)&&
        (0 == dataParam->bufLen))
    {
        instHandle->currentActiveChannel = NULL;    
        retVal = IOM_EBADARGS;
    }
    else
    {
        /* Check for Bus Busy if not restarting                               */
        if (((flags & I2c_RESTART) == 0)
            && ((flags & I2c_IGNORE_BUS_BUSY) == 0))
        {
            /* Loop on busy condition                                         */
            if (I2c_OpMode_POLLED == instHandle->opMode)
            {
                /*Check for Bus Busy                                          */
                retCode = i2cWaitForBB(
                            instHandle,
                            I2c_BUS_BUSY_TIMEOUT_CNT,
                            0u);

                if (IOM_COMPLETED != retCode)
                {
                    instHandle->currentActiveChannel = NULL;
                    retVal = IOM_EABORT;
                }
            }
            else
            {
                /*Check for Bus Busy                                      */
                retCode = i2cWaitForBB(
                            instHandle,
                            I2c_BUS_BUSY_TIMEOUT_MSECS,
                            1u);

                if (IOM_COMPLETED != retCode)
                {
                    instHandle->currentActiveChannel = NULL;
                    retVal = IOM_EABORT;
                }
            }
        }
    }

    if (IOM_COMPLETED == retVal)
    {
        I2c_localPrepareForTransfer(chanHandle);

/*======================= EDMA MODE CODE STARTS===============================*/
#ifdef I2c_EDMA_ENABLE
        /* DMA Interrupt Mode of operation                                    */
        if (instHandle->opMode == I2c_OpMode_DMAINTERRUPT)
        {
            (instHandle->deviceInfo.baseAddress)->ICIMR = 0;
            /* Set the current buffer params correctly                        */
            chanHandle->currError = 0;
            chanHandle->currFlags = flags;
            chanHandle->currBuffer = (dataParam->buffer);
            chanHandle->currBufferLen = (dataParam->bufLen);

            /* Enable AL, NACK and AAS interrupts                             */
            (instHandle->deviceInfo.baseAddress)->ICIMR =
                (CSL_I2C_ICIMR_AL_MASK |    /* Arbitration lost               */
                CSL_I2C_ICIMR_NACK_MASK |   /* NACK received/not received     */
                CSL_I2C_ICIMR_AAS_MASK);    /* Addressed as Slave             */

            /* Call this to complete edma configuration and transfer          */
            retVal = (int)I2c_localEdmaTransfer((Ptr)instHandle,flags);

            /* Start bit set here to start transaction                        */
            (instHandle->deviceInfo.baseAddress)->ICMDR |= \
                 chanHandle->tempICMDRValue;

            if (IOM_COMPLETED ==  retVal)
            {
                retVal = IOM_PENDING;
            }
        }
#endif /* I2c_EDMA_ENABLE */
/*====================== EDMA MODE CODE ENDS==================================*/
/*================ INTERUPPT MODE CODE STARTS=================================*/
        if (instHandle->opMode == I2c_OpMode_INTERRUPT)
        {
            /* Set the current buffer params correctly                        */
            chanHandle->currError = 0;
            chanHandle->currFlags = flags;
            chanHandle->currBuffer = (dataParam->buffer);
            chanHandle->currBufferLen = (dataParam->bufLen);

            /* Enable interrupts */
            (instHandle->deviceInfo.baseAddress)->ICIMR =
                (CSL_I2C_ICIMR_AL_MASK |    /* Arbitration lost               */
                CSL_I2C_ICIMR_NACK_MASK |   /* NACK received/not received     */
                CSL_I2C_ICIMR_SCD_MASK |    /* SCD interrupt                  */
                CSL_I2C_ICIMR_AAS_MASK);    /* Addressed as Slave             */

            if ((flags & I2c_WRITE) != NULL)
            {
                /* Transmit ready                                             */
                (instHandle->deviceInfo.baseAddress)->ICIMR |=
                    CSL_I2C_ICIMR_ICXRDY_MASK;

            }
            else if((flags & I2c_READ) != NULL)
            {
                /* Receive ready                                              */
                (instHandle->deviceInfo.baseAddress)->ICIMR |=
                    CSL_I2C_ICIMR_ICRRDY_MASK;
            }
            else
            {
                /* do nothing                                                 */
            }

            /* Start I2C transaction                                          */
            /* Clear IVR before doing a start                                 */
            (instHandle->deviceInfo.baseAddress)->ICIVR =
                CSL_I2C_ICIVR_INTCODE_RESETVAL;

            /* Start bit set here to start transaction                        */
            (instHandle->deviceInfo.baseAddress)->ICMDR |= \
             chanHandle->tempICMDRValue;

            retVal = IOM_PENDING;
        }

 /*================ INTERUPPT MODE CODE ENDS==================================*/
 /*=================== POLLED MODE CODE ENDS==================================*/
        else if (instHandle->opMode == I2c_OpMode_POLLED)
        {
            /* we will take this semaphore to protect the driver from multiple*
             * tasks during the polled mode operation                         */
            if (TRUE == SEM_pend(&(instHandle->syncSem),
                            instHandle->polledModeTimeout))
            {
                retVal = i2cPolledModeTransfer(chanHandle);

#ifdef BIOS_PWRM_ENABLE
                if ((TRUE == instHandle->pscPwrmEnable) && 
                    (TRUE == instHandle->pwrmInfo.ioSuspend))
                {
                    /* set the current active channel as NULL                 */
                    instHandle->currentActiveChannel = NULL;
                    
                    instHandle->pwrmInfo.ioSuspend = FALSE;
                    instHandle->devStatus = I2c_DriverState_PWRM_SUSPEND;

                    /* call the delayed completion function                   */
                    (instHandle->pwrmInfo.delayedCompletionFxn  \
                        [instHandle->pwrmInfo.pwrmEvent])();
                }
#endif
                /* post the semaphore after the processing is complete        */
                SEM_post(&(instHandle->syncSem));
            }
            else
            {
                retVal = IOM_ETIMEOUT;
            }
        }
        else
        {
            /* do nothing                                                     */
        }
/*==================== POLLED MODE CODE ENDS==================================*/
    } /*Bus busy check*/
    return retVal;
}

/*
 *  \brief  function is used to prepare for data transfer
 *
 *  This is the function where a registers are prepared for transfer.Function
 *  validates input parameters,set the parameter values in to the registers.
 *
 *  \param  chanHandle  [IN]    Channel handle
 *
 *  \return None
 *
 *  \enter  chanHandle  must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
Void I2c_localPrepareForTransfer(I2c_ChanObj *chanHandle)
{
    I2c_Object     *instHandle = NULL;
    I2c_DataParam  *dataParam  = NULL;

    assert(NULL != chanHandle);
    /* ok to check chanHandle struct members now because we know it's not NULL*/
    assert(NULL != chanHandle->instHandle);

    instHandle = (I2c_Object *)(chanHandle->instHandle);
    dataParam  = (I2c_DataParam *)chanHandle->activeIOP->addr;
    assert((NULL != instHandle) && (NULL != dataParam));

    /* Set count for data transfer and init mode control register             */
    (instHandle->deviceInfo.baseAddress)->ICCNT =
        (dataParam->bufLen & I2c_BUFFER_MAXLENGTH);

    /* Check flags and set accordingly                                        */
    /* Except for start and stop bit others can be
     * written directly on to the register                                    */
    if ((dataParam->flags & I2c_MASTER)  != 0)
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR |=
            CSL_I2C_ICMDR_MST_MASK;
    }
    else
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &=
            ~CSL_I2C_ICMDR_MST_MASK;
    }
    if ((dataParam->flags & I2c_START)  != 0)
    {
        chanHandle->tempICMDRValue |= CSL_I2C_ICMDR_STT_MASK;
    }
    else
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &= ~CSL_I2C_ICMDR_STT_MASK;
    }
    if ((dataParam->flags & I2c_STOP) != 0)
    {
        chanHandle->tempICMDRValue |= CSL_I2C_ICMDR_STP_MASK;
    }
    else
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &= ~CSL_I2C_ICMDR_STP_MASK;
    }

    if ((dataParam->flags & I2c_WRITE) != 0)
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR |=
            CSL_I2C_ICMDR_TRX_MASK;
    }
    if ((dataParam->flags & I2c_READ) != 0)
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &=
            ~CSL_I2C_ICMDR_TRX_MASK;
    }

    if ((dataParam->flags & I2c_REPEAT) != 0)
    {
        chanHandle->tempICMDRValue |= CSL_I2C_ICMDR_RM_MASK;
    }
    else
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &=
            ~CSL_I2C_ICMDR_RM_MASK;
    }

    /* Write slave address - restrict to 10 bits                              */
    (instHandle->deviceInfo.baseAddress)->ICSAR =
        (dataParam->slaveAddr & I2c_SLAVEADRR_10BIT);

    /* Check if restart required to set the bus busy                          */
    if ((dataParam->flags & I2c_RESTART) != 0)
    {
        (instHandle->deviceInfo.baseAddress)->ICSTR |=
            CSL_I2C_ICSTR_BB_MASK;
        chanHandle->tempICMDRValue |= CSL_I2C_ICMDR_STT_MASK;
    }

    /* To make transaction is in progress                                     */
    chanHandle->pendingState = TRUE;
    chanHandle->currBuffer   = 0;
}

/*
 *  \brief  function is used to transfer data in polled mode
 *
 *  This function is used to transfer data in polled mode.
 *
 *  \param  chanHandle  [IN]    Channel handle
 *
 *  \return DriverTypes_COMPLETED in case of success
 *          DriverTypes_ERROR in case of failure
 *
 *  \enter  chanHandle  must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 *
 */
static Int32 i2cPolledModeTransfer(I2c_ChanObj *chanHandle)
{
    Int32              retCode    = IOM_COMPLETED;
    Int32              retVal     = IOM_COMPLETED;
    volatile Uint32    delay      = 0;
    Uint32             timeCnt    = 0;
    Uint32             status     = 0;
    Uint32             cnt        = 0;
    I2c_Object        *instHandle = NULL;
    Uint32             flags      = 0;
    I2c_DataParam     *dataParam  = NULL;

    assert(NULL != chanHandle);
    /* ok to check chanHandle struct members now because we know it's not NULL*/
    assert(NULL != chanHandle->instHandle);

    instHandle = (I2c_Object *)(chanHandle->instHandle);
    dataParam  = (I2c_DataParam *)chanHandle->activeIOP->addr;

    assert((NULL != instHandle) && (NULL != dataParam));

    flags = dataParam->flags;

    /* Mask all interrupts just to make sure                                  */
    (instHandle->deviceInfo.baseAddress)->ICIMR = 0;

    /* Start bit set here to start transaction                                */
    (instHandle->deviceInfo.baseAddress)->ICMDR |= chanHandle->tempICMDRValue;

    delay = I2c_POLLED_TIMEOUT;

    /* wait for start bit to get generated only when start bit set            */
    if ((flags & I2c_START)  != 0)
    {
        while ((((instHandle->deviceInfo.baseAddress)->ICSTR
            & CSL_I2C_ICSTR_BB_MASK) != CSL_I2C_ICSTR_BB_MASK)
            && (delay != 0))
        {
            delay--;
        }
    }

    if (0 == delay)
    {
        /* timeout has occured due to bus busy                                */
        retCode = IOM_ETIMEOUT;
    }

    /* set this value to exit from I2C when the delay expires as              *
     * stops responding hardware                                              */
    delay = I2c_POLLED_TIMEOUT;
    timeCnt = CLK_gethtime();

    while (cnt < dataParam->bufLen)
    {
        if ((FALSE == i2cCheckTimeOut(timeCnt,instHandle->polledModeTimeout))
            && (IOM_COMPLETED == retCode))
        {
            delay --;

            if (0 == delay)
            {
                retCode = IOM_ETIMEOUT;
            }

            if ((flags & I2c_WRITE) != NULL)
            {
                /* Transmit                                                   */
                if (((instHandle->deviceInfo.baseAddress)->ICSTR
                    & CSL_I2C_ICSTR_ICXRDY_MASK) != 0)
                {
                    (instHandle->deviceInfo.baseAddress)->ICDXR =
                        (Uint32)*(dataParam->buffer);
                    ++(dataParam->buffer);
                    ++cnt;
                    delay = I2c_POLLED_TIMEOUT;
                }
            }
            else
            {
                /* Receive                                                    */
                if (((instHandle->deviceInfo.baseAddress)->ICSTR
                    & CSL_I2C_ICIMR_ICRRDY_MASK) != 0)
                {
                    *(dataParam->buffer) =
                       (Uint8)(instHandle->deviceInfo.baseAddress->ICDRR);
                    ++(dataParam->buffer);
                    ++cnt;
                    delay = I2c_POLLED_TIMEOUT;
                }
            }
            /* Check for Arbitration loss, transmit underflow and
             * receive overflow errors                                        */
            status = (instHandle->deviceInfo.baseAddress)->ICSTR;

            if ((status & CSL_I2C_ICSTR_AL_MASK) != 0)
            {
                (instHandle->deviceInfo.baseAddress)->ICSTR |=
                    CSL_I2C_ICSTR_AL_MASK;
                retCode = IOM_ENOPACKETS;
                instHandle->stats.arbLossError++;
            }
            else if ((status & CSL_I2C_ICSTR_RSFULL_MASK) != 0)
            {
                (instHandle->deviceInfo.baseAddress)->ICSTR |=
                    CSL_I2C_ICSTR_RSFULL_MASK;
                retCode = IOM_EOF;
                instHandle->stats.overFlowError++;
            }
            else /* Check for NACK error                                      */
            {
                if ((status & CSL_I2C_ICSTR_NACK_MASK) != 0)
                {
                    (instHandle->deviceInfo.baseAddress)->ICSTR |=
                        CSL_I2C_ICSTR_NACK_MASK;
                    if ((flags & I2c_IGNORE_NACK) != 0)
                    {
                        retCode = IOM_COMPLETED;
                    }
                    else
                    {
                        retCode = IOM_ETIMEOUT;
                        instHandle->stats.nackError++;
                    }
                }
            }
            /* check for cancelling IO                                        */
            if (TRUE == chanHandle->abortAllIo)
            {
                chanHandle->abortAllIo = FALSE;

                retCode = IOM_EBADIO;
            }/* Cancel IO                                                     */
        }
        else
        {
            if (retCode == IOM_COMPLETED)
            {
                retCode = IOM_ETIMEOUT;
                instHandle->stats.timeoutError++;
            }
            break;
        }
    } /* While number of byte to be transmitted/received loop                 */

    /* Wait for the current IO to be completed                                */
    retVal = I2c_localEnsureIsReadyforNext(instHandle);

    if(retVal != IOM_COMPLETED)
    {
        retCode = retVal;
    }

    /* Updating the transmit and receive stats                                */
    if ((flags & I2c_WRITE) != NULL)
    {
        instHandle->stats.txBytes += cnt;
    }
    else
    {
        instHandle->stats.rxBytes += cnt;
    }

    if (IOM_COMPLETED != retCode)
    {
        i2cCancelIO(instHandle,retCode);
        instHandle->stats.timeoutError++;
        retVal = retCode;
    }

    instHandle->currentActiveChannel = NULL;
    chanHandle->pendingState = FALSE;

    return retVal;
}

/*
 *  \brief  function is used get the next channel
 *
 *  This function is used to get the next highest priority channel. Also it
 *  checks for any pending data in that channel.
 *
 *  \param  instHandle  [IN]    handle to the I2C instance
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
Void I2c_localGetNextChannel(I2c_Object *instHandle, I2c_ChanObj **pChanHandle)
{
    Uint32 counter;
    Uint32 chanIndexWithMaxPri = I2c_NUM_CHANS;
    Uint32 lastFoundMaxPri = 0;

    assert(NULL != instHandle);
    assert(NULL != pChanHandle);

    for(counter=0; counter<I2c_NUM_CHANS; counter++)
    {
        if ((I2c_DriverState_OPENED == instHandle->chans[counter].channelState)
         && (lastFoundMaxPri < instHandle->chans[counter].taskPriority))
           {
            if(FALSE ==
                QUE_empty(&(instHandle->chans[counter].queuePendingList)))
            {
                lastFoundMaxPri = instHandle->chans[counter].taskPriority;
                chanIndexWithMaxPri = counter;
            }
        }
    }

    if (I2c_NUM_CHANS != chanIndexWithMaxPri)
    {
        *pChanHandle = &(instHandle->chans[chanIndexWithMaxPri]);
    }
    else
    {
        *pChanHandle = NULL;
    }
}



/*
 * \brief   I2c driver interrupt handler
 *
 * \Note:   count has been added to this function as an exit condition for
 *          continuous interrupt conditions which may arise because of any
 *          hardware issue.
 *
 * \param   instHandle [IN] pointer to the I2c driver object
 *
 * \return  None
 *
 * \enter   instHandle  must be a valid pointer and should not be null.
 *
 * \leave   Not Implemented.
 */
static Void i2cIntrHandler(I2c_Object *instHandle)
{
    volatile Uint32     intStatus  = 0;
#ifdef I2c_EDMA_ENABLE
    Uint32              edmaChan   = 0x00;
#endif
    Uint32              count      = 0;
    I2c_ChanObj        *chanHandle = NULL;
    I2c_DataParam      *dataParam  = NULL;

    assert(NULL != instHandle);

    /* Read Status from ICIVR register                                        */
    intStatus = (instHandle->deviceInfo.baseAddress)->ICIVR;
    chanHandle = instHandle->currentActiveChannel;

    while ((0 != intStatus) && (count < 10u) && (NULL != chanHandle))
    {
        switch (intStatus)
        {
            case CSL_I2C_ICIVR_INTCODE_NONE:
                /* No interrupt - Ideally, control should not come here.      */
                break;

            /****************** Arbitration loss Interrupt*********************/
            case CSL_I2C_ICIVR_INTCODE_AL:
                /* increment the statistics to indicate arbitration loss      */
                instHandle->stats.arbLossError++;

                /* complete the packet with error status                      */
                chanHandle->currError = IOM_ENOPACKETS;

                /* cancel the current IOP                                     */
                i2cCancelIO(instHandle,chanHandle->currError);

                /* complete the current IOP processing                        */
                if (I2c_OpMode_INTERRUPT == instHandle->opMode)
                {
                    i2cCompleteIOInIsr(instHandle);
                }
#ifdef I2c_EDMA_ENABLE
                else
                {
                    if (IOM_INPUT == chanHandle->mode)
                    {
                        edmaChan =
                            instHandle->deviceInfo.rxDmaEventNumber;
                    }
                    else
                    {
                        edmaChan =
                            instHandle->deviceInfo.txDmaEventNumber;
                    }

                    EDMA3_DRV_disableLogicalChannel(
                        instHandle->hEdma,
                        edmaChan,
                        EDMA3_DRV_TRIG_MODE_EVENT);

                    chanHandle->activeIOP->status = IOM_EBADIO;

                    I2c_completeIOedmaCallback(instHandle);
                }
#endif
                /*disable interrupt avoid any other interrupts getting raised */
                (instHandle->deviceInfo.baseAddress)->ICIMR =
                    CSL_I2C_ICIMR_RESETVAL;
                break;

            /************************ NACK interrupt***************************/
            case CSL_I2C_ICIVR_INTCODE_NACK:
                /* check if the NACK is to be ignored or to be processed      */
                if ((chanHandle->currFlags & I2c_IGNORE_NACK) != 0)
                {
                    chanHandle->currError = IOM_COMPLETED;
                }
                else
                {
                    instHandle->stats.nackError++;

                    if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
                    {
                        (instHandle->deviceInfo.baseAddress)->ICMDR |=
                            CSL_I2C_ICMDR_STP_MASK;
                    }

                    if ((NULL != chanHandle) && (NULL != chanHandle->activeIOP))
                    {
                        /* update the statistics for the nack error           */
                        chanHandle->currError = IOM_ETIMEOUT;

                        /* cancel the current IOP                             */
                        i2cCancelIO(instHandle,chanHandle->currError);

                        if (I2c_OpMode_INTERRUPT == instHandle->opMode)
                        {
                            /* complete the current IOP                       */
                            i2cCompleteIOInIsr(instHandle);
                        }
#ifdef I2c_EDMA_ENABLE                  
                        else
                        {
                            if (IOM_INPUT == chanHandle->mode)
                            {
                                edmaChan =
                                    instHandle->deviceInfo.rxDmaEventNumber;
                            }
                            else
                            {
                                edmaChan =
                                    instHandle->deviceInfo.txDmaEventNumber;
                            }

                            EDMA3_DRV_disableLogicalChannel(
                                instHandle->hEdma,
                                edmaChan,
                                EDMA3_DRV_TRIG_MODE_EVENT);

                            chanHandle->activeIOP->status = IOM_EBADIO;

                            I2c_completeIOedmaCallback(instHandle);
                        }
#endif
                    }
                }
                /*disable interrupt avoid any interrupts in getting raised    */
                (instHandle->deviceInfo.baseAddress)->ICIMR =
                     CSL_I2C_ICIMR_RESETVAL;
                break;

            /********************** Register Access ready**********************/
            case CSL_I2C_ICIVR_INTCODE_ARDY:
                /* We do not need to handle this interrupt in this driver     *
                 * Disable this interrupt enable bit                          */
                (instHandle->deviceInfo.baseAddress)->ICIMR &=
                    ~I2c_ICIVR_INTCODE_ARDY;
                break;

            /****************** Receive Data ready interrupt*******************/
            case CSL_I2C_ICIVR_INTCODE_ICRRDY:
                /* if the receive buffer is exhausted                         */
                if (0 != chanHandle->currBufferLen)
                {
                    /* if the receive buffer still has bytes remaining to be  *
                     * received then copy the data to the receive buffer.     */
                    /* copy the data to the buffer & update the buffer pointer*/
                    *chanHandle->currBuffer++ =
                        (Uint8)((instHandle->deviceInfo.baseAddress)->ICDRR);
                
                    /* Decrement the recieve buffer length                    */
                    --chanHandle->currBufferLen;
                
                    /* increment the statistics                               */
                    instHandle->stats.rxBytes++;

                    if ((0 == chanHandle->currBufferLen) || 
                        (TRUE == chanHandle->abortAllIo))
                    {
                        if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
                        {
                            if (((instHandle->deviceInfo.baseAddress)->ICMDR &
                                CSL_I2C_ICMDR_RM_MASK) == CSL_I2C_ICMDR_RM_MASK)
                            {
                                /* Repeat mode is ON                          */
                                (instHandle->deviceInfo.baseAddress)->ICMDR |=
                                    CSL_I2C_ICMDR_STP_MASK;
                            }
                            else
                            {
                                if (((instHandle->deviceInfo.baseAddress)->ICMDR &
                                    CSL_I2C_ICMDR_STP_MASK) != CSL_I2C_ICMDR_STP_MASK)
                                {
                                    (instHandle->deviceInfo.baseAddress)->ICMDR |=
                                        CSL_I2C_ICMDR_STP_MASK;
                                }
                            }
                        }
                        else
                        {
                            /* set a NACK bit to indicate end of receive      */
                            (instHandle->deviceInfo.baseAddress)->ICMDR |=
                                CSL_I2C_ICMDR_NACKMOD_MASK;
                        }
                    }
                }
                break;
            /****************Transmit Data ready interrupt*********************/
            case CSL_I2C_ICIVR_INTCODE_ICXRDY:
                /* check if the TX buffer is not empty and has some data to   *
                 * transmit                                                   */
                if (0 != chanHandle->currBufferLen)
                {
                    /* write the transmit data                                */
                    (instHandle->deviceInfo.baseAddress)->ICDXR =
                        (Uint8)*(chanHandle->currBuffer++);

                    /* Decrement the Buffer length                            */
                    --chanHandle->currBufferLen;

                    /* update the stats                                       */
                    instHandle->stats.txBytes++;

                    /* if abort is set process it                             */
                    if (TRUE == chanHandle->abortAllIo)
                    {
                        /* Set a nack for closing the transaction             */
                        (instHandle->deviceInfo.baseAddress)->ICMDR |=
                            (CSL_I2C_ICMDR_NACKMOD_MASK);

                        /* if the device is in master mode we need to set the *
                         * stop bit also                                      */
                        if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
                        { 
                            (instHandle->deviceInfo.baseAddress)->ICMDR |=
                                CSL_I2C_ICMDR_STP_MASK;
                        }
                    }
                }

                if ((0 == chanHandle->currBufferLen) && (NULL != chanHandle->activeIOP))
                {
                    dataParam  = (I2c_DataParam *)chanHandle->activeIOP->addr;

                    if (I2c_CommMode_SLAVE == chanHandle->masterOrSlave)
                    {
                        /* Buffer is exhausted release the data line          */
                        (instHandle->deviceInfo.baseAddress)->ICDXR =
                            I2c_SLAVE_TX_DATA;
                    
                         chanHandle->currError = IOM_EOF;
                    }
                    else
                    {
                        if ((NULL != dataParam) && 
                            (I2c_STOP != (I2c_STOP & dataParam->flags)))
                        {
                            /* No stop command is issued hence we need to     *
                             * complete the IOP here                          */
                            i2cCompleteIOInIsr(instHandle);                            
                        }
                    }
                }
                break;
            /******************* Stop condition detected***********************/
            case CSL_I2C_ICIVR_INTCODE_SCD:
                if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
                {
                    i2cCompleteIOInIsr(instHandle);
                }
                else
                {
                    /* clear the NACK mode bit                                */
                    (instHandle->deviceInfo.baseAddress)->ICMDR &=
                        (~CSL_I2C_ICMDR_NACKMOD_MASK);

                    if (IOM_EOF == chanHandle->currError)
                    {
                        i2cCancelIO(instHandle,chanHandle->currError);
                    }

                    if (NULL != chanHandle->activeIOP)
                    {
                        if (I2c_OpMode_INTERRUPT == instHandle->opMode)
                        {
                            /* complete the current IOP                       */
                            i2cCompleteIOInIsr(instHandle);
                        }
#ifdef I2c_EDMA_ENABLE
                        else
                        {
                            /* DMA mode of operation                          */
                            if (IOM_INPUT == chanHandle->mode)
                            {
                                edmaChan =
                                    instHandle->deviceInfo.rxDmaEventNumber;
                            }
                            else
                            {
                                edmaChan =
                                    instHandle->deviceInfo.txDmaEventNumber;
                            }

                            EDMA3_DRV_disableLogicalChannel(
                                instHandle->hEdma,
                                edmaChan,
                                EDMA3_DRV_TRIG_MODE_EVENT);

                            chanHandle->activeIOP->status = IOM_COMPLETED;

                            /* complete the current IOP                       */
                            I2c_completeIOedmaCallback(instHandle);
                        }
#endif
                    }
                }
                break;

            /***************** Addressed as slave interrupt********************/
            case CSL_I2C_ICIVR_INTCODE_AAS:
                if (I2c_CommMode_MASTER == chanHandle->masterOrSlave)
                {
                    i2cCancelIO(instHandle,chanHandle->currError);

                    if(I2c_OpMode_INTERRUPT == instHandle->opMode)
                    {
                        i2cCompleteIOInIsr(instHandle);
                    }
#ifdef I2c_EDMA_ENABLE
                    else
                    {
                        /* DMA mode of operation                              */
                        if (IOM_INPUT == chanHandle->mode)
                        {
                            edmaChan =
                                instHandle->deviceInfo.rxDmaEventNumber;
                        }
                        else
                        {
                            edmaChan =
                                instHandle->deviceInfo.txDmaEventNumber;
                        }

                        EDMA3_DRV_disableLogicalChannel(
                            instHandle->hEdma,
                            edmaChan,
                            EDMA3_DRV_TRIG_MODE_EVENT);

                        chanHandle->activeIOP->status = IOM_EBADIO;

                        I2c_completeIOedmaCallback(instHandle);
                    }
#endif
                }
                else
                {
                    /* now the device is addresed as slave in slave mode.Hence*
                     * it is a valid interrupt.                               */
                    instHandle->deviceInfo.baseAddress->ICIMR &=
                        (~(CSL_I2C_ICIMR_AAS_MASK));

                    /* clear the SCD interrupts from the status register      */
                    instHandle->deviceInfo.baseAddress->ICSTR |=
                        (CSL_I2C_ICSTR_SCD_MASK);

                    /* enable the SCD interrupt                               */
                    instHandle->deviceInfo.baseAddress->ICIMR |=
                        (CSL_I2C_ICIMR_SCD_MASK);
                }
                break;
            default:
                /* Wrong interrupt - Ideally, control should not come here    */
                count++;
                break;
        }

        /* Read Status from ICIVR register                                    */
        intStatus = (instHandle->deviceInfo.baseAddress)->ICIVR;
        chanHandle = instHandle->currentActiveChannel;
    }
}

/*
 * \brief   function is used after the completion of ISR
 *
 * This function is called after Interrupt routine is proccessed out.
 * This functions ensure whether driver is ready for next operation or not.
 * Also it gets the next available channel to be processed for data transfer.
 *
 * \param   instHandle [IN] pointer to the i2c driver object
 *
 * \return  None
 *
 * \enter   instHandle  must be a valid pointer and should not be null.
 *
 * \leave   Not Implemented.
 */
static Void i2cCompleteIOInIsr(I2c_Object *instHandle)
{
    I2c_ChanObj    *chanHandle = NULL;
    IOM_Packet     *ioPacket   = NULL;
#ifdef BIOS_PWRM_ENABLE
    Uint32          count      = 0x00;
#endif
    Int32           status     = IOM_COMPLETED;
    Int32           retVal     = IOM_COMPLETED;

    assert(NULL != instHandle);

    chanHandle = instHandle->currentActiveChannel;

    assert(NULL != chanHandle);

    (instHandle->deviceInfo.baseAddress)->ICIMR = CSL_I2C_ICIMR_RESETVAL;

    /* update the size of the actual buffer transferred                       */
    chanHandle->activeIOP->size -= chanHandle->currBufferLen;

    status = I2c_localEnsureIsReadyforNext(instHandle);

    if (TRUE == chanHandle->abortAllIo)
    {
        /* All the IO is to be aborted ; however if bus errors set status so  */
        if (IOM_COMPLETED != status)
        {
            chanHandle->activeIOP->status = status;
        }
        else
        {
            chanHandle->activeIOP->status = IOM_ABORTED;
        }

        chanHandle->abortAllIo = FALSE;

        /* Power OFF the module                                               */
        retVal = I2c_localLpscOff(instHandle);

        if (NULL != chanHandle->cbFxn)
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, chanHandle->activeIOP);
        }

        /* we will ensure that all the IOPs will be aborted                   */
        while (TRUE != QUE_empty(&(chanHandle->queuePendingList)))
        {
            ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));

            assert(NULL != ioPacket);

            instHandle->stats.pendingPacket--;
            ioPacket->status = IOM_ABORTED;
            ioPacket->size = 0;

            /* Power OFF the module                                           */
            retVal = I2c_localLpscOff(instHandle);

            if ((NULL != chanHandle->cbFxn) && (NULL != chanHandle->cbArg))
            {
                /* Invoke Application callback for this channel               */
                (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg,ioPacket);
            }
        }
    }
    else
    {
        if (0 != chanHandle->currError)
        {
            chanHandle->activeIOP->status = chanHandle->currError;
        }
        else
        {
            /* There are not channel errors; But just update the status of    *
             * bus being ready for the next IOP                               */
            chanHandle->activeIOP->status = status;
        }

        /* call the power off function to reduce the depedencies              */
        retVal = I2c_localLpscOff(instHandle);

        if (IOM_COMPLETED == chanHandle->activeIOP->status)
        {
            chanHandle->activeIOP->status = retVal;
        }

        /* call the application completion callback function registered       *
         * with us during opening of the channel                              */
        if (NULL != chanHandle->cbFxn)
        {
            /* Invoke Application callback for this channel                   */
            (*chanHandle->cbFxn)((Ptr)chanHandle->cbArg, chanHandle->activeIOP);
        }
    }


    /* There is no transaction for now.It will be set again when there is     *
     * actual transaction is to be started                                    */
    chanHandle->pendingState = FALSE;
    chanHandle->activeIOP = NULL;
    chanHandle->currError = 0;

#ifdef BIOS_PWRM_ENABLE
    if (TRUE != instHandle->pwrmInfo.ioSuspend)
    {
#endif
        I2c_loadPendedIops(instHandle);
#ifdef BIOS_PWRM_ENABLE
    }
    else
    {
        if (TRUE == instHandle->pscPwrmEnable)
        {
            if (((PWRM_GOINGTOSLEEP == instHandle->pwrmInfo.pwrmEvent) ||
                  (PWRM_GOINGTODEEPSLEEP == instHandle->pwrmInfo.pwrmEvent)) &&
                       (0 != instHandle->instNum))
            {
                /* reduce the dependency count                                */
                status = (Int32)PWRM_getDependencyCount(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId,
                             &count);

                instHandle->pwrmInfo.dependencyCount = count;
                
                if (PWRM_SOK == status)
                {
                    while (count > 0)
                    {
                        status =  (Int32)PWRM_releaseDependency(
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
            instHandle->devStatus = I2c_DriverState_PWRM_SUSPEND;

            /* call the delayed completion function                           */
            (instHandle->pwrmInfo.delayedCompletionFxn  \
                [instHandle->pwrmInfo.pwrmEvent])();
        }        
    }
#endif    
}

/**
 * \brief   Function used to ensure channel is ready for next operation
 *
 *          This functions ensure whether driver is ready for next operation or
 *          not.Also it checks for bus busy and wait till stop bit is generated.
 *
 * \param   instHandle [IN] pointer to the i2c driver object
 *
 * \return  None
 *
 * \enter   instHandle  must be a valid pointer and should not be null.
 *
 * \leave   Not Implemented.
 */
Int32 I2c_localEnsureIsReadyforNext(I2c_Object *instHandle)
{
    Uint32           counter   = 0;
    Uint32           delay     = 0;
    I2c_DataParam   *dataParam = NULL;
    Int32            status    = IOM_COMPLETED;

    assert(NULL != instHandle);

     dataParam =
         (I2c_DataParam *)instHandle->currentActiveChannel->activeIOP->addr;

    assert(NULL != dataParam);

    /* If Bus is Busy after successful transmission                           *
     * then check for nack errors                                             */
    if (((instHandle->deviceInfo.baseAddress)->ICSTR
        & CSL_I2C_ICSTR_BB_MASK) == CSL_I2C_ICSTR_BB_MASK)
    {
        /* Running this check two times for making sure that I2C              *
         * bus free before exiting to app                                     */
        for (counter = 0; counter <= 1u; counter ++)
        {
            delay = I2c_POLLED_TIMEOUT;

            /* wait till stop bit is generated                                */
            while ((((instHandle->deviceInfo.baseAddress->ICSTR
                & CSL_I2C_ICSTR_BB_MASK) != 0)
                && ((instHandle->deviceInfo.baseAddress->ICSTR
                & CSL_I2C_ICSTR_ARDY_MASK) == 0))
                && (delay != 0))
            {
                delay--;
            }

            /* set the status as timerout, and proceed to check if further we *
             * can still gain access                                          */
            if (0 == delay)
            {
                status = IOM_ETIMEOUT;
            }

            if (FALSE != (dataParam->flags & I2c_STOP))
            {
                delay = I2c_POLLED_TIMEOUT;
                /* wait till Master bit goes low                              */
                while (((instHandle->deviceInfo.baseAddress->ICMDR
                    & CSL_I2C_ICMDR_MST_MASK) != FALSE)
                     && (delay != 0))
                {
                    delay--;
                }

                if (0 == delay)
                {
                    status = IOM_ETIMEOUT;
                }
                else
                {
                /* MST bit has been reset. Check if the bus busy error was   *
                 * indicated in the status. If it was indeed set and the MST *
                 * bit is reset then we need to check bus busy again just to *
                 * be sure if bus us fine                                    */
                    if ((instHandle->deviceInfo.baseAddress->ICSTR
                        & CSL_I2C_ICSTR_BB_MASK) == CSL_I2C_ICSTR_BB_MASK)
                    {
                        status = IOM_ETIMEOUT;
                    }
                    else
                    {
                        status = IOM_COMPLETED;
                    }
                }
            }
            else
            {
                delay = I2c_POLLED_TIMEOUT;
                /* wait till ARDY bit is set                                  */
                while ((((instHandle->deviceInfo.baseAddress)->ICSTR
                    & CSL_I2C_ICSTR_ARDY_MASK) == 0)
                    && (delay != 0))
                {
                    delay--;
                }

                if(0 == delay)
                {
                    status = IOM_ETIMEOUT;
                }
                else
                {
                    /* since STOP condition is not intended to be sent, bus   *
                     * remains busy and hence we can return IOM_COMPLETED     */
                     status = IOM_COMPLETED;
                }
            }
        }
    }
    else if (((instHandle->deviceInfo.baseAddress)->ICMDR & 
                CSL_I2C_ICMDR_MST_MASK) == CSL_I2C_ICMDR_MST_MASK)
    {
        if (I2c_STOP == (dataParam->flags & I2c_STOP))
        {
            delay = I2c_POLLED_TIMEOUT;
            /* wait till Master bit goes low                              */
            while (((instHandle->deviceInfo.baseAddress->ICMDR
                & CSL_I2C_ICMDR_MST_MASK) == CSL_I2C_ICMDR_MST_MASK)
                && (delay != 0))
            {
                delay--;
            }
        
            if (0 == delay)
            {
                status = IOM_ETIMEOUT;
            }                
        }
    }
    else
    {
        /* Do nothing */
    }
    /* Mask off interrupts                                                    */
    (instHandle->deviceInfo.baseAddress)->ICIMR = 0;

    /* Since the STOP bit generation was set in ICMDR and it should
     * have been accomplished, reset generation of the STOP bit here.
     */
    if(CSL_I2C_ICMDR_STP_MASK & 
        (instHandle->deviceInfo.baseAddress)->ICMDR)
    {
        (instHandle->deviceInfo.baseAddress)->ICMDR &=
            (~(CSL_I2C_ICMDR_STP_MASK));
    }

    return status;
}

/*
 *  \brief  The local routine Check for Bus Busy
 *
 *  \param  instHandle    [IN]  pointer to I2c driver object
 *  \param  waitCount     [IN]  Count to wait when bus is busy
 *  \param  sleepAllowed  [IN]  Verify whether sleep allowed while waiting
 *
 *  \return Bus busy or NOT
 *  \enter  instHandle   must be a valid pointer and should not be null.
 *          waitCount    must be a valid count.
 *          sleepAllowed must be a valid flag, either 0 or 1.
 *
 *  \leave  Not Implemented.
 */
static Int i2cWaitForBB(const  I2c_Object *instHandle,
                        Uint32             waitCount,
                        Uint32             sleepAllowed)
{
    volatile Uint32 countdown = waitCount;
    Int             status    = IOM_COMPLETED;

    assert(NULL != instHandle);

    while (CSL_I2C_ICSTR_BB_MASK == ((instHandle->deviceInfo.baseAddress)->ICSTR
        & CSL_I2C_ICSTR_BB_MASK))
    {
        if (countdown != 0)
        {
            if (sleepAllowed != 0)
            {
                /* Wait for 1 milliSeconds so that bus is free                */
                TSK_sleep(1u);
            }
        }
        else
        {
            SYS_printf("\ni2cWaitForBB: Bus is busy\n");

            status = IOM_EABORT;
            break;
        }
        --countdown;/* Decrement the Countdown                                */
    }
    return status;
}


/**
 * \brief   Cancel the current pending IO operations
 *
 *  This function is used to cancel the pending IO operations.
 *
 *  \param  instHandle  [IN]  pointer to the I2c driver instance object
 *  \param  retCode     [IN]  return code
 *
 *  \return None
 *
 *  \enter  instHandle   must be a valid pointer and should not be null.
 *          retCode      must be a valid return code.
 *
 *  \leave  Not Implemented.
 */
static Void i2cCancelIO(I2c_Object *instHandle,Int retCode)
{
    /* local variables to store register contents                             */
    Uint32 psc               = 0;
    Uint32 clkh              = 0;
    Uint32 clkl              = 0;
    Uint32 oar               = 0;
    Uint32 counter           = 0;
    volatile Uint32 icmdr    = 0;
    volatile Uint32 delay    = I2c_POLLED_TIMEOUT;
    volatile Uint8  buffer   = 0x00;

    assert(NULL != instHandle);

    /* Store the register values into temporary variables                     */
    icmdr = (instHandle->deviceInfo.baseAddress)->ICMDR & I2c_VALUE_RESTORE;
    psc   = (instHandle->deviceInfo.baseAddress)->ICPSC;
    clkh  = (instHandle->deviceInfo.baseAddress)->ICCLKH;
    clkl  = (instHandle->deviceInfo.baseAddress)->ICCLKL;
    oar   = (instHandle->deviceInfo.baseAddress)->ICOAR;

    /* Wait for read/write to complete only when there is no I2C bus error    */
    if ((IOM_ETIMEOUT == retCode)
        && (((instHandle->deviceInfo.baseAddress)->ICSTR
                & CSL_I2C_ICSTR_BB_MASK) != 0))
    {
        for (counter=0; counter < 1u; counter++)
        {
            if (((instHandle->deviceInfo.baseAddress)->ICMDR
                & CSL_I2C_ICMDR_TRX_MASK) == 0)
            {
                while ((((instHandle->deviceInfo.baseAddress)->ICSTR
                    & CSL_I2C_ICIMR_ICRRDY_MASK) == 0)
                        && (delay != 0))
                {
                    delay--;
                }
            }
        }
    }

    /* Put the module in reset                                                */
    (instHandle->deviceInfo.baseAddress)->ICMDR =
        CSL_I2C_ICMDR_IRS_RESETVAL;

    /* Wait till status register reads its default value                      */
    delay = I2c_POLLED_TIMEOUT;

    /** wait till stop bit is generated                                       */
    while (((instHandle->deviceInfo.baseAddress)->ICSTR
        != CSL_I2C_ICSTR_RESETVAL)
            && (delay != 0))
    {
        delay--;
    }

    /* Load the Counter register with zero value                              */
    (instHandle->deviceInfo.baseAddress)->ICCNT = CSL_I2C_ICCNT_RESETVAL;

    /* Restore the value from temporary variables to register                 */
    (instHandle->deviceInfo.baseAddress)->ICPSC   = psc;
    (instHandle->deviceInfo.baseAddress)->ICCLKH  = clkh;
    (instHandle->deviceInfo.baseAddress)->ICCLKL  = clkl;
    (instHandle->deviceInfo.baseAddress)->ICOAR   = oar;

    /* Bring module out of reset                                              */
    (instHandle->deviceInfo.baseAddress)->ICMDR   = icmdr;

    /*  Set the backward compatibility mode off -this is critical to get      *
     *  interrupts correctly                                                  */
    (instHandle->deviceInfo.baseAddress)->ICEMDR &=
        (~CSL_I2C_ICEMDR_BCM_MASK);
}

/*!
 *  \brief  Register interrrupt with BIOS and enable it
 *
 *  \param  instNum     Instance number
 *  \param  intNum      Interrupt number
 *  \param  isrFunc     Function Pointer to ISR function
 *  \param  i2cObj      Handle to be passed to ISR function
 *
 *  \return void
 *
 *  \enter  instNum   must be a valid instance number
 *          intNum    must be a valid hardware interrupt number
 *          isrFunc   must be a valid pointer to ISR function and should
 *                    not be  null.
 *          i2cObj    must be valid pointer to I2c instance & should not be null
 *
 *  \leave  Not Implemented.
 */
static Void i2cRegisterIntrHandler(Uint32      instNum,
                                   Uint32      intNum,
                                   I2c_isr     isrFunc,
                                   Ptr         i2cObj)
{

    I2c_Object     *instHandle = NULL;
    ECM_Attrs       ecmattrs   = ECM_ATTRS;

    assert((NULL != isrFunc) && (NULL != i2cObj)
        && (CSL_I2C_PER_CNT > instNum));
    assert((127u >= intNum) && (4 <= intNum));
    /* To remove compiler warnings                                            */
    instNum = instNum;

    instHandle = (I2c_Object *)i2cObj;
    assert(NULL != instHandle);

    /* Mapping the eventid to the ECM Dispatch of the DSP/BIOS                */
    ecmattrs.unmask = 1u;

    /*setup arg such that isr will get 'chan' as arg                          */
    ecmattrs.arg = (Arg)instHandle;

    ECM_dispatchPlug(intNum, (ECM_Fxn)isrFunc, &ecmattrs);

    /* Enabling the event                                                     */
    ECM_enableEvent(intNum);

    /* Enabling the HWI_ID                                                    */
    C64_enableIER(1 << instHandle->hwiNumber);
}

/*
 *  \brief  Un-Register interrrupt with BIOS and disable it
 *
 *  \param  instNum      Instance number
 *  \param  intNum       Interrupt number
 *
 *  \return None
 *
 *  \enter  instNum   must be a valid instance number
 *          intNum    must be a valid hardware interrupt number
 *
 *  \leave  Not Implemented.
 */
static void i2cUnregisterIntrHandler(Uint32 instNum, Uint32 intNum)
{
    /* To remove the compiler warning                                         */
    instNum = instNum;

    ECM_disableEvent(intNum);
}

/*
 * \brief   As per I2C specs the following formulas provide prescalar and
 *          low/high divider values
 *
 *          input clk --> PSC Div -----------> ICCL/H Div --> output clock
 *                         module clk
 *
 *          output clk = module clk / (PSC + 1) [ (ICCL + d) + (ICCH + d) ]
 *          Thus,ICCL = ICCH = clk = input clk / ( (psc +1) * output clk * 2 );
 *
 *          where if PSC == 0, d = 7,
 *                if PSC == 1, d = 6
 *                if PSC > 1 , d = 5
 *
 *  \param   instHandle   [IN]   Pointer to I2c handle
 *  \param   psc   [IN]   Pointer that points to prescalar value
 *  \param   clock [IN]   Pointerst that points to the calculated clock value
 *
 *  \return  success or negative error code
 *
 *  \enter  chanHandle  must be a valid pointer and should not be null
 *          psc         must be a valid pointer and should not be null
 *          clock       must be a valid pointer and should not be null
 *          eb          if null raise error and aborts the program, if not null
 *                      and valid raises error but shall allow continuation of
 *                      execution
 *
 *  \leave  Not Implemented.
 */

static Int i2cCalculateClockSettings(const I2c_ChanObj *chanHandle,
                                     Uint32            *psc,
                                     Uint32            *clock)
{
    Uint32     prescalar   = 0;
    Uint32     Dvalue      = 0;
    Int        clk         = 0;
    Int        retVal      = IOM_COMPLETED;
    I2c_Object *instHandle = NULL;


    assert((NULL != chanHandle)&&(NULL != psc)&&(NULL != clock));
    instHandle = chanHandle->instHandle;
    assert(NULL != instHandle);

    /* Divisor 12MHz is I2C perpheral module clock frewuency                  */
    prescalar = (instHandle->deviceInfo.inputFrequency
                    / I2c_peripheralClkFreq) - 1u;

    /* Select the Dvalue Value as per Prescalar Value                         */
    if (prescalar > 1u)
    {
        Dvalue = 5u;
    }

    if (prescalar == 1u)
    {
        Dvalue = 6u;
    }

    if (prescalar == 0)
    {
        Dvalue = 7u;
    }

    clk = (Int)((instHandle->deviceInfo.inputFrequency
              / ((prescalar + (Uint32)1u) * chanHandle->busFreq)) -
                    ((Uint32)2u * Dvalue));

    if (clk <= 0)
    {
        /* CLKL/CLKH are gettting 0 or negative values - flag error           */
        /* Cannot go so high in input freq                                    */
        retVal = (IOM_EBADARGS);
    }

    /* Pass the prescalar value                                               */
    *psc = prescalar ;
    *clock = (Uint32)clk ;
    return retVal;
}



/*
 *  \brief   checks if a timeout has happened from the given time.
 *
 *  This function checks if the timeout with respect to the start time has
 *  occured. The timeout and the start time is also input to the function.
 *
 * \param   startValue  [IN]   Start value
 * \param   timeout     [IN]   Timeout value
 *
 * \return  TRUE   if time out occured.
 *          FALSE  if time out has not occured.
 *
 * \enter   startValue  must be a valid start value
 *          timeout     must be a valid timeout value
 *
 * \leave   Not Implemented.
 *
 */
static Bool i2cCheckTimeOut(Uint32 startValue,Uint32 timeout)
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
 * \brief    I2c_localLpscOn
 *
 *           This function is invoked to power on the I2c peripheral and
 *           configure it according to the operating mode. Note that this
 *           function uses different functions to power ON the module,
 *           depending on the PWRM is enabled in this driver or NOT
 *
 * \param    chanHandle    [IN]    handle to the IO channel
 *
 * \return   IOM_COMPLETED in case of success or else an error code is returned
 *
 * \enter    chanHandle must be a valid pointer
 *
 * \leave    Not Implemented.
 */
Int32 I2c_localLpscOn(I2c_Object *instHandle)
{
    Int32 status = IOM_COMPLETED;

    assert(NULL != instHandle);

    if (0 != instHandle->instNum)
    {
#ifdef BIOS_PWRM_ENABLE
       /* power on using bios PWRM API                                        */
        status = (Int32)PWRM_setDependency(
                    (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
        if (TRUE == instHandle->pscPwrmEnable)
        {
            /* I2c 0 is in the always power on hence we will only power on if *
             * it is a instance other than "0"                                */
            status = Psc_ModuleClkCtrl(
                        (Psc_DevId)instHandle->deviceInfo.pscInstance,
                        (Uint32)instHandle->deviceInfo.pwrmLpscId,
                        TRUE);
        }
#endif
    }

    return (status);
}

/**
 * \brief    I2c_localLpscOff
 *
 *           This function is invoked to power off the peripheral handler and
 *           interrupt cause respectively.
 *
 * \param    instHandle  [IN] Handle to the device instance
 *
 * \return   IOM_COMPLETED in case of success or else an error code is returned
 *
 * \enter    chanHandle must be a valid channel object
 *           No pending IO
 *
 * \leave    Not Implemented.
 */
Int32 I2c_localLpscOff(I2c_Object *instHandle)
{
    Uint32          counter = 0;
    volatile Uint32 delay   = I2c_POLLED_TIMEOUT;
    Int32           status  = IOM_COMPLETED;

    assert(NULL!=instHandle);

    if (TRUE == instHandle->pscPwrmEnable)
    {
        /* Check if all the tranmission have completed                        */
        if (((instHandle->deviceInfo.baseAddress)->ICSTR
            & CSL_I2C_ICSTR_BB_MASK) != 0)
        {
            for (counter=0; counter < 1u; counter++)
            {
                if (((instHandle->deviceInfo.baseAddress)->ICMDR
                    & CSL_I2C_ICMDR_TRX_MASK) == 0)
                {
                    while ((((instHandle->deviceInfo.baseAddress)->ICSTR
                        & CSL_I2C_ICIMR_ICRRDY_MASK) == 0)
                            && (delay != 0))
                    {
                        delay--;
                    }
                }
            }
        }

        /* power off if it is an instance other than "0"(as instance 0 does not*
         * have any PSC control)                                              */
        if (0 != instHandle->instNum)
        {
#ifdef BIOS_PWRM_ENABLE
            /* power off  using PWRM API                                      */
            status = (Int32)PWRM_releaseDependency(
                         (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);
#else
            status = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->deviceInfo.pscInstance,
                         (Uint32)instHandle->deviceInfo.pwrmLpscId,
                         FALSE);
#endif
        }
    }
    return (status);
}

/*
 *  \brief  function used to perform a soft reset on the I2C module
 *
 *  This is the function in which a soft reset of the I2C module is performed.
 *  This function resets th I2C module, sets an error on the current IO packet,
 *  returns the current packet back to the upper layer and schedules the next
 *  packet for communication
 *
 *  \param  chanHandle  [IN]    Channel handle
 *
 *  \return  Success  in case of reset was sucessful
 *           Error id in case of error
 *
 *  \enter  chanHandle must be a valid pointer and should not be null.
 *  \leave  Not Implemented.
 *
 */
static Int i2cSoftReset(I2c_ChanObj   *chanHandle)
{
    /* local variables to store register contents                             */
    Uint32      psc               = 0;
    Uint32      clkh              = 0;
    Uint32      clkl              = 0;
    Uint32      oar               = 0;
    Uint32      counter           = 0;
    volatile    Uint32 icmdr      = 0;
    volatile    Uint32 delay      = I2c_POLLED_TIMEOUT;
    volatile    Uint8  buffer     = 0x00;
    Int32       status            = IOM_COMPLETED;
    I2c_Object *instHandle;

    assert(NULL != chanHandle);
    assert(NULL != chanHandle->instHandle);

    instHandle = (I2c_Object*)chanHandle->instHandle;

    assert(NULL != instHandle);

    /* Store the register values into temporary variables                     */
    icmdr = (instHandle->deviceInfo.baseAddress)->ICMDR & I2c_VALUE_RESTORE;
    psc   = (instHandle->deviceInfo.baseAddress)->ICPSC;
    clkh  = (instHandle->deviceInfo.baseAddress)->ICCLKH;
    clkl  = (instHandle->deviceInfo.baseAddress)->ICCLKL;
    oar   = (instHandle->deviceInfo.baseAddress)->ICOAR;

    /* Wait for read/write to complete only when there is no I2C bus error    */
    if ((((instHandle->deviceInfo.baseAddress)->ICSTR
                & CSL_I2C_ICSTR_BB_MASK) != 0))
    {
        for (counter=0; counter < 1u; counter++)
        {
            if (((instHandle->deviceInfo.baseAddress)->ICMDR
                & CSL_I2C_ICMDR_TRX_MASK) == 0)
            {
                while ((((instHandle->deviceInfo.baseAddress)->ICSTR
                    & CSL_I2C_ICIMR_ICRRDY_MASK) == 0)
                        && (delay != 0))
                {
                    delay--;
                }
            }
        }
    }

    /* Put the module in reset                                                */
    (instHandle->deviceInfo.baseAddress)->ICMDR =
        CSL_I2C_ICMDR_IRS_RESETVAL;

    /* Wait till status register reads its default value                      */
    delay = I2c_POLLED_TIMEOUT;

    /** wait till stop bit is generated                                       */
    while (((instHandle->deviceInfo.baseAddress)->ICSTR
        != CSL_I2C_ICSTR_RESETVAL)
            && (delay != 0))
    {
        delay--;
    }


    /* Load the Counter register with zero value                              */
    (instHandle->deviceInfo.baseAddress)->ICCNT = CSL_I2C_ICCNT_RESETVAL;

    /* Restore the value from temporary variables to register                 */
    (instHandle->deviceInfo.baseAddress)->ICPSC   = psc;
    (instHandle->deviceInfo.baseAddress)->ICCLKH  = clkh;
    (instHandle->deviceInfo.baseAddress)->ICCLKL  = clkl;
    (instHandle->deviceInfo.baseAddress)->ICOAR   = oar;

    /* Bring module out of reset                                              */
    (instHandle->deviceInfo.baseAddress)->ICMDR   = icmdr;

    /*  Set the backward compatibility mode off -this is critical to get      *
     *  interrupts correctly                                                  */
    (instHandle->deviceInfo.baseAddress)->ICEMDR &=
        (~CSL_I2C_ICEMDR_BCM_MASK);

    chanHandle->currError = IOM_EBADIO;

    if (I2c_OpMode_DMAINTERRUPT == instHandle->opMode)
    {
        I2c_completeIOedmaCallback(instHandle);
    }
    else if (I2c_OpMode_INTERRUPT == instHandle->opMode)
    {
        i2cCompleteIOInIsr(instHandle);
    }
    else
    {
        /* Polled mode gives a timeout during transaction. No need to complete*
         * the IOP here                                                       */
    }

    return status;
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
Void I2c_loadPendedIops(I2c_Object *instHandle)
{
    I2c_ChanObj    *chanHandle = NULL;
    IOM_Packet     *ioPacket   = NULL;
    I2c_DataParam  *dataParam  = NULL;

    assert(NULL != instHandle);

    /* get the channel with highest priority                              */
    I2c_localGetNextChannel(instHandle, &(instHandle->currentActiveChannel));

    /* store for local use                                                    */
    chanHandle = instHandle->currentActiveChannel;

    /* check and load next pending packet                                     */
    if (NULL != chanHandle)
    {
        if(FALSE == QUE_empty(&(chanHandle->queuePendingList)))
        {
            /* we have atleast one packet                                         */
            ioPacket = (IOM_Packet *)QUE_get(&(chanHandle->queuePendingList));

            instHandle->stats.pendingPacket--;
        }

        /* validate and update the iop                                        */
        if (NULL  != ioPacket)
        {
            chanHandle->activeIOP = ioPacket;
            dataParam = (I2c_DataParam *)ioPacket->addr;
            assert(NULL != dataParam);

            chanHandle->tempICMDRValue =0;

            I2c_localPrepareForTransfer(chanHandle);

            /* Set the current buffer params correctly                        */
            chanHandle->currError = 0;
            chanHandle->currFlags = dataParam->flags;
            chanHandle->currBuffer = (dataParam->buffer);
            chanHandle->currBufferLen = (dataParam->bufLen);

            if (I2c_OpMode_INTERRUPT == instHandle->opMode)
            {
                /* Enable interrupts                                          */
                (instHandle->deviceInfo.baseAddress)->ICIMR =
                    (CSL_I2C_ICIMR_AL_MASK |    /* Arbitration lost           */
                    CSL_I2C_ICIMR_NACK_MASK |   /* NACK received/not received */
                    CSL_I2C_ICIMR_SCD_MASK |    /* SCD interrupt              */
                    CSL_I2C_ICIMR_AAS_MASK);    /* Addressed as Slave         */

                if ((dataParam->flags & I2c_WRITE) != NULL)
                {
                    /* Transmit ready                                         */
                    (instHandle->deviceInfo.baseAddress)->ICIMR |=
                        CSL_I2C_ICIMR_ICXRDY_MASK;

                }
                else if((dataParam->flags & I2c_READ) != NULL)
                {
                    /* Receive ready                                          */
                    (instHandle->deviceInfo.baseAddress)->ICIMR |=
                        CSL_I2C_ICIMR_ICRRDY_MASK;
                }
                else
                {
                    /* do nothing                                             */
                }
                
                /* Clear IVR before doing a start                             */
                (instHandle->deviceInfo.baseAddress)->ICIVR =
                    CSL_I2C_ICIVR_INTCODE_RESETVAL;
            }
            else if (I2c_OpMode_DMAINTERRUPT == instHandle->opMode)
            {
                /* Call this to complete edma configuration and transfer      */
                I2c_localEdmaTransfer((Ptr)instHandle,dataParam->flags);

                /* Enable AL, NACK and AAS interrupts                         */
                (instHandle->deviceInfo.baseAddress)->ICIMR =
                    (CSL_I2C_ICIMR_AL_MASK |    /* Arbitration lost           */
                    CSL_I2C_ICIMR_NACK_MASK |   /* NACK received/not received */
                    CSL_I2C_ICIMR_AAS_MASK);    /* Addressed as Slave         */
            }
            /* Start bit set here to start transaction                        */
            (instHandle->deviceInfo.baseAddress)->ICMDR |= \
                 chanHandle->tempICMDRValue;
        }
    }

    /* update the state of the driver                                         */
    instHandle->devStatus = I2c_DriverState_CREATED; 
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
static Int32 I2cRegisterNotification(I2c_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < I2c_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL, 
                    &I2cNotifyCallback,
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
static Int32 I2cUnregisterNotification(I2c_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < I2c_MAX_PWRM_EVENTS; eventCnt++)
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

static Int32 I2cCalculateConstraints(I2c_Object *instHandle)
{
    I2c_ChanObj    *chanHandle      = NULL;
    Uns             numberSetpoints = 0x00;
    Uns             setpoint        = 0x00;
    Uint32          prescale        = 0x00;
    Uint32          clkDiv          = 0x00;
    Uint32          mask            = 0x00;
    Int32           status          = IOM_COMPLETED;
    PWRM_Constraint constraint      = PWRM_DISALLOWEDSLEEPSTATE_MASK;
    PWRM_Status     retVal          = PWRM_SOK;
    PWRM_Domain     domain          = PWRM_CPU;

    assert(NULL != instHandle);

    /* get the Handle to any of the channel                                   */
    chanHandle = &instHandle->chans[0];
    assert(NULL != chanHandle);
   
    /* check what clock domain is supported by the device                     */
    if (I2c_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
    {
        domain = PWRM_CPU;
        constraint = PWRM_DISALLOWED_CPU_SETPOINT_MASK;
    }
    else if (I2c_pllDomain_1 == instHandle->pwrmInfo.pllDomain)
    {
        domain = PWRM_PER;
        constraint = PWRM_DISALLOWED_PER_SETPOINT_MASK;
    }

    if (I2c_pllDomain_NONE != instHandle->pwrmInfo.pllDomain)
    {
        /* get the number of supported setpoint info for this instance        */
        retVal = PWRM_getNumSetpoints(domain, &numberSetpoints);

        if (PWRM_SOK == retVal)
        {
            /* calculate the contraints for the required bus frequency        */
            for (setpoint = 0x00; setpoint < numberSetpoints; setpoint++)
            {
                status = I2cCalculateClkSettings(
                              instHandle,
                              setpoint,
                              chanHandle->busFreq,
                              &prescale,
                              &clkDiv);

                if (IOM_COMPLETED == status)
                {
                    /* successfully calculated now check if the prescaler is OK   */
                    if ((clkDiv > 255u) || (prescale > 255))
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

            /* check if this instance has already registered some constraint  *
             * if so then unregister those before registering the new         *
             * contraints(for CPU or PER event's unsupported DVFS only)       */
            if (PWRM_SOK == retVal)
            {
                if (NULL != instHandle->pwrmInfo.constraintHandle[constraint - 1])
                {
                    retVal = PWRM_unregisterConstraint(
                                  instHandle->pwrmInfo.constraintHandle[constraint - 1]);
                }
            
                /* register the new set of constraints (only if any contraints*
                 * are present)                                               */
                if ((PWRM_SOK == retVal) && (0 != mask))
                {
                    /* store the value of the constraint mask being registered*/
                    instHandle->pwrmInfo.constraintMask = mask;

                    retVal = PWRM_registerConstraint(
                                 constraint,
                                 mask,
                                 &instHandle->pwrmInfo.constraintHandle[constraint - 1]);
                }
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
static PWRM_NotifyResponse I2cNotifyCallback(PWRM_Event    eventType,
                                             Arg           eventArg1,
                                             Arg           eventArg2,
                                             Arg           clientArg)
{
    I2c_Object          *instHandle = NULL;
    PWRM_NotifyResponse  pwrmStatus = PWRM_NOTIFYDONE;
    PWRM_Status          status     = PWRM_SOK;

    assert(NULL != clientArg);

    instHandle = (I2c_Object *)clientArg;
    assert(NULL != instHandle);
                                            
    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:        
            /* I/O shall be suspended immaterial of the peripheral domain since*
             * the PWRM uses the i2c for configuration changes                */
            pwrmStatus = I2cSuspendCurrentIops(instHandle,eventType);
            
            /* Store the current state of the device                          */
            I2c_saveContext(instHandle);
            break;            

        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
            /* clear any errorneous ses set because of the change in settings */
            /* It is observed that when set-point done occurs a stop condition*
             * is asserted. This needs to be cleared                          */
            (instHandle->deviceInfo.baseAddress)->ICSTR |= 
                                                    CSL_I2C_ICSTR_SCD_MASK;
                
            /* Restore the context of the device with the saved settings      */
            I2c_restoreContext(instHandle);
                    
            /* The pending CPU setpoint change has now been made.Hence now we *
             * will recalculate the new settings for this instance and then   *
             * restore the pending state of the driver                        */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                     (I2c_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                      (I2c_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {             
                pwrmStatus = I2cConfigureNewVfSettings(
                                 instHandle,
                                 (Uns)eventArg1,
                                 (Uns)eventArg2);
            
            }            
            /* irrespective of the PLL domain it is required to load any pended*
             * IOPs because they were suspended above                         */
            if (PWRM_NOTIFYDONE == pwrmStatus)
            {
                I2c_loadPendedIops(instHandle);
            }
            break;
            
        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
            /* clear any errorneous ses set because of the change in settings */
            /* It is observed that when set-point done occurs a stop condition*
             * is asserted. This needs to be cleared                          */
            (instHandle->deviceInfo.baseAddress)->ICSTR |= 
                                                    CSL_I2C_ICSTR_SCD_MASK;

            /* Restore the context of the device with the saved settings      */
            I2c_restoreContext(instHandle);                                     
                    
            /* restore the dependency count                                   */
            while ((instHandle->pwrmInfo.dependencyCount > 0) &&
                    (0 != instHandle->instNum))
            {
                status = (Int32)PWRM_setDependency(
                             (PWRM_Resource)instHandle->deviceInfo.pwrmLpscId);

                if (PWRM_SOK != status)
                {
                    break;
                }
                instHandle->pwrmInfo.dependencyCount--;
            }
         
            if (PWRM_SOK == status)
            {
                I2c_loadPendedIops(instHandle);
            }
            else
            {
                pwrmStatus = PWRM_NOTIFYNOTDONE;
            }
            break;
            
        case PWRM_AWAKEFROMSTANDBY:
            /* clear any errorneous ses set because of the change in settings */
            /* It is observed that when set-point done occurs a stop condition*
             * is asserted. This needs to be cleared                          */
            (instHandle->deviceInfo.baseAddress)->ICSTR |= 
                                                    CSL_I2C_ICSTR_SCD_MASK;
        
            /* Restore the context of the device with the saved settings      */
            I2c_restoreContext(instHandle);                                             
            I2c_loadPendedIops(instHandle);
            break;
            
        default:
            break;
    }
    
    return (pwrmStatus);
}



/**
 * \brief    This function saves the I2C device context so as to restore it back 
 *           after between PWRM operations.
 *
 * \param    instHandle [IN] Handle to the device instance object
 * \return   NONE
 */
static Void I2c_saveContext(I2c_Object *instHandle)
{
    assert(NULL != instHandle);
    
    instHandle->saveContext.icmdr  = 
        ((instHandle->deviceInfo.baseAddress)->ICMDR & I2c_VALUE_RESTORE);
    instHandle->saveContext.psc    = 
        (instHandle->deviceInfo.baseAddress)->ICPSC;
    instHandle->saveContext.clkh   = 
        (instHandle->deviceInfo.baseAddress)->ICCLKH;  
    instHandle->saveContext.clkl   = 
        (instHandle->deviceInfo.baseAddress)->ICCLKL;
    instHandle->saveContext.oar    = 
        (instHandle->deviceInfo.baseAddress)->ICOAR;   
    instHandle->saveContext.icemdr = 
        (instHandle->deviceInfo.baseAddress)->ICEMDR;
        
    return;
}



/**
 * \brief    This function restores the I2C device context that was saved
             between PWRM operations.
 *
 * \param    instHandle [IN] Handle to the device instance object
 * \return   NONE
 */
static Void I2c_restoreContext(I2c_Object *instHandle)
{
    Uint32  volatile delay = I2c_POLLED_TIMEOUT;
    
    assert(NULL != instHandle);
    
    /* Put the module in reset                                                */
    (instHandle->deviceInfo.baseAddress)->ICMDR =
        CSL_I2C_ICMDR_IRS_RESETVAL;

    /* Wait till status register reads its default value                      */
    /** wait till stop bit is generated                                       */
    while (((instHandle->deviceInfo.baseAddress)->ICSTR
        != CSL_I2C_ICSTR_RESETVAL)
            && (delay != 0))
    {
        delay--;
    }


    /* Load the Counter register with zero value                              */
    (instHandle->deviceInfo.baseAddress)->ICCNT = CSL_I2C_ICCNT_RESETVAL;

    /* Restore the value from temporary variables to register                 */
    (instHandle->deviceInfo.baseAddress)->ICPSC   =  
        instHandle->saveContext.psc;
    (instHandle->deviceInfo.baseAddress)->ICCLKH  =
        instHandle->saveContext.clkh;
    (instHandle->deviceInfo.baseAddress)->ICCLKL  = 
        instHandle->saveContext.clkh;
    (instHandle->deviceInfo.baseAddress)->ICOAR   = 
        instHandle->saveContext.oar;
    (instHandle->deviceInfo.baseAddress)->ICEMDR  = 
        instHandle->saveContext.icemdr;
        
    /* Bring module out of reset                                              */
    (instHandle->deviceInfo.baseAddress)->ICMDR   = 
        instHandle->saveContext.icmdr;

    /*  Set the backward compatibility mode off -this is critical to get      *
     *  interrupts correctly                                                  */
    (instHandle->deviceInfo.baseAddress)->ICEMDR &=
        (~CSL_I2C_ICEMDR_BCM_MASK);

    return;
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
static PWRM_NotifyResponse I2cSuspendCurrentIops(I2c_Object  *instHandle,
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
        instHandle->devStatus = I2c_DriverState_PWRM_SUSPEND;
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
static PWRM_NotifyResponse I2cConfigureNewVfSettings(I2c_Object   *instHandle,
                                                     Uns           eventArg1,
                                                     Uns           eventArg2)
{
    Uint32               prescale = 0x00;
    Uint32               clkDiv   = 0x00;
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
        status = I2cCalculateClkSettings(
                     instHandle,
                     eventArg2,
                     instHandle->chans[0].busFreq,
                     &prescale,
                     &clkDiv);
        
        if ((IOM_COMPLETED == status)  &&
            ((clkDiv <= 255u) && (prescale <= 255u)))
        {
            /* calcualation of the prescaler value is successful.Hence update *
             * the hardware with the new prescaler values                     */
            status = I2cConfigureNewClkSettings(instHandle,prescale,clkDiv);
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
 * \leave     Not imeplemented
 * 
 * \note      Please note that the calculated prescaler values are not validated
 *            in this function.
 */
static Int32 I2cCalculateClkSettings(I2c_Object *instHandle,
                                     Uint32      setpoint,
                                     Uint32      opFreq,
                                     Uint32     *prescale,
                                     Uint32     *clkDiv)
{
    PWRM_Status     retVal          = PWRM_SOK;
    PWRM_Domain     domain          = PWRM_CPU;
    Uns             numberSetpoints = 0x00;    
    Uns             frequency       = 0x00;
    Uns             voltage         = 0x00;
    Uint32          sysclkDiv       = 0x00;
    Uint32          dValue          = 0x00;
    Int32           status          = IOM_COMPLETED;
    
    assert((NULL != instHandle) && (NULL != prescale));
    assert(NULL != clkDiv);

    /* check what clock domain is supported by the device                     */
    if (I2c_pllDomain_0 != instHandle->pwrmInfo.pllDomain)
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
                /* convert the frequency to Hz                                */
                frequency = (frequency * 1000)/sysclkDiv;

                /* Divisor 12MHz is I2C perpheral module clock frewuency      */
                (*prescale) = (frequency/I2c_peripheralClkFreq) - 1u;
            
                /* Select the Dvalue Value as per Prescalar Value             */
                if ((*prescale) > 1u)
                {
                    dValue = 5u;
                }
                else if ((*prescale) == 1u)
                {
                    dValue = 6u;
                }
                else if ((*prescale) == 0)
                {
                    dValue = 7u;
                }
            
                (*clkDiv) =  ((frequency/ 
                                (((*prescale) + 1u) * opFreq)) -
                                ((Uint32)2u * dValue));
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
 * \brief     This function configures the I2c device with the new prescalar
 *            and clock divider values
 * 
 * \param     instHandle [IN]  Handle to the driver instance object
 * \param     prescale   [IN]  prescaler value to program.
 * 
 * \return    None
 * 
 * \enter     instHandle is a valid non null pointer
 *            prescalar  is within the permissible limits
 *            clkDiv     is within the permissible limits
 * 
 * \leave     Not implemented
 */
static Int32 I2cConfigureNewClkSettings(I2c_Object *instHandle,
                                        Uint32      prescale,
                                        Uint32      clkDiv)
{
    Uint32  hwiKey   = 0x00;
    Int32   status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* switch on the module in the LPSC(to configure the register values)     */
    status = I2c_localLpscOn(instHandle);

    if (IOM_COMPLETED == status)
    {
        /* enter the critical section                                         */
        hwiKey = _disable_interrupts();

        /* Put the module in reset                                            */
        (instHandle->deviceInfo.baseAddress)->ICMDR =
            CSL_I2C_ICMDR_IRS_RESETVAL;
    
        /* Load the Prescalar value in to the register                        */
        (instHandle->deviceInfo.baseAddress)->ICPSC = prescale;
        (instHandle->deviceInfo.baseAddress)->ICCLKH = (clkDiv/2u);
        (instHandle->deviceInfo.baseAddress)->ICCLKL =
            (clkDiv - (instHandle->deviceInfo.baseAddress)->ICCLKH);
    
        /* Set own address and bits per byte                                  */
        (instHandle->deviceInfo.baseAddress)->ICOAR =
            instHandle->chans[0].ownAddr;

        (instHandle->deviceInfo.baseAddress)->ICMDR =
             (instHandle->chans[0].numBits & I2c_NUMBITS_MASK);
    
        /* Bring the module out of reset                                      */
        (instHandle->deviceInfo.baseAddress)->ICMDR |=
            CSL_I2C_ICMDR_IRS_MASK;
    
        /* Set the backward compatibility mode off - this is critical to get  *
         * interrupts correctly                                               */
        (instHandle->deviceInfo.baseAddress)->ICEMDR &=
            (~CSL_I2C_ICEMDR_BCM_MASK);
    
        /* end critical section                                               */
        _restore_interrupts(hwiKey);

        /* Reduce the dependency count                                        */
        status = I2c_localLpscOff(instHandle);
    }

    return (status);
}

/**
 * \brief     This function unregisters all the constraints for a given device 
 *            instance.
 * 
 * \param     instHandle  [IN]  Handle to the driver instance object
 *
 * \return    IOM_COMPLETED in case of success
 *            IOM_EBADMODE  in case of failure
 * 
 * \enter     instHandle is a valid non null pointer
 *
 * \leave     Not implemented
 */
static Int32 I2cUnregisterConstraints(I2c_Object *instHandle)
{
    Uint32       count  = 0x00;
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;

    assert(NULL != instHandle);
    
    for (count = 0; count < I2c_MAX_PWRM_CONSTRAINTS; count++)
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
                /* reset the value of the contraint Handle                    */
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

