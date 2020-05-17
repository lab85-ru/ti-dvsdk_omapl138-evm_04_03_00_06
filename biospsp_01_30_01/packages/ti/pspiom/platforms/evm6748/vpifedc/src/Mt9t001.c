/*
 * Mt9t001.c
 *
 * This file contains Driver Layer Interface implementation for Mt9t001 sensor
 * driver. Mt9t001 sensor driver provides Driver Layer Interface to do
 * operations on the Mt9t001 sensor like open, close and control commands for
 * peripheral specific operations etc
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
 *  \file   Mt9t001.c
 *
 *  \brief  This file contains interface functions for MT9T001 external Image
 *          Sensor.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <iom.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ti/pspiom/vpif/VpifDebug.h>

#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/Mt9t001.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/src/Mt9t001Local.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/src/CodecInterfaceLocal.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/** \brief Macro for lower byte mask */
#define MT9T001_I2C_TXRX_DATA_MASK		        (0x00FF)
/** \brief Macro for upper byte mask */
#define MT9T001_I2C_TXRX_DATA_MASK_UPPER	    (0xFF00)
/** \brief Data shift macro for getting second byte */
#define MT9T001_I2C_TXRX_DATA_SHIFT		        (8)

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 mt9t001SetDefaultParams(MT9T001_SensorObj      *mt9t001handle,
                                     MT9T001_StandardFormat stdFormat);

static Int32 mt9t001SetStd(MT9T001_SensorObj      *mt9t001handle,
                           MT9T001_StandardFormat stdFormat);

static Int32 mt9t001WriteReg(MT9T001_SensorObj *mt9t001handle,
                             Edc_RegData *data);

static Int32 mt9t001SetFormat(MT9T001_SensorObj    *mt9t001handle,
                              MT9T001_FormatParams *mt9t001FmtParams);

static Int32 mt9t001PowerDown(MT9T001_SensorObj *mt9t001handle,
                              Bool powerDownEnable);

static Int32 mt9t001Reset(MT9T001_SensorObj *mt9t001handle);

static Int32 MT9T001_close(EDC_Handle handle);

static Int32 MT9T001_ctrl(EDC_Handle handle, Uns cmd, Ptr param);

static EDC_Handle MT9T001_open(String devName, Ptr optArg);

/* ========================================================================== */
/*                            FUNCTION TABLE                                  */
/* ========================================================================== */
/** MT9T001 EDC function table */
EDC_Fxns MT9T001_Fxns =
{
    &MT9T001_open,
    &MT9T001_close,
    &MT9T001_ctrl
};

/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */
/** Local structure array to contain channel objects, semaphore handles
 * and channel open count
 */
static MT9T001_SensorObj mt9t001Obj =
{
    "\0", Edc_ControlBusType_UNKNOWN, "\0", 0
};

/* ========================================================================== */
/*                      LOCAL FUNCTION DEFINITIONS                            */
/* ========================================================================== */
/**
 * \brief To configure MT9T001 default parameter
 */
static Int32 mt9t001SetDefaultParams(MT9T001_SensorObj      *mt9t001handle,
                                     MT9T001_StandardFormat stdFormat)
{
    Int32 initStatus = IOM_COMPLETED;
    Uint8 buffer[2] = {0,0};
    Uint8 defAddrI2c = 0;
    Bool initResult;

    VPIF_DEBUG("Entered ---> mt9t001SetDefaultParams\n");
    assert(NULL != mt9t001handle);

    /* Sets other default parameters */
    if (IOM_COMPLETED == initStatus)
    {

        /*Configure the MT9T001 in normalpower up mode */
        buffer[0] = (Uint8)(MT9T001_ROW_START_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_ROW_START_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult = codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_ROW_START_REG,
                         &buffer[0],
                         2u);

        buffer[0] = (Uint8)(MT9T001_COL_START_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_COL_START_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_COL_START_REG,
                         &buffer[0],
                         2u);

        buffer[0] = (Uint8)(MT9T001_WIDTH_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_WIDTH_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_COL_SIZE_REG,
                         &buffer[0],
                         2u);

        buffer[0] = (Uint8)(MT9T001_HEIGHT_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_HEIGHT_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_ROW_SIZE_REG,
                         &buffer[0],
                         2u);

        buffer[0] = (Uint8)(MT9T001_HBLANK_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_HBLANK_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_HORZ_BLANKING_REG,
                         &buffer[0],
                         2u);

        buffer[0] = (Uint8)(MT9T001_VBLANK_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_VBLANK_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_VERT_BLANKING_REG,
                         &buffer[0],
                         2u);

        defAddrI2c = mt9t001handle->edcCtrlDeviceAddr;
        /* Write to Output Control Register */
        buffer[0] = (Uint8)(MT9T001_OUTPUT_CONTROL_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_OUTPUT_CONTROL_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         defAddrI2c,
                         MT9T001_OUTPUT_CONTROL_REG,
                         &buffer[0],
                         2u);

        /* Write to Shutter Width Upper Register */
        buffer[0] = (Uint8)(MT9T001_SHUTTER_WIDTH_UPPER_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_SHUTTER_WIDTH_UPPER_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_SHUTTER_WIDTH_UPPER_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_SHUTTER_WIDTH_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_SHUTTER_WIDTH_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_SHUTTER_WIDTH_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_PIXEL_CLK_CTRL_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_PIXEL_CLK_CTRL_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_PIXEL_CLK_CONTROL_REG,
                          &buffer[0],
                          2u);

        /* Write to Restart Register */
        buffer[0] = (Uint8)(MT9T001_RESTART_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_RESTART_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_RESTART_REG,
                          &buffer[0],
                          2u);

        /* Write to Shutter Delay Register */
        buffer[0] = (Uint8)(MT9T001_SHUTTER_DELAY_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_SHUTTER_DELAY_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_SHUTTER_DELAY_REG,
                          &buffer[0],
                          2u);

        /* Write to Read Mode1 Register */
        buffer[0] = (Uint8)(MT9T001_READ_MODE1_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_READ_MODE1_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_READ_MODE1_REG,
                          &buffer[0],
                          2u);

        /* Write to Read Mode2 Register */
        buffer[0] = (Uint8)(MT9T001_READ_MODE2_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_READ_MODE2_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_READ_MODE2_REG,
                          &buffer[0],
                          2u);

        /* Write to Read Mode3 Register */
        buffer[0] = (Uint8)(MT9T001_READ_MODE3_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_READ_MODE3_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_READ_MODE3_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_ROW_ADDR_MODE_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_ROW_ADDR_MODE_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_ROW_ADDR_MODE_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_COL_ADDR_MODE_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_COL_ADDR_MODE_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_COL_ADDR_MODE_REG,
                          &buffer[0],
                          2u);

        /* Write to Green1 Gain Register */
        buffer[0] = (Uint8)(MT9T001_GREEN1_GAIN_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_GREEN1_GAIN_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_GREEN1_GAIN_REG,
                          &buffer[0],
                          2u);

        /* Write to Blue Gain Register */
        buffer[0] = (Uint8)(MT9T001_BLUE_GAIN_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_BLUE_GAIN_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_BLUE_GAIN_REG,
                          &buffer[0],
                          2u);

        /* Write to Red Gain Register */
        buffer[0] = (Uint8)(MT9T001_RED_GAIN_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_RED_GAIN_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_RED_GAIN_REG,
                          &buffer[0],
                          2u);

        /* Write to Green2 Gain Register */
        buffer[0] = (Uint8)(MT9T001_GREEN2_GAIN_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_GREEN2_GAIN_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_GREEN2_GAIN_REG,
                          &buffer[0],
                          2u);

        /* Write to Global Gain Register */
        buffer[0] = (Uint8)(MT9T001_GLOBAL_GAIN_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_GLOBAL_GAIN_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_GLOBAL_GAIN_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_BLACK_LEVEL_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_BLACK_LEVEL_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_BLACK_LEVEL_REG,
                          &buffer[0],
                          2u);

        /* Write to Coarse Threshold Register */
        buffer[0] = (Uint8)(MT9T001_CAL_COARSE_THRESHOLD_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_CAL_COARSE_THRESHOLD_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_CAL_COARSE_THRESHOLD_REG,
                          &buffer[0],
                          2u);

        /* Write to Target Threshold Register */
        buffer[0] = (Uint8)(MT9T001_CAL_TARGET_THRESHOLD_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_CAL_TARGET_THRESHOLD_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_CAL_TARGET_THRESHOLD_REG,
                          &buffer[0],
                          2u);

        /* Write to Black Level Calibration Register */
        buffer[0] = (Uint8)(MT9T001_BLK_LEVEL_CALIBRATION_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_BLK_LEVEL_CALIBRATION_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_BLK_LEVEL_CALIBRATION_REG,
                          &buffer[0],
                          2u);

        /* Write to Chip Enable Sync Register */
        buffer[0] = (Uint8)(MT9T001_CHIP_ENABLE_SYNC_DEFAULT >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_CHIP_ENABLE_SYNC_DEFAULT &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_CHIP_ENABLE_SYNC_REG,
                          &buffer[0],
                          2u);

        /* delay applying changes  */
        buffer[0] = (Uint8)(MT9T001_HALT_MODE >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_HALT_MODE &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_OUTPUT_CONTROL_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_INVERT_PIXEL_CLK >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_INVERT_PIXEL_CLK &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_PIXEL_CLK_CONTROL_REG,
                          &buffer[0],
                          2u);

        buffer[0] = (Uint8)(MT9T001_NORMAL_OPERATION_MODE >>
                            MT9T001_I2C_TXRX_DATA_SHIFT);
        buffer[1] = (Uint8)(MT9T001_NORMAL_OPERATION_MODE &
                            MT9T001_I2C_TXRX_DATA_MASK);
        initResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                          defAddrI2c,
                          MT9T001_OUTPUT_CONTROL_REG,
                          &buffer[0],
                          2u);


        if (initResult == TRUE)
        {
            DBG_PRINT_ERR("MT9T001: Error from mt9t001SetDefaultParams\n");
            initStatus = IOM_EBADIO;
        }
        else
        {
            /* Configure for default video standard */
            initStatus = mt9t001SetStd(mt9t001handle, stdFormat);
        }
    }

    VPIF_DEBUG("Exited ---> mt9t001SetDefaultParams\n");
    return initStatus;
}


/**
 * \brief Function to configure MT9T001 sensor for desired capture resolution.
 */
static Int32 mt9t001SetStd(MT9T001_SensorObj      *mt9t001handle,
                           MT9T001_StandardFormat stdFormat)
{
    Int32 stdStatus = IOM_COMPLETED;
    MT9T001_FormatParams mt9t001FmtParams;
    /* Parameters for  various format supported - Format  is
    {
        NUMBER OF PIXELS PER LINE, NUMBER OF LINES, HRIZONTAL BLANKING WIDTH,
        VERTICAL BLANKING WIDTH, SHUTTER WIDTH, BLACK_LEVEL,
        PIXEL CLOCK CONTROL, ROW START, COL START
    }
    */
    const MT9T001_FormatParams fmtVga =/*640X480 VGA 93 fps*/
        { 639, 479, 21, 15, 495, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmtSvga =/*800X600 SVGA 65 fps*/
        { 799, 599, 21, 15, 615, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmtXga =/*1024X768 XGA 43 fps*/
        { 1023, 767, 21, 15, 783, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmt480P =/*720X480*/
        { 719, 479, 21, 15, 480, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmt576P =/*720X576*/
        { 719, 575, 21, 15, 480, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmt720P =/*1280X720 HDTV 39 fps*/
        { 1279, 719, 21, 15, 735, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmt1080P =/*1920X1080 HDTV 18 fps*/
        { 1919, 1079, 21, 15, 1095, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmtQxga =/*2048X1536 QXGA 12 fps*/
        { 2047, 1535, 21, 15, 1551, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmtUxga =/*1600X1200 UXGA 20 fps*/
        { 1599, 1199, 21, 15, 1215, 64, 0x8000, 0, 0 };
    const MT9T001_FormatParams fmtSxga =/*1280X1024 SXGA 27 fps*/
        { 1279, 1023, 21, 15, 1039, 64, 0x8000, 0, 0 };

    VPIF_DEBUG("Entered ---> mt9t001SetStd\n");

    assert(NULL != mt9t001handle);

    /* Select configuration parameters as per video mode */
    if (stdFormat == MT9T001_MODE_VGA)
    {
        mt9t001FmtParams = fmtVga;
    }
    else if (stdFormat == MT9T001_MODE_SVGA)
    {
        mt9t001FmtParams = fmtSvga;
    }
    else if (stdFormat == MT9T001_MODE_XGA)
    {
        mt9t001FmtParams = fmtXga;
    }
    else if (stdFormat == MT9T001_MODE_480P)
    {
        mt9t001FmtParams = fmt480P;
    }
    else if (stdFormat == MT9T001_MODE_576P)
    {
        mt9t001FmtParams = fmt576P;
    }
    else if (stdFormat == MT9T001_MODE_720P)
    {
        mt9t001FmtParams = fmt720P;
    }
    else if (stdFormat == MT9T001_MODE_1080P)
    {
        mt9t001FmtParams = fmt1080P;
    }
    else if (stdFormat == MT9T001_MODE_QXGA)
    {
        mt9t001FmtParams = fmtQxga;
    }
    else if (stdFormat == MT9T001_MODE_UXGA)
    {
        mt9t001FmtParams = fmtUxga;
    }
    else
    {
        /* stdFormat is always MT9T001_MODE_SXGA when comes here bec of enum */
        mt9t001FmtParams = fmtSxga;
    }

    if (IOM_COMPLETED == stdStatus)
    {
        stdStatus = mt9t001SetFormat(mt9t001handle, &mt9t001FmtParams);
    }

    VPIF_DEBUG("Exited ---> mt9t001SetStd\n");
    return stdStatus;
}


/**
 * \brief To configure MT9T001 Format Parameters
 */
static Int32 mt9t001SetFormat(MT9T001_SensorObj    *mt9t001handle,
                              MT9T001_FormatParams *mt9t001FmtParams)
{
    Int32 fmtStatus = IOM_COMPLETED;
    Uint8 fmtBuffer[2] = {0, 0};
    Uint8 fmtAddrI2c = 0;
    Bool fmtResult;

    VPIF_DEBUG("Entered ---> mt9t001SetFormat\n");

    assert(NULL != mt9t001handle);
    assert(NULL != mt9t001FmtParams);

    fmtAddrI2c = mt9t001handle->edcCtrlDeviceAddr;

	/*Write the height width and blanking information required
	   for particular format */
    /* Write to Row Size Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->rowSize >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->rowSize &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult = codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_ROW_SIZE_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Col Size Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->columnSize >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->columnSize &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_COL_SIZE_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Horizontal Blanking Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->hBlank >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->hBlank &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_HORZ_BLANKING_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Vertical Blanking Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->vBlank >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->vBlank &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_VERT_BLANKING_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Shutter Width Register - lower mask */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->shutterWidth >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->shutterWidth &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_SHUTTER_WIDTH_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Shutter Width Register - upper */
    fmtBuffer[0] = (Uint8)(0x00);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->shutterWidth >>
                           MT9T001_SHUTTER_WIDTH_UPPER_SHIFT);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_SHUTTER_WIDTH_UPPER_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Black Level Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->blackLevel >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->blackLevel &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_BLACK_LEVEL_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Row Start Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->rowStart >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->rowStart &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_ROW_START_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Col Start Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->colStart >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->colStart &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                     fmtAddrI2c,
                     MT9T001_COL_START_REG,
                     &fmtBuffer[0],
                     2u);

    /* Write to Pixel Clock Control Register */
    fmtBuffer[0] = (Uint8)(mt9t001FmtParams->pixelClockCtrl >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(mt9t001FmtParams->pixelClockCtrl &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                    fmtAddrI2c,
                    MT9T001_PIXEL_CLK_CONTROL_REG,
                    &fmtBuffer[0],
                    2u);

	/* applying changes  */
    fmtBuffer[0] = (Uint8)(MT9T001_NORMAL_OPERATION_MODE >>
                           MT9T001_I2C_TXRX_DATA_SHIFT);
    fmtBuffer[1] = (Uint8)(MT9T001_NORMAL_OPERATION_MODE &
                           MT9T001_I2C_TXRX_DATA_MASK);
    fmtResult |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                    fmtAddrI2c,
                    MT9T001_OUTPUT_CONTROL_REG,
                    &fmtBuffer[0],
                    2u);

    if (fmtResult == TRUE)
    {
        DBG_PRINT_ERR("MT9T001: Error from mt9t001SetFormat\n");
        fmtStatus = IOM_EBADIO;
    }
    else
    {
        fmtStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> mt9t001SetFormat\n");

    return fmtStatus;
}


/**
 * \brief Write to MT9T001 Sensor
 */
static Int32 mt9t001WriteReg(MT9T001_SensorObj *mt9t001handle,
                             Edc_RegData *data)
{
    Int32 wStatus = IOM_COMPLETED;
    Bool wFlag;
    Uint32 i;

    VPIF_DEBUG("Entered ---> mt9t001WriteReg\n");

    assert(NULL != mt9t001handle);
    assert(NULL != data);

    for (i = 0; i < data->noRegToRW; i++)
    {
        wFlag = codecWriteReg(mt9t001handle->edcCtrlBusType,
                    mt9t001handle->edcCtrlDeviceAddr,
                    (Uint8)(data->startReg + i),
                    data->value,
                    data->noRegToRW + 1u);
        if (wFlag == TRUE)
        {
            DBG_PRINT_ERR("MT9T001: Error from MT9T001_ctrl\n");
            wStatus = IOM_EBADIO;
            break;
        }
        else
        {
            wStatus = IOM_COMPLETED;
        }
    }

    VPIF_DEBUG("Exited ---> mt9t001WriteReg\n");
    return wStatus;
}


/**
 *  \brief  Power Up or Power Down MT9T001. Sets devices power state to
 *          requested state, if possible.
 *
 *  \param  mt9t001handle   [In] MT9T001 channel parameters
 *  \param  powerDownEnable [In] Flag to power-up or power-down MT9T001
 *
 *  \return IOM_COMPLETED on successful operation
 *          IOM_EBADARGS if error occurred
 */
static Int32 mt9t001PowerDown(MT9T001_SensorObj *mt9t001handle,
                              Bool powerDownEnable)
{
    Uint8 pwrDnBuffer[2] = {0, 0};
    Uint8 pwrDnAddrI2c = 0;
    Bool pwrDnFlag;
    Int32 retVal = IOM_COMPLETED;

    VPIF_DEBUG("Entered ---> mt9t001PowerDown\n");
    VPIF_DEBUG2("MT9T001_SensorObj = 0x%0.8X, powerDownEnable = %d \n",
               mt9t001handle,
               powerDownEnable);

    assert(NULL != mt9t001handle);
    pwrDnAddrI2c = mt9t001handle->edcCtrlDeviceAddr;

    /* Write to Reset Register */
    pwrDnBuffer[0] = (Uint8)(MT9T001_RESET_ENABLE >>
                             MT9T001_I2C_TXRX_DATA_SHIFT);
    pwrDnBuffer[1] = (Uint8)(MT9T001_RESET_ENABLE & MT9T001_I2C_TXRX_DATA_MASK);
    pwrDnFlag = codecWriteReg(mt9t001handle->edcCtrlBusType,
                     pwrDnAddrI2c,
                     MT9T001_RESET_REG,
                     &pwrDnBuffer[0],
                     2u);
    /* Check for Power Up */
    if (FALSE == powerDownEnable)
    {
        /* Power Up Sequence */
        pwrDnBuffer[0] = (Uint8)(MT9T001_RESET_DISABLE >>
                                 MT9T001_I2C_TXRX_DATA_SHIFT);
        pwrDnBuffer[1] = (Uint8)(MT9T001_RESET_DISABLE &
                                 MT9T001_I2C_TXRX_DATA_MASK);
        pwrDnFlag |= codecWriteReg(mt9t001handle->edcCtrlBusType,
                         pwrDnAddrI2c,
                         MT9T001_RESET_REG,
                         &pwrDnBuffer[0],
                         2u);
    }

    if (TRUE == pwrDnFlag)
    {
        DBG_PRINT_ERR("MT9T001: Error from mt9t001PowerDown\n");
        retVal = IOM_EBADIO;
    }
    else
    {
        retVal = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> mt9t001PowerDown\n");
    return retVal;
}


/**
 *  \brief  Reset MT9T001
 *
 *  \param  mt9t001handle  [In]  MT9T001 channel parameters
 *
 *  \return IOM_COMPLETED on successful operation
 *          IOM_EBADARGS if error occurred
 */
static Int32 mt9t001Reset(MT9T001_SensorObj *mt9t001handle)
{
    /* Reset MT9T001 */
    return mt9t001PowerDown(mt9t001handle, FALSE);
}

/* ========================================================================== */
/*                     MT9T001 GLOBAL FUNCTION DEFINITIONS                    */
/* ========================================================================== */
/**
 *  \brief  MT9T001 Close
 *
 *  This Function is used to deinitialize the MT9T001 external image sensor.
 *
 *  \param  handle [IN]  Handle of MT9T001 driver
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 MT9T001_close(EDC_Handle handle)
{
    MT9T001_SensorObj *closeDevObj;
    Int32 closeStatus = IOM_EBADARGS;

    VPIF_DEBUG("Entered ---> MT9T001_close\n");
    VPIF_DEBUG1("EDC_Handle = 0x%0.8x\n", handle);

    /* First Check if the Handle/params are correct */
    assert(NULL != handle);
    closeDevObj = (MT9T001_SensorObj *)handle;
    assert(MT9T001_IIC_ADDR == closeDevObj->edcCtrlDeviceAddr);
    assert(Edc_ControlBusType_UNKNOWN != closeDevObj->edcCtrlBusType);

    /*Put MT9T001 in power down mode */
    closeStatus = mt9t001PowerDown(closeDevObj, TRUE);
    if (IOM_COMPLETED == closeStatus)
    {
        /*close I2C channel*/
        closeStatus = deInitCodecInterface(closeDevObj->edcCtrlBusType);
    }
    memset(&mt9t001Obj, 0x0, sizeof(MT9T001_SensorObj));
    closeDevObj->edcCtrlBusType = Edc_ControlBusType_UNKNOWN;

    VPIF_DEBUG("Exited ---> MT9T001_close\n");
    return closeStatus;
}


/**
 *  \brief  MT9T001 Ioctls
 *
 *  This Function is to configure and change parameters of MT9T001 image sensor
 *  runtime.
 *
 *  \param  handle [IN]  Handle of MT9T001 image sensor
 *  \param  cmd    [IN]  IOCTL to be performed (mostly enum).
 *  \param  param  [IN]  Argument as required by Ioctl
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 MT9T001_ctrl(EDC_Handle handle, Uns cmd, Ptr param)
{

    MT9T001_SensorObj *ctrlDevObj;
    Int32 mtCtrlStatus = IOM_COMPLETED;
    Mt9t001_ConfParams *stdFmt;

    VPIF_DEBUG("Entered ---> MT9T001_ctrl\n");
    VPIF_DEBUG2("EDC_Handle = 0x%0.8X, Arg = 0x%0.8X\n", handle, param);

    /* First Check if the Handle/params are correct */
    assert(NULL != handle);
    ctrlDevObj = (MT9T001_SensorObj *)handle;
    assert(MT9T001_IIC_ADDR == ctrlDevObj->edcCtrlDeviceAddr);

    if (Edc_IOCTL_CONFIG == cmd)
    {
        VPIF_DEBUG("MT9T001: Command: Edc_IOCTL_CONFIG\n");
        assert(NULL != param);
        stdFmt = (Mt9t001_ConfParams *)param;
        if (NULL == stdFmt->fmtParams)
        {
            mtCtrlStatus = mt9t001SetStd(ctrlDevObj, stdFmt->stdFormat);
        }
        else
        {
            mtCtrlStatus = mt9t001SetFormat(ctrlDevObj, stdFmt->fmtParams);
        }
    }
    else if (Edc_IOCTL_RESET == cmd)
    {
        VPIF_DEBUG("MT9T001: Command: Edc_IOCTL_RESET\n");
        mtCtrlStatus = mt9t001Reset(ctrlDevObj);
    }
    else if (Edc_IOCTL_SET_REG == cmd)
    {
        VPIF_DEBUG("MT9T001: Command: Edc_IOCTL_SET_REG\n");
        assert(NULL != param);
        mtCtrlStatus = mt9t001WriteReg(ctrlDevObj, (Edc_RegData *)param);
    }
    else if (Edc_IOCTL_GET_REG == cmd)
    {
        mtCtrlStatus = IOM_ENOTIMPL;
    }
    else
    {
        VPIF_DEBUG("MT9T001: Command: default\n");
        mtCtrlStatus = IOM_EBADARGS;
    }

    VPIF_DEBUG("Exited ---> MT9T001_ctrl\n");
    return mtCtrlStatus;
}


/**
 *  \brief  MT9T001 device open
 *
 *  This Function is used to initialize the MT9T001 external image sensor.
 *  It returns MT9T001 handle.
 *
 *  \param  devName [IN]  External device name
 *  \param  optArg  [IN]  Optional argument (not used here)
 *
 *  \return  handle  EDC channel handle when success
 *                   NULL when unsuccessful
 */
static EDC_Handle MT9T001_open(String devName, Ptr optArg)
{
    MT9T001_SensorObj *handle = NULL;
    Int8 mt9t001Str[20] = "MT9T001";
    Int8 i2cDevice[20] = "\0";
    Int8 senName[20] = "\0";
    /** Default configuration of MT9T001 */
    static MT9T001_StandardFormat openStdFormat = MT9T001_MODE_480P;

    VPIF_DEBUG("Entered ---> MT9T001_open\n");
    VPIF_DEBUG2("devName = %s, Arg = 0x%0.8X\n", devName, optArg);

    assert(NULL != devName);
    optArg = optArg; /* to remove compilation warning */
    /* Check if the address is correct */
    if (devName[0] != '/')
    {
        handle = (MT9T001_SensorObj *)NULL;
    }
    else
    {
        /*only copy 5 chars into positions 0-4 for /I2C0*/
        strncpy(i2cDevice, devName, 5u);
        /* position 5 gets the terminator */
        i2cDevice[5] = '\0';

        devName += 5;
        /* Check if the address is correct*/
        if ('/' == devName[0])
        {
            devName++;
            /*only copy 7 chars into positions 0-6 for devName*/
            strncpy(senName, devName, 7u);
            /* position 7 gets the terminator */
            senName[7] = '\0';

            devName += 7;
            /* Compare devName with "MT9T001" */
            if (0 == strcmp(senName, mt9t001Str))
            {
                strncpy(mt9t001Obj.sensorName, mt9t001Str, 20u);
                /* check if already opened  */
                if (('/' == devName[0]) && (0 == mt9t001Obj.edcCtrlDeviceAddr))
                {
                    devName++;
                    mt9t001Obj.edcCtrlDeviceAddr = (Uint8)strtol(
                                                              devName + 2,
                                                              NULL,
                                                              16);
                    if (MT9T001_IIC_ADDR == mt9t001Obj.edcCtrlDeviceAddr)
                    {
                        strncpy(mt9t001Obj.edcCtrlBusName, i2cDevice, 20u);
                        if (0 == strcmp(i2cDevice, "/I2C0"))
                        {
                            mt9t001Obj.edcCtrlBusType = Edc_ControlBusType_I2C;
                        }
                        else
                        {
                            mt9t001Obj.edcCtrlBusType = \
                                Edc_ControlBusType_UNKNOWN;
                        }
                        /* assigning channel object structure */
                        handle = &mt9t001Obj;
                    }
                    else
                    {
                        /* wrong i2c address */
                        handle = (MT9T001_SensorObj *)NULL;
                        mt9t001Obj.sensorName[0] = '\0';
                    }

                }
                else
                {
                    /* no i2c address in the string or device already open*/
                    handle = (MT9T001_SensorObj *)NULL;
                    mt9t001Obj.sensorName[0] = '\0';
                }
            }
            else
            {
                /* Sensor name incorrect */
                handle = (MT9T001_SensorObj *)NULL;
            }
        }
        else
        {
            /* no Sensor name */
            handle = (MT9T001_SensorObj *)NULL;
        }
    }
    if (handle != NULL)
    {
        /* open I2C channel */
        if (IOM_COMPLETED != initCodecInterface(
                                (String)mt9t001Obj.edcCtrlBusName))
        {
            handle = (MT9T001_SensorObj *)NULL;
        }
        /* power on sensor */
        if (handle != NULL)
        {
            if (IOM_COMPLETED != mt9t001Reset(handle))
            {
                    deInitCodecInterface(mt9t001Obj.edcCtrlBusType);
                    handle = (MT9T001_SensorObj *)NULL;
                    mt9t001Obj.edcCtrlDeviceAddr = 0;
            }
            else
            {
                /* Configure MT9T001 for default video standard */
                if (IOM_COMPLETED != mt9t001SetDefaultParams(handle,
                                         openStdFormat))
                {
                    deInitCodecInterface(mt9t001Obj.edcCtrlBusType);
                    handle = (MT9T001_SensorObj *)NULL;
                    mt9t001Obj.edcCtrlDeviceAddr = 0;
                }
            }
        }
        else
        {
            DBG_PRINT_ERR("MT9T001: ERR ---> edcSensor_open\n");
        }
    }

    VPIF_DEBUG("Exited ---> MT9T001_open\n");
    return (EDC_Handle)handle;
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
