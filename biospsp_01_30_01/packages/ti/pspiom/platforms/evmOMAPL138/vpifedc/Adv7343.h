/*
 * Adv7343.h
 *
 * This file contains Application programming interface for the Adv7343 encoder
 * driver and command/macro definitions used by the Adv7343 encoder driver.
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
 *  \file   Adv7343.h
 *
 *  \brief  This file contains configuration structure and enums for ADV7343
 *          encoder. The Standard definition (SD) programmable features for
 *          slice VBI are:
 *              - Copy generation management system (CGMS)
 *              - Wide screen signaling
 *              - Closed captioning
 *
 */

#ifndef _ADV7343_H_
#define _ADV7343_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include "ti/pspiom/vpif/Edc.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             ADV7343 DEFINES                                */
/* ========================================================================== */
/** \brief Maximum Gamma Coefficients                                         */
#define ADV7343_MAX_GAMMA_COEFFS                10u

/* ========================================================================== */
/*                             ADV7343 ENUMS                                  */
/* ========================================================================== */
/**
 *  \brief  ADV7343 Ioctl commands
 * Following IOCTL commands will be used to perform control operation on ADV7343
 * device. The following Ioctl commands are specifically for ADV7343. The
 * IOCTL command is passed as second argument to ctrl() function pointer of the
 * ADV7343 encoder device
 * NOTE: These IOCTL's will be available only if user adds Vpif_IOCTL_CMD_MAX
 * and Edc_IOCTL_CMD_MAX to these IOCTL calls, from application.
 *
 */
typedef enum Adv7343_IOCTL_t
{
    Adv7343_IOCTL_POWERDOWN = 0,
    /**< ADV7343 power down command.
     *   This ioctl will power down the ADV7343 encoder. Only this ioctl command
     *   needs passed as second argument in ctrl() function. This doesn't take
     *   any argument.                                                        */
    Adv7343_IOCTL_POWERUP,
    /**< ADV7343 power up command.
     *   This ioctl will power up the ADV7343 encoder. Only this ioctl command
     *   needs passed as second argument in ctrl() function. This doesn't take
     *   any argument.                                                        */
    Adv7343_IOCTL_ENABLE_COLORBAR,
    /**< ADV7343 internal color bar enable or disable command.
     *   Application has to pass pointer to Bool variable in  third argument
     *   in ctrl() function. The value of TRUE - Enables color bar and
     *   FALSE - Disables color bar
     *   For example
     *   Bool enable = TRUE;
     *   cmdArg = (&enable);                                                 */
    Adv7343_IOCTL_SET_CONTROL,
    /**< Set control for ADV7343.
     *   Application has to pass pointer to Adv7343_Control structure variable
     *   as a third argument in ctrl() function.
     *   For example
     *   Adv7343_Control control;
     *   control.advVidtype = Edc_VideoType_SD;
     *   control.advCtrlId = Adv7343_ControlId_BRIGHTNESS;
     *   control.advValue = 0x1F;
     *   cmdArg = (&control);                                                 */
    Adv7343_IOCTL_SET_GAMMA,
    /**< Set gamma for ADV7343.
     *   Application has to pass pointer to Adv7343_GammaParams structure
     *   variable as a third argument in ctrl() function.
     *   For example
     *   Adv7343_GammaParams gammaParams;
     *   Uint8 coeff[ADV7343_MAX_GAMMA_COEFFS];
     *   gammaParams.type = Edc_VideoType_SD;
     *   gammaParams.enGamma = TRUE;
     *   gammaParams.curve = Adv7343_GammaCurve_A;
     *   gammaParams.coeff = coeff;
     *   cmdArg = (&gammaParams);                                             */
    Adv7343_IOCTL_SET_SLICE_VBI_SERVICE,
    /**< Set Slice VBI services for ADV7343.
     *   Application has to pass pointer to Int32 structure variable
     *   as a third argument in ctrl() function. Available values for this field
     *   are defined with ADV7343 Slice VBI Service title.
     *   NOTE : This ioctl does not check whether current set standard supports
     *   the slice service or not. It just sets them.
     *   For example
     *   Uint32 services;
     *   services = Fvid_SLICE_VBI_SERVICES_CC_NTSC;
     *   cmdArg = (&services);                                                */
    Adv7343_IOCTL_CMD_MAX
    /**< Book-keep - Max ioctl's                                              */
}Adv7343_IOCTL;

/**
 * Apart from above IOCTLS the following generic EDC IOCTLS are supported
 *
 * Edc_IOCTL_CONFIG     - Argument is a pointer to Adv7343_ConfParams structure
 * Edc_IOCTL_RESET      - Doesn't take any argument
 * Edc_IOCTL_SET_REG    - Argument is a pointer to Edc_RegData structure
 * Edc_IOCTL_GET_REG    - Argument is a pointer to Edc_RegData structure
 * Edc_IOCTL_WRITE_SLICE_VBI_DATA  - Argument is a pointer to FVID_SliceFrame
 *                                   structure
 *
 */

/**
 *  \brief ADV7343 Gamma Curve
 *  This enum is used to select gamma curve on ADV7343 encoder
 */
typedef enum Adv7343_GammaCurve_t
{
    Adv7343_GammaCurve_A = 0,
    /**< Gamma curve A                                                        */
    Adv7343_GammaCurve_B
    /**< Gamma curve B                                                        */
}Adv7343_GammaCurve;


/**
 *  \brief Adv7343_ControlId - Constants used for control type IDs
 */
typedef enum Adv7343_ControlId_t
{
    Adv7343_ControlId_BRIGHTNESS,
    /**< Brightness control
         Brightness supported is (0-127), Values in the range of 0x3F to 0x44
         could result in an invalid output signal                             */
    Adv7343_ControlId_HUE
    /**< Hue control
         Hue Supported is -
         For normal operation (zero adjustment), value is set to 0x80. Values
         0xFF and 0x00 represent the upper and lower limits, respectively,
         of the attainable adjustment in NTSC mode. Values 0xFF and
         0x01 represent the upper and lower limits, respectively, of the
         attainable adjustment in PAL mode.                                   */
}Adv7343_ControlId;


/**
 *  \brief ADV7343 video operation mode
 *  This enum gives available operation mode for ADV7343 encoder
 */
typedef enum Adv7343_Std_t
{
    Adv7343_Std_INVALID = -1,
    /**< Invalid Video Mode                                                   */
    Adv7343_Std_AUTO,
    /**< Use auto switch mode of operation. The standard will be detected
    automatically                                                             */
    Adv7343_Std_NTSC720,
    /**< NTSC 720 pixels                                                      */
    Adv7343_Std_PAL720
    /**< PAL 720 pixels                                                       */
}Adv7343_Std;


/**
 *  \brief ADV7343 analog format
 *  This enum gives available analog format selection with ADV7343 encoder
 */
typedef enum Adv7343_AnalogFormat_t
{
    Adv7343_AnalogFormat_SVIDEO = 0,
    /**< SVIDEO(Y/C) out cable used                                           */
    Adv7343_AnalogFormat_COMPOSITE
    /**< CVBS out cable used                                                  */
}Adv7343_AnalogFormat;

/**
 *  \brief ADV7343 input format
 *  This enum gives available input format for ADV7343 encoder
 */
typedef enum Adv7343_InputFormat_t
{
    Adv7343_InputFormat_YCBCR422
    /**< Interlaced YCbCr 422 input                                           */
}Adv7343_InputFormat;

/* ========================================================================== */
/*                             ADV7343 STRUCTURES                             */
/* ========================================================================== */
/**
 *  \brief ADV7343 gamma parameter structure
 */
typedef struct Adv7343_GammaParams_t
{
    Edc_VideoType                       type;
    /**< Video Type for this gamma feature                                    */
    Bool                                enGamma;
    /**< Enables/disables gamma correction                                    */
    Adv7343_GammaCurve                  curve;
    /**< Selects gamma correction curve                                       */
    Uint8                               coeff[ADV7343_MAX_GAMMA_COEFFS];
    /**< Gamma correction coefficients                                        */
}Adv7343_GammaParams;

/**
 *  \brief Setting Control data structure for Adv7343
 */
typedef struct Adv7343_Control_t
{
    Edc_VideoType                       advVidtype;
    /**< Video Type for this control feature                                  */
    Adv7343_ControlId                   advCtrlId;
    /**< Control Id defined for ADV7343                                       */
    Uint32                              advValue;
    /**< Value to be write to the control reg                                 */
}Adv7343_Control;

/**
 *  \brief  ADV7343 configuration parameters structure
 *  Structure to contain configuration parameters for ADV7343 encoder
 */
typedef struct Adv7343_ConfParams_t
{
    Adv7343_AnalogFormat                    advAnaFmt;
    /**< indicates analog output format for ADV7343 */
    Adv7343_Std                             advMode;
    /**< Indicates operation mode(NTSC/PAL) for ADV7343 */
    Adv7343_InputFormat                     advInFmt;
    /**< Selection for digital input format for ADV7343 */
    Uint32                                  advServices;
    /**< Type of Slice VBI service. Available values for this field are defined
         in "Fvid.h" file with FVID Slice VBI Service title. This should
         be passed appropriately according to the Video standard mode desired.
         CAUTION : If wrong service is sent, the driver does not verify its
         validity                                                             */
}Adv7343_ConfParams;

/* ========================================================================== */
/*                             ADV7343 EXTERNS                                */
/* ========================================================================== */
/** \brief Encoder (ADV7343) driver function table                            */
extern EDC_Fxns ADV7343_Fxns;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* #ifndef _ADV7343_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
