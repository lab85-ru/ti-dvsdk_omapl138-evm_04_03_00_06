/*
 * Mt9t001.h
 *
 * This file contains Application programming interface for the Mt9t001 sensor
 * driver and command/macro definitions used by the Mt9t001 sensor driver.
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
 *  \file   Mt9t001.h
 *
 *  \brief  This file contains configuration structure and enums for MT9T001
 *          external Image Sensor.
 *
 */

#ifndef _MT9T001_H_
#define _MT9T001_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include "ti/pspiom/vpif/Edc.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                             MT9T001 ENUMS                                  */
/* ========================================================================== */
/**
 *  \brief  MT9T001 Ioctl commands
 * Following IOCTL commands will be used to perform control operation on MT9T001
 * device. These are the generic EDC IOCTLS are supported
 *
 * Edc_IOCTL_CONFIG  - Argument is a pointer to Mt9t001_ConfParams structure
 * Edc_IOCTL_RESET   - Doesn't take any argument
 * Edc_IOCTL_SET_REG - Argument is a pointer to Edc_RegData structure. This
 *                     IOCTL is supported with restriction that only one
 *                     register can be written to the MT9T001 device also note
 *                     the way register values should be passed. For example:
 *     Edc_RegData regval;
 *     Uint8 val[2];
 *     regval.startReg = 0x07u;
 *     regval.noRegToRW = 1u;
 *     val[0] = 0x00; NOTE: MSB value of register
 *     val[1] = 0x42; NOTE: LSB value of register
 *     regval.value = (&val);
 * Edc_IOCTL_GET_REG - This IOCTL is not suported by MT9T001 device as some
 *                     registers when the driver tries to read back and verify,
 *                     it will give errors.
 *
 */


/* define for various video format supported by MT9T001 driver
*/
typedef enum MT9T001_StandardFormat_t
{
    MT9T001_MODE_VGA,
    MT9T001_MODE_SVGA,
    MT9T001_MODE_XGA,
    MT9T001_MODE_480P,
    MT9T001_MODE_576P,
    MT9T001_MODE_720P,
    MT9T001_MODE_1080P,
    MT9T001_MODE_QXGA,
    MT9T001_MODE_UXGA,
    MT9T001_MODE_SXGA
}MT9T001_StandardFormat;

/* ========================================================================== */
/*                             MT9T001 STRUCTURES                             */
/* ========================================================================== */
/**
 *  \brief MT9T001 Format structure
 *  Format structure for changing the MT9T001 external Image Sensor resolution
 */
typedef struct MT9T001_FormatParams_t
{
    Uint16 columnSize;
    /**< Value to be written in Col Size Register: 0x04*/
    Uint16 rowSize;
    /**< Value to be written in Row Size Register: 0x03*/
    Uint16 hBlank;
    /**< Value to be written in Horizontal Blanking Register: 0x05*/
    Uint16 vBlank;
    /**< Value to be written in Vertical Blanking Register: 0x06*/
    Uint32 shutterWidth;
    /**< Value to be written in Shutter Width Register - lower mask: 0x09 and
     *   Shutter Width Register - upper: 0x08 */
    Uint16 blackLevel;
    /**< Value to be written in Black Level Register: 0x49 */
    Uint16 pixelClockCtrl;
    /**< Value to be written in Pixel Clock Control Register: 0x0A */
    Uint16 rowStart;
    /**< Value to be written in Row Start Register: 0x01*/
    Uint16 colStart;
    /**< Value to be written in Col Start Register: 0x02 */
}MT9T001_FormatParams;

/**
 *  \brief  MT9T001 configuration parameters structure
 *  Structure to contain MT9T001 register setting values
 */
typedef struct Mt9t001_ConfParams_t
{
    MT9T001_FormatParams                    *fmtParams;
    /**< If not NULL, indicates the individual parameters ("params") are sent by
     *   application and they should be set instead of driver "stdFormat" for
     *   a standard. If NULL then parameters for "stdFormat" inside the driver
     *   are set.                                                             */
    MT9T001_StandardFormat                  stdFormat;
    /**< Indicates Standard format for MT9T001. This field is not valid if
     *   "fmtParams" is not NULL.                                             */
}Mt9t001_ConfParams;

/* ========================================================================== */
/*                             MT9T001 EXTERNS                                */
/* ========================================================================== */
/** \brief External Image Sensor (MT9T001) driver function table              */
extern EDC_Fxns MT9T001_Fxns;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* #ifndef _MT9T001_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
