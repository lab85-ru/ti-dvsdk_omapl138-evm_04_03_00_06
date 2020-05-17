/*
 * Tvp5147Local.h
 *
 * Tvp5147 decoder Driver internal header file
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
 *  \file   Tvp5147Local.h file
 *
 *  \brief  This file contains internal structures and defines for TVP5147
 *          decoder
 */

#ifndef _TVP5147LOCAL_H_
#define _TVP5147LOCAL_H_

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/Tvp5147.h"

#ifdef __cplusplus
extern "C" {
#endif /* extern "C" */

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/**\brief Indicates two TVP5147 decoders                                      */
#define NUM_DECODERS                    (2)
/**\brief Number of channels supported by one decoder                         */
#define NUM_CHANNELS                    (1)
/**\brief TVP5147 #0 I2C address */
#define TVP5147_0_IIC_ADDR             (0xB8 >> 1)
/**\brief TVP5147 #1 I2C address */
#define TVP5147_1_IIC_ADDR             (0xBA >> 1)


/**\brief Defines for TVP5147 register address                                */
#define TVP5147_INPUT_SEL                                       (0x00u)
#define TVP5147_AFE_GAIN_CTRL                                   (0x01u)
#define TVP5147_VIDEO_STD                                       (0x02u)
#define TVP5147_OPERATION_MODE                                  (0x03u)
#define TVP5147_AUTOSWITCH_MASK                                 (0x04u)

#define TVP5147_COLOR_KILLER                                    (0x05u)
#define TVP5147_LUMA_CONTROL1                                   (0x06u)
#define TVP5147_LUMA_CONTROL2                                   (0x07u)
#define TVP5147_LUMA_CONTROL3                                   (0x08u)

#define TVP5147_BRIGHTNESS                                      (0x09u)
#define TVP5147_CONTRAST                                        (0x0Au)
#define TVP5147_SATURATION                                      (0x0Bu)
#define TVP5147_HUE                                             (0x0Cu)

#define TVP5147_CHROMA_CONTROL1                                 (0x0Du)
#define TVP5147_CHROMA_CONTROL2                                 (0x0Eu)

/* 0x0F Reserved */

#define TVP5147_COMP_PR_SATURATION                              (0x10u)
#define TVP5147_COMP_Y_CONTRAST                                 (0x11u)
#define TVP5147_COMP_PB_SATURATION                              (0x12u)

/* 0x13 Reserved */

#define TVP5147_COMP_Y_BRIGHTNESS                               (0x14u)

/* 0x15 Reserved */

#define TVP5147_AVID_START_PIXEL_LSB                            (0x16u)
#define TVP5147_AVID_START_PIXEL_MSB                            (0x17u)
#define TVP5147_AVID_STOP_PIXEL_LSB                             (0x18u)
#define TVP5147_AVID_STOP_PIXEL_MSB                             (0x19u)

#define TVP5147_HSYNC_START_PIXEL_LSB                           (0x1Au)
#define TVP5147_HSYNC_START_PIXEL_MSB                           (0x1Bu)
#define TVP5147_HSYNC_STOP_PIXEL_LSB                            (0x1Cu)
#define TVP5147_HSYNC_STOP_PIXEL_MSB                            (0x1Du)

#define TVP5147_VSYNC_START_LINE_LSB                            (0x1Eu)
#define TVP5147_VSYNC_START_LINE_MSB                            (0x1Fu)
#define TVP5147_VSYNC_STOP_LINE_LSB                             (0x20u)
#define TVP5147_VSYNC_STOP_LINE_MSB                             (0x21u)

#define TVP5147_VBLK_START_LINE_LSB                             (0x22u)
#define TVP5147_VBLK_START_LINE_MSB                             (0x23u)
#define TVP5147_VBLK_STOP_LINE_LSB                              (0x24u)
#define TVP5147_VBLK_STOP_LINE_MSB                              (0x25u)

/* 0x26 - 0x27 Reserved */

#define TVP5147_FAST_SWTICH_CONTROL                             (0x28u)

/* 0x29 Reserved */

#define TVP5147_FAST_SWTICH_SCART_DELAY                         (0x2Au)

/* 0x2B Reserved */

#define TVP5147_SCART_DELAY                                     (0x2Cu)
#define TVP5147_CTI_DELAY                                       (0x2Du)
#define TVP5147_CTI_CONTROL                                     (0x2Eu)

/* 0x2F - 0x31 Reserved */

#define TVP5147_SYNC_CONTROL                                    (0x32u)
#define TVP5147_OUTPUT_FORMATTER1                               (0x33u)
#define TVP5147_OUTPUT_FORMATTER2                               (0x34u)
#define TVP5147_OUTPUT_FORMATTER3                               (0x35u)
#define TVP5147_OUTPUT_FORMATTER4                               (0x36u)
#define TVP5147_OUTPUT_FORMATTER5                               (0x37u)
#define TVP5147_OUTPUT_FORMATTER6                               (0x38u)
#define TVP5147_CLEAR_LOST_LOCK                                 (0x39u)

#define TVP5147_STATUS1                                         (0x3Au)
#define TVP5147_STATUS2                                         (0x3Bu)

#define TVP5147_AGC_GAIN_STATUS_LSB                             (0x3Cu)
#define TVP5147_AGC_GAIN_STATUS_MSB                             (0x3Du)

/* 0x3E Reserved */

#define TVP5147_VIDEO_STD_STATUS                                (0x3Fu)
#define TVP5147_GPIO_INPUT1                                     (0x40u)
#define TVP5147_GPIO_INPUT2                                     (0x41u)

/* 0x42 - 0x45 Reserved */

#define TVP5147_AFE_COARSE_GAIN_CH1                             (0x46u)
#define TVP5147_AFE_COARSE_GAIN_CH2                             (0x47u)
#define TVP5147_AFE_COARSE_GAIN_CH3                             (0x48u)
#define TVP5147_AFE_COARSE_GAIN_CH4                             (0x49u)

#define TVP5147_AFE_FINE_GAIN_PB_B_LSB                          (0x4Au)
#define TVP5147_AFE_FINE_GAIN_PB_B_MSB                          (0x4Bu)
#define TVP5147_AFE_FINE_GAIN_Y_G_CHROMA_LSB                    (0x4Cu)
#define TVP5147_AFE_FINE_GAIN_Y_G_CHROMA_MSB                    (0x4Du)
#define TVP5147_AFE_FINE_GAIN_PR_R_LSB                          (0x4Eu)
#define TVP5147_AFE_FINE_GAIN_PR_R_MSB                          (0x4Fu)
#define TVP5147_AFE_FINE_GAIN_CVBS_LUMA_LSB                     (0x50u)
#define TVP5147_AFE_FINE_GAIN_CVBS_LUMA_MSB                     (0x51u)

/* 0x52 - 0x68 Reserved */

#define TVP5147_FBIT_VBIT_CONTROL1                              (0x69u)

/* 0x6A - 0x6B Reserved */

#define TVP5147_BACKEND_AGC_CONTROL                             (0x6Cu)

/* 0x6D - 0x6E Reserved */

#define TVP5147_AGC_DECREMENT_SPEED_CONTROL                     (0x6Fu)
#define TVP5147_ROM_VERSION                                     (0x70u)

/* 0x71 - 0x73 Reserved */

#define TVP5147_AGC_WHITE_PEAK_PROCESSING                       (0x74u)
#define TVP5147_FBIT_VBIT_CONTROL2                              (0x75u)
#define TVP5147_VCR_TRICK_MODE_CONTROL                          (0x76u)
#define TVP5147_HORIZONTAL_SHAKE_INCREMENT                      (0x77u)
#define TVP5147_AGC_INCREMENT_SPEED                             (0x78u)
#define TVP5147_AGC_INCREMENT_DELAY                             (0x79u)

/* 0x7A - 0x7F Reserved */

#define TVP5147_CHIP_ID_MSB                                     (0x80u)
#define TVP5147_CHIP_ID_LSB                                     (0x81u)

/* 0x82 Reserved */

#define TVP5147_CPLL_SPEED_CONTROL                              (0x83u)

/* 0x84 - 0x96 Reserved */

#define TVP5147_STATUS_REQUEST                                  (0x97u)

/* 0x98 - 0x99 Reserved */

#define TVP5147_VERTICAL_LINE_COUNT_LSB                         (0x9Au)
#define TVP5147_VERTICAL_LINE_COUNT_MSB                         (0x9Bu)

/* 0x9C - 0x9D Reserved */

#define TVP5147_AGC_DECREMENT_DELAY                             (0x9Eu)

/* 0x9F - 0xB0 Reserved */

#define TVP5147_VDP_TTX_FILTER_1_MASK1                          (0xB1u)
#define TVP5147_VDP_TTX_FILTER_1_MASK2                          (0xB2u)
#define TVP5147_VDP_TTX_FILTER_1_MASK3                          (0xB3u)
#define TVP5147_VDP_TTX_FILTER_1_MASK4                          (0xB4u)
#define TVP5147_VDP_TTX_FILTER_1_MASK5                          (0xB5u)
#define TVP5147_VDP_TTX_FILTER_2_MASK1                          (0xB6u)
#define TVP5147_VDP_TTX_FILTER_2_MASK2                          (0xB7u)
#define TVP5147_VDP_TTX_FILTER_2_MASK3                          (0xB8u)
#define TVP5147_VDP_TTX_FILTER_2_MASK4                          (0xB9u)
#define TVP5147_VDP_TTX_FILTER_2_MASK5                          (0xBAu)
#define TVP5147_VDP_TTX_FILTER_CONTROL                          (0xBBu)
#define TVP5147_VDP_FIFO_WORD_COUNT                             (0xBCu)
#define TVP5147_VDP_FIFO_INTERRUPT_THRLD                        (0xBDu)

/* 0xBE Reserved */

#define TVP5147_VDP_FIFO_RESET                                  (0xBFu)
#define TVP5147_VDP_FIFO_OUTPUT_CONTROL                         (0xC0u)
#define TVP5147_VDP_LINE_NUMBER_INTERRUPT                       (0xC1u)
#define TVP5147_VDP_PIXEL_ALIGNMENT_LSB                         (0xC2u)
#define TVP5147_VDP_PIXEL_ALIGNMENT_MSB                         (0xC3u)

/* 0xC4 - 0xD5 Reserved */

#define TVP5147_VDP_LINE_START                                  (0xD6u)
#define TVP5147_VDP_LINE_STOP                                   (0xD7u)
#define TVP5147_VDP_GLOBAL_LINE_MODE                            (0xD8u)
#define TVP5147_VDP_FULL_FIELD_ENABLE                           (0xD9u)
#define TVP5147_VDP_FULL_FIELD_MODE                             (0xDAu)

/* 0xDB - 0xDF Reserved */

#define TVP5147_VBUS_DATA_ACCESS_NO_VBUS_ADDR_INCR              (0xE0u)
#define TVP5147_VBUS_DATA_ACCESS_VBUS_ADDR_INCR                 (0xE1u)
#define TVP5147_FIFO_READ_DATA                                  (0xE2u)

/* 0xE3 - 0xE7 Reserved */

/*
VBUS address [23:0]: VBUS is a 24-bit wide internal bus. The user needs to
program in these registers the 24-bit address of the internal register to be
accessed via host port indirect access mode.
*/
/*VBUS address [7:0]*/
#define TVP5147_VBUS_ADDRESS_ACCESS1                            (0xE8u)
/*VBUS address [15:8]*/
#define TVP5147_VBUS_ADDRESS_ACCESS2                            (0xE9u)
/*VBUS address [23:16]*/
#define TVP5147_VBUS_ADDRESS_ACCESS3                            (0xEAu)

/* 0xEB - 0xEF Reserved */

#define TVP5147_INTERRUPT_RAW_STATUS0                           (0xF0u)
#define TVP5147_INTERRUPT_RAW_STATUS1                           (0xF1u)
#define TVP5147_INTERRUPT_STATUS0                               (0xF2u)
#define TVP5147_INTERRUPT_STATUS1                               (0xF3u)
#define TVP5147_INTERRUPT_MASK0                                 (0xF4u)
#define TVP5147_INTERRUPT_MASK1                                 (0xF5u)
#define TVP5147_INTERRUPT_CLEAR0                                (0xF6u)
#define TVP5147_INTERRUPT_CLEAR1                                (0xF7u)

/* 0xF8 - 0xFF Reserved */

/* masks                                                                      */
/* defines for configuration values                                           */
#define TVP5147_LOST_LOCK_MASK                                  (0x10u)
/* mask to enable auto switch for all standards                               */

#define TVP5147_AUTOSWITCH_MASK_VALUE                           (0x7Fu)
#define TVP5147_COMPOSITE_INPUT                                 (0x05u)
#define TVP5147_SVIDEO_INPUT                                    (0x46u)


/* The TVP chip ID values                                                     */
#define TVP5147_CHIP_ID_MSB_VALUE                               (0x51)
#define TVP5147_CHIP_ID_LSB_VALUE                               (0x47)

#define VIDEO_STD_MASK                                          (0x07u)
#define VIDEO_STD_AUTO_SWITCH_BIT                               (0x00)
#define VIDEO_STD_NTSC_MJ_BIT                                   (0x01)
#define VIDEO_STD_PAL_BDGHIN_BIT                                (0x02)
#define VIDEO_STD_PAL_M_BIT                                     (0x03)
#define VIDEO_STD_PAL_COMBINATION_N_BIT                         (0x04)
#define VIDEO_STD_NTSC_4_43_BIT                                 (0x05)
#define VIDEO_STD_SECAM_BIT                                     (0x06)
#define VIDEO_STD_PAL_60_BIT                                    (0x07)

/*
 * Status bit
 */
#define STATUS_TV_VCR_BIT                                       (1<<0)
#define STATUS_HORZ_SYNC_LOCK_BIT                               (1<<1)
#define STATUS_VIRT_SYNC_LOCK_BIT                               (1<<2)
#define STATUS_CLR_SUBCAR_LOCK_BIT                              (1<<3)
#define STATUS_LOST_LOCK_DETECT_BIT                             (1<<4)
#define STATUS_FEILD_RATE_BIT                                   (1<<5)
#define STATUS_LINE_ALTERNATING_BIT                             (1<<6)
#define STATUS_PEAK_WHITE_DETECT_BIT                            (1<<7)
/* DEFAULTS */

#define TVP5147_OPERATION_MODE_RESET                            (0x1u)
#define TVP5147_OPERATION_MODE_DEFAULT                          (0x0)
#define TVP5147_AFE_GAIN_CTRL_DEFAULT                           (0x0Fu)
#define TVP5147_COLOR_KILLER_DEFAULT                            (0x10u)
#define TVP5147_LUMA_CONTROL1_DEFAULT                           (0x00u)
#define TVP5147_LUMA_CONTROL2_DEFAULT                           (0x00u)
#define TVP5147_LUMA_CONTROL3_DEFAULT                           (0x02u)
#define TVP5147_BRIGHTNESS_DEFAULT                              (0x80u)
#define TVP5147_CONTRAST_DEFAULT                                (0x80u)
#define TVP5147_SATURATION_DEFAULT                              (0x80u)
#define TVP5147_HUE_DEFAULT                                     (0x00u)
#define TVP5147_CHROMA_CONTROL1_DEFAULT                         (0x00u)
#define TVP5147_CHROMA_CONTROL2_DEFAULT                         (0x0Eu)
#define TVP5147_OUTPUT1_DEFAULT                                 (0x40u)
#define TVP5147_OUTPUT2_DEFAULT                                 (0x11u)
#define TVP5147_OUTPUT3_DEFAULT                                 (0xEEu)
#define TVP5147_OUTPUT4_DEFAULT                                 (0xAFu)
#define TVP5147_OUTPUT5_DEFAULT                                 (0xFFu)
#define TVP5147_OUTPUT6_DEFAULT                                 (0xFFu)
#define TVP5147_FIFO_OUTPUT_CTRL_DEFAULT                        (0x01u)

#define TVP5147_VBUS_DATA_ACCESS                                (0xE0u)
#define TVP5147_VBUS_DATA_ACCESS_AUTO_INCR                      (0xE1u)

#define TVP5147_LINE_ADDRESS_START                              (0x80u)
#define TVP5147_LINE_ADDRESS_MIDDLE                             (0x06u)
#define TVP5147_LINE_ADDRESS_END                                (0x00u)

#define TVP5147_LINE_ADDRESS_DEFAULT                            (0x00)
#define TVP5147_LINE_MODE_DEFAULT                               (0xFFu)

/** \brief Macro for maximum number of slice services supported */
#define TVP5147_VBI_NUM_SERVICES                                (3)

/* ========================================================================== */
/*                              STRUCTURES                                    */
/* ========================================================================== */
/**
 *  \brief TVP5147 VDP register structure
 */
typedef struct Tvp5147_VdpReg_t
{
    Uint8 fifoLineAddr[3];
    /**< Sub address of Line Address Register                                 */
    Uint8 fifoModeVal;
    /**< Line mode register value                                             */
}Tvp5147_VdpReg;

/**
 *  \brief Slice service register structure
 */
typedef struct Tvp5147_ServiceReg_t
{
    Uint32                  service;
    /**< Slice service type                                                   */
    Uint8                   lineNo;
    /**< Line number to be processed by a VDP set by a line mode register     */
    Uint8                   lineStart;
    /**< VDP line starting address                                            */
    Uint8                   lineEnd;
    /**< VDP stop line address                                                */
    Tvp5147_VdpReg          field[2];
    /**< VDP reg for both fields                                              */
    Tvp5147_Std             std;
    /**< Video standard                                                       */
}Tvp5147_ServiceReg;


/**
 *  \brief Structure holding value of Slice Service data address registers
 *         required per field. This structure holds the no of registers required
 *         to be read to get the Slice VBI data.
 */
typedef struct Tvp5147_FieldAddr_t
{
    Uint8                   addr[3];
    /**< Subaddress of VDP Data Register per field                            */
}Tvp5147_FieldAddr;

/**
 *  \brief Structure for reading Slice services data
 */
typedef struct Tvp5147_ServiceDataReg_t
{
    Uint32                  tvpSliceService;
    /**< Slice service type                                                   */
    Tvp5147_FieldAddr       tvpSliceField[2];
    /**< Slice service data register address for even and odd fields          */
    Uint8                   bytestoread;
    /**< Bytes of Slice data                                                  */
}Tvp5147_ServiceDataReg;

/**
 *  \brief  TVP5147 decoder object structure
 *  Decoder object structure for TVP5147 decoder
 */
typedef struct Tvp5147_DecObj_t
{
    Int8                    codecName[20];
    /**< Decoder name                                                         */
    Edc_ControlBusType      edcCtrlBusType;
    /**< Type of the control bus (SPI or I2C)                                 */
    Int8                    edcCtrlBusName[20];
    /**< Control bus (I2C or SPI) name                                        */
    Uint8                   edcCtrlDeviceAddr;
    /**< Decoder Control bus address - I2C slave address or SPI CS number     */
    Int32                   decNum;
    /**< Decoder number to which channel belongs to                           */
    Uint32                  servicesSet;
    /**< Slice VBI service set for decoder                                    */
    Uint8                   numServices;
    /**< Number of slice vbi service set                                      */
    Tvp5147_Std             currentVideoStd;
    /**< Current standard for Video                                           */
}Tvp5147_DecObj;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* #ifndef __TVP5147LOCAL_H_ */

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
