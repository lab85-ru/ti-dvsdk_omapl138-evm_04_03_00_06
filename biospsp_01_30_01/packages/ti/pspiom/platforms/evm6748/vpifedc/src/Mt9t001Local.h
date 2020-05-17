/*
 * Mt9t001Local.h
 *
 * Mt9t001 sensor Driver internal header file
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
 *  \file   Mt9t001Local.h
 *
 *  \brief  MT9T001 external Image Sensor related parameters. This file contains
 *          the interfaces, data types and symbolic definitions that are
 *          required for the MT9T001 external image sensor interface.
 */

#ifndef _MT9T001LOCAL_H_
#define _MT9T001LOCAL_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/Mt9t001.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/**\brief MT9T001 8bit I2C address.  */
#define MT9T001_IIC_ADDR                    (0xBA >> 1)

/**
 *  \brief Defines for MT9T001 System Registers
 */
#define MT9T001_CHIP_VERSION_REG            (0x00u)
/**< Chip Version Register                      */
#define MT9T001_ROW_START_REG               (0x01u)
/**< Row Start Register                         */
#define MT9T001_COL_START_REG               (0x02u)
/**< Column Start Register                      */
#define MT9T001_ROW_SIZE_REG                (0x03u)
/**< Row Size Register                          */
#define MT9T001_COL_SIZE_REG                (0x04u)
/**< Column Size Register                       */
#define MT9T001_HORZ_BLANKING_REG           (0x05u)
/**< Horizontal Blanking Register               */
#define MT9T001_VERT_BLANKING_REG           (0x06u)
/**< Vertical Blanking Register                 */
#define MT9T001_OUTPUT_CONTROL_REG          (0x07u)
/**< Output Control Register                    */
#define MT9T001_SHUTTER_WIDTH_UPPER_REG     (0x08u)
/**< Shutter Width Upper Register               */
#define MT9T001_SHUTTER_WIDTH_REG           (0x09u)
/**< Shutter Width Register                     */
#define MT9T001_PIXEL_CLK_CONTROL_REG       (0x0Au)
/**< Pixel Clock Control Register               */
#define MT9T001_RESTART_REG                 (0x0Bu)
/**< Restart Register                           */
#define MT9T001_SHUTTER_DELAY_REG           (0x0Cu)
/**< Shutter Delay Register                     */
#define MT9T001_RESET_REG                   (0x0Du)
/**< Reset Register                             */
#define MT9T001_READ_MODE1_REG              (0x1Eu)
/**< Read Mode1 Register                        */
#define MT9T001_READ_MODE2_REG              (0x20u)
/**< Read Mode2 Register                        */
#define MT9T001_READ_MODE3_REG              (0x21u)
/**< Read Mode3 Register                        */
#define MT9T001_ROW_ADDR_MODE_REG           (0x22u)
/**< Row Address Mode Register                  */
#define MT9T001_COL_ADDR_MODE_REG           (0x23u)
/**< Column Address Mode Register               */
#define MT9T001_GREEN1_GAIN_REG             (0x2Bu)
/**< Green 1 Gain Register                      */
#define MT9T001_BLUE_GAIN_REG               (0x2Cu)
/**< Blue Gain Register                         */
#define MT9T001_RED_GAIN_REG                (0x2Du)
/**< Red Gain Register                          */
#define MT9T001_GREEN2_GAIN_REG             (0x2Eu)
/**< Green2 Gain Register                       */
#define MT9T001_GLOBAL_GAIN_REG             (0x35u)
/**< Global Gain Register                       */
#define MT9T001_BLACK_LEVEL_REG             (0x49u)
/**< Black Level Register                       */
#define MT9T001_ROW_BLACK_DEF_OFFSET_REG    (0x4Bu)
/**< Row Black Default Offset Register          */
#define MT9T001_CAL_COARSE_THRESHOLD_REG    (0x5Du)
/**< Coarse Thresholds Register                 */
#define MT9T001_CAL_TARGET_THRESHOLD_REG    (0x5Fu)
/**< Target Thresholds Register                 */
#define MT9T001_GREEN1_OFFSET_REG           (0x60u)
/**< Green1 Offset Register                     */
#define MT9T001_GREEN2_OFFSET_REG           (0x61u)
/**< Green2 Offset Register                     */
#define MT9T001_BLK_LEVEL_CALIBRATION_REG   (0x62u)
/**< Black Level Calibration  Register          */
#define MT9T001_RED_OFFSET_REG              (0x63u)
/**< Red Offset Register                        */
#define MT9T001_BLUE_OFFSET_REG             (0x64u)
/**< Blue Offset Register                       */
#define MT9T001_CHIP_ENABLE_SYNC_REG        (0xF8u)
/**< Chip Enable/Synchronize Register           */
#define MT9T001_CHIP_VERSION_END_REG        (0xFFu)
/**< Chip Version End Register                  */

/**\brief Define Shift and Mask for gain register*/
#define	MT9T001_ANALOG_GAIN_SHIFT	        (0x0000)
#define	MT9T001_DIGITAL_GAIN_SHIFT	        (8)
#define	MT9T001_ANALOG_GAIN_MASK	        (0x007F)
#define	MT9T001_DIGITAL_GAIN_MASK	        (0x7F00)

/**\brief Define Shift and Mask for black level caliberation register*/
#define	MT9T001_MANUAL_OVERRIDE_MASK		(0x0001)
#define	MT9T001_DISABLE_CALLIBERATION_SHIFT	(1)
#define	MT9T001_DISABLE_CALLIBERATION_MASK	(0x0002)
#define	MT9T001_RECAL_BLACK_LEVEL_SHIFT		(12)
#define	MT9T001_RECAL_BLACK_LEVEL_MASK		(0x1000)
#define	MT9T001_LOCK_RB_CALIBRATION_SHIFT	(13)
#define	MT9T001_LOCK_RB_CALLIBERATION_MASK	(0x2000)
#define	MT9T001_LOCK_GREEN_CALIBRATION_SHIFT	(14)
#define	MT9T001_LOCK_GREEN_CALLIBERATION_MASK	(0x4000)
#define	MT9T001_LOW_COARSE_THELD_MASK		(0x007F)
#define	MT9T001_HIGH_COARSE_THELD_SHIFT		(8)
#define	MT9T001_HIGH_COARSE_THELD_MASK		(0x7F00)
#define	MT9T001_LOW_TARGET_THELD_MASK		(0x007F)
#define	MT9T001_HIGH_TARGET_THELD_SHIFT		(8)
#define	MT9T001_HIGH_TARGET_THELD_MASK		(0x7F00)
#define	MT9T001_SHUTTER_WIDTH_LOWER_MASK	(0xFFFF)
#define	MT9T001_SHUTTER_WIDTH_UPPER_SHIFT	(16)
#define	MT9T001_SHUTTER_WIDTH_UPPER_MASK	(0xFFFF)
#define	MT9T001_ROW_START_MASK			    (0x07FF)
#define	MT9T001_COL_START_MASK			    (0x0FFF)
#define	 MT9T001_GREEN1_OFFSET_MASK 		(0x01FF)
#define	 MT9T001_GREEN2_OFFSET_MASK 		(0x01FF)
#define	 MT9T001_RED_OFFSET_MASK 		    (0x01FF)
#define	 MT9T001_BLUE_OFFSET_MASK 		    (0x01FF)

/**\brief defines for MT9T001 register values */
#define	MT9T001_NORMAL_OPERATION_MODE		(0x0002)
#define	MT9T001_HALT_MODE			        (0x0003)
#define MT9T001_RESET_ENABLE			    (0x0001)
#define MT9T001_RESET_DISABLE			    (0x0000)
#define	MT9T001_INVERT_PIXEL_CLK		    (0x8000)
#define MT9T001_GAIN_MINVAL			        (0)
#define MT9T001_GAIN_MAXVAL			        (128)
#define MT9T001_GAIN_STEP			        (1)
#define MT9T001_GAIN_DEFAULTVAL			    (8)

/**\brief Default values for MT9T001 registers */
#define MT9T001_ROW_START_DEFAULT		    (0x14)
#define MT9T001_COL_START_DEFAULT		    (0x20)
#define MT9T001_HEIGHT_DEFAULT			    (0x5FF)
#define MT9T001_WIDTH_DEFAULT			    (0x7FF)
#define MT9T001_HBLANK_DEFAULT			    (0x8E)
#define MT9T001_VBLANK_DEFAULT			    (0x19)
#define MT9T001_OUTPUT_CONTROL_DEFAULT      (0x0002u)
/**< Output control default setting               */
#define MT9T001_SHUTTER_WIDTH_UPPER_DEFAULT (0x0000u)
/**< Shutter width default setting                */
#define MT9T001_SHUTTER_WIDTH_DEFAULT		(0x619)
#define MT9T001_PIXEL_CLK_CTRL_DEFAULT		(0x0)
#define MT9T001_RESTART_DEFAULT             (0x0000u)
/**< Restart default setting                      */
#define MT9T001_SHUTTER_DELAY_DEFAULT       (0x0000u)
/**< Shutter delay default setting                */
#define MT9T001_RESET_DEFAULT               (0x0000u)
/**< Reset default setting                        */
#define MT9T001_READ_MODE1_DEFAULT          (0x8040u)
/**< Read mode1 default setting                   */
#define MT9T001_READ_MODE2_DEFAULT          (0x2000u)
/**< Read mode2 default setting                   */
#define MT9T001_READ_MODE3_DEFAULT          (0x0000u)
/**< Read mode3 default setting                   */
#define MT9T001_ROW_ADDR_MODE_DEFAULT		(0x0)
#define MT9T001_COL_ADDR_MODE_DEFAULT		(0x0)
#define MT9T001_GREEN1_GAIN_DEFAULT         (0x0008u)
/**< Green1 gain default setting                  */
#define MT9T001_BLUE_GAIN_DEFAULT           (0x0008u)
/**< Blue gain default setting                    */
#define MT9T001_RED_GAIN_DEFAULT            (0x0008u)
/**< Res gain default setting                     */
#define MT9T001_GREEN2_GAIN_DEFAULT         (0x0008u)
/**< Green2 gain default setting                  */
#define MT9T001_GLOBAL_GAIN_DEFAULT         (0x0008u)
/**< Global gain default setting                  */
#define MT9T001_BLACK_LEVEL_DEFAULT		    (0xA8)
#define MT9T001_CAL_COARSE_THRESHOLD_DEFAULT (0x2D13u)
/**< Coarse Thresholds default setting            */
#define MT9T001_CAL_TARGET_THRESHOLD_DEFAULT  (0x231Du)
/**< Target Thresholds default setting            */
#define MT9T001_GREEN1_OFFSET_DEFAULT         (0x0020u)
/**< Green1 Offset default setting                */
#define MT9T001_GREEN2_OFFSET_DEFAULT         (0x0020u)
/**< Green2 Offset default setting                */
#define MT9T001_BLK_LEVEL_CALIBRATION_DEFAULT (0x0000u)
/**< Black Level Calibration  default setting     */
#define MT9T001_RED_OFFSET_DEFAULT            (0x0020u)
/**< Red Offset default setting                   */
#define MT9T001_BLUE_OFFSET_DEFAULT           (0x0020u)
/**< Blue Offset default setting                  */
#define MT9T001_CHIP_ENABLE_SYNC_DEFAULT      (0x0001u)
/**< Chip Enable/Synchronize default setting      */

/* ========================================================================== */
/*                              STRUCTURES                                    */
/* ========================================================================== */
/**
 *  \brief Instance State object
 *  Sensor object structure for MT9T001 external Image Sensor
 */
typedef struct MT9T001_SensorObj_t
{
    Int8                    sensorName[20];
    /**< Sensor name                                                          */
    Edc_ControlBusType      edcCtrlBusType;
    /**< Type of the control bus (SPI or I2C)                                 */
    Int8                    edcCtrlBusName[20];
    /**< Control bus (I2C or SPI) name                                        */
    Uint8                   edcCtrlDeviceAddr;
    /**< Sensor Control bus address                                           */
}MT9T001_SensorObj;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* #ifndef _MT9T001LOCAL_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
