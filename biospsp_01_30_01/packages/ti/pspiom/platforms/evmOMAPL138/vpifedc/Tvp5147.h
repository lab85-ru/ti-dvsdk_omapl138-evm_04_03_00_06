/*
 * Tvp5147.h
 *
 * This file contains Application programming interface for the Tvp5147 decoder
 * driver and command/macro definitions used by the Tvp5147 decoder driver.
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
 *  \file   Tvp5147.h
 *
 *  \brief  This file contains configuration structure and enums for TVP5147
 *          decoder.
 *          User-programmable VBI formats for TVP5147 :
 *          Sliced VBI data during a vertical blanking period or active video
 *          period (full field mode)
 *            - CC and extended data service (EDS)
 *            - Wide screen signaling (WSS)
 *            - Copy generation management system (CGMS)
 *
 */

#ifndef _TVP5147_H_
#define _TVP5147_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include "ti/pspiom/vpif/Edc.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             TVP5147 DEFINES                                */
/* ========================================================================== */

/* ========================================================================== */
/*                             TVP5147 ENUMS                                  */
/* ========================================================================== */
/**
 *  \brief  TVP5147 Ioctl commands
 * Following IOCTL commands will be used to perform control operation on TVP5147
 * device. The following Ioctl commands are specifically for TVP5147. The
 * IOCTL command is passed as second argument to ctrl() function pointer of the
 * TVP5147 decoder device
 * NOTE: These IOCTL's will be available only if user adds Vpif_IOCTL_CMD_MAX
 * and Edc_IOCTL_CMD_MAX to these IOCTL calls, from application.
 *
 */
typedef enum Tvp5147_IOCTL_t
{
    Tvp5147_IOCTL_POWERDOWN = 0,
    /**< TVP5147 power down command.
     *   This ioctl will power down the TVP5147 decoder. Only this ioctl command
     *   needs passed as second argument in ctrl() function. This doesn't take
     *   any argument.                                                        */
    Tvp5147_IOCTL_POWERUP,
    /**< TVP5147 power up command.
     *   This ioctl will power up the TVP5147 decoder. Only this ioctl command
     *   needs passed as second argument in ctrl() function. This doesn't take
     *   any argument.                                                        */
    Tvp5147_IOCTL_SET_CONTROL,
    /**< Set control for TVP5147.
     *   Application has to pass pointer to Tvp5147_Control structure variable
     *   as a third argument in ctrl() function.
     *   For example
     *   Tvp5147_Control control;
     *   control.tvpVidtype = Edc_VideoType_SD;
     *   control.tvpCtrlId = Tvp5147_ControlId_BRIGHTNESS;
     *   control.tvpValue = 128;
     *   cmdArg = (&control);                                                 */
    Tvp5147_IOCTL_SET_SLICE_VBI_SERVICE,
    /**< Set Slice VBI services for TVP5147.
     *   Application has to pass pointer to Int32 structure variable
     *   as a third argument in ctrl() function. Available values for this
     *   field are defined with TVP5147 Slice VBI Service title.
     *   NOTE : This ioctl does not check whether current set standard supports
     *   the slice service or not. It just sets them.
     *   For example
     *   Uint32 services;
     *   services = Fvid_SLICE_VBI_SERVICES_CC_NTSC;
     *   cmdArg = (&services);                                                */
    Tvp5147_IOCTL_CMD_MAX
    /**< Book-keep - Max ioctl's                                              */
}Tvp5147_IOCTL;

/**
 * Apart from above IOCTLS the following generic EDC IOCTLS are supported
 *
 * Edc_IOCTL_CONFIG     - Argument is a pointer to Tvp5147_ConfParams structure
 * Edc_IOCTL_RESET      - Doesn't take any argument
 * Edc_IOCTL_SET_REG    - Argument is a pointer to Edc_RegData structure
 * Edc_IOCTL_GET_REG    - Argument is a pointer to Edc_RegData structure
 * Edc_IOCTL_READ_SLICE_VBI_DATA  - Argument is a pointer to FVID_SliceFrame
 *                                  structure
 *
 */

/**
 *  \brief  Tvp5147_ControlId - Constants used for control settings for TVP5147.
 */
typedef enum Tvp5147_ControlId_t
{
    Tvp5147_ControlId_AUTO_GAIN,
    /**< Gain control. A value of 0 sets Manual gain, and value of 1 enables
         auto gain. Auto gain is either 0 or 1.                               */
    Tvp5147_ControlId_BRIGHTNESS,
    /**< Brightness control. A value of 255 (bright), 128 (default), 0 (dark).
         Brightness supported is (0-255).                                     */
    Tvp5147_ControlId_CONTRAST,
    /**< Contrast control(Luminance Contrast). A value of 255(maximum contrast),
         128 (default), 0 (minimum contrast).
         Contrast supported is - 0 - 255 (Default - 128)                      */
    Tvp5147_ControlId_HUE,
    /**< Hue control. A value of 0x80(-180 degrees) or 0x7F(+180 degrees) or
         0(0 degrees) can be used. HUE does not apply to component video.
         Hue Supported is - 0 - 255 (Default - 0)                             */
    Tvp5147_ControlId_SATURATION
    /**< Saturation(Chrominance Saturation). Control A value of 255 (maximum),
         128 (default), 0 (no color)
         Saturation supported is - 0 - 255 (Default - 128)                    */
}Tvp5147_ControlId;


/**
 *  \brief Tvp5147 video operation mode
 *  This enum gives available operation mode for Tvp5147 decoder
 */
typedef enum Tvp5147_Std_t
{
    Tvp5147_Std_INVALID = -1,
    /**< Invalid Video Mode                                                   */
    Tvp5147_Std_AUTO,
    /**< Use auto switch mode of operation. The standard will be detected
    automatically                                                             */
    Tvp5147_Std_NTSC720,
    /**< NTSC 720 pixels                                                      */
    Tvp5147_Std_PAL720
    /**< PAL 720 pixels                                                       */
}Tvp5147_Std;


/**
 *  \brief Tvp5147 analog format
 *  This enum gives available analog format selection with Tvp5147 decoder
 */
typedef enum Tvp5147_AnalogFormat_t
{
    Tvp5147_AnalogFormat_SVIDEO = 0,
    /**< SVIDEO(Y/C) IN cable used                                            */
    Tvp5147_AnalogFormat_COMPOSITE
    /**< CVBS IN cable used                                                   */
}Tvp5147_AnalogFormat;

/**
 *  \brief Tvp5147 output format
 *  This enum gives available output format for Tvp5147 decoder
 */
typedef enum Tvp5147_OutputFormat_t
{
    Tvp5147_OutputFormat_YCBCR422
    /**< Interlaced YCbCr 422 output                                          */
}Tvp5147_OutputFormat;

/* ========================================================================== */
/*                             TVP5147 STRUCTURES                             */
/* ========================================================================== */
/**
 *  \brief Setting Control data structure for Tvp5147
 */
typedef struct Tvp5147_Control_t
{
    Edc_VideoType                       tvpVidtype;
    /**< Video Type for this control feature                                  */
    Tvp5147_ControlId                   tvpCtrlId;
    /**< Control Id defined for TVP5147                                       */
    Uint32                              tvpValue;
    /**< Value to be write to the control reg                                 */
}Tvp5147_Control;

/**
 *  \brief TVP5147 configuration structure
 *  This structure contains configuration parameters for TVP5147 decoder
 */
typedef struct Tvp5147_ConfParams_t
{
    Tvp5147_AnalogFormat                    tvpAnaFmt;
    /**< Indicates analog input format for TVP5147                            */
    Tvp5147_Std                             tvpMode;
    /**< Indicates operation mode(NTSC/PAL) for TVP5147                       */
    Tvp5147_OutputFormat                    tvpOutFmt;
    /**< Indicates output format for TVP5147                                  */
    Uint32                                  tvpServices;
    /**< Type of Slice VBI service. Available values for this field are defined
         in "Fvid.h" file with FVID Slice VBI Service title. This should
         be passed appropriately  according to the Video standard mode desired.
         CAUTION : If wrong service is sent, the driver does not verify its
         validity                                                             */
}Tvp5147_ConfParams;

/* ========================================================================== */
/*                             TVP5147 EXTERNS                                */
/* ========================================================================== */
/** \brief Decoder (TVP5147) driver function table                            */
extern EDC_Fxns TVP5147_Fxns;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* #ifndef _TVP5147_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
