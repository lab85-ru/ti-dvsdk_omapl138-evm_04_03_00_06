/*
 * Vpif.h
 *
 * This file contains the Application programming interface, data types and
 * command/macro definitions that are needed by the application to utilize the
 * services of the VPIF device driver. This file contains defines and structures
 * for both capture and display devices.
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

/** \mainpage    PSP VPIF Driver on BIOS
 *
 *  \section    Introduction
 *
 *  \subsection    xxx    Purpose and Scope
 *  The purpose of this document is to explain the VPIF driver on the PSP
 *  Framework architecture written in DSP/BIOS.
 *
 *  \subsection    bbb    VPIF Controller
 *   The video port interface (VPIF) has two video input channels and two video
 *   output channels. Channels 0 and 1 are prepared only for input. Channels 2
 *   and 3 are prepared only for output. VPIF also supports raw data capturing.
 *   VPIF can transmit and receive ancillary data in the video blanking
 *   interval (VBI) of BT.656 video streams. The ancillary data regions are
 *   referred to as: Horizontal Ancillary (HANC) - Data between EAV and SAV
 *   (horizontal blanking interval), and Vertical Ancillary (VANC) - Data
 *   between SAV and EAV (horizontal active video area).
 *   Currently, the driver supports NTSC and PAL standards.
 *
 *  \subsection    ccc    DSP/BIOS System
 *  DSP/BIOS is a real time kernel provided by TI to use with its
 *  DSP processors.
 *
 *  \subsection    ddd   Compiler defines to add/remove features
 *  Define to turn OFF parameter checking.
 *  Use -DPSP_DISABLE_INPUT_PARAMETER_CHECK when building library
 *
 *  Define to turn OFF asserts. Use -DNDEBUG when building library.
 *
 *  \subsection    References
 *    -# DSP/BIOS driver development <b>SPRU616.pdf</b>
 *
 */

#ifndef _VPIF_H_
#define _VPIF_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include "ti/pspiom/vpif/Edc.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             VPIF DEFINES                                   */
/* ========================================================================== */
/** \brief VPIF VBI Ancillary Data service: NONE. No Ancillary Data is
required                                                                      */
#define Vpif_VbiServiceType_NONE            0x0
/** \brief VPIF VBI Ancillary Data service: Horizontal Ancillary (HANC) -
Data between EAV and SAV (horizontal blanking interval)                       */
#define Vpif_VbiServiceType_HBI             0x1u
/** \brief VPIF VBI Ancillary Data service: Vertical Ancillary (VANC) -
Data between SAV and EAV (horizontal active video area).                      */
#define Vpif_VbiServiceType_RAW_VBI         0x2u
/** \brief VPIF VBI Ancillary Data service: Specific Ancillary Data.
ancillary data that is not video image data but is VBI data.                  */
#define Vpif_VbiServiceType_SLICE_VBI       0x4u

/** \brief Base error code for VPIF                                           */
#define Vpif_ERROR_BASE                     (IOM_EINUSE)
/** \brief I2C error happened during VBI                                      */
#define Vpif_VBI_I2C_ERR                    (Vpif_ERROR_BASE - 1)


/* ========================================================================== */
/*                             VPIF ENUMS                                     */
/* ========================================================================== */
/**
 *  \brief  Vpif Ioctl commands
 * Following IOCTL commands will be used to perform control operation on VPIF.
 * The following commands are common for both capture and display operation.
 * The IOCTL command is passed as second argument in vpifMdControlChan()
 * function when the driver is used directly with the application.
 */
typedef enum Vpif_IOCTL_t
{
    Vpif_IOCTL_CMD_START,
    /**< Start the VPIF channel operation.
     *   Only this ioctl command needs to be passed as second argument in
     *   vpifMdControlChan() function.                                        */
    Vpif_IOCTL_CMD_STOP,
    /**< Stop the VPIF channel operation.
     *   Only this ioctl command needs to be passed as second argument in
     *   vpifMdControlChan() function.                                        */
    Vpif_IOCTL_CMD_GET_NUM_IORQST_PENDING,
    /**< Get number of pending I/O requests in the driver queue.
     *   This IOCTL will be used to get number of pending I/O requests in VPIF
     *   channel. Application has to pass pointer to Int32 variable as a
     *   third argument in vpifMdControlChan() function.
     *   For example
     *   Int32 pendReq;
     *   cmdArg = (&pendReq);                                                 */
    Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO,
    /**< Get the current configuration parameters of driver.
     *   This IOCTL will be used to get the current configuration parameters for
     *   the VPIF channel. Application has to pass pointer to Vpif_StdInfo
     *   variable as a third argument in vpifMdControlChan() function.
     *   For example
     *   Vpif_StdInfo stdInfo;
     *   cmdArg = (&stdInfo);                                                 */
     Vpif_IOCTL_CMD_CHANGE_RESOLUTION,
    /**< Change the current resolution of the channel.
     *   This IOCTL will be used to change the current resolution for
     *   the VPIF channel. Please note that changing the resolution between SD,
     *   HD and  RAW mode is not allowed i.e. Channel properties cannot be
     *   changed (Application may need to close the channel and create channel
     *   in that case). Using this IOCTL the application can switch between
     *   different resolutions with in SD (PAL to NTSC) or HD (720P to 1080P)
     *   or RAW (VGA to SVGA). Application has to pass pointer to
     *   Vpif_ConfigParams variable as a third argument in vpifMdControlChan()
     *   function.
     *   NOTE 1: Please use either of the following
     *          a) Application can choose to specify the pre-defined modes
     *             (Vpif_VideoMode) in the "mode" parameter
     *          b) Application can set the "mode" parameter to
     *             "Vpif_VideoMode_NONE" and provide the filled up
     *             Vpif_ConfigParams structure as third parameter.
     *          c) If application sets valid mode in "mode" parameter and also
     *             sends the filled structure, the driver would consider the
     *             "mode" parameter and update accordingly.
     *   NOTE 2: The driver does not check the validity for these parameters
     *           when application uses point b method (mentioned above).
     *   NOTE 3: The channel should be stopped using Vpif_IOCTL_CMD_STOP IOCTL.
     *           Also the buffers should be freed up, as the buffer requirement
     *           changes once the resolution changes.
     *   For example
     *   Vpif_ConfigParams chResolution;
     *   chResolution.mode = Vpif_VideoMode_RAW_SVGA;
     *   cmdArg = (&chResolution);                                            */
    Vpif_IOCTL_CMD_MAX
    /**< Book-keep - Max ioctl's                                              */
}Vpif_IOCTL;


/**
 * \brief SDRAM Storage type
 *
 * This enum defines the different storage modes of operation. Progressive
 * video must use the Frame storage mode, but interlaced video can use either
 * Field or Frame storage modes.
 */
typedef enum Vpif_SdramStorage_t
{
    Vpif_SdramStorage_FIELD = 0,
    /**< VPIF field format storage: field 1 and field 2 will be stored
     *   separately.                                                          */
    Vpif_SdramStorage_FRAME
    /**< VPIF frame format storage: field 1 and field 2 will be stored in
     *   merged pattern i.e. one line of field 1, one line of field 2.
     *   CAUTION: For Progressive mode SDRAM storage should be Frame ONLY.    */
}Vpif_SdramStorage;

/**
 * \brief Video mode type
 *
 * This enum defines the different video modes of operation.
 */
typedef enum Vpif_VideoMode_t
{
    Vpif_VideoMode_NONE = 0,
    /**< VPIF operation mode: NONE. Used when user wants to send the different
     *   video parameters and do not want to use internal look-up table.      */
    Vpif_VideoMode_NTSC,
    /**< VPIF operation mode: NTSC - 480 I Video Standard                     */
    Vpif_VideoMode_PAL,
    /**< VPIF operation mode: PAL - 576 I Video Standard                      */
    Vpif_VideoMode_RAW_VGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_SVGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_XGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_SXGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_UXGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_QXGA,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_480P,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_576P,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_720P,
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
    Vpif_VideoMode_RAW_1080P
    /**< VPIF operation mode: Raw Mode - Bayer Pattern GrRBGb only            */
}Vpif_VideoMode;

/**
 * \brief VPIF Raw capture pin polarity
 *
 * This enum defines the polarity of external control signal.
 */
typedef enum Vpif_RawCapturePinPol_t
{
    Vpif_RawCapturePinPol_SAME = 0,
    /**< No inversion.                                                        */
    Vpif_RawCapturePinPol_INVERT
    /**< Invert incoming signal inside the VPIF.                              */
}Vpif_RawCapturePinPol;

/**
 * \brief VPIF Raw capture data width
 *
 * This enum defines the data width for the raw capture mode.
 */
typedef enum Vpif_RawCaptureDataWidth_t
{
    Vpif_RawCaptureDataWidth_8BITS = 0,
    /**< 8 bits/pixel                                                         */
    Vpif_RawCaptureDataWidth_10BITS,
    /**< 10 bits/pixel                                                        */
    Vpif_RawCaptureDataWidth_12BITS
    /**< 12 bits/pixel                                                        */
}Vpif_RawCaptureDataWidth;

/**
 * \brief Dma Request Size
 *
 * Enumerates the request size settings for DMA transfer.
 */
typedef enum Vpif_DmaReqSize_t
{
    Vpif_DmaReqSize_32BYTE,
    /**< Request size of 32 bytes                                             */
    Vpif_DmaReqSize_64BYTE,
    /**< Request size of 64 bytes                                             */
    Vpif_DmaReqSize_128BYTE,
    /**< Request size of 128 bytes                                            */
    Vpif_DmaReqSize_256BYTE
    /**< Request size of 256 bytes                                            */
}Vpif_DmaReqSize;

/**
 * \brief Frame format type
 *
 * Enum which keeps track of kind of the frame format. VPIF supports both
 * interlaced and progressive video formats.
 */
typedef enum Vpif_FrameFormat_t
{
    Vpif_FrameFormat_INTERLACED,
    /**< Interlaced frame format                                              */
    Vpif_FrameFormat_PROGRESSIVE
    /**< Progressive frame format                                             */
}Vpif_FrameFormat;

/**
 * \brief YC Mux mode type
 *
 * Enum which keeps track of Y/C streams are muxed or not.
 */
typedef enum Vpif_YCMuxed_t
{
    Vpif_YCMuxed_NO,
    /**< For BT.656 video, luminance (Y) and chrominance (C) values are
     *   multiplexed into a single byte-stream on one channel.                */
    Vpif_YCMuxed_YES
    /**< For BT.1120 video, channels function as a pair without Y/C
     *   multiplexing.                                                        */
}Vpif_YCMuxed;

/**
 * \brief Capture format type
 *
 * Enum which keeps track of Capture format.
 */
typedef enum Vpif_CaptureFormat_t
{
    Vpif_CaptureFormat_BT,
    /**< BT.xxx  The BT/YC video mode will look for video sync signals that
     *   are embedded within the video byte stream (standard for BT video).   */
    Vpif_CaptureFormat_CCDC
    /**< The CCD/CMOS (Raw Data Capture) mode will look for video sync signals
     *   on the dedicated VPIF sync pins (common for CCD and CMOS sensors).   */
}Vpif_CaptureFormat;

/**
 *  \brief  Mode of the channel operation.
 */
typedef enum Vpif_IoMode_t
{
    Vpif_IoMode_NONE,
    /**< No operation selected                                                */
    Vpif_IoMode_RAW_CAP,
    /**< Raw mode of Capture                                                  */
    Vpif_IoMode_CAP,
    /**< BT mode of Capture                                                   */
    Vpif_IoMode_DIS
    /**< Display mode of operation                                            */
}Vpif_IoMode;

/**
 * \brief  PLL domain to be used by the device
 */
typedef enum Vpif_PllDomain_t
{
    Vpif_PllDomain_0 = 0,
    /**< PLL domain 0       */

    Vpif_PllDomain_1 = 1
    /**< PLL domain 1       */
}Vpif_PllDomain;

/* ========================================================================== */
/*                             VPIF STRUCTURES                                */
/* ========================================================================== */
/**
 *  \brief VPIF NON Selective VBI Params structure
 *
 *  This structure will store vpif parameters for raw vbi/hbi data for capture
 *  and display. This is used to calculate the size of raw vbi and raw hbi
 *  buffers.
 *
 */
typedef struct Vpif_RawVbiParams_t
{
    Uint32                              samplePerLine;
    /**< Byte count of valid data within the ancillary blanking region.       */
    Uint32                              countFld0;
    /**< Line count of valid top field ancillary data                         */
    Uint32                              countFld1;
    /**< Line count of valid bottom field ancillary data                      */
}Vpif_RawVbiParams;


/**
 *  \brief VPIF Selective VBI Params structure
 *
 *  This structure will store vpif parameters for raw vbi/hbi data when VPIF
 *  *SELECTIVELY* wants to display sub-regions in the VBI space. The VPIF can
 *  selectively transmit sub-regions in the VBI space but cannot selectively
 *  receive sub-regions in the VBI space.
 *  Note that the user is expected to place valid ancillary data in a memory
 *  buffer that is representative of the entire VBI region of interest. However,
 *  only the valid ancillary data region needs to be initialized -- the VPIF
 *  will automatically transmit blanking data (Y=10h, C=80h) for non-valid
 *  ancillary data regions.
 *
 */
typedef struct Vpif_RawSelectiveVbiParams_t
{
    Uint32                              vbi0StrtHps;
    /**< Horizontal start of vbi data for first field. Horizontal position
    (byte-count) of valid data within the top field horizontal ancillary
    blanking region. Byte positions are enumerated beginning with 0. The value
    of HPOS must be a multiple of 8.                                          */
    Uint32                              vbi0StrtVps;
    /**< Vertical start of vbi data for first field. Vertical position
    (line-count) of valid data within the top field horizontal ancillary
    blanking region. Line positions are enumerated beginning with 1.          */
    Uint32                              vbi0Hsz;
    /**< Horizontal size of vbi data for first field. Horizontal size
    (byte-count) of valid top field horizontal ancillary data beginning at
    vbi0StrtHps. The value of HSIZE must be a multiple of 8.                  */
    Uint32                              vbi0Vsz;
    /**< Vertical size of vbi data for first field. Vertical size
    (line-count) of valid top field horizontal ancillary data beginning at
    vbi0StrtVps.                                                              */
    Uint32                              vbi1StrtHps;
    /**< Horizontal start of vbi data for second field. Horizontal position
    (byte-count) of valid data within the bottom field horizontal ancillary
    blanking region. Byte positions are enumerated beginning with 0. The value
    of HPOS must be a multiple of 8.                                          */
    Uint32                              vbi1StrtVps;
    /**< Vertical start of vbi data for second field. Vertical position
    (line-count) of valid data within the bottom field horizontal ancillary
    blanking region. Line positions are enumerated beginning with 1.          */
    Uint32                              vbi1Hsz;
    /**< Horizontal size of vbi data for second field. Horizontal size
    (byte-count) of valid bottom field horizontal ancillary data beginning at
    vbi1StrtHps. The value of HSIZE must be a multiple of 8.                  */
    Uint32                              vbi1Vsz;
    /**< Vertical size of vbi data for second field. Vertical size
    (line-count) of valid bottom field horizontal ancillary data beginning at
    vbi1StrtVps.                                                              */
}Vpif_RawSelectiveVbiParams;


/**
 *  \brief VPIF Config Params structure
 *  This structure will store different parameters as per the mode selected by
 *  the user.
 */
typedef struct Vpif_ConfigParams_t
{
    Vpif_VideoMode                      mode;
    /**< Video Standard mode. If the mode is not defined in enum Vpif_VideoMode,
     *   "mode" should be Vpif_VideoMode_NONE.                                */
    Uint16                              width;
    /**< Indicates width of the image for this mode                           */
    Uint16                              height;
    /**< Indicates height of the image for this mode. Active lines            */
    Uint8                               fps;
    /**< Indicates frames per sec for this mode                               */
    Vpif_FrameFormat                    frameFmt;
    /**< Indicates whether this is interlaced or progressive format.          */
    Vpif_YCMuxed                        ycMuxMode;
    /**< Indicates whether this mode requires single or two channels.         */
    Uint16                              eav2sav;
    /**< The number of bytes in the inactive (EAV2SAV) video regions. The
     *   EAV2SAV value must be even.                                          */
    Uint16                              sav2eav;
    /**< The number of bytes in the active (SAV2EAV) video regions. The
     *   SAV2EAV value must be even.                                          */
    Uint16                              l1;
    /**< Enumerated line number for the L1 field position                     */
    Uint16                              l3;
    /**< Enumerated line number for the L3 field position                     */
    Uint16                              l5;
    /**< Enumerated line number for the L5 field position                     */
    Uint16                              l7;
    /**< Enumerated line number for the L7 field position. Note that L7 is not
     *   used with the progressive video mode.                                */
    Uint16                              l9;
    /**< Enumerated line number for the L9 field position. Note that L9 is not
     *   used with the progressive video mode.                                */
    Uint16                              l11;
    /**< Enumerated line number for the L11 field position. Note that L11 is not
     *   used with the progressive video mode.                                */
    Uint16                              vsize;
    /**< Vertical size of the image. Actual lines                             */
    Vpif_CaptureFormat                  captureFormat;
    /**< Indicates whether capture format is in BT or in CCD/CMOS.            */
    Bool                                isVbiSupported;
    /**< Indicates whether this mode supports capturing vbi or not            */
    Bool                                isHd;
    /**< Indicates whether this mode is HD or not                             */

    /*NOTE: Below parameters are valid only if vbi is supported by the
    video mode and isVbiSupported is set to TRUE */
    Uint32                              hancOffset;
    /**< Offset for the horizontal ancillary data                             */
    Vpif_RawVbiParams                   rawHbiParams;
    /**< Raw non selective HBI params                                         */
    Vpif_RawVbiParams                   rawVbiParams;
    /**< Raw non selective VBI params                                         */
}Vpif_ConfigParams;

/**
 *  \brief VPIF Standard Channel Info structure
 *  This structure will store different video channel parameters.
 *
 */
typedef struct Vpif_StdInfo_t
{
    Vpif_VideoMode                      stdMode;
    /**< Video Standard mode                                                  */
    Uint32                              activePixels;
    /**< Same as bytes per line or width                                      */
    Uint32                              activeLines;
    /**< Same as height                                                       */
    Uint16                              framePerSec;
    /**< Frames per second                                                    */
    Vpif_FrameFormat                    stdFrameFormat;
    /**< Frame format - I or P                                                */
    Uint32                              stdVbiService;
    /**< Indicates what all VBI services supported by this mode. Available
    values for this field are defined in "Vpif.h" file with VPIF VBI Ancillary
    Data service title.                                                       */
    Vpif_SdramStorage                   sdramStorage;
    /**< SDRAM storage mode                                                   */
}Vpif_StdInfo;

/**
 *  \brief VPIF Frame buffer settings structure
 *
 */
typedef struct Vpif_FrameBufferParams_t
{
    Uint16                              frmBufAlignment;
    /**< Frame buffer alignment used by driver while allocating memory for video
     *   frame buffer                                                         */
    Int32                               frmBufSegId;
    /**< Memory segment ID, used by driver to allocate video frame buffer     */
}Vpif_FrameBufferParams;

/**
 *  \brief VPIF Capture Params structure
 *  Structure to hold the parameters for Capture. This will act as parameters
 *  for the channel itself passed to vpifMdCreateChan. The parameter structure
 *  below is for capture (BT or RAW) mode only.
 */
typedef struct Vpif_CapChanParams_t
{
    Vpif_VideoMode                      capStdMode;
    /**< Operation mode title. If the value of this mode is Vpif_VideoMode_NONE,
     *   it suggests that user do not want to use internal lookup table for
     *   video parameters.                                                    */
    Vpif_IoMode                         capChannelIoMode;
    /**< Operation mode for which the channel is opened                       */
    Vpif_FrameBufferParams              capFbParams;
    /**< Frame buffer settings                                                */
    Vpif_SdramStorage                   capStorageMode;
    /**< Indicates whether it is field or frame based storage mode. This is
    only applicable for interlaced mode of operation                          */
    EDC_Fxns                            *capEdcTbl;
    /**< Function table of decoder module for the channel. A statically
    defined EDC function table is passed to the vpifMdCreateChan() function via
    the channel parameters argument. If NULL is passed then it is assumed that
    there is no EDC available for that channel.                               */
    Vpif_ConfigParams                   *capVideoParams;
    /**< Specify the Video parameters if application would like to specify them.
    This is an optional parameter. If not used, set this element to NULL. If set
    to NULL, the driver will read the video parameters depending upon the "mode"
    set. If it is not NULL, its value will prevail over whatever mode being set.
    In this case the mode parameter in "videoParams" should be
    Vpif_VideoMode_NONE. CAUTION: If wrong parameters are sent, the driver does
    not verify the validity of these parameters.                              */

    /*NOTE : Below parameters are only for BT capture they are not valid for
    RAW capture mode. Ancillary data is only supported for BT byte streams. */
    Uint32                              capVbiService;
    /**< Indicates what type VBI services are required by this mode. Available
    values for this field are defined in "Vpif.h" file with VPIF VBI Ancillary
    Data service title.                                                       */
    Uint32                              capVbiSliceService;
    /**< If the VBI type is Slice VBI then what kind of service it is. Valid
    only if one of the vbiService is set as Vpif_VbiServiceType_SLICE_VBI.
    Whatever slice service is set here only that data is captured. Available
    values for this field are defined in "Fvid.h" file with FVID Slice VBI
    service type title.                                                       */

    /*NOTE : Below params are only for RAW capture mode they are not valid for
    BT mode*/
    Vpif_RawCaptureDataWidth            capDataSize;
    /**< The data width bit is only used with the CCD/CMOS data capture mode. */
    Vpif_RawCapturePinPol               capFieldPol;
    /**< Field ID polarity inverting control                                  */
    Vpif_RawCapturePinPol               capVPixPol;
    /**< Vertical pixel valid signal polarity control.                        */
    Vpif_RawCapturePinPol               capHPixPol;
    /**< Horizontal pixel valid signal polarity control.                      */
}Vpif_CapChanParams;


/**
 * \brief  VPIF Display Params structure
 *  Structure to hold the parameters for Display. This will act as parameters
 *  for the channel itself passed to vpifMdCreateChan. The parameter structure
 *  below is for BT display only.
 */
typedef struct Vpif_DisChanParams_t
{
    Vpif_VideoMode                      dispStdMode;
    /**< Operation mode title. If the value of this mode is Vpif_VideoMode_NONE,
     *   it suggests that user do not want to use internal lookup table for
     *   video parameters.                                                    */
    Vpif_IoMode                         dispChannelIoMode;
    /**< Operation mode for which the channel is opened                       */
    Vpif_FrameBufferParams              dispFbParams;
    /**< Frame buffer settings                                                */
    Vpif_SdramStorage                   dispStorageMode;
    /**< Indicates whether it is field or frame based storage mode. This is
    only applicable for interlaced mode of operation                          */
    EDC_Fxns                            *dispEdcTbl;
    /**< Function table of encoder module for the channel. A statically
    defined EDC function table is passed to the vpifMdCreateChan() function via
    the channel parameters argument. If NULL is passed then it is assumed that
    there is no EDC available for that channel.                               */
    Vpif_ConfigParams                   *dispVideoParams;
    /**< Specify the Video parameters if application would like to specify them.
    This is an optional parameter. If not used, set this element to NULL. If set
    to NULL, the driver will read the video parameters depending upon the "mode"
    set. If it is not NULL, its value will prevail over whatever mode being set.
    In this case the mode parameter in "videoParams" should be
    Vpif_VideoMode_NONE. CAUTION: If wrong parameters are sent, the driver does
    not verify the validity of these parameters.                              */

    /*Ancillary data is only supported for BT byte streams.*/
    Uint32                              dispVbiService;
    /**< Indicates what type VBI services are required by this mode. Available
    values for this field are defined in "Vpif.h" file with VPIF VBI Ancillary
    Data service title.                                                       */
    Uint32                              dispVbiSliceService;
    /**< If the VBI type is Slice VBI then what kind of service it is. Valid
    only if one of the vbiService is set as Vpif_VbiServiceType_SLICE_VBI.
    Whatever slice service is set here only that data is displayed. Available
    values for this field are defined in "Fvid.h" file with FVID Slice VBI
    service type title.                                                       */
    Vpif_RawSelectiveVbiParams          *dispVVbiParams;
    /**< Indicates the parameters for selective Vertical blanking data. Value
    of NULL suggests that selective sub-regions in the VBI space are not
    required. For selectively sub-regions in the VBI space this should hold
    appropriate value                                                         */
    Vpif_RawSelectiveVbiParams          *dispHVbiParams;
    /**< Indicates the parameters for selective Horizontal blanking data. Value
    of NULL suggests that selective sub-regions in the HBI space are not
    required. For selectively sub-regions in the HBI space this should hold
    appropriate value                                                         */
}Vpif_DisChanParams;


/* ========================================================================== */
/*                       PER-INSTANCE CONFIGS                                 */
/* ========================================================================== */
/**
 *  \brief VPIF Instance configuration parameter structure
 *
 *  A pointer to such a structure is used when driver is instantiated
 *
 */
typedef struct Vpif_Params_t
{
    Uint16                              hwiNumber;
    /**< HWI number associated with this device event. The value of this
     *   depends on which ECM block the VPIF ISR fall.                        */
    Vpif_DmaReqSize                     dmaReqSize;
    /**< Request size for DMA data transfer from/to VPIF. Data size is either
     *   luminance or chrominance.                                            */
    Bool                                pscPwrmEnable;
    /**< Control for module power management enable & disable                 */
    Vpif_PllDomain                      pllDomain;
    /**< Pll domain where the device is                                       */
}Vpif_Params;


/**
 *  \brief Default Vpif_Params structure
 *
 *  Following values are defaults and application can assign and change
 *  interested parameters.
 *
 *  const Vpif_Params Vpif_PARAMS = {
 *
 *      9,                          hwiNumber
 *
 *      Vpif_DmaReqSize_128BYTE,    dmaReqSize
 *
 *      FALSE,                      pscPwrmEnable
 *
 *      Vpif_PllDomain_0            pllDomain
 *
 *  };
 */
extern const Vpif_Params Vpif_PARAMS;


/**
 *  \brief Vpif IOM_Fxns table
 *
 *  Driver function table to be used by applications.
 */
extern IOM_Fxns Vpif_IOMFXNS;


/**
 *  \brief    Initializes Vpif instances which are statically allocated
 *
 *  This function needs to be called as part of BIOS initialization by
 *  setting initFxn for that particular UDEV instance or by calling this
 *  function as part of user specific initFxn.
 */
Void Vpif_init(Void);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* _VPIF_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
