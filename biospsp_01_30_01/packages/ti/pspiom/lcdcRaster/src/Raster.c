/*
 * Raster.c
 *
 * This file contains Driver Layer Interface implementation for the LCDC Raster
 * Driver. LCDC Raster Driver provides Driver Layer Interface to do operations
 * on the LCDC Raster peripheral like device initialization, channel creation,
 * control commands for peripheral specific operations etc
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
 * \file       Raster.c
 * \brief      This file exposes the interfaces, based on PSP Arch.,
 *             which can be used by the applicaitons to program the LCDC h/w.
 *
 * \author     PSG, Steve Connell, Madhvapathi Sriram
 * \version    0.1  created from IDriver Raster.c
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <string.h>
#include <assert.h>

#include <mem.h>
#include <que.h>
#include <iom.h>
#include <ecm.h>
#include <hwi.h>
#include <c64.h>

#include "ti/pspiom/psc/Psc.h"
#include "ti/pspiom/lcdcraster/Raster.h"
#include "RasterLocal.h"

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#ifdef CHIP_C6747
#include "ti/pspiom/cslr/soc_C6747.h"
#define RASTER_DEFAULT_HWI_NUM 9
#endif

#ifdef CHIP_OMAPL137
#include "ti/pspiom/cslr/soc_OMAPL137.h"
#define RASTER_DEFAULT_HWI_NUM 9
#endif

#ifdef CHIP_C6748
#include "ti/pspiom/cslr/soc_C6748.h"
#define RASTER_DEFAULT_HWI_NUM 8
#ifdef BIOS_PWRM_ENABLE
#include <pwrm.h>
#endif
#endif

#ifdef CHIP_OMAPL138
#include "ti/pspiom/cslr/soc_OMAPL138.h"
#define RASTER_DEFAULT_HWI_NUM 8
#endif

/* -------- constants -------- */
const struct Raster_Params RasterDefault_PARAMS = {
    {                               /* DeviceConf struct    */
        5000000,                    /* clkFreqHz            */
        Raster_OpMode_DMAINTERRUPT, /* opMode               */
        0,                          /* hwiNum               */
        {                           /* DmaConfig struct     */
            Raster_DmaFb_DOUBLE,    /* fbMode               */
            Raster_DmaBurstSize_4,  /* burstSize            */
            FALSE,                  /* bigEndian            */
            TRUE,                   /* eofInt               */
        },
        FALSE,
        Raster_pllDomain_0
        
    }
};

/* ========================================================================== */
/*                       GLOBAL MODULE STATE                                  */
/* ========================================================================== */

static Bool inUse[CSL_LCDC_PER_CNT];

static Raster_Module_State Raster_module = {&inUse[0]};

static Raster_Object Raster_Instances[CSL_LCDC_PER_CNT];

static Raster_HwInfo Raster_deviceInstInfo[CSL_LCDC_PER_CNT];

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */

static Void rasterGetConf(Raster_ChanObject *hChan, Raster_RasterConf *rasterConf);

static Void rasterSetDisplayO(Raster_ChanObject *hChan, Raster_DataOrder rasterDO);

static Void rasterSetDisplay(Raster_ChanObject *hChan,  Raster_Display  rasterD);

static Void rasterSetIntrF(Raster_ChanObject *hChan,  Raster_Intface rasterIF);

static Void rasterFreeFB(Raster_ChanObject *hChan, Raster_FrameBuffer *fbPtr);

static Int32 rasterDeviceSetClk(Raster_Object* instHandle, Uint32 clkFreqHz);

static Void rasterSetOF(Raster_ChanObject *hChan,  Raster_Output rasterOF);

static Void rasterSetPanel(Raster_ChanObject *hChan, Raster_Panel rasterP);

static Void rasterMaskInterrupt(Raster_ChanObject *hChan, Uint32 rasterI);

static Void rasterAddEvtNotify(Raster_ChanObject *hChan, Uint32 *event);

static Void rasterRemEvtNotify(Raster_ChanObject *hChan, Uint32 *event);

static Void rasterSetNibbleMode(Raster_ChanObject *hChan, Bool rasterN);

static Void rasterSetFDD(Raster_ChanObject *hChan, Uint32 rasterFDD);

static Void rasterDeviceValidateConf(const Raster_DeviceConf * dConf);
#ifdef BIOS_PWRM_ENABLE
static Int32 RasterCalculateConstraints(Raster_Object *instHandle);

static Int32 RasterUnregisterConstraints(Raster_Object *instHandle);

static Int32 RasterUnregisterNotification(Raster_Object *instHandle);

static Int32 RasterRegisterNotification(Raster_Object *instHandle);
#endif
static Void rasterValidateConf(const Raster_RasterConf * rConf);

static Void rasterSetIntr(Raster_ChanObject *hChan,Uint32 rasterI);

static Int32 rasterDeviceCloseChannel(Raster_ChanObject *const hChan);

static Void rasterDmaSetEofInterrupt(Raster_Object *instHandle);

static inline Void rasterDeviceReset(Raster_Object *instHandle);

static Int32 rasterResetController(Raster_Object *instHandle);

static Void rasterDeviceDeleteInst(Raster_Object *instHandle);

static Void rasterDmaSetBurstSize(Raster_Object *instHandle);

static Void rasterClearRasterConf(Raster_ChanObject *hChan);

static Void rasterUnregIntHandler(Raster_Object *instHandle);

static Void rasterDmaSetEndian(Raster_Object *instHandle);

static Void rasterDmaSetFbMode(Raster_Object *instHandle);

static Void rasterRegIntHandler(Raster_Object* instHandle);

static Void rasterDeviceEnable(Raster_ChanObject *hChan);

static Void rasterResetRaster(Raster_Object *instHandle);

static Void rasterDeviceDisable(Raster_ChanObject *hChan);

static Void rasterSetFBContent(Raster_ChanObject *hChan,
                               Raster_FBContent rasterFBC);
#ifdef BIOS_PWRM_ENABLE
static PWRM_NotifyResponse RasterConfigureNewVfSettings(
                                                Raster_Object   *instHandle,
                                                Uns              eventArg1,
                                                Uns              eventArg2);

static PWRM_NotifyResponse RasterNotifyCallback(PWRM_Event    eventType,
                                                Arg           eventArg1,
                                                Arg           eventArg2,
                                                Arg           clientArg);

static Int32 RasterCalculateClockSettings(Raster_Object *instHandle,
                                       Uint32         setpoint,
                                       Uint32         opFreq,
                                       Uint32        *prescale);

static Int32 RasterConfigureNewClkSettings(Raster_Object *instHandle,
                                          Uint32         prescale);
#endif
static Void rasterDeviceOpenChannel(Raster_Object    *instHandle,
                                   Raster_Controller  controller,
                                   Ptr               *chanConf);

static Void rasterDeviceSetController(Raster_ChanObject *hChan,
                                      Raster_Controller  controller);

static Void rasterSetBitsPP(Raster_ChanObject *hChan,
                            Raster_FbBitsPerPixel bitsPP);

static Void rasterGetSubpanel(Raster_ChanObject *hChan,
                              Raster_Subpanel *subpanel);

static Int rasterDeviceCreateInst(Raster_Object* instHandle,
                                  const Raster_Params *params);

static Void rasterChannelConfigRaster(Raster_ChanObject *hChan,
                                      Raster_RasterConf *rConf);

static Int32 rasterAllocFB(Raster_ChanObject *hChan,
                           Raster_FrameBuffer **fbPtrPtr);

static Void rasterSetSubpanel(Raster_ChanObject *hChan,
                              Raster_SubpanelCmd cmd,
                              Void *conf);

static Int32 rasterEnqueueIop(Raster_ChanObject *hChan,
                              IOM_Packet *ioPacket);

static Void rasterSetTiming2(Raster_ChanObject *hChan,
                             Raster_Timing2 *rasterT2);

static Void rasterDmaSetConf(Raster_ChanObject *hChan,
                             Raster_DmaConfCmd  cmd,
                             Void *conf);

static Void rasterSetHParam(Raster_ChanObject *hChan,
                            Uint32      hBP,
                            Uint32      hFP,
                            Uint32      hSPW,
                            Uint32      pPL);

static Void rasterSetVParam(Raster_ChanObject *hChan,
                           Uint32      vBP,
                           Uint32      vFP,
                           Uint32      vSPW,
                           Uint32      lPP);


static Int32 rasterLpscOff(Raster_Object *instHandle);


static Int32 rasterLpscOn(Raster_Object *instHandle);

static inline Void rasterDmaGetConf(Raster_ChanObject *hChan,
                                     Raster_DmaConfig *dmaConf);

static inline Void rasterDeviceReadId(Raster_ChanObject *hChan, Uint32 *revId);

static inline Void rasterDeviceClearEvtStat(Raster_ChanObject *hChan);

static inline Void rasterDmaReset(Raster_Object *instHandle);

static inline Void rasterDeviceGetEvtStat(Raster_ChanObject *hChan,
                                          Raster_EventStat *eventStat);

static inline Void rasterDeviceGetConf(Raster_ChanObject *hChan,
                                       Raster_DeviceConf  *devConf);

Void rasterIntrHandler(Arg isrArg);

/* ========================================================================== */
/*                           MINI DRIVER FUNCTIONS                            */
/* ========================================================================== */

static Int rasterMdBindDev(Ptr *devp, Int devId, Ptr devParams);

static Int rasterMdUnBindDev(Ptr devp);

static Int rasterMdCreateChan(Ptr                *chanp,
                              Ptr                 devp,
                              String              name,
                              Int                 mode,
                              Ptr                 chanParams,
                              IOM_TiomCallback    cbFxn,
                              Ptr                 cbArg);

static Int rasterMdDeleteChan(Ptr chanp);

static Int rasterMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);

static Int rasterMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs);

const IOM_Fxns Raster_IOMFXNS =
{
    &rasterMdBindDev,
    &rasterMdUnBindDev,
    &rasterMdControlChan,
    &rasterMdCreateChan,
    &rasterMdDeleteChan,
    &rasterMdSubmitChan,
};

/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */

/*
 *  \brief  Function called by BIOS during instance initialisation
 *
 */
Void Raster_init(Void)
{
    Int i;

    for (i = 0; i < CSL_LCDC_PER_CNT; i++)
    {
        /* have to initialize statically */
        Raster_module.inUse[i] = FALSE;
        memset((Void *)&Raster_Instances[i], 0x0, sizeof(Raster_Object));
    }
}

/* ========================================================================== */
/*                           IOM FUNCTIONS                                    */
/* ========================================================================== */

/*
 *  \fn     static Int rasterMdBindDev()
 *
 *  \brief  Function called by Bios during instance initialisation
 *
 *  \param  devp       [OUT]      pointer to Lcdc driver object
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
static Int rasterMdBindDev(Ptr *devp, Int devId, Ptr devParams)
{
    Int              status     = IOM_COMPLETED;
    Raster_Params   *params     = NULL;
    Raster_Object   *instHandle = NULL;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* check if the current instance is within the limit supported by the H/W */
    if ((NULL == devParams) || (CSL_LCDC_PER_CNT <= devId) || (0 > devId))
    {
        status = IOM_EBADARGS;
    }
    else
    {
#endif
        params = (Raster_Params*)devParams;
        instHandle =  &Raster_Instances[devId];
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if((NULL != &Raster_Instances[devId]) &&
           (NULL != params) &&
           (TRUE != Raster_module.inUse[devId]))
        {
            status = IOM_COMPLETED;
        }
        else
        {
            status = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == status)
    {
        Raster_module.inUse[devId] = TRUE;

#if defined(CHIP_C6747) || defined(CHIP_OMAPL137) || defined(CHIP_C6748) || defined(CHIP_OMAPL138)
        if (0 == devId)
        {
            Raster_deviceInstInfo[0].baseAddress    =
                (CSL_LcdcRegsOvly)CSL_LCDC_0_REGS;
            Raster_deviceInstInfo[0].cpuEventNumber =
                (Uint32)CSL_INTC_EVENTID_LCDC_INT0;
            Raster_deviceInstInfo[0].inputFrequency = 
                (Uint32)CSL_LCDC_0_MODULE_FREQ;
            Raster_deviceInstInfo[0].hwiNumber      =
                (Uint32)RASTER_DEFAULT_HWI_NUM;
            Raster_deviceInstInfo[0].pscInstance    =
                (Uint32)CSL_LPSC_INST_LCDC_0;

#ifdef BIOS_PWRM_ENABLE
            Raster_deviceInstInfo[0].pwrmLpscId = (Uint32)CSL_LPSC_NUMBER_LCDC_0;
#else
            Raster_deviceInstInfo[0].pwrmLpscId = (Uint32)CSL_PSC_LCDC;
#endif

        }
#endif  /* defined(CHIP_C6747) || defined(CHIP_OMAPL137) */

        rasterDeviceValidateConf(&params->devConf);

        instHandle->instNum = (Uint32)devId;

        instHandle->instHwInfo.baseAddress     =
            Raster_deviceInstInfo[devId].baseAddress;
        instHandle->instHwInfo.cpuEventNumber  =
            Raster_deviceInstInfo[devId].cpuEventNumber;
        instHandle->instHwInfo.inputFrequency  =
            Raster_deviceInstInfo[devId].inputFrequency;
        instHandle->instHwInfo.pscInstance     =
            Raster_deviceInstInfo[devId].pscInstance;
        instHandle->instHwInfo.pwrmLpscId      =
            Raster_deviceInstInfo[devId].pwrmLpscId;

        instHandle->instHwInfo.hwiNumber = params->devConf.hwiNum;

        /*Initialize event statistics to zero                             */
        memset(&instHandle->eventStat,0,sizeof(instHandle->eventStat));

        /*Initialize Channel configuration                                */
        instHandle->ChannelObj.dmaThrsld = 0;
        instHandle->ChannelObj.devInst = (Uint32)devId;
        instHandle->ChannelObj.type = Raster_Controller_RASTER;
        instHandle->ChannelObj.state = Raster_DriverState_DELETED;
        instHandle->ChannelObj.chanConf.outputFormat =
            Raster_Output_OUTPUT_UNDEFINED;
        instHandle->ChannelObj.chanConf.intface =
            Raster_Intface_INTERFACE_UNDEFINED;
        instHandle->ChannelObj.chanConf.panel =
            Raster_Panel_UNDEFINED;
        instHandle->ChannelObj.chanConf.display =
            Raster_Display_UNDEFINED;
        instHandle->ChannelObj.chanConf.bitsPP =
            Raster_FbBitsPerPixel_UNDEFINED;
        instHandle->ChannelObj.chanConf.fbContent =
            Raster_FBContent_UNDEFINED;
        instHandle->ChannelObj.chanConf.dataOrder =
            Raster_DataOrder_UNDEFINED;
        instHandle->ChannelObj.chanConf.nibbleMode = FALSE;
        instHandle->ChannelObj.chanConf.subPanel.enable = FALSE;
        instHandle->ChannelObj.chanConf.subPanel.position =
            Raster_SubpanelPos_UNDEFINED;
        instHandle->ChannelObj.chanConf.subPanel.lPPT = 0;
        instHandle->ChannelObj.chanConf.subPanel.defaultData = 0;
        instHandle->ChannelObj.chanConf.timing2.iAcbias = FALSE;
        instHandle->ChannelObj.chanConf.timing2.iPclk = FALSE;
        instHandle->ChannelObj.chanConf.timing2.iLclk = FALSE;
        instHandle->ChannelObj.chanConf.timing2.iFclk = FALSE;
        instHandle->ChannelObj.chanConf.timing2.syncCtrl = FALSE;
        instHandle->ChannelObj.chanConf.timing2.syncEdge =
            Raster_SyncEdge_UNDEFINED;
        instHandle->ChannelObj.chanConf.timing2.acBiasInt = 0;
        instHandle->ChannelObj.chanConf.timing2.acBiasFreq = 0;
        instHandle->ChannelObj.chanConf.fifoDmaDelay = 0;
        instHandle->ChannelObj.chanConf.intMask = 0;
        instHandle->ChannelObj.chanConf.hFP = 0;
        instHandle->ChannelObj.chanConf.hBP = 0;
        instHandle->ChannelObj.chanConf.hSPW = 0;
        instHandle->ChannelObj.chanConf.pPL = 0;
        instHandle->ChannelObj.chanConf.vFP = 0;
        instHandle->ChannelObj.chanConf.vBP = 0;
        instHandle->ChannelObj.chanConf.vSPW = 0;
        instHandle->ChannelObj.chanConf.lPP = 0;
        instHandle->ChannelObj.appCb = NULL;
        instHandle->ChannelObj.cbArg = NULL;
        instHandle->ChannelObj.submitCount = 0;
        instHandle->ChannelObj.enabled = FALSE;
        instHandle->ChannelObj.fbSize = 0;
        instHandle->ChannelObj.instHandle = NULL;
        instHandle->ChannelObj.segId = 0;

        /* Create the pending and active queues here.                     */
        QUE_new(&(instHandle->ChannelObj.pendingQ));
        QUE_new(&(instHandle->ChannelObj.activeQ));

        /* Clear the active and pending packet counts                     */
        instHandle->ChannelObj.pendingPktCount = 0;
        instHandle->ChannelObj.activePktCount = 0;
         
        status = rasterDeviceCreateInst(instHandle, params);

        if (IOM_COMPLETED == status)
        {
            rasterRegIntHandler(instHandle);
        }

        if ((IOM_COMPLETED == status) &&
            (FALSE == instHandle->devConf.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            status = (Int32)PWRM_setDependency(
                        (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else
            status = Psc_ModuleClkCtrl(
                        (Psc_DevId)instHandle->instHwInfo.pscInstance,
                        (Uint32)instHandle->instHwInfo.pwrmLpscId,
                        TRUE);
#endif
        }
    }

#ifdef BIOS_PWRM_ENABLE
        if ((IOM_COMPLETED == status)
            && (TRUE == instHandle->devConf.pscPwrmEnable))
        {
            /* register the notify function for the PWRM events               */
            status = RasterRegisterNotification(instHandle);
        }
#endif

    if (IOM_COMPLETED == status)
    {
        *devp = instHandle;
    }
    else
    {
        *devp = NULL;
    }

    return status;
}


/**
 *  \fn     static Int rasterMdUnBindDev()
 *
 *  \brief  Function called by Bios during closing of the instance
 *
 *  \param  devp       [IN]  pointer to Lcdc driver object
 *
 *  \return success code
 *
 *  \enter  devp       must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int rasterMdUnBindDev(Ptr devp)
{
    Int32 retVal = IOM_COMPLETED;
    Raster_Object *instHandle   = NULL;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* Check if this is a valid handle                                        */
   if (NULL != devp)
   {
#endif
       instHandle = (Raster_Object*)devp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
       /* Validate instance ID                                                */
       if ((Raster_DriverState_CLOSED != instHandle->state) ||
           (CSL_LCDC_PER_CNT <= instHandle->instNum))
       {
           retVal = IOM_EBADARGS;
       }
    }
    else
    {
        retVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == retVal)
    {
        rasterUnregIntHandler(instHandle);

        retVal = rasterLpscOn(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            rasterDeviceDeleteInst(instHandle);
            retVal = rasterLpscOff(instHandle);
        }

        /* Empty the lists                                                    */
        QUE_delete(&(instHandle->ChannelObj.pendingQ));
        QUE_delete(&(instHandle->ChannelObj.activeQ));

        /* set state to deleted                                               */
        instHandle->state  = Raster_DriverState_DELETED;

        /* set the module state to not in use                                 */
        Raster_module.inUse[instHandle->instNum] = FALSE;

        if ((IOM_COMPLETED == retVal) &&
            (FALSE == instHandle->devConf.pscPwrmEnable))
        {
#ifdef BIOS_PWRM_ENABLE
            /* power on using bios PWRM API                                   */
            retVal = (Int32)PWRM_releaseDependency(
                        (PWRM_Resource)instHandle->instHwInfo.pwrmLpscId);
#else
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
            retVal = RasterUnregisterNotification(instHandle);
        }
#endif
    }
    return (retVal);
}

/**
 *  \fn     static Ptr rasterMdCreateChan()
 *
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the McASP device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions.it returns configured channel handle to application.
 *          which will be used in all further transactions with the channel.
 *
 *          This function is called in response to a SIO_create call.
 *  \param     chanp        [OUT]    channel object pointer
 *  \param     devp         [IN]     pointer to Lcdc instance
 *  \param     name         [IN]     Lcdc Instance name like raster0
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
static Int rasterMdCreateChan(Ptr        *chanp,
                              Ptr                 devp,
                              String              name,
                              Int                 mode,
                              Ptr                 chanParams,
                              IOM_TiomCallback    cbFxn,
                              Ptr                 cbArg)
{
    Raster_ChanParams    *lcdParams  = NULL;
    Raster_Object        *instHandle = NULL;
    Raster_ChanObject    *hChan      = NULL;
    Bool                  pscPwrOn   = FALSE;
    Int32                 retVal     = IOM_COMPLETED;
    Int32                 status     = IOM_COMPLETED;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp)
       || (IOM_OUTPUT != mode)
       || (NULL == chanParams)
       || (NULL == chanp)
       || (NULL == cbFxn)
       || (NULL == cbArg))
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
        instHandle = (Raster_Object*)devp;
        lcdParams = (Raster_ChanParams*)chanParams;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if((NULL != instHandle) && (NULL != lcdParams))
        {
           if((CSL_LCDC_PER_CNT <= instHandle->instNum)
               || (NULL == ((Raster_ChanParams*)chanParams)->chanConf))
            {
                retVal = IOM_EBADARGS;
            }
        }
        else
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    *chanp = NULL;

    if (IOM_COMPLETED == retVal)
    {
        /* To remove the compiler warnings                                    */
        if((NULL != name) && (mode == mode))
        {
            name = name;
            mode = mode;
        }

        assert(RASTER_NUM_CHAN_PER_INST>instHandle->chCount);

        if (Raster_Controller_RASTER == lcdParams->controller)
        {
            hChan = &(instHandle->ChannelObj);
            assert(NULL != hChan);
            /* copy it for later use*/
            memcpy(&(hChan->chanParams),lcdParams, sizeof(lcdParams));
            hChan->segId = lcdParams->segId;
            hChan->appCb = cbFxn;
            hChan->cbArg = cbArg;
            hChan->enabled   = FALSE;
            hChan->state = Raster_DriverState_OPENED;
            instHandle->chCount ++;
            hChan->devInst   = instHandle->instNum;
            hChan->instHandle = (Ptr)instHandle;
            hChan->type = lcdParams->controller;
            hChan->segId = lcdParams->segId;

            /* validate the channel configuration                             */
            rasterValidateConf((Raster_RasterConf *)lcdParams->chanConf);

            /* power on the raster module here                                */
            retVal = rasterLpscOn(instHandle);

            /* if the raster power on is succesful we will proceed further    */
            if (IOM_COMPLETED == status)
            {
                retVal = rasterResetController(instHandle);
                pscPwrOn = TRUE;
            }

            if (IOM_COMPLETED == retVal)
            {
                rasterDeviceOpenChannel(
                        instHandle,
                        lcdParams->controller,
                        lcdParams->chanConf);

                *chanp = hChan;

                /*
                * Change the device state to OPENED if it was CLOSED.
                * Do nothing if the device state was OPENED
                */
                if((Raster_DriverState_CREATED == instHandle->state)
                   || (Raster_DriverState_CLOSED == instHandle->state))
                {
                   instHandle->state = Raster_DriverState_OPENED;
                }
            }

            /* power OFF the raster module (only if it was powered on earlier)*/
            if (TRUE == pscPwrOn)
            {
                status = rasterLpscOff(instHandle);

                if ((IOM_COMPLETED == retVal) && (IOM_COMPLETED != status))
                {
                    retVal = status;
                    *chanp = NULL;
                    instHandle->state = Raster_DriverState_CLOSED;
                }
            }
        }
    }
    return (retVal);
}

/**
 *  \fn      static Uint rasterMdSubmitChan()
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
static Int rasterMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Raster_ChanObject   *hChan  = NULL;
    Int                  retVal = IOM_COMPLETED;

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
            && ((IOM_ABORT != ioPacket->cmd)))
    {
        retVal = IOM_ENOTIMPL;
    }
    else
    {
#endif
        hChan  = (Raster_ChanObject *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if(NULL == hChan)
        {
            retVal = IOM_EBADARGS;
        }
        else
        {
            retVal = IOM_COMPLETED;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == retVal)
    {
        if (IOM_WRITE == ioPacket->cmd)
        {
            retVal = rasterEnqueueIop(hChan, ioPacket);
            if (IOM_COMPLETED == retVal)
            {
                retVal = IOM_PENDING;
            }
        }
        else
        {
            if ((IOM_FLUSH == ioPacket->cmd) || (IOM_ABORT == ioPacket->cmd))
            {
                rasterDeviceDisable(hChan);
                retVal = IOM_COMPLETED;
            }
        }
    }
    else
    {
        retVal = IOM_EBADIO;
    }

    return (retVal);
}

/**
 * \fn       static Int rasterMdDeleteChan()
 *
 * \brief    This function is called by the application to close a previously
 *           opened channel.it deletes the channel so that it is not available
 *           for further transactions. All the allocated resources are freed
 *           and the channel will be ready for the "open" operation once again.
 *
 *  \param   chanp      [IN]   Handle to the channel.
 *
 *  \return  status
 *
 *  \enter   chanp      must be a valid pointer and should not be null.
 *
 * \leave  Not implemented
 */
static Int rasterMdDeleteChan(Ptr chanp)
{
    Raster_ChanObject *hChan      = NULL;
    Raster_Object     *instHandle = NULL;
    Int32 retVal = IOM_COMPLETED;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if(NULL == chanp)
    {
        retVal = IOM_EBADARGS;
    }
    else
    {
#endif
        hChan = (Raster_ChanObject *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if(NULL != hChan)
        {
#endif
            instHandle = hChan->instHandle;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
            if(NULL != instHandle)
            {
                if((Raster_DriverState_OPENED != instHandle->state)
                   || (Raster_DriverState_OPENED != hChan->state))
                {
                    retVal = IOM_EBADARGS;
                }
            }
            else
            {
                retVal = IOM_EBADARGS;
            }
        }
        else
        {
            retVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if(IOM_COMPLETED == retVal)
    {
        hChan = (Raster_ChanObject *)chanp;
        instHandle = hChan->instHandle;

        retVal = rasterLpscOn(instHandle);

        if (IOM_COMPLETED == retVal)
        {
            retVal = rasterDeviceCloseChannel(hChan);

            if(IOM_COMPLETED == retVal)
            {
                retVal = rasterLpscOff(instHandle);
            }
        }
    }

    return (retVal);
}

/**
 * \fn       static Int rasterMdControlChan()
 *
 * \brief    This function allows the control commands to be sent to the Lcdc
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
static Int rasterMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArgs)
{
    Int32                retVal   = IOM_COMPLETED;
    Int32                status   = IOM_COMPLETED;
    Bool                 pscPwrOn = FALSE;
    Raster_ChanObject   *hChan    = NULL;

    /* Begin parameter checking                                               */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL != chanp)
    {
#endif
        hChan  = (Raster_ChanObject *)chanp;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if(NULL == hChan)
        {
            retVal = IOM_EBADARGS;
        }
        /* ok to check hChan struct members now because we know it's not NULL */
        else
        {
            if (NULL == hChan->instHandle)
            {
                retVal = IOM_EBADARGS;
            }
        }
    }
    else
    {
        retVal = IOM_EBADARGS;
    }
#endif /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    if (IOM_COMPLETED == retVal)
    {
        hChan  = (Raster_ChanObject *)chanp;

        retVal = rasterLpscOn(hChan->instHandle);
    }

    if (IOM_COMPLETED == retVal)
    {
        pscPwrOn = TRUE;

        if (Raster_IOCTL_GET_DEVICE_CONF == cmd)
        {
            /* 1. Get current device configurations */
            rasterDeviceGetConf(hChan, (Raster_DeviceConf*)cmdArgs);
        }
        else if(Raster_IOCTL_GET_RASTER_CONF == cmd)
        {
            /* 2. Get current raster configurations */
            rasterGetConf(hChan, (Raster_RasterConf*)cmdArgs);
        }
        else if(Raster_IOCTL_GET_RASTER_SUBPANEL_CONF == cmd)
        {
            /* 3. Get current raster subpanel configurations */
            rasterGetSubpanel(hChan, (Raster_Subpanel*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_RASTER_SUBPANEL_EN == cmd)
        {
            /* 4. Enable the raster subpanel feature */
            rasterSetSubpanel(hChan,
                              Raster_SubpanelCmd_SET_SPEN,
                              (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_RASTER_SUBPANEL_POS == cmd)
        {
            /* 5. Set the position of the subpanel */
            rasterSetSubpanel(hChan,
                              Raster_SubpanelCmd_SET_POSITION,
                              (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_RASTER_SUBPANEL_LPPT == cmd)
        {
            /* 6. Set the Lines per pixel threshold */
            rasterSetSubpanel(hChan,
                              Raster_SubpanelCmd_SET_LPPT,
                              (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_RASTER_SUBPANEL_DATA == cmd)
        {
            /* 7. Set the default data for the subpanel region */
            rasterSetSubpanel(hChan,
                              Raster_SubpanelCmd_SET_DATA,
                              (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_GET_DMA_CONF == cmd)
        {
            /* 8. Get current DMA configurations */
            rasterDmaGetConf(hChan, (Raster_DmaConfig*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_DMA_FB_MODE == cmd)
        {
            /* 9. Set new DMA Frame buffer mode */
            rasterDmaSetConf(hChan,
                Raster_DmaConfCmd_SET_FB_MODE,
                (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_DMA_BURST_SIZE == cmd)
        {
            /* 10. Set new DMA Burst size */
            rasterDmaSetConf(hChan,
                Raster_DmaConfCmd_SET_BURST_SIZE,
                (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_SET_DMA_EOF_INT == cmd)
        {
            /* 11. Set new DMA End-of-interrupt values */
            rasterDmaSetConf(hChan,
                Raster_DmaConfCmd_SET_EOF_INT,
                (Void*)cmdArgs);
        }
        else if(Raster_IOCTL_ADD_RASTER_EVENT == cmd)
        {
             /* 12. Add new event notification */
             rasterAddEvtNotify(hChan, (Uint32*)cmdArgs);
        }
        else if(Raster_IOCTL_REM_RASTER_EVENT == cmd)
        {
            /* 13. Remove specified event notifications */
            rasterRemEvtNotify(hChan, (Uint32*)cmdArgs);
        }
        else if(Raster_IOCTL_GET_EVENT_STAT == cmd)
        {
            /* 14. Get current event statistics */
            rasterDeviceGetEvtStat(hChan, (Raster_EventStat*)cmdArgs);
        }
        else if(Raster_IOCTL_CLEAR_EVENT_STAT == cmd)
        {
            /* 15. Clear all event statistics */
            rasterDeviceClearEvtStat(hChan);
        }
        else if(Raster_IOCTL_RASTER_ENABLE == cmd)
        {
            /* 16. Enable raster controller */
            rasterDeviceEnable(hChan);
        }
        else if(Raster_IOCTL_RASTER_DISABLE == cmd)
        {
            /* 17. Disable raster controller */
            rasterDeviceDisable(hChan);
        }
        else if(Raster_IOCTL_GET_DEVICE_VERSION == cmd)
        {
            /* 18. Get device id */
            rasterDeviceReadId(hChan, (Uint32*)cmdArgs);
        }
        else if(Raster_IOCTL_ALLOC_FB == cmd)
        {
            Raster_FrameBuffer *fbPtr  = NULL;
            
            assert(NULL != cmdArgs);

            fbPtr = (Raster_FrameBuffer *)cmdArgs;
            retVal = rasterAllocFB(hChan, (Raster_FrameBuffer **)fbPtr);
        }
        else if(Raster_IOCTL_FREE_FB == cmd)
        {
            Raster_FrameBuffer *fbPtr  = NULL;

            fbPtr = (Raster_FrameBuffer *)cmdArgs;

            rasterFreeFB(hChan, fbPtr);
        }
        else
        {
            retVal = IOM_EBADARGS;
        }
    }

    if (TRUE == pscPwrOn)
    {
        status = rasterLpscOff(hChan->instHandle);

        if (IOM_COMPLETED == retVal)
        {
            retVal  = status;
        }
    }

    return (retVal);
}

/**
 * Allocates the frame buffer from the memory segment reserved for LCDC
 */
static Int32 rasterAllocFB(Raster_ChanObject *hChan,
                           Raster_FrameBuffer **fbPtrPtr)
{
    Int32               retVal   = IOM_COMPLETED;
    Raster_FrameBuffer   *newFbPtr = NULL;


    assert(NULL != hChan);
    assert(NULL != fbPtrPtr);

    newFbPtr = *fbPtrPtr;
    /* Alloc the memory aligned to cache line*/
    newFbPtr = MEM_alloc(hChan->segId,
                         (size_t)sizeof(Raster_FrameBuffer),
                         (size_t)128);

    if(NULL == newFbPtr)
    {
        retVal = IOM_EALLOC;
    }

    if(IOM_COMPLETED == retVal)
    {
        newFbPtr->frameBufferPtr = MEM_alloc(hChan->segId,
                                             (size_t)hChan->fbSize,
                                             (size_t)128);
        if(NULL == newFbPtr->frameBufferPtr)
        {
            MEM_free(hChan->segId, (Ptr)newFbPtr, sizeof(Raster_FrameBuffer));
            retVal = IOM_EALLOC;
        }
    }

    if(IOM_COMPLETED == retVal)
    {
        *fbPtrPtr = newFbPtr;
        retVal = IOM_COMPLETED;
    }
    else
    {
        *fbPtrPtr   = NULL;
    }

   return (retVal);
}

/**
 * Free's the frame buffer from the segment
 */
static Void rasterFreeFB(Raster_ChanObject *hChan, Raster_FrameBuffer *fbPtr)
{
    assert(NULL != hChan);
    assert(NULL != fbPtr);

    MEM_free(hChan->segId, (Ptr)fbPtr->frameBufferPtr, hChan->fbSize);

    MEM_free(hChan->segId, (Ptr)fbPtr, sizeof(Raster_FrameBuffer));

    return;
}



/**
 * Selects the controller (here, either raster or lidd)
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDeviceSetController(Raster_ChanObject *hChan,
                                      Raster_Controller  controller)
{
    Raster_Object       *instHandle;
    CSL_LcdcRegsOvly     lcdcRegs;
    assert(NULL != hChan);
    assert(Raster_Controller_RASTER == controller);

    instHandle = (Raster_Object *)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    lcdcRegs->LCD_CTRL = \
        ((lcdcRegs->LCD_CTRL & ~CSL_LCDC_LCD_CTRL_MODESEL_MASK) |
         ((CSL_LCDC_LCD_CTRL_MODESEL_RASTER << CSL_LCDC_LCD_CTRL_MODESEL_SHIFT)&
          CSL_LCDC_LCD_CTRL_MODESEL_MASK));

    hChan->type = (Raster_Controller) controller;

    return;
}


/**
 * Reads the current device configuration options
 */
static inline Void rasterDeviceGetConf(Raster_ChanObject *hChan,
                                      Raster_DeviceConf  *devConf)
{
    assert(NULL != hChan);
    assert(NULL != devConf);

    memcpy(devConf,
        &((Raster_Object *)hChan->instHandle)->devConf,
        sizeof(Raster_DeviceConf));

    return;
}


/**
 * Clear the event statistics structure associated with a given channel
 * This function updates the eventstat object. The caller is expected to take
 * care of concurrent access policy
 */
static inline Void rasterDeviceClearEvtStat(Raster_ChanObject *hChan)
{
    assert(NULL != hChan);

    memset(&((Raster_Object *)hChan->instHandle)->eventStat,
        0,
        sizeof(Raster_EventStat));

    return;
}


/**
 * Returns the event statistics information collected to the application
 */
static inline Void rasterDeviceGetEvtStat(Raster_ChanObject *hChan,
                                          Raster_EventStat *eventStat)
{
    assert(NULL != hChan);
    assert(NULL != eventStat);

    memcpy(eventStat,
        &((Raster_Object *)hChan->instHandle)->eventStat,
        sizeof(Raster_EventStat));

    return;
}


/**
 * Reads the device identification register
 */
static inline Void rasterDeviceReadId(Raster_ChanObject *hChan, Uint32 *revId)
{
    assert(NULL != hChan);
    assert(NULL != revId);

    *revId = ((Raster_Object *)hChan->instHandle)->instHwInfo.baseAddress->REV;

    return;
}

/**
 * Sets the clock to the given frequency
 * This function is platform dependant
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Int32 rasterDeviceSetClk(Raster_Object* instHandle, Uint32 clkFreqHz)
{
    Int32               retVal     = IOM_COMPLETED;
    Uint32              prescale   = 0;
#ifdef BIOS_PWRM_ENABLE
    Uns                 setpoint   = 0x00;
    PWRM_Domain         domain     = PWRM_CPU;
#endif
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

#ifdef BIOS_PWRM_ENABLE
    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
        /* check what clock domain is supported by the device                     */
        if (Raster_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
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
            retVal = RasterCalculateClockSettings(
                          instHandle,
                          setpoint,
                          instHandle->devConf.clkFreqHz,
                          &prescale);
       }
    }
    else
    {
#endif
        prescale = instHandle->instHwInfo.inputFrequency/clkFreqHz;
#ifdef BIOS_PWRM_ENABLE
    }
#endif

    if(((RASTER_DEV_CLKDIV_MIN <= prescale)
            && (RASTER_DEV_CLKDIV_MAX >= prescale)) && (IOM_COMPLETED == retVal))
    {
        lcdcRegs->LCD_CTRL = \
            ((lcdcRegs->LCD_CTRL & ~CSL_LCDC_LCD_CTRL_CLKDIV_MASK) |
             ((prescale << CSL_LCDC_LCD_CTRL_CLKDIV_SHIFT) &
               CSL_LCDC_LCD_CTRL_CLKDIV_MASK));

            instHandle->devConf.clkFreqHz = clkFreqHz;
    }
    else
    {
        retVal = IOM_EBADARGS;
    }

    return retVal;
}


/**
 * Selects the Dma operation mode as single or dual buffer
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDmaSetFbMode (Raster_Object *instHandle)
{
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != instHandle);
    assert((Raster_DmaFb_SINGLE == instHandle->devConf.dma.fbMode) ||
           (Raster_DmaFb_DOUBLE == instHandle->devConf.dma.fbMode));

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(Raster_DmaFb_SINGLE == instHandle->devConf.dma.fbMode)
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_ONE_FB <<
               CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_SHIFT)&
              CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_MASK));
    }
    else
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_TWO_FB <<
               CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_SHIFT)&
              CSL_LCDC_LCDDMA_CTRL_FRAME_MODE_MASK));
    }
    return;
}


/**
 * Sets the Burst size of the Dma transfers
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDmaSetBurstSize(Raster_Object *instHandle)
{

    CSL_LcdcRegsOvly lcdcRegs;

    assert(NULL != instHandle);
    assert((Raster_DmaBurstSize_1 == instHandle->devConf.dma.burstSize) ||
           (Raster_DmaBurstSize_2 == instHandle->devConf.dma.burstSize) ||
           (Raster_DmaBurstSize_4 == instHandle->devConf.dma.burstSize) ||
           (Raster_DmaBurstSize_8 == instHandle->devConf.dma.burstSize) ||
           (Raster_DmaBurstSize_16 == instHandle->devConf.dma.burstSize));

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(instHandle->devConf.dma.burstSize)
    {
        case Raster_DmaBurstSize_1:
            lcdcRegs->LCDDMA_CTRL = \
                ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK) |
                 ((CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_ONE <<
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SHIFT) &
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK));
                break;
        case Raster_DmaBurstSize_2:
            lcdcRegs->LCDDMA_CTRL = \
                ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK) |
                 ((CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_TWO <<
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SHIFT) &
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK));
                break;
        case Raster_DmaBurstSize_4:
            lcdcRegs->LCDDMA_CTRL = \
                ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK) |
                 ((CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_FOUR <<
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SHIFT) &
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK));
                break;
        case Raster_DmaBurstSize_8:
            lcdcRegs->LCDDMA_CTRL = \
                ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK) |
                 ((CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_EIGHT <<
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SHIFT) &
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK));
                break;
        case Raster_DmaBurstSize_16:
            lcdcRegs->LCDDMA_CTRL = \
                ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK) |
                 ((CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SIXTEEN <<
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_SHIFT) &
                   CSL_LCDC_LCDDMA_CTRL_BURST_SIZE_MASK));
                break;
        default:
                break;
    }
    return;
}


/**
 * Sets the endianness of the Dma
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDmaSetEndian(Raster_Object *instHandle)
{
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != instHandle);
    assert((TRUE == instHandle->devConf.dma.bigEndian )||
           (FALSE == instHandle->devConf.dma.bigEndian));

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(FALSE == instHandle->devConf.dma.bigEndian)
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_DISABLE <<
               CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_SHIFT) &
               CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_MASK));
    }
    else
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_ENABLE <<
               CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_SHIFT) &
               CSL_LCDC_LCDDMA_CTRL_BIGENDIAN_MASK));
    }

   return;
}


/**
 * Enables/Disables the end-of-frame interrupt for the device
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDmaSetEofInterrupt(Raster_Object *instHandle)
{
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL!=instHandle);
    assert((TRUE == instHandle->devConf.dma.eofInt) ||
           (FALSE == instHandle->devConf.dma.eofInt));

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(TRUE == instHandle->devConf.dma.eofInt)
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_ENABLE <<
               CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_SHIFT) &
               CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_MASK));
    }
    else
    {
        lcdcRegs->LCDDMA_CTRL = \
            ((lcdcRegs->LCDDMA_CTRL & ~CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_MASK) |
             ((CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_DISABLE <<
               CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_SHIFT) &
               CSL_LCDC_LCDDMA_CTRL_EOF_INTEN_MASK));
    }

   return;
}


/**
 * Reads the current Dma configuration
 * This function updates the device object. The caller is expected to take
 * care of concurrent access policy
 */
static inline Void rasterDmaGetConf(Raster_ChanObject *hChan,
                                    Raster_DmaConfig *dmaConf)
{
    Raster_Object *instHandle;

    assert(NULL!=hChan);
    assert(NULL!=dmaConf);

    instHandle = hChan->instHandle;
    assert(NULL != instHandle);

    memcpy(dmaConf, &(instHandle->devConf.dma), sizeof(Raster_DmaConfig));

    return;
}


/**
 * Configures the internal Dma of the LCDC device
 */
static Void rasterDmaSetConf(Raster_ChanObject *hChan,
                             Raster_DmaConfCmd  cmd,
                             Void *conf)
{
    assert(NULL!=hChan);
    assert(NULL!=conf);
    assert((Raster_DmaConfCmd_SET_FB_MODE    == cmd) ||
           (Raster_DmaConfCmd_SET_BURST_SIZE == cmd) ||
           (Raster_DmaConfCmd_SET_EOF_INT    == cmd));

    switch(cmd)
    {
        case Raster_DmaConfCmd_SET_FB_MODE:
            ((Raster_Object *)hChan->instHandle)->devConf.dma.fbMode =
                    *(Raster_DmaFb *)conf;
            rasterDmaSetFbMode(((Raster_Object *)hChan->instHandle));
            break;

        case  Raster_DmaConfCmd_SET_BURST_SIZE:
            ((Raster_Object *)hChan->instHandle)->devConf.dma.burstSize =
                    *(Raster_DmaBurstSize *)conf;
            rasterDmaSetBurstSize(((Raster_Object *)hChan->instHandle));
            break;

        case Raster_DmaConfCmd_SET_EOF_INT:
            ((Raster_Object *)hChan->instHandle)->devConf.dma.eofInt =
                *(Bool *)conf;
            rasterDmaSetEofInterrupt((Raster_Object *)hChan->instHandle);
            break;

        default:
            SYS_error("Unknown Raster_DmaConfCmd arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    return;
}


/**
 * Resets the Dma registers of the LCDC device
 * This function does not update any device structures; update the
 * ds before calling this function (if needed)
 */
static inline Void rasterDmaReset(Raster_Object *instHandle)
{
    assert(NULL!=instHandle);

    instHandle->instHwInfo.baseAddress->LCDDMA_CTRL =
        CSL_LCDC_LCDDMA_CTRL_RESETVAL;
    instHandle->instHwInfo.baseAddress->LCDDMA_FB0_BASE =
        CSL_LCDC_LCDDMA_FB0_BASE_RESETVAL;
    instHandle->instHwInfo.baseAddress->LCDDMA_FB0_CEILING =
        CSL_LCDC_LCDDMA_FB0_CEILING_RESETVAL;
    instHandle->instHwInfo.baseAddress->LCDDMA_FB1_BASE =
        CSL_LCDC_LCDDMA_FB1_BASE_RESETVAL;
    instHandle->instHwInfo.baseAddress->LCDDMA_FB1_CEILING =
        CSL_LCDC_LCDDMA_FB1_CEILING_RESETVAL;

    return;
}


/**
 * Resets the device specific drivers of the LCDC
 * This function does not update the ds
 * Update the ds before calling this function (if needed)
 */
static inline Void rasterDeviceReset(Raster_Object *instHandle)
{
    assert(NULL!=instHandle);

    instHandle->instHwInfo.baseAddress->LCD_CTRL = CSL_LCDC_LCD_CTRL_RESETVAL;
    instHandle->instHwInfo.baseAddress->LCD_STAT = CSL_LCDC_LCD_STAT_RESETVAL;

    return;
}


/**
 * Enqueue's IOP to Active/Free queue
 */
static Int32 rasterEnqueueIop(Raster_ChanObject *hChan,
                              IOM_Packet *ioPacket)
{
    Raster_FrameBuffer      *fbPtr;
    Raster_Object           *instHandle;
    CSL_LcdcRegsOvly         lcdcRegs;
    Int32                    status = IOM_COMPLETED;

    assert(NULL != hChan);
    assert(NULL != hChan->instHandle);
    assert(NULL != ioPacket);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    ECM_disableEvent(instHandle->instHwInfo.cpuEventNumber);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    fbPtr = (Raster_FrameBuffer*)ioPacket->addr;
    assert(NULL != fbPtr);

    /* we are going to program the device.Hence enable the device in the PSC  */
    status = rasterLpscOn(instHandle);

    if (IOM_COMPLETED == status)
    {
        /* Here, if the activeQ is empty directly put it into the activeQ.Else*
         * put it into the pendingQ. Also, we need two frame buffers in case  *
         * of double frame buffer mode                                        */
        if (TRUE == QUE_empty(&(hChan->activeQ)))
        {
            /* Here, if the activeQ is empty directly put it into activeQ. The*
             * reason is that, if the activeQ is empty the there are no       *
             * outstanding requests,so directly enque it here                 */
            QUE_put(&(hChan->activeQ), (Ptr)ioPacket);
            hChan->submitCount++;
            hChan->activePktCount++;

            ECM_enableEvent(instHandle->instHwInfo.cpuEventNumber);

            if (FALSE == hChan->enabled)
            {
                instHandle->instHwInfo.baseAddress->LCDDMA_FB0_BASE =
                    (Uint32)(fbPtr->frameBufferPtr);

                instHandle->instHwInfo.baseAddress->LCDDMA_FB0_CEILING =
                    (Uint32)((Uint32)(fbPtr->frameBufferPtr)
                    +(fbPtr->pitch*fbPtr->lines)
                    +(hChan->dmaThrsld)-4u);

                if (Raster_DmaFb_DOUBLE == instHandle->devConf.dma.fbMode)
                {
                    instHandle->instHwInfo.baseAddress->LCDDMA_FB1_BASE =
                        (Uint32)(fbPtr->frameBufferPtr);
                    instHandle->instHwInfo.baseAddress->LCDDMA_FB1_CEILING =
                        (Uint32)((Uint32)(fbPtr->frameBufferPtr)
                        +(fbPtr->pitch*fbPtr->lines)
                        +(hChan->dmaThrsld)-4u);
                }

                if (Raster_Controller_RASTER == hChan->type)
                {
                    hChan->enabled = TRUE;
                    lcdcRegs->RASTER_CTRL = \
                       ((lcdcRegs->RASTER_CTRL &
                         ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
                        ((CSL_LCDC_RASTER_CTRL_RASTER_EN_ENABLE <<
                          CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT) &
                          CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));

                }
            }
        }
        else
        {
            QUE_put(&(hChan->pendingQ),(Ptr)ioPacket);
            hChan->pendingPktCount++;
            hChan->submitCount++;
            ECM_enableEvent(instHandle->instHwInfo.cpuEventNumber);
        }
    }
    return status;
}


static Int32 rasterResetController(Raster_Object *instHandle)
{
    Int32 retVal = IOM_COMPLETED;

    rasterDmaReset(instHandle);
    rasterDeviceReset(instHandle);

#ifdef BIOS_PWRM_ENABLE
    /* Calculate the constraints here. Its done here because only here are we *
     * actually setting up the clock details. If there are any problems we    *
     * should be notifying here and the channel creation should fail          */
    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
        retVal = RasterCalculateConstraints(instHandle);
    }
#endif

    if (IOM_COMPLETED == retVal)
    {
        retVal = rasterDeviceSetClk(instHandle, instHandle->devConf.clkFreqHz);
    }

    if (IOM_COMPLETED == retVal)
    {
        rasterDmaSetEofInterrupt(instHandle);
        rasterDmaSetBurstSize(instHandle);
        rasterDmaSetEndian(instHandle);
        rasterDmaSetFbMode(instHandle);
    }

    return retVal;
}


/**
 * Creates a new device instance
 *
 * 1. The name of the semaphore created in this function is the same for
 *    all instances. This doesnt affect in PrOS as this name field is not
 *    used.
 */
static Int rasterDeviceCreateInst(Raster_Object* instHandle,
                                  const Raster_Params *params)
{
    Int32             retVal      = IOM_COMPLETED;

    assert(NULL != instHandle);
    assert(NULL != params);
    assert(CSL_LCDC_PER_CNT > instHandle->instNum);

    instHandle->type     = Raster_DevType_LCDC;
    instHandle->chCount  = 0;

    /* Store the device configuration                                  */
    memcpy(&instHandle->devConf,&params->devConf, sizeof(Raster_DeviceConf));
    /* Zero intialize the event Statistics for this instance at start  */
    memset(&instHandle->eventStat,0,sizeof(Raster_EventStat));

    instHandle->instHwInfo.baseAddress =
        Raster_deviceInstInfo[instHandle->instNum].baseAddress;

    instHandle->devConf.clkFreqHz = params->devConf.clkFreqHz;
    instHandle->devConf.dma.burstSize = params->devConf.dma.burstSize;
    instHandle->devConf.dma.eofInt = params->devConf.dma.eofInt;
    instHandle->devConf.dma.bigEndian = params->devConf.dma.bigEndian;
    instHandle->devConf.dma.fbMode = params->devConf.dma.fbMode;
    instHandle->devConf.hwiNum  = params->devConf.hwiNum;
    instHandle->devConf.pscPwrmEnable = params->devConf.pscPwrmEnable;
    instHandle->devConf.pllDomain = params->devConf.pllDomain;
#ifdef BIOS_PWRM_ENABLE    
    instHandle->pwrmInfo.pllDomain = params->devConf.pllDomain;
#endif
    /* Change the state to CREATED */
    instHandle->state = Raster_DriverState_CREATED;
    /* For state machine sake, move the state to CLOSED, as no
       channels are yet open */
    instHandle->state = Raster_DriverState_CLOSED;

    return (retVal);
}


/**
 * Deletes the specific device instances identified by the device Id
 */
static Void rasterDeviceDeleteInst(Raster_Object *instHandle)
{
    assert(NULL != instHandle);

    rasterDmaReset(instHandle);
    rasterDeviceReset(instHandle);

    memset(&instHandle->devConf,0,sizeof(Raster_DeviceConf));
    memset(&instHandle->devConf,0,sizeof(Raster_EventStat));
    instHandle->state = Raster_DriverState_DELETED;
}

/**
 * This function does not update the ds.
 * Update the ds before calling this function, if needed
 */
static Void rasterResetRaster(Raster_Object *instHandle)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    volatile Uint32 tempVal;

    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    lcdcRegs->RASTER_CTRL = \
    ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
     ((CSL_LCDC_RASTER_CTRL_RASTER_EN_DISABLE <<
       CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT) &
      CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));

    /* Reset the status bits in the LCD Status register. This is done by writing
       back the status bits which are set                                     */

    tempVal = instHandle->instHwInfo.baseAddress->LCD_STAT;
    instHandle->instHwInfo.baseAddress->LCD_STAT = tempVal;

    instHandle->instHwInfo.baseAddress->RASTER_CTRL =
                        CSL_LCDC_RASTER_CTRL_RESETVAL;
    instHandle->instHwInfo.baseAddress->RASTER_TIMING_0 =
                CSL_LCDC_RASTER_TIMING_0_RESETVAL;
    instHandle->instHwInfo.baseAddress->RASTER_TIMING_1 =
                CSL_LCDC_RASTER_TIMING_1_RESETVAL;
    instHandle->instHwInfo.baseAddress->RASTER_TIMING_2 =
                CSL_LCDC_RASTER_TIMING_2_RESETVAL;
    instHandle->instHwInfo.baseAddress->RASTER_SUBPANEL =
                    CSL_LCDC_RASTER_SUBPANEL_RESETVAL;

    return;
}


/**
 * Selects the output formatting of the image data for both STN and TFT
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetOF(Raster_ChanObject *hChan,  Raster_Output rasterOF)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    Raster_Object      *instHandle;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterOF)
    {
        case Raster_Output_STN_RIGHTALIGNED:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_LCD_CTRL_MODESEL_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_STN_565_DISABLE <<
                   CSL_LCDC_RASTER_CTRL_STN_565_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_STN_565_MASK));
            break;

        case Raster_Output_STN_INTRAPOLATED:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_STN_565_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_STN_565_ENABLE <<
                   CSL_LCDC_RASTER_CTRL_STN_565_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_STN_565_MASK));
            break;

        case Raster_Output_TFT_RIGHTALIGNED:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_444_DATA <<
                   CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_MASK));
            break;

        case Raster_Output_TFT_INTRAPOLATED:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_565_DATA <<
                   CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_TFT_ALT_MAP_MASK));
            break;

        default:
            SYS_error("Unknown Raster_Output arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.outputFormat = rasterOF;
}


/**
 * Sets the interface for mono display
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetIntrF(Raster_ChanObject *hChan,  Raster_Intface rasterIF)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    Raster_Object      *instHandle;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs =  instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterIF)
    {
        case Raster_Intface_SERIAL_4PIN:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_MONO8B_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_MONO8B_4DATA_PIN <<
                   CSL_LCDC_RASTER_CTRL_MONO8B_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_MONO8B_MASK));
            break;

        case Raster_Intface_PARALLEL_8PIN:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_MONO8B_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_MONO8B_8DATA_PIN <<
                   CSL_LCDC_RASTER_CTRL_MONO8B_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_MONO8B_MASK));
            break;

        case Raster_Intface_PARALLEL_16PIN:
             /* This is ignored for all other modes */
            break;

        default:
            SYS_error("Unknown Raster_Intface arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.intface = rasterIF;

    return;
}


/**
 * Sets the panel type
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetPanel(Raster_ChanObject *hChan, Raster_Panel rasterP)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    Raster_Object      *instHandle;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterP)
    {
        case Raster_Panel_STN:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_TFT_STN_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_TFT_STN_STN <<
                   CSL_LCDC_RASTER_CTRL_TFT_STN_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_TFT_STN_MASK));
            break;

        case Raster_Panel_TFT:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_TFT_STN_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_TFT_STN_TFT <<
                   CSL_LCDC_RASTER_CTRL_TFT_STN_SHIFT)&
                  CSL_LCDC_RASTER_CTRL_TFT_STN_MASK));
            break;

        default:
            SYS_error("Unknown Raster_Panel arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.panel = rasterP;

    return;
}


/**
 * Sets the display mode
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetDisplay(Raster_ChanObject *hChan,  Raster_Display  rasterD)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterD)
    {
        case Raster_Display_MONOC:
             lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL &
                  ~CSL_LCDC_RASTER_CTRL_MONO_COLOR_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_MONO_COLOR_MONOCHROME <<
                   CSL_LCDC_RASTER_CTRL_MONO_COLOR_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_MONO_COLOR_MASK));
            break;

        case Raster_Display_COLOR:
             lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL &
                  ~CSL_LCDC_RASTER_CTRL_MONO_COLOR_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_MONO_COLOR_COLOR <<
                   CSL_LCDC_RASTER_CTRL_MONO_COLOR_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_MONO_COLOR_MASK));
            break;

        default:
            SYS_error("Unknown Raster_Display arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.display  = rasterD;

    return;
}


/**
 * Sets the frame buffer content
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetFBContent(Raster_ChanObject *hChan,
                               Raster_FBContent rasterFBC)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterFBC)
    {
        /* Palette Loading Mode, bit[21:20], RASTER_CTRL */
        case Raster_FBContent_PALETTE_DATA:
            /* value = 00b */
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_PLM_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_PLM_PALETTE_DATA <<
                   CSL_LCDC_RASTER_CTRL_PLM_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_PLM_MASK));
            break;
        case Raster_FBContent_PALETTE:
            /* value = 01b */
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_PLM_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_PLM_PALETTE <<
                   CSL_LCDC_RASTER_CTRL_PLM_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_PLM_MASK));
            break;
        case Raster_FBContent_DATA:
            /* value = 10b */
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_PLM_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_PLM_DATA <<
                   CSL_LCDC_RASTER_CTRL_PLM_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_PLM_MASK));
            break;
        default:
            SYS_error("Unknown Raster_FBContent arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.fbContent    = rasterFBC;

    return;
}


/**
 * Sets the data order; the controller is a little endian device
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetDisplayO(Raster_ChanObject *hChan, Raster_DataOrder rasterDO)
{
    CSL_LcdcRegsOvly    lcdcRegs;
    Raster_Object   *instHandle;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterDO)
    {
        case Raster_DataOrder_LSBMSB:
            /* value=0b */
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RD_ORDER_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_RD_ORDER_LSBTOMSB <<
                   CSL_LCDC_RASTER_CTRL_RD_ORDER_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_RD_ORDER_MASK));
            break;

        case Raster_DataOrder_MSBLSB:
            /* value=1b */
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RD_ORDER_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_RD_ORDER_MSBTOLSB <<
                   CSL_LCDC_RASTER_CTRL_RD_ORDER_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_RD_ORDER_MASK));
            break;

        default:
            SYS_error("Unknown Raster_DataOrder arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.dataOrder = rasterDO;

    return;
}


/**
 * Enables or disables the nibble mode
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetNibbleMode(Raster_ChanObject *hChan, Bool rasterN)
{
    Raster_Object   *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    switch(rasterN)
    {
        case TRUE:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_NIB_MODE_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_NIB_MODE_ENABLE <<
                   CSL_LCDC_RASTER_CTRL_NIB_MODE_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_NIB_MODE_MASK));
        break;

        case FALSE:
            lcdcRegs->RASTER_CTRL = \
                ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_NIB_MODE_MASK) |
                 ((CSL_LCDC_RASTER_CTRL_NIB_MODE_DISABLE <<
                   CSL_LCDC_RASTER_CTRL_NIB_MODE_SHIFT) &
                  CSL_LCDC_RASTER_CTRL_NIB_MODE_MASK));
        break;

        default:
            SYS_error("Invalid value for boolean arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    hChan->chanConf.nibbleMode   = rasterN;

    return;
}


/**
 * Sets the subpanel parameters as supplied by the user.
 * Note that the input is not validated in this case.
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetSubpanel(Raster_ChanObject *hChan,
                              Raster_SubpanelCmd cmd,
                              Void *conf)
{

    Uint32               newValue    = 0;
    Raster_SubpanelPos   spPos       = Raster_SubpanelPos_UNDEFINED;
    Raster_Object       *instHandle;
    CSL_LcdcRegsOvly     lcdcRegs;

    assert(NULL != hChan);
    assert(NULL != conf);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);


    switch(cmd)
    {
        case Raster_SubpanelCmd_SET_SPEN:
            if(FALSE == (*(Bool *)conf))
            {
                hChan->chanConf.subPanel.enable = *(Bool *)conf;
                lcdcRegs->RASTER_SUBPANEL = \
                    ((lcdcRegs->RASTER_SUBPANEL &
                      ~CSL_LCDC_LCD_CTRL_MODESEL_MASK) |
                     ((CSL_LCDC_RASTER_SUBPANEL_SPEN_DISABLE <<
                       CSL_LCDC_RASTER_SUBPANEL_SPEN_SHIFT)&
                      CSL_LCDC_RASTER_SUBPANEL_SPEN_MASK));
            }
            else if(TRUE == (*(Bool *)conf))
            {
                hChan->chanConf.subPanel.enable = *(Bool *)conf;
                lcdcRegs->RASTER_SUBPANEL = \
                    ((lcdcRegs->RASTER_SUBPANEL &
                      ~CSL_LCDC_LCD_CTRL_MODESEL_MASK) |
                     ((CSL_LCDC_RASTER_SUBPANEL_SPEN_ENABLE <<
                       CSL_LCDC_RASTER_SUBPANEL_SPEN_SHIFT)&
                      CSL_LCDC_RASTER_SUBPANEL_SPEN_MASK));
            }
            else
            {
                SYS_error("Invalid value for boolean arg.\n", SYS_EINVAL);
                assert(FALSE);
            }
            break;

        case Raster_SubpanelCmd_SET_POSITION:
            if(Raster_SubpanelPos_HIGH == (*(Raster_SubpanelPos*)conf))
            {
                spPos = *(Raster_SubpanelPos*)conf;
                hChan->chanConf.subPanel.position = spPos;
                lcdcRegs->RASTER_SUBPANEL = \
                    ((lcdcRegs->RASTER_SUBPANEL &
                      ~CSL_LCDC_RASTER_SUBPANEL_HOLS_MASK) |
                     ((CSL_LCDC_RASTER_SUBPANEL_HOLS_HIGH <<
                       CSL_LCDC_RASTER_SUBPANEL_HOLS_SHIFT)&
                      CSL_LCDC_RASTER_SUBPANEL_HOLS_MASK));
            }
            else if(Raster_SubpanelPos_LOW == (*(Raster_SubpanelPos*)conf))
            {
               spPos = *(Raster_SubpanelPos*)conf;
               hChan->chanConf.subPanel.position = spPos;
               lcdcRegs->RASTER_SUBPANEL = \
                    ((lcdcRegs->RASTER_SUBPANEL &
                      ~CSL_LCDC_RASTER_SUBPANEL_HOLS_MASK) |
                     ((CSL_LCDC_RASTER_SUBPANEL_HOLS_LOW <<
                       CSL_LCDC_RASTER_SUBPANEL_HOLS_SHIFT)&
                      CSL_LCDC_RASTER_SUBPANEL_HOLS_MASK));
            }
            else
            {
                SYS_error("Unknown Raster_SubpanelPos arg.\n", SYS_EINVAL);
                assert(FALSE);
            }
            break;

        case Raster_SubpanelCmd_SET_LPPT:
            assert(RASTER_SUBPANEL_LPPT_MAX >= (*(Uint32 *)conf));

            newValue = *(Uint32 *)conf;
            lcdcRegs->RASTER_SUBPANEL = \
                ((lcdcRegs->RASTER_SUBPANEL &
                  ~CSL_LCDC_RASTER_SUBPANEL_LPPT_MASK) |
                 ((newValue << CSL_LCDC_RASTER_SUBPANEL_LPPT_SHIFT) &
                   CSL_LCDC_RASTER_SUBPANEL_LPPT_MASK));
            hChan->chanConf.subPanel.lPPT = newValue;
            break;

        case Raster_SubpanelCmd_SET_DATA:
            assert(RASTER_SUBPANEL_DPD_MAX >= (*(Uint32 *)conf));

            newValue = *(Uint32 *)conf;
            lcdcRegs->RASTER_SUBPANEL = \
                ((lcdcRegs->RASTER_SUBPANEL &
                  ~CSL_LCDC_RASTER_SUBPANEL_DPD_MASK) |
                 ((newValue << CSL_LCDC_RASTER_SUBPANEL_DPD_SHIFT) &
                   CSL_LCDC_RASTER_SUBPANEL_DPD_MASK));
            hChan->chanConf.subPanel.defaultData = newValue;
            break;

        default:
            SYS_error("Unknown Raster_SubpanelCmd arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    return;
}


/**
 * Masks the interrupts generated by the controller
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetIntr(Raster_ChanObject *hChan,Uint32 rasterI)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if((Raster_Intr_OFIFO_UNDERRUN_ENABLE & rasterI)
        == Raster_Intr_OFIFO_UNDERRUN_ENABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_FUF_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_FUF_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_FUF_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_FUF_EN_MASK));
    }

    if((Raster_Intr_FRAMESYNC_LOST_ENABLE & rasterI)
        == Raster_Intr_FRAMESYNC_LOST_ENABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_SL_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_SL_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_SL_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_SL_EN_MASK));
    }

    if((Raster_Intr_PALETTE_LOADED_ENABLE & rasterI)
        == Raster_Intr_PALETTE_LOADED_ENABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_PL_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_PL_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_PL_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_PL_EN_MASK));
    }

    if((Raster_Intr_FRAME_DONE_ENABLE & rasterI)
        == Raster_Intr_FRAME_DONE_ENABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_DONE_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_DONE_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_DONE_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_DONE_EN_MASK));
    }

    if((Raster_Intr_ACBIAS_TRANSITION_ENABLE & rasterI)
        == Raster_Intr_ACBIAS_TRANSITION_ENABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_AC_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_AC_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_AC_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_AC_EN_MASK));
    }

    hChan->chanConf.intMask  |= rasterI;

    return;
}


/**
 * Masks the interrupts generated by the controller
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterMaskInterrupt(Raster_ChanObject *hChan, Uint32 rasterI)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if((Raster_Intr_OFIFO_UNDERRUN_DISABLE | rasterI)
        == Raster_Intr_OFIFO_UNDERRUN_DISABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_FUF_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_FUF_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_FUF_EN_DISABLE)&
              CSL_LCDC_RASTER_CTRL_FUF_EN_MASK));
    }

    if((Raster_Intr_FRAMESYNC_LOST_DISABLE | rasterI)
        == Raster_Intr_FRAMESYNC_LOST_DISABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_SL_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_SL_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_SL_EN_DISABLE)&
              CSL_LCDC_RASTER_CTRL_SL_EN_MASK));
    }

    if((Raster_Intr_PALETTE_LOADED_DISABLE | rasterI)
        == Raster_Intr_PALETTE_LOADED_DISABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_PL_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_PL_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_PL_EN_DISABLE)&
              CSL_LCDC_RASTER_CTRL_PL_EN_MASK));
    }

    if((Raster_Intr_FRAME_DONE_DISABLE | rasterI)
        == Raster_Intr_FRAME_DONE_DISABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_DONE_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_DONE_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_DONE_EN_DISABLE)&
              CSL_LCDC_RASTER_CTRL_DONE_EN_MASK));
    }

    if((Raster_Intr_ACBIAS_TRANSITION_DISABLE | rasterI)
        == Raster_Intr_ACBIAS_TRANSITION_DISABLE)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_AC_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_AC_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_AC_EN_DISABLE)&
              CSL_LCDC_RASTER_CTRL_AC_EN_MASK));
    }

    hChan->chanConf.intMask  &= rasterI;


   return;
}


/**
 * Sets the horizontal parameters for timing generation
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetHParam(Raster_ChanObject *hChan,
                            Uint32      hBP,
                            Uint32      hFP,
                            Uint32      hSPW,
                            Uint32      pPL)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);


    lcdcRegs->RASTER_TIMING_0 = \
        ((lcdcRegs->RASTER_TIMING_0 & ~CSL_LCDC_RASTER_TIMING_0_HBP_MASK) |
         ((hBP << CSL_LCDC_RASTER_TIMING_0_HBP_SHIFT) &
           CSL_LCDC_RASTER_TIMING_0_HBP_MASK));

    lcdcRegs->RASTER_TIMING_0 = \
        ((lcdcRegs->RASTER_TIMING_0 & ~CSL_LCDC_RASTER_TIMING_0_HFP_MASK) |
         ((hFP << CSL_LCDC_RASTER_TIMING_0_HFP_SHIFT) &
           CSL_LCDC_RASTER_TIMING_0_HFP_MASK));

    lcdcRegs->RASTER_TIMING_0 = \
        ((lcdcRegs->RASTER_TIMING_0 & ~CSL_LCDC_RASTER_TIMING_0_HSW_MASK) |
         ((hSPW << CSL_LCDC_RASTER_TIMING_0_HSW_SHIFT) &
           CSL_LCDC_RASTER_TIMING_0_HSW_MASK));

    lcdcRegs->RASTER_TIMING_0 = \
        ((lcdcRegs->RASTER_TIMING_0 & ~CSL_LCDC_RASTER_TIMING_0_PPL_MASK) |
         ((pPL << CSL_LCDC_RASTER_TIMING_0_PPL_SHIFT) &
           CSL_LCDC_RASTER_TIMING_0_PPL_MASK));

    hChan->chanConf.hFP = hFP;
    hChan->chanConf.hBP  = hBP;
    hChan->chanConf.hSPW = hSPW;
    hChan->chanConf.pPL  = pPL;

    return;
}


/**
 * Sets the vertical parameters for timing generation
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetVParam(Raster_ChanObject *hChan,
                           Uint32      vBP,
                           Uint32      vFP,
                           Uint32      vSPW,
                           Uint32      lPP)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    lcdcRegs->RASTER_TIMING_1 = \
        ((lcdcRegs->RASTER_TIMING_1 & ~CSL_LCDC_RASTER_TIMING_1_VBP_MASK) |
         ((vBP << CSL_LCDC_RASTER_TIMING_1_VBP_SHIFT) &
           CSL_LCDC_RASTER_TIMING_1_VBP_MASK));

    lcdcRegs->RASTER_TIMING_1 = \
        ((lcdcRegs->RASTER_TIMING_1 & ~CSL_LCDC_RASTER_TIMING_1_VFP_MASK) |
         ((vFP << CSL_LCDC_RASTER_TIMING_1_VFP_SHIFT) &
           CSL_LCDC_RASTER_TIMING_1_VFP_MASK));

    lcdcRegs->RASTER_TIMING_1 = \
        ((lcdcRegs->RASTER_TIMING_1 & ~CSL_LCDC_RASTER_TIMING_1_VSW_MASK) |
         ((vSPW << CSL_LCDC_RASTER_TIMING_1_VSW_SHIFT) &
           CSL_LCDC_RASTER_TIMING_1_VSW_MASK));

    lcdcRegs->RASTER_TIMING_1 = \
        ((lcdcRegs->RASTER_TIMING_1 & ~CSL_LCDC_RASTER_TIMING_1_LPP_MASK) |
         ((lPP << CSL_LCDC_RASTER_TIMING_1_LPP_SHIFT) &
           CSL_LCDC_RASTER_TIMING_1_LPP_MASK));

    hChan->chanConf.vFP  = vFP;
    hChan->chanConf.vBP  = vBP;
    hChan->chanConf.vSPW = vSPW;
    hChan->chanConf.lPP  = lPP;

    return;
}


/**
 * Clears the internal structure that contains the current raster configuration
 * This API only clears the software structure, it doesn't read/write the
 * registers; do so before calling this API (if needed)
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterClearRasterConf(Raster_ChanObject *hChan)
{
    assert(NULL != hChan);

    memset(&hChan->chanConf, 0, sizeof(Raster_RasterConf));

    return;
}


/**
 * This function calculates the size of the frame buffer and dmaThreshold
 * depending on the BPP.
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetBitsPP(Raster_ChanObject *hChan,
                            Raster_FbBitsPerPixel bitsPP)
{
    Uint32              noOfBits        = 0;
    Uint32              paletteSize     = 0;

    assert(NULL != hChan);

    switch(bitsPP)
    {
        /* 1. One bit per pixel */
        case Raster_FbBitsPerPixel_1BPP:
            noOfBits        = 1u;
            paletteSize     = 32u;
            break;
        /* 2. Two bits per pixel */
        case Raster_FbBitsPerPixel_2BPP:
            noOfBits        = 2u;
            paletteSize     = 32u;
            break;
        /* 3. Four bits per pixel */
        case Raster_FbBitsPerPixel_4BPP:
            noOfBits        = 4u;
            paletteSize     = 32u;
            break;
        /* 4. Eight bits per pixel */
        case Raster_FbBitsPerPixel_8BPP:
            noOfBits        = 8u;
            paletteSize     = 512u;
            break;
        /* 5. Twelve bits per pixel */
        /* 6. Sixteen bits per pixel */
        case Raster_FbBitsPerPixel_16BPP:
            noOfBits        = 16u;
            paletteSize     = 32u;
            break;
        default:
            SYS_error("Unknown Raster_FbBitsPerPixel arg.\n", SYS_EINVAL);
            assert(FALSE);
            break;
    }

    /*Calculating the size of the buffer in bytes */
    hChan->dmaThrsld = paletteSize;
    hChan->fbSize = (((16u*(hChan->chanConf.pPL+1u))
                      *(hChan->chanConf.lPP + 1u)
                      *noOfBits)/8u)
                    + paletteSize;

    hChan->chanConf.bitsPP = bitsPP;

    return;
}


/**
 * Sets raster timing 2 configurations
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetTiming2(Raster_ChanObject *hChan,
                             Raster_Timing2 *rasterT2)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);
    assert(NULL != rasterT2);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(TRUE == rasterT2->syncCtrl)
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_ACTIVE <<
                CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_MASK));

        if(Raster_SyncEdge_RISING == rasterT2->syncEdge)
        {
            lcdcRegs->RASTER_TIMING_2 = \
                ((lcdcRegs->RASTER_TIMING_2 &
                  ~CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_MASK) |
                  ((CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_RISING <<
                    CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_SHIFT)&
                    CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_MASK));
        }
        else
        {
            if(Raster_SyncEdge_FALLING == rasterT2->syncEdge)
            {
                lcdcRegs->RASTER_TIMING_2 = \
                    ((lcdcRegs->RASTER_TIMING_2 &
                      ~CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_MASK) |
                      ((CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_FALLING <<
                        CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_SHIFT)&
                        CSL_LCDC_RASTER_TIMING_2_SYNC_EDGE_MASK));
            }
        }
    }
    else
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_INACTIVE <<
                CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_SYNC_CTRL_MASK));
    }

    if(TRUE == rasterT2->iAcbias)
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_BIAS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_BIAS_ACTIVE_LOW <<
                CSL_LCDC_RASTER_TIMING_2_BIAS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_BIAS_MASK));
    }
    else
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_BIAS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_BIAS_ACTIVE_HIGH <<
                CSL_LCDC_RASTER_TIMING_2_BIAS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_BIAS_MASK));
    }

    if(TRUE == rasterT2->iPclk)
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IPC_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IPC_FALLING <<
                CSL_LCDC_RASTER_TIMING_2_IPC_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IPC_MASK));
    }
    else
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IPC_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IPC_RISING <<
                CSL_LCDC_RASTER_TIMING_2_IPC_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IPC_MASK));
    }

    if(TRUE ==  rasterT2->iLclk)
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IHS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IHS_ACTIVE_LOW <<
                CSL_LCDC_RASTER_TIMING_2_IHS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IHS_MASK));
    }
    else
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IHS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IHS_ACTIVE_HIGH <<
                CSL_LCDC_RASTER_TIMING_2_IHS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IHS_MASK));
    }

    if(TRUE == rasterT2->iFclk)
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IVS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IVS_ACTIVE_LOW <<
                CSL_LCDC_RASTER_TIMING_2_IVS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IVS_MASK));
    }
    else
    {
        lcdcRegs->RASTER_TIMING_2 = \
            ((lcdcRegs->RASTER_TIMING_2 &
              ~CSL_LCDC_RASTER_TIMING_2_IVS_MASK) |
              ((CSL_LCDC_RASTER_TIMING_2_IVS_ACTIVE_HIGH <<
                CSL_LCDC_RASTER_TIMING_2_IVS_SHIFT)&
                CSL_LCDC_RASTER_TIMING_2_IVS_MASK));
    }

    lcdcRegs->RASTER_TIMING_2 = \
        ((lcdcRegs->RASTER_TIMING_2 & ~CSL_LCDC_RASTER_TIMING_2_ACB_I_MASK) |
         ((rasterT2->acBiasInt << CSL_LCDC_RASTER_TIMING_2_ACB_I_SHIFT) &
           CSL_LCDC_RASTER_TIMING_2_ACB_I_MASK));

    lcdcRegs->RASTER_TIMING_2 = \
        ((lcdcRegs->RASTER_TIMING_2 & ~CSL_LCDC_RASTER_TIMING_2_ACB_MASK) |
         ((rasterT2->acBiasFreq << CSL_LCDC_RASTER_TIMING_2_ACB_SHIFT) &
           CSL_LCDC_RASTER_TIMING_2_ACB_MASK));

    hChan->chanConf.timing2.syncCtrl     = rasterT2->syncCtrl;
    hChan->chanConf.timing2.syncEdge     = rasterT2->syncEdge;
    hChan->chanConf.timing2.iAcbias      = rasterT2->iAcbias;
    hChan->chanConf.timing2.iPclk        = rasterT2->iPclk;
    hChan->chanConf.timing2.iLclk        = rasterT2->iLclk;
    hChan->chanConf.timing2.iFclk        = rasterT2->iFclk;
    hChan->chanConf.timing2.acBiasInt    = rasterT2->acBiasInt;
    hChan->chanConf.timing2.acBiasFreq   = rasterT2->acBiasFreq;

   return;
}

/**
 * Reads the current subpanel configurations to the user
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterGetSubpanel(Raster_ChanObject *hChan,
                              Raster_Subpanel *subpanel)
{
    assert(NULL != hChan);
    assert(NULL != subpanel);

    memcpy(subpanel,&hChan->chanConf.subPanel, sizeof(Raster_Subpanel));

    return;
}


/**
 * Sets the fifo dma delay
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterSetFDD(Raster_ChanObject *hChan, Uint32 rasterFDD)
{
    Raster_Object   *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != hChan);
    assert(RASTER_DMA_FIFO_DELAY_MAX >= rasterFDD);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    lcdcRegs->RASTER_CTRL = \
        ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_FIFO_DMA_DELAY_MASK) |
         ((rasterFDD << CSL_LCDC_RASTER_CTRL_FIFO_DMA_DELAY_SHIFT) &
           CSL_LCDC_RASTER_CTRL_FIFO_DMA_DELAY_MASK));

    hChan->chanConf.fifoDmaDelay = rasterFDD;

    return;
}


/**
 * Configures raster controller of LCDC and stores the info in channel object
 */
static Void rasterChannelConfigRaster(Raster_ChanObject *hChan,
                                      Raster_RasterConf *rConf)
{
    assert(NULL != hChan);
    assert(NULL != rConf);

    rasterSetOF(hChan, rConf->outputFormat);
    rasterSetIntrF(hChan, rConf->intface);
    rasterSetPanel(hChan, rConf->panel);
    rasterSetDisplay(hChan, rConf->display);
    rasterSetFBContent(hChan, rConf->fbContent);
    rasterSetDisplayO(hChan, rConf->dataOrder);
    rasterSetNibbleMode(hChan, rConf->nibbleMode);

    rasterSetSubpanel(hChan,
        Raster_SubpanelCmd_SET_POSITION,
        (void *)&rConf->subPanel.position);
    rasterSetSubpanel(hChan,
        Raster_SubpanelCmd_SET_LPPT,
        (void *)&rConf->subPanel.lPPT);
    rasterSetSubpanel(hChan,
        Raster_SubpanelCmd_SET_DATA,
        (void *)&rConf->subPanel.defaultData);
    rasterSetSubpanel(hChan,
        Raster_SubpanelCmd_SET_SPEN,
        (void*)&rConf->subPanel.enable);

    rasterSetTiming2(hChan, &rConf->timing2);
    rasterSetFDD(hChan, rConf->fifoDmaDelay);
    rasterSetIntr(hChan, rConf->intMask);

    rasterSetHParam(hChan,
                    rConf->hBP,
                    rConf->hFP,
                    rConf->hSPW,
                    rConf->pPL);

    rasterSetVParam(hChan,
                    rConf->vBP,
                    rConf->vFP,
                    rConf->vSPW,
                    rConf->lPP);
    rasterSetBitsPP(hChan, rConf->bitsPP);

    rasterDeviceSetController(hChan, Raster_Controller_RASTER);

    return;
}


/**
 * Creates a channel on this device
 * This action doesn't start the channel
 * Only IOCTL or submit-
  can start a channel
 * This API assumes that rasterConf is validated and will only check for NULL
 */
static void rasterDeviceOpenChannel(Raster_Object    *instHandle,
                                   Raster_Controller controller,
                                   Ptr              *chanConf)
{
    Raster_ChanObject *hChan      = NULL;

    assert(NULL != chanConf);
    assert(NULL != instHandle);
    assert(NULL != chanConf);

    assert(Raster_Controller_RASTER == controller);
	/* To remove compiler warnings */
    controller = controller;

    hChan = &instHandle->ChannelObj;
    assert(NULL != hChan);

    rasterResetRaster(instHandle);
    rasterChannelConfigRaster(hChan, (Raster_RasterConf*)chanConf);

    hChan->state = Raster_DriverState_OPENED;
    /*
    * Change the device state to OPENED if it was CLOSED.
    * Do nothing if the device state was OPENED
    */
    if((Raster_DriverState_CREATED == instHandle->state)
       || (Raster_DriverState_CLOSED == instHandle->state))
    {
       instHandle->state = Raster_DriverState_OPENED;
    }

   return; /* Should be made NULL if some error occurs*/
}

/**
 * Enables the raster controller. The raster controller will start sending the
 * data using its internal-DMA.
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDeviceEnable(Raster_ChanObject *hChan)
{
    Raster_Object      *instHandle;
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL!=hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(FALSE == hChan->enabled)
    {
        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_RASTER_EN_ENABLE <<
               CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));
        hChan->enabled  = TRUE;
    }

   return;
}


/**
 * Disables the raster controller.
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterDeviceDisable(Raster_ChanObject *hChan)
{
    Raster_Object       *instHandle;
    CSL_LcdcRegsOvly     lcdcRegs;
    IOM_Packet          *ioPacket;
    Int32                status = IOM_COMPLETED;

    assert(NULL!=hChan);

    instHandle = (Raster_Object*)hChan->instHandle;
    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    if(TRUE == hChan->enabled)
    {
        /* Disable the interrupt from pre-emting this operation               */
        ECM_disableEvent(instHandle->instHwInfo.cpuEventNumber);

        lcdcRegs->RASTER_CTRL = \
            ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
             ((CSL_LCDC_RASTER_CTRL_RASTER_EN_DISABLE <<
               CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT)&
               CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));

        hChan->enabled = FALSE;



        /* De-queue all the packets in both lists and return them to stream   *
         * However, note that the sequence of activeQ first and then pendindQ *
         * is important. This is pre-requisite of Stream                      */
        while (FALSE == QUE_empty(&(hChan->activeQ)))
        {
            ioPacket = (IOM_Packet*)QUE_get(&(hChan->activeQ));

            /* complete the packet processing by calling the callback function*/
            if (NULL != ioPacket)
            {
                /* The packet is aborted due to disable device command        */
                ioPacket->status = IOM_ABORTED;

                /* Update the packet with the status of LPSC disable operation*
                 * if it is not all well here, so that application is aware   *
                 * of this error                                              */
                status = rasterLpscOff(instHandle);

                if(IOM_COMPLETED != status)
                {
                    ioPacket->status = status;
                }

                (*hChan->appCb)(hChan->cbArg,ioPacket);
                hChan->submitCount--;
            }
        }

        while(FALSE == QUE_empty(&(hChan->pendingQ)))
        {
            ioPacket = (IOM_Packet*)QUE_get(&(hChan->pendingQ));

            /* complete the packet processing by calling the callback function*/
            if (NULL != ioPacket)
            {
                /* The packet is aborted due to disable device command        */
                ioPacket->status = IOM_ABORTED;

                /* Update the packet with the status of LPSC disable operation*
                 * if it is not all well here, so that application is aware   *
                 * of this error                                              */
                status = rasterLpscOff(instHandle);

                if(IOM_COMPLETED != status)
                {
                    ioPacket->status = status;
                }

                (*hChan->appCb)(hChan->cbArg,ioPacket);
                hChan->submitCount--;
            }
        }

        /* Clear the active and pending packet counts */
        hChan->pendingPktCount = 0;
        hChan->activePktCount = 0;
    }

#ifdef BIOS_PWRM_ENABLE
    if (TRUE == instHandle->devConf.pscPwrmEnable)
    {
        status = RasterUnregisterConstraints(instHandle);
    }
#endif

    return ;
}


/**
 * Closes the channel identified by hChan
 */
static Int32 rasterDeviceCloseChannel(Raster_ChanObject *hChan)
{
    Int32          status = IOM_COMPLETED;
    Raster_Object *instHandle = NULL;

    assert(NULL != hChan);
    assert(NULL != hChan->instHandle);
    instHandle = (Raster_Object *)hChan->instHandle;
    assert(Raster_Controller_RASTER == hChan->type);

    instHandle->chCount --;

    rasterDeviceDisable(hChan);

    rasterResetRaster(instHandle);
    rasterClearRasterConf(hChan);

    /* Clear the active and pending packet counts */
    hChan->pendingPktCount = 0;
    hChan->activePktCount = 0;

    hChan->state    = Raster_DriverState_CLOSED;

    if(0 == instHandle->chCount)
    {
       instHandle->state = Raster_DriverState_CLOSED;
    }

    hChan->type     = Raster_Controller_UNDEFINED;

    return status;
}


/*
 * This function enables and disables the raster
 */
static inline Void rasterEnableDisable(Raster_Object *instHandle)
{
    CSL_LcdcRegsOvly    lcdcRegs;

    assert(NULL != instHandle);

    lcdcRegs = instHandle->instHwInfo.baseAddress;
    assert(NULL != lcdcRegs);

    lcdcRegs->RASTER_CTRL = \
        ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
         ((CSL_LCDC_RASTER_CTRL_RASTER_EN_DISABLE <<
           CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT)&
           CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));

    lcdcRegs->RASTER_CTRL = \
        ((lcdcRegs->RASTER_CTRL & ~CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK) |
         ((CSL_LCDC_RASTER_CTRL_RASTER_EN_ENABLE <<
           CSL_LCDC_RASTER_CTRL_RASTER_EN_SHIFT)&
           CSL_LCDC_RASTER_CTRL_RASTER_EN_MASK));

    return;
}

/**
 Set the base and celing addresses of the frame buffers
 */
static inline Void rasterSetFrameAddress(volatile CSL_LcdcRegs *lcdcRegs,
                                         Raster_FrameBuffer *fBuffer,
                                         Uint32 dmaThrsld,
                                         Uint32 frame0or1)
{
    if((NULL != lcdcRegs) && (NULL != fBuffer))
    {
        if(0 == frame0or1)
        {
            /* Set the start address */
            lcdcRegs->LCDDMA_FB0_BASE =
                (Uint32)fBuffer->frameBufferPtr;
            /* Set the end address - size of frame */
            lcdcRegs->LCDDMA_FB0_CEILING =
                (Uint32)((Uint32)(fBuffer->frameBufferPtr)
                +(fBuffer->pitch * fBuffer->lines)
                +dmaThrsld)
                -4u;
        }
        else if(1u == frame0or1)
        {
            /* Set the start address */
            lcdcRegs->LCDDMA_FB1_BASE =
                (Uint32)fBuffer->frameBufferPtr;
            /* Set the end address - size of frame */
            lcdcRegs->LCDDMA_FB1_CEILING =
                (Uint32)((Uint32)(fBuffer->frameBufferPtr)
                +(fBuffer->pitch * fBuffer->lines)
                +dmaThrsld)
                -4u;
        }
        else
        {
            /* Do nothing */
        }
    }
}

/**
 * Generic interrupt handler for any instance of the device
 */
Void rasterIntrHandler(Arg isrArg)
{
    Raster_Object             *instHandle;
    volatile  Uint32           savedStatus = 0;
    volatile  CSL_LcdcRegs    *lcdcRegs    = NULL;
    Raster_ChanObject         *hChan       = NULL;
    IOM_Packet                *ioPacket;
    Raster_FrameBuffer        *fBuffer;

    if (NULL != isrArg)
    {
        instHandle = (Raster_Object*)isrArg;

        if(NULL != instHandle)
        {
            lcdcRegs = instHandle->instHwInfo.baseAddress;
            hChan = &instHandle->ChannelObj;
        }
    }

    if ((NULL != lcdcRegs) && (NULL != hChan))
    {

        /* Get the current interrupt status                                   */
        savedStatus     = lcdcRegs->LCD_STAT;
        /* Clear current interrupt status. Else we wont get next interrupts   */
        lcdcRegs->LCD_STAT = savedStatus;

        if((CSL_LCDC_LCD_STAT_SYNC_MASK
                == (CSL_LCDC_LCD_STAT_SYNC_MASK & savedStatus))
           ||(CSL_LCDC_LCD_STAT_FUF_MASK
                ==(CSL_LCDC_LCD_STAT_FUF_MASK & savedStatus)))
        {
            rasterEnableDisable(instHandle);
        }

        /*Frame Done*/
        if((savedStatus
             & CSL_LCDC_LCD_STAT_DONE_MASK)
             == CSL_LCDC_LCD_STAT_DONE_MASK)
        {
    #ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.done++;
    #endif /* Raster_CAPTURE_EVENT_STATISTICS */

        }

        /* Sync lost */
        if((CSL_LCDC_LCD_STAT_SYNC_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_SYNC_MASK)
        {
            if((lcdcRegs->LCD_STAT
                & CSL_LCDC_LCD_STAT_DONE_MASK)
                == CSL_LCDC_LCD_STAT_DONE_MASK)
            {
                lcdcRegs->LCD_STAT = CSL_LCDC_LCD_STAT_DONE_MASK;
                /* Not updating eventStat->done. This is not an interrupt     */
            }

    #ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.sync++;
    #endif /* Raster_CAPTURE_EVENT_STATISTICS */
        }

        /* Fifo underflow status */
        if((CSL_LCDC_LCD_STAT_FUF_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_FUF_MASK)
        {
    #ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.fuf++;
    #endif /* Raster_CAPTURE_EVENT_STATISTICS */
        }

        /* Palette loaded */
        if((CSL_LCDC_LCD_STAT_PL_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_PL_MASK)
        {
    #ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.pl++;
    #endif /* Raster_CAPTURE_EVENT_STATISTICS */
        }

        /* Frame 0 done */
        if((CSL_LCDC_LCD_STAT_EOF0_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_EOF0_MASK)
        {
            /*
             Only one packet at hand. Don't return it. Keep displaying it. This
             occurs when application had submitted one packet and after that,
             for long time (long enough for this interrupt to occur) no further
             packets were submitted.
             */
            if((1u == hChan->activePktCount) && (0u == hChan->pendingPktCount))
            {
                ioPacket = (IOM_Packet*)QUE_head(&(hChan->activeQ));

                if( NULL != ioPacket)
                {
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,0);
                }
            }

            /* We have some new packet pending. Take it and display it.In case*
             * of single frame buffer mode return the formerly active packet. *
             * However,in case of duble frame buffer mode do not return the   *
             * formerly active packet, since it might be used for frame 1.    */
            else if ((1u == hChan->activePktCount) &&
                        (0 != hChan->pendingPktCount))
            {
                if (Raster_DmaFb_SINGLE == instHandle->devConf.dma.fbMode)
                {
                    ioPacket = (IOM_Packet*)QUE_get(&(hChan->activeQ));
                    hChan->activePktCount--;

                    if (NULL != ioPacket)
                    {
                        ioPacket->status = rasterLpscOff(instHandle);

                        (*hChan->appCb)(hChan->cbArg,ioPacket);
                        hChan->submitCount--;
                    }
                }

                ioPacket = (IOM_Packet*)QUE_get(&(hChan->pendingQ));
                hChan->pendingPktCount--;

                if (NULL != ioPacket)
                {
                    QUE_put(&(hChan->activeQ),(Ptr)ioPacket);
                    hChan->activePktCount++;

                    /*Get actual Raster_FrameBuffer member from the ioPacket  */
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,0);
                }
            }
            /* This case can occur when the active packet was not dequeued.   *
             * This can occur, when the active packet was shared between frame*
             * 0 and frame 1, like in the case of first two submits.          */
            else
            {
                /*
                Here we have more than one active packets. Hence free the head.
                Get the active packet at head. This can be freed because,this
                packet is actually from the "other" frame (frame 1) and it has
                completed with it. However, we did not free it there because
                it might have been in use for the frame0, that is this frame.
                */
                ioPacket = (IOM_Packet*)QUE_get(&(hChan->activeQ));
                hChan->activePktCount--;

                if (NULL != ioPacket)
                {
                    ioPacket->status = rasterLpscOff(instHandle);

                    (*hChan->appCb)(hChan->cbArg,ioPacket);
                    hChan->submitCount--;
                }
                /*
                 What hsould be displayed now, depends on if there is any
                 newly submitted packet in the pending queue. If indeed pending
                 queue has any newly submitted packets, then take it from
                 pending queue. Else take it from the active queue. However,
                 freeing of the next pachet in active queue, will be done in the
                 next frame done interrupt.
                 */
                if(0 != hChan->pendingPktCount)
                {
                    ioPacket = (IOM_Packet*)QUE_get(&(hChan->pendingQ));
                    hChan->pendingPktCount--;

                    if(NULL != ioPacket)
                    {
                        QUE_put(&(hChan->activeQ),(Ptr)ioPacket);
                        hChan->activePktCount++;

                        /*Get actual Raster_FrameBuffer member from the ioPacket  */
                        fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                        rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,0);
                    }
                }
                else
                {
                    /*
                    Now get the packet at the head of the active queue. This needs
                    to be displayed. However, we can neither return this packet, nor
                    remove it from the queue, since this is the only packet at hand.
                    */
                    ioPacket = (IOM_Packet*)QUE_head(&(hChan->activeQ));

                    /* Get actual Raster_FrameBuffer member from the ioPacket  */
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,0);
                }
            }
    #ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.eof0++;
    #endif /* Raster_CAPTURE_EVENT_STATISTICS */
        }

        /* Frame 1 done */
        if((CSL_LCDC_LCD_STAT_EOF1_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_EOF1_MASK)
        {
            /*
             Only one packet at hand. Don't return it. Keep displaying it. This
             occurs when application had submitted one packet and after that,
             for long time (long enough for this interrupt to occur) no further
             packets were submitted.
             */
            if((1u == hChan->activePktCount) && (0u == hChan->pendingPktCount))
            {
                /*Re-program the LCDC, with 'previous' frame only         */
                ioPacket = (IOM_Packet*)QUE_head(&(hChan->activeQ));

                /* Let's have this just check incase...                   */
                if( NULL != ioPacket)
                {
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,1u);
                }
            }
            /*
             We have some new packet pending. Take it and display it. In case of
             single frame buffer mode return the formerly active packet. However,
             in case of duble frame buffer mode do not return the formerly
             active packet, since it might be used for frame 1.
             */
            else if((1u == hChan->activePktCount) && (0 != hChan->pendingPktCount))
            {
                ioPacket = (IOM_Packet*)QUE_get(&(hChan->pendingQ));
                hChan->pendingPktCount--;

                if(NULL != ioPacket)
                {
                    QUE_put(&(hChan->activeQ),(Ptr)ioPacket);
                    hChan->activePktCount++;

                    /*Get actual Raster_FrameBuffer member from the ioPacket  */
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,1u);
                }
            }
            /*
            This case can occur when the active packet was not dequeued. This
            can occur, when the active packet was shared between frame 0
            and frame 1, like in the case of first two submits.
            */
            else
            {
                /*
                Here we have more than one active packets. Hence free the head.
                Get the active packet at head. This can be freed because,this
                packet is actually from the "other" frame (frame 1) and it has
                completed with it. However, we did not free it there because
                it might have been in use for the frame0, that is this frame.
                */
                ioPacket = (IOM_Packet*)QUE_get(&(hChan->activeQ));
                hChan->activePktCount--;

                if (NULL != ioPacket)
                {
                    ioPacket->status = rasterLpscOff(instHandle);

                    (*hChan->appCb)(hChan->cbArg,ioPacket);
                    hChan->submitCount--;
                }
                /*
                 What should be displayed now, depends on if there is any
                 newly submitted packet in the pending queue. If indeed pending
                 queue has any newly submitted packets, then take it from
                 pending queue. Else take it from the active queue. However,
                 freeing of the next pachet in active queue, will be done in the
                 next frame done interrupt.
                 */
                if (0 != hChan->pendingPktCount)
                {
                    ioPacket = (IOM_Packet*)QUE_get(&(hChan->pendingQ));
                    hChan->pendingPktCount--;

                    if (NULL != ioPacket)
                    {
                        QUE_put(&(hChan->activeQ),(Ptr)ioPacket);
                        hChan->activePktCount++;

                        /* Get actual Raster_FrameBuffer member from the IOP  */
                        fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                        rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,1u);
                    }
                }
                else
                {
                    /* Now get the packet at the head of the active queue.    *
                     * This needs to be displayed. However, we can neither    *
                     * return this packet, nor remove it from the queue, since*
                     * this is the only packet at hand.                       */
                    ioPacket = (IOM_Packet*)QUE_head(&(hChan->activeQ));

                    /* Get actual Raster_FrameBuffer member from the ioPacket */
                    fBuffer = (Raster_FrameBuffer*)ioPacket->addr;
                    rasterSetFrameAddress(lcdcRegs,fBuffer,hChan->dmaThrsld,1u);
                }
            }
#ifdef Raster_CAPTURE_EVENT_STATISTICS
            instHandle->eventStat.eof1++;
#endif /* Raster_CAPTURE_EVENT_STATISTICS */
        }
 #ifdef Raster_CAPTURE_EVENT_STATISTICS
        /* ac bias count done                                                 */
        if((CSL_LCDC_LCD_STAT_ABC_MASK & savedStatus)
            == CSL_LCDC_LCD_STAT_ABC_MASK)
        {
            instHandle->eventStat.abc++;
        }
#endif /* Raster_CAPTURE_EVENT_STATISTICS */
    }
    return;
}


/**
 * Reads the current raster configuration to the user buffer
 */
static Void rasterGetConf(Raster_ChanObject *hChan, Raster_RasterConf *rasterConf)
{

    assert(NULL != hChan);
    assert(NULL != rasterConf);

   memcpy(rasterConf, &hChan->chanConf, sizeof(Raster_RasterConf));

   return;
}


/**
 * This API updates the current interrupt masks and ends up in adding new
 * masks or maintaining the old one
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterAddEvtNotify(Raster_ChanObject *hChan, Uint32 *event)
{
    Uint32  newEventA = 0;
    Uint32  oldMaskA  = 0;
    Uint32  eventMask = (Raster_Intr_OFIFO_UNDERRUN_ENABLE
                         |Raster_Intr_FRAMESYNC_LOST_ENABLE
                         |Raster_Intr_PALETTE_LOADED_ENABLE
                         |Raster_Intr_ACBIAS_TRANSITION_ENABLE
                         |Raster_Intr_FRAME_DONE_ENABLE);

    assert(NULL != hChan);
    assert(NULL != event);

    if(0x0 != (eventMask & (*event)))
    {
        oldMaskA    = (Uint32)hChan->chanConf.intMask;
        newEventA   = (~oldMaskA) & (*event);
        if(0x0 != newEventA)
        {
            rasterSetIntr(hChan, (0xffu & newEventA));
        }
    }

    return;
}


/**
 * This API updates the interrupt masks and ends up in removing some masks or
 * maintaining the same
 * This function updates the channel object. The caller is expected to take
 * care of concurrent access policy
 */
static Void rasterRemEvtNotify(Raster_ChanObject *hChan, Uint32 *event)
{
    Uint32  newEventR = 0;
    Uint32  oldMaskR  = 0;
    Uint32  eventMask = (Raster_Intr_OFIFO_UNDERRUN_ENABLE
                         |Raster_Intr_FRAMESYNC_LOST_ENABLE
                         |Raster_Intr_PALETTE_LOADED_ENABLE
                         |Raster_Intr_ACBIAS_TRANSITION_ENABLE
                         |Raster_Intr_FRAME_DONE_ENABLE);

    assert(NULL != hChan);
    assert(NULL != event);

    if(eventMask != (eventMask & (*event)))
    {
        oldMaskR    = (Uint32)hChan->chanConf.intMask;
        newEventR   = oldMaskR & (~(*event));
        if(0x0 != newEventR)
        {
            rasterMaskInterrupt(hChan, (0xffu & (~newEventR)));
        }
    }

    return;
}


/**
 * Validates the raster configuration received from the user for boundary
 * values
 */
static Void rasterValidateConf(const Raster_RasterConf * rConf)
{
    assert(NULL != rConf);
    /* To remove compiler warnings */
	rConf = rConf;
    assert((Raster_Output_STN_RIGHTALIGNED == rConf->outputFormat) ||
           (Raster_Output_STN_INTRAPOLATED == rConf->outputFormat) ||
           (Raster_Output_TFT_RIGHTALIGNED == rConf->outputFormat) ||
           (Raster_Output_TFT_INTRAPOLATED == rConf->outputFormat));

    assert((Raster_Intface_SERIAL_4PIN == rConf->intface) ||
           (Raster_Intface_SERIAL_8PIN == rConf->intface) ||
           (Raster_Intface_PARALLEL_8PIN == rConf->intface) ||
           (Raster_Intface_PARALLEL_16PIN == rConf->intface));

    assert((Raster_Panel_STN == rConf->panel) ||
           (Raster_Panel_TFT == rConf->panel));

    assert((Raster_Display_MONOC == rConf->display) ||
           (Raster_Display_COLOR == rConf->display));

    assert((Raster_FbBitsPerPixel_1BPP == rConf->bitsPP) ||
           (Raster_FbBitsPerPixel_2BPP == rConf->bitsPP) ||
           (Raster_FbBitsPerPixel_4BPP == rConf->bitsPP) ||
           (Raster_FbBitsPerPixel_8BPP == rConf->bitsPP) ||
           (Raster_FbBitsPerPixel_12BPP == rConf->bitsPP) ||
           (Raster_FbBitsPerPixel_16BPP == rConf->bitsPP));

    assert((Raster_FBContent_PALETTE == rConf->fbContent) ||
           (Raster_FBContent_DATA == rConf->fbContent) ||
           (Raster_FBContent_PALETTE_DATA == rConf->fbContent));

    assert((Raster_DataOrder_LSBMSB == rConf->dataOrder) ||
           (Raster_DataOrder_MSBLSB == rConf->dataOrder));

    assert((FALSE == rConf->nibbleMode) || (TRUE == rConf->nibbleMode));

    assert((FALSE == rConf->subPanel.enable) || (TRUE == rConf->subPanel.enable));

    assert(RASTER_SUBPANEL_DPD_MAX > rConf->subPanel.defaultData);

    assert((FALSE == rConf->subPanel.enable) ||
           (TRUE == rConf->subPanel.enable));

    assert(RASTER_TIMING_LPP_MAX > rConf->subPanel.lPPT);

    assert((Raster_SubpanelPos_HIGH == rConf->subPanel.position) ||
           (Raster_SubpanelPos_LOW == rConf->subPanel.position));

    assert((FALSE == rConf->timing2.iAcbias) ||
           (TRUE == rConf->timing2.iAcbias));

    assert((FALSE == rConf->timing2.iPclk) ||
           (TRUE == rConf->timing2.iPclk));

    assert((FALSE == rConf->timing2.iLclk) ||
           (TRUE == rConf->timing2.iLclk));

    assert((FALSE == rConf->timing2.iFclk) ||
           (TRUE == rConf->timing2.iFclk));

    assert((FALSE == rConf->timing2.syncCtrl) ||
           (TRUE == rConf->timing2.syncCtrl));

    assert((Raster_SyncEdge_RISING == rConf->timing2.syncEdge) ||
           (Raster_SyncEdge_FALLING == rConf->timing2.syncEdge));

    assert(RASTER_ACB_I_MAX >= rConf->timing2.acBiasInt);

    assert(RASTER_ACB_FREQUENCY_MAX >= rConf->timing2.acBiasFreq);

    assert(RASTER_DMA_FIFO_DELAY_MAX >= rConf->fifoDmaDelay);

    assert(RASTER_TIMING_HFP_MAX >= rConf->hFP);

    assert(RASTER_TIMING_HBP_MAX >= rConf->hBP);

    assert(RASTER_TIMING_HSPW_MAX >= rConf->hSPW);

    assert(RASTER_TIMING_PPL_MAX >= rConf->pPL);

    assert(RASTER_TIMING_VFP_MAX >= rConf->vFP);

    assert(RASTER_TIMING_VBP_MAX >= rConf->vBP);

    assert(RASTER_TIMING_VSPW_MAX >= rConf->vSPW);

    assert(RASTER_TIMING_LPP_MAX >= rConf->lPP);

   return;
}


/**
 * This functions registers the LCDC main device interrupt handler
 * with PrKernel. This main ISR will call instance specific ISR.
 * This function registers only one ISR for any (different)lines
 */
static Void rasterRegIntHandler(Raster_Object* instHandle)
{
    Uint32   hwiKey = 0;
    ECM_Attrs       ecmattrs   = ECM_ATTRS;

    assert(NULL != instHandle);

    hwiKey = (Uint32)_disable_interrupts();

    /* Mapping the eventid to the ECM Dispatch of the DSP/BIOS                */
    ecmattrs.unmask = 1u;

    /*setup arg such that isr will get 'chan' as arg                          */
    ecmattrs.arg = (Arg)instHandle;

    /* Mapp the eventid to the Event Combiner Dispatch of the DSP/BIOS        */
    ECM_dispatchPlug(instHandle->instHwInfo.cpuEventNumber,
                     &rasterIntrHandler, &ecmattrs);

    /* Enabling the event                                                     */
    ECM_enableEvent(instHandle->instHwInfo.cpuEventNumber);

    /* Enabling the HWI_ID */
    C64_enableIER(1u << instHandle->instHwInfo.hwiNumber);

    /* Enabling the interrupts */
    _restore_interrupts(hwiKey);

    return;
}

/**
 * Undoes actions taken by rasterUnregIntHandler
 */
static Void rasterUnregIntHandler(Raster_Object *instHandle)
{
    assert(NULL != instHandle);
    /* Disabling interrupt */
    ECM_disableEvent(instHandle->instHwInfo.cpuEventNumber);

   return;
}


/**
 * Validates the device configuration values received from the user for
 * boundary conditions
 */
static Void rasterDeviceValidateConf(const Raster_DeviceConf * dConf)
{
    assert(NULL != dConf);
	/* To remove compiler warnings*/
	dConf = dConf;
    assert(0 != dConf->clkFreqHz);
    assert(Raster_OpMode_DMAINTERRUPT == dConf->opMode);
    assert(FALSE == dConf->dma.bigEndian);
    assert((FALSE == dConf->dma.eofInt) || (TRUE == dConf->dma.eofInt));
    assert((Raster_DmaBurstSize_1 == dConf->dma.burstSize) ||
           (Raster_DmaBurstSize_2 == dConf->dma.burstSize) ||
           (Raster_DmaBurstSize_4 == dConf->dma.burstSize) ||
           (Raster_DmaBurstSize_8 == dConf->dma.burstSize) ||
           (Raster_DmaBurstSize_16 == dConf->dma.burstSize));

    assert((Raster_DmaFb_SINGLE == dConf->dma.fbMode) ||
           (Raster_DmaFb_DOUBLE == dConf->dma.fbMode));

    return;
}

/**
 * \brief    rasterLpscOn
 *
 *           This function is invoked to power on the raster module and
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
Int32 rasterLpscOn(Raster_Object *instHandle)
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
 * \brief    rasterLpscOff
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
Int32 rasterLpscOff(Raster_Object *instHandle)
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
static Int32 RasterRegisterNotification(Raster_Object *instHandle)
{
    Uint32       eventCnt = 0;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Raster_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    &RasterNotifyCallback,
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
static Int32 RasterUnregisterNotification(Raster_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Raster_MAX_PWRM_EVENTS; eventCnt++)
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
static PWRM_NotifyResponse RasterNotifyCallback(PWRM_Event    eventType,
                                                Arg           eventArg1,
                                                Arg           eventArg2,
                                                Arg           clientArg)
{
    Raster_Object          *instHandle = NULL;
    Raster_ChanObject      *hChan      = NULL;
    PWRM_NotifyResponse     pwrmStatus = PWRM_NOTIFYDONE;

    assert(NULL != clientArg);

    instHandle = (Raster_Object *)clientArg;
    assert(NULL != instHandle);

    hChan = &(instHandle->ChannelObj);

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
                /* The raster shall support PWRM events only when channel is  *
                 * disabled and no packets are pending in driver. This means  *
                 * that the raster should be disabled. At this point it is    *
                 * expected that raster be disabled via IOCTL_RASTER_DISABLE. *
                 * Hence, there nothing much to be done here but for channel  *
                 * state and then returning the status                        */
            if (FALSE != hChan->enabled)
            {
                pwrmStatus = PWRM_NOTIFYERROR;
            }
            break;
        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
             /* The raster shall support PWRM events only when channel is      *
              * disabled and no packets are pending in driver. This means      *
              * that the raster should be disabled. At this point it is        *
              * expected that raster be disabled via IOCTL_RASTER_DISABLE.     *
              * Hence, there nothing much to be done here but for channel      *
              * state and then returning the status                            */
             if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                     (Raster_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                     (Raster_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                 if (FALSE != hChan->enabled)
                 {
                     pwrmStatus = PWRM_NOTIFYERROR;
                 }
            }
            break;
        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
             /* The driver shall now reconfigure the clock settings based     *
              * on the new set point of the system                            */
            if (((PWRM_DONE_CPU_SETPOINTCHANGE == eventType) &&
                     (Raster_pllDomain_0 == instHandle->pwrmInfo.pllDomain)) ||
               ((PWRM_DONE_PER_SETPOINTCHANGE == eventType) &&
                      (Raster_pllDomain_1 == instHandle->pwrmInfo.pllDomain)))
            {
                pwrmStatus = RasterConfigureNewVfSettings(
                                 instHandle,
                                 (Uns)eventArg1,
                                 (Uns)eventArg2);
            }
            break;
        case PWRM_AWAKEFROMSTANDBY:
        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
            /* There is nothing for the raster driver to be done here. Just   *
             * check the channel status. The raster will be enabled, during   *
             * first IOP submit                                               */
             if (FALSE != hChan->enabled)
             {
                 pwrmStatus = PWRM_NOTIFYERROR;
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
static PWRM_NotifyResponse RasterConfigureNewVfSettings(
                                                Raster_Object   *instHandle,
                                                Uns              eventArg1,
                                                Uns              eventArg2)
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
        status = RasterCalculateClockSettings(
                     instHandle,
                     eventArg2,
                     instHandle->devConf.clkFreqHz,
                     &preScale);
        /* successfully calculated now check if the prescaler is OK           */
        if (((RASTER_DEV_CLKDIV_MIN <= preScale)
            || (RASTER_DEV_CLKDIV_MAX >= preScale)) && (IOM_COMPLETED == status))
        {
            /* calcualation of the prescaler value is successful.Hence update *
             * the hardware with the new prescaler values                     */
            status = RasterConfigureNewClkSettings(instHandle,preScale);
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

static Int32 RasterCalculateConstraints(Raster_Object *instHandle)
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
    if (Raster_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
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
            status = RasterCalculateClockSettings(
                          instHandle,
                          setpoint,
                          instHandle->devConf.clkFreqHz,
                          &preScale);

            if (IOM_COMPLETED == status)
            {
                /* successfully calculated now check if the prescaler is OK   */
                if ((RASTER_DEV_CLKDIV_MIN > preScale)
                    || (RASTER_DEV_CLKDIV_MAX < preScale))
                {
                    /* register the constraint for this setpoint              */
                    mask |= (1u << setpoint);
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
static Int32 RasterCalculateClockSettings(Raster_Object *instHandle,
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
    if (Raster_pllDomain_0 == instHandle->pwrmInfo.pllDomain)
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
static Int32 RasterConfigureNewClkSettings(Raster_Object *instHandle,
                                          Uint32         prescale)
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
static Int32 RasterUnregisterConstraints(Raster_Object *instHandle)
{
    Int32        status = IOM_COMPLETED;
    PWRM_Status  retVal = PWRM_SOK;
    Uint32       count  = 0x00;

    for (count = 0; count < Raster_MAX_PWRM_CONSTRAINTS; count++)
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
/* ========================================================================== */
/*                            END OF FILE                                     */
/* ========================================================================== */

