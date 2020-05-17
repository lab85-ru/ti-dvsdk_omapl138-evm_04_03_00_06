/*
 * Lidd.c
 *
 * This file contains Driver Layer Interface implementation for the LIDD Driver.
 * LIDD Driver provides Driver Layer Interface to do operations on the LCDC LIDD
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
 * \file       lidd.c
 * \brief      This file exposes the interfaces, based on PSP Arch.,
 *             which can be used by the applicaitons to program the LCDC h/w.
 *             All implementations in this file are os-independant.
 *
 * \author     PSG, Steve Connell, Madhvapathi Sriram
 * \version    0.1  created from IDriver Lidd.c
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */

#include <std.h>
#include <string.h>
#include <assert.h>
#include <que.h>
#include <tsk.h>
#include <iom.h>
#include "ti/pspiom/lcdclidd/Lidd.h"
#include "ti/pspiom/psc/Psc.h"
#include "LiddLocal.h"

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#ifdef CHIP_C6747
    #include "ti/pspiom/cslr/soc_C6747.h"
    #define LIDD_DEFAULT_HWI_NUM 9
#endif

#ifdef CHIP_OMAPL137
    #include "ti/pspiom/cslr/soc_OMAPL137.h"
    #define LIDD_DEFAULT_HWI_NUM 9
#endif

#ifdef CHIP_OMAPL138
    #include "ti/pspiom/cslr/soc_OMAPL138.h"
    #define LIDD_DEFAULT_HWI_NUM 8
#endif

#ifdef CHIP_C6748
    #include "ti/pspiom/cslr/soc_C6748.h"
    #define LIDD_DEFAULT_HWI_NUM 8
#endif

/* -------- constants -------- */
const struct Lidd_Params Lidd_PARAMS =
{
    {           /* DeviceConf struct   */
        5000000,/* clkFreqHz           */
        0,      /* hwiNum              */
        0,      /* numLines            */
        0,      /* numCharPerLine      */
        {
            0,      /* addressArray[0] */
            0,      /* addressArray[1] */
            0,      /* addressArray[2] */
            0,      /* addressArray[3] */
        },
        FALSE,       /* pscPwrmEnable   */
        Lidd_pllDomain_0
    }
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */

static Bool inUse[CSL_LCDC_PER_CNT];

static Lidd_Module_State Lidd_module = {&inUse[0]};

static Lidd_Object Lidd_Instances[CSL_LCDC_PER_CNT];

static Lidd_HwInfo Lidd_deviceInstInfo[CSL_LCDC_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static inline Void liddDeviceClearScreen(Lidd_Object* instHandle, Uint8 chipSel);

static Void liddDeviceSetCSConf(Lidd_ChanObject *hChan, Lidd_DisplayConf *lConf);

static Void liddDeviceDisplayMoveRight(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceDisplayMoveLeft(Lidd_Object *instHandle, Uint8 chipSel);

static Int liddDeviceCreateInst(Lidd_Object* instHandle, Lidd_Params *params);

static Void liddDeviceCursorMoveRight(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceSetCursorPosition(Lidd_Object *instHandle, Ptr cmdArg);

static Void liddDeviceCursorMoveLeft(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceSetDisplayOff(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceSetDisplayOn(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceSetCursorOff(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddDeviceSetCursorOn(Lidd_Object *instHandle, Uint8 chipSel);

static Void liddSetDataRamAddress(Lidd_ChanObject *hChan, Uint32 chipSel);

static Int32 liddDeviceSetClk(Lidd_Object* instHandle, Uint32 clkFreqHz);
#ifdef BIOS_PWRM_ENABLE
static Int32 LiddUnregisterNotification(Lidd_Object *instHandle);

static Int32 LiddUnregisterConstraints(Lidd_Object *instHandle);

static Int32 LiddCalculateConstraints(Lidd_Object *instHandle);

static Int32 LiddRegisterNotification(Lidd_Object *instHandle);

static Void LiddRestoreSuspendedIops(Lidd_Object *instHandle);
#endif
static Void liddValidateDeviceConf(Lidd_DeviceConf * dConf);

static Void liddValidateLiddConf(Lidd_DisplayConf * lConf);

static Void liddDeviceCloseChannel(Lidd_ChanObject *hChan);

static Int32 liddResetController(Lidd_Object *instHandle);

static Void liddDeviceDeleteInst(Lidd_Object *instHandle);

static inline Void liddDmaReset(Lidd_Object *instHandle);

static Void liddDeviceReset(Lidd_Object *instHandle);

static Int32 liddLpscOff(Lidd_Object *instHandle);

static Int32 liddLpscOn(Lidd_Object *instHandle);

static Void liddGeneralDelay(Uint32 delay);
#ifdef BIOS_PWRM_ENABLE
static PWRM_NotifyResponse LiddConfigureNewVfSettings(
                                                    Lidd_Object   *instHandle,
                                                    Uns              eventArg1,
                                                    Uns              eventArg2
                                                    );
                                                    
static PWRM_NotifyResponse LiddNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg);
                                              
static PWRM_NotifyResponse LiddSuspendCurrentIops(Lidd_Object  *instHandle,
                                                  PWRM_Event   eventType);

static Int32 LiddCalculateClockSettings(Lidd_Object   *instHandle,
                                        Uint32         setpoint,
                                        Uint32         opFreq,
                                        Uint32        *prescale);

static Int32 LiddConfigureNewClkSettings(Lidd_Object *instHandle,
                                        Uint32       prescale);
#endif

static inline Void liddDeviceSetCursorToHome(Lidd_Object *instHandle, 
                                             Uint8 chipSel);

static Void liddDeviceSetDisplayShiftOn(Lidd_Object *instHandle,
                                        Uint8 chipSel);

static Void liddDeviceSetDisplayShiftOff(Lidd_Object *instHandle,
                                        Uint8 chipSel);
                                        
static Ptr liddDeviceOpenChannel(Lidd_Object       *instHandle,
                                 Lidd_Controller    controller,
                                 Ptr               *chanConf,
                                 IOM_TiomCallback   appCb,
                                 Ptr                cbArg);

static Void liddDeviceSetController(Lidd_ChanObject   *hChan,
                                    Lidd_DisplayConf  *lConf);
                                    
static Int liddDataReadWrite(Lidd_ChanObject *hChan,
                              IOM_Packet *ioPacket);

static Void liddSetDevCtrlRegValue(Lidd_Object *instHandle,
                                   Uint8 chipSel,
                                   Uint32 value);

static Void liddInitializeDisplay(Lidd_Object *instHandle,
                                  Lidd_DisplayConf *lConf);

static Void liddCharRead(Lidd_Object *instHandle,
                          Uint8 chipSel,
                          Uint8 *value);

static Void liddDeviceSetBlinkOn(Lidd_Object* instHandle,
                                 Uint8 chipSel);

static Void liddDeviceSetBlinkOff(Lidd_Object* instHandle,
                                  Uint8 chipSel);
                                  
static Void liddCharWrite(Lidd_Object *instHandle,
                          Uint8 chipSel,
                          Uint32 value);

static Void liddChannelConfigLidd(Lidd_ChanObject *hChan,
                                  Lidd_DisplayConf *lConf);

/* ========================================================================== */
/*                           MINI DRIVER FUNCTIONS                            */
/* ========================================================================== */

static Int liddMdBindDev(Ptr *devp, Int devId, Ptr devParams);

static Int liddMdUnBindDev(Ptr devp);

static Int liddMdCreateChan(Ptr        *chanp,
                    Ptr                 devp,
                    String              name,
                    Int                 mode,
                    Ptr                 chanParams,
                    IOM_TiomCallback    cbFxn,
                    Ptr                 cbArg);

static Int liddMdDeleteChan(Ptr chanp);

static Int liddMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);

static Int liddMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs);

const IOM_Fxns Lidd_IOMFXNS =
{
    &liddMdBindDev,
    &liddMdUnBindDev,
    &liddMdControlChan,
    &liddMdCreateChan,
    &liddMdDeleteChan,
    &liddMdSubmitChan,
};

/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */

/*
 *  \brief  Function called by BIOS during instance initialisation
 *
 */
Void Lidd_init(Void)
{
    Int i;

    for (i = 0; i < CSL_LCDC_PER_CNT; i++)
    {
        /* have to initialize statically */
        Lidd_module.inUse[i] = FALSE;
        memset((Void *)&Lidd_Instances[i], 0x0, sizeof(Lidd_Object));
    }
}

/* ========================================================================== */
/*                           MINI DRIVER FUNCTIONS                            */
/* ========================================================================== */

/*
 *  \fn     static Int liddMdBindDev()
 *
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp       [OUT]      pointer to Lidd driver object
 *  \param  devId      [IN]       device Id
 *  \param  devParams  [IN]       device Parameters
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID         in case of error
 *
 *  \enter  *devp           must be a valid pointer and should not be null.
 *          devParams       must be a valid pointer and should not be null.
 *          devId           must be a valid device number (<CSL_LCDC_PER_CNT)
 *
 *  \leave  Not Implemented.
 */
static Int liddMdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Int            retVal     = IOM_COMPLETED;
    Lidd_Params   *params     = NULL;
    Lidd_Object   *instHandle = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* check if the current instance is within the limit supported by the H/W */
    if ((NULL == devParams) || (CSL_LCDC_PER_CNT <= devId) || (0 > devId))
    {
        retVal = IOM_EBADARGS;
    }
    else if (TRUE == Lidd_module.inUse[devId])
    {
        retVal = IOM_EINUSE;
    }
    else
    {
        retVal = IOM_COMPLETED;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {
        params = (Lidd_Params*)devParams;

        instHandle =  &Lidd_Instances[devId];

#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        if (0 == devId)
        {
            Lidd_deviceInstInfo[0].baseAddress    =
                (CSL_LcdcRegsOvly)CSL_LCDC_0_REGS;
            Lidd_deviceInstInfo[0].cpuEventNumber = CSL_INTC_EVENTID_LCDC_INT0;
            Lidd_deviceInstInfo[0].inputFrequency = CSL_LCDC_0_MODULE_FREQ;
            Lidd_deviceInstInfo[0].hwiNumber      = LIDD_DEFAULT_HWI_NUM;

#ifdef BIOS_PWRM_ENABLE            
            Lidd_deviceInstInfo[0].pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_LCDC_0;
#else            
            Lidd_deviceInstInfo[0].pwrmLpscId = (Uint32)CSL_PSC_LCDC;
#endif
            Lidd_deviceInstInfo[0].pscInstance    = CSL_LPSC_INST_LCDC_0;
        }
#endif  /* defined(CHIP_C6747) || defined(CHIP_OMAPL137) */

        liddValidateDeviceConf(&params->devConf);

        instHandle->instHwInfo.baseAddress         =
            Lidd_deviceInstInfo[devId].baseAddress;
        instHandle->instHwInfo.cpuEventNumber  =
            Lidd_deviceInstInfo[devId].cpuEventNumber;
        instHandle->instHwInfo.inputFrequency      =
            Lidd_deviceInstInfo[devId].inputFrequency;
        instHandle->instHwInfo.pwrmLpscId =
            Lidd_deviceInstInfo[devId].pwrmLpscId;
        instHandle->instHwInfo.pscInstance =
            Lidd_deviceInstInfo[devId].pscInstance;

        instHandle->instHwInfo.hwiNumber = params->devConf.hwiNum;
        instHandle->chCount = 0;
        instHandle->instNum = (Uint32)devId;
        instHandle->devType = Lidd_DevType_LCDC;
        instHandle->state = Lidd_DriverState_DELETED;

        retVal = liddDeviceCreateInst(instHandle, params);

        /* Initialize the channel members                                     */
        instHandle->ChannelObj.devInst = (Uint32)devId;
        instHandle->ChannelObj.state = Lidd_DriverState_DELETED;
        instHandle->ChannelObj.chanConf.displayType = Lidd_DisplayType_Hitachi;

        /*Initialize Channel configuration                                    */
        /* Initialize timing to default values                                */
        instHandle->ChannelObj.chanConf.cs0Timing.wSu = 0;
        instHandle->ChannelObj.chanConf.cs0Timing.wStrobe = 1u;
        instHandle->ChannelObj.chanConf.cs0Timing.wHold = 1u;
        instHandle->ChannelObj.chanConf.cs0Timing.rSu = 0;
        instHandle->ChannelObj.chanConf.cs0Timing.rStrobe = 1u;
        instHandle->ChannelObj.chanConf.cs0Timing.rHold = 1u;
        instHandle->ChannelObj.chanConf.cs0Timing.ta = 0;
        instHandle->ChannelObj.chanConf.cs1Timing.wSu = 0;
        instHandle->ChannelObj.chanConf.cs1Timing.wStrobe = 1u;
        instHandle->ChannelObj.chanConf.cs1Timing.wHold = 1u;
        instHandle->ChannelObj.chanConf.cs1Timing.rSu = 0;
        instHandle->ChannelObj.chanConf.cs1Timing.rStrobe = 1u;
        instHandle->ChannelObj.chanConf.cs1Timing.rHold = 1u;
        instHandle->ChannelObj.chanConf.cs1Timing.ta = 0;

        instHandle->ChannelObj.appCb = NULL;
        instHandle->ChannelObj.cbArg = NULL;
        instHandle->ChannelObj.submitCount = 0;
        instHandle->ChannelObj.enabled = FALSE;
        instHandle->ChannelObj.instHandle = NULL;
        instHandle->ChannelObj.currPos = 0;
        instHandle->ChannelObj.currSetting = 0;
        instHandle->ChannelObj.onOffSetting = 0;
        instHandle->ChannelObj.currShift    = 0;
        instHandle->ChannelObj.entryMode    = 0x06u;
        instHandle->ChannelObj.activeIOP    = NULL;

        if ((IOM_COMPLETED == retVal) &&
            (FALSE == instHandle->devConf.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE            
            /* power on using bios PWRM API                                   */
            retVal = (Int32)PWRM_setDependency(
                        (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else
            /* power on using PSC API                                         */
            retVal = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->instHwInfo.pscInstance,
                         (Uint32)instHandle->instHwInfo.pwrmLpscId,
                         TRUE);
#endif                         
        }
    }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == retVal)
            && (TRUE == instHandle->devConf.pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            retVal = LiddRegisterNotification(instHandle);
        }
#endif
    
    if (IOM_COMPLETED == retVal)
    {
        *devp = instHandle;
    }
    else
    {
        *devp = NULL;
    }

    return (retVal);
}


/**
 *  \fn     static Int liddMdUnBindDev()
 *
 *  \brief  Function called by Bios during closing of the instance
 *
 *  \param  devp       [IN]  pointer to Lidd driver object
 *
 *  \return success code
 *
 *  \enter  devp       must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int liddMdUnBindDev(Ptr devp)
{
    Int32        retVal     = IOM_COMPLETED;
    Lidd_Object *instHandle = NULL;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* Check if this is a valid handle                                        */
    if (NULL == devp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle = (Lidd_Object*)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        /* Validate instance ID                                                */
        if ((Lidd_DriverState_CLOSED != instHandle->state)||
            (CSL_LCDC_PER_CNT <= instHandle->instNum))
        {
            retVal = IOM_EBADARGS;
        }
        else
        {
            retVal = IOM_COMPLETED;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {

        if (IOM_COMPLETED == liddLpscOn(instHandle))
        {
            /* Undo everything                                                */
            liddDeviceDeleteInst(instHandle);

            /* Power OFF the module in the LSPC                               */
            retVal = liddLpscOff(instHandle);
        }

        if ((IOM_COMPLETED == retVal) &&
            (FALSE == instHandle->devConf.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            retVal = (Int32)PWRM_releaseDependency(
                        (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else            
            /* power off using PSC API                                        */
            retVal = Psc_ModuleClkCtrl(
                         (Psc_DevId)instHandle->instHwInfo.pscInstance,
                         (Uint32)instHandle->instHwInfo.pwrmLpscId,
                         FALSE);
#endif                         
        }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == retVal)
            && (TRUE == instHandle->devConf.pscPwrmEnable))
        {
            /* unregister all the PWRM event notifications                    */
            retVal = LiddUnregisterNotification(instHandle);
        }
#endif        
        /* set the module state to not in use                                 */
        Lidd_module.inUse[instHandle->instNum] = FALSE;
    }
    return (retVal);
}

/**
 *  \fn     static Ptr liddMdCreateChan()
 *
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the McASP device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a SIO_create call.
 *  \param     chanp        [OUT]    channel object pointer
 *  \param     devp         [IN]     pointer to Lidd instance
 *  \param     name         [IN]     Lidd Instance name like lcdc0
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
static Int liddMdCreateChan(Ptr                *chanp,
                            Ptr                 devp,
                            String              name,
                            Int                 mode,
                            Ptr                 chanParams,
                            IOM_TiomCallback    cbFxn,
                            Ptr                 cbArg)
{
    Int32            retVal     = IOM_COMPLETED;
    Int32            status     = IOM_COMPLETED;
    Lidd_ChanParams *lcdParams  = NULL;
    Lidd_Object     *instHandle = NULL;
    Bool             pscPwrOn   = FALSE;
    Lidd_Controller  controller = Lidd_Controller_UNDEFINED;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if( (NULL == devp)
        || (NULL == chanp)
        || (IOM_OUTPUT != mode)
        || (NULL == chanParams) )
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif
        instHandle = (Lidd_Object*)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if((CSL_LCDC_PER_CNT <= instHandle->instNum)
            || (NULL == ((Lidd_ChanParams*)chanParams)->chanConf))
        {
            retVal = IOM_EBADARGS;
        }
        else
        {
            retVal = IOM_COMPLETED;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {
        /* to remove the compiler warnings                                    */
        if((NULL != name) && (mode == mode))
        {
            name = name;
            mode = mode;
        }

        /* switch on the module in the PSC                                    */
        retVal = liddLpscOn(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            pscPwrOn = TRUE;

            retVal = liddResetController(instHandle);

            if (IOM_COMPLETED != retVal)
            {
                /* unable to reset the controller hance reset the device      */
                liddDmaReset(instHandle);
                liddDeviceReset(instHandle);

                /* Set the retVal to error state                              */
                retVal = IOM_EBADIO;
            }
        }
    }

    if (IOM_COMPLETED == retVal)
    {
        lcdParams = (Lidd_ChanParams*)chanParams;
        controller = lcdParams->controller;

        liddValidateLiddConf((Lidd_DisplayConf *)lcdParams->chanConf);

        *chanp = (Ptr) liddDeviceOpenChannel(instHandle,
                            controller,
                            lcdParams->chanConf,
                            cbFxn,
                            cbArg);
        if (NULL == chanp)
        {
            retVal = IOM_EBADARGS;
        }
    }

    if (TRUE == pscPwrOn)
    {
        /* Switch off the module in the PSC                                   */
        status = liddLpscOff(instHandle);

        /* give preference to the error generated during the creation of the  *
         * channel. if the creation of channel is successful but power off    *
         * fails, then we will send the status of the power off failure to    *
         * application                                                        */
        if (IOM_COMPLETED == retVal)
        {
            retVal = status;
        }
    }

    if (IOM_COMPLETED != retVal)
    {
        *chanp = NULL;
        retVal = IOM_EBADARGS;
    }

    return (retVal);
}

/**
 *  \fn      static Uint liddMdSubmitChan()
 *
 *  \brief   Submit a I/O packet to a channel for processing
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
 * \leave  Not implemented
 */
static Int liddMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Lidd_Object     *instHandle = NULL;
    Lidd_ChanObject *hChan      = NULL;
    Int32            retVal     = IOM_COMPLETED;
    Int32            status     = IOM_COMPLETED;
    Uint32           hwiKey     = 0;


    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(( NULL == chanp) || (NULL == ioPacket))
    {
        retVal = IOM_EBADARGS;
    }
    else if(NULL == ioPacket->addr)
    {
        retVal = IOM_EBADARGS;
    }
    else if((IOM_WRITE != ioPacket->cmd) && (IOM_FLUSH != ioPacket->cmd)
            && (IOM_ABORT != ioPacket->cmd))
    {
        retVal = IOM_ENOTIMPL;
    }
    else
    {
        retVal = IOM_COMPLETED;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {
        hChan  = (Lidd_ChanObject *)chanp;
        instHandle = hChan->instHandle;

        assert(NULL != instHandle);

        if(IOM_WRITE == ioPacket->cmd)
        {
            if (IOM_COMPLETED == liddLpscOn(instHandle))
            {
                hwiKey = _disable_interrupts();
                                
                if ((NULL == hChan->activeIOP) && 
                    (Lidd_DriverState_PWRM_SUSPEND != instHandle->state))
                {
                    hChan->submitCount++;
                    hChan->activeIOP = ioPacket;
                    _restore_interrupts(hwiKey);
                    
                    retVal = liddDataReadWrite(hChan, ioPacket);
#ifdef BIOS_PWRM_ENABLE
                    /* If the IO needs to be suspended then do so and notify  *
                     * however, here nothing to be done for suspending IOs    */
                    if ((TRUE == instHandle->devConf.pscPwrmEnable) && 
                        (TRUE == instHandle->pwrmInfo.ioSuspend))
                    {
                        instHandle->pwrmInfo.ioSuspend = FALSE;
                        instHandle->state = Lidd_DriverState_PWRM_SUSPEND;
              
                        /* call the delayed completion function               */
                        (instHandle->pwrmInfo.delayedCompletionFxn  \
                            [instHandle->pwrmInfo.pwrmEvent])();
                    }
#endif
                    hwiKey = _disable_interrupts();
                    hChan->activeIOP = NULL;
                    hChan->submitCount--;
                    _restore_interrupts(hwiKey);
                    
                    status = liddLpscOff(instHandle);
                    
                    if (IOM_COMPLETED == retVal)
                    {
                        retVal = status;
                    }
                }
                else
                {
                    retVal = IOM_EBADIO;
                    _restore_interrupts(hwiKey);
                }
            }
        }

        /* must handle ABORT & FLUSH commands sent by GIO                     */
        else if ((IOM_ABORT == ioPacket->cmd) || (IOM_FLUSH == ioPacket->cmd))
        {
            /* Just return completed as we do not have anything in the queue.
               LIDD is a synchronous driver */
            retVal = IOM_COMPLETED;
        }
        else
        {
            retVal = IOM_ENOTIMPL;
        }
    }
    return (retVal);
}

/**
 * \fn       static Int liddMdDeleteChan()
 *
 * \brief    This function is called by the application to close a previously
 *           opened channel.it deletes the channel so that it is not available
 *           for further transactions. All the allocated resources are freed
 *           and the channel will be ready for the "open" operation once again.
 *
 *  \param    chanp      [IN]   Handle to the channel.
 *
 *  \return   status
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *
 * \leave  Not implemented
 */
static Int liddMdDeleteChan(Ptr chanp)
{
    Lidd_ChanObject *hChan  = NULL;
    Int32 retVal = IOM_COMPLETED;
#if defined(BIOS_PWRM_ENABLE) || !defined(PSP_DISABLE_INPUT_PARAMETER_CHECK) 
    Lidd_Object *instHandle;
#endif
    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == chanp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif

        hChan  = (Lidd_ChanObject *)chanp;
#if defined(BIOS_PWRM_ENABLE) || !defined(PSP_DISABLE_INPUT_PARAMETER_CHECK) 
        instHandle = hChan->instHandle;
#endif
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if((NULL != instHandle) && 
           (Lidd_DriverState_OPENED != instHandle->state))
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == retVal)
    {
        liddDeviceCloseChannel(hChan);
        
#ifdef BIOS_PWRM_ENABLE
    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
        retVal = LiddUnregisterConstraints(instHandle);
    }
#endif
    }

    return (retVal);
}


/* ========================================================================== */
/*                           INTERNAL FUNCTIONS                               */
/* ========================================================================== */
static Void liddDeviceSetCursorPosition(Lidd_Object *instHandle,
                                        Ptr cmdArg)
{
    Lidd_CursorPosition *cursorPos;

    assert(NULL!=instHandle);
    assert(NULL!=cmdArg);

    cursorPos = (Lidd_CursorPosition*) cmdArg;

    assert(instHandle->devConf.numLines >= cursorPos->line);
    assert(instHandle->devConf.numCharPerLine >= cursorPos->col);

    instHandle->ChannelObj.currPos =
        (((cursorPos->line-1u) * instHandle->devConf.numCharPerLine)
          + cursorPos->col)-1u;

    liddSetDataRamAddress(&instHandle->ChannelObj,cursorPos->chipSel);

    return;
}


static Void liddDeviceSetDisplayOff(Lidd_Object *instHandle,
                                    Uint8 chipSel)
{
    Uint32  val = 0;

    assert(NULL!=instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val &= ~(0x04u);

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle, chipSel,(Uint32)(0x08u|val));

}


static Void liddDeviceSetDisplayOn(Lidd_Object *instHandle,
                                   Uint8    chipSel)
{
    Uint32  val = 0;

    assert(NULL!= instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val |= (0x04u);

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle, chipSel, (Uint32)(0x08u|val));
}


static Void liddDeviceSetBlinkOn(Lidd_Object* instHandle,
                                 Uint8 chipSel)
{
    Uint32  val = 0;

    assert(NULL!=instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val |= 0x01u;

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x08u|val));

}


static Void liddDeviceSetBlinkOff(Lidd_Object* instHandle,
                                  Uint8 chipSel)
{
    Uint32 val = 0;

    assert(NULL!=instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val &= ~0x01u;

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x08u|val));
}


static Void liddDeviceSetCursorOn(Lidd_Object *instHandle,
                                  Uint8 chipSel)
{
    Uint32 val = 0;

    assert(NULL!=instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val |= 0x02u;

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x08u | val));
}


static Void liddDeviceSetCursorOff(Lidd_Object *instHandle,
                                   Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.onOffSetting;

    val &= ~0x02u;

    instHandle->ChannelObj.onOffSetting = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x08u|val));
}


static Void liddDeviceSetDisplayShiftOn(Lidd_Object *instHandle,
                                        Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;

    val |= 0x08u;

    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x10u | val));
}


static Void liddDeviceSetDisplayShiftOff(Lidd_Object *instHandle,
                                         Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;
    val &= ~0x08u;
    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle, chipSel, (Uint32)(0x10u | val));
}


static Void liddDeviceCursorMoveRight(Lidd_Object *instHandle,
                                      Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;

    val &= ~(0x08u);
    val |= 0x04u;

    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x10u|val));
}


static Void liddDeviceCursorMoveLeft(Lidd_Object *instHandle,
                                     Uint8 chipSel)

{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;

    val &= ~(0x0Cu);

    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x10u|val));
}


static Void liddDeviceDisplayMoveLeft(Lidd_Object *instHandle,
                                      Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;
    val |= 0x08u;
    val &= ~(0x04u);

    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x10u | val));
}


static Void liddDeviceDisplayMoveRight(Lidd_Object *instHandle,
                                       Uint8 chipSel)
{
    Uint32 val = 0;

    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    val = instHandle->ChannelObj.currShift;

    val |= 0x0Cu;

    instHandle->ChannelObj.currShift = val;

    liddSetDevCtrlRegValue(instHandle,chipSel,(Uint32)(0x10u | val));
}


static inline Void liddDeviceClearScreen(Lidd_Object* instHandle,
                                         Uint8   chipSel)
{
    assert(NULL!=instHandle);
    assert((0 == chipSel) || (1u == chipSel));

    liddSetDevCtrlRegValue(instHandle, chipSel, (Uint32)0x01u);
}


static inline Void liddDeviceSetCursorToHome(Lidd_Object *instHandle,
                                             Uint8 chipSel)
{
    assert((NULL!=instHandle));
    assert((0 == chipSel) || (1u == chipSel));

    liddSetDevCtrlRegValue(instHandle, chipSel, (Uint32)0x02u);
}


/**
 * \fn       static Int liddMdControlChan()
 *
 * \brief    This function allows the control commands to be sent to the Lidd
 *           driver. it internally calls the mcaspSubmitIoctl Function to
 *           perform the desired operation.
 *
 * \param    chanp      [IN]    Channel handle
 * \param    cmd        [IN]    control command given by the application
 * \param    cmdArgs    [IN]    Optional arguments required for the command
 *
 * \return   None
 *
 *  \enter  chanp      must be a valid pointer and should not be null.
 *          cmd          is a valid IOCTL command,
 *          cmdArgs      are valid and appropriate for the command passed,
 *
 * \leave  Not implemented
 *
 */
static Int liddMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs)
{
    Int32             retVal     = IOM_COMPLETED;
    Int32             status     = IOM_COMPLETED;
    Lidd_ChanObject  *hChan      = NULL;
    Lidd_IoctlCmdArg *ioctlArg   = NULL;
    Bool              pscPwrOn   = FALSE;
    Lidd_Object      *instHandle = NULL;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == chanp) || (NULL == cmdArgs))
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif
        hChan = (Lidd_ChanObject *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if(NULL != hChan)
        {
#endif
            instHandle = hChan->instHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            /* ok to check hChan struct members now because we know it's not NULL */
            if (NULL == instHandle)
            {
                retVal = IOM_EBADARGS;
            }
            else
            {
                retVal = IOM_COMPLETED;
            }
        }
        else
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {
        hChan = (Lidd_ChanObject *)chanp;
        ioctlArg = (Lidd_IoctlCmdArg*)cmdArgs;
        instHandle = hChan->instHandle;

        retVal = liddLpscOn(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            pscPwrOn = TRUE;
        }
    }

    if (IOM_COMPLETED == retVal)
    {
        if(Lidd_IOCTL_CLEAR_SCREEN == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceClearScreen(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_CURSOR_HOME == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetCursorToHome(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_CURSOR_POSITION == cmd)
        {
            liddDeviceSetCursorPosition(instHandle,ioctlArg);
        }
        else if(Lidd_IOCTL_SET_DISPLAY_ON == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetDisplayOn(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_DISPLAY_OFF == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetDisplayOff(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_BLINK_ON == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetBlinkOn(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_BLINK_OFF == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetBlinkOff(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_CURSOR_ON == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetCursorOn(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_CURSOR_OFF == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetCursorOff(instHandle,chipSel);
        }
        else if(Lidd_IOCTL_SET_DISPLAY_SHIFT_ON == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetDisplayShiftOn(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_SET_DISPLAY_SHIFT_OFF == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceSetDisplayShiftOff(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_CURSOR_MOVE_LEFT == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceCursorMoveLeft(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_CURSOR_MOVE_RIGHT == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceCursorMoveRight(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_DISPLAY_MOVE_LEFT == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceDisplayMoveLeft(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_DISPLAY_MOVE_RIGHT == cmd)
        {
            Uint8 chipSel = ioctlArg->chipSel;
            liddDeviceDisplayMoveRight(instHandle, chipSel);
        }
        else if(Lidd_IOCTL_COMMAND_REG_WRITE == cmd)
        {
            Uint8   chipSel = ioctlArg->chipSel;
            Uint32  val = ioctlArg->val;
            liddSetDevCtrlRegValue(instHandle, chipSel,val);
        }
        else
        {
            retVal = IOM_EBADARGS;
        }
    }

    if (TRUE == pscPwrOn)
    {
        status = liddLpscOff(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            retVal = status;
        }
    }
    return (retVal);
}

/**
 * Selects the controller (here, either raster or lidd)
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void liddDeviceSetController(Lidd_ChanObject *hChan,
                                    Lidd_DisplayConf  *lConf)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    Lidd_Object        *instHandle;

    assert((NULL != hChan));
    assert((NULL != hChan->instHandle));

    instHandle = (Lidd_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    assert((NULL != lConf));

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert((NULL != lcdcRegs));

    lcdcRegs->LCD_CTRL = \
        ((lcdcRegs->LCD_CTRL & ~CSL_LCDC_LCD_CTRL_MODESEL_MASK) |
         ((CSL_LCDC_LCD_CTRL_MODESEL_LIDD << CSL_LCDC_LCD_CTRL_MODESEL_SHIFT)&
          CSL_LCDC_LCD_CTRL_MODESEL_MASK));

    switch (lConf->displayType)
    {
        case Lidd_DisplayType_SyncMPU68:
            instHandle->instHwInfo.baseAddress->LIDD_CTRL =
                (CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SYNC_MPU68
                << CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SHIFT);
            break;

        case Lidd_DisplayType_AsyncMPU68:
            instHandle->instHwInfo.baseAddress->LIDD_CTRL =
                (CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_ASYNC_MPU68
                << CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SHIFT);
            break;

        case Lidd_DisplayType_SyncMPU80:
            instHandle->instHwInfo.baseAddress->LIDD_CTRL =
                (CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SYNC_MPU80
                << CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SHIFT);
            break;

        case Lidd_DisplayType_AsyncMPU80:
            instHandle->instHwInfo.baseAddress->LIDD_CTRL =
                (CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_ASYNC_MPU80
                << CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SHIFT);
            break;

        case Lidd_DisplayType_Hitachi:
            instHandle->instHwInfo.baseAddress->LIDD_CTRL =
                (CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_HITACHI
                << CSL_LCDC_LIDD_CTRL_LIDD_MODE_SEL_SHIFT);
            break;

        default:
            break;
    }

   return;
}


/**
 * Sets the clock to the given frequency
 * This function is platform dependant
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Int32 liddDeviceSetClk(Lidd_Object* instHandle, Uint32 clkFreqHz)
{
    Int32           retVal = IOM_COMPLETED;
    volatile Uint32 clkDiv = 0;
    volatile Uint32 regVal = 0;

    assert((NULL != instHandle));

    clkDiv = ((Uint32)instHandle->instHwInfo.inputFrequency)/clkFreqHz;

    if(Lidd_DEV_CLKDIV_MAX >= clkDiv)
    {
        regVal = (instHandle->instHwInfo.baseAddress->LCD_CTRL
                  & CSL_LCDC_LCD_CTRL_CLKDIV_MASK) ;
        regVal |= (clkDiv << CSL_LCDC_LCD_CTRL_CLKDIV_SHIFT);
        instHandle->instHwInfo.baseAddress->LCD_CTRL = regVal;

        instHandle->devConf.clkFreqHz = clkFreqHz;
    }
    else
    {
        retVal = IOM_EBADIO;
    }

    return retVal;
}


/**
 * Resets the Dma registers of the LCDC device
 * This function does not update any device structures; update the
 * ds before calling this function (if needed)
 */
static inline Void liddDmaReset(Lidd_Object *instHandle)
{
    assert((NULL!=instHandle));

    instHandle->instHwInfo.baseAddress->LCDDMA_CTRL =
        CSL_LCDC_LCDDMA_CTRL_RESETVAL;

    return;
}


/**
 * Resets the device specific drivers of the LCDC
 * This function does not update the ds
 * Update the ds before calling this function (if needed)
 */
static Void liddDeviceReset(Lidd_Object *instHandle)
{
    volatile Uint32 val = 0;
    assert((NULL!=instHandle));

    instHandle->instHwInfo.baseAddress->LCD_CTRL = CSL_LCDC_LCD_CTRL_RESETVAL;
    /* Reset Interrupt stats */
    val = instHandle->instHwInfo.baseAddress->LCD_STAT;
    instHandle->instHwInfo.baseAddress->LCD_STAT = val;

    return;
}

/* Write a single character to the LCD panel
 */
static Void liddCharWrite(Lidd_Object *instHandle,
                          Uint8 chipSel,
                          Uint32 value)
{
    Bool isTask = FALSE;

    assert((NULL != instHandle));
    assert(((0 == chipSel) || (1u == chipSel)));

    if(0 == chipSel)
    {
        instHandle->instHwInfo.baseAddress->LIDD_CS0_DATA = value;
    }
    else
    {
        instHandle->instHwInfo.baseAddress->LIDD_CS1_DATA = value;
    }

    /*Wait for the operation to complete                                      */

    /* sleep only if in a task context and not in SWI or HWI              */
    isTask = TSK_isTSK();
    if (TRUE == isTask)
    {
        TSK_sleep(1);
    }
    else
    {
        liddGeneralDelay(100u);
    }

    instHandle->ChannelObj.currPos++;
}


/* Read a single character from the LCD panel
 */
static Void liddCharRead(Lidd_Object *instHandle,
                          Uint8 chipSel,
                          Uint8 *value)
{
    Bool isTask = FALSE;

    assert((NULL != instHandle));
    assert(((0 == chipSel) || (1u == chipSel)));
    assert((NULL != value));

    if(0 == chipSel)
    {
        *value = (Uint8)instHandle->instHwInfo.baseAddress->LIDD_CS0_DATA;
    }
    else
    {
        *value = (Uint8)instHandle->instHwInfo.baseAddress->LIDD_CS1_DATA;
    }

    /*Wait for the operation to  conplete                                     */

    /* sleep only if in a task context and not in SWI or HWI              */
    isTask = TSK_isTSK();
    if (TRUE == isTask)
    {
        /* sleep for this amount of time before it checks for status
         *  again      TBD: This value to be determined                   */
        TSK_sleep(1);
    }
    else
    {
        liddGeneralDelay(1000u);
    }
}



static Void liddSetDataRamAddress(Lidd_ChanObject *hChan, Uint32 chipSel)
{
    volatile Uint32  lineFeed = 0;
    volatile Uint8   ddRamAddr = 0x80u; /* Cmd 'bit' for setting ddrRamAddr */
    Lidd_Object     *instHandle;
    Uint32           numLines, numCharPerLine, currentLine, currentChar;
    Uint32          *addrArray;

    assert((NULL != hChan));
    assert((NULL != hChan->instHandle));
    assert(((0 == chipSel) || (1u == chipSel)));

    instHandle = (Lidd_Object *)hChan->instHandle;
    assert(NULL != instHandle);

    numLines    = instHandle->devConf.numLines;
    numCharPerLine = instHandle->devConf.numCharPerLine;
    addrArray = &instHandle->devConf.addressArray[0];

    if(hChan->currPos > ((numCharPerLine * numLines)-1))
    {
        /* Reset the current position of the cursor if it exceeds the number
           of characters per panel */
        hChan->currPos = 0;
    }

    currentLine = hChan->currPos/numCharPerLine; /* 0,1,...numLines-1 */
    currentChar = hChan->currPos%numCharPerLine;

    ddRamAddr += (addrArray[currentLine] + currentChar);

    if(0 == chipSel)
    {
        ((Lidd_Object *)hChan->instHandle)->instHwInfo.baseAddress->LIDD_CS0_ADDR = ddRamAddr;
    }
    else
    {
        ((Lidd_Object *)hChan->instHandle)->instHwInfo.baseAddress->LIDD_CS1_ADDR = ddRamAddr;
    }


    TSK_sleep(1);
}

/**
 * Read/Write data to /from the LCD panel
 */
static Int liddDataReadWrite(Lidd_ChanObject *hChan,
                              IOM_Packet *ioPacket)
{
    Lidd_Object        *instHandle;
    Lidd_DataParam     *dataParam;
    Uint8              *buf;
    Uint32              len;
    Uint32              maxLen;
    Uint32              i;
    Int                 retVal = IOM_COMPLETED;

    assert((NULL!=hChan));
    assert((NULL!=hChan->instHandle));

    instHandle = (Lidd_Object *)hChan->instHandle;
    assert(NULL != instHandle);

    assert((NULL!=ioPacket));
    assert(NULL != ioPacket->addr);

    dataParam = (Lidd_DataParam*)ioPacket->addr;
    assert(NULL != dataParam);
    assert(NULL != dataParam->buffer);

    buf    = dataParam->buffer;
    len    = dataParam->len;
    maxLen = instHandle->devConf.numCharPerLine * instHandle->devConf.numLines;

    /* Limit the count to max LCD size if it is greater than that */
    if(len > maxLen)
    {
        len = maxLen;
    }

    if(IOM_WRITE == ioPacket->cmd)
    {
        for(i=0; i<len; i++)
        {
            liddSetDataRamAddress(hChan,(Uint32)dataParam->chipSel);
            liddCharWrite((Lidd_Object *)hChan->instHandle,
                    dataParam->chipSel, buf[i]);
        }
    }
    /* For now only write is supported though */
    else if(IOM_READ == ioPacket->cmd)
    {
        for(i=0; i<len; i++)
        {
            liddCharRead((Lidd_Object *)hChan->instHandle,
                    dataParam->chipSel, &buf[i]);
        }
    }
    else
    {
        retVal = IOM_ENOTIMPL;
    }

    return retVal;
}


static Int32 liddResetController(Lidd_Object *instHandle)
{
    Int32 retVal = IOM_COMPLETED;
    
    assert(NULL != instHandle);
    
    liddDeviceReset(instHandle);
#ifdef BIOS_PWRM_ENABLE
    /* Calculate the constraints here. Its done here because only here are we *
     * actually setting up the clock details. If there are any problems we    *
     * should be notifying here and the channel creation should fail          */
    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
        retVal = LiddCalculateConstraints(instHandle);
    }
#endif
    
    retVal = liddDeviceSetClk(instHandle, instHandle->devConf.clkFreqHz);

    return retVal;
}


/**
 * Creates a new device instance
 *
 * 1. The name of the semaphore created in this function is the same for
 *    all instances. This doesnt affect in PrOS as this name field is not
 *    used.
 */
static Int liddDeviceCreateInst(Lidd_Object* instHandle, Lidd_Params *params)
{
    Int32             retVal      = IOM_COMPLETED;
    Int32             i = 0;
    assert((NULL != instHandle));
    assert((NULL != params));
    assert((Lidd_numInstances > instHandle->instNum));

    instHandle->devType     = Lidd_DevType_LCDC;
    instHandle->chCount  = 0;

    /* Store the device configuration                                         */
    memcpy(&instHandle->devConf,&params->devConf, sizeof(Lidd_DeviceConf));
    /* Zero intialize the event Statistics for this instance at start         */

    instHandle->instHwInfo.baseAddress =
        Lidd_deviceInstInfo[instHandle->instNum].baseAddress;

    instHandle->devConf.clkFreqHz = params->devConf.clkFreqHz;
    instHandle->devConf.hwiNum  = params->devConf.hwiNum;
    instHandle->devConf.numLines = params->devConf.numLines;
    instHandle->devConf.numCharPerLine = params->devConf.numCharPerLine;
    instHandle->devConf.pscPwrmEnable = params->devConf.pscPwrmEnable;
    instHandle->devConf.pllDomain = params->devConf.pllDomain;
#ifdef BIOS_PWRM_ENABLE    
    instHandle->pwrmInfo.pllDomain = params->devConf.pllDomain;
#endif
    for (i = 0; i < params->devConf.numLines; i++)
    {
        instHandle->devConf.addressArray[i] = params->devConf.addressArray[i];
    }

    /* Change the state to CREATED                                            */
    instHandle->state = Lidd_DriverState_CREATED;
    /* For state machine sake, move the state to CLOSED, as no
       channels are yet open                                                  */
    instHandle->state = Lidd_DriverState_CLOSED;

    return (retVal);
}


/**
 * Deletes the specific device instances identified by the device Id
 */
static Void liddDeviceDeleteInst(Lidd_Object *instHandle)
{
    assert((NULL != instHandle));

    liddDeviceReset(instHandle);

    memset(&instHandle->devConf,0,sizeof(Lidd_DeviceConf));

    instHandle->state = Lidd_DriverState_DELETED;
}


static Void liddDeviceSetCSConf(Lidd_ChanObject *hChan, Lidd_DisplayConf *lConf)
{
    Lidd_StrobeTiming *timing;

    assert((NULL != hChan));
    assert((NULL != lConf));

    timing = &lConf->cs0Timing;
    assert(NULL != timing);

    ((Lidd_Object *)hChan->instHandle)->instHwInfo.baseAddress->LIDD_CS0_CONF =
        ((timing->rSu << CSL_LCDC_LIDD_CS0_CONF_R_SU_SHIFT)
        |(timing->rStrobe << CSL_LCDC_LIDD_CS0_CONF_R_STROBE_SHIFT)
        |(timing->rHold << CSL_LCDC_LIDD_CS0_CONF_R_HOLD_SHIFT)
        |(timing->wSu << CSL_LCDC_LIDD_CS0_CONF_W_SU_SHIFT)
        |(timing->wStrobe << CSL_LCDC_LIDD_CS0_CONF_W_STROBE_SHIFT)
        |(timing->wHold << CSL_LCDC_LIDD_CS0_CONF_W_HOLD_SHIFT)
        |(timing->ta << CSL_LCDC_LIDD_CS0_CONF_TA_SHIFT));

    timing = &lConf->cs1Timing;
    assert(NULL != timing);

    ((Lidd_Object *)hChan->instHandle)->instHwInfo.baseAddress->LIDD_CS1_CONF =
        ((timing->rSu  << CSL_LCDC_LIDD_CS1_CONF_R_SU_SHIFT)
        |(timing->rStrobe << CSL_LCDC_LIDD_CS1_CONF_R_STROBE_SHIFT)
        |(timing->rHold << CSL_LCDC_LIDD_CS1_CONF_R_HOLD_SHIFT)
        |(timing->wSu << CSL_LCDC_LIDD_CS1_CONF_W_SU_SHIFT)
        |(timing->wStrobe << CSL_LCDC_LIDD_CS1_CONF_W_STROBE_SHIFT)
        |(timing->wHold << CSL_LCDC_LIDD_CS1_CONF_W_HOLD_SHIFT)
        |(timing->ta << CSL_LCDC_LIDD_CS1_CONF_TA_SHIFT));
}


static Void liddGeneralDelay(Uint32 delay)
{
    volatile Uint32 count = delay;

    while(count)
    {
        count--;
    }
}


static Void liddSetDevCtrlRegValue(Lidd_Object *instHandle,
                                   Uint8 chipSel,
                                   Uint32 value)
{
    Bool isTask = FALSE;

    assert(((0 == chipSel) || (1u == chipSel)));
    assert(NULL != instHandle);

    if(0 == chipSel)
    {
        instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = value;
    }
    else
    {
        instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = value;
    }


    /*Wait for the operation to conplete                                      */

    isTask = TSK_isTSK();
    /* sleep only if in a task context and not in SWI or HWI                  */
    if (TRUE == isTask)
    {
        /* sleep for this amount of time before it checks for status
         *  again      TBD: This value to be determined                       */
        TSK_sleep(2);
    }
    else
    {
        liddGeneralDelay(100u);
    }
}


static Void liddInitializeDisplay(Lidd_Object *instHandle,
                                  Lidd_DisplayConf *lConf)
{
    assert(NULL != instHandle);
    assert(NULL != lConf);

    /* For initialization sequence refer to LCD display specific datasheet    */

    switch (lConf->displayType)
    {
        case Lidd_DisplayType_Hitachi:
            TSK_sleep(2);
            if(0 == lConf->chipSel)
            {
                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x30u;
                TSK_sleep(2);

                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x30u;
                TSK_sleep(2);

                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x30u;
                TSK_sleep(2);

                /* Set N = 1 (two lines), F=0 (5x7 dots per char)             */
                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x38u;
                TSK_sleep(2);

                /*Turn the display off                                        */
                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x08u;
                TSK_sleep(2);
                /*Clear the display                                           */
                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x01u;
                TSK_sleep(2);
                /*Entry mode to I/D =1(Increment Mode),S=0(Disp shift off)    */
                instHandle->instHwInfo.baseAddress->LIDD_CS0_ADDR = 0x06u;
                TSK_sleep(2);

                instHandle->ChannelObj.onOffSetting = 0;
                instHandle->ChannelObj.currShift    = 0;
                instHandle->ChannelObj.entryMode    = 0x06u;
            }
            else if(1u == lConf->chipSel)
            {
                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x30u;
                TSK_sleep(2);

                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x30u;
                TSK_sleep(2);

                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x30u;
                TSK_sleep(2);

                /* Set N = 1 (two lines), F=0 (5x7 dots per char)             */
                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x38u;
                TSK_sleep(2);

                /*Turn the display off                                        */
                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x08u;
                TSK_sleep(2);
                /*Clear the display                                           */
                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x01u;
                TSK_sleep(2);
                /*Entry mode to I/D =1(Increment Mode),S=0(Disp shift off)    */
                instHandle->instHwInfo.baseAddress->LIDD_CS1_ADDR = 0x06u;
                TSK_sleep(2);

                instHandle->ChannelObj.onOffSetting = 0;
                instHandle->ChannelObj.currShift    = 0;
                instHandle->ChannelObj.entryMode    = 0x06u;
            }

            break;

        case Lidd_DisplayType_SyncMPU68 :
        case Lidd_DisplayType_AsyncMPU68:
        case Lidd_DisplayType_SyncMPU80 :
        case Lidd_DisplayType_AsyncMPU80:
            /* Currenly only Hitachi Type character display is supported      */
            break;

        default:
            break;
    }
}

/**
 * Configures LIDD controller of LCDC and stores the info in channel object
 */
static Void liddChannelConfigLidd(Lidd_ChanObject *hChan, Lidd_DisplayConf *lConf)
{
    assert((NULL != hChan));
    assert((NULL != lConf));

    liddDeviceSetController(hChan, lConf);

    liddDeviceSetCSConf(hChan,lConf);

    liddInitializeDisplay((Lidd_Object *)hChan->instHandle, lConf);

    return;
}


/**
 * Creates a channel on this device
 * This action doesn't start the channel
 * Only IOCTL or submit-
  can start a channel
 * This API assumes that LiddConf is validated and will only check for NULL
 */
static Ptr liddDeviceOpenChannel(Lidd_Object      *instHandle,
                                 Lidd_Controller   controller,
                                 Ptr              *chanConf,
                                 IOM_TiomCallback  appCb,
                                 Ptr               cbArg)
{
    Lidd_ChanObject *hChan      = NULL;

    assert((NULL != chanConf) );
    assert((NULL != instHandle));
    assert((NULL != appCb));
    assert((NULL != cbArg));
    assert((Lidd_NUM_CHAN_PER_INST>instHandle->chCount));
    assert((Lidd_Controller_LIDD == controller));
    /* To remove compiler warnings */
	controller = controller;

    hChan = &instHandle->ChannelObj;
    assert(NULL != hChan);

    instHandle->chCount ++;

    hChan->devInst   = instHandle->instNum;
    hChan->instHandle = (Ptr)instHandle;


    QUE_new(&(hChan->activeQ));

    hChan->appCb = appCb;
    hChan->cbArg = cbArg;
    hChan->enabled   = FALSE;
    hChan->currPos = 0;
    hChan->currSetting = 0;
    hChan->submitCount = 0;
    hChan->activeIOP = NULL;

    liddChannelConfigLidd(hChan, (Lidd_DisplayConf*)chanConf);

    hChan->state = Lidd_DriverState_OPENED;

    /*
    * Change the device state to OPENED if it was CLOSED.
    * Do nothing if the device state was OPENED. TBd : Verify this properly
    */
    instHandle->state = Lidd_DriverState_OPENED;

    return (Ptr)hChan; /* Should be made NULL if some error occurs*/
}


/**
 * Closes the channel identified by hChan
 */
static Void liddDeviceCloseChannel(Lidd_ChanObject *hChan)
{
    assert((NULL != hChan));

    ((Lidd_Object *)hChan->instHandle)->chCount --;

    hChan->state    = Lidd_DriverState_CLOSED;

    if(0 == ((Lidd_Object *)hChan->instHandle)->chCount)
    {
       ((Lidd_Object *)hChan->instHandle)->state = Lidd_DriverState_CLOSED;
    }

    return;
}


/**
 * Validates the Lidd configuration received from the user for boundary
 * values
 */
static Void liddValidateLiddConf(Lidd_DisplayConf * lConf)
{
    assert((NULL != lConf));
	/* To remove compiler warnings */
	lConf = lConf;
    assert((Lidd_DisplayType_SyncMPU68 == lConf->displayType)
            ||(Lidd_DisplayType_AsyncMPU68 == lConf->displayType)
            ||(Lidd_DisplayType_SyncMPU80 == lConf->displayType)
            ||(Lidd_DisplayType_AsyncMPU80 == lConf->displayType)
            ||(Lidd_DisplayType_Hitachi == lConf->displayType));

   return;
}


/**
 * Validates the device configuration values received from the user for
 * boundary conditions
 */
static Void liddValidateDeviceConf(Lidd_DeviceConf * dConf)
{
   assert((NULL != dConf));
   /* to remove compiler warnings */
   dConf = dConf;

   assert((0 != dConf->clkFreqHz));

   return;
}

/**
 * \brief    liddLpscOn
 *
 *           This function is invoked to power on the lidd module and
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
static Int32 liddLpscOn(Lidd_Object *instHandle)
{
    Int32 status = IOM_COMPLETED;

    assert(NULL != instHandle);

    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power on using bios PWRM API                                       */
        status = (Int32)PWRM_setDependency(
                    (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else
        status = Psc_ModuleClkCtrl(
                    (Psc_DevId)instHandle->instHwInfo.pscInstance,
                    (Uint32)instHandle->instHwInfo.pwrmLpscId,
                    TRUE);
#endif
    }

    return (status);
}

/**
 * \brief    liddLpscOff
 *
 *           This function is invoked to power off the Module using either the
 *           PSC variables or BIOS PWRM apis.
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
static Int32 liddLpscOff(Lidd_Object *instHandle)
{
    Int32   status  = IOM_COMPLETED;

    assert(NULL!=instHandle);

    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
#ifdef BIOS_PWRM_ENABLE
        /* power off  using PWRM API                                          */
        status = (Int32)PWRM_releaseDependency(
                     (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else
        status = Psc_ModuleClkCtrl(
                     (Psc_DevId)instHandle->instHwInfo.pscInstance,
                     (Uint32)instHandle->instHwInfo.pwrmLpscId,
                     FALSE);
#endif
    }

    return (status);
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
static Int32 LiddRegisterNotification(Lidd_Object *instHandle)
{
    Uint32       eventCnt = 0;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Lidd_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    &LiddNotifyCallback,
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
static Int32 LiddUnregisterNotification(Lidd_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Lidd_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        if (NULL != instHandle->pwrmInfo.notifyHandle[eventCnt])
        {
            retVal = PWRM_unregisterNotify(
                        instHandle->pwrmInfo.notifyHandle[eventCnt]);

            if (PWRM_SOK != retVal)
            {
                status = IOM_EBADARGS;
                break;
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
static PWRM_NotifyResponse LiddNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg)
{
    Lidd_Object          *instHandle = NULL;
    PWRM_NotifyResponse     pwrmStatus = PWRM_NOTIFYDONE;
    
    assert(NULL != clientArg);

    instHandle = (Lidd_Object *)clientArg;
    assert(NULL != instHandle);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
            pwrmStatus = LiddSuspendCurrentIops(instHandle,eventType);
            break;
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
             /* Suspend the current IOPs, since the VF settings are going to be*
              * changed                                                       */
             if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                     (Lidd_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                     (Lidd_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                pwrmStatus = LiddSuspendCurrentIops(instHandle,eventType);
            }
            break;
        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
             /* The driver shall now reconfigure the clock settings based     *
              * on the new set point of the system                            */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                     (Lidd_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                      (Lidd_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                pwrmStatus = LiddConfigureNewVfSettings(
                                 instHandle,
                                 (Uns)eventArg1,
                                 (Uns)eventArg2);
            }
            /* Fall through is intentional                                    */
        case PWRM_AWAKEFROMSTANDBY:
        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
            LiddRestoreSuspendedIops(instHandle);
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
static PWRM_NotifyResponse LiddConfigureNewVfSettings(
                                                    Lidd_Object   *instHandle,
                                                    Uns              eventArg1,
                                                    Uns              eventArg2
                                                    )
{
    Uint32               preScale = 0x00;
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
        status = LiddCalculateClockSettings(
                     instHandle,
                     eventArg2,
                     instHandle->devConf.clkFreqHz,
                     &preScale);
        /* successfully calculated now check if the prescaler is OK           */
        if ((Lidd_DEV_CLKDIV_MAX >= preScale) && (IOM_COMPLETED == status))
        {
            /* calcualation of the prescaler value is successful.Hence update *
             * the hardware with the new prescaler values                     */
            status = LiddConfigureNewClkSettings(instHandle,preScale);
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
    else
    {
        retVal = PWRM_NOTIFYDONE;
    }

    return (retVal);
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
static PWRM_NotifyResponse LiddSuspendCurrentIops(Lidd_Object  *instHandle,
                                                  PWRM_Event   eventType)
{
    Uint32               hwiKey = 0x00;
    Lidd_ChanObject     *hChan  = NULL;
    PWRM_NotifyResponse  status = PWRM_NOTIFYDONE; 

    assert(NULL != instHandle);
    hChan = &instHandle->ChannelObj;
    assert(NULL != hChan);
        
    hwiKey = _disable_interrupts();

    if (NULL == hChan->activeIOP)
    {
        /* No IOP is pending currently. we can return the status of the       *
         * notify call as sucess                                              */
        instHandle->state = Lidd_DriverState_PWRM_SUSPEND;
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
static Void LiddRestoreSuspendedIops(Lidd_Object *instHandle)
{
    Uint32  hwiKey = 0x00;
    
    assert(NULL != instHandle);
    
    /* Nothing much to be done here. Since the LIDD driver is synchronous, any *
     * no IOP is queued. Just need to update the device state to facilitate    *
     * further requests to the driver                                         */
    hwiKey = _disable_interrupts();
    instHandle->state = Lidd_DriverState_OPENED;    
    _restore_interrupts(hwiKey);
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

static Int32 LiddCalculateConstraints(Lidd_Object *instHandle)
{
    Uns                numberSetpoints    = 0x00;
    Uns                setpoint           = 0x00;
    Uint32             preScale           = 0x00;
    Uint32             mask               = 0x00;
    Int32              status             = IOM_COMPLETED;
    PWRM_Constraint    constraint         = PWRM_DISALLOWEDSLEEPSTATE_MASK;
    PWRM_Status        retVal             = PWRM_SOK;
    PWRM_Domain        domain             = PWRM_CPU;

    assert(NULL != instHandle);

    /* check what clock domain is supported by the device                     */
    if (Lidd_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
    {
        domain     = PWRM_CPU;
        constraint = PWRM_DISALLOWED_CPU_SETPOINT_MASK;
    }
    else
    {
        domain     = PWRM_PER;
        constraint = PWRM_DISALLOWED_PER_SETPOINT_MASK;
    }

    /* get the number of supported setpoint info for this instance            */
    retVal = PWRM_getNumSetpoints(domain, &numberSetpoints);

    if (PWRM_SOK == retVal)
    {
        /* calculate the contraints for the required bus frequency            */
        for (setpoint = 0x00; setpoint < numberSetpoints; setpoint++)
        {
            status = LiddCalculateClockSettings(
                          instHandle,
                          setpoint,
                          instHandle->devConf.clkFreqHz,
                          &preScale);

            if (IOM_COMPLETED == status)
            {
                /* successfully calculated now check if the prescaler is OK   */
                if (Lidd_DEV_CLKDIV_MAX < preScale)
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
static Int32 LiddCalculateClockSettings(Lidd_Object   *instHandle,
                                        Uint32         setpoint,
                                        Uint32         opFreq,
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
    if (Lidd_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
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
                             / instHandle->instHwInfo.inputFrequency);

            /* get the information about the current set point                */
            retVal = PWRM_getSetpointInfo(
                         domain,
                         setpoint,
                         &frequency,
                         &voltage);

            if (PWRM_SOK == retVal)
            {
                /* calculate the new prescaler values                         */
                (*prescale) = ((((frequency * 1000)/sysclkDiv)/opFreq));
            }
        }
    }

    if (PWRM_SOK != retVal)
    {
        status = IOM_EBADMODE;
    }

    return (status);
}



/*
 * \brief     This function configures the LCDC device with the new prescalar
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
static Int32 LiddConfigureNewClkSettings(Lidd_Object *instHandle,
                                        Uint32       prescale)
{
    CSL_LcdcRegsOvly  lcdcRegs  = NULL;
#ifdef BIOS_PWRM_ENABLE
    Int32             status    = 0;
#endif
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;

    assert(NULL != lcdcRegs);
#ifdef BIOS_PWRM_ENABLE
    status = (Int32)
        PWRM_setDependency((PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#endif
    lcdcRegs->LCD_CTRL = \
        ((lcdcRegs->LCD_CTRL & ~CSL_LCDC_LCD_CTRL_CLKDIV_MASK) |
         ((prescale << CSL_LCDC_LCD_CTRL_CLKDIV_SHIFT) &
           CSL_LCDC_LCD_CTRL_CLKDIV_MASK));
#ifdef BIOS_PWRM_ENABLE
    status = (Int32)
        PWRM_releaseDependency((PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#endif
    return status;
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
static Int32 LiddUnregisterConstraints(Lidd_Object *instHandle)
{
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;
    Uint32       count  = 0x00;

    assert(NULL != instHandle);
    
    for (count = 0; count < Lidd_MAX_PWRM_CONSTRAINTS; count++)
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
        }
    }
    return (status);
}
#endif /*BIOS_PWRM_ENABLE*/
/*!
*! Revision History
*! ================
*! 19-Dec-2008 connell created from IDriver Lidd.c
*!
*/
