/*
 * VpifLocal.h
 *
 * Vpif Driver internal header file
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
 *  \file   VpifLocal.h
 *
 *  \brief  Internal Header file for Vpif device.
 *
 */

#ifndef _VPIFLOCAL_H_
#define _VPIFLOCAL_H_

#if !defined(CHIP_C6748) && !defined(CHIP_OMAPL138)
#error "No chip type defined! (Must use -DCHIP_C6748 or -DCHIP_OMAPL138)"
#endif

#if (defined(CHIP_C6748) || defined(CHIP_OMAPL138)) && !defined(BIOS_PWRM_ENABLE)
#error "Wrong Chip ID defined for BIOS_PWRM_ENABLE."
#endif

#if (defined(CHIP_C6747) || defined(CHIP_OMAPL137)) && defined(BIOS_PWRM_ENABLE)
#error "Do not use Bios PWRM module for this SoC"
#endif

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include "ti/pspiom/vpif/Vpif.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             DEFINES                                        */
/* ========================================================================== */
/** \brief Number of capture channels this VPIF instance supports             */
#define Vpif_CAPTURE_NUM_CHANNELS          (2u)
/** \brief Number of display channels this VPIF instance supports             */
#define Vpif_DISPLAY_NUM_CHANNELS          (2u)
/** \brief Total number of channels this VPIF instance supports               */
#define Vpif_TOTAL_NUM_CHANNELS            (Vpif_CAPTURE_NUM_CHANNELS + \
                                            Vpif_DISPLAY_NUM_CHANNELS)
/** \brief Channel 0 define                                                   */
#define Vpif_CHANNEL0_VIDEO                (0u)
/** \brief Channel 1 define                                                   */
#define Vpif_CHANNEL1_VIDEO                (1u)
/** \brief Channel 2 define                                                   */
#define Vpif_CHANNEL2_VIDEO                (2u)
/** \brief Channel 3 define                                                   */
#define Vpif_CHANNEL3_VIDEO                (3u)
/** \brief All VBI services macro                                             */
#define Vpif_ALL_VBI_SERVICES              (Vpif_VbiServiceType_HBI | \
                                            Vpif_VbiServiceType_RAW_VBI | \
                                            Vpif_VbiServiceType_SLICE_VBI)
/** \brief All Slice VBI services define                                      */
#define Vpif_ALL_SLICE_VBI_SERVICES       (FVID_SLICE_VBI_SERVICES_CGMS_NTSC | \
                                           FVID_SLICE_VBI_SERVICES_CC_NTSC | \
                                           FVID_SLICE_VBI_SERVICES_WSS_PAL)
/** \brief Tasklet priority for VBI interrupt handling in SWI context of DSP  */
#define Vpif_TASKLET_PRIORITY              (1)

/** \brief Task priority for slice VBI data read/write                        */
#define Vpif_VBI_TASK_PRIO                 (1)

/** \brief Task stack size for slice VBI                                      */
#define Vpif_VBI_TASK_STACK_SIZE_BYTE      (0x1000)

/** \brief Maximum PWRM events supported by VPIF                              */
#define Vpif_MAX_PWRM_EVENTS               (PWRM_INVALIDEVENT)

/** \brief Vpif driver state : Closed a opened channel or deleted a created
 instance                                                                     */
#define Vpif_DriverState_CLOSED            (0x0u)
/** \brief Vpif driver state : Instance created                               */
#define Vpif_DriverState_CREATED           (0x01u)
/** \brief Vpif driver state : Channel opened                                 */
#define Vpif_DriverState_OPENED            (0x02u)
/** \brief Vpif driver state : Ready to start operation                       */
#define Vpif_DriverState_READY             (0x04u)
/** \brief Vpif driver state : Channel operation started                      */
#define Vpif_DriverState_STARTED           (0x08u)

/* ========================================================================== */
/*                             ENUMS                                          */
/* ========================================================================== */

/* ========================================================================== */
/*                              STRUCTURES                                    */
/* ========================================================================== */
/**
 *  \brief module-wide state
 *
 *  inUse in module state points to an array whose size is soc specific
 *  Pointer used to match IDriver codebase.
 */
typedef struct Vpif_Module_State_t
{
    Bool *inUse;
    /**< Maintain inUse state of each vpif device                             */
}Vpif_Module_State;


/**
 *  \brief PWRM information
 *
 *  Structure holding PWRM related information
 */
typedef struct Vpif_PwrmInfo_t
{
    Vpif_PllDomain              pllDomain;
    /**< PLL domain to be used for the device                                 */

    PWRM_Event                  pwrmEvent;
    /**< Current PWRM event being processed                                   */

    PWRM_NotifyHandle           notifyHandle[Vpif_MAX_PWRM_EVENTS];
    /**< Handles required for unregistering of the events with PWRM           */

    Fxn                         delayedCompletionFxn[Vpif_MAX_PWRM_EVENTS];
    /**< Delayed completion callback function pointer                         */
}Vpif_PwrmInfo;


/**
 *  \brief  VPIF channel object structure
 *  vpif channel object structure to maintain driver's internal
 *  parameters and state.
 */
typedef struct Vpif_ChanObj_t
{
    Uint32                                  chState;
    /**< Channel status opened, configured, allocated, etc...                 */
    Ptr                                     devHandle;
    /**< Handle to the Device                                                 */
    Uint32                                  chError;
    /**< Number of Channel errors                                             */
    Uint32                                  frameInt;
    /**< Number of Frame interrupts                                           */
    Uint8                                   chanNum;
    /**< Channel number of instance                                           */
    CSL_VpifRegsOvly                        baseAddress;
    /**< Pointer to VPIF register structure defined in cslr_vpif.h
     * This will point to base address of VPIF                                */
    Vpif_SdramStorage                       frameStorageMode;
    /**< Indicates whether it is field or frame based storage mode            */
    IOM_TiomCallback                        cbFxn;
    /**< IOM callback function                                                */
    Ptr                                     cbArg;
    /**< IOM callback function argument                                       */
    Vpif_VideoMode                          vidMode;
    /**< Video mode supported by the channel                                  */
    Vpif_IoMode                             channelMode;
    /**< Channel's IO mode of operation                                       */
    EDC_Fxns                                *edcFxns;
    /**< Pointer to function table of external device                         */
    Ptr                                     edcHandle;
    /**< Handle of external device                                            */
    Int32                                   segId;
    /**< Memory segment id to heap from where allocation has to be done on
    application's behalf to allocate frame buffer memory                      */
    Uint32                                  bufSz;
    /**< Video Buffer size in bytes                                           */
    Uint16                                  hpitch;
    /**< Horizontal pitch size in bytes                                       */
    Uint16                                  lineOffset;
    /**< Line offset for DMA in bytes                                         */
    Uint16                                  alignment;
    /**< Frame buffer alignment                                               */
    Uint8                                   started;
    /**< Indicates whether streaming started                                  */
    Bool                                    useTwoChannels;
    /**< Indicates whether to use one channel or two                          */
    Uint32                                  fieldId;
    /**< Indicates id of the field of the current frame                       */
    Vpif_ConfigParams                       chVidParams;
    /**< Video parameters for the channel                                     */

    /**NOTE: Buffer specific parameters */
    QUE_Obj                                 qIn;
    /**< IN queue associated with this channel. For display this queue object
    contains buffers to be displayed. For capture this queue object contains
    empty buffers.                                                            */
    QUE_Obj                                 qOut;
    /**< OUT queue associated with this channel. For display this queue object
    that contains buffers already displayed.  For capture this is not used.   */
    volatile Bool                           queEmpty;
    /**< Flag to maintain queue status                                        */
    FVID_Frame                              *curViop;
    /**< Current video IO packet. For display it is the current frame that
    DMA is transferring data out of. For capture it is the current frame that
    DMA is transferring data into                                             */
    FVID_Frame                              *nextViop;
    /**< Next video IO packet. For display it is the buffer DMA is going to
    move data from after display completion of current frame. For capture
    it is the buffer DMA is going to move data to after capture completion
    of current frame                                                          */
    FVID_Frame                              *mrViop;
    /**< Most recent video IO packet. For capture it is the buffer containing
    data of the most recently captured frame. For display this is not used.   */
    IOM_Packet                              *packetIOM;
    /**< Pointer to IOM packet. IOM Packet passed by the GIO layer is stored
    here                                                                      */
    Int32                                   numQueBuf;
    /**< Number of submitted buffer                                           */

    /**NOTE: Below params are for VBI Capture/Display*/
    Uint32                                  hbiBufSz;
    /**< HBI Buffer size in bytes. A value of 0 suggests that RAW HBI is not
    enabled on the channel.                                                   */
    Uint32                                  vbiBufSz;
    /**< VBI Buffer size in bytes. A value of 0 suggests that RAW HBI is not
    enabled on the channel.                                                   */
    Ptr                                     vbiTskletHandle;
    /**< Tasklet (Software interrupt) handle                                  */
    Uint32                                  sliceServType;
    /**< Type of Slice VBI service. Available values for this field are defined
     *   in "Vpif.h" file with VPIF Slice VBI service type title. This should be
         passed appropriately  according to the Video standard mode desired.  */
    Uint8                                   numSliceServices;
    /**< Number of slice VBI services enabled. A value of 0 suggests that Slice
    VBI is not enabled for the channel.                                       */
    Int32                                   sliceVbiFld0I2cSubmitStatus;
    /**< Slice VBI field 0 submit status for I2C                              */
    Int32                                   sliceVbiFld0I2cCbStatus;
    /**< Slice VBI field 0 callback status for I2C                            */
    Int32                                   sliceVbiFld1I2cSubmitStatus;
    /**< Slice VBI field 1 submit status for I2C                              */
    Int32                                   sliceVbiFld1I2cCbStatus;
    /**< Slice VBI field 1 callback status for I2C                            */
    Int32                                   currPktStatus;
    /**< Packet status of the current channel                                 */
    TSK_Handle                              taskHandle;
    /**< VPIF Slice VBI Task Handle                                           */
    Bool                                    taskRun;
    /**< VPIF Slice VBI Task running or not.                                  */
    SEM_Handle                              syncSem;
    /**< Semaphore for synchronization between task and ISR                   */

    /**NOTE: Place holder for saving vbi and hbi params reqd only for display*/
    Vpif_RawSelectiveVbiParams              *setSelVbiDispParams;
    /**< Place holder for Selective positioning of RAW VBI Display data. If set
    to NULL implies no selective positioning required                         */
    Vpif_RawSelectiveVbiParams              *setSelHbiDispParams;
    /**< Place holder for Selective positioning of RAW HBI Display data. If set
    to NULL implies no selective positioning required                         */

    /**NOTE: These parameters are only reqd for raw capture*/
    Vpif_RawCaptureDataWidth                dataSz;
    /**< The data width bit is only used with the CCD/CMOS data capture mode  */
    Vpif_RawCapturePinPol                   fieldPolarity;
    /**< Field ID polarity inverting control                                  */
    Vpif_RawCapturePinPol                   vPixelPol;
    /**< Vertical pixel valid signal polarity control.                        */
    Vpif_RawCapturePinPol                   hPixelPol;
    /**< Horizontal pixel valid signal polarity control.                      */
}Vpif_ChanObj;

/**
 *  \brief  Vpif structure to hold the instance specific information for the
 *  VPIF device
 */
typedef struct Vpif_HwInfo_t
{
    CSL_VpifRegsOvly                        baseAddr;
    /**< Pointer to VPIF register structure defined in cslr_vpif.h
     * This will point to base address of VPIF                                */
    Uint16                                  cpuEventNumber;
    /**< CPU event number                                                     */
    Uint16                                  hwiNo;
    /**< Hwi number used for vpif                                             */
    Uint32                                  pwrmLpscID;
    /**< LPSC Id                                                              */
    Uint32                                  pscInstance;
    /**< PSC instance                                                         */
}Vpif_HwInfo;

/**
 *  \brief data structure for VPIF object per-instance state
 */
typedef struct Vpif_Object_t
{
    Int32                                   instNum;
    /**< Instance number or device ID of the VPIF                             */
    Vpif_DmaReqSize                         dmaSize;
    /**< VPIF DMA Size value                                                  */
    Uint32                                  devState;
    /**< Instance status, contains information on whether
     * instance is opened, opened, etc.                                       */
    Vpif_HwInfo                             deviceInfo;
    /**< Instance specific information                                        */
    Bool                                    isPwrAware;
    /**< Is the driver power management aware or not                          */
    Vpif_PwrmInfo                           pwrmInfo;
    /**< PWRM related information                                             */
    Vpif_ChanObj                            chanObj[Vpif_TOTAL_NUM_CHANNELS];
    /**< four channel objects for channel 0, 1, 2 and 3.                      */
}Vpif_Object;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /*_VPIFLOCAL_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
