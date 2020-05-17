/*
 * Adv7343Local.h
 *
 * Adv7343 encoder Driver internal header file
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
 *  \file   Adv7343Local.h
 *
 *  \brief  This file contains internal structures and defines for ADV7343
 *          encoder
 */

#ifndef _ADV7343LOCAL_H_
#define _ADV7343LOCAL_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/Adv7343.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/**\brief Indicates one ADV7343 encoders */
#define NUM_ENCODERS                        (1)
/**\brief Number of channels supported by one encoder */
#define NUM_CHANNELS                        (1)
/**\brief ADV7343 8bit I2C address.  */
#define ADV7343_IIC_ADDR                    (0x54 >> 1)

/**\brief Defines for ADV7343 System Registers.*/
#define ADV7343_POWER_MODE_REG              0x00u
#define ADV7343_MODE_SELECT_REG             0x01u
#define ADV7343_MODE_REG0                   0x02u
#define ADV7343_CSC_MATRIX0                 0x03u
#define ADV7343_CSC_MATRIX1                 0x04u
#define ADV7343_CSC_MATRIX2                 0x05u
#define ADV7343_CSC_MATRIX3                 0x06u
#define ADV7343_CSC_MATRIX4                 0x07u
#define ADV7343_CSC_MATRIX5                 0x08u
#define ADV7343_CSC_MATRIX6                 0x09u
#define ADV7343_DAC1_OUTPUT_LEVEL           0x0Au
#define ADV7343_DAC2_OUTPUT_LEVEL           0x0Bu
#define ADV7343_DAC_POWER_MODE              0x0Du
#define ADV7343_CABLE_DETECTION             0x10u
#define ADV7343_SBUS_READ                   0x12u
#define ADV7343_YBUS_READ                   0x13u
#define ADV7343_CBUS_READ                   0x14u
#define ADV7343_CONTROL_READ                0x16u
#define ADV7343_SOFT_RESET                  0x17u
#define ADV7343_HD_MODE_REG1                0x30u
#define ADV7343_HD_MODE_REG2                0x31u
#define ADV7343_HD_MODE_REG3                0x32u
#define ADV7343_HD_MODE_REG4                0x33u
#define ADV7343_HD_MODE_REG5                0x34u
#define ADV7343_HD_MODE_REG6                0x35u
#define ADV7343_HD_Y_LEVEL                  0x36u
#define ADV7343_HD_CR_LEVEL                 0x37u
#define ADV7343_HD_CB_LEVEL                 0x38u
#define ADV7343_HD_MODE_REG7                0x39u
#define ADV7343_HD_SHARPNESS_FLTR_GAIN      0x40u
#define ADV7343_HD_CGMS_DATA_0              0x41u
#define ADV7343_HD_CGMS_DATA_1              0x42u
#define ADV7343_HD_CGMS_DATA_2              0x43u
#define ADV7343_HD_GAMMA_A0                 0x44u
#define ADV7343_HD_GAMMA_A1                 0x45u
#define ADV7343_HD_GAMMA_A2                 0x46u
#define ADV7343_HD_GAMMA_A3                 0x47u
#define ADV7343_HD_GAMMA_A4                 0x48u
#define ADV7343_HD_GAMMA_A5                 0x49u
#define ADV7343_HD_GAMMA_A6                 0x4au
#define ADV7343_HD_GAMMA_A7                 0x4bu
#define ADV7343_HD_GAMMA_A8                 0x4cu
#define ADV7343_HD_GAMMA_A9                 0x4du
#define ADV7343_HD_GAMMA_B0                 0x4Eu
#define ADV7343_HD_GAMMA_B1                 0x4Fu
#define ADV7343_HD_GAMMA_B2                 0x50u
#define ADV7343_HD_GAMMA_B3                 0x51u
#define ADV7343_HD_GAMMA_B4                 0x52u
#define ADV7343_HD_GAMMA_B5                 0x53u
#define ADV7343_HD_GAMMA_B6                 0x54u
#define ADV7343_HD_GAMMA_B7                 0x55u
#define ADV7343_HD_GAMMA_B8                 0x56u
#define ADV7343_HD_GAMMA_B9                 0x57u
#define ADV7343_HD_ADPT_FLTR_GAIN1          0x58u
#define ADV7343_HD_ADPT_FLTR_GAIN2          0x59u
#define ADV7343_HD_ADPT_FLTR_GAIN3          0x5Au
#define ADV7343_HD_ADPT_FLTR_THRLDA         0x5Bu
#define ADV7343_HD_ADPT_FLTR_THRLDB         0x5Cu
#define ADV7343_HD_ADPT_FLTR_THRLDC         0x5Du
#define ADV7343_HD_CGMS_B0                  0x5Eu
#define ADV7343_HD_CGMS_B1                  0x5Fu
#define ADV7343_HD_CGMS_B2                  0x60u
#define ADV7343_HD_CGMS_B3                  0x61u
#define ADV7343_HD_CGMS_B4                  0x62u
#define ADV7343_HD_CGMS_B5                  0x63u
#define ADV7343_HD_CGMS_B6                  0x64u
#define ADV7343_HD_CGMS_B7                  0x65u
#define ADV7343_HD_CGMS_B8                  0x66u
#define ADV7343_HD_CGMS_B9                  0x67u
#define ADV7343_HD_CGMS_B10                 0x68u
#define ADV7343_HD_CGMS_B11                 0x69u
#define ADV7343_HD_CGMS_B12                 0x6Au
#define ADV7343_HD_CGMS_B13                 0x6Bu
#define ADV7343_HD_CGMS_B14                 0x6Cu
#define ADV7343_HD_CGMS_B15                 0x6Du
#define ADV7343_HD_CGMS_B16                 0x6Eu

#define ADV7343_SD_MODE_REG1                0x80u
#define ADV7343_SD_MODE_REG2                0x82u
#define ADV7343_SD_MODE_REG3                0x83u
#define ADV7343_SD_MODE_REG4                0x84u
#define ADV7343_SD_MODE_REG5                0x86u
#define ADV7343_SD_MODE_REG6                0x87u
#define ADV7343_SD_MODE_REG7                0x88u
#define ADV7343_SD_MODE_REG8                0x89u
#define ADV7343_SD_TIMING_REG0              0x8Au
#define ADV7343_SD_TIMING_REG1              0x8Bu
#define ADV7343_SD_FSC_REG0                 0x8Cu
#define ADV7343_SD_FSC_REG1                 0x8Du
#define ADV7343_SD_FSC_REG2                 0x8Eu
#define ADV7343_SD_FSC_REG3                 0x8Fu
#define ADV7343_SD_FSC_PHASE                0x90u
#define ADV7343_SD_CLOSE_CAPTION_EVEN0      0x91u
#define ADV7343_SD_CLOSE_CAPTION_EVEN1      0x92u
#define ADV7343_SD_CLOSE_CAPTION_ODD0       0x93u
#define ADV7343_SD_CLOSE_CAPTION_ODD1       0x94u
#define ADV7343_SD_PEDESTAL_REG0            0x95u
#define ADV7343_SD_PEDESTAL_REG1            0x96u
#define ADV7343_SD_PEDESTAL_REG2            0x97u
#define ADV7343_SD_PEDESTAL_REG3            0x98u
#define ADV7343_SD_CGMS_WSS0                0x99u
#define ADV7343_SD_CGMS_WSS1                0x9Au
#define ADV7343_SD_CGMS_WSS2                0x9Bu

#define ADV7343_SD_SCALE_LSB                0x9Cu
#define ADV7343_SD_Y_SCALE                  0x9Du
#define ADV7343_SD_CB_SCALE                 0x9Eu
#define ADV7343_SD_CR_SCALE                 0x9Fu

#define ADV7343_SD_HUE_REG                  0xA0u
#define ADV7343_SD_BRIGHTNESS_WSS           0xA1u
#define ADV7343_SD_LUMA_SSAF                0xA2u
#define ADV7343_SD_DNR0                     0xA3u
#define ADV7343_SD_DNR1                     0xA4u
#define ADV7343_SD_DNR2                     0xA5u

#define ADV7343_SD_GAMMA_A0                 0xA6u
#define ADV7343_SD_GAMMA_A1                 0xA7u
#define ADV7343_SD_GAMMA_A2                 0xA8u
#define ADV7343_SD_GAMMA_A3                 0xA9u
#define ADV7343_SD_GAMMA_A4                 0xAAu
#define ADV7343_SD_GAMMA_A5                 0xABu
#define ADV7343_SD_GAMMA_A6                 0xACu
#define ADV7343_SD_GAMMA_A7                 0xADu
#define ADV7343_SD_GAMMA_A8                 0xAEu
#define ADV7343_SD_GAMMA_A9                 0xAFu
#define ADV7343_SD_GAMMA_B0                 0xB0u
#define ADV7343_SD_GAMMA_B1                 0xB1u
#define ADV7343_SD_GAMMA_B2                 0xB2u
#define ADV7343_SD_GAMMA_B3                 0xB3u
#define ADV7343_SD_GAMMA_B4                 0xB4u
#define ADV7343_SD_GAMMA_B5                 0xB5u
#define ADV7343_SD_GAMMA_B6                 0xB6u
#define ADV7343_SD_GAMMA_B7                 0xB7u
#define ADV7343_SD_GAMMA_B8                 0xB8u
#define ADV7343_SD_GAMMA_B9                 0xB9u
#define ADV7343_SD_BRIGHTNESS_DETECT        0xBAu
#define ADV7343_FIELD_COUNT_REG             0xBBu

/** \brief Defines for Configuration Values */
#define ADV7343_SD_BRIGHTNESS_MIN           0
#define ADV7343_SD_BRIGHTNESS_MAX           127
#define ADV7343_SD_BRIGHTNESS_DEF           (3)
#define ADV7343_SD_HUE_MIN                  0
#define ADV7343_SD_HUE_MAX                  255
#define ADV7343_SD_HUE_DEF                  (127)
#define ADV7343_SD_GAIN_MAX                 (255)
#define ADV7343_SD_GAIN_MIN                 (0)
#define ADV7343_SD_GAIN_DEF                 (0)


/** \brief Default values for the registers */
#define ADV7343_POWER_MODE_REG_DEFAULT      0x10u   /* Changed */
#define ADV7343_MODE_SELECT_REG_DEFAULT     0x00
#define ADV7343_MODE_REG0_DEFAULT           0x20u   /* Doubt on Sync signals */
#define ADV7343_DAC1_OUTPUT_LEVEL_DEFAULT   0x00
#define ADV7343_DAC2_OUTPUT_LEVEL_DEFAULT   0x00
#define ADV7343_HD_MODE_REG1_DEFAULT        0x3Cu
/* Changed Default 720p and EAV/SAV code */
#define ADV7343_HD_MODE_REG2_DEFAULT        0x01u
/* Changed Pixel data valid */
#define ADV7343_HD_MODE_REG3_DEFAULT        0x00
/* Color delay is 0 clks */
#define ADV7343_HD_MODE_REG4_DEFAULT        0xE8u   /* Changed */
#define ADV7343_HD_MODE_REG5_DEFAULT        0x08u
#define ADV7343_HD_MODE_REG6_DEFAULT        0x00u
#define ADV7343_HD_MODE_REG7_DEFAULT        0x00u

#define ADV7343_SD_MODE_REG1_DEFAULT        0x10u
#define ADV7343_SD_MODE_REG2_DEFAULT        0xC9u
#define ADV7343_SD_MODE_REG3_DEFAULT        0x00u
#define ADV7343_SD_MODE_REG4_DEFAULT        0x00u
#define ADV7343_SD_MODE_REG5_DEFAULT        0x02u
#define ADV7343_SD_MODE_REG6_DEFAULT        0x00u
#define ADV7343_SD_MODE_REG7_DEFAULT        0x24u
#define ADV7343_SD_MODE_REG8_DEFAULT        0x00u
#define ADV7343_SD_TIMING_REG0_DEFAULT      0x08u
#define ADV7343_SD_TIMING_REG1_DEFAULT      0x00
#define ADV7343_SOFT_RESET_DEFAULT          0x02u
/** \brief ADV7343 output mode values */
#define ADV7343_COMPOSITE_POWER_VALUE       0x10u
#define ADV7343_SVIDEO_POWER_VALUE          0x0Cu
#define ADV7343_SD_HUE_REG_DEFAULT          127u
#define ADV7343_SD_BRIGHTNESS_WSS_DEFAULT   0x03u
#define ADV7343_SD_CGMS_WSS0_DEFAULT        0x10u

/** \brief Bit masks for Mode Select Register */
#define YC_Y_BUS                            0x80
#define YC_S_BUS                            0x7F
#define INPUT_MODE_MASK                     0x70
#define SD_INPUT_MODE                       0x00
#define HD_720P_INPUT_MODE                  0x10
#define HD_1080I_INPUT_MODE                 0x10
#define HD_DDR_INPUT_MODE                   0x20
#define SD_HD_SDR_INPUT_MODE                0x30
#define SD_HD_DDR_INPUT_MODE                0x40
#define ED_INPUT_MODE                       0x70

/** \brief Bit masks for Mode Register 0 */
#define TEST_PATTERN_BLACK_BAR_EN           0x04
#define TEST_PATTERN_BLACK_BAR_DI           0xFB
#define YUV_OUTPUT_SELECT                   0x20u
#define RGB_OUTPUT_SELECT                   0xDF
#define SD_SYNC_OUTPUT_EN                   0x40
#define SD_SYNC_OUTPUT_DI                   0xBF
#define HD_SYNC_OUTPUT_EN                   0x80
#define HD_SYNC_OUTPUT_DI                   0x7F
#define CSC_MATRIX_EN                       0x08
#define CSC_MATRIX_DI                       0xF7

/** \brief Bit masks for CSC matrices */
#define CSC_LSB_MASK                        0x03
#define CSC_MSB_MASK                        0xFF
#define CSC_SHIFT                           2
#define CSC_MATRIX_MASK                     0x3FF

/** \brief Bit masks for DAC output levels */
#define DAC_OUTPUT_LEVEL_MASK               0xFF
#define POSITIVE_GAIN_MAX                   0x40
#define POSITIVE_GAIN_MIN                   0x00
#define NEGATIVE_GAIN_MAX                   0xFF
#define NEGATIVE_GAIN_MIN                   0xC0

/** \brief Bit masks for soft reset register */
#define SOFT_RESET                          0x02

/** \brief Bit masks for HD Mode Register 1 */
#define OUTPUT_STD_MASK                     0x03
#define OUTPUT_STD_SHIFT                    0
#define OUTPUT_STD_EIA0_2                   0x00
#define OUTPUT_STD_EIA0_1                   0x01
#define OUTPUT_STD_FULL                     0x02
#define EMBEDDED_SYNC                       0x04
#define EXTERNAL_SYNC                       0xFB
#define STD_MODE_SHIFT                      3
#define STD_MODE_MASK                       0x1F
#define STD_MODE_720P                       0x05
#define STD_MODE_720P_25                    0x08
#define STD_MODE_720P_30                    0x07
#define STD_MODE_720P_50                    0x06
#define STD_MODE_1080I                      0x0D
#define STD_MODE_1080I_25fps                0x0E
#define STD_MODE_1080P_24                   0x12
#define STD_MODE_1080P_25                   0x10
#define STD_MODE_1080P_30                   0x0F
#define STD_MODE_525P                       0x00
#define STD_MODE_625P                       0x03

/** \brief Bit masks for HD Mode Register 2 */
#define HD_PIXEL_DATA_VALID                 0x01
#define HD_TEST_PATTERN_EN                  0x04
#define HD_TEST_PATTERN_DI                  0xFB
#define HD_TEST_PATTERN_HATCH               0xF7
#define HD_TEST_PATTERN_FRAME               0x08
#define HD_VBI_EN                           0x10
#define HD_VBI_DI                           0xEF
#define HD_UNDERSHOOTER_DI                  0x9F
#define HD_UNDERSHOOTER_11IRE               0x20
#define HD_UNDERSHOOTER_6IRE                0x40
#define HD_UNDERSHOOTER_1_5IRE              0x60
#define HD_SHARPNESS_FLTR_EN                0x80
#define HD_SHARPNESS_FLTR_DI                0x7F

/** \brief Bit masks for HD Mode Register 3 */
#define HD_HSYNC_Y_DELAY_SHIFT              0
#define HD_HSYNC_Y_DELAY_MASK               0x07
#define HD_HSYNC_C_DELAY_SHIFT              3
#define HD_HSYNC_C_DELAY_MASK               0x38
#define HD_CGMS_EN                          0x40
#define HD_CGMS_DI                          0xBF
#define HD_CGMS_CRC_EN                      0x80
#define HD_CGMS_CRC_DI                      0x7F

/** \brief Bit masks for HD Mode Register 4 */
#define HD_HSYNC_CR                         0x01
#define HD_HSYNC_CB                         0xFE
#define HD_SYNC_FLTR_EN                     0x08
#define HD_SYNC_FLTR_DI                     0xF7
#define HD_CHROMA_SSAF_EN                   0x20
#define HD_CHROMA_SSAF_DI                   0xDF
#define HD_CHROMA_INPUT_422                 0x40
#define HD_CHROMA_INPUT_444                 0xBF
#define HD_DOUBLE_BUFFERING_EN              0x80
#define HD_DOUBLE_BUFFERING_DI              0x7F

/** \brief Bit masks for HD Mode Register 5 */
#define HD_MACROVISION_EN                   0x10
#define HD_MACROVISION_DI                   0xEF

/** \brief Bit masks for HD Mode Register 6 */
#define HD_RGB_INPUT_EN                     0x02
#define HD_RGB_INPUT_DI                     0xFD
#define HD_PBPR_SYNC_EN                     0x04
#define HD_PBPR_SYNC_DI                     0xFB
#define HD_DAC_SWAP_EN                      0x08
#define HD_DAC_SWAP_DI                      0xF7
#define HD_GAMMA_CURVE_A                    0xEF
#define HD_GAMMA_CURVE_B                    0x10
#define HD_GAMMA_EN                         0x20
#define HD_GAMMA_DI                         0xDF
#define HD_ADPT_FLTR_MODEB                  0x40
#define HD_ADPT_FLTR_MODEA                  0xBF
#define HD_ADPT_FLTR_EN                     0x80
#define HD_ADPT_FLTR_DI                     0x7F

/** \brief Bit masks for HD Sharpness filter */
#define HD_SHARPNESS_FLTR_A_SHIFT           0
#define HD_SHARPNESS_FLTR_A_MASK            0x0F
#define HD_SHARPNESS_FLTR_B_SHIFT           4
#define HD_SHARPNESS_FLTR_B_MASK            0x0F

#define GAMMA_MASK                          0xFF

#define HD_ADPT_FLTR_GAIN_A_SHIFT           0
#define HD_ADPT_FLTR_GAIN_A_MASK            0x0F
#define HD_ADPT_FLTR_GAIN_B_SHIFT           4
#define HD_ADPT_FLTR_GAIN_B_MASK            0x0F
#define HD_ADPT_FLTR_THRLD_MASK             0xFF

/** \brief Bit masks for SD Mode Register 1 */
#define SD_STD_MASK                         0x03u
#define SD_STD_NTSC                         0x00u
#define SD_STD_PAL_BDGHI                    0x01u
#define SD_STD_PAL_M                        0x02
#define SD_STD_PAL_N                        0x03
#define SD_LUMA_FLTR_MASK                   0x7
#define SD_LUMA_FLTR_SHIFT                  0x2
#define SD_CHROMA_FLTR_MASK                 0x7
#define SD_CHROMA_FLTR_SHIFT                0x5

/** \brief Bit masks for SD Mode Register 2 */
#define SD_PBPR_SSAF_EN                     0x01
#define SD_PBPR_SSAF_DI                     0xFE
#define SD_DAC_1_DI                         0xFDu
#define SD_DAC_2_DI                         0xFBu
#define SD_PEDESTAL_EN                      0x08
#define SD_PEDESTAL_DI                      0xF7
#define SD_SQUARE_PIXEL_EN                  0x10
#define SD_SQUARE_PIXEL_DI                  0xEF
#define SD_PIXEL_DATA_VALID                 0x40
#define SD_ACTIVE_EDGE_EN                   0x80
#define SD_ACTIVE_EDGE_DI                   0x7F

/** \brief Bit masks for SD Mode Register 3 */
#define SD_CLOSE_CAPTION_DI                 0x9Fu
#define SD_CLOSE_CAPTION_EVEN               0x40
#define SD_CLOSE_CAPTION_ODD                0x20
#define SD_CLOSE_CAPTION_BOTH               0x60u
#define SD_VBI_EN                           0x10
#define SD_VBI_DI                           0xEF
#define SD_PEDESTAL_YPBPR_EN                0x01
#define SD_PEDESTAL_YPBPR_DI                0xFE

/** \brief Bit masks for SD Mode Register 4 */
#define SD_CHROMA_EN                        0x10
#define SD_CHROMA_DI                        0xEF
#define SD_BURST_EN                         0x20
#define SD_BURST_DI                         0xDF
#define SD_COLOR_BARS_EN                    0x40u
#define SD_COLOR_BARS_DI                    0xBFu

/** \brief Bit masks for SD Mode Register 6 */
#define SD_PBPR_SCALE_EN                    0x01
#define SD_PBPR_SCALE_DI                    0xFE
#define SD_Y_SCALE_EN                       0x02
#define SD_Y_SCALE_DI                       0xFD
#define SD_HUE_ADJST_EN                     0x04u
#define SD_HUE_ADJST_DI                     0xFB
#define SD_BRIGHTNESS_EN                    0x08u
#define SD_BRIGHTNESS_DI                    0xF7
#define SD_LUMA_SSAF_GAIN_EN                0x10
#define SD_LUMA_SSAF_GAIN_DI                0xEF
#define SD_AUTO_DETECT_EN                   0x20u
#define SD_AUTO_DETECT_DI                   0xBF
#define SD_RGB_INPUT_EN                     0x80
#define SD_RGB_INPUT_DI                     0x7F

/** \brief Bit masks for SD Mode Register 7 */
#define SD_NON_INTERLACED_EN                0x02
#define SD_NON_INTERLACED_DI                0xFD
#define SD_DOUBLE_BUFFERING_EN              0x04
#define SD_DOUBLE_BUFFERING_DI              0xFB
#define SD_DNR_EN                           0x20
#define SD_DNR_DI                           0xDF
#define SD_GAMMA_EN                         0x40u
#define SD_GAMMA_DI                         0xBFu
#define SD_GAMMA_CURVE_B                    0x80u
#define SD_GAMMA_CURVE_A                    0x7Fu
#define SD_INPUT_FORMAT_8BIT                0x00
#define SD_INPUT_FORMAT_16BIT               0x08

/** \brief Bit masks for SD Timing Register 0 */
#define SD_MASTER_MODE                      0x01
#define SD_SLAVE_MODE                       0xFE
#define SD_TIMING_MODE_SHIFT                1
#define SD_TIMING_MODE_MASK                 0x03
#define SD_TIMING_MODE0                     0xFC
#define SD_TIMING_MODE1                     0x01
#define SD_TIMING_MODE2                     0x02
#define SD_TIMING_MODE3                     0x03
#define SD_LUMA_DELAY                       0
#define SD_MIN_LUMA_VAL_7_5IRE              0x40
#define SD_MIN_LUMA_VAL_40IRE               0xBF
#define SD_TIMING_RESET                     0x80

/** \brief Macros for Timing Mode Register 1 */
#define SD_HSYNC_DELAY                      0
#define SD_HSYNC2VSYNC_DELAY                0
#define SD_HSYNC2DATA_DELAY                 0
#define SD_HSYNC2PIXELDATA_ADJUST           0

#define SD_FSC_REG_MASK                     0xFF
#define SD_CLOSED_CAPT_MASK                 0xFF
#define SD_PEDESTAL_MASK                    0xFF

#define SD_CGMS_CRC_EN                      0x10
#define SD_CGMS_CRC_DI                      0xEF
#define SD_CGMS_EN                          0x60u
#define SD_CGMS_ODD_FIELD_EN                0x20
#define SD_CGMS_ODD_FIELD_DI                0xDF
#define SD_CGMS_EVEN_FIELD_EN               0x40
#define SD_CGMS_EVEN_FIELD_DI               0xBF
#define SD_CGMS_DI                          0x9Fu
#define SD_WSS_EN                           0x80u
#define SD_WSS_DI                           0x7Fu

#define SD_BRIGHTNESS_MASK                  0x7F

#define SD_DNR_BORDER_GAIN_MASK             0x0F
#define SD_DNR_BORDER_GAIN_SHIFT            0x0
#define SD_DNR_DATA_GAIN_MASK               0x0F
#define SD_DNR_DATA_GAIN_SHIFT              0x4

#define SD_DNR_THRESHOLD_MASK               0x3F
#define SD_DNR_THRESHOLD_MAX                0x3F
#define SD_BORDER_AREA_4PIXELS              0x40
#define SD_BORDER_AREA_2PIXELS              0xBF
#define SD_BLOCK_SIZE_16x16                 0x80
#define SD_BLOCK_SIZE_8x8                   0x7F
#define SD_DNR_INPUT_SELECT_MASK            0x07
#define SD_DNR_SHARPNESS_MODE               0x08
#define SD_DNR_MODE                         0xF7
#define SD_DNR_BLOCK_OFFSET_MASK            0x0F
#define SD_DNR_BLOCK_OFFSET_SHIFT           0x04
#define SD_DNR_GAIN_MAX                     0x08

#define SD_YPBPR_SCALE_SHIFT                0x02
#define SD_YPBPR_SCALE_LMASK                0x03
#define SD_YPBPR_SCALE_HMASK                0x3FC
#define SD_YPBPR_SCALE_MASK                 0x3FF

#define SD_LUMA_SSAF_GAIN_MAX               0x0C

/** \brief Macro for maximum number of slice services supported */
#define ADV7343_VBI_NUM_SERVICES            (3u)

/* ========================================================================== */
/*                              STRUCTURES                                    */
/* ========================================================================== */
/**
 *  \brief Slice service register structure
 */
typedef struct Adv7343_ServiceReg_t
{
    Uint32 service;
    /**< Slice service type                                                   */
    Uint8 reg;
    /**< Register for that slice service id                                   */
    Uint8 enableVal;
    /**< Value to be written to enable the service                            */
    Uint8 disableVal;
    /**< Value to be written to disable the service                           */
    Adv7343_Std std;
    /**< Video standard                                                       */
}Adv7343_ServiceReg;


/**
 *  \brief Structure holding value of Slice Service data address registers
 *         required per field. This structure holds the no of registers required
 *         to be written to put the Slice VBI data.
 */
typedef struct Adv7343_FieldAddr_t
{
    Uint8 addr[2];
    /**< Per field data registers required                                    */
}Adv7343_FieldAddr;


/**
 *  \brief Structure for writing Slice services data
 */
typedef struct Adv7343_ServiceDataReg_t
{
    Uint32 advSliceService;
    /**< Slice service type                                                   */
    Adv7343_FieldAddr field[2];
    /**< Slice service data register address for even and odd fields          */
    Uint8 bytestowrite;
    /**< Bytes of Slice data                                                  */
}Adv7343_ServiceDataReg;


/**
 *  \brief Instance State object
 *  Encoder object structure for Adv7343 encoder
 */
typedef struct Adv7343_EncObj_t
{
    Int8                    codecName[20];
    /**< Encoder name                                                         */
    Edc_ControlBusType      edcCtrlBusType;
    /**< Type of the control bus (SPI or I2C)                                 */
    Int8                    edcCtrlBusName[20];
    /**< Control bus (I2C or SPI) name                                        */
    Uint8                   edcCtrlDeviceAddr;
    /**< Encoder Control bus address                                          */
    Uint32                  servicesSet;
    /**< Slice VBI service set for encoder                                    */
    Uint8                   numServices;
    /**< Number of slice vbi service set                                      */
    Adv7343_Std             currentVideoStd;
    /**< Current standard for Video                                           */
    Edc_Callback            sliceCbFxn;
    /**< The callback function to be called for asynce calls                  */
    Ptr                     sliceCbData;
    /**< Callback data to be passed to the callback function                  */
}Adv7343_EncObj;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* #ifndef __ADV7343LOCAL_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
