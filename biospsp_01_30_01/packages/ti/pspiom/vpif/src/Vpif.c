/*
 * Vpif.c
 *
 * This file contains Driver Layer Interface implementation for the VPIF
 * Driver. VPIF Driver provides Driver Layer Interface to do operations
 * on the VPIF peripheral like device initialization, channel creation,
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
 *  \file   Vpif.c
 *
 *  \brief  IOM implementation for Vpif device.
 *
 *   The video port interface (VPIF) has two video input channels and two video
 *   output channels. Channels 0 and 1 are prepared only for input. Channels 2
 *   and 3 are prepared only for output. The VPIF supports raw data capturing.
 *   The VPIF can transmit and receive ancillary data in the video blanking
 *   interval (VBI) of BT.656 video streams. The ancillary data regions are
 *   referred to as: Horizontal Ancillary (HANC) - Data between EAV and SAV
 *   (horizontal blanking interval), and Vertical Ancillary (VANC) - Data
 *   between SAV and EAV (horizontal active video area).
 *   This file contains the driver implementation for the Vpif device.
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

#include <ti/pspiom/psc/Psc.h>
#include <ti/pspiom/cslr/cslr_vpif.h>
#include <ti/pspiom/vpif/VpifDebug.h>

#ifdef CHIP_OMAPL138
    #include <ti/pspiom/cslr/soc_OMAPL138.h>
#endif /* #ifdef CHIP_OMAPL138 */

#ifdef CHIP_C6748
    #include <ti/pspiom/cslr/soc_C6748.h>
#endif /* #ifdef CHIP_C6748 */

#ifdef BIOS_PWRM_ENABLE
    #include <pwrm.h>
#endif

#include "ti/pspiom/vpif/Fvid.h"
#include "ti/pspiom/vpif/Vpif.h"
#include "VpifLocal.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
#define Vpif_SLICED_MAX_SERVICES            (FVID_SLICE_VBI_SERVICES_MAX)
/**< Maximum Slice services supported                                         */
#define Vpif_CH0_MAX_MODES                  (12u)
/**< Maximum modes supported by channel 0                                     */
#define Vpif_CH1_MAX_MODES                  (2u)
/**< Maximum modes supported by channel 1                                     */
#define Vpif_CH2_MAX_MODES                  (2u)
/**< Maximum modes supported by channel 2                                     */
#define Vpif_CH3_MAX_MODES                  (2u)
/**< Maximum modes supported by channel 3                                     */
#define Vpif_NTSC_VBI_START_FIELD0          1
/**< Raw VBI start line number for NTSC std for field 0                       */
#define Vpif_NTSC_VBI_START_FIELD1          263
/**< Raw VBI start line number for NTSC std for field 1                       */
#define Vpif_PAL_VBI_START_FIELD0           624
/**< Raw VBI start line number for PAL std for field 0                        */
#define Vpif_PAL_VBI_START_FIELD1           311
/**< Raw VBI start line number for PAL std for field 1                        */
#define Vpif_NTSC_HBI_START_FIELD0          1
/**< Raw HBI start line number for NTSC std for field 0                       */
#define Vpif_NTSC_HBI_START_FIELD1          263
/**< Raw HBI start line number for NTSC std for field 1                       */
#define Vpif_PAL_HBI_START_FIELD0           624
/**< Raw HBI start line number for PAL std for field 0                        */
#define Vpif_PAL_HBI_START_FIELD1           311
/**< Raw HBI start line number for PAL std for field 1                        */
#define Vpif_NTSC_VBI_COUNT_FIELD0          20
/**< Raw VBI line count for NTSC std for field 0                              */
#define Vpif_NTSC_VBI_COUNT_FIELD1          19
/**< Raw VBI line count for NTSC std for field 0                              */
#define Vpif_PAL_VBI_COUNT_FIELD0           24
/**< Raw VBI line count for PAL std for field 0                               */
#define Vpif_PAL_VBI_COUNT_FIELD1           25
/**< Raw VBI line count for PAL std for field 1                               */
#define Vpif_NTSC_HBI_COUNT_FIELD0          263
/**< Raw HBI line count for NTSC std for field 0                              */
#define Vpif_NTSC_HBI_COUNT_FIELD1          262
/**< Raw HBI line count for NTSC std for field 1                              */
#define Vpif_PAL_HBI_COUNT_FIELD0           312
/**< Raw HBI line count for PAL std for field 0                               */
#define Vpif_PAL_HBI_COUNT_FIELD1           313
/**< Raw HBI line count for PAL std for field 1                               */
#define Vpif_NTSC_VBI_SAMPLES_PER_LINE      720
/**< Raw VBI samples per line for NTSC std                                    */
#define Vpif_PAL_VBI_SAMPLES_PER_LINE       720
/**< Raw VBI samples per line for PAL std                                     */
#define Vpif_NTSC_HBI_SAMPLES_PER_LINE      268
/**< Raw HBI samples per line for NTSC std                                    */
#define Vpif_PAL_HBI_SAMPLES_PER_LINE       280
/**< Raw HBI samples per line for PAL std                                     */

/* RAW VBI/HBI Params define
    samplePerLine,
    countFld0,
    countFld1
*/
/**\brief RAW VBI parameters for NTSC Std*/
#define Vpif_NTSC_VBI_PARAMS \
    { \
        Vpif_NTSC_VBI_SAMPLES_PER_LINE, \
        Vpif_NTSC_VBI_COUNT_FIELD0, \
        Vpif_NTSC_VBI_COUNT_FIELD1 \
    }
/**\brief RAW VBI parameters for PAL Std*/
#define Vpif_NTSC_HBI_PARAMS \
    { \
        Vpif_NTSC_HBI_SAMPLES_PER_LINE, \
        Vpif_NTSC_HBI_COUNT_FIELD0, \
        Vpif_NTSC_HBI_COUNT_FIELD1 \
    }
/**\brief RAW HBI parameters for NTSC Std*/
#define Vpif_PAL_VBI_PARAMS \
    { \
        Vpif_PAL_VBI_SAMPLES_PER_LINE, \
        Vpif_PAL_VBI_COUNT_FIELD0, \
        Vpif_PAL_VBI_COUNT_FIELD1 \
    }
/**\brief RAW HBI parameters for PAL Std*/
#define Vpif_PAL_HBI_PARAMS \
    { \
        Vpif_PAL_HBI_SAMPLES_PER_LINE, \
        Vpif_PAL_HBI_COUNT_FIELD0, \
        Vpif_PAL_HBI_COUNT_FIELD1 \
    }

/*
    Vpif_VideoMode, width, height, fps, Vpif_FrameFormat,
    Vpif_YCMuxed, eav2sav, sav2eav, l1, l3, l5, l7,
    l9, l11, vsize, Vpif_CaptureFormat, isVbiSupported, isHd,
    hancOffset, rawHbiParams, rawVbiParams
*/
/**\brief SD parameters for VPIF */
#define VPIF_SD_PARAMS \
    {Vpif_VideoMode_NTSC, 720, 480, 30, Vpif_FrameFormat_INTERLACED, \
     Vpif_YCMuxed_YES, 268, 1440, 1, 23, 263, 266, \
     286, 525, 525, Vpif_CaptureFormat_BT, TRUE, FALSE, \
     720, Vpif_NTSC_HBI_PARAMS, Vpif_NTSC_VBI_PARAMS}, \
    {Vpif_VideoMode_PAL, 720, 576, 25, Vpif_FrameFormat_INTERLACED, \
     Vpif_YCMuxed_YES, 280, 1440, 1, 23, 311, 313, \
     336, 624, 625, Vpif_CaptureFormat_BT, TRUE, FALSE, \
     720, Vpif_PAL_HBI_PARAMS, Vpif_PAL_VBI_PARAMS}

/**\brief MT9T001 parameters for VPIF */
#define VPIF_MT9T001_PARAMS \
    {Vpif_VideoMode_RAW_VGA, 640, 480, 93, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_SVGA, 800, 600, 65, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_XGA, 1024, 768, 43, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_SXGA, 1280, 1024, 27, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_UXGA, 1600, 1200, 20, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_QXGA, 2048, 1536, 12, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_480P, 720, 480, 60, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_576P, 720, 576, 50, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_720P, 1280, 720, 39, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}, \
    {Vpif_VideoMode_RAW_1080P, 1920, 1080, 18, Vpif_FrameFormat_PROGRESSIVE, \
     Vpif_YCMuxed_NO, 0, 0, 0, 0, 0, 0, \
     0, 0, 0, Vpif_CaptureFormat_CCDC, FALSE, FALSE, \
     0, {0, 0, 0}, {0, 0, 0}}

/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */
/**\brief Array used to save the different format supported by channel 0 */
static const Vpif_ConfigParams ch0Params[Vpif_CH0_MAX_MODES] =
{
    VPIF_SD_PARAMS,
    VPIF_MT9T001_PARAMS
};

/**\brief Array used to save the different format supported by channel 1 */
static const Vpif_ConfigParams ch1Params[Vpif_CH1_MAX_MODES] =
{
    VPIF_SD_PARAMS
};

/**\brief Array used to save the different format supported by channel 2 */
static const Vpif_ConfigParams ch2Params[Vpif_CH2_MAX_MODES] =
{
    VPIF_SD_PARAMS
};

/**\brief Array used to save the different format supported by all channels */
static const Vpif_ConfigParams *vpifConfigParams[Vpif_TOTAL_NUM_CHANNELS] =
{
    ch0Params,
    ch1Params,
    ch2Params,
    ch2Params
};

/**\brief Default instance parameters */
const Vpif_Params Vpif_PARAMS =
{
    9,                                          /* hwiNumber            */
    Vpif_DmaReqSize_128BYTE,                    /* dma request size     */
    FALSE,                                      /* driver power aware or not */
    Vpif_PllDomain_0                            /* PLL domain */
};

/**\brief Array used to ignore the first interrupt when it comes */
static Uint8 channelFirstInt[Vpif_TOTAL_NUM_CHANNELS] = {1u, 1u, 1u, 1u};

/**\brief Array which is part of Vpif Module State */
static Bool drvInUse[CSL_VPIF_PER_CNT];

/**\brief Vpif Module State Object */
static Vpif_Module_State Vpif_module = {&drvInUse[CSL_VPIF]};

/**\brief Array of Vpif instance State objects array */
static Vpif_Object Vpif_Instances[CSL_VPIF_PER_CNT];

/**\brief Variable saving the raw selective Vbi parameters in case of display */
static Vpif_RawSelectiveVbiParams gVpifRawSelVbiParam[Vpif_DISPLAY_NUM_CHANNELS];

/**\brief Variable saving the raw selective Hbi parameters in case of display */
static Vpif_RawSelectiveVbiParams gVpifRawSelHbiParam[Vpif_DISPLAY_NUM_CHANNELS];

/* ========================================================================== */
/*                           IOM FUNCTION PROTOTYPES                          */
/* ========================================================================== */
static Int32 vpifMdBindDev(Ptr *devp, Int32 devId, Ptr devParams);

static Int32 vpifMdUnBindDev(Ptr devp);

static Int32 vpifMdCreateChan(Ptr              *chanp,
                              Ptr              devp,
                              String           name,
                              Int32            mode,
                              Ptr              chanParams,
                              IOM_TiomCallback cbFxn,
                              Ptr              cbArg);

static Int32 vpifMdDeleteChan(Ptr chanp);

static Int32 vpifMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket);

static Int32 vpifMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg);

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 vpifLpscOn(Vpif_Object *instHandle);

static Int32 vpifLpscOff(Vpif_Object *instHandle);

#ifdef BIOS_PWRM_ENABLE
static Int32 vpifRegisterNotification(Vpif_Object *instHandle);

static PWRM_NotifyResponse vpifNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg);

static Int32 vpifUnregisterNotification(Vpif_Object *instHandle);
#endif

static Void vpifSliceVbiCaptureTask(Arg arg);

static Void vpifSliceVbiDisplayTask(Arg arg);

static Void vpifProcessErrFrm(Vpif_ChanObj *chanHandle);

static Void vpifSliceVbiSwiHandler(Arg arg0, Arg unUsedArg);

static Uint32 vpifGetFieldId(Vpif_ChanObj *chanHandle);

static Void vpifProcessCurrentCapFrm(Vpif_ChanObj *chanHandle);

static Void vpifConfigureNextCapFrm(Vpif_ChanObj *chanHandle);

static Void vpifProcessCurrentDispFrm(Vpif_ChanObj *chanHandle);

static Void vpifConfigureNextDispFrm(Vpif_ChanObj *chanHandle);

static Void vpifSliceCb(Ptr handle, Int32 *status);

Void vpifIsr(Arg arg);

static Void vpifRegisterInterrupt(Ptr inDevp, Uint16 hwiNumber, Uint16 evt);

static Void vpifUnRegisterInterrupt(Uint16 evt);

static Int32 vpifAllocBuffer(Vpif_ChanObj *chanHandle, FVID_Frame **newFrame);

static Int32 vpifAllocAssignBuffer(Vpif_ChanObj *chanHandle,
                                   FVID_Frame *newFrame);

static Int32 vpifFreeBuffer(Vpif_ChanObj *chanHandle, FVID_Frame **freeFrame);

static Int32 vpifGetNumPendingIoReq(Vpif_ChanObj *chanHandle, Ptr args);

static Int32 vpifSetResolution(Vpif_ChanObj *chanHandle,
                               Vpif_ConfigParams *resolution);

static Int32 vpifGetChanParams(Vpif_ChanObj *chanHandle,
                               Vpif_StdInfo *currParams);

static Void vpifSetHancAddressOffset(Vpif_ChanObj *chanHandle);

static Void vpifSetHbiDisplayParams(Vpif_ChanObj *chanHandle);

static Void vpifSetVbiDisplayParams(Vpif_ChanObj *chanHandle);

static Void vpifSetRawParams(Vpif_ChanObj *chanHandle, Bool enable);

static Uint8 vpifSetVideoParams(Vpif_ChanObj *chanHandle);

static Void vpifSetModeInfo(Vpif_ChanObj *chanHandle, Uint8 channelId);

static Void vpifConfigureAddress(Vpif_ChanObj *chanHandle);

static Void vpifConfigureVbiAddress(Vpif_ChanObj *chanHandle);

static Void vpifConfigureHbiAddress(Vpif_ChanObj *chanHandle);

static Int32 vpifStartTransfer(Vpif_ChanObj *chanHandle);

static Void vpifSetChannelFrameInterrupt(Vpif_ChanObj *chanHandle);

static Void vpifChannelIntrEnable(Vpif_ChanObj *chanHandle, Bool enable);

static inline Void vpifEnableChannel(Vpif_ChanObj *chanHandle, Bool enable);

static Int32 vpifStopTransfer(Vpif_ChanObj *chanHandle);

static Void vpifValidateFrameParms(Vpif_ChanObj *chanHandle, FVID_Frame *viop);

static Int32 vpifQueueBuffer(Vpif_ChanObj *chanHandle, IOM_Packet *packet);

static Int32 vpifDequeueBuffer(Vpif_ChanObj *chanHandle, IOM_Packet *packet);

static Int32 vpifGetBufs(Vpif_ChanObj *chanHandle, IOM_Packet *packet);

/* ========================================================================== */
/*                   Public Mini Driver interface table                       */
/* ========================================================================== */
IOM_Fxns Vpif_IOMFXNS =
{
    &vpifMdBindDev,        /* VPIF Driver Bind function             */
    &vpifMdUnBindDev,      /* VPIF Driver Un-Bind function          */
    &vpifMdControlChan,    /* VPIF Driver control function          */
    &vpifMdCreateChan,     /* VPIF Driver channel create function   */
    &vpifMdDeleteChan,     /* VPIF Driver channel deletion function */
    &vpifMdSubmitChan      /* VPIF Driver submit function           */
};

/* ========================================================================== */
/*                          FUNCTION DEFINITIONS                              */
/* ========================================================================== */


/* ========================================================================== */
/*                           MODULE FUNCTIONS                                 */
/* ========================================================================== */
/**
 *  \brief  Function called by Bios during instance initialization
 *
 *  \return None
 */
Void Vpif_init(Void)
{
    Int32 i;

    for (i = 0; i < CSL_VPIF_PER_CNT; i++)
    {
        /* have to initialize statically */
        Vpif_module.inUse[i] = FALSE;
        memset((Void *)&Vpif_Instances[i], 0x0, sizeof(Vpif_Object));
    }
}


/**
 *  \brief  Function called by Bios during instance initialization. This
 *          function initializes the channel parameters with default values
 *          and registers the interrupt handler. It also creates the SWI
 *          handler for VBI/HBI handling.
 *
 *  \param  devp       [OUT]      pointer to Vpif driver object
 *  \param  devId      [IN]       device Id
 *  \param  devParams  [IN]       device Parameters
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID         in case of error
 *
 *  \enter  *devp           must be a valid pointer and should not be null.
 *          devId           must be a valid device number (<CSL_VPIF_PER_CNT)
 *          devParams       must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 vpifMdBindDev(Ptr *devp, Int32 devId, Ptr devParams)
{
    Int32           bndRtnVal = IOM_COMPLETED;
    Vpif_Object     *bndInstHandle = NULL;
    Vpif_Params     *params;
    Int32           j;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == devp) || (NULL == devParams) || (CSL_VPIF_PER_CNT  <= devId))
    {
        bndRtnVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    bndInstHandle = &Vpif_Instances[devId];
    params = (Vpif_Params *)devParams;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == bndRtnVal)
    {
        if ((NULL == params) || (0 == params->hwiNumber))
        {
            bndRtnVal = IOM_EBADARGS;
        }

        if ((((TRUE == Vpif_module.inUse[devId]) ||
              (Vpif_DmaReqSize_32BYTE > params->dmaReqSize)) ||
             (Vpif_DmaReqSize_256BYTE < params->dmaReqSize)) ||
            (Vpif_DriverState_CLOSED != bndInstHandle->devState))
        {
            /* Driver not in deleted (initial) state or in use                */
            bndRtnVal = IOM_EBADMODE;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */
    if (IOM_COMPLETED == bndRtnVal)
    {
        /* Initialize the soc specific information                            */
        if (0 == devId)
        {
            bndInstHandle->deviceInfo.baseAddr = (Ptr)CSL_VPIF_0_REGS;
            bndInstHandle->deviceInfo.cpuEventNumber = CSL_INTC_EVENTID_VPIFINT;
            bndInstHandle->deviceInfo.hwiNo = params->hwiNumber;
            bndInstHandle->dmaSize = params->dmaReqSize;
            bndInstHandle->isPwrAware = params->pscPwrmEnable;
            bndInstHandle->deviceInfo.pscInstance = \
                (Uint32)CSL_LPSC_INST_VPIF_0;
            #ifndef BIOS_PWRM_ENABLE
                bndInstHandle->deviceInfo.pwrmLpscID = (Uint32)CSL_PSC_VPIF;
            #else
                bndInstHandle->deviceInfo.pwrmLpscID = \
                    (Uint32)CSL_LPSC_NUMBER_VPIF_0;
            #endif
        }

        if (FALSE == params->pscPwrmEnable)
        {
            /*
             * The first thing to do when power management is not desired is to
             * turn on the controller using the PSC or PWRM API
             */
            bndRtnVal = vpifLpscOn(bndInstHandle);
        }
    }

    if (IOM_COMPLETED == bndRtnVal)
    {
        /* Initialize all the channel specific parameters to default state    */
        for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
        {
            bndInstHandle->chanObj[j].chState = Vpif_DriverState_CLOSED;
            bndInstHandle->chanObj[j].devHandle = NULL;
            bndInstHandle->chanObj[j].chError = 0;
            bndInstHandle->chanObj[j].frameInt = 0;
            bndInstHandle->chanObj[j].chanNum = 0;
            /* Assign same VPIF base address for all channels                 */
            bndInstHandle->chanObj[j].baseAddress = \
                bndInstHandle->deviceInfo.baseAddr;
            bndInstHandle->chanObj[j].frameStorageMode = \
                Vpif_SdramStorage_FIELD;
            bndInstHandle->chanObj[j].cbFxn = NULL;
            bndInstHandle->chanObj[j].cbArg = NULL;
            bndInstHandle->chanObj[j].vidMode = Vpif_VideoMode_NONE;
            bndInstHandle->chanObj[j].channelMode = Vpif_IoMode_NONE;
            bndInstHandle->chanObj[j].edcFxns = NULL;
            bndInstHandle->chanObj[j].edcHandle = NULL;
            bndInstHandle->chanObj[j].segId = -1;
            bndInstHandle->chanObj[j].bufSz = 0;
            bndInstHandle->chanObj[j].hpitch = 0;
            bndInstHandle->chanObj[j].lineOffset = 0;
            bndInstHandle->chanObj[j].alignment = 0;
            bndInstHandle->chanObj[j].started = 0;
            bndInstHandle->chanObj[j].useTwoChannels = 0;
            bndInstHandle->chanObj[j].fieldId = 0;
            memset(&bndInstHandle->chanObj[j].chVidParams,
                0x00,
                sizeof(Vpif_ConfigParams));
            bndInstHandle->chanObj[j].queEmpty = 0;
            bndInstHandle->chanObj[j].curViop = NULL;
            bndInstHandle->chanObj[j].nextViop = NULL;
            bndInstHandle->chanObj[j].mrViop = NULL;
            bndInstHandle->chanObj[j].packetIOM = NULL;
            bndInstHandle->chanObj[j].numQueBuf = 0;
            bndInstHandle->chanObj[j].hbiBufSz = 0;
            bndInstHandle->chanObj[j].vbiBufSz = 0;
            bndInstHandle->chanObj[j].vbiTskletHandle = NULL;
            bndInstHandle->chanObj[j].sliceServType = 0;
            bndInstHandle->chanObj[j].numSliceServices = 0;
            bndInstHandle->chanObj[j].setSelVbiDispParams = NULL;
            bndInstHandle->chanObj[j].setSelHbiDispParams = NULL;

            bndInstHandle->chanObj[j].sliceVbiFld0I2cCbStatus = IOM_COMPLETED;
            bndInstHandle->chanObj[j].sliceVbiFld0I2cSubmitStatus = IOM_PENDING;
            bndInstHandle->chanObj[j].sliceVbiFld1I2cCbStatus = IOM_COMPLETED;
            bndInstHandle->chanObj[j].sliceVbiFld1I2cSubmitStatus = IOM_PENDING;
            bndInstHandle->chanObj[j].currPktStatus = IOM_COMPLETED;

            bndInstHandle->chanObj[j].dataSz = Vpif_RawCaptureDataWidth_8BITS;
            bndInstHandle->chanObj[j].fieldPolarity = \
                Vpif_RawCapturePinPol_SAME;
            bndInstHandle->chanObj[j].vPixelPol = Vpif_RawCapturePinPol_SAME;
            bndInstHandle->chanObj[j].hPixelPol = Vpif_RawCapturePinPol_SAME;
        }

        bndInstHandle->devState = Vpif_DriverState_CLOSED;
        bndInstHandle->instNum = devId;

        /* If the driver is power aware enable the PSC */
        if (TRUE == bndInstHandle->isPwrAware)
        {
            /* Enable LPSC as registers are touched */
            bndRtnVal = vpifLpscOn(bndInstHandle);
        }
        if (IOM_COMPLETED == bndRtnVal)
        {
            /* Clear VPIF interrupts                                          */
            bndInstHandle->deviceInfo.baseAddr->INTSTATCLR = \
               (((CSL_VPIF_INTCLR_ERROR_CLR) << CSL_VPIF_INTCLR_ERROR_SHIFT) &
                  CSL_VPIF_INTCLR_ERROR_MASK) |
               (((CSL_VPIF_INTCLR_FRAME0_CLR) << CSL_VPIF_INTCLR_FRAME0_SHIFT) &
                  CSL_VPIF_INTCLR_FRAME0_MASK) |
               (((CSL_VPIF_INTCLR_FRAME1_CLR) << CSL_VPIF_INTCLR_FRAME1_SHIFT) &
                  CSL_VPIF_INTCLR_FRAME1_MASK) |
               (((CSL_VPIF_INTCLR_FRAME2_CLR) << CSL_VPIF_INTCLR_FRAME2_SHIFT) &
                  CSL_VPIF_INTCLR_FRAME2_MASK) |
               (((CSL_VPIF_INTCLR_FRAME3_CLR) << CSL_VPIF_INTCLR_FRAME3_SHIFT) &
                  CSL_VPIF_INTCLR_FRAME3_MASK);

            /* Clear registers in case VPIF is still running from a previous
            run  */
            bndInstHandle->deviceInfo.baseAddr->C0CTRL = \
                CSL_VPIF_C0CTRL_RESETVAL;
            bndInstHandle->deviceInfo.baseAddr->C1CTRL = \
                CSL_VPIF_C1CTRL_RESETVAL;
            bndInstHandle->deviceInfo.baseAddr->C2CTRL = \
                CSL_VPIF_C2CTRL_RESETVAL;
            bndInstHandle->deviceInfo.baseAddr->C3CTRL = \
                CSL_VPIF_C3CTRL_RESETVAL;

            while (bndInstHandle->deviceInfo.baseAddr->INTSTAT)
            {
                /* Clear any previous interrupts */
                bndInstHandle->deviceInfo.baseAddr->INTSTATCLR = \
                                    bndInstHandle->deviceInfo.baseAddr->INTSTAT;
            }

            bndInstHandle->deviceInfo.baseAddr->INTCLR = 0x1Fu;
            bndInstHandle->deviceInfo.baseAddr->EMUCTRL = \
                CSL_VPIF_EMUCTRL_FREE_SUSP;

            /* Register interrupts                                            */
            vpifRegisterInterrupt((Ptr)bndInstHandle,
                bndInstHandle->deviceInfo.hwiNo,
                (Uint16)(bndInstHandle->deviceInfo.cpuEventNumber));

            /* Set one time configuration field like program the dma size */
            switch (bndInstHandle->dmaSize)
            {
                case Vpif_DmaReqSize_32BYTE:
                    bndInstHandle->deviceInfo.baseAddr->REQSIZE = \
                        CSL_VPIF_REQSIZE_BYTES_32;
                    break;
                case Vpif_DmaReqSize_64BYTE:
                    bndInstHandle->deviceInfo.baseAddr->REQSIZE = \
                        CSL_VPIF_REQSIZE_BYTES_64;
                    break;
                case Vpif_DmaReqSize_128BYTE:
                    bndInstHandle->deviceInfo.baseAddr->REQSIZE = \
                        CSL_VPIF_REQSIZE_BYTES_128;
                    break;
                case Vpif_DmaReqSize_256BYTE:
                    bndInstHandle->deviceInfo.baseAddr->REQSIZE = \
                        CSL_VPIF_REQSIZE_BYTES_256;
                    break;
                default:
                    bndRtnVal = IOM_EBADARGS;
                    break;
            }

            /* If the driver is power aware disable the PSC */
            if (TRUE == bndInstHandle->isPwrAware)
            {
                /* Disable LPSC */
                bndRtnVal = vpifLpscOff(bndInstHandle);
            }
        }
    }

#ifdef BIOS_PWRM_ENABLE
    if (IOM_COMPLETED == bndRtnVal)
    {
        memset(&bndInstHandle->pwrmInfo, 0x00, sizeof(Vpif_PwrmInfo));
        bndInstHandle->pwrmInfo.pllDomain = params->pllDomain;
        if (TRUE == bndInstHandle->isPwrAware)
        {
            /* register the notify function for the PWRM events               */
            bndRtnVal = vpifRegisterNotification(bndInstHandle);
        }
    }
#endif


    if (IOM_COMPLETED == bndRtnVal)
    {
        /* set the status of the module as in use                             */
        Vpif_module.inUse[devId] = TRUE;
        bndInstHandle->devState |= Vpif_DriverState_CREATED;
        *devp = (Ptr)bndInstHandle;
    }

    return bndRtnVal;
}

/**
 *  \brief  Function called by Bios during closing of the instance. This
 *          function unregisters the interrupt handler and frees up the
 *          resources, if any.
 *
 *  \param  devp       [IN]  pointer to Vpif driver object
 *
 *  \return IOM_COMPLETED    if success
 *          Error ID         in case of error
 *
 *  \enter  devp       must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 vpifMdUnBindDev(Ptr devp)
{
    Int32           unBndRtnVal = IOM_COMPLETED;
    Vpif_Object     *unBndInstHandle = NULL;
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    Uint8           j;
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */

    VPIF_DEBUG("Entered ---> vpifMdUnBindDev\n");

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == devp)
    {
        unBndRtnVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    unBndInstHandle = devp;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == unBndRtnVal)
    {

        if (((CSL_VPIF_PER_CNT <= unBndInstHandle->instNum) ||
             (FALSE == Vpif_module.inUse[unBndInstHandle->instNum])) ||
            (Vpif_DriverState_CREATED !=
             (Vpif_DriverState_CREATED & unBndInstHandle->devState)))
        {
            unBndRtnVal = IOM_EBADARGS;
        }

        /* Check if all the channels of port are closed */
        for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
        {
            if (Vpif_DriverState_OPENED ==
                (Vpif_DriverState_OPENED & unBndInstHandle->chanObj[j].chState))
            {
                unBndRtnVal = IOM_EBADARGS;
            }
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == unBndRtnVal)
    {
        /* Unregister interrupts                                          */
        vpifUnRegisterInterrupt(unBndInstHandle->deviceInfo.cpuEventNumber);

        /* Since all the channels of port are closed make unBndInstHandle
         * status as closed */
        unBndInstHandle->devState = Vpif_DriverState_CLOSED;
        Vpif_module.inUse[unBndInstHandle->instNum] = FALSE;
        if (FALSE == unBndInstHandle->isPwrAware)
        {
            /*
             * The last thing to do when power management is not desired is to
             * turn off the controller using the PSC or PWRM API
             */
            unBndRtnVal = vpifLpscOff(unBndInstHandle);
        }
    }

#ifdef BIOS_PWRM_ENABLE
    if ((IOM_COMPLETED == unBndRtnVal) && (TRUE == unBndInstHandle->isPwrAware))
    {
        /* unregister all the PWRM event notifications                    */
        unBndRtnVal = vpifUnregisterNotification(unBndInstHandle);
    }
#endif

    VPIF_DEBUG("Exited ---> vpifMdUnBindDev\n");

    return (unBndRtnVal);
}

/* ========================================================================== */
/*                           IOM FUNCTIONS                                    */
/* ========================================================================== */

/**
 *  \brief  Creates a communication channel in specified mode to communicate
 *          data between the application and the Vpif device instance. This
 *          function sets the required hardware configurations for the data
 *          transactions. It returns configured channel handle to application,
 *          which will be used in all further transactions with the channel.
 *          This function is called in response to a FVID_create/GIO_create call.
 *
 *  \param     chanp        [OUT]    channel object pointer
 *  \param     devp         [IN]     pointer to vpif instance
 *  \param     name         [IN]     Vpif Instance name like Vpif0
 *  \param     mode         [IN]     channel  mode -> input or output
 *  \param     chanParams   [IN]     channel parameters from user
 *  \param     cbFxn        [IN]     callback function pointer
 *  \param     cbArg        [IN]     callback function Arguments
 *
 *  \return    IOM_COMPLETED    in case of success
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
static Int32 vpifMdCreateChan(Ptr              *chanp,
                              Ptr              devp,
                              String           name,
                              Int32            mode,
                              Ptr              chanParams,
                              IOM_TiomCallback cbFxn,
                              Ptr              cbArg)
{
    Vpif_Object            *crtChanInstHandle = NULL;
    Vpif_ChanObj           *crtChanHandle = NULL;
    Int32                  crtChanRtnVal = IOM_COMPLETED;
    Uint8                  crtChanNum = 255u;/* invalid value */
    Uint8                  maxModes;/* Max video modes supported by the chnl */
    Uint32                 index;
    Uint8                  dispChNo;
    Vpif_CapChanParams     *paramsCap;
    Vpif_DisChanParams     *paramsDis;
    SWI_Attrs              swiAttrs   = {0};
    Edc_Data               edcParams;
    TSK_Attrs              tskAttrs;
    SEM_Attrs              semAttrs;
    Uint32                 sliceServices = 0;

    VPIF_DEBUG("Entered ---> vpifMdCreateChan\n");

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if ((NULL == cbFxn) || (NULL == cbArg) || (NULL == chanParams) ||
        (NULL == chanp) || (NULL == devp) || (NULL == name) ||
        (name[0] != '/') || ((IOM_INPUT != mode) && (IOM_OUTPUT != mode)))
    {
        /* Return error if any pointer parameter is NULL */
        crtChanRtnVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    paramsCap = (Vpif_CapChanParams *)chanParams;
    paramsDis = (Vpif_DisChanParams *)chanParams;
    crtChanInstHandle = (Vpif_Object *)devp;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == crtChanRtnVal)
    {
        crtChanNum = (Uint8)(name[1] - '0');

        /* check if the device is created or not. Also check if the channel was
           already opened. if already open then flag error and bail out       */


        if (Vpif_IoMode_DIS == paramsDis->dispChannelIoMode)
        {
            if ((IOM_OUTPUT != mode) ||
                (FALSE == ((Vpif_CHANNEL2_VIDEO == crtChanNum) ||
                           (Vpif_CHANNEL3_VIDEO == crtChanNum))))
            {
                /*
                 * Return error as either opened in invalid mode or with invalid
                 * channel numbers for the I/O mode
                 */
                crtChanRtnVal = IOM_EBADARGS;
            }
        }
        else if ((Vpif_IoMode_CAP == paramsCap->capChannelIoMode) ||
                 (Vpif_IoMode_RAW_CAP == paramsCap->capChannelIoMode))
        {
            if ((IOM_INPUT != mode) ||
                (FALSE == ((Vpif_CHANNEL0_VIDEO == crtChanNum) ||
                           (Vpif_CHANNEL1_VIDEO == crtChanNum))))
            {
                /*
                 * Return error as either opened in invalid mode or with invalid
                 * channel numbers for the I/O mode
                 */
                crtChanRtnVal = IOM_EBADARGS;
            }
        }
        else
        {
            /* Return error as wrong I/O mode for channel to work             */
            crtChanRtnVal = IOM_EBADARGS;
        }
    }

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        if ((Vpif_DriverState_CREATED != crtChanInstHandle->devState) ||
            (Vpif_DriverState_CLOSED !=
             crtChanInstHandle->chanObj[crtChanNum].chState))
        {
            crtChanRtnVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        /* name is like => /channel no/ enc or dec string. */
        name++;/* Get rid of '/' */
        crtChanNum = (Uint8)(name[0] - '0');
        name++;/* Get rid of channel number */

        if ((NULL == paramsCap->capVideoParams) &&
            (Vpif_VideoMode_NONE == paramsCap->capStdMode))
        {
            crtChanRtnVal = IOM_EBADARGS;
        }
    }

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        /* Initialize the channel object and assign parameters passed from
           application to Vpif_ChanObj global member */
        crtChanHandle = &crtChanInstHandle->chanObj[crtChanNum];
        crtChanHandle->devHandle = crtChanInstHandle;
        crtChanHandle->chanNum = crtChanNum;
        /*Assign the respective callback function                             */
        crtChanHandle->cbFxn = cbFxn;
        crtChanHandle->cbArg = cbArg;
        crtChanHandle->vidMode = paramsCap->capStdMode;
        crtChanHandle->numQueBuf = 0;
        crtChanHandle->mrViop = NULL;
        crtChanHandle->packetIOM = NULL;
        crtChanHandle->curViop = NULL;
        crtChanHandle->nextViop = NULL;
        crtChanHandle->queEmpty = FALSE;
        crtChanHandle->alignment = paramsCap->capFbParams.frmBufAlignment;
        crtChanHandle->segId = paramsCap->capFbParams.frmBufSegId;
        crtChanHandle->edcFxns = paramsCap->capEdcTbl;
        crtChanHandle->channelMode = paramsCap->capChannelIoMode;

        if (IOM_INPUT == mode)
        {
            if (Vpif_IoMode_RAW_CAP == crtChanHandle->channelMode)
            {
                /* It is raw mode video capture request */
                crtChanHandle->dataSz = paramsCap->capDataSize;
                crtChanHandle->fieldPolarity = paramsCap->capFieldPol;
                crtChanHandle->vPixelPol = paramsCap->capVPixPol;
                crtChanHandle->hPixelPol = paramsCap->capHPixPol;
            }
            /* Create the input queues here as only one is required for
            capture*/
            QUE_new(&(crtChanHandle->qIn));
        }
        else
        {
            /* Create the input and output queues here.                 */
            QUE_new(&(crtChanHandle->qOut));
            QUE_new(&(crtChanHandle->qIn));
        }

        if (NULL == paramsCap->capVideoParams)
        {
            switch (crtChanNum)
            {
                case Vpif_CHANNEL0_VIDEO:
                    maxModes = Vpif_CH0_MAX_MODES;
                    break;
                case Vpif_CHANNEL1_VIDEO:
                    maxModes = Vpif_CH1_MAX_MODES;
                    break;
                case Vpif_CHANNEL2_VIDEO:
                    maxModes = Vpif_CH2_MAX_MODES;
                    break;
                case Vpif_CHANNEL3_VIDEO:
                    maxModes = Vpif_CH3_MAX_MODES;
                    break;
                default:
                    /* This condition will not arise. */
                    maxModes = 0;
                    break;
            }
            /* loop on the number of mode supported per channel */
            for (index = 0; index < maxModes; index++)
            {
                /* If the mode is found, set the parameter in VPIF register */
                if (vpifConfigParams[crtChanNum][index].mode ==
                        crtChanHandle->vidMode)
                {
                    break;
                }
            }
            if (index == maxModes)
            {
                crtChanRtnVal = IOM_EBADARGS;
            }
            else
            {
                crtChanHandle->chVidParams = \
                    vpifConfigParams[crtChanNum][index];
                crtChanRtnVal = IOM_COMPLETED;
            }
        }
        else
        {
            crtChanHandle->chVidParams = *(paramsCap->capVideoParams);
        }

        /* Calculate VBI params */
        if (((Vpif_VbiServiceType_NONE != paramsCap->capVbiService) &&
            (TRUE == crtChanHandle->chVidParams.isVbiSupported)) &&
            (IOM_COMPLETED == crtChanRtnVal))
        {
            if (Vpif_ALL_VBI_SERVICES ==
                (Vpif_ALL_VBI_SERVICES | paramsCap->capVbiService))
            {
                if (Vpif_VbiServiceType_RAW_VBI ==
                    (Vpif_VbiServiceType_RAW_VBI & paramsCap->capVbiService))
                {
                    /* Calculate the size of the RAW VBI buffer */
                    crtChanHandle->vbiBufSz = \
                        crtChanHandle->chVidParams.rawVbiParams.samplePerLine *
                        (crtChanHandle->chVidParams.rawVbiParams.countFld0 +
                        crtChanHandle->chVidParams.rawVbiParams.countFld1);

                    /* Buffer size is not calculated for display based on
                    vVbiParams bec of following note in spruee. Note that the
                    user is expected to place valid ancillary data in a memory
                    buffer that is representative of the entire VBI region of
                    interest. However, only the valid ancillary data region
                    needs to be initialized -- the VPIF will automatically
                    transmit blanking data (Y=10h, C=80h) for non-valid
                    ancillary data regions. */
                    if ((Vpif_IoMode_DIS == crtChanHandle->channelMode) &&
                        (NULL != paramsDis->dispVVbiParams))
                    {
                        dispChNo = crtChanNum - Vpif_DISPLAY_NUM_CHANNELS;
                        gVpifRawSelVbiParam[dispChNo] = \
                            *(paramsDis->dispVVbiParams);
                        crtChanHandle->setSelVbiDispParams = \
                            &gVpifRawSelVbiParam[dispChNo];
                    }
                }
                if (Vpif_VbiServiceType_HBI ==
                    (Vpif_VbiServiceType_HBI & paramsCap->capVbiService))
                {
                    /* Calculate the size of the RAW HBI buffer */
                    crtChanHandle->hbiBufSz = \
                        crtChanHandle->chVidParams.rawHbiParams.samplePerLine *
                        (crtChanHandle->chVidParams.rawHbiParams.countFld0 +
                        crtChanHandle->chVidParams.rawHbiParams.countFld1);
                    /* Buffer size is not calculated for display based on
                    hVbiParams bec of following note in spruee. Note that the
                    user is expected to place valid ancillary data in a memory
                    buffer that is representative of the entire VBI region of
                    interest. However, only the valid ancillary data region
                    needs to be initialized -- the VPIF will automatically
                    transmit blanking data (Y=10h, C=80h) for non-valid
                    ancillary data regions. */
                    if ((Vpif_IoMode_DIS == crtChanHandle->channelMode) &&
                        (NULL != paramsDis->dispHVbiParams))
                    {
                        dispChNo = crtChanNum - Vpif_DISPLAY_NUM_CHANNELS;
                        gVpifRawSelHbiParam[dispChNo] = \
                            *(paramsDis->dispHVbiParams);
                        crtChanHandle->setSelHbiDispParams = \
                            &gVpifRawSelHbiParam[dispChNo];
                    }
                }
                if (Vpif_ALL_SLICE_VBI_SERVICES ==
                    (Vpif_ALL_SLICE_VBI_SERVICES |
                    paramsCap->capVbiSliceService))
                {
                    /* Save the type of slice service*/
                    crtChanHandle->sliceServType = \
                        paramsCap->capVbiSliceService;
                    for (index = 0; index < Vpif_SLICED_MAX_SERVICES; index++)
                    {
                        if (0 !=
                            (paramsCap->capVbiSliceService & (0x1 << index)))
                        {
                            /* Calculate no of slice services */
                            crtChanHandle->numSliceServices++;
                        }
                    }
                }
            }
            else
            {
                crtChanRtnVal = IOM_EBADARGS;
            }
        }
    }

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        /* Check if width is 8 byte aligned or not                            */
        crtChanHandle->hpitch = (crtChanHandle->chVidParams.width + 7u) & (~7u);

        crtChanHandle->bufSz = (Uint32)((crtChanHandle->hpitch) *
                               (crtChanHandle->chVidParams.height) *
                               2u);/* MUL by 2 bec of chroma and luma */

        if (crtChanHandle->channelMode == Vpif_IoMode_RAW_CAP)
        {
            /* As RAW data does not have Croma and Luma, so no MUL by *2* */
            crtChanHandle->bufSz = (Uint32)((crtChanHandle->hpitch) *
                                   (crtChanHandle->chVidParams.height));
            /*
             * MUL by *2* bec of 10 or 12 bit per pixel
             * 8  bit/pixel mode Pixel 0[7:0]             => 1 byte for 1 pixel
             * 10 bit/pixel mode “000000” & pixel 0[9:0]  => 2 byte for 1 pixel
             * 12 bit/pixel mode “0000” & pixel 0[11:0]   => 2 byte for 1 pixel
             */
            if (crtChanHandle->dataSz != Vpif_RawCaptureDataWidth_8BITS)
            {
                crtChanHandle->bufSz = (Uint32)((crtChanHandle->hpitch) *
                                       (crtChanHandle->chVidParams.height) *
                                       (2u));
            }
        }

        if (Vpif_FrameFormat_PROGRESSIVE == crtChanHandle->chVidParams.frameFmt)
        {
            /* Override whatever is set by the app. as only frame based storage
            is supported. */
            crtChanHandle->frameStorageMode = Vpif_SdramStorage_FRAME;
            crtChanHandle->lineOffset = crtChanHandle->chVidParams.width;
            if ((crtChanHandle->channelMode == Vpif_IoMode_RAW_CAP) &&
                (crtChanHandle->dataSz != Vpif_RawCaptureDataWidth_8BITS))
            {
                crtChanHandle->lineOffset = \
                    crtChanHandle->chVidParams.width * 2u;
            }
        }
        else
        {
            crtChanHandle->frameStorageMode = paramsCap->capStorageMode;
            if (Vpif_SdramStorage_FIELD == crtChanHandle->frameStorageMode)
            {
                /* Field based storage: The line pitch is just the line size  */
                crtChanHandle->lineOffset = crtChanHandle->chVidParams.width;
            }
            else
            {
                /*
                 * Frame based storage: Merge the two fields together.
                 * Line pitch is twice the line size
                 */
                crtChanHandle->lineOffset = \
                    crtChanHandle->chVidParams.width * 2u;
            }
        }

        if (Vpif_YCMuxed_NO == crtChanHandle->chVidParams.ycMuxMode)
        {
            /*
             * This will use 2 channels simultaneously. This create channel call
             * should not come on channel 1 and 3. Return error in that case.
             * Also check if ch 1 or 3 are already engaged
             */
            if ((Vpif_CHANNEL1_VIDEO == crtChanNum) ||
                (Vpif_CHANNEL3_VIDEO == crtChanNum) ||
                (Vpif_DriverState_OPENED ==
                 ((crtChanInstHandle->chanObj[crtChanNum + 1].chState) &
                 Vpif_DriverState_OPENED)))
            {
                crtChanRtnVal = IOM_EBADMODE;
            }
        }
    }

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        sliceServices = crtChanHandle->numSliceServices;
        if (0 != sliceServices)
        {
            if ((FALSE == crtChanHandle->chVidParams.isVbiSupported) ||
                (NULL == crtChanHandle->edcFxns))
                {
                    crtChanRtnVal = IOM_EBADARGS;
                }
        }
    }

    if (IOM_COMPLETED == crtChanRtnVal)
    {
        if (crtChanHandle->edcFxns != NULL)
        {
            if (0 != sliceServices)
            {
                edcParams.cbData = crtChanHandle;
                edcParams.cbFxn = &vpifSliceCb;
                /* open external device */
                crtChanHandle->edcHandle = crtChanHandle->edcFxns->open(name,
                    &edcParams);
                if (NULL == crtChanHandle->edcHandle)
                {
                    crtChanRtnVal = IOM_EBADIO;
                }
            }
            else
            {
                /* open external device */
                crtChanHandle->edcHandle = crtChanHandle->edcFxns->open(name,
                    NULL);
                if (NULL == crtChanHandle->edcHandle)
                {
                    crtChanRtnVal = IOM_EBADIO;
                }
            }
        }
        else
        {
            /* Channel does not have EDC */
        }
    }

    if ((IOM_COMPLETED == crtChanRtnVal) && (0 != sliceServices))
    {
        if (Vpif_IoMode_CAP == crtChanHandle->channelMode)
        {
            /* Create Capture VBI task */
            tskAttrs.priority = Vpif_VBI_TASK_PRIO;
            tskAttrs.stack = NULL;
            tskAttrs.stacksize = Vpif_VBI_TASK_STACK_SIZE_BYTE;
            tskAttrs.stackseg = 0;
            tskAttrs.environ = NULL;
            tskAttrs.name = "Vpif_Vbi_Cap_Task";
            tskAttrs.initstackflag = 1U;
            tskAttrs.exitflag = 1U;

            crtChanHandle->taskHandle = TSK_create((Fxn)&vpifSliceVbiCaptureTask,
                &tskAttrs,
                (Arg)crtChanHandle);

            crtChanHandle->taskRun = TRUE;
            semAttrs.name = (String)"Vpif_Sync_Sema";
            crtChanHandle->syncSem = SEM_create(0U, &semAttrs);
            if ((NULL == crtChanHandle->syncSem) ||
                (NULL == crtChanHandle->taskHandle))
            {
                crtChanRtnVal = IOM_EALLOC;
            }
        }
        if (Vpif_IoMode_DIS == crtChanHandle->channelMode)
        {
            /* Create Capture VBI task */
            tskAttrs.priority = Vpif_VBI_TASK_PRIO;
            tskAttrs.stack = NULL;
            tskAttrs.stacksize = Vpif_VBI_TASK_STACK_SIZE_BYTE;
            tskAttrs.stackseg = 0;
            tskAttrs.environ = NULL;
            tskAttrs.name = "Vpif_Vbi_Disp_Task";
            tskAttrs.initstackflag = 1U;
            tskAttrs.exitflag = 1U;

            crtChanHandle->taskHandle = TSK_create((Fxn)&vpifSliceVbiDisplayTask,
                &tskAttrs,
                (Arg)crtChanHandle);

            crtChanHandle->taskRun = TRUE;
            semAttrs.name = (String)"Vpif_Sync_Sema";
            crtChanHandle->syncSem = SEM_create(0U, &semAttrs);
            if ((NULL == crtChanHandle->syncSem) ||
                (NULL == crtChanHandle->taskHandle))
            {
                crtChanRtnVal = IOM_EALLOC;
            }
        }
        if (IOM_COMPLETED == crtChanRtnVal)
        {
            crtChanHandle->vbiTskletHandle = NULL;

            /* Creating Tasklet(SWI)to schedule VBI cause. Because VBI needs the
             * data to be read/write from the I2C. This delay in Vpif         *
             * HWI handler context shall cause interrupt latencies for other  *
             * HWIs which are having less priority than Vpif HWI. Thus        *
             * scheduling this TX processing in Tasklet(SWI) context for      *
             * better system performance.                                     */
            swiAttrs = SWI_ATTRS;

            /* modify the default parameters with the required params         */
            swiAttrs.priority = Vpif_TASKLET_PRIORITY;
            swiAttrs.arg0     = (Arg)crtChanHandle;
            swiAttrs.fxn      = &vpifSliceVbiSwiHandler;

            /* create the swi that will be used for buffering TX data         */
            crtChanHandle->vbiTskletHandle = (Ptr)SWI_create(&swiAttrs);
            if (NULL == crtChanHandle->vbiTskletHandle)
            {
               crtChanRtnVal = IOM_EBADARGS;
            }
        }
    }

    if (IOM_COMPLETED != crtChanRtnVal)
    {
        /* failed, free all resources and return error */
        if (NULL != chanp)
        {
            *chanp = NULL;
        }
    }
    else
    {
        if (Vpif_YCMuxed_NO == crtChanHandle->chVidParams.ycMuxMode)
        {
            /*
             * Since two channels  will be used simultaneously, so both 0 and
             * 1 or 2 and 3 will be blocked.
             */
            crtChanInstHandle->chanObj[crtChanNum + 1].chState |= \
                Vpif_DriverState_OPENED;
        }
        /* Initialize channel state as Opened */
        crtChanHandle->chState |= Vpif_DriverState_OPENED;
        /* configure the channel */
        *chanp = (Ptr)crtChanHandle;
    }
    VPIF_DEBUG("Exited ---> vpifMdCreateChan\n");

    return crtChanRtnVal;
}


/**
 *  \brief    This function is called by the application to close a previously
 *            opened channel. It deletes the channel so that it is not available
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
static Int32 vpifMdDeleteChan(Ptr chanp)
{
    Int32         delChanRtnVal = IOM_COMPLETED;
    Vpif_ChanObj  *delChanHandle = NULL;
    Vpif_Object   *delInstHandle = NULL;
    Uint8         delDispChNo;

    VPIF_DEBUG("Entered ---> vpifMdDeleteChan\n");

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
        delChanRtnVal = IOM_EBADARGS;
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    delChanHandle = (Vpif_ChanObj *)chanp;
    if (IOM_COMPLETED == delChanRtnVal)
    {
        delInstHandle = delChanHandle->devHandle;
    }

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (IOM_COMPLETED == delChanRtnVal)
    {
        if ((Vpif_DriverState_OPENED != delChanHandle->chState) ||
            (Vpif_DriverState_STARTED ==
                (delChanHandle->chState & Vpif_DriverState_STARTED)))
        {
            /* Application need to stop the channel first. If channel is
               stopped here memory will not be free. */
            delChanRtnVal = IOM_EBADMODE;
        }
        if (NULL == delInstHandle)
        {
            delChanRtnVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == delChanRtnVal)
    {
        if (Vpif_DriverState_OPENED ==
            (delChanHandle->chState & Vpif_DriverState_OPENED))
        {
            /* close external device */
            if (delChanHandle->edcFxns != NULL)
            {
                if (IOM_COMPLETED != delChanHandle->edcFxns->close(
                                      delChanHandle->edcHandle))
                {
                    delChanRtnVal = IOM_EBADIO;
                }
            }
        }
    }
    if (IOM_COMPLETED == delChanRtnVal)
    {
        /* Deleting Tasklet and task if VBI is supported                      */
        if (0 != delChanHandle->numSliceServices)
        {
            if (NULL != delChanHandle->vbiTskletHandle)
            {
                SWI_delete(delChanHandle->vbiTskletHandle);
            }
            if (Vpif_IoMode_CAP == delChanHandle->channelMode)
            {
                delChanHandle->taskRun = FALSE;
                TSK_delete(delChanHandle->taskHandle);
                SEM_delete(delChanHandle->syncSem);
            }
        }

        if (Vpif_IoMode_DIS == delChanHandle->channelMode)
        {
            delDispChNo = delChanHandle->chanNum - Vpif_DISPLAY_NUM_CHANNELS;
            memset(&gVpifRawSelVbiParam[delDispChNo],
                0,
                sizeof(Vpif_RawSelectiveVbiParams));
            memset(&gVpifRawSelHbiParam[delDispChNo],
                0,
                sizeof(Vpif_RawSelectiveVbiParams));
            if ((TRUE == QUE_empty(&(delChanHandle->qIn))) &&
                (TRUE == QUE_empty(&(delChanHandle->qOut))))
            {
                /* Delete the two queue for display channel                   */
                QUE_delete(&(delChanHandle->qOut));
                QUE_delete(&(delChanHandle->qIn));
            }
            else
            {
                /* Some IOPs are pending                                      */
                delChanRtnVal = IOM_EBADMODE;
            }
        }
        else
        {
            if ((TRUE == QUE_empty(&(delChanHandle->qIn))))
            {
                /* Delete the queue for capture channel                       */
                QUE_delete(&(delChanHandle->qIn));
            }
            else
            {
                /* Some IOPs are pending                                      */
                delChanRtnVal = IOM_EBADMODE;
            }
        }
    }
    if (IOM_COMPLETED == delChanRtnVal)
    {
        /* If the driver is power aware enable the PSC */
        if (TRUE == delInstHandle->isPwrAware)
        {
            /*Enable LPSC of VPIF */
            delChanRtnVal = vpifLpscOn(delInstHandle);
        }
        if (IOM_COMPLETED == delChanRtnVal)
        {
            /* Disable channel as per its device type and channel id */
            vpifEnableChannel(delChanHandle, FALSE);
            vpifChannelIntrEnable(delChanHandle, FALSE);
            /* If the driver is power aware disable the PSC */
            if (TRUE == delInstHandle->isPwrAware)
            {
                /*Disable LPSC of VPIF */
                delChanRtnVal = vpifLpscOff(delInstHandle);
            }
        }
    }

    if (IOM_COMPLETED == delChanRtnVal)
    {

        delChanHandle->chError = 0;
        delChanHandle->frameInt = 0;
        delChanHandle->cbFxn = NULL;
        delChanHandle->cbArg = NULL;
        delChanHandle->edcFxns = NULL;
        delChanHandle->edcHandle = NULL;
        delChanHandle->segId = -1;
        delChanHandle->bufSz = 0;
        delChanHandle->hpitch = 0;
        delChanHandle->lineOffset = 0;
        delChanHandle->alignment = 0;
        delChanHandle->started = 0;
        delChanHandle->useTwoChannels = 0;
        delChanHandle->fieldId = 0;
        delChanHandle->queEmpty = 0;
        delChanHandle->curViop = NULL;
        delChanHandle->nextViop = NULL;
        delChanHandle->mrViop = NULL;
        delChanHandle->packetIOM = NULL;
        delChanHandle->numQueBuf = 0;
        delChanHandle->hbiBufSz = 0;
        delChanHandle->vbiBufSz = 0;
        delChanHandle->vbiTskletHandle = NULL;
        delChanHandle->sliceServType = 0;
        delChanHandle->numSliceServices = 0;
        delChanHandle->setSelVbiDispParams = NULL;
        delChanHandle->setSelHbiDispParams = NULL;

        /* Updates the channel port Void                                      */
        delChanHandle->devHandle = NULL;
        delChanHandle->channelMode = Vpif_IoMode_NONE;
        if (Vpif_YCMuxed_NO == delChanHandle->chVidParams.ycMuxMode)
        {
            /*
             * Since two channels were used simultaneously, so both 0 and 1 or
             * 2 and 3 will be freed.
             */
            delInstHandle->chanObj[delChanHandle->chanNum + 1].chState = \
                Vpif_DriverState_CLOSED;
        }
        memset(&delChanHandle->chVidParams, 0x00, sizeof(Vpif_ConfigParams));
        /* Free this channel resource                                         */
        delChanHandle->chState = Vpif_DriverState_CLOSED;
    }
    VPIF_DEBUG("Exited ---> vpifMdDeleteChan\n");

    return (delChanRtnVal);
}


/**
 *  \brief Submit a I/O packet to a channel for processing
 *
 *   The FVID/GIO layer calls this function to cause the mini-driver
 *   to process the IOM_Packet for read/write operations. This function
 *   is called to Queue/Dequeue Frame buffer.
 *
 *  \param   chanp      [IN]  Handle to the channel
 *  \param   ioPacket   [IN]  Pointer to packet to be submitted. IOM packet
 *                            passed by the upper layer containing
 *                            information about the size,address,timeout and
 *                            other necessary parameters
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
static Int32 vpifMdSubmitChan(Ptr chanp, IOM_Packet *ioPacket)
{
    Vpif_ChanObj       *subChanHandle = NULL;
    Int32              subRtnVal = IOM_COMPLETED;
    Uint32             submitKey = 0;

    VPIF_DEBUG("Entered ---> vpifMdSubmitChan\n");

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    /* check the command and process it stream will take care that            *
     * proper command is passed hence no need to check it once again          */

    /* Check if the buffer pointer is not null                                */
    if ((NULL == chanp) || (NULL == ioPacket))
    {
         subRtnVal = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

        /* get the handle to the channel to which the request if posted       */
        subChanHandle = (Vpif_ChanObj *)chanp;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (Vpif_DriverState_OPENED !=
            (Vpif_DriverState_OPENED & subChanHandle->chState))
        {
            subRtnVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == subRtnVal)
    {
        submitKey = (Uint32)_disable_interrupts();
        switch (ioPacket->cmd)
        {
            case FVID_QUEUE:
                VPIF_DEBUG("FVID_QUEUE\n");
                /* function call to queue the buffer to driver                */
                subRtnVal = vpifQueueBuffer(subChanHandle, ioPacket);
                break;

            case FVID_DEQUEUE:
                VPIF_DEBUG("FVID_DEQUEUE\n");
                if (Vpif_DriverState_STARTED !=
                    (subChanHandle->chState & Vpif_DriverState_STARTED))
                {
                    /* If capture/display is stopped then return all buffs
                    one-by-one */
                    subRtnVal = vpifGetBufs(subChanHandle, ioPacket);
                }
                else
                {
                    if (Vpif_IoMode_DIS == subChanHandle->channelMode)
                    {
                        if (subChanHandle->packetIOM == NULL)
                        {
                            /*
                             * If display is started then return empty buffer to
                             * application. If buffer is not available, then
                             * return IOM_PENDING
                             */
                            subRtnVal = vpifDequeueBuffer(subChanHandle,
                                            ioPacket);
                        }
                        else
                        {
                            /* Already a request. return IOM_EINUSE if there is
                            any outstanding pending request */
                            subRtnVal = IOM_EINUSE;
                        }
                    }
                    else
                    {
                        if (subChanHandle->mrViop != NULL)
                        {
                            /* only when there is no outstanding pending request
                            */
                            if (NULL == subChanHandle->packetIOM)
                            {
                                /*
                                 * If capture is started then return captured
                                 * buffer to application. If buffer is not
                                 * available, return IOM_PENDING
                                 */
                                subRtnVal = vpifDequeueBuffer(subChanHandle,
                                                ioPacket);
                            }
                            else
                            {
                                subRtnVal = IOM_EINUSE;
                            }
                        }
                        else
                        {
                            /*
                             * if mrviop is NULL then return IOM_PENDING. This
                             * call will wait at GIO layer, when there will be
                             * new buffer available with driver, GIO layer
                             * callback will be called
                             */
                            subChanHandle->packetIOM = ioPacket;
                            subRtnVal = ioPacket->status = IOM_PENDING;
                        }
                    }
                }
                break;

            case FVID_EXCHANGE:
                VPIF_DEBUG("FVID_EXCHANGE\n");
                if (Vpif_IoMode_DIS == subChanHandle->channelMode)
                {
                    if (subChanHandle->packetIOM == NULL)
                    {
                        /* only when there is no outstanding pending request */
                        subRtnVal = vpifQueueBuffer(subChanHandle, ioPacket);
                        if (IOM_COMPLETED == subRtnVal)
                        {
                            subRtnVal = vpifDequeueBuffer(subChanHandle,
                                            ioPacket);
                        }
                    }
                    else
                    {
                        /* return IOM_EINUSE if there is any outstanding pending
                         * request
                         */
                        subRtnVal = IOM_EINUSE;
                    }
                }
                else
                {
                    if (Vpif_DriverState_STARTED !=
                        (subChanHandle->chState & Vpif_DriverState_STARTED))
                    {
                        subRtnVal = IOM_EABORT;
                    }
                    else if (subChanHandle->mrViop != NULL)
                    {
                        /* only when there is no outstanding pending request */
                        if (NULL == subChanHandle->packetIOM)
                        {
                            subRtnVal = vpifQueueBuffer(subChanHandle,
                                            ioPacket);
                            if (IOM_COMPLETED == subRtnVal)
                            {
                                subRtnVal = vpifDequeueBuffer(subChanHandle,
                                                ioPacket);
                            }
                        }
                        else
                        {
                            /* return IOM_EINUSE if there is any outstanding
                             * pending request*/
                            subRtnVal = IOM_EINUSE;
                        }
                    }
                    else
                    {
                        /* if mrviop is NULL then queue the buffer provided by
                         * application.
                         */
                        subRtnVal = vpifQueueBuffer(subChanHandle, ioPacket);
                        if (IOM_COMPLETED == subRtnVal)
                        {
                            /* If queue is successful then return IOM_PENDING.
                             * Returning IOM_PENDING will wait at GIO layer.
                             * When there is new buffer available with driver,
                             * GIO layer callback will be called
                             */
                            subChanHandle->packetIOM = ioPacket;
                            subRtnVal = ioPacket->status = IOM_PENDING;
                        }
                    }
                }
                break;

            default:
                subRtnVal = IOM_ENOTIMPL;
                break;
        }
        _restore_interrupts(submitKey);
        /* request has been posted for processing now depending on the mode of
         * operation either wait for processing to complete or the callback
         * function to be called                                              */
    }
    VPIF_DEBUG("Exited ---> vpifMdSubmitChan\n");

    return subRtnVal;
}


/**
 *  \brief   This function executes a control command passed by the application
 *
 *   The application's request for a IOCTL to be executed is routed here by the
 *   stream. if the command is supported then the control command is executed.
 *
 *  \param    chanp      [IN]  Channel handle
 *  \param    cmd        [IN]  Control command given by the application
 *  \param    cmdArg     [IN]  Optional argument required for command execution
 *
 *  \return   None
 *
 *  \enter    chanp      must be a valid pointer and should not be null.
 *            cmd        must be a valid IOCTL command.
 *            cmdArg     must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
static Int32 vpifMdControlChan(Ptr chanp, Uns cmd, Ptr cmdArg)
{
    Vpif_ChanObj       *ctrlChanHandle = NULL;
    Int32               ctrlRtnVal     = IOM_COMPLETED;

    VPIF_DEBUG("Entered ---> vpifMdControlChan\n");

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
    if (NULL == chanp)
    {
         ctrlRtnVal = IOM_EBADARGS;
    }
    else
    {
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

        /* get the handle to the channel to send the ICOTL to                 */
        ctrlChanHandle = (Vpif_ChanObj*)chanp;

/* Begin parameter checking                                                   */
#ifndef PSP_DISABLE_INPUT_PARAMETER_CHECK
        if (Vpif_DriverState_OPENED !=
            (ctrlChanHandle->chState & Vpif_DriverState_OPENED))
        {
            ctrlRtnVal = IOM_EBADARGS;
        }
    }
#endif  /* PSP_DISABLE_INPUT_PARAMETER_CHECK */
/* End parameter checking                                                     */

    if (IOM_COMPLETED == ctrlRtnVal)
    {
        if (Vpif_IOCTL_CMD_START == cmd)
        {
            VPIF_DEBUG("Vpif_IOCTL_CMD_START\n");
            if ((Vpif_DriverState_READY !=
                (Vpif_DriverState_READY & ctrlChanHandle->chState)) ||
                (Vpif_DriverState_STARTED ==
                 (ctrlChanHandle->chState & Vpif_DriverState_STARTED)))
            {
                ctrlRtnVal = IOM_EBADIO;
            }
            else
            {
                ctrlRtnVal = vpifStartTransfer(chanp);
            }
        }
        /* Stop the channel */
        else if (Vpif_IOCTL_CMD_STOP == cmd)
        {
            VPIF_DEBUG("Vpif_IOCTL_CMD_STOP\n");
            if (Vpif_DriverState_STARTED !=
                (ctrlChanHandle->chState & Vpif_DriverState_STARTED))
            {
                ctrlRtnVal = IOM_EBADIO;
            }
            else
            {
                ctrlRtnVal = vpifStopTransfer(ctrlChanHandle);
            }
        }
        else if (FVID_ALLOC_BUFFER == cmd)
        {
            VPIF_DEBUG("FVID_ALLOC_BUFFER\n");
            if (NULL == cmdArg)
            {
                ctrlRtnVal = IOM_EBADARGS;
            }
            else
            {
                /* function call for allocating buffer */
                ctrlRtnVal = vpifAllocBuffer(chanp, cmdArg);
            }
        }
        else if (FVID_FREE_BUFFER == cmd)
        {
            VPIF_DEBUG("FVID_FREE_BUFFER\n");
            if (NULL == cmdArg)
            {
                ctrlRtnVal = IOM_EBADARGS;
            }
            else
            {
                /* function call to free the allocated buffer */
                ctrlRtnVal = vpifFreeBuffer(chanp, cmdArg);
            }
        }
        else if (Vpif_IOCTL_CMD_GET_NUM_IORQST_PENDING == cmd)
        {
            VPIF_DEBUG("Vpif_IOCTL_CMD_GET_NUM_IORQST_PENDING\n");
            if (NULL == cmdArg)
            {
                ctrlRtnVal = IOM_EBADARGS;
            }
            else if (Vpif_DriverState_READY !=
                     (Vpif_DriverState_READY & ctrlChanHandle->chState))
            {
                ctrlRtnVal = IOM_EBADIO;
            }
            else
            {
                ctrlRtnVal = vpifGetNumPendingIoReq(chanp, cmdArg);
            }
        }
        else if (Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO == cmd)
        {
            /* copy the statistics for the channel                            */
            VPIF_DEBUG("Vpif_IOCTL_CMD_GET_PARAMS\n");
            if (NULL == cmdArg)
            {
                ctrlRtnVal = IOM_EBADARGS;
            }
            else
            {
                ctrlRtnVal = vpifGetChanParams(chanp, (Vpif_StdInfo *)cmdArg);
            }
        }
        else if (Vpif_IOCTL_CMD_CHANGE_RESOLUTION == cmd)
        {
            VPIF_DEBUG("Vpif_IOCTL_CMD_CHANGE_RESOLUTION\n");
            /*
             * The channel should be stopped and all its buffers released
             * then only user can change resolution
             */
            if ((NULL == cmdArg) ||
               (Vpif_DriverState_OPENED != ctrlChanHandle->chState))
            {
                ctrlRtnVal = IOM_EBADARGS;
            }
            else
            {
                ctrlRtnVal = vpifSetResolution(chanp,
                                 (Vpif_ConfigParams *)cmdArg);
            }
        }
        else
        {
            VPIF_DEBUG("VPIF default IOCTL\n");
            if (ctrlChanHandle->edcFxns != NULL)
            {
                /* Call EDC control function for EDC configuration */
                ctrlRtnVal = ctrlChanHandle->edcFxns->ctrl(
                                 ctrlChanHandle->edcHandle,
                                 cmd - Vpif_IOCTL_CMD_MAX,
                                 cmdArg);
            }
            else
            {
                /* unrecognized IOCTL is passed, hence status not implemented */
                ctrlRtnVal = IOM_ENOTIMPL;
            }
        }
    }
    VPIF_DEBUG("Exited ---> vpifMdControlChan\n");

    return (ctrlRtnVal);
}

/* ========================================================================== */
/*                            LOCAL  FUNCTIONS                                */
/* ========================================================================== */

/*============================================================================*/
/*                             PWRM FUNCTIONS                                 */
/*============================================================================*/
/**
 *  \brief    vpifLpscOn
 *
 *   This function is invoked to power on the peripheral. Note that this
 *   function uses different functions to power ON the module, depending on
 *   the PWRM is enabled in this driver or NOT
 *
 *  \param    instHandle    [IN]    handle to the instance
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  instHandle    must be a valid instance object
 *          The driver/channel should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 vpifLpscOn(Vpif_Object *instHandle)
{
    Int32 lpscOnStatus;
    assert(NULL != instHandle);

    #ifdef BIOS_PWRM_ENABLE
        /* power on using PWRM API */
        lpscOnStatus = PWRM_setDependency(
                           (PWRM_Resource)instHandle->deviceInfo.pwrmLpscID);
    #else
        /* power on using PSC API */
        lpscOnStatus = Psc_ModuleClkCtrl(
                           (Psc_DevId)instHandle->deviceInfo.pscInstance,
                           (Uint32)instHandle->deviceInfo.pwrmLpscID,
                           TRUE);
    #endif
    return lpscOnStatus;
}

/**
 *  \brief    vpifLpscOff
 *
 *   This function is invoked to power off the peripheral
 *   handler and interrupt cause respectively.
 *
 *  \param    instHandle    [IN]    channel handle
 *
 *  \return   IOM_COMPLETED in case of success else an error code is returned.
 *
 *  \enter  instHandle    must be a valid instance object
 *          The driver/channel should be in opened state.
 *
 *  \leave  Not Implemented.
 */
static Int32 vpifLpscOff(Vpif_Object *instHandle)
{
    Int32 lpscOffStatus;
    assert(NULL != instHandle);

    #ifdef BIOS_PWRM_ENABLE
        /* power off  using PWRM API */
        lpscOffStatus = PWRM_releaseDependency(
                            (PWRM_Resource)instHandle->deviceInfo.pwrmLpscID);
    #else
         /* power off using PSC API */
        lpscOffStatus = Psc_ModuleClkCtrl(
                            (Psc_DevId)instHandle->deviceInfo.pscInstance,
                            (Uint32)instHandle->deviceInfo.pwrmLpscID,
                            FALSE);
    #endif
    return lpscOffStatus;
}


#ifdef BIOS_PWRM_ENABLE
/**
 * \brief     This function registers with the BIOS PWRM module for all the
 *            possible power management events.
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
static Int32 vpifRegisterNotification(Vpif_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    /* register for the events to be notified by the driver                   */
    for (eventCnt = 0; eventCnt < Vpif_MAX_PWRM_EVENTS; eventCnt++)
    {
        /* register the notify function for the PWRM event                    */
        retVal = PWRM_registerNotify(
                    (PWRM_Event)eventCnt,
                    NULL,
                    vpifNotifyCallback,
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
static Int32 vpifUnregisterNotification(Vpif_Object *instHandle)
{
    Uint32       eventCnt = 0x00;
    PWRM_Status  retVal   = PWRM_SOK;
    Int32        status   = IOM_COMPLETED;

    assert(NULL != instHandle);

    for (eventCnt = 0; eventCnt < Vpif_MAX_PWRM_EVENTS; eventCnt++)
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
 * \return    IOM_COMPLETED if success else error code
 *
 * \enter     eventType is a valid event
 *
 * \leave     Not implemented
 */
static PWRM_NotifyResponse vpifNotifyCallback(PWRM_Event    eventType,
                                              Arg           eventArg1,
                                              Arg           eventArg2,
                                              Arg           clientArg)
{
    Vpif_Object          *cbInstHandle = NULL;
    PWRM_NotifyResponse  pwrmStatus = PWRM_NOTIFYDONE;
    Uint8                j;

    assert(NULL != clientArg);
    cbInstHandle = (Vpif_Object *)clientArg;
    assert(NULL != cbInstHandle);

    eventArg1 = eventArg1; /* To remove compilation warning */
    eventArg2 = eventArg2; /* To remove compilation warning */

    /* check the event notified by the PWRM module and process accordingly    */
    switch (eventType)
    {
        case PWRM_GOINGTOSLEEP:
        case PWRM_GOINGTOSTANDBY:
        case PWRM_GOINGTODEEPSLEEP:
        /* The vpif driver shall support PWRM events only when channel is   *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
            {
                if (Vpif_DriverState_STARTED ==
                    (cbInstHandle->chanObj[j].chState & Vpif_DriverState_STARTED))
                {
                    pwrmStatus = PWRM_NOTIFYERROR;
                    break;
                }
            }
            break;

        case PWRM_PENDING_CPU_SETPOINTCHANGE:
        case PWRM_PENDING_PER_SETPOINTCHANGE:
        /* The vpif driver shall support PWRM events only when channel is   *
         * disabled and no packets are pending in driver.                   *
         * Hence, there nothing much to be done here but to check submit    *
         * count and then returning the status                              */
            if (((PWRM_PENDING_CPU_SETPOINTCHANGE == eventType) &&
                 (Vpif_PllDomain_0 == cbInstHandle->pwrmInfo.pllDomain)) ||
                ((PWRM_PENDING_PER_SETPOINTCHANGE == eventType) &&
                 (Vpif_PllDomain_1 == cbInstHandle->pwrmInfo.pllDomain)))
            {
                for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
                {
                    if (Vpif_DriverState_STARTED ==
                        (cbInstHandle->chanObj[j].chState & Vpif_DriverState_STARTED))
                    {
                        pwrmStatus = PWRM_NOTIFYERROR;
                        break;
                    }
                }
            }
            break;

        case PWRM_DONE_CPU_SETPOINTCHANGE:
        case PWRM_DONE_PER_SETPOINTCHANGE:
             /* The driver shall now reconfigure the clock settings based     *
              * on the new set point of the system                            */
            break;

        case PWRM_AWAKEFROMSLEEP:
        case PWRM_AWAKEFROMDEEPSLEEP:
        case PWRM_AWAKEFROMSTANDBY:
            /* There is nothing for the vpif driver to be done here. Just   *
             * check the channel status. The vpif will be enabled, during   *
             * first IOP submit                                             */
            for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
            {
                if (Vpif_DriverState_STARTED ==
                    (cbInstHandle->chanObj[j].chState & Vpif_DriverState_STARTED))
                {
                    pwrmStatus = PWRM_NOTIFYERROR;
                    break;
                }
            }
            break;

        default:
            break;
    }
    return (pwrmStatus);
}

#endif

/*============================================================================*/
/*                             PWRM FUNCTIONS END                             */
/*============================================================================*/


/**
 *  \brief  The entry function for Slice VBI capture task
 *
 *  \param  arg       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifSliceVbiCaptureTask(Arg arg)
{
    Vpif_ChanObj *tskChanHandle = NULL;
    Bool taskFlag = FALSE;

    assert(NULL != arg);
    tskChanHandle = (Vpif_ChanObj *)ArgToPtr(arg);
    assert(NULL != tskChanHandle);

    taskFlag = TRUE;
    while (taskFlag)
    {
        /* take sync sema which will be released only by vpif isr for vbi capture*/
        if (TRUE != SEM_pend(tskChanHandle->syncSem, SYS_FOREVER))
        {
            taskFlag = FALSE;
        }
        else
        {
            if (Vpif_DriverState_STARTED ==
                (tskChanHandle->chState & Vpif_DriverState_STARTED))
            {
                /* start reading the vbi data */
                /* If this is even/top field */
                if (0 == tskChanHandle->fieldId)
                {
                    /* Call EDC control function for reading Slice VBI data */
                    tskChanHandle->sliceVbiFld0I2cSubmitStatus = \
                        tskChanHandle->edcFxns->ctrl(tskChanHandle->edcHandle,
                        Edc_IOCTL_READ_SLICE_VBI_DATA,
                        (FVID_SliceFrame *)tskChanHandle->curViop->vbiFrm.s1);
                    if (IOM_COMPLETED != tskChanHandle->sliceVbiFld0I2cSubmitStatus)
                    {
                        vpifProcessErrFrm(tskChanHandle);
                    }
                    vpifProcessCurrentCapFrm(tskChanHandle);
                }
                else/* odd/bottom field */
                {
                    /* Call EDC control function for reading Slice VBI data */
                    tskChanHandle->sliceVbiFld1I2cSubmitStatus = \
                        tskChanHandle->edcFxns->ctrl(tskChanHandle->edcHandle,
                        Edc_IOCTL_READ_SLICE_VBI_DATA,
                        (FVID_SliceFrame *)tskChanHandle->curViop->vbiFrm.s2);
                    if (IOM_COMPLETED != tskChanHandle->sliceVbiFld1I2cSubmitStatus)
                    {
                        vpifProcessErrFrm(tskChanHandle);
                    }
                    vpifConfigureNextCapFrm(tskChanHandle);
                }/* else odd mode */
            }
            else
            {
                if (tskChanHandle->taskRun == FALSE)
                {
                    taskFlag = FALSE;
                }
            }
        }
    }
}

static Void vpifProcessErrFrm(Vpif_ChanObj *chanHandle)
{
    volatile CSL_VpifRegsOvly vpifRegs;
    Uint32 chIntStatClrVal = 0;
    Uint32 chIntClrVal = 0;
    Uint32 chIntEnVal = 0;
	Uint32 i;

    /* Disable clock and channel */
    assert(NULL != chanHandle);
    vpifRegs = chanHandle->baseAddress;
    assert(NULL != vpifRegs);
    for (i = 0; i < 4; i++)
    {
        switch (i)
        {
            case Vpif_CHANNEL0_VIDEO:
                chanHandle->baseAddress->C0CTRL &= \
                    ~CSL_VPIF_C0CTRL_CHANEN_ENABLE;
                chIntEnVal = CSL_VPIF_INTEN_FRAME0_ENABLE;
                chIntClrVal = CSL_VPIF_INTCLR_FRAME0_CLR;
                chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME0_CLR;
                break;
            case Vpif_CHANNEL1_VIDEO:
                chanHandle->baseAddress->C1CTRL &= \
                    ~CSL_VPIF_C1CTRL_CHANEN_ENABLE;
                chIntEnVal = CSL_VPIF_INTEN_FRAME1_ENABLE;
                chIntClrVal = CSL_VPIF_INTCLR_FRAME1_CLR;
                chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME1_CLR;
                break;
            case Vpif_CHANNEL2_VIDEO:
                chanHandle->baseAddress->C2CTRL &= \
                    ~(CSL_VPIF_C2CTRL_CLKEN_ENABLE <<
                      CSL_VPIF_C2CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C2CTRL &= \
                    ~CSL_VPIF_C2CTRL_CHANEN_ENABLE;
                chIntEnVal = CSL_VPIF_INTEN_FRAME2_ENABLE;
                chIntClrVal = CSL_VPIF_INTCLR_FRAME2_CLR;
                chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME2_CLR;
                break;
            case Vpif_CHANNEL3_VIDEO:
                chanHandle->baseAddress->C3CTRL &= \
                    ~(CSL_VPIF_C3CTRL_CLKEN_ENABLE <<
                      CSL_VPIF_C3CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C3CTRL &= \
                    ~CSL_VPIF_C3CTRL_CHANEN_ENABLE;
                chIntEnVal = CSL_VPIF_INTEN_FRAME3_ENABLE;
                chIntClrVal = CSL_VPIF_INTCLR_FRAME3_CLR;
                chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME3_CLR;
                break;
            default:
                /* print error */
                break;
        }

        /* disable error interrupt - Clear ERROR bit in INTSTAT. */
        vpifRegs->INTSTATCLR |= (CSL_VPIF_INTSTATCLR_ERROR_CLR <<
            CSL_VPIF_INTSTATCLR_ERROR_SHIFT);
        /* Interrupt on ERROR is masked */
        vpifRegs->INTCLR |= (CSL_VPIF_INTCLR_ERROR_CLR <<
            CSL_VPIF_INTCLR_ERROR_SHIFT);
        /* Error interrupt is disabled. */
        vpifRegs->INTEN &= ~(CSL_VPIF_INTEN_ERROR_ENABLE <<
            CSL_VPIF_INTEN_ERROR_SHIFT);

        /* disable corresponding channel interrupt                            */
        /* Clear CHANNELx bit in INTSTAT.                                     */
        vpifRegs->INTSTATCLR |= (chIntStatClrVal << i);
        /* Interrupt on CHANNELx is masked                                    */
        vpifRegs->INTCLR |= (chIntClrVal << i);
        /* CHANNELx frame interrupt is disabled.                              */
        vpifRegs->INTEN &= (~(chIntEnVal << i));
    }


    /* Some problem with I2C write to the codec for VBI */
    chanHandle->currPktStatus = Vpif_VBI_I2C_ERR;
}

static Void vpifSliceVbiDisplayTask(Arg arg)
{
    Vpif_ChanObj *tskChanHandle = NULL;
    Bool taskFlag = FALSE;

    assert(NULL != arg);
    tskChanHandle = (Vpif_ChanObj *)ArgToPtr(arg);
    assert(NULL != tskChanHandle);

    taskFlag = TRUE;
    while (taskFlag)
    {
        /* take sync sema which will be released only by vpif isr for vbi capture*/
        if (TRUE != SEM_pend(tskChanHandle->syncSem, SYS_FOREVER))
        {
            taskFlag = FALSE;
        }
        else
        {
            if (Vpif_DriverState_STARTED ==
                (tskChanHandle->chState & Vpif_DriverState_STARTED))
            {
                /* start reading the vbi data */
                /* If this is even/top field */
                if (0 == tskChanHandle->fieldId)
                {

                    tskChanHandle->sliceVbiFld0I2cSubmitStatus
                            = tskChanHandle->edcFxns->ctrl(
                                tskChanHandle->edcHandle,
                                Edc_IOCTL_WRITE_SLICE_VBI_DATA,
                                tskChanHandle->curViop->vbiFrm.s1);
                    if ((IOM_PENDING != tskChanHandle->sliceVbiFld0I2cSubmitStatus) &&
                        (IOM_COMPLETED != tskChanHandle->sliceVbiFld0I2cSubmitStatus))
                    {
                        vpifProcessErrFrm(tskChanHandle);
                        SWI_post(tskChanHandle->vbiTskletHandle);
                    }
                }
                else/* odd/bottom field */
                {
                    tskChanHandle->sliceVbiFld1I2cSubmitStatus
                            = tskChanHandle->edcFxns->ctrl(
                                tskChanHandle->edcHandle,
                                Edc_IOCTL_WRITE_SLICE_VBI_DATA,
                                tskChanHandle->curViop->vbiFrm.s2);
                    if ((IOM_PENDING != tskChanHandle->sliceVbiFld1I2cSubmitStatus) &&
                        (IOM_COMPLETED != tskChanHandle->sliceVbiFld1I2cSubmitStatus))
                    {
                        vpifProcessErrFrm(tskChanHandle);
                        SWI_post(tskChanHandle->vbiTskletHandle);
                    }
                }/* else odd mode */
            }
            else
            {
                if (tskChanHandle->taskRun == FALSE)
                {
                    taskFlag = FALSE;
                }
            }
        }
    }
}


/**
 *  \brief  The entry function for Slice VBI Swi Handler
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
static Void vpifSliceVbiSwiHandler(Arg arg0, Arg unUsedArg)
{
    Vpif_ChanObj *chanHandle = NULL;

    assert(NULL != arg0);
    chanHandle = (Vpif_ChanObj *)ArgToPtr(arg0);
    unUsedArg = unUsedArg;      /* to remove compilation warning */
    assert(NULL != chanHandle);

    /* Inside SWI context. Disable VPIF HWI in order to avoid          *
     * multiple SWI posting from VPIF ISR.                                    */
    ECM_disableEvent(
        ((Vpif_Object *)(chanHandle->devHandle))->deviceInfo.cpuEventNumber);

    if (Vpif_IoMode_DIS == chanHandle->channelMode)
    {
        if (0 == chanHandle->fieldId)
        {
            vpifProcessCurrentDispFrm(chanHandle);
        }
        else
        {
            vpifConfigureNextDispFrm(chanHandle);
        }
    }

    /* Enable VPIF HWI event                                                  */
    ECM_enableEvent(
        ((Vpif_Object *)(chanHandle->devHandle))->deviceInfo.cpuEventNumber);
}


/**
 *  \brief  Get field id from VPIF registers
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return field id
 *
 */
static Uint32 vpifGetFieldId(Vpif_ChanObj *chanHandle)
{
    Uint32 getField;
    assert(NULL != chanHandle);

    switch (chanHandle->chanNum)
    {
        case Vpif_CHANNEL0_VIDEO:
            /* use the Field EXTract macro */
            getField = \
                ((chanHandle->baseAddress->C0CTRL & CSL_VPIF_C0CTRL_FID_MASK) >>
                CSL_VPIF_C0CTRL_FID_SHIFT);
            break;

        case Vpif_CHANNEL1_VIDEO:
            getField = \
                ((chanHandle->baseAddress->C1CTRL & CSL_VPIF_C1CTRL_FID_MASK) >>
                CSL_VPIF_C1CTRL_FID_SHIFT);
            break;

        case Vpif_CHANNEL2_VIDEO:
            getField = \
                ((chanHandle->baseAddress->C2CTRL & CSL_VPIF_C2CTRL_FID_MASK) >>
                CSL_VPIF_C2CTRL_FID_SHIFT);
            break;

        case Vpif_CHANNEL3_VIDEO:
            getField = \
                ((chanHandle->baseAddress->C3CTRL & CSL_VPIF_C3CTRL_FID_MASK) >>
                CSL_VPIF_C3CTRL_FID_SHIFT);
            break;

        default:
            /* This case will not happen but return some value which is not 0
            or 1 */
            getField = 2u;
            break;
    }
    return getField;
}


/**
 *  \brief  Process the current captured frame. If a request is pending,
 *          callback is called. If not, the packet is made most recent IOP.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifProcessCurrentCapFrm(Vpif_ChanObj *chanHandle)
{
    FVID_Frame *capCurViop;

    assert(NULL != chanHandle);

    /* Copy frame capture time and store current time in timeStamp field */
    chanHandle->curViop->timeStamp = CLK_getltime();
    /* one frame is captured. If next frame is available, release curFrm and
       update it as most recent viop. Update the current and next viop
       pointers */
    capCurViop = chanHandle->curViop;
    assert(NULL != capCurViop);
    chanHandle->curViop = chanHandle->nextViop;

    if ((chanHandle->mrViop != NULL) &&
        ((capCurViop != chanHandle->mrViop) &&
         (chanHandle->nextViop != chanHandle->mrViop)))
    {
        /* If most recent viop is avaliable then put it back in queue */
        QUE_put(&chanHandle->qIn, chanHandle->mrViop);
    }
    /* update the most recent viop with the captured frame */
    chanHandle->mrViop = capCurViop;

    /* call the channel's callback function */
    if (chanHandle->mrViop != chanHandle->curViop)
    {
        if (chanHandle->packetIOM != NULL)
        {
            /* call the chanHandle's callback function */
            *(Void**)chanHandle->packetIOM->addr = capCurViop;
            chanHandle->packetIOM->size = sizeof(FVID_Frame);
            chanHandle->packetIOM->status = chanHandle->currPktStatus;
            chanHandle->mrViop = NULL;
            /* decrement number of buffers available */
            chanHandle->numQueBuf--;

            chanHandle->cbFxn(chanHandle->cbArg, chanHandle->packetIOM);
            chanHandle->packetIOM = NULL;
        }
        else if (TRUE == chanHandle->queEmpty)
        {
            chanHandle->nextViop = chanHandle->mrViop;
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        chanHandle->mrViop = NULL;
    }
}


/**
 *  \brief  Configure the VPIF register with the next frame buffers for capture.
 *          If raw VBI or HBI is enabled, update their frame buffer address also.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifConfigureNextCapFrm(Vpif_ChanObj *chanHandle)
{
    FVID_Frame *capViop;

    assert(NULL != chanHandle);

    /* one *field* is captured, configure the next *frame* if it is available
       otherwise hold on current frame */
    capViop = (FVID_Frame *)QUE_get(&chanHandle->qIn);
    if (capViop != (FVID_Frame *)&chanHandle->qIn)
    {
        /* queue IS not empty */
        chanHandle->nextViop = capViop;
    }
    else
    {
        if (NULL == chanHandle->packetIOM)
        {
            /* in queue is empty, but no request is pending recycle the current
             * VIOP back but still set it as the most recent VIOP
             */
            chanHandle->nextViop = chanHandle->mrViop;
        }
        chanHandle->queEmpty = TRUE;
    }
    vpifConfigureAddress(chanHandle);
    /* Configure the VBI addresses*/
    if ((0 != chanHandle->hbiBufSz) || (0 != chanHandle->vbiBufSz))
    {
        /* Calculate the offset for vbi/hbi data in the buffer */
        if (0 != chanHandle->hbiBufSz)
        {
            vpifConfigureHbiAddress(chanHandle);
        }
        if (0 != chanHandle->vbiBufSz)
        {
            vpifConfigureVbiAddress(chanHandle);
        }
    }
}

/**
 *  \brief  Process the current displayed frame. If a request is pending,
 *          callback is called. If not, the packet is put inside the OUT queue.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifProcessCurrentDispFrm(Vpif_ChanObj *chanHandle)
{
    FVID_Frame *dispCurViop;

    assert(NULL != chanHandle);
    /* Copy frame display time and store current time in timeStamp field */
    chanHandle->curViop->timeStamp = CLK_getltime();

    /* one frame is displayed If next frame is available, release curFrm. Update
       the current and next viop pointers */
    dispCurViop = chanHandle->curViop;
    chanHandle->curViop = chanHandle->nextViop;
    if (dispCurViop != chanHandle->curViop)
    {
        if (chanHandle->packetIOM != NULL)/*some request is pending*/
        {
            /* call the channel's callback function */
            *(Void**)chanHandle->packetIOM->addr = dispCurViop;
            chanHandle->packetIOM->size = sizeof(FVID_Frame);
            chanHandle->packetIOM->status = chanHandle->currPktStatus;

            if ((0 != chanHandle->numSliceServices) &&
                (((IOM_COMPLETED != chanHandle->sliceVbiFld0I2cCbStatus) ||
                (IOM_COMPLETED != chanHandle->sliceVbiFld1I2cCbStatus)) ||
                ((IOM_PENDING != chanHandle->sliceVbiFld0I2cSubmitStatus) ||
                (IOM_PENDING != chanHandle->sliceVbiFld1I2cSubmitStatus))))
            {
                /* Some problem with I2C write to the codec for VBI */
                chanHandle->packetIOM->status = Vpif_VBI_I2C_ERR;
            }

            /* decrement number of buffers available */
            chanHandle->numQueBuf--;

            chanHandle->cbFxn(chanHandle->cbArg, chanHandle->packetIOM);
            chanHandle->packetIOM = NULL;
        }
        else
        {
            QUE_put(&chanHandle->qOut, dispCurViop);
        }
    }
}


/**
 *  \brief  Configure the VPIF register with the next frame buffers for display.
 *          If raw VBI or HBI is enabled, update their frame buffer address also.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifConfigureNextDispFrm(Vpif_ChanObj *chanHandle)
{
    FVID_Frame *dispViop;

    assert(NULL != chanHandle);
    dispViop = (FVID_Frame *)QUE_get(&chanHandle->qIn);
    if (dispViop != (FVID_Frame *)&chanHandle->qIn)
    {
        /* queue IS not empty */
        chanHandle->nextViop = dispViop;
    }
    else
    {
        chanHandle->queEmpty = TRUE;
    }
    vpifConfigureAddress(chanHandle);
    /* Configure the VBI addresses */
    if ((0 != chanHandle->hbiBufSz) || (0 != chanHandle->vbiBufSz))
    {
        if (0 != chanHandle->hbiBufSz)
        {
            /* Set HBI address */
            vpifConfigureHbiAddress(chanHandle);
        }
        if (0 != chanHandle->vbiBufSz)
        {
            /* Set VBI address */
            vpifConfigureVbiAddress(chanHandle);
        }
    }
}

/**
 *  \brief  Callback function for Slice VBI display data to codec.
 *
 *          This function is the callback for the display operation when
 *          writing VBI data to the codec using theinterface(I2C/SPI)
 *          is completed
 *
 *  \param  handle  [IN]   Handle of the Channel Object
 *  \param  status  [IN]   Status returned by the codec layer
 *
 *  \return None
 *
 */
static Void vpifSliceCb(Ptr handle, Int32 *status)
{
    Vpif_ChanObj *channel = NULL;

    assert(NULL != handle);
    assert(NULL != status);
    channel = (Vpif_ChanObj *)handle;
    assert(NULL != channel);

    if (0 == channel->fieldId)
    {
        channel->sliceVbiFld0I2cCbStatus = *status;
    }
    else
    {
        channel->sliceVbiFld1I2cCbStatus = *status;
    }
    if ((IOM_COMPLETED != channel->sliceVbiFld0I2cCbStatus) ||
        (IOM_COMPLETED != channel->sliceVbiFld1I2cCbStatus))
    {
        vpifProcessErrFrm(channel);
    }
    SWI_post(channel->vbiTskletHandle);
}



/**
 *  \brief Vpif ISR
 *
 *  This function is the ISR for the entire Vpif device. This routine further
 *  processes the exact cause of interrupt by reading interrupt status register.
 *  It changes status of the displayed or captured buffer, takes next
 *  buffer from the queue and sets its address in VPIF registers.
 *
 *  \param  arg  [IN]    Handle to the Vpif instance object
 *
 *  \return Void
 *
 *  \enter  arg   must be a valid pointer and should not be null.
 *
 *  \leave  Not Implemented.
 */
Void vpifIsr(Arg arg)
{
    Vpif_Object             *isrInstHandle = NULL;
    Uint32                  isrChannelId = 0, j = 0;
    Uint32                  fid;
    Int32                   isrStatus = IOM_COMPLETED;
    volatile Uint32         temp = 0;
    Vpif_ChanObj            *channel = NULL;
    volatile  CSL_VpifRegs  *vpifIsrRegs = NULL;
    /*
     *  Note that the VPIF peripheral has no mechanism to reissue interrupts
     *  that may have been dropped by the CPU. In a combined
     *  interrupt configuration, the CPU may be busy servicing a VPIF interrupt
     *  when another VPIF interrupt arrives. If the interrupt controller has
     *  no means to count VPIF interrupts, the CPU will drop subsequent
     *  VPIF interrupts until servicing of the first VPIF interrupt is complete.
     */

    assert(NULL != arg);
    isrInstHandle = (Vpif_Object *)ArgToPtr(arg);
    assert(NULL != isrInstHandle);

    vpifIsrRegs = isrInstHandle->deviceInfo.baseAddr;
    assert(NULL != vpifIsrRegs);

    /*
     *  INTSTAT(R) : Interrupt status register for INTEN. This register
     *               works even if the interrupt register INTEN is inactivated.
     *               To clear this register, you should configure INTSTATCLR
     *               register to 1. This status value is internally cleared if
     *               INTSTATCLR is configured to 1.
     */

    /* Get the current interrupt status                           */
    temp = vpifIsrRegs->INTSTAT;
    if (0 == temp)
    {
        /* fake interrupts coming. make a count if you want to count */
        isrStatus = IOM_EBADARGS;
    }


    if (IOM_COMPLETED == isrStatus)
    {
        /*
         * Clear CHANNELx bit in INTSTAT. Else next interrupts won't come
         * INTSTATCLR(W) : If user would like to clear status register INTSTAT,
         *                 user should configure this register to be 1. This
         *                 register is “write-only”, and written value cannot
         *                 be read.
         */
        vpifIsrRegs->INTSTATCLR = temp;
        if (0 != vpifIsrRegs->INTSTATCLR)
        {
            /* Error as interrupt status reg not cleared*/
        }
        for (j = 0; j < Vpif_TOTAL_NUM_CHANNELS; j++)
        {
            isrChannelId = temp & (0x1 << j);
            if (0 != isrChannelId)
            {
                /* isrChannelId can only be either 0, 1, 2 or 3 */
                isrChannelId = j;
                channel = &isrInstHandle->chanObj[isrChannelId];
                if (((channel->chState & Vpif_DriverState_READY) !=
                     Vpif_DriverState_READY) ||
                    (0 == channel->started))
                {
                    /* Should not happen - as channel is either not ready or
                    streaming is not started on this channel */
                    isrStatus = IOM_EBADMODE;
                }
                if (IOM_COMPLETED == isrStatus)
                {
                    if (0x10u == (temp & 0x10u))
                    {
                        channel->chError++;
                    }
                    else
                    {
                        channel->frameInt++;
                    }

                    /*
                     * If it is first interrupt, ignore it. The first interrupt
                     * comes as soon as the channel operation starts.
                     */
                    if (1 == channelFirstInt[isrChannelId])
                    {
                        /* ignore first interrupt */
                        channelFirstInt[isrChannelId] = 0;
                        isrStatus = IOM_EBADMODE;
                    }
                }
                if (IOM_COMPLETED == isrStatus)
                {

                    /*
                     * Here start the processing of capture first and then
                     * display, assuming capture frame is precious
                     */

                    /* capture channel */
                    if ((Vpif_CHANNEL0_VIDEO == isrChannelId) ||
                        (Vpif_CHANNEL1_VIDEO == isrChannelId))
                    {
                        /* Check the field format */
                        if (Vpif_FrameFormat_PROGRESSIVE ==
                            channel->chVidParams.frameFmt)
                        {
                            vpifProcessCurrentCapFrm(channel);
                            vpifConfigureNextCapFrm(channel);
                        }
                        else/* Interlaced mode */
                        {
                            channel->fieldId ^= 1u;
                            /* Get field id from VPIF registers */
                            fid = vpifGetFieldId(channel);
                            /* If field id does not match with stored field id
                            */
                            if (fid != channel->fieldId)
                            {
                                /* Make them in sync */
                                if (0 == fid)
                                {
                                    channel->fieldId = fid;
                                }
                            }
                            else
                            {
                                /* device field id and local field id are in
                                sync */
                                /* If this is even/top field */
                                if (0 == fid)
                                {
                                    if (channel->curViop != channel->nextViop)
                                    {
                                        if (0 == channel->numSliceServices)
                                        {
                                            vpifProcessCurrentCapFrm(channel);
                                        }
                                        else
                                        {
                                            /* Post sema to start read of vbi
                                            data */
                                            SEM_post(channel->syncSem);
                                        }
                                    }
                                }
                                else if (1 == fid)/* odd/bottom field */
                                {
                                    /* one field is captured, configure the next
                                    frame if it is available otherwise hold on
                                    current frame */
                                    if ((channel->queEmpty == TRUE) ||
                                        (channel->curViop != channel->nextViop))
                                    {
                                        /* do nothing continue */
                                    }
                                    else
                                    {
                                        if (0 == channel->numSliceServices)
                                        {
                                            vpifConfigureNextCapFrm(channel);
                                        }
                                        else
                                        {
                                            /* Post sema to start read of vbi
                                            data */
                                            SEM_post(channel->syncSem);
                                        }
                                    }
                                }/* else odd mode */
                            }/* else field id are in sync */
                        }/* else interlaced mode */
                    }/* end if capture channel */
                    /* display channel */
                    else
                    {
                        /* Check the field format */
                        if (Vpif_FrameFormat_PROGRESSIVE ==
                            channel->chVidParams.frameFmt)
                        {
                            vpifProcessCurrentDispFrm(channel);
                            vpifConfigureNextDispFrm(channel);
                        }
                        else/* Interlaced mode */
                        {
                            channel->fieldId ^= 1u;
                            /* Get field id from VPIF registers */
                            fid = vpifGetFieldId(channel);
                            /* If field id does not match with stored field
                            id */
                            if (fid != channel->fieldId)
                            {
                                /* Make them in sync */
                                if (0 == fid)
                                {
                                    channel->fieldId = fid;
                                }
                            }
                            else
                            {
                                /* device field id and local field id are in
                                sync */
                                /* If this is even/top field */
                                if (0 == fid)
                                {
                                    if (channel->curViop != channel->nextViop)
                                    {
                                        if (0 == channel->numSliceServices)
                                        {
                                            vpifProcessCurrentDispFrm(channel);
                                        }
                                        else
                                        {
                                            SEM_post(channel->syncSem);
                                        }
                                    }
                                }
                                else if (1 == fid)/* odd/bottom field */
                                {
                                    /* one field is displayed configure the next
                                    frame if it is available otherwise hold on
                                    current frame */
                                    if ((channel->queEmpty == TRUE) ||
                                        (channel->curViop != channel->nextViop))
                                    {
                                        /* do nothing continue */
                                    }
                                    else
                                    {
                                        if (0 != channel->numSliceServices)
                                        {
                                            SEM_post(channel->syncSem);
                                        }
                                        else
                                        {
                                            vpifConfigureNextDispFrm(channel);
                                        }
                                    }
                                }/* else odd mode */
                            }/* else field id are in sync */
                        }/* else interlaced mode */
                    }/* else display channel */
                }
            }/* end if valid channel id */
        }/* end for loop */
    }/* if (IOM_COMPLETED == isrStatus) */
}



/**
 *  \brief    vpifRegisterInterrupt
 *
 *   This function is invoked in order for to register Vpif interrupts
 *   with the requested interrupt handler with the OS (BIOS) and enables
 *   the interrupt.
 *
 *  \param    inDevp       [IN]    Pointer to the device
 *  \param    hwiNumber    [IN]    interrupt number
 *  \param    evt          [IN]    event number
 *
 *  \return   Void
 *
 */
static Void vpifRegisterInterrupt(Ptr inDevp, Uint16 hwiNumber, Uint16 evt)
{
    Uint32              intKey = 0;
    Vpif_Object         *regIntInstHandle;
    ECM_Attrs           ecmattrs = ECM_ATTRS;

    VPIF_DEBUG("Entered ---> vpifRegisterInterrupt\n");

    assert(NULL != inDevp);
    regIntInstHandle = inDevp;
    assert(NULL != regIntInstHandle);

    /*Enable the interrupt later                                              */
    ecmattrs.unmask = 0u;

    /*setup arg such that isr will get 'chan' as arg                          */
    ecmattrs.arg = (Arg)regIntInstHandle;

    intKey = (Uint32)_disable_interrupts();
    /* Plug the ISR with interrupt (event) number                             */
    ECM_dispatchPlug(evt, (ECM_Fxn)&vpifIsr, &ecmattrs);

    /* Enabling the ECM event                                                 */
    ECM_enableEvent(evt);

    /* Enabling the HWI_ID where corresponding ECM group is configured        */
    C64_enableIER(1 << hwiNumber);

    _restore_interrupts(intKey);
    VPIF_DEBUG("Exited ---> vpifRegisterInterrupt\n");
}

/**
 *  \brief    vpifUnRegisterInterrupt
 *
 *   This function is invoked to unregister and disable interrupt
 *   handler and interrupt cause respectively.
 *
 *  \param    evt           [IN]    event number
 *
 *  \return   NONE
 *
 */
static Void vpifUnRegisterInterrupt(Uint16 evt)
{
    Uint32 key = 0;

    VPIF_DEBUG("Entered ---> vpifUnRegisterInterrupt\n");

    key = (Uint32)_disable_interrupts();
    /* Disable the event                                                      */
    ECM_disableEvent(evt);
    _restore_interrupts(key);

    VPIF_DEBUG("Exited ---> vpifUnRegisterInterrupt\n");
}

/**
 *  \brief   allocating buffer as requested by application. Allocates the
 *           frame buffer from the memory segment reserved for VPIF
 *
 *  \param   chanHandle     [In]   VPIF channel object pointer
 *  \param   newFrame       [Out]  Pointer to FVID frame pointer
 *
 *  \return  IOM_COMPLETED on success
 *           IOM_EALLOC on failure
 */
static Int32 vpifAllocBuffer(Vpif_ChanObj *chanHandle, FVID_Frame **newFrame)
{
    Int32 allocBuffRtnVal = IOM_COMPLETED;
    Bool allocStatus; /* TRUE if successful */

    VPIF_DEBUG("Entered ---> vpifAllocBuffer\n");
    assert(NULL != chanHandle);
    assert(NULL != newFrame);

    /* allocate frame buffer structure. Alloc the memory aligned to cache line*/
    *newFrame = (FVID_Frame *)MEM_calloc(chanHandle->segId,
                    sizeof(FVID_Frame),
                    chanHandle->alignment);
    if (NULL == (*newFrame))
    {
        allocBuffRtnVal = IOM_EALLOC;
    }

    if (IOM_EALLOC != allocBuffRtnVal)
    {
        /* allocate frame buffer and assign to FVID_Frame structure */
        if (IOM_COMPLETED != vpifAllocAssignBuffer(chanHandle, *newFrame))
        {
            allocBuffRtnVal = IOM_EALLOC;
            allocStatus = MEM_free(chanHandle->segId,
                              *newFrame,
                              sizeof(FVID_Frame));
            if (TRUE != allocStatus)
            {
                allocBuffRtnVal = IOM_EFREE;
            }
        }
    }
    VPIF_DEBUG("Exited ---> vpifAllocBuffer\n");

    return allocBuffRtnVal;
}



/**
 *  \brief   This function allocates buffer and assigning it to FVID_Frame
 *           structure. This function calculates buffers offset for Y and C
 *           in the top and bottom field
 *
 *  \param   chanHandle     [In]  VPIF channel object pointer
 *  \param   newFrame       [Out] FVID frame pointer
 *
 *  \return  IOM_COMPLETED on success
 *           IOM_EALLOC on failure
 */
static Int32 vpifAllocAssignBuffer(Vpif_ChanObj *chanHandle,
                                   FVID_Frame *newFrame)
{
    Int32 allocAssignRtnVal = IOM_COMPLETED;
    Uint8 *curAddr = NULL;
    Uint32 totalSize;

    assert((NULL != chanHandle) && (NULL != newFrame));

    VPIF_DEBUG("Entered ---> vpifAllocAssignBuffer\n");

    totalSize = chanHandle->bufSz + chanHandle->hbiBufSz +
                chanHandle->vbiBufSz +
                (2u * chanHandle->numSliceServices * sizeof(FVID_SliceFrame));

    /* allocate frame buffer */
    curAddr = (Uint8 *)MEM_calloc(chanHandle->segId,
                           totalSize,
                           chanHandle->alignment);

    /* Update the VBI service in the frame */
    newFrame->vbiService = FVID_VBI_SERVICE_NONE;
    if (0 != chanHandle->hbiBufSz)
    {
        newFrame->vbiService |= FVID_VBI_SERVICE_HBI;
    }
    if (0 != chanHandle->vbiBufSz)
    {
        newFrame->vbiService |= FVID_VBI_SERVICE_RAW_VBI;
    }
    if (0 != chanHandle->numSliceServices)
    {
        newFrame->vbiService |= FVID_VBI_SERVICE_SLICE_VBI;
    }

    if (NULL != curAddr)
    {
        /* Move the data to physical memory and invalidate the cache line     */
        BCACHE_wbInv(curAddr, totalSize, TRUE);

        /* Calculate offsets for Y top, Y Bottom, C top and C Bottom */
        if (Vpif_CaptureFormat_CCDC == chanHandle->chVidParams.captureFormat)
        {
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                /*
                 * buf1 contains top field data and buf2 contains bottom field
                 * data in raw data capture mode.
                 */
                newFrame->frame.riFrm.buf1 = (Char *)curAddr;
                /* interlaced storage can be frame based or field based */
                if (Vpif_SdramStorage_FIELD == chanHandle->frameStorageMode)
                {
                    newFrame->frame.riFrm.buf2 = \
                        (Char *)((Uint32)curAddr + (chanHandle->bufSz / 2u));
                }
                else
                {
                    newFrame->frame.riFrm.buf2 = \
                        (Char *)((Uint32)curAddr + chanHandle->hpitch);
                }
            }
            else
            {
                newFrame->frame.rpFrm.buf = (Char *)curAddr;
            }
            newFrame->frameFormat = FVID_RAW_FORMAT;

            if (Vpif_RawCaptureDataWidth_8BITS == chanHandle->dataSz)
            {
                newFrame->bpp = FVID_BPP_BITS8;
            }
            else if (Vpif_RawCaptureDataWidth_10BITS == chanHandle->dataSz)
            {
                newFrame->bpp = FVID_BPP_BITS10;
            }
            else if (Vpif_RawCaptureDataWidth_12BITS == chanHandle->dataSz)
            {
                newFrame->bpp = FVID_BPP_BITS12;
            }
            else
            {
                assert(FALSE);
            }
            newFrame->pitch = chanHandle->hpitch;
        }
        else
        {
            /* field 1 */
            newFrame->frame.spFrm.y1 = curAddr;
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                /* interlaced storage can be frame based or field based */
                if (Vpif_SdramStorage_FIELD == chanHandle->frameStorageMode)
                {
                    /* field based storage - Both field 0 and 1 are stored
                    seperately. The line pitch is just the line size          */
                    newFrame->frame.spFrm.y2 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz / 4u));
                    newFrame->frame.spFrm.c1 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz / 2u));
                    newFrame->frame.spFrm.c2 = \
                        (Uint8 *)((Uint32)curAddr + ((chanHandle->bufSz / 2u) +
                        (chanHandle->bufSz / 4u)));
                    newFrame->storeFormat = FVID_STORAGE_FORMAT_FIELD;
                }
                else
                {
                    /* frame based storage - To merge the two fields together.
                    The line pitch is twice the line size */
                    newFrame->frame.spFrm.y2 = \
                        (Uint8 *)((Uint32)curAddr + chanHandle->hpitch);
                    newFrame->frame.spFrm.c1 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz / 2u));
                    newFrame->frame.spFrm.c2 = \
                        (Uint8 *)((Uint32)curAddr + ((chanHandle->bufSz / 2u) +
                        chanHandle->hpitch));
                    newFrame->storeFormat = FVID_STORAGE_FORMAT_FRAME;
                }
            }
            else
            {
                /* in progressive only frame based storage is supported */
                newFrame->frame.spFrm.c1 = \
                    (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz / 2u));
                newFrame->frame.spFrm.y2 = NULL;
                newFrame->frame.spFrm.c2 = NULL;
                newFrame->storeFormat = FVID_STORAGE_FORMAT_FRAME;
            }
            newFrame->pitch = chanHandle->hpitch;
            /* Calculate the offset for vbi data in the buffer */
            if (0 != chanHandle->hbiBufSz)
            {
                if (Vpif_FrameFormat_INTERLACED ==
                    chanHandle->chVidParams.frameFmt)
                {
                    newFrame->vbiFrm.h1 = \
                        (Uint8 *)((Uint32)curAddr + chanHandle->bufSz);
                    newFrame->vbiFrm.h2 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz +
                        (chanHandle->chVidParams.rawHbiParams.samplePerLine *
                         chanHandle->chVidParams.rawHbiParams.countFld0)));
                }
                else
                {
                    newFrame->vbiFrm.h1 = \
                        (Uint8 *)((Uint32)curAddr + chanHandle->bufSz);
                    newFrame->vbiFrm.h2 = NULL;
                }
            }
            else
            {
                newFrame->vbiFrm.h1 = NULL;
                newFrame->vbiFrm.h2 = NULL;
            }
            if (0 != chanHandle->vbiBufSz)
            {
                if (Vpif_FrameFormat_INTERLACED ==
                    chanHandle->chVidParams.frameFmt)
                {
                    newFrame->vbiFrm.v1 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz +
                         chanHandle->hbiBufSz));
                    newFrame->vbiFrm.v2 = \
                        (Uint8 *)((Uint32)curAddr +
                        (chanHandle->bufSz + chanHandle->hbiBufSz +
                        (chanHandle->chVidParams.rawVbiParams.samplePerLine *
                        chanHandle->chVidParams.rawVbiParams.countFld0)));
                }
                else
                {
                    newFrame->vbiFrm.v1 = \
                        (Uint8 *)((Uint32)curAddr + (chanHandle->bufSz +
                        chanHandle->hbiBufSz));
                    newFrame->vbiFrm.v2 = NULL;
                }
            }
            else
            {
                newFrame->vbiFrm.v1 = NULL;
                newFrame->vbiFrm.v2 = NULL;
            }
            if (0 != chanHandle->numSliceServices)
            {
                newFrame->vbiFrm.s1 = \
                    (FVID_SliceFrame *)((Uint32)curAddr + (chanHandle->bufSz +
                    (chanHandle->hbiBufSz + chanHandle->vbiBufSz)));
                newFrame->vbiFrm.s2 = \
                    (FVID_SliceFrame *)((Uint32)curAddr + (chanHandle->bufSz +
                    (chanHandle->hbiBufSz + chanHandle->vbiBufSz)) +
                    (chanHandle->numSliceServices * sizeof(FVID_SliceFrame)));
            }
            else
            {
                newFrame->vbiFrm.s1 = NULL;
                newFrame->vbiFrm.s2 = NULL;
            }
            newFrame->frameFormat = FVID_YCbCr422_SEMIPLANAR_UV;
            newFrame->bpp = FVID_BPP_BITS8;
        }

        if (Vpif_IoMode_DIS == chanHandle->channelMode)
        {
            memset(newFrame->frame.spFrm.y1, 0x10, chanHandle->bufSz / 2);
            memset(newFrame->frame.spFrm.c1, 0x80, chanHandle->bufSz / 2);
        }

        /* Set FVID_Frame fields */
        newFrame->timeStamp = 0;
        newFrame->lines = chanHandle->chVidParams.height;
    }
    else
    {
        allocAssignRtnVal = IOM_EALLOC;
    }
    VPIF_DEBUG("Exited ---> vpifAllocAssignBuffer\n");

    return allocAssignRtnVal;
}


/**
 *  \brief   Free the given frame buffer and its structure memory
 *
 *  \param   chanHandle     [In]  VPIF channel object pointer
 *  \param   freeFrame      [In]  FVID frame pointer
 *
 *  \return  IOM_COMPLETED on success
 *           IOM_EFREE on failure
 *
 */
static Int32 vpifFreeBuffer(Vpif_ChanObj *chanHandle, FVID_Frame **freeFrame)
{
    Int32 freeBuffRtnVal = IOM_COMPLETED;
    FVID_Frame *ptrFrame;
    Bool freeStatus; /* TRUE if successful */

    VPIF_DEBUG("Entered ---> vpifFreeBuffer\n");

    assert(NULL != chanHandle);
    assert(NULL != freeFrame);
    ptrFrame = *freeFrame;
    assert(NULL != ptrFrame);

    /* free frame buffer */
    freeStatus = MEM_free(chanHandle->segId,
                     ptrFrame->frame.spFrm.y1,
                     chanHandle->bufSz + chanHandle->hbiBufSz +
                         chanHandle->vbiBufSz +
                         (2u * chanHandle->numSliceServices *
                             sizeof(FVID_SliceFrame)));
    if (TRUE != freeStatus)
    {
        freeBuffRtnVal = IOM_EFREE;
    }
    else
    {
        freeStatus = MEM_free(chanHandle->segId, ptrFrame, sizeof(FVID_Frame));
        if (TRUE != freeStatus)
        {
            freeBuffRtnVal = IOM_EFREE;
        }
    }
    VPIF_DEBUG("Exited ---> vpifFreeBuffer\n");
    return freeBuffRtnVal;
}



/**
 *  \brief   get the number of pending requests
 *
 *  \param   chanHandle     [In]  VPIF channel object pointer
 *  \param   args           [Out] number of pending requests
 *
 *  \return  IOM_COMPLETED
 */
static Int32 vpifGetNumPendingIoReq(Vpif_ChanObj *chanHandle, Ptr args)
{
    QUE_Handle nextElem;
    Int32 i = 0;

    assert(NULL != chanHandle);
    assert(NULL != args);

    nextElem = &chanHandle->qIn;
    assert(NULL != nextElem);
    nextElem = QUE_next(nextElem);

    while (nextElem != &chanHandle->qIn)
    {
        i++;
        nextElem = QUE_next(nextElem);
    }

    if (chanHandle->curViop != chanHandle->nextViop)
    {
        i++;
    }

    *(Int32*)args = i;

    return IOM_COMPLETED;
}

/**
 *  \brief   Set the resolution of the current channel
 *
 *  \param   chanHandle [In]  VPIF channel object pointer
 *  \param   resolution [In]  Pointer to the desired resoltuion
 *
 *  \return  IOM_COMPLETED
 */
static Int32 vpifSetResolution(Vpif_ChanObj *chanHandle,
                               Vpif_ConfigParams *resolution)
{
    Int32 setResRtnVal = IOM_COMPLETED;
    Uint8 setResChNum;
    Uint8 setResMaxModes;/* Maximum video modes supported by the channel */
    Int32 setResIndex;

    assert(NULL != chanHandle);
    assert((NULL != resolution) || (Vpif_VideoMode_NONE != resolution->mode));
    setResChNum = chanHandle->chanNum;
    if (Vpif_VideoMode_NONE != resolution->mode)
    {
        switch (setResChNum)
        {
            case Vpif_CHANNEL0_VIDEO:
                setResMaxModes = Vpif_CH0_MAX_MODES;
                break;
            case Vpif_CHANNEL1_VIDEO:
                setResMaxModes = Vpif_CH1_MAX_MODES;
                break;
            case Vpif_CHANNEL2_VIDEO:
                setResMaxModes = Vpif_CH2_MAX_MODES;
                break;
            case Vpif_CHANNEL3_VIDEO:
                setResMaxModes = Vpif_CH3_MAX_MODES;
                break;
            default:
                /* This condition will not arise. */
                setResMaxModes = 0;
                break;
        }
        /* loop on the number of mode supported per channel */
        for (setResIndex = 0; setResIndex < setResMaxModes; setResIndex++)
        {
            /* If the mode is found, set the parameter in VPIF register */
            if (vpifConfigParams[setResChNum][setResIndex].mode ==
                resolution->mode)
            {
                break;
            }
        }
        if (setResIndex == setResMaxModes)
        {
            setResRtnVal = IOM_EBADARGS;
        }
        else
        {
            chanHandle->chVidParams = \
                vpifConfigParams[setResChNum][setResIndex];
            setResRtnVal = IOM_COMPLETED;
        }
    }
    else
    {
        chanHandle->chVidParams = *resolution;
    }


    /* recalculate the size parameters for buffers */
    if (IOM_COMPLETED == setResRtnVal)
    {
        if (0 != chanHandle->vbiBufSz)
        {
            /* Calculate the size of the RAW VBI buffer */
            chanHandle->vbiBufSz = \
                chanHandle->chVidParams.rawVbiParams.samplePerLine *
                (chanHandle->chVidParams.rawVbiParams.countFld0 +
                chanHandle->chVidParams.rawVbiParams.countFld1);
        }
        if (0 != chanHandle->hbiBufSz)
        {
            /* Calculate the size of the RAW HBI buffer */
            chanHandle->hbiBufSz = \
                chanHandle->chVidParams.rawHbiParams.samplePerLine *
                (chanHandle->chVidParams.rawHbiParams.countFld0 +
                chanHandle->chVidParams.rawHbiParams.countFld1);
        }
        /* Check if width is 8 byte aligned or not                            */
        chanHandle->hpitch = (chanHandle->chVidParams.width + 7u) & (~7u);

        chanHandle->bufSz = (Uint32)((chanHandle->hpitch) *
                               (chanHandle->chVidParams.height) *
                               2u);/* MUL by 2 bec of chrominance and luminance*/

        if (chanHandle->channelMode == Vpif_IoMode_RAW_CAP)
        {
            /* As RAW data does not have Croma and Luma, so no MUL by *2* */
            chanHandle->bufSz = (Uint32)((chanHandle->hpitch) *
                                   (chanHandle->chVidParams.height));
            /*
             * MUL by *2* bec of 10 or 12 bit per pixel
             * 8  bit/pixel mode Pixel 0[7:0]             => 1 byte for 1 pixel
             * 10 bit/pixel mode “000000” & pixel 0[9:0]  => 2 byte for 1 pixel
             * 12 bit/pixel mode “0000” & pixel 0[11:0]   => 2 byte for 1 pixel
             */
            if (chanHandle->dataSz != Vpif_RawCaptureDataWidth_8BITS)
            {
                chanHandle->bufSz = (Uint32)((chanHandle->hpitch) *
                                       (chanHandle->chVidParams.height) *
                                       (2u));
            }
        }

        if (Vpif_FrameFormat_PROGRESSIVE == chanHandle->chVidParams.frameFmt)
        {
            chanHandle->lineOffset = chanHandle->chVidParams.width;
            if ((chanHandle->channelMode == Vpif_IoMode_RAW_CAP) &&
                (chanHandle->dataSz != Vpif_RawCaptureDataWidth_8BITS))
            {
                chanHandle->lineOffset = \
                    chanHandle->chVidParams.width * 2u;
            }
        }
        else
        {
            if (Vpif_SdramStorage_FIELD == chanHandle->frameStorageMode)
            {
                /* Field based storage: The line pitch is just the line size  */
                chanHandle->lineOffset = chanHandle->chVidParams.width;
            }
            else
            {
                /*
                 * Frame based storage: Merge the two fields together.
                 * Line pitch is twice the line size
                 */
                chanHandle->lineOffset = \
                    chanHandle->chVidParams.width * 2u;
            }
        }
    }

    return setResRtnVal;
}


/**
 *  \brief   Get current channel Video parameters
 *
 *  \param   chanHandle [In]  VPIF channel object pointer
 *  \param   currParams [Out] pointer to current channel parameters structure
 *
 *  \return  IOM_COMPLETED
 */
static Int32 vpifGetChanParams(Vpif_ChanObj *chanHandle,
                               Vpif_StdInfo *currParams)
{

    assert((NULL != chanHandle) && (NULL != currParams));
    assert((Vpif_CHANNEL0_VIDEO == chanHandle->chanNum) ||
           (Vpif_CHANNEL1_VIDEO == chanHandle->chanNum) ||
           (Vpif_CHANNEL2_VIDEO == chanHandle->chanNum) ||
           (Vpif_CHANNEL3_VIDEO == chanHandle->chanNum));

    /* Return current parameters from channel object */
    currParams->stdMode = chanHandle->vidMode;
    currParams->sdramStorage = chanHandle->frameStorageMode;
    currParams->activeLines = chanHandle->chVidParams.height;
    currParams->activePixels = chanHandle->chVidParams.width;
    currParams->framePerSec = chanHandle->chVidParams.fps;
    currParams->stdFrameFormat = chanHandle->chVidParams.frameFmt;
    currParams->stdVbiService = Vpif_VbiServiceType_NONE;
    if (0 != chanHandle->hbiBufSz)
    {
        currParams->stdVbiService |= Vpif_VbiServiceType_HBI;
    }
    if (0 != chanHandle->vbiBufSz)
    {
        currParams->stdVbiService |= Vpif_VbiServiceType_RAW_VBI;
    }
    if (0 != chanHandle->numSliceServices)
    {
        currParams->stdVbiService |= Vpif_VbiServiceType_SLICE_VBI;
    }

    return IOM_COMPLETED;
}


/**
 *  \brief  Configure memory address line-offset for the horizontal ancillary
 *          buffers.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifSetHancAddressOffset(Vpif_ChanObj *chanHandle)
{
    CSL_VpifCapRegsOvly setHancCapReg;
    CSL_VpifDisRegsOvly setHancDisReg;
    Uint8 setHancDispChId;
    Uint8 channelId;

    assert(NULL != chanHandle);
    channelId = chanHandle->chanNum;
    switch (channelId)
    {
        case Vpif_CHANNEL0_VIDEO:
        case Vpif_CHANNEL1_VIDEO:
            setHancCapReg = &(chanHandle->baseAddress->CAPTURE[channelId]);
            /* Write Horizontal Ancillary Address Offset Register */
            setHancCapReg->HANCOFFSET = chanHandle->chVidParams.hancOffset;
            break;

        case Vpif_CHANNEL2_VIDEO:
        case Vpif_CHANNEL3_VIDEO:
            setHancDispChId = channelId - Vpif_DISPLAY_NUM_CHANNELS;
            setHancDisReg = &(chanHandle->baseAddress->DISPLAY[setHancDispChId]);
            /* Write Horizontal Ancillary Address Offset Register */
            setHancDisReg->HANCOFFSET = chanHandle->chVidParams.hancOffset;
            break;

        default:
            /* print error */
            break;
    }
}



/**
 *  \brief  Set selective RAW HBI display parameters.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifSetHbiDisplayParams(Vpif_ChanObj *chanHandle)
{
    Uint32 thancPos, bhancPos, thancSize, bhancSize;
    CSL_VpifDisRegsOvly setHbiDisReg;
    Vpif_RawSelectiveVbiParams *hbiparams;
    Uint8 setHancDispCh;

    assert(NULL != chanHandle);
    assert((Vpif_CHANNEL2_VIDEO == chanHandle->chanNum) ||
           (Vpif_CHANNEL3_VIDEO == chanHandle->chanNum));
    setHancDispCh = chanHandle->chanNum - Vpif_DISPLAY_NUM_CHANNELS;
    setHbiDisReg = \
        &(chanHandle->baseAddress->DISPLAY[setHancDispCh]);
    hbiparams = chanHandle->setSelHbiDispParams;
    assert(NULL != hbiparams);

    thancPos = CSL_VPIF_C2THANCPOS_HPOS_MASK & (hbiparams->vbi0StrtHps);
    thancPos |= (CSL_VPIF_C2THANCPOS_VPOS_MASK & ((hbiparams->vbi0StrtVps) <<
        CSL_VPIF_C2THANCPOS_VPOS_SHIFT));

    bhancPos = CSL_VPIF_C2BHANCPOS_HPOS_MASK & (hbiparams->vbi1StrtHps);
    bhancPos |= (CSL_VPIF_C2BHANCPOS_VPOS_MASK & ((hbiparams->vbi1StrtVps) <<
        CSL_VPIF_C2BHANCPOS_VPOS_SHIFT));

    thancSize = CSL_VPIF_C2THANCSIZE_HSIZE_MASK & (hbiparams->vbi0Hsz);
    thancSize |= (CSL_VPIF_C2THANCSIZE_VSIZE_MASK & ((hbiparams->vbi0Vsz) <<
        CSL_VPIF_C2THANCSIZE_VSIZE_SHIFT));

    bhancSize = CSL_VPIF_C2BHANCSIZE_HSIZE_MASK & (hbiparams->vbi1Hsz);
    bhancSize |= (CSL_VPIF_C2BHANCSIZE_VSIZE_MASK & ((hbiparams->vbi1Vsz) <<
        CSL_VPIF_C2BHANCSIZE_VSIZE_SHIFT));

    setHbiDisReg->THANCPOS = thancPos;
    setHbiDisReg->BHANCPOS = bhancPos;
    setHbiDisReg->THANCSIZE = thancSize;
    setHbiDisReg->BHANCSIZE = bhancSize;
}


/**
 *  \brief  Set selective RAW VBI display parameters.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifSetVbiDisplayParams(Vpif_ChanObj *chanHandle)
{
    Uint32 tvancPos, bvancPos, tvancSize, bvancSize;
    CSL_VpifDisRegsOvly setVbiDisReg;
    Vpif_RawSelectiveVbiParams *vbiparams;
    Uint8 setVbiDispCh;

    assert(NULL != chanHandle);
    assert((Vpif_CHANNEL2_VIDEO == chanHandle->chanNum) ||
           (Vpif_CHANNEL3_VIDEO == chanHandle->chanNum));
    setVbiDispCh = chanHandle->chanNum - Vpif_DISPLAY_NUM_CHANNELS;
    setVbiDisReg = \
        &(chanHandle->baseAddress->DISPLAY[setVbiDispCh]);
    vbiparams = chanHandle->setSelVbiDispParams;
    assert(NULL != vbiparams);

    tvancPos = CSL_VPIF_C2TVANCPOS_HPOS_MASK & (vbiparams->vbi0StrtHps);
    tvancPos |= (CSL_VPIF_C2TVANCPOS_VPOS_MASK & ((vbiparams->vbi0StrtVps) <<
        CSL_VPIF_C2TVANCPOS_VPOS_SHIFT));

    bvancPos = CSL_VPIF_C2BVANCPOS_HPOS_MASK & (vbiparams->vbi1StrtHps);
    bvancPos |= (CSL_VPIF_C2BVANCPOS_VPOS_MASK & ((vbiparams->vbi1StrtVps) <<
        CSL_VPIF_C2BVANCPOS_VPOS_SHIFT));

    tvancSize = CSL_VPIF_C2TVANCSIZE_HSIZE_MASK & (vbiparams->vbi0Hsz);
    tvancSize |= (CSL_VPIF_C2TVANCSIZE_VSIZE_MASK & ((vbiparams->vbi0Vsz) <<
        CSL_VPIF_C2TVANCSIZE_VSIZE_SHIFT));

    bvancSize = CSL_VPIF_C2BVANCSIZE_HSIZE_MASK & (vbiparams->vbi1Hsz);
    bvancSize |= (CSL_VPIF_C2BVANCSIZE_VSIZE_MASK & ((vbiparams->vbi1Vsz) <<
        CSL_VPIF_C2BVANCSIZE_VSIZE_SHIFT));

    setVbiDisReg->TVANCPOS = tvancPos;
    setVbiDisReg->BVANCPOS = bvancPos;
    setVbiDisReg->TVANCSIZE = tvancSize;
    setVbiDisReg->BVANCSIZE = bvancSize;
}

/**
 *  \brief  Enables ancillary data (HANC and VANC) in the CnCTRL control
 *          registers. Should be called only if need to enable RAW - HANC
 *          and RAW - VANC
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *  \param  enable           [IN]  Enable or disable the ANC data.
 *
 *  \return None
 *
 */
static Void vpifSetRawParams(Vpif_ChanObj *chanHandle, Bool enable)
{
    Uint32 setrawCtrlVal;
    assert(NULL != chanHandle);
    assert((0 != chanHandle->hbiBufSz) || (0 != chanHandle->vbiBufSz));

    setrawCtrlVal = 0;
    switch (chanHandle->chanNum)
    {
        case Vpif_CHANNEL0_VIDEO:
            setrawCtrlVal = chanHandle->baseAddress->C0CTRL;
            if (TRUE == enable)
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C0CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C0CTRL_VANC_SHIFT);
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C0CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C0CTRL_HANC_SHIFT);
                }
            }
            else
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C0CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C0CTRL_VANC_SHIFT));
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C0CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C0CTRL_HANC_SHIFT));
                }
            }
            chanHandle->baseAddress->C0CTRL = setrawCtrlVal;
            break;
        case Vpif_CHANNEL1_VIDEO:
            setrawCtrlVal = chanHandle->baseAddress->C1CTRL;
            if (TRUE == enable)
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C1CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C1CTRL_VANC_SHIFT);
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C1CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C1CTRL_HANC_SHIFT);
                }
            }
            else
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C1CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C1CTRL_VANC_SHIFT));
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C1CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C1CTRL_HANC_SHIFT));
                }
            }
            chanHandle->baseAddress->C1CTRL = setrawCtrlVal;
            break;
        case Vpif_CHANNEL2_VIDEO:
            setrawCtrlVal = chanHandle->baseAddress->C2CTRL;
            if (TRUE == enable)
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C2CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C2CTRL_VANC_SHIFT);
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C2CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C2CTRL_HANC_SHIFT);
                }
            }
            else
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C2CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C2CTRL_VANC_SHIFT));
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C2CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C2CTRL_HANC_SHIFT));
                }

            }
            chanHandle->baseAddress->C2CTRL = setrawCtrlVal;
            break;
        case Vpif_CHANNEL3_VIDEO:
            setrawCtrlVal = chanHandle->baseAddress->C3CTRL;
            if (TRUE == enable)
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C3CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C3CTRL_VANC_SHIFT);
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal |= ((CSL_VPIF_C3CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C3CTRL_HANC_SHIFT);
                }
            }
            else
            {
                if (0 != chanHandle->hbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C3CTRL_VANC_ENABLE) <<
                        CSL_VPIF_C3CTRL_VANC_SHIFT));
                }
                if (0 != chanHandle->vbiBufSz)
                {
                    setrawCtrlVal &= (~((CSL_VPIF_C3CTRL_HANC_ENABLE) <<
                        CSL_VPIF_C3CTRL_HANC_SHIFT));
                }
            }
            chanHandle->baseAddress->C3CTRL = setrawCtrlVal;
            break;
        default:
            /* print error */
            break;
    }
}



/**
 *  \brief  This function is used to set video parameters in VPIF register.
 *          It sets size parameters, frame format, yc mux mode etc.
 *          It mainly modifies the channel control register.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return Number of channel used
 *
 */
static Uint8 vpifSetVideoParams(Vpif_ChanObj *chanHandle)
{
    Uint8 start, end, i;
    Uint32 ctrlValue;
    Uint8 setVidDispCh;
    Uint8 found = 1u;

    assert(NULL != chanHandle);

    /*
     * In raw capture mode, VPIF automatically detects the incoming image size
     * by using the raw_h_valid and raw_v_valid signals. Thus, the CnHCFG,
     * CnVCFG0, CnVCFG1, and CnVSIZE registers are not required.
     */
    if (Vpif_CaptureFormat_CCDC != chanHandle->chVidParams.captureFormat)
    {
        /* Set the size parameters in the VPIF registers */
        vpifSetModeInfo(chanHandle, chanHandle->chanNum);

        if (Vpif_YCMuxed_NO == chanHandle->chVidParams.ycMuxMode)
        {
            /*
             * Set the same size parameters in the VPIF registers for the second
             * channel as it not yc mux mode.  YC are on separate channels
             * (HDTV formats)
             */
            vpifSetModeInfo(chanHandle, chanHandle->chanNum + 1u);
            found = 2u;
        }
    }
    else
    {
        found = 2u;
    }
    start = chanHandle->chanNum;
    end = chanHandle->chanNum + found;

    for (i = start; i < end; i++)
    {
        ctrlValue = 0;
        switch (i)
        {
            case Vpif_CHANNEL0_VIDEO:
                /* Set the frame format in the control register */
                if (Vpif_FrameFormat_PROGRESSIVE ==
                    chanHandle->chVidParams.frameFmt)
                {
                    /* Progressive Frame Format */
                    ctrlValue |= ((CSL_VPIF_C0CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C0CTRL_INTRPROG_SHIFT);
                }
                else
                {
                    /* Interlaced Frame Format */
                    ctrlValue &= (~((CSL_VPIF_C0CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C0CTRL_INTRPROG_SHIFT));
                }

                /* Set YC mux mode in the control register */
                if (Vpif_YCMuxed_YES == chanHandle->chVidParams.ycMuxMode)
                {
                    /* YC Mux mode */
                    ctrlValue |= ((CSL_VPIF_C0CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C0CTRL_YCMUX_SHIFT);
                }
                else
                {
                    /* YC Non Mux mode */
                    ctrlValue &= (~((CSL_VPIF_C0CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C0CTRL_YCMUX_SHIFT));
                }
                /* Set field/frame mode in control register */
                if (Vpif_SdramStorage_FIELD != chanHandle->frameStorageMode)
                {
                    ctrlValue |= ((CSL_VPIF_C0CTRL_FIELDFRAME_FRAME) <<
                        CSL_VPIF_C0CTRL_FIELDFRAME_SHIFT);/* frame mode */
                }
                else
                {
                    ctrlValue &= (~((CSL_VPIF_C0CTRL_FIELDFRAME_FRAME) <<
                        CSL_VPIF_C0CTRL_FIELDFRAME_SHIFT));/* field mode */
                }

                /* Set the capture format */
                if (Vpif_CaptureFormat_CCDC ==
                    chanHandle->chVidParams.captureFormat)
                {
                    ctrlValue = ((ctrlValue & ~CSL_VPIF_C0CTRL_CAPMODE_MASK) |
                     ((CSL_VPIF_C0CTRL_CAPMODE_CCDCMOS <<
                       CSL_VPIF_C0CTRL_CAPMODE_SHIFT) &
                       CSL_VPIF_C0CTRL_CAPMODE_MASK));
                    /* Set the polarity of various pins */
                    if (Vpif_RawCapturePinPol_INVERT ==
                        chanHandle->fieldPolarity)
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_FIDINV_ENABLE) <<
                            CSL_VPIF_C0CTRL_FIDINV_SHIFT);
                    }
                    else
                    {
                        ctrlValue &= (~((CSL_VPIF_C0CTRL_FIDINV_ENABLE) <<
                            CSL_VPIF_C0CTRL_FIDINV_SHIFT));
                    }
                    if (Vpif_RawCapturePinPol_INVERT == chanHandle->vPixelPol)
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_VVINV_ENABLE) <<
                            CSL_VPIF_C0CTRL_VVINV_SHIFT);
                    }
                    else
                    {
                       ctrlValue &= (~((CSL_VPIF_C0CTRL_VVINV_ENABLE) <<
                           CSL_VPIF_C0CTRL_VVINV_SHIFT));
                    }
                    if (Vpif_RawCapturePinPol_INVERT == chanHandle->hPixelPol)
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_HVINV_ENABLE) <<
                            CSL_VPIF_C0CTRL_HVINV_SHIFT);
                    }
                    else
                    {
                        ctrlValue &= (~((CSL_VPIF_C0CTRL_HVINV_ENABLE) <<
                            CSL_VPIF_C0CTRL_HVINV_SHIFT));
                    }
                    /* Set data width */
                    ctrlValue &= ~CSL_VPIF_C0CTRL_DATAWIDTH_MASK;
                    if (Vpif_RawCaptureDataWidth_8BITS == chanHandle->dataSz)
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_DATAWIDTH_8BIT) <<
                            CSL_VPIF_C0CTRL_DATAWIDTH_SHIFT);
                    }
                    else if (Vpif_RawCaptureDataWidth_10BITS ==
                             chanHandle->dataSz)
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_DATAWIDTH_10BIT) <<
                            CSL_VPIF_C0CTRL_DATAWIDTH_SHIFT);
                    }
                    else
                    {
                        ctrlValue |= ((CSL_VPIF_C0CTRL_DATAWIDTH_12BIT) <<
                            CSL_VPIF_C0CTRL_DATAWIDTH_SHIFT);
                    }
                }
                else
                {
                    ctrlValue = ((ctrlValue & ~CSL_VPIF_C0CTRL_CAPMODE_MASK) |
                     ((CSL_VPIF_C0CTRL_CAPMODE_BTYC <<
                       CSL_VPIF_C0CTRL_CAPMODE_SHIFT) &
                       CSL_VPIF_C0CTRL_CAPMODE_MASK));
                }
                chanHandle->baseAddress->C0CTRL = ctrlValue;
                /* Write the pitch in the driver */
                chanHandle->baseAddress->CAPTURE[i].IMGOFFSET = \
                    chanHandle->lineOffset;
                break;

            case Vpif_CHANNEL1_VIDEO:
                /* Set the frame format in the control register */
                if (Vpif_FrameFormat_PROGRESSIVE ==
                    chanHandle->chVidParams.frameFmt)
                {
                    /* Progressive Frame Format */
                    ctrlValue |= ((CSL_VPIF_C1CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C1CTRL_INTRPROG_SHIFT);
                }
                else
                {
                    /* Interlaced Frame Format */
                    ctrlValue &= (~((CSL_VPIF_C1CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C1CTRL_INTRPROG_SHIFT));
                }

                /* Set YC mux mode in the control register */
                if (Vpif_YCMuxed_YES == chanHandle->chVidParams.ycMuxMode)
                {
                    /* YC Mux mode */
                    ctrlValue |= ((CSL_VPIF_C1CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C1CTRL_YCMUX_SHIFT);
                }
                else
                {
                    /* YC Non Mux mode */
                    ctrlValue &= (~((CSL_VPIF_C1CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C1CTRL_YCMUX_SHIFT));
                }
                /* Set field/frame mode in control register */
                /* TRICKY : channel 1 can be used independently from channel 0
                for SD, but the channel 1 modes will be dictated by the channel
                0 settings.  So whatever FIELDFRAME mode is selected for channel
                0 will apply to channel 1 as well.  This is true even when
                channel 0 is not used. */
                if (Vpif_SdramStorage_FIELD != chanHandle->frameStorageMode)
                {
                    chanHandle->baseAddress->C0CTRL |= \
                        ((CSL_VPIF_C0CTRL_FIELDFRAME_FRAME) <<
                         CSL_VPIF_C0CTRL_FIELDFRAME_SHIFT);/* frame mode */
                }
                else
                {
                    chanHandle->baseAddress->C0CTRL &= \
                        (~((CSL_VPIF_C0CTRL_FIELDFRAME_FRAME) <<
                         CSL_VPIF_C0CTRL_FIELDFRAME_SHIFT));/* field mode */
                }

                /* Set the capture format */
                if (Vpif_CaptureFormat_CCDC ==
                    chanHandle->chVidParams.captureFormat)
                {
                    ctrlValue = ((ctrlValue & ~CSL_VPIF_C1CTRL_CAPMODE_MASK) |
                     ((CSL_VPIF_C1CTRL_CAPMODE_CCDCMOS <<
                       CSL_VPIF_C1CTRL_CAPMODE_SHIFT) &
                       CSL_VPIF_C1CTRL_CAPMODE_MASK));
                }
                else
                {
                    ctrlValue = ((ctrlValue & ~CSL_VPIF_C1CTRL_CAPMODE_MASK) |
                     ((CSL_VPIF_C1CTRL_CAPMODE_BTYC <<
                       CSL_VPIF_C1CTRL_CAPMODE_SHIFT) &
                       CSL_VPIF_C1CTRL_CAPMODE_MASK));
                }
                chanHandle->baseAddress->C1CTRL = ctrlValue;
                /* Write the pitch in the driver */
                chanHandle->baseAddress->CAPTURE[i].IMGOFFSET = \
                    chanHandle->lineOffset;
                break;

            case Vpif_CHANNEL2_VIDEO:
                /* Set the frame format in the control register */
                if (Vpif_FrameFormat_PROGRESSIVE ==
                    chanHandle->chVidParams.frameFmt)
                {
                    /* Progressive Frame Format */
                    ctrlValue |= ((CSL_VPIF_C2CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C2CTRL_INTRPROG_SHIFT);
                }
                else
                {
                    /* Interlaced Frame Format */
                    ctrlValue &= (~((CSL_VPIF_C2CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C2CTRL_INTRPROG_SHIFT));
                }

                /* Set YC mux mode in the control register */
                if (Vpif_YCMuxed_YES == chanHandle->chVidParams.ycMuxMode)
                {
                    /* YC Mux mode */
                    ctrlValue |= ((CSL_VPIF_C2CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C2CTRL_YCMUX_SHIFT);
                }
                else
                {
                    /* YC Non Mux mode */
                    ctrlValue &= (~((CSL_VPIF_C2CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C2CTRL_YCMUX_SHIFT));
                }
                /* Set field/frame mode in control register */
                if (Vpif_SdramStorage_FIELD != chanHandle->frameStorageMode)
                {
                    ctrlValue |= ((CSL_VPIF_C2CTRL_FIELDFRAME_FRAME) <<
                        CSL_VPIF_C2CTRL_FIELDFRAME_SHIFT);/* frame mode */
                }
                else
                {
                    ctrlValue &= (~((CSL_VPIF_C2CTRL_FIELDFRAME_FRAME) <<
                        CSL_VPIF_C2CTRL_FIELDFRAME_SHIFT));/* field mode */
                }

                /* Set the Pixel enable bit */
                ctrlValue |= ((CSL_VPIF_C2CTRL_PIXEL_ENABLE) <<
                    CSL_VPIF_C2CTRL_PIXEL_SHIFT);
                chanHandle->baseAddress->C2CTRL = ctrlValue;
                /* Write the pitch in the driver */
                setVidDispCh = i - Vpif_DISPLAY_NUM_CHANNELS;
                chanHandle->baseAddress->DISPLAY[setVidDispCh].IMGOFFSET = \
                    chanHandle->lineOffset;
                break;

            case Vpif_CHANNEL3_VIDEO:

                /* Set the frame format in the control register */
                if (Vpif_FrameFormat_PROGRESSIVE ==
                    chanHandle->chVidParams.frameFmt)
                {
                    /* Progressive Frame Format */
                    ctrlValue |= ((CSL_VPIF_C3CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C3CTRL_INTRPROG_SHIFT);
                }
                else
                {
                    /* Interlaced Frame Format */
                    ctrlValue &= (~((CSL_VPIF_C3CTRL_INTRPROG_PROGRESSIVE) <<
                        CSL_VPIF_C3CTRL_INTRPROG_SHIFT));
                }

                /* Set YC mux mode in the control register */
                if (Vpif_YCMuxed_YES == chanHandle->chVidParams.ycMuxMode)
                {
                    /* YC Mux mode */
                    ctrlValue |= ((CSL_VPIF_C3CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C3CTRL_YCMUX_SHIFT);
                }
                else
                {
                    /* YC Non Mux mode */
                    ctrlValue &= (~((CSL_VPIF_C3CTRL_YCMUX_ENABLE) <<
                        CSL_VPIF_C3CTRL_YCMUX_SHIFT));
                }
                /* Set field/frame mode in control register */
                /* TRICKY : channel 3 can be used independently from channel 2
                for SD, but the channel 3 modes will be dictated by the channel
                2 settings.  So whatever FIELDFRAME mode is selected for channel
                2 will apply to channel 3 as well.  This is true even when
                channel 2 is not used. */
                if (Vpif_SdramStorage_FIELD != chanHandle->frameStorageMode)
                {
                    chanHandle->baseAddress->C2CTRL |= \
                        ((CSL_VPIF_C2CTRL_FIELDFRAME_FRAME) <<
                         CSL_VPIF_C2CTRL_FIELDFRAME_SHIFT);/* frame mode */
                }
                else
                {
                    chanHandle->baseAddress->C2CTRL &= \
                        (~((CSL_VPIF_C2CTRL_FIELDFRAME_FRAME) <<
                         CSL_VPIF_C2CTRL_FIELDFRAME_SHIFT));/* field mode */
                }
                /* Set the Pixel enable bit */
                ctrlValue |= ((CSL_VPIF_C3CTRL_PIXEL_ENABLE) <<
                    CSL_VPIF_C3CTRL_PIXEL_SHIFT);
                chanHandle->baseAddress->C3CTRL = ctrlValue;
                /* Write the pitch in the driver */
                setVidDispCh = i - Vpif_DISPLAY_NUM_CHANNELS;
                chanHandle->baseAddress->DISPLAY[setVidDispCh].IMGOFFSET = \
                    chanHandle->lineOffset;
                break;

            default:
                /* print error */
                break;
        }
    }

    chanHandle->baseAddress->EMUCTRL = CSL_VPIF_EMUCTRL_FREE_FREE;
    return found;
}


/**
 *  \brief  This function is used to set horizontal and vertical config
 *          parameters in VPIF registers As per the standard in the channel,
 *          configure the values of L1, L3, L5, L7  L9, L11 in VPIF Register,
 *          also write width and height.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *  \param  channelId        [IN]  Channel Id
 *
 *  \return None
 *
 */
static Void vpifSetModeInfo(Vpif_ChanObj *chanHandle, Uint8 channelId)
{
    Uint32              modeValue = 0;
    Vpif_Object         *setModeInstHandle = NULL;
    CSL_VpifCapRegsOvly setModeCapReg;
    CSL_VpifDisRegsOvly setModeDisReg;
    volatile CSL_VpifRegs *setModeRegs = NULL;

    assert(NULL != chanHandle);

    VPIF_DEBUG("Entered ---> vpifSetModeInfo\n");

    setModeInstHandle = (Vpif_Object *)chanHandle->devHandle;
    assert(NULL != setModeInstHandle);
    setModeRegs = setModeInstHandle->chanObj[channelId].baseAddress;
    assert(NULL != setModeRegs);
    /* As per the standard in the channel, configure the values of L1, L3,
       L5, L7  L9, L11 in VPIF Register */

    /* TRICKY : The channel handle is not taken from the chan param
    but passed this as an individual parameter.
    As channel id can also be derived from channel handle but here channel
    id is also passed as a parameter as when it is not yc mux mode then these
    parameters are also need to be updated for the second channel using the
    parameters of first channel pointer.*/
    switch (channelId)
    {
        case Vpif_CHANNEL0_VIDEO:
        case Vpif_CHANNEL1_VIDEO:
            setModeCapReg = \
                &(setModeRegs->CAPTURE[channelId]);
            /* Write width of the image */
            modeValue = (((Uint32)chanHandle->chVidParams.eav2sav <<
                CSL_VPIF_C0HCFG_EAV2SAV_SHIFT) & CSL_VPIF_C0HCFG_EAV2SAV_MASK);
            modeValue |= (chanHandle->chVidParams.sav2eav &
                CSL_VPIF_C0HCFG_SAV2EAV_MASK);
            setModeCapReg->HCFG = modeValue;

            /* Write the L1 and L3 parameters in VPIF register */
            modeValue = (((Uint32)chanHandle->chVidParams.l1 <<
                CSL_VPIF_C0VCFG0_L1_SHIFT) & CSL_VPIF_C0VCFG0_L1_MASK);
            modeValue |= (chanHandle->chVidParams.l3 & CSL_VPIF_C0VCFG0_L3_MASK);
            setModeCapReg->VCFG0 = modeValue;
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                /* Write the L5 and L7 parameters in VPIF register */
                modeValue = (((Uint32)chanHandle->chVidParams.l5 <<
                    CSL_VPIF_C0VCFG1_L5_SHIFT) & CSL_VPIF_C0VCFG1_L5_MASK);
                modeValue |= (chanHandle->chVidParams.l7 &
                    CSL_VPIF_C0VCFG1_L7_MASK);
                setModeCapReg->VCFG1 = modeValue;

                /* Write the L9 and L11 parameters in VPIF register */
                modeValue = (((Uint32)chanHandle->chVidParams.l9 <<
                    CSL_VPIF_C0VCFG2_L9_SHIFT) & CSL_VPIF_C0VCFG2_L9_MASK);
                modeValue |= (chanHandle->chVidParams.l11 &
                    CSL_VPIF_C0VCFG2_L11_MASK);
                setModeCapReg->VCFG2 = modeValue;
            }
            else
            {
                /* Write the L5 parameters in VPIF register. The CnVCFG2
                registers are not used with progressive video mode. Note that L7
                in CnVCFG1 is not used with the progressive video mode */
                modeValue = 0;
                modeValue = (((Uint32)chanHandle->chVidParams.l5 <<
                    CSL_VPIF_C0VCFG1_L5_SHIFT) & CSL_VPIF_C0VCFG1_L5_MASK);
                setModeCapReg->VCFG1 = modeValue;
            }

            /* Write the image height in VPIF register */
            modeValue = (chanHandle->chVidParams.vsize &
                CSL_VPIF_C0VSIZE_VSIZE_MASK);
            setModeCapReg->VSIZE = modeValue;
            break;

        case Vpif_CHANNEL2_VIDEO:
        case Vpif_CHANNEL3_VIDEO:
            setModeDisReg = \
                &(setModeRegs->DISPLAY[channelId - Vpif_DISPLAY_NUM_CHANNELS]);
            /* Write width of the image */
            modeValue = (((Uint32)chanHandle->chVidParams.eav2sav <<
                CSL_VPIF_C2HCFG_EAV2SAV_SHIFT) & CSL_VPIF_C2HCFG_EAV2SAV_MASK);
            modeValue |= (chanHandle->chVidParams.sav2eav &
                CSL_VPIF_C2HCFG_SAV2EAV_MASK);
            setModeDisReg->HCFG = modeValue;

            /* Write the L1 and L3 parameters in VPIF register */
            modeValue = (((Uint32)chanHandle->chVidParams.l1 <<
                CSL_VPIF_C2VCFG0_L1_SHIFT) & CSL_VPIF_C2VCFG0_L1_MASK);
            modeValue |= (chanHandle->chVidParams.l3 & CSL_VPIF_C2VCFG0_L3_MASK);
            setModeDisReg->VCFG0 = modeValue;

            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                /* Write the L5 and L7 parameters in VPIF register */
                modeValue = (((Uint32)chanHandle->chVidParams.l5 <<
                    CSL_VPIF_C2VCFG1_L5_SHIFT) & CSL_VPIF_C2VCFG1_L5_MASK);
                modeValue |= (chanHandle->chVidParams.l7 &
                    CSL_VPIF_C2VCFG1_L7_MASK);
                setModeDisReg->VCFG1 = modeValue;

                /* Write the L9 and L11 parameters in VPIF register */
                modeValue = (((Uint32)chanHandle->chVidParams.l9 <<
                    CSL_VPIF_C2VCFG2_L9_SHIFT) & CSL_VPIF_C2VCFG2_L9_MASK);
                modeValue |= (chanHandle->chVidParams.l11 &
                    CSL_VPIF_C2VCFG2_L11_MASK);
                setModeDisReg->VCFG2 = modeValue;
            }
            else
            {
                /* Write the L5 parameters in VPIF register. The CnVCFG2
                registers are not used with progressive video mode. Note that L7
                in CnVCFG1 is not used with the progressive video mode*/
                modeValue = 0;
                modeValue = (((Uint32)chanHandle->chVidParams.l5 <<
                    CSL_VPIF_C2VCFG1_L5_SHIFT) & CSL_VPIF_C2VCFG1_L5_MASK);
                setModeDisReg->VCFG1 = modeValue;
            }

            /* Write the image height in VPIF register */
            modeValue = \
                (chanHandle->chVidParams.vsize & CSL_VPIF_C2VSIZE_VSIZE_MASK);
            setModeDisReg->VSIZE = modeValue;
            break;

        default:
            /* print error */
            break;
    }
}


/**
 *  \brief  Configures the VPIF video registers with the frame buffer address.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifConfigureAddress(Vpif_ChanObj *chanHandle)
{
    Uint8 confChanId;
    Uint8 confDispCh;
    CSL_VpifCapRegsOvly confCapReg;
    CSL_VpifDisRegsOvly confDisReg;
    volatile CSL_VpifRegs *confVpifRegs = NULL;

    VPIF_DEBUG("Entered ---> vpifConfigureAddress\n");

    assert(NULL != chanHandle);
    confChanId = chanHandle->chanNum;
    if ((TRUE == chanHandle->useTwoChannels) &&
        ((confChanId == Vpif_CHANNEL1_VIDEO) ||
        (confChanId == Vpif_CHANNEL3_VIDEO)))
    {
        assert(FALSE);
    }

    confVpifRegs = chanHandle->baseAddress;
    assert(NULL != confVpifRegs);
    switch (confChanId)
    {
        case Vpif_CHANNEL0_VIDEO:
        case Vpif_CHANNEL1_VIDEO:
            /* C0TCHROMA, C0BCHROMA, C1TLUMA and C1BLUMA registers are only used
            in Y/C multiplexed mode.*/
            if (TRUE == chanHandle->useTwoChannels)
            {
                assert(Vpif_CHANNEL1_VIDEO != confChanId);
                if (Vpif_CaptureFormat_CCDC ==
                    chanHandle->chVidParams.captureFormat)
                {
                    confCapReg = \
                        &(confVpifRegs->CAPTURE[Vpif_CHANNEL0_VIDEO]);
                    if (Vpif_FrameFormat_INTERLACED ==
                        chanHandle->chVidParams.frameFmt)
                    {
                        /*
                         * Set buffer addresses in case of CCD mode. TLUMA is
                         * used for SDRAM start address of top field and BLUMA
                         * is used for SDRAM start address of bottom field in
                         * raw data capture mode.
                         */
                        confCapReg->TLUMA = \
                            (Uint32)chanHandle->curViop->frame.riFrm.buf1;
                        confCapReg->BLUMA = \
                            (Uint32)chanHandle->curViop->frame.riFrm.buf2;
                    }
                    else
                    {
                        confCapReg->TLUMA = \
                            (Uint32)chanHandle->curViop->frame.rpFrm.buf;
                    }
                }
                else
                {
                    /* set buffer addresses in case of Y/C non mux mode */
                    if (Vpif_FrameFormat_INTERLACED ==
                        chanHandle->chVidParams.frameFmt)
                    {
                        confCapReg = \
                            &(confVpifRegs->CAPTURE[Vpif_CHANNEL0_VIDEO]);
                        confCapReg->TLUMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.y1;
                        confCapReg->BLUMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.y2;
                        confCapReg = \
                            &(confVpifRegs->CAPTURE[Vpif_CHANNEL1_VIDEO]);
                        confCapReg->TCHROMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.c1;
                        confCapReg->BCHROMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.c2;
                    }
                    else
                    {
                        /* CnBLUMA  and CnBCHROMA registers are not used in
                        progressive video mode. */
                        confCapReg = \
                            &(confVpifRegs->CAPTURE[Vpif_CHANNEL0_VIDEO]);
                        confCapReg->TLUMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.y1;
                        confCapReg = \
                            &(confVpifRegs->CAPTURE[Vpif_CHANNEL1_VIDEO]);
                        confCapReg->TCHROMA = \
                            (Uint32)chanHandle->curViop->frame.spFrm.c1;
                    }
                }
            }
            else
            {
                confCapReg = &(confVpifRegs->CAPTURE[confChanId]);
                /* CnBLUMA  and CnBCHROMA registers are not used in progressive
                video mode. */
                confCapReg->TLUMA = \
                    (Uint32)chanHandle->curViop->frame.spFrm.y1;
                confCapReg->TCHROMA = \
                    (Uint32)chanHandle->curViop->frame.spFrm.c1;
                if (Vpif_FrameFormat_INTERLACED ==
                    chanHandle->chVidParams.frameFmt)
                {
                    confCapReg->BLUMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.y2;
                    confCapReg->BCHROMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.c2;
                }
            }
            break;

        case Vpif_CHANNEL2_VIDEO:
        case Vpif_CHANNEL3_VIDEO:
            /* C2BCHROMA, C2TCHROMA, C3BLUMA and C3TLUMA registers are only used
            in Y/C multiplexed mode.*/
            if (TRUE == chanHandle->useTwoChannels)
            {
                assert(Vpif_CHANNEL1_VIDEO != confChanId);
                /* set buffer addresses in case of Y/C non mux mode */
                if (Vpif_FrameFormat_INTERLACED ==
                    chanHandle->chVidParams.frameFmt)
                {
                    confDispCh = Vpif_CHANNEL2_VIDEO - Vpif_DISPLAY_NUM_CHANNELS;
                    confDisReg = \
                        &(confVpifRegs->DISPLAY[confDispCh]);
                    confDisReg->TLUMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.y1;
                    confDisReg->BLUMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.y2;
                    confDispCh = Vpif_CHANNEL3_VIDEO - Vpif_DISPLAY_NUM_CHANNELS;
                    confDisReg = \
                        &(confVpifRegs->DISPLAY[confDispCh]);
                    confDisReg->TCHROMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.c1;
                    confDisReg->BCHROMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.c2;
                }
                else
                {
                    confDispCh = Vpif_CHANNEL2_VIDEO - Vpif_DISPLAY_NUM_CHANNELS;
                    /* CnBLUMA  and CnBCHROMA registers are not used in
                    progressive video mode. */
                    confDisReg = \
                        &(confVpifRegs->DISPLAY[confDispCh]);
                    confDisReg->TLUMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.y1;
                    confDispCh = Vpif_CHANNEL3_VIDEO - Vpif_DISPLAY_NUM_CHANNELS;
                    confDisReg = \
                        &(confVpifRegs->DISPLAY[confDispCh]);
                    confDisReg->TCHROMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.c1;

                }
            }
            else
            {
                confDispCh = confChanId - Vpif_DISPLAY_NUM_CHANNELS;
                confDisReg = \
                    &(confVpifRegs->DISPLAY[confDispCh]);
                /* CnBLUMA  and CnBCHROMA registers are not used in progressive
                video mode. */
                confDisReg->TLUMA = \
                    (Uint32)chanHandle->curViop->frame.spFrm.y1;
                confDisReg->TCHROMA = \
                    (Uint32)chanHandle->curViop->frame.spFrm.c1;
                if (Vpif_FrameFormat_INTERLACED ==
                    chanHandle->chVidParams.frameFmt)
                {
                    confDisReg->BLUMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.y2;
                    confDisReg->BCHROMA = \
                        (Uint32)chanHandle->curViop->frame.spFrm.c2;
                }
            }
            break;

        default:
            /* print error */
            break;
    }
    VPIF_DEBUG("Exited ---> vpifConfigureAddress\n");
}


/**
 *  \brief  Configures the VPIF raw VBI registers with the frame buffer address.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifConfigureVbiAddress(Vpif_ChanObj *chanHandle)
{
    Uint8 confVbiChanId;
    Uint8 confVbiDispCh;
    CSL_VpifCapRegsOvly confVbiCapReg;
    CSL_VpifDisRegsOvly confVbiDisReg;

    VPIF_DEBUG("Entered ---> vpifConfigureVbiAddress\n");

    assert(NULL != chanHandle);
    confVbiChanId = chanHandle->chanNum;
    switch (confVbiChanId)
    {
        case Vpif_CHANNEL0_VIDEO:
        case Vpif_CHANNEL1_VIDEO:
            confVbiCapReg = &(chanHandle->baseAddress->CAPTURE[confVbiChanId]);
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                confVbiCapReg->TVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v1;
                confVbiCapReg->BVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v2;
            }
            else
            {
                /* CnBVANC registers are not used in progressive video mode. */
                confVbiCapReg->TVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v1;
            }
            break;

        case Vpif_CHANNEL2_VIDEO:
        case Vpif_CHANNEL3_VIDEO:
            confVbiDispCh = confVbiChanId - Vpif_DISPLAY_NUM_CHANNELS;
            confVbiDisReg = \
                &(chanHandle->baseAddress->DISPLAY[confVbiDispCh]);
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                confVbiDisReg->TVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v1;
                confVbiDisReg->BVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v2;
            }
            else
            {
                /* CnBVANC registers are not used in progressive video mode. */
                confVbiDisReg->TVANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.v1;
            }
            break;

        default:
            /* print error */
            break;
    }
    VPIF_DEBUG("Exited ---> vpifConfigureVbiAddress\n");
}

/**
 *  \brief  Configures the VPIF raw HBI registers with the frame buffer address.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifConfigureHbiAddress(Vpif_ChanObj *chanHandle)
{
    Uint8 confHbiChanId;
    Uint8 confHbiDispCh;
    CSL_VpifCapRegsOvly confHbiCapReg;
    CSL_VpifDisRegsOvly confHbiDisReg;

    VPIF_DEBUG("Entered ---> vpifConfigureHbiAddress\n");

    assert(NULL != chanHandle);
    confHbiChanId = chanHandle->chanNum;
    switch (confHbiChanId)
    {
        case Vpif_CHANNEL0_VIDEO:
        case Vpif_CHANNEL1_VIDEO:
            confHbiCapReg = &(chanHandle->baseAddress->CAPTURE[confHbiChanId]);
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                confHbiCapReg->THANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h1;
                confHbiCapReg->BHANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h2;
            }
            else
            {
                /* CnBHANC registers are not used in progressive video mode. */
                confHbiCapReg->THANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h1;
            }
            break;

        case Vpif_CHANNEL2_VIDEO:
        case Vpif_CHANNEL3_VIDEO:
            confHbiDispCh = confHbiChanId - Vpif_DISPLAY_NUM_CHANNELS;
            confHbiDisReg = \
                &(chanHandle->baseAddress->DISPLAY[confHbiDispCh]);
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                confHbiDisReg->THANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h1;
                confHbiDisReg->BHANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h2;
            }
            else
            {
                /* CnBHANC registers are not used in progressive video mode. */
                confHbiDisReg->THANC = \
                    (Uint32)chanHandle->curViop->vbiFrm.h1;
            }
            break;

        default:
            /* print error */
            break;
    }
    VPIF_DEBUG("Exited ---> vpifConfigureHbiAddress\n");
}

/**
 *  \brief  Starts the VPIF transfer for the channel.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return IOM_COMPLETED       If success.
 *          IOM_EBADIO          If channel is already started or start of
 *                              channel is not possible.
 *
 */
static Int32 vpifStartTransfer(Vpif_ChanObj *chanHandle)
{
    Int32        strtTfrRtnVal = IOM_COMPLETED;
    Vpif_Object  *strtTfrInstHandle = NULL;
    Uint32       strtKey = 0;
    Uint8        ret;

    VPIF_DEBUG("Entered ---> vpifStartTransfer\n");

    assert(NULL != chanHandle);
    strtTfrInstHandle = (Vpif_Object *)chanHandle->devHandle;
    assert(NULL != strtTfrInstHandle);
    assert(0 == chanHandle->started);/* Streaming is started */
    if (Vpif_IoMode_DIS == chanHandle->channelMode)
    {
        if ((((chanHandle->chanNum == Vpif_CHANNEL2_VIDEO) &&
             (1 == strtTfrInstHandle->chanObj[Vpif_CHANNEL3_VIDEO].started)) &&
             (chanHandle->chVidParams.ycMuxMode == Vpif_YCMuxed_NO)) ||
            ((chanHandle->chanNum == Vpif_CHANNEL3_VIDEO) &&
             (2 == strtTfrInstHandle->chanObj[Vpif_CHANNEL2_VIDEO].started)))
        {
            DBG_PRINT_ERR("other channel is using\n");
            strtTfrRtnVal = IOM_EBADIO;
        }
    }
    else/* Capture */
    {
        if ((((chanHandle->chanNum == Vpif_CHANNEL0_VIDEO) &&
             (1 == strtTfrInstHandle->chanObj[Vpif_CHANNEL1_VIDEO].started)) &&
             (chanHandle->chVidParams.ycMuxMode == Vpif_YCMuxed_NO)) ||
            ((chanHandle->chanNum == Vpif_CHANNEL1_VIDEO) &&
             (2 == strtTfrInstHandle->chanObj[Vpif_CHANNEL0_VIDEO].started)))
        {
            DBG_PRINT_ERR("other channel is using\n");
            strtTfrRtnVal = IOM_EBADIO;
        }
    }

    if ((IOM_COMPLETED == strtTfrRtnVal) &&
        (TRUE == strtTfrInstHandle->isPwrAware))
    {
        /*Enable LPSC of VPIF if not already done */
        strtTfrRtnVal = vpifLpscOn(strtTfrInstHandle);
    }

    if (IOM_COMPLETED == strtTfrRtnVal)
    {
        /* Initialize fieldId and started member */
        chanHandle->fieldId = 0;
        chanHandle->started = 1u;

        /* Call vpifSetVideoParams function to set the parameters and
        addresses */
        ret = vpifSetVideoParams(chanHandle);
        chanHandle->started = ret;

        if (2u == ret)
        {
            chanHandle->useTwoChannels = TRUE;
        }
        else
        {
            chanHandle->useTwoChannels = FALSE;
        }
        vpifConfigureAddress(chanHandle);
        if ((0 != chanHandle->hbiBufSz) || (0 != chanHandle->vbiBufSz))
        {
            vpifSetHancAddressOffset(chanHandle);
            /* Enable ANC data in control reg */
            vpifSetRawParams(chanHandle, TRUE);
            /* Calculate the offset for vbi/hbi data in the buffer */
            if (0 != chanHandle->hbiBufSz)
            {
                /* Call vpifSetHbiDisplayParams function to set selective RAW
                   HBI parameters for display */
                if (NULL != chanHandle->setSelHbiDispParams)
                {
                    vpifSetHbiDisplayParams(chanHandle);
                }
                /* Set HBI address */
                vpifConfigureHbiAddress(chanHandle);
            }
            if (0 != chanHandle->vbiBufSz)
            {
                /* Call vpifSetVbiDisplayParams function to set selective
                   RAW VBI parameters for display */
                if (NULL != chanHandle->setSelVbiDispParams)
                {
                    vpifSetVbiDisplayParams(chanHandle);
                }
                /* Set VBI address */
                vpifConfigureVbiAddress(chanHandle);
            }
        }

        vpifSetChannelFrameInterrupt(chanHandle);

        /* Enter critical section                                         */
        strtKey = (Uint32)_disable_interrupts();
        vpifChannelIntrEnable(chanHandle, TRUE);
        vpifEnableChannel(chanHandle, TRUE);
        /* Exit critical section                                          */
        _restore_interrupts(strtKey);
        channelFirstInt[chanHandle->chanNum] = 1u;
        /* Set state as start operation done successfully */
        chanHandle->chState |= Vpif_DriverState_STARTED;
    }

    return strtTfrRtnVal;
}

/**
 *  \brief  Set the channel frame or field interrupt
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void vpifSetChannelFrameInterrupt(Vpif_ChanObj *chanHandle)
{
    volatile CSL_VpifRegsOvly vpifSetChRegs;

    assert(NULL != chanHandle);
    vpifSetChRegs = chanHandle->baseAddress;
    assert(NULL != vpifSetChRegs);

    if (Vpif_FrameFormat_PROGRESSIVE == chanHandle->chVidParams.frameFmt)
    {
        /* Set interrupt for TOP field in VPIF channel control register */
        if (Vpif_CHANNEL0_VIDEO == chanHandle->chanNum)
        {
            /* enabled interrupt on top field on ch0 ctrl register */
            vpifSetChRegs->C0CTRL = \
                ((vpifSetChRegs->C0CTRL & ~CSL_VPIF_C0CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C0CTRL_INTFRAME_TOP <<
                   CSL_VPIF_C0CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C0CTRL_INTFRAME_MASK));
        }

        if ((Vpif_IoMode_CAP == chanHandle->channelMode) &&
            ((Vpif_CHANNEL1_VIDEO == chanHandle->chanNum) ||
            (chanHandle->started == 2)))
        {
            /* enabled interrupt on top field on ch1 ctrl register */
            vpifSetChRegs->C1CTRL = \
                ((vpifSetChRegs->C1CTRL & ~CSL_VPIF_C1CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C1CTRL_INTFRAME_TOP <<
                   CSL_VPIF_C1CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C1CTRL_INTFRAME_MASK));
        }

        if (Vpif_CHANNEL2_VIDEO == chanHandle->chanNum)
        {
            /* enabled interrupt on top field on ch2 ctrl register */
            vpifSetChRegs->C2CTRL = \
                ((vpifSetChRegs->C2CTRL & ~CSL_VPIF_C2CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C2CTRL_INTFRAME_TOP <<
                   CSL_VPIF_C2CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C2CTRL_INTFRAME_MASK));
        }

        if ((Vpif_IoMode_DIS == chanHandle->channelMode) &&
            ((Vpif_CHANNEL3_VIDEO == chanHandle->chanNum) ||
            (chanHandle->started == 2)))
        {
            /* enabled interrupt on top field on ch3 ctrl register */
            vpifSetChRegs->C3CTRL = \
                ((vpifSetChRegs->C3CTRL & ~CSL_VPIF_C3CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C3CTRL_INTFRAME_TOP <<
                   CSL_VPIF_C3CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C3CTRL_INTFRAME_MASK));
        }
    }
    else
    {
        /* Set interrupt for both the fields in VPIF channel control register */
        if (Vpif_CHANNEL0_VIDEO == chanHandle->chanNum)
        {
            /* enabled interrupt on both the fields on ch0 ctrl register */
            vpifSetChRegs->C0CTRL = \
                ((vpifSetChRegs->C0CTRL & ~CSL_VPIF_C0CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C0CTRL_INTFRAME_BOTH <<
                   CSL_VPIF_C0CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C0CTRL_INTFRAME_MASK));
        }

        if ((Vpif_IoMode_CAP == chanHandle->channelMode) &&
            ((Vpif_CHANNEL1_VIDEO == chanHandle->chanNum) ||
            (chanHandle->started == 2)))
        {
            /* enabled interrupt on both the fields on ch1 ctrl register */
            vpifSetChRegs->C1CTRL = \
                ((vpifSetChRegs->C1CTRL & ~CSL_VPIF_C1CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C1CTRL_INTFRAME_BOTH <<
                   CSL_VPIF_C1CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C1CTRL_INTFRAME_MASK));
        }

        if (Vpif_CHANNEL2_VIDEO == chanHandle->chanNum)
        {
            /* enabled interrupt on both the fields on ch2 ctrl register */
            vpifSetChRegs->C2CTRL = \
                ((vpifSetChRegs->C2CTRL & ~CSL_VPIF_C2CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C2CTRL_INTFRAME_BOTH <<
                   CSL_VPIF_C2CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C2CTRL_INTFRAME_MASK));
        }

        if ((Vpif_IoMode_DIS == chanHandle->channelMode) &&
            ((Vpif_CHANNEL3_VIDEO == chanHandle->chanNum) ||
            (chanHandle->started == 2)))
        {
            /* enabled interrupt on both the fields on ch3 ctrl register */
            vpifSetChRegs->C3CTRL = \
                ((vpifSetChRegs->C3CTRL & ~CSL_VPIF_C3CTRL_INTFRAME_MASK) |
                 ((CSL_VPIF_C3CTRL_INTFRAME_BOTH <<
                   CSL_VPIF_C3CTRL_INTFRAME_SHIFT) &
                  CSL_VPIF_C3CTRL_INTFRAME_MASK));
        }
    }
}


/**
 *  \brief  Enable and disable interrupt for the channel.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *  \param  enable           [IN]  Enable or disable the interrupt
 *
 *  \return None
 *
 */
static Void vpifChannelIntrEnable(Vpif_ChanObj *chanHandle, Bool enable)
{
    volatile CSL_VpifRegsOvly vpifRegs;
    Uint32 chIntStatClrVal = 0;
    Uint32 chIntSetVal = 0;
    Uint32 chIntClrVal = 0;
    Uint32 chIntEnVal = 0;

    assert(NULL != chanHandle);
    vpifRegs = chanHandle->baseAddress;
    assert(NULL != vpifRegs);

    switch (chanHandle->chanNum)
    {
        case Vpif_CHANNEL0_VIDEO:
            chIntEnVal = CSL_VPIF_INTEN_FRAME0_ENABLE;
            chIntSetVal = CSL_VPIF_INTSET_FRAME0_ENABLE;
            chIntClrVal = CSL_VPIF_INTCLR_FRAME0_CLR;
            chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME0_CLR;
            break;
        case Vpif_CHANNEL1_VIDEO:
            chIntEnVal = CSL_VPIF_INTEN_FRAME1_ENABLE;
            chIntSetVal = CSL_VPIF_INTSET_FRAME1_ENABLE;
            chIntClrVal = CSL_VPIF_INTCLR_FRAME1_CLR;
            chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME1_CLR;
            break;
        case Vpif_CHANNEL2_VIDEO:
            chIntEnVal = CSL_VPIF_INTEN_FRAME2_ENABLE;
            chIntSetVal = CSL_VPIF_INTSET_FRAME2_ENABLE;
            chIntClrVal = CSL_VPIF_INTCLR_FRAME2_CLR;
            chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME2_CLR;
            break;
        case Vpif_CHANNEL3_VIDEO:
            chIntEnVal = CSL_VPIF_INTEN_FRAME3_ENABLE;
            chIntSetVal = CSL_VPIF_INTSET_FRAME3_ENABLE;
            chIntClrVal = CSL_VPIF_INTCLR_FRAME3_CLR;
            chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME3_CLR;
            break;
        default:
            /* print error */
            break;
    }

/*  Interrupt register definition:
    INTEN(R/W)    : To activate frame level interrupt (1 pulse per 1 frame)
                    from this module, you should configure this bit to 1, and
                    then you should also configure proper bit of INTSET register
                    to 1.
    INTSET(R/W)   : If user would like to activate interrupt assertion function
                    of INTEN register, user should configure this register bit
                    to 1 with activating INTEN. During activated, this register
                    value keeps 1. Please notice that this register has effect
                    only when INTEN is activated. Otherwise, any configured
                    value to this register is ignored, and register value stays
                    at 0. User can write only ‘1’, and cannot write ‘0’ to this
                    register. If user writes 1 to INTCLR during this
                    bit being activated, this bit is internally cleared to 0.
    INTCLR(W)     : If user would like to inactivate interrupt assertion
                    function of INTEN register, user should configure this
                    register bit to 1 with activating above INTEN and INTSET.
                    Please notice that this register has effect only when INTEN
                    and INTSET are activated. Otherwise, any configured
                    value to this register is ignored. Also, this register bit
                    is write only register and user can write this register only
                    value of 1 (cannot write 0).
    INTSTAT(R)    : Interrupt status register for INTEN. This register
                    works even if the interrupt register INTEN is inactivated.
                    To clear this register, you should configure INTSTATCLR
                    register to 1. This status value is internally cleared if
                    INTSTATCLR is configured to 1.
    INTSTATCLR(W) : If user would like to clear status register INTSTAT, user
                    should configure this register to be 1. This register is
                    “write-only”, and written value cannot be read.
*/
    if (TRUE == enable)
    {
        /* Clear ERROR and CHANNELx bit in INTSTAT. */
        vpifRegs->INTSTATCLR |= (CSL_VPIF_INTSTATCLR_ERROR_CLR <<
            CSL_VPIF_INTSTATCLR_ERROR_SHIFT);
        vpifRegs->INTSTATCLR |= (chIntStatClrVal << chanHandle->chanNum);

        /* VPIF error interrupt enable. */
        vpifRegs->INTEN |= (CSL_VPIF_INTEN_ERROR_ENABLE <<
            CSL_VPIF_INTEN_ERROR_SHIFT);
        /* Error interrupt enable set. */
        vpifRegs->INTSET |= (CSL_VPIF_INTSET_ERROR_ENABLE <<
            CSL_VPIF_INTSET_ERROR_SHIFT);
        /* CHANNELx frame interrupt is enabled */
        vpifRegs->INTEN |= (chIntEnVal << chanHandle->chanNum);
        /* CHANNELx frame interrupt enable set. */
        vpifRegs->INTSET |= (chIntSetVal << chanHandle->chanNum);
    }
    else
    {
        /* disable error interrupt - Clear ERROR bit in INTSTAT. */
        vpifRegs->INTSTATCLR |= (CSL_VPIF_INTSTATCLR_ERROR_CLR <<
            CSL_VPIF_INTSTATCLR_ERROR_SHIFT);
        /* Interrupt on ERROR is masked */
        vpifRegs->INTCLR |= (CSL_VPIF_INTCLR_ERROR_CLR <<
            CSL_VPIF_INTCLR_ERROR_SHIFT);
        /* Error interrupt is disabled. */
        vpifRegs->INTEN &= ~(CSL_VPIF_INTEN_ERROR_ENABLE <<
            CSL_VPIF_INTEN_ERROR_SHIFT);

        /* disable corresponding channel interrupt                            */
        /* Clear CHANNELx bit in INTSTAT.                                     */
        vpifRegs->INTSTATCLR |= (chIntStatClrVal << chanHandle->chanNum);
        /* Interrupt on CHANNELx is masked                                    */
        vpifRegs->INTCLR |= (chIntClrVal << chanHandle->chanNum);
        /* CHANNELx frame interrupt is disabled.                              */
        vpifRegs->INTEN &= (~(chIntEnVal << chanHandle->chanNum));
    }
    /*
     * Line interrupts are not required in case of RAW capture so do not enable
     * second interrupt. Always need interrupt on frame completion. If interrupt
     * register bit of this interrupt(line interval) is activated, interrupt
     * signal is asserted to CPU at the end of each interval lines set.
     */
    if ((TRUE == chanHandle->useTwoChannels) &&
        (Vpif_CaptureFormat_CCDC != chanHandle->chVidParams.captureFormat))
    {
        if ((Vpif_CHANNEL0_VIDEO == chanHandle->chanNum) ||
            (Vpif_CHANNEL2_VIDEO == chanHandle->chanNum))
        {
            switch (chanHandle->chanNum + 1)
            {
                case Vpif_CHANNEL1_VIDEO:
                    chIntEnVal = CSL_VPIF_INTEN_FRAME1_ENABLE;
                    chIntSetVal = CSL_VPIF_INTSET_FRAME1_ENABLE;
                    chIntClrVal = CSL_VPIF_INTCLR_FRAME1_CLR;
                    chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME1_CLR;
                    break;
                case Vpif_CHANNEL3_VIDEO:
                    chIntEnVal = CSL_VPIF_INTEN_FRAME3_ENABLE;
                    chIntSetVal = CSL_VPIF_INTSET_FRAME3_ENABLE;
                    chIntClrVal = CSL_VPIF_INTCLR_FRAME3_CLR;
                    chIntStatClrVal = CSL_VPIF_INTSTATCLR_FRAME3_CLR;
                    break;
                default:
                    /* print error */
                    break;
            }

            if (TRUE == enable)
            {
                /* CHANNELx frame interrupt is enabled */
                vpifRegs->INTEN |= (chIntEnVal << (chanHandle->chanNum + 1));
                /* CHANNELx frame interrupt enable set. */
                vpifRegs->INTSET |= (chIntSetVal << (chanHandle->chanNum + 1));
            }
            else
            {
                /* Clear CHANNELx bit in INTSTAT. */
                vpifRegs->INTSTATCLR |= (chIntStatClrVal <<
                    (chanHandle->chanNum + 1));
                /* Interrupt on CHANNELx is masked */
                vpifRegs->INTCLR |= (chIntClrVal << (chanHandle->chanNum + 1));
                /* CHANNELx frame interrupt is disabled. */
                vpifRegs->INTEN &= (~(chIntEnVal << (chanHandle->chanNum + 1)));
            }
        }
        else
        {
            assert(FALSE);
        }
    }
}


/**
 *  \brief  Enable and disable the channel.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *  \param  enable           [IN]  Enable or disable the channel
 *
 *  \return None
 *
 */
static inline Void vpifEnableChannel(Vpif_ChanObj *chanHandle, Bool enable)
{
    assert(NULL != chanHandle);
    if (TRUE == enable)
    {
        /* Enable clock and channel */
        switch (chanHandle->chanNum)
        {
            case Vpif_CHANNEL0_VIDEO:
                chanHandle->baseAddress->C0CTRL |= CSL_VPIF_C0CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL1_VIDEO:
                chanHandle->baseAddress->C1CTRL |= CSL_VPIF_C1CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL2_VIDEO:
                chanHandle->baseAddress->C2CTRL |= \
                    (CSL_VPIF_C2CTRL_CLKEN_ENABLE <<
                     CSL_VPIF_C2CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C2CTRL |= CSL_VPIF_C2CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL3_VIDEO:
                chanHandle->baseAddress->C3CTRL |= \
                    (CSL_VPIF_C3CTRL_CLKEN_ENABLE <<
                     CSL_VPIF_C3CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C3CTRL |= CSL_VPIF_C3CTRL_CHANEN_ENABLE;
                break;

            default:
                /* print error */
                break;
        }
    }
    else
    {
        /* Disable clock and channel */
        switch (chanHandle->chanNum)
        {
            case Vpif_CHANNEL0_VIDEO:
                chanHandle->baseAddress->C0CTRL &= \
                    ~CSL_VPIF_C0CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL1_VIDEO:
                chanHandle->baseAddress->C1CTRL &= \
                    ~CSL_VPIF_C1CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL2_VIDEO:
                chanHandle->baseAddress->C2CTRL &= \
                    ~(CSL_VPIF_C2CTRL_CLKEN_ENABLE <<
                      CSL_VPIF_C2CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C2CTRL &= \
                    ~CSL_VPIF_C2CTRL_CHANEN_ENABLE;
                break;

            case Vpif_CHANNEL3_VIDEO:
                chanHandle->baseAddress->C3CTRL &= \
                    ~(CSL_VPIF_C3CTRL_CLKEN_ENABLE <<
                      CSL_VPIF_C3CTRL_CLKEN_SHIFT);
                chanHandle->baseAddress->C3CTRL &= \
                    ~CSL_VPIF_C3CTRL_CHANEN_ENABLE;
                break;

            default:
                /* print error */
                break;
        }
    }
    if (TRUE == chanHandle->useTwoChannels)
    {
        if ((Vpif_CHANNEL0_VIDEO == chanHandle->chanNum) ||
            (Vpif_CHANNEL2_VIDEO == chanHandle->chanNum))
        {
            if (TRUE == enable)
            {
                /* Enable clock and channel */
                switch (chanHandle->chanNum + 1)
                {
                    case Vpif_CHANNEL1_VIDEO:
                        chanHandle->baseAddress->C1CTRL |= \
                            CSL_VPIF_C1CTRL_CHANEN_ENABLE;
                        break;

                    case Vpif_CHANNEL3_VIDEO:
                        chanHandle->baseAddress->C3CTRL |= \
                            (CSL_VPIF_C3CTRL_CLKEN_ENABLE <<
                             CSL_VPIF_C3CTRL_CLKEN_SHIFT);
                        chanHandle->baseAddress->C3CTRL |= \
                            CSL_VPIF_C3CTRL_CHANEN_ENABLE;
                        break;

                    default:
                        /* print error */
                        break;
                }
            }
            else
            {
                /* Disable clock and channel */
                switch (chanHandle->chanNum + 1)
                {
                    case Vpif_CHANNEL1_VIDEO:
                        chanHandle->baseAddress->C1CTRL &= \
                            ~CSL_VPIF_C1CTRL_CHANEN_ENABLE;
                        /*
                         * TRICKY: Clear the CAPMODE bit for both the channels,
                         * so that the next BT capture on any channel works fine.
                         * The design spec states that using different CAPMODE
                         * settings between channels 0 and 1 will prevent the
                         * VPIF module from operating
                         */
                        chanHandle->baseAddress->C0CTRL = \
                            ((chanHandle->baseAddress->C0CTRL &
                              ~CSL_VPIF_C0CTRL_CAPMODE_MASK) |
                                ((CSL_VPIF_C0CTRL_CAPMODE_BTYC <<
                                CSL_VPIF_C0CTRL_CAPMODE_SHIFT) &
                                CSL_VPIF_C0CTRL_CAPMODE_MASK));

                        chanHandle->baseAddress->C1CTRL = \
                            ((chanHandle->baseAddress->C1CTRL &
                              ~CSL_VPIF_C1CTRL_CAPMODE_MASK) |
                                ((CSL_VPIF_C1CTRL_CAPMODE_BTYC <<
                                CSL_VPIF_C1CTRL_CAPMODE_SHIFT) &
                                CSL_VPIF_C1CTRL_CAPMODE_MASK));
                        break;

                    case Vpif_CHANNEL3_VIDEO:
                        chanHandle->baseAddress->C3CTRL &= \
                            ~(CSL_VPIF_C3CTRL_CLKEN_ENABLE <<
                              CSL_VPIF_C3CTRL_CLKEN_SHIFT);
                        chanHandle->baseAddress->C3CTRL &= \
                            ~CSL_VPIF_C3CTRL_CHANEN_ENABLE;
                        break;

                    default:
                        /* print error */
                        break;
                }
            }
        }
        else
        {
            assert(FALSE);
        }
    }
}

/**
 *  \brief  Stop the transfer for the channel.
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *
 *  \return IOM_COMPLETED
 *
 */
static Int32 vpifStopTransfer(Vpif_ChanObj *chanHandle)
{
    Vpif_Object  *stopTfrInstHandle = NULL;
    Int32 stopRtnVal = IOM_COMPLETED;
    Uint32 stopKey = 0;

    assert(NULL != chanHandle);
    stopTfrInstHandle = (Vpif_Object *)chanHandle->devHandle;
    assert(NULL != stopTfrInstHandle);
    assert(0 != chanHandle->started);/* Streaming is not started */

    /* enter critical section                                         */
    stopKey = (Uint32)_disable_interrupts();
    /* disable channel */
    vpifEnableChannel(chanHandle, FALSE);
    /* Disable Vpif interrupt causes                                  */
    vpifChannelIntrEnable(chanHandle, FALSE);
    /* exit critical section                                          */
    _restore_interrupts(stopKey);

    /* disable ancillary data in channel control */
    if ((0 != chanHandle->hbiBufSz) || (0 != chanHandle->vbiBufSz))
    {
        vpifSetRawParams(chanHandle, FALSE);
    }

    /* set state as channel stopped */
    chanHandle->chState &= ~Vpif_DriverState_STARTED;
    chanHandle->started = 0;

    /* If the driver is power aware disable the PSC */
    if (TRUE == stopTfrInstHandle->isPwrAware)
    {
        /* Disable LPSC of VPIF if not already done */
        stopRtnVal = vpifLpscOff(stopTfrInstHandle);
    }

    return stopRtnVal;
}


/**
 *  \brief  Frame parameter check for a particular frame
 *
 *  \param  chanHandle       [IN]  Handle of the Channel Object
 *  \param  viop             [IN]  FVID frame pointer
 *
 *  \return None
 *
 */
static Void vpifValidateFrameParms(Vpif_ChanObj *chanHandle, FVID_Frame *viop)
{
    if ((NULL != chanHandle) && (NULL != viop))
    {
        /* Validate the data pointer and check the address must be a multiple of
           8([2:0] = 0) */
        if (Vpif_CaptureFormat_CCDC == chanHandle->chVidParams.captureFormat)
        {
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                assert(NULL != (viop->frame.riFrm.buf1));
                assert(0 == ((Uint32)viop->frame.riFrm.buf1 % 8));
                assert(NULL != (viop->frame.riFrm.buf2));
                assert(0 == ((Uint32)viop->frame.riFrm.buf2 % 8));
            }
            else
            {
                assert(NULL != (viop->frame.rpFrm.buf));
                assert(0 == ((Uint32)viop->frame.rpFrm.buf % 8));
            }
            assert(FVID_RAW_FORMAT == viop->frameFormat);
            if (Vpif_RawCaptureDataWidth_8BITS == chanHandle->dataSz)
            {
                assert(FVID_BPP_BITS8 == viop->bpp);
                assert(!(((chanHandle->bufSz) /
                    (viop->pitch)) < chanHandle->chVidParams.height));
            }
            else if (Vpif_RawCaptureDataWidth_10BITS == chanHandle->dataSz)
            {
                assert(FVID_BPP_BITS10 == viop->bpp);
                assert(!(((chanHandle->bufSz) /
                    (viop->pitch * 2)) < chanHandle->chVidParams.height));
            }
            else
            {
                assert(FVID_BPP_BITS12 == viop->bpp);
                assert(!(((chanHandle->bufSz) /
                    (viop->pitch * 2)) < chanHandle->chVidParams.height));
            }
        }
        else
        {
            assert(NULL != (viop->frame.spFrm.y1));
            assert(NULL != (viop->frame.spFrm.c1));
            assert(0 == ((Uint32)viop->frame.spFrm.y1 % 8));
            assert(0 == ((Uint32)viop->frame.spFrm.c1 % 8));
            if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
            {
                assert(NULL != (viop->frame.spFrm.c2));
                assert(NULL != (viop->frame.spFrm.y2));
                assert(0 == ((Uint32)viop->frame.spFrm.c2 % 8));
                assert(0 == ((Uint32)viop->frame.spFrm.y2 % 8));
            }
            assert(FVID_YCbCr422_SEMIPLANAR_UV == viop->frameFormat);
            assert(FVID_BPP_BITS8 == viop->bpp);
            if (0 != chanHandle->hbiBufSz)
            {
                assert(NULL != (viop->vbiFrm.h1));
                assert(0 == ((Uint32)viop->vbiFrm.h1 % 8));
                if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
                {
                    assert(NULL != (viop->vbiFrm.h2));
                    assert(0 == ((Uint32)viop->vbiFrm.h2 % 8));
                }
            }
            if (0 != chanHandle->vbiBufSz)
            {
                assert(NULL != (viop->vbiFrm.v1));
                assert(0 == ((Uint32)viop->vbiFrm.v1 % 8));
                if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
                {
                    assert(NULL != (viop->vbiFrm.v2));
                    assert(0 == ((Uint32)viop->vbiFrm.v2 % 8));
                }
            }
            if (0 != chanHandle->numSliceServices)
            {
                assert(NULL != (viop->vbiFrm.s1));
                if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
                {
                    assert(NULL != (viop->vbiFrm.s2));
                }
            }
            assert(!(((chanHandle->bufSz) /
                (viop->pitch * 2)) < chanHandle->chVidParams.height));
        }

        assert(0 == (viop->pitch % 8));
        assert(!(viop->pitch <= 0));
        assert(!(viop->pitch < chanHandle->chVidParams.width));
        assert(!(viop->lines < chanHandle->chVidParams.height));

        if (Vpif_FrameFormat_INTERLACED == chanHandle->chVidParams.frameFmt)
        {
            if (Vpif_SdramStorage_FIELD == chanHandle->frameStorageMode)
            {
                assert(FVID_STORAGE_FORMAT_FIELD == viop->storeFormat);
            }
            else
            {
                assert(FVID_STORAGE_FORMAT_FRAME == viop->storeFormat);
            }
        }
        else
        {
            assert(FVID_STORAGE_FORMAT_FRAME == viop->storeFormat);
        }
    }
    else
    {
        assert(FALSE);
    }
}



/**
 *  \brief En-queing buffer in the buffer queue. Submit the empty buffer to
 *         driver for capture or filled buffer to driver for display.
 *
 *  \param  chanHandle      [In] VPIF channel object pointer
 *  \param  packet          [In] video IO packet
 *
 *  \return  IOM_COMPLETED on success
 *           IOM errors on failure
 */
static Int32 vpifQueueBuffer(Vpif_ChanObj *chanHandle, IOM_Packet *packet)
{
    FVID_Frame *queViop;
    Int32 queRtnVal = IOM_COMPLETED;

    assert(NULL != chanHandle);
    assert(NULL != packet);
    if (NULL == (*(Void**)packet->addr))
    {
        queRtnVal = IOM_EBADARGS;
    }
    else
    {
        queViop = *(Void**)packet->addr; /* pointer of a video I/O packet */
        assert(NULL != queViop);
        vpifValidateFrameParms(chanHandle, queViop);
        /* increment number of buffers available with driver */
        chanHandle->numQueBuf++;
        QUE_put(&chanHandle->qIn, (QUE_Handle)queViop);
        if (Vpif_DriverState_READY !=
            (Vpif_DriverState_READY & chanHandle->chState))
        {
            /* VPIF can only be ready after minimum required
             * number of buffers are queued into the buffer queue*/
            chanHandle->queEmpty = FALSE;
            if (2 == chanHandle->numQueBuf)
            {
                chanHandle->curViop = (FVID_Frame *)(QUE_get(&chanHandle->qIn));
                chanHandle->nextViop = chanHandle->curViop;
                chanHandle->queEmpty = FALSE;
                chanHandle->chState |= Vpif_DriverState_READY;
            }
        }
        else
        {
            /* channel is ready i.e. min buffers are available */
            if (TRUE == chanHandle->queEmpty)
            {
                chanHandle->queEmpty = FALSE;
            }
        }
        queRtnVal = packet->status = IOM_COMPLETED;

    }

    return queRtnVal;
}


/**
 *  \brief Provide the empty buffer or the captured buffer to application
 *
 *  \param  chanHandle      [In] VPIF channel object pointer
 *  \param  packet          [OUT] video IO packet
 *
 *  \return  IOM_COMPLETED on success
 *           IOM_PENDING if buffer is not available
 *           suitable error code on failure
 */
static Int32 vpifDequeueBuffer(Vpif_ChanObj *chanHandle, IOM_Packet *packet)
{
    Int32 dequeRtnVal = IOM_COMPLETED;
    FVID_Frame *dequeViop = NULL;
    assert(NULL != chanHandle);
    assert(NULL != packet);
    if (Vpif_IoMode_DIS == chanHandle->channelMode)
    {
        dequeViop = (FVID_Frame *)QUE_get(&chanHandle->qOut);
        if (dequeViop != (FVID_Frame *)&chanHandle->qOut)
        {
            assert(NULL != dequeViop);
            /* only when there is no outstanding pending request */
            *(Void**)packet->addr = (Void*)dequeViop;
            packet->size = sizeof(FVID_Frame);
            dequeRtnVal = packet->status = IOM_COMPLETED;

            /* decrement number of buffers available with driver */
            chanHandle->numQueBuf--;
        }
        else
        {
            /* qOut has no packet to release */
            chanHandle->packetIOM = packet;
            dequeRtnVal = packet->status = IOM_PENDING;
        }
    }
    else
    {
        if (chanHandle->nextViop != chanHandle->mrViop)
        {
            *(Void**)packet->addr = (Void*)chanHandle->mrViop;
            chanHandle->mrViop = NULL;
            packet->size = sizeof(FVID_Frame);
            dequeRtnVal = packet->status = IOM_COMPLETED;
        }
        else
        {
            /* too late, just recycle mrViop */
            chanHandle->mrViop = NULL;
            chanHandle->packetIOM = packet;
            dequeRtnVal = packet->status = IOM_PENDING;
        }

        if (IOM_COMPLETED == dequeRtnVal)
        {
            /* decrement number of buffers available with driver */
            chanHandle->numQueBuf--;
        }
    }

    return dequeRtnVal;
}


/**
 *  \brief Return all buffers to application
 *
 *  This function returns all buffers to application held by driver. The buffers
 *  may not contain proper data.
 *
 *  \param  chanHandle    [In]  VPIF channel object pointer
 *  \param  packet        [Out] video IO packet
 *
 *  \return  IOM_COMPLETED on success
 *           IOM_ENOPACKETS if buffer is not available
 */
static Int32 vpifGetBufs(Vpif_ChanObj *chanHandle, IOM_Packet *packet)
{
    Int32 getBufRtnVal = IOM_COMPLETED;
    FVID_Frame *getBufViop = NULL;
    assert(NULL != chanHandle);
    assert(NULL != packet);
    if (0 == chanHandle->numQueBuf)
    {
        chanHandle->queEmpty = TRUE;
        getBufRtnVal = IOM_ENOPACKETS;
    }
    else
    {
        if (Vpif_IoMode_DIS == chanHandle->channelMode)
        {
            /* Take buffer from qOut if it is not empty */
            getBufViop = (FVID_Frame *)QUE_get(&chanHandle->qOut);
            if (getBufViop == (FVID_Frame *)&chanHandle->qOut)
            {
                /* Take buffer from qIn if qOut is empty */
                getBufViop = (FVID_Frame *)QUE_get(&chanHandle->qIn);
            }
            if ((getBufViop == (FVID_Frame *)&chanHandle->qIn) &&
                (chanHandle->nextViop != NULL))
            {
                /* If qIn is empty then return buffers from viop pointers */
                chanHandle->queEmpty = TRUE;
                getBufViop = chanHandle->nextViop;
                /* if curViop is same as nextViop then make it NULL */
                if (chanHandle->curViop == chanHandle->nextViop)
                {
                    chanHandle->curViop = NULL;
                }
                chanHandle->nextViop = NULL;
            }
            /* Fill buffer address and other information in packet */
            *(Void**)packet->addr = (Void*)getBufViop;
            packet->size = sizeof(FVID_Frame);
            getBufRtnVal = packet->status = IOM_COMPLETED;
            /* Decrement number of buffers with driver */
            chanHandle->numQueBuf--;
            if (chanHandle->numQueBuf < 2)
            {
                /* If chanHandle->numQueBuf is less than 2 then, put curViop
                 * and nextViop in queue.
                 * Take care that every buffer is being queued only once.
                 */
                if (chanHandle->nextViop != NULL)
                {
                    QUE_put(&chanHandle->qIn, (QUE_Handle)chanHandle->nextViop);
                    chanHandle->queEmpty = FALSE;
                }
                if ((chanHandle->curViop != NULL) &&
                    (chanHandle->curViop != chanHandle->nextViop))
                {
                    QUE_put(&chanHandle->qIn, (QUE_Handle)chanHandle->curViop);
                    chanHandle->queEmpty = FALSE;
                }
                /* make all viop pointers NULL */
                chanHandle->nextViop = NULL;
                chanHandle->curViop = NULL;

                /* set channel state as VPIF not ready */
                chanHandle->chState &= ~Vpif_DriverState_READY;
            }
        }
        else
        {
            /* Take buffer from queue if queue is not empty */
            getBufViop = (FVID_Frame *)QUE_get(&chanHandle->qIn);
            if (getBufViop == (FVID_Frame *)&chanHandle->qIn)
            {
                /* If queue is empty then return buffers from viop pointers
                 * one-by-one
                 */
                chanHandle->queEmpty = TRUE;
                if (chanHandle->mrViop != NULL)
                {
                    getBufViop = chanHandle->mrViop;

                    /* if nextViop or curViop is same as mrViop then make it
                    NULL */
                    if (chanHandle->nextViop == chanHandle->mrViop)
                    {
                        chanHandle->nextViop = NULL;
                    }
                    if (chanHandle->curViop == chanHandle->mrViop)
                    {
                        chanHandle->curViop = NULL;
                    }
                    chanHandle->mrViop = NULL;
                }
                else if (chanHandle->nextViop != NULL)
                {
                    getBufViop = chanHandle->nextViop;
                    /* if curViop is same as nextViop then make it NULL */
                    if (chanHandle->curViop == chanHandle->nextViop)
                    {
                        chanHandle->curViop = NULL;
                    }
                    chanHandle->nextViop = NULL;
                }
            }

            /* Fill buffer address and other information in packet */
            *(Void**)packet->addr = (Void*)getBufViop;
            packet->size = sizeof(FVID_Frame);
            getBufRtnVal = packet->status = IOM_COMPLETED;
            /* Decrement number of buffers with driver */
            chanHandle->numQueBuf--;
            if (chanHandle->numQueBuf < 2)
            {
                /* If chanHandle->numQueBuf is less than 2 then, put curViop,
                 * nextViop and mrViop in queue.
                 * Take care that every buffer is being queued only once.
                 */
                if (chanHandle->mrViop != NULL)
                {
                    QUE_put(&chanHandle->qIn, (QUE_Handle)chanHandle->mrViop);
                    chanHandle->queEmpty = FALSE;
                }
                if ((chanHandle->nextViop != NULL) &&
                    (chanHandle->nextViop != chanHandle->mrViop))
                {
                    QUE_put(&chanHandle->qIn, (QUE_Handle)chanHandle->nextViop);
                    chanHandle->queEmpty = FALSE;
                }
                if ((chanHandle->curViop != NULL) &&
                    (chanHandle->curViop != chanHandle->mrViop) &&
                    (chanHandle->curViop != chanHandle->nextViop))
                {
                    QUE_put(&chanHandle->qIn, (QUE_Handle)chanHandle->curViop);
                    chanHandle->queEmpty = FALSE;
                }

                /* make all viop pointers NULL */
                chanHandle->mrViop = NULL;
                chanHandle->nextViop = NULL;
                chanHandle->curViop = NULL;
                /* set channel state as VPIF not ready */
                chanHandle->chState &= ~Vpif_DriverState_READY;
            }
        }
    }

    return getBufRtnVal;
}

/* ========================================================================== */
/*                              END OF FILE                                   */
/* ========================================================================== */
