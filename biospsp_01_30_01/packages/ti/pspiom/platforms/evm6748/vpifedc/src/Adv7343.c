/*
 * Adv7343.c
 *
 * This file contains Driver Layer Interface implementation for Adv7343 encoder
 * driver. Adv7343 encoder driver provides Driver Layer Interface to do
 * operations on the Adv7343 encoder like open, close and control commands for
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
 *  \file   Adv7343.c
 *
 *  \brief  This file contains interface functions for ADV7343 encoder
 *          The encoder hardware does not support SECAM
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

#include "ti/pspiom/vpif/Fvid.h"
#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/Adv7343.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/src/Adv7343Local.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/src/CodecInterfaceLocal.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/**\brief Macro defining all the vbi silce services supported by ADV7343 */
#define ADV7343_ALL_VBI_SERVICES        (FVID_SLICE_VBI_SERVICES_WSS_PAL| \
                                         FVID_SLICE_VBI_SERVICES_CGMS_NTSC | \
                                         FVID_SLICE_VBI_SERVICES_CC_NTSC)

/**\brief Macro defining the one time register read for VBI data registers */
#define MAX_READ_FOR_VBI                11

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 adv7343SetSliceVbiService(Adv7343_EncObj *chan, Uint32 *services);

static Void adv7343Callback(Ptr handle, Int32 *status);

static Int32 adv7343WriteVbiData(Adv7343_EncObj *chan, FVID_SliceFrame data[]);

static Int32 adv7343SetDefaultValues(Adv7343_EncObj *chan);

static Int32 adv7343PowerDown(Adv7343_EncObj *chan, Bool powerDownEnable);

static Int32 adv7343Reset(Adv7343_EncObj *chan);

static Int32 adv7343SetControl(Adv7343_EncObj *chan, Adv7343_Control *ctrl);

static Int32 adv7343Config(Adv7343_EncObj *chan, Adv7343_ConfParams *param);

static Int32 adv7343EnableColorBars(Adv7343_EncObj *chan, Bool enable);

static Int32 adv7343SetOutputMode(Adv7343_EncObj *chan,
                                  Adv7343_AnalogFormat outMode);

static Int32 adv7343SetVideoDisplay(Adv7343_EncObj          *chan,
                                    Adv7343_Std             displayType,
                                    Adv7343_AnalogFormat    outMode,
                                    Adv7343_InputFormat     inFormat,
                                    Uint32                  services);

static Int32 adv7343SetGammaParams(Adv7343_EncObj *chan,
                                   Adv7343_GammaParams *params);

static Int32 ADV7343_close(EDC_Handle handle);

static Int32 ADV7343_ctrl(EDC_Handle handle, Uns cmd, Ptr param);

static EDC_Handle ADV7343_open(String devName, Ptr optArg);

/* ========================================================================== */
/*                            FUNCTION TABLE                                  */
/* ========================================================================== */
/** ADV7343 EDC function table */
EDC_Fxns ADV7343_Fxns =
{
    &ADV7343_open,
    &ADV7343_close,
    &ADV7343_ctrl
};

/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */
/** Local structure array to contain channel objects, semaphore handles
 * and channel open count
 */
static Adv7343_EncObj adv7343Obj =
{
    "\0", Edc_ControlBusType_UNKNOWN, "\0", 0, 0, 0, Adv7343_Std_INVALID
};

/** Place holder to save the Callback function and parameter */
Edc_Data wrCbData;

/** Place holder to save the VBI data for one time read */
Uint8 vbiData[MAX_READ_FOR_VBI];

/* ========================================================================== */
/*                      LOCAL FUNCTION DEFINITIONS                            */
/* ========================================================================== */
/**
 *  \brief  Enable support for sending set of sliced vbi services.
 *
 *  \param  chan      [IN]  Handle of the Channel Object
 *  \param  services  [IN]  Pointer to the slice service
 *
 *  \return No of services set, if success.
 *          Error code, in case of error.
 *
 */
static Int32 adv7343SetSliceVbiService(Adv7343_EncObj *chan, Uint32 *services)
{
    Int32 setVbiServStatus = IOM_COMPLETED;
    Uint8 readVal = 0, setVbiNumServices = 0;
    Bool setServiceFlag;
    Uint32 i;
    Adv7343_ServiceReg adv7343SlicedReg[ADV7343_VBI_NUM_SERVICES] =
    {
        {
            FVID_SLICE_VBI_SERVICES_WSS_PAL,
            ADV7343_SD_CGMS_WSS0,
            SD_WSS_EN,
            SD_WSS_DI,
            Adv7343_Std_PAL720
        },
        {
            FVID_SLICE_VBI_SERVICES_CC_NTSC,
            ADV7343_SD_MODE_REG3,
            SD_CLOSE_CAPTION_BOTH,
            SD_CLOSE_CAPTION_DI,
            Adv7343_Std_NTSC720
        },
        {
            FVID_SLICE_VBI_SERVICES_CGMS_NTSC,
            ADV7343_SD_CGMS_WSS0,
            SD_CGMS_EN,
            SD_CGMS_DI,
            Adv7343_Std_NTSC720
        }
    };

    assert(NULL != chan);
    assert(NULL != services);

    VPIF_DEBUG("Entered ---> adv7343SetSliceVbiService\n");

    if ((((*services) | ADV7343_ALL_VBI_SERVICES) != ADV7343_ALL_VBI_SERVICES))
    {
        DBG_PRINT_ERR("ADV7343: Invalid VBI service\n");
        setVbiServStatus = IOM_EBADARGS;
    }

    if (IOM_COMPLETED == setVbiServStatus)
    {
        for (i = 0; i < ADV7343_VBI_NUM_SERVICES; i++)
        {
            if (((0 != ((*services) & adv7343SlicedReg[i].service)) &&
                (adv7343SlicedReg[i].std != chan->currentVideoStd)) &&
                ((Adv7343_Std_AUTO != chan->currentVideoStd) &&
                (IOM_COMPLETED != setVbiServStatus)))
            {
                DBG_PRINT_ERR("ADV7343: Std doesn't support this service\n");
                setVbiServStatus = IOM_EBADMODE;
                break;
            }
            setServiceFlag = codecReadReg(chan->edcCtrlBusType,
                                 chan->edcCtrlDeviceAddr,
                                 adv7343SlicedReg[i].reg,
                                 &readVal,
                                 1u);

            if (FVID_SLICE_VBI_SERVICES_NONE != ((*services) & adv7343SlicedReg[i].service))
            {
                readVal |= adv7343SlicedReg[i].enableVal;
                setVbiNumServices++;
            }
            else
            {
                readVal &= adv7343SlicedReg[i].disableVal;
            }
            setServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                  chan->edcCtrlDeviceAddr,
                                  adv7343SlicedReg[i].reg,
                                  &readVal,
                                  1u);
            if (setServiceFlag == TRUE)
            {
                DBG_PRINT_ERR("ADV7343: Error from adv7343SetOutputMode\n");
                setVbiServStatus = IOM_EBADIO;
            }
        }
    }

    if (IOM_COMPLETED == setVbiServStatus)
    {
        chan->servicesSet = *services;
        chan->numServices = setVbiNumServices;
        setVbiServStatus = (Int32)chan->numServices;
        /*Read and save the value of vbi registers */
        setServiceFlag = codecReadReg(chan->edcCtrlBusType,
                     chan->edcCtrlDeviceAddr,
                     ADV7343_SD_CLOSE_CAPTION_EVEN0,
                     &vbiData[0],
                     MAX_READ_FOR_VBI);
        if (setServiceFlag == TRUE)
        {
            DBG_PRINT_ERR("ADV7343: Error reading VBI data regs\n");
            setVbiServStatus = IOM_EBADIO;
        }
    }
    else
    {
        chan->numServices = 0;
        chan->servicesSet = 0;
    }
    VPIF_DEBUG("Exited ---> adv7343SetSliceVbiService\n");

    return setVbiServStatus;
}

/**
 *  \brief  This function is used to callback once VBI data is written.
 *
 *  \param  handle   [IN]  Handle of the Channel Object
 *  \param  status   [IN]  Status of the write to the codec.
 *
 *  \return None
 *
 */
static Void adv7343Callback(Ptr handle, Int32 *status)
{
    Adv7343_EncObj *chan = handle;
    chan->sliceCbFxn(chan->sliceCbData, status);
}


/**
 *  \brief  This function is used to write the vbi data to the encoder device.
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *  \param  data   [IN]  Array containing Slice VBI data to write.
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 *
 */
static Int32 adv7343WriteVbiData(Adv7343_EncObj *chan, FVID_SliceFrame data[])
{
    Int32 writeVbiStatus = IOM_EBADARGS;
    Uint8 val2, found = 0;
    Uint8 i = 0;
    Uint8 advFld = 0;
    Uint8 writeVbiNumServices;
    CodecRegData wrData;

    Adv7343_ServiceDataReg advServicesRegs[ADV7343_VBI_NUM_SERVICES] =
    {
        {
            FVID_SLICE_VBI_SERVICES_CC_NTSC,
            {
                {
                    {ADV7343_SD_CLOSE_CAPTION_ODD0,
                     ADV7343_SD_CLOSE_CAPTION_ODD1}
                },
                {
                    {ADV7343_SD_CLOSE_CAPTION_EVEN0,
                     ADV7343_SD_CLOSE_CAPTION_EVEN1}
                }
            },
            2
        },
        {
            FVID_SLICE_VBI_SERVICES_WSS_PAL,
            {
                {
                    {ADV7343_SD_CGMS_WSS2, ADV7343_SD_CGMS_WSS1}
                },
                {
                    {ADV7343_SD_CGMS_WSS2, ADV7343_SD_CGMS_WSS1}
                }
            },
            2
        }
    };

    if ((NULL != chan) && (NULL != data))
    {
        writeVbiStatus = IOM_COMPLETED;
        writeVbiNumServices = chan->numServices;
        wrCbData.cbData = chan;
        wrCbData.cbFxn = &adv7343Callback;

        for (i = 0; i < writeVbiNumServices; i++)
        {
            if (FVID_SLICE_VBI_SERVICES_NONE != data[i].fvidSliceServiceId)
            {
                if ((data[i].fvidSliceServiceId | chan->servicesSet) !=
                    chan->servicesSet)
                {
                    writeVbiStatus = IOM_EBADARGS;
                    break;
                }
            }
        }

        if (0 == writeVbiNumServices)
        {
            writeVbiStatus = IOM_EBADIO;
        }

        if (IOM_COMPLETED == writeVbiStatus)
        {
            wrData.edcControlBusType = chan->edcCtrlBusType;
            wrData.slaveAddr = chan->edcCtrlDeviceAddr;

            if (writeVbiNumServices == 2)
            {
                for (i = 0; i < ADV7343_VBI_NUM_SERVICES; i++)
                {
                    if (FVID_SLICE_VBI_SERVICES_CGMS_NTSC ==
                        (data[i].fvidSliceServiceId &
                        FVID_SLICE_VBI_SERVICES_CGMS_NTSC))
                    {
                        vbiData[10] = data[i].fvidData[0];
                        vbiData[9] = data[i].fvidData[1];
                        val2 = data[i].fvidData[2] & 0x0F;
                        vbiData[8] |= val2;
                        found++;
                    }
                    if (FVID_SLICE_VBI_SERVICES_CC_NTSC ==
                        (data[i].fvidSliceServiceId &
                        FVID_SLICE_VBI_SERVICES_CC_NTSC))
                    {
                        if (0 == data[i].fvidField)/*even field*/
                        {
                            vbiData[0] = data[i].fvidData[0];
                            vbiData[1] = data[i].fvidData[1];
                        }
                        else
                        {
                            vbiData[2] = data[i].fvidData[0];
                            vbiData[3] = data[i].fvidData[1];
                        }
                        found++;
                    }
                    if (2 == found)
                    {
                        wrData.length = MAX_READ_FOR_VBI;
                        wrData.buffer = &vbiData[0];
                        wrData.subAddr = ADV7343_SD_CLOSE_CAPTION_EVEN0;
                        writeVbiStatus = codecWriteRegAsync(&wrData, &wrCbData);
                    }
                }
            }
            else
            {
                if (FVID_SLICE_VBI_SERVICES_CGMS_NTSC ==
                    (data[0].fvidSliceServiceId &
                    FVID_SLICE_VBI_SERVICES_CGMS_NTSC))
                {
                    vbiData[10] = data[0].fvidData[0];
                    vbiData[9] = data[0].fvidData[1];
                    val2 = data[0].fvidData[2] & 0x0F;
                    vbiData[8] |= val2;
                    wrData.length = 3u;
                    wrData.buffer = &vbiData[8];
                    wrData.subAddr = ADV7343_SD_CGMS_WSS0;
                    writeVbiStatus = codecWriteRegAsync(&wrData, &wrCbData);
                }
                else if (FVID_SLICE_VBI_SERVICES_CC_NTSC ==
                    (data[0].fvidSliceServiceId &
                    FVID_SLICE_VBI_SERVICES_CC_NTSC))
                {
                    advFld = data[0].fvidField;
                    wrData.length = advServicesRegs[0].bytestowrite;
                    wrData.buffer = &data[0].fvidData[0];
                    wrData.subAddr = advServicesRegs[0].field[advFld].addr[0];
                    writeVbiStatus = codecWriteRegAsync(&wrData, &wrCbData);
                }
                else /* SLICE VBI SERVICES WSS PAL*/
                {
                    advFld = data[0].fvidField;
                    wrData.length = advServicesRegs[1].bytestowrite;
					vbiData[10] = data[0].fvidData[0];
					vbiData[9] = data[0].fvidData[1];
                    wrData.buffer = &vbiData[9];
                    wrData.subAddr = advServicesRegs[1].field[advFld].addr[1];
                    writeVbiStatus = codecWriteRegAsync(&wrData, &wrCbData);
                }
            }
        }
    }
    return writeVbiStatus;
}


/**
 *  \brief  Set Output Mode
 *
 *  \param  chan    [IN]  Handle of the Channel Object
 *  \param  outMode [IN]  ADV7343 analog format
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 adv7343SetOutputMode(Adv7343_EncObj *chan,
                                  Adv7343_AnalogFormat outMode)
{
    Int32 setOutStatus = IOM_COMPLETED;
    Bool setOutFlag;
    Uint8 setOutAddrI2c = 0;
    Uint8 setOutPowerMode = 0, sdModeReg2 = 0;

    assert(NULL != chan);
    assert((Adv7343_AnalogFormat_COMPOSITE == outMode) ||
           (Adv7343_AnalogFormat_SVIDEO == outMode));
    VPIF_DEBUG("Entered ---> adv7343SetOutputMode\n");
    VPIF_DEBUG1("Adv7343_AnalogFormat = %d\n", outMode);
    setOutAddrI2c = chan->edcCtrlDeviceAddr;

    setOutFlag = codecReadReg(chan->edcCtrlBusType,
                     setOutAddrI2c,
                     ADV7343_POWER_MODE_REG,
                     &setOutPowerMode,
                     1u);
    setOutPowerMode &= 0x03u;

    /* Enable Appropriate DAC */
    if (Adv7343_AnalogFormat_COMPOSITE == outMode)
    {
        setOutPowerMode |= ADV7343_COMPOSITE_POWER_VALUE;
    }
    else
    {
        setOutPowerMode |= ADV7343_SVIDEO_POWER_VALUE;
    }

    setOutFlag |= codecWriteReg(chan->edcCtrlBusType,
                      setOutAddrI2c,
                      ADV7343_POWER_MODE_REG,
                      &setOutPowerMode,
                      1u);

    /* Read SD MODE REGISTER 2 */
    setOutFlag |= codecReadReg(chan->edcCtrlBusType,
                      setOutAddrI2c,
                      ADV7343_SD_MODE_REG2,
                      &sdModeReg2,
                      1u);
    /* configure SD DAC Output 2 bit to zero and SD DAC Output 1 bit to one */
    sdModeReg2 &= SD_DAC_2_DI;
    sdModeReg2 |= ~SD_DAC_1_DI;
    /* write SD MODE REGISTER 2 */
    setOutFlag |= codecWriteReg(chan->edcCtrlBusType, setOutAddrI2c,
                          ADV7343_SD_MODE_REG2,
                          &sdModeReg2,
                          1u);
    if (setOutFlag == TRUE)
    {
        DBG_PRINT_ERR("ADV7343: Error from adv7343SetOutputMode\n");
        setOutStatus = IOM_EBADIO;
    }
    else
    {
        setOutStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> adv7343SetOutputMode\n");
    return setOutStatus;
}



/**
 *  \brief  Enable/Disable Color bars
 *
 *  \param  chan    [IN]  Handle of the Channel Object
 *  \param  enable  [IN]  Flag to enable or disable color bars
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 adv7343EnableColorBars(Adv7343_EncObj *chan, Bool enable)
{
    Int32 colorStatus = IOM_COMPLETED;
    Uint8 sdModeReg4;
    Bool colorFlag;

    assert(NULL != chan);
    VPIF_DEBUG("Entered ---> adv7343EnableColorBars\n");
    VPIF_DEBUG1("enable = %d\n", enable);

    if (enable == TRUE)
    {
        sdModeReg4 = ADV7343_SD_MODE_REG4_DEFAULT | SD_COLOR_BARS_EN;
    }
    else
    {
        sdModeReg4 = ADV7343_SD_MODE_REG4_DEFAULT & SD_COLOR_BARS_DI;
    }
    /*write of 0x24 to Sub address 0x02 for an SD black bar test pattern*/
    colorFlag = codecWriteReg(chan->edcCtrlBusType,
                    chan->edcCtrlDeviceAddr,
                    ADV7343_SD_MODE_REG4,
                    &sdModeReg4,
                    1u);
    if (colorFlag == TRUE)
    {
        DBG_PRINT_ERR("ADV7343: Error from adv7343EnableColorBars\n");
        colorStatus = IOM_EBADIO;
    }
    else
    {
        colorStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> adv7343EnableColorBars\n");

    return colorStatus;
}


/**
 *  \brief  This function is used to set Gamma Correction parameters.
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *  \param  params [IN]  Pointer to Adv7343_GammaParams structure.
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 *
 */
static Int32 adv7343SetGammaParams(Adv7343_EncObj *chan,
                                   Adv7343_GammaParams *params)
{
    Uint8 val1 = 0, gammaReg = 0;
    Int32 gammaStatus = IOM_COMPLETED;
    Bool gammaFlag;
    Uint8 gammaAddrI2c = 0;

    /* Check for null value */
    assert(NULL != params);
    assert(NULL != chan);

    /* Check for invalid curve is not required because of enum */

    if (IOM_COMPLETED == gammaStatus)
    {
        gammaAddrI2c = chan->edcCtrlDeviceAddr;

        /* Check If gamma enabled, for SD */
        if (Edc_VideoType_SD == params->type)
        {
            gammaFlag = codecReadReg(chan->edcCtrlBusType,
                            gammaAddrI2c,
                            ADV7343_SD_MODE_REG7,
                            &val1,
                            1u);

            val1 |= ADV7343_SD_MODE_REG7_DEFAULT;
            /* If gamma enabled, enable gamma correction in SD Mode Register 5*/
            if (TRUE == params->enGamma)
            {
                /* Enable the gamma curve */
                val1 |= SD_GAMMA_EN;
                /* Select gamma curve in the same register */
                if (params->curve == Adv7343_GammaCurve_B)
                {
                    val1 |= SD_GAMMA_CURVE_B;
                    /* Select base register for gamma coefficients as per
                     * the curve selected */
                    gammaReg = ADV7343_SD_GAMMA_B0;
                }
                else
                {
                    val1 &= SD_GAMMA_CURVE_A;
                    /* Select base register for gamma coefficients as per
                     * the curve selected */
                    gammaReg = ADV7343_SD_GAMMA_A0;
                }


                /* Write the gamma coefficient in */
                gammaFlag |= codecWriteReg(chan->edcCtrlBusType,
                                 gammaAddrI2c,
                                 gammaReg,
                                 &(params->coeff[0]),
                                 ADV7343_MAX_GAMMA_COEFFS);
            }
            else
            {
                /* Disable the gamma curve */
                val1 &= SD_GAMMA_DI;
            }
            /* Write the value to the register */
            gammaFlag |= codecWriteReg(chan->edcCtrlBusType,
                             gammaAddrI2c,
                             ADV7343_SD_MODE_REG7,
                             &val1,
                             1u);
            if (gammaFlag == TRUE)
            {
                DBG_PRINT_ERR("ADV7343: Error from adv7343SetGammaParams\n");
                gammaStatus = IOM_EBADIO;
            }
            else
            {
                gammaStatus = IOM_COMPLETED;
            }
        }
        else
        {
            DBG_PRINT_ERR("ADV7343: Param Type not supported\n");
            gammaStatus = IOM_ENOTIMPL;
        }
    }
    return gammaStatus;
}

/**
 *  \brief  Set Video Display Mode
 *
 *  \param  chan             [IN]  Handle of the Channel Object
 *  \param  displayType      [IN]  ADV7343 Video Mode
 *  \param  outMode          [IN]  ADV7343 analog format
 *  \param  inFormat         [IN]  ADV7343 input format
 *  \param  services         [IN]  Slice service to enable
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 adv7343SetVideoDisplay(Adv7343_EncObj          *chan,
                                    Adv7343_Std             displayType,
                                    Adv7343_AnalogFormat    outMode,
                                    Adv7343_InputFormat     inFormat,
                                    Uint32                  services)
{
    Uint8 sdModeReg1, mode_reg0, sdModeReg6 = 0, mode6Select;
    Uint8 sdFscReg0, sdFscReg1, sdFscReg2, sdFscReg3;
    Uint8 setDisplayAddrI2c = 0, modeSelect = 0;
    Int32 setDisStatus = IOM_COMPLETED;
    Bool setDisplayFlag;

    assert(NULL != chan);
    VPIF_DEBUG("Entered ---> adv7343SetVideoDisplay\n");
    VPIF_DEBUG2("Std = %d, AnalogFormat = %d, ", displayType, outMode);
    VPIF_DEBUG1("InputFormat = %d", inFormat);
    VPIF_DEBUG1("Slice Vbi Service = %d\n", services);

    /* Validate all parameters */
    /* Validate Mode */
    assert((Adv7343_Std_NTSC720 == displayType) ||
           (Adv7343_Std_PAL720 == displayType) ||
           (Adv7343_Std_AUTO == displayType));

    /* Validate Analog Format */
    assert((Adv7343_AnalogFormat_SVIDEO == outMode) ||
           (Adv7343_AnalogFormat_COMPOSITE == outMode));

    /* Validate Input Format */
    assert(Adv7343_InputFormat_YCBCR422 == inFormat);
    setDisplayAddrI2c = chan->edcCtrlDeviceAddr;

    setDisStatus = adv7343SetOutputMode(chan, outMode);
    if (IOM_COMPLETED == setDisStatus)
    {
        if (Adv7343_InputFormat_YCBCR422 == inFormat)
        {
            /* Enable YUV output mode in Mode Register 0 */
            mode_reg0 = YUV_OUTPUT_SELECT;
            setDisplayFlag = codecWriteReg(chan->edcCtrlBusType,
                                 setDisplayAddrI2c,
                                 ADV7343_MODE_REG0,
                                 &mode_reg0,
                                 1u);

            /* 8-Bit 4:2:2 YCrCb Mode */
            setDisplayFlag |= codecReadReg(chan->edcCtrlBusType,
                                  setDisplayAddrI2c,
                                  ADV7343_SD_MODE_REG6,
                                  &mode6Select,
                                  1u);
            mode6Select &= (~((Uint8) SD_RGB_INPUT_EN));/* SD YCrCb input. */
            /* Write value to Mode select register */
            setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                                  setDisplayAddrI2c,
                                  ADV7343_SD_MODE_REG6,
                                  &mode6Select,
                                  1u);
        }
        /* Set the standard */
        sdModeReg1 = ADV7343_SD_MODE_REG1_DEFAULT;
        sdModeReg1 &= (~(SD_STD_MASK));
        if (Adv7343_Std_PAL720 == displayType)
        {
            sdModeReg1 |= SD_STD_PAL_BDGHI;
            if ((Adv7343_AnalogFormat_COMPOSITE == outMode) ||
                (Adv7343_AnalogFormat_SVIDEO == outMode))
            {
                /* Programme the sub carrier frequency. The sub carrier
                frequency updates only after the last sub carrier frequency
                register byte has been received by the ADV7343.*/
                sdFscReg0 = 0xCBu;
                sdFscReg1 = 0x8Au;
                sdFscReg2 = 0x09u;
                sdFscReg3 = 0x2Au;
            }
        }
        else if (Adv7343_Std_NTSC720 == displayType)
        {
            sdModeReg1 |= SD_STD_NTSC;
            if ((Adv7343_AnalogFormat_COMPOSITE == outMode) ||
                (Adv7343_AnalogFormat_SVIDEO == outMode))
            {
                /* Programme the sub carrier frequency. The sub carrier
                frequency updates only after the last sub carrier frequency
                register byte has been received by the ADV7343.*/
                sdFscReg0 = 0x1Fu;
                sdFscReg1 = 0x7Cu;
                sdFscReg2 = 0xF0u;
                sdFscReg3 = 0x21u;
            }
        }
        else
        {
            setDisplayFlag |= codecReadReg(chan->edcCtrlBusType,
                                  setDisplayAddrI2c,
                                  ADV7343_SD_MODE_REG6,
                                  &sdModeReg6,
                                  1u);
            /*enable Auto Detection*/
            sdModeReg6 |= SD_AUTO_DETECT_EN;
            setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                                  setDisplayAddrI2c,
                                  ADV7343_SD_MODE_REG6,
                                  &sdModeReg6,
                                  1u);
        }
        chan->currentVideoStd = displayType;


        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_SD_MODE_REG1,
                              &sdModeReg1,
                              1u);

        modeSelect &= (~((Uint8) INPUT_MODE_MASK));
        modeSelect |= SD_INPUT_MODE;
        /* Write value to Mode select register */
        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_MODE_SELECT_REG,
                              &modeSelect,
                              1u);

        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_SD_FSC_REG0,
                              &sdFscReg0,
                              1u);

        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_SD_FSC_REG1,
                              &sdFscReg1,
                              1u);

        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_SD_FSC_REG2,
                              &sdFscReg2,
                              1u);

        setDisplayFlag |= codecWriteReg(chan->edcCtrlBusType,
                              setDisplayAddrI2c,
                              ADV7343_SD_FSC_REG3,
                              &sdFscReg3,
                              1u);

        if (setDisplayFlag == TRUE)
        {
            DBG_PRINT_ERR("ADV7343: Error from adv7343SetVideoDisplay\n");
            setDisStatus = IOM_EBADIO;
        }
        else
        {
            setDisStatus = adv7343SetSliceVbiService(chan, &services);
            if (setDisStatus >= 0)
            {
                VPIF_INFO1("ADV7343: Slice set for %d\n", setDisStatus);
                setDisStatus = IOM_COMPLETED;
            }
            else
            {
                setDisStatus = IOM_EBADIO;
            }
        }
    }

    VPIF_DEBUG("Exited ---> adv7343SetVideoDisplay\n");
    return setDisStatus;
}


/**
 *  \brief Configure ADV7343 registers
 *
 *  \param  chan        [IN]  Handle of the Channel Object
 *  \param  param       [IN]  config param object
 *
 *  \return  Success    IOM_COMPLETED
 *           Error code, in case of error.
 */
static Int32 adv7343Config(Adv7343_EncObj *chan, Adv7343_ConfParams *param)
{
    Int32 configStatus = IOM_COMPLETED;

    VPIF_DEBUG("Entered ---> adv7343Config\n");

    configStatus = adv7343SetDefaultValues(chan);
    if (IOM_COMPLETED == configStatus)
    {
        configStatus = adv7343SetVideoDisplay(chan,
                           param->advMode,
                           param->advAnaFmt,
                           param->advInFmt,
                           param->advServices);
        if (IOM_COMPLETED == configStatus)
        {
            configStatus = adv7343EnableColorBars(chan, FALSE);
        }
    }

    VPIF_DEBUG("Exited ---> adv7343Config\n");
    return configStatus;
}


/**
 *  \brief Initialize the ADV7343 in default setup. This function is called by
 *         the vpif driver to initialize ADV7343 driver. It initializes all
 *         registers of ADV7343 with the default values.
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 adv7343SetDefaultValues(Adv7343_EncObj *chan)
{
    Int32 initStatus = IOM_COMPLETED;
    Bool initFlag;
    Uint8 initAddrI2c = 0, defValue;

    VPIF_DEBUG("Entered ---> adv7343SetDefaultValues\n");
    assert(NULL != chan);

    initAddrI2c = chan->edcCtrlDeviceAddr;

    defValue = ADV7343_HD_MODE_REG1_DEFAULT;
    initFlag = codecWriteReg(chan->edcCtrlBusType,
                   initAddrI2c,
                   ADV7343_HD_MODE_REG1,
                   &defValue,
                   1u);

    defValue = ADV7343_HD_MODE_REG2_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG2,
                    &defValue,
                    1u);

    defValue = ADV7343_HD_MODE_REG3_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG3,
                    &defValue,
                    1u);

    defValue = ADV7343_HD_MODE_REG4_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG4,
                    &defValue,
                    1u);
    defValue = ADV7343_HD_MODE_REG5_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG5,
                    &defValue,
                    1u);
    defValue = ADV7343_HD_MODE_REG6_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG6,
                    &defValue,
                    1u);
    defValue = ADV7343_HD_MODE_REG7_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_HD_MODE_REG7,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG1_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG1,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG2_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG2,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG3_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG3,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG4_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG4,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG5_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG5,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG6_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG6,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG7_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG7,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_MODE_REG8_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_MODE_REG8,
                    &defValue,
                    1u);

    defValue = ADV7343_SD_HUE_REG_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_HUE_REG,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_CGMS_WSS0_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_CGMS_WSS0,
                    &defValue,
                    1u);
    defValue = ADV7343_SD_BRIGHTNESS_WSS_DEFAULT;
    initFlag |= codecWriteReg(chan->edcCtrlBusType,
                    initAddrI2c,
                    ADV7343_SD_BRIGHTNESS_WSS,
                    &defValue,
                    1u);

    if (initFlag == TRUE)
    {
        DBG_PRINT_ERR("ADV7343: Error from adv7343SetDefaultValues\n");
        initStatus = IOM_EBADIO;
    }
    else
    {
        initStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> adv7343SetDefaultValues\n");

    return initStatus;
}


/**
 *  \brief This function is used to set the value of the control like
 *         brightness, hue.
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *  \param  ctrl   [IN]  Pointer to Adv7343_Control control structure
 *
 *  \return  Success    IOM_COMPLETED
 *           Error code, in case of error.
 */
static Int32 adv7343SetControl(Adv7343_EncObj *chan, Adv7343_Control *ctrl)
{
    Int32 ctrlStatus = IOM_COMPLETED;
    Int ctrlValue;
    Bool ctrlFlag;
    Uint8 ctrlAddrI2c = 0, ctrlSdModeReg6, writeValue;

    VPIF_DEBUG("Entered ---> adv7343SetControl\n");
    VPIF_DEBUG2("Edc_VideoType = %d, Adv7343_ControlId = %d, ",
               ctrl->advVidtype,
               ctrl->advCtrlId);
    assert(NULL != ctrl);
    assert(NULL != chan);

    ctrlValue = (Int)ctrl->advValue;
    ctrlAddrI2c = chan->edcCtrlDeviceAddr;
    writeValue = (Uint8)ctrlValue;
    if (Edc_VideoType_SD == ctrl->advVidtype)
    {
        if (Adv7343_ControlId_BRIGHTNESS == ctrl->advCtrlId)
        {
            if (ADV7343_SD_BRIGHTNESS_MAX < ctrlValue)
            {
                /* ctrlValue is always > 0 so no check made*/
                ctrlStatus = IOM_EBADARGS;
            }
            if (IOM_COMPLETED == ctrlStatus)
            {
                ctrlFlag = codecReadReg(chan->edcCtrlBusType,
                               ctrlAddrI2c,
                               ADV7343_SD_MODE_REG6,
                               &ctrlSdModeReg6,
                               1u);
                /*enable brightness*/
                ctrlSdModeReg6 |= SD_BRIGHTNESS_EN;
                ctrlFlag |= codecWriteReg(chan->edcCtrlBusType,
                                ctrlAddrI2c,
                                ADV7343_SD_MODE_REG6,
                                &ctrlSdModeReg6,
                                1u);
                /*write brightness value*/
                ctrlFlag |= codecWriteReg(chan->edcCtrlBusType,
                                ctrlAddrI2c,
                                ADV7343_SD_BRIGHTNESS_WSS,
                                &(writeValue),
                                1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("ADV7343: Error from adv7343SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
        }
        else
        {
            /* advCtrlId should always be Adv7343_ControlId_HUE here */
            if (ADV7343_SD_HUE_MAX < ctrlValue)
            {
                /* ctrlValue is always > 0 so no check made*/
                ctrlStatus = IOM_EBADARGS;
            }
            if (IOM_COMPLETED == ctrlStatus)
            {
                ctrlFlag = codecReadReg(chan->edcCtrlBusType,
                               ctrlAddrI2c,
                               ADV7343_SD_MODE_REG6,
                               &ctrlSdModeReg6,
                               1u);
                /*enable hue*/
                ctrlSdModeReg6 |= SD_HUE_ADJST_EN;
                ctrlFlag |= codecWriteReg(chan->edcCtrlBusType,
                                ctrlAddrI2c,
                                ADV7343_SD_MODE_REG6,
                                &ctrlSdModeReg6,
                                1u);
                /*write hue value*/
                ctrlFlag |= codecWriteReg(chan->edcCtrlBusType,
                                ctrlAddrI2c,
                                ADV7343_SD_HUE_REG,
                                &(writeValue),
                                1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("ADV7343: Error from adv7343SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
        }
    }
    else
    {
        DBG_PRINT_ERR("ADV7343: Param Type not supported\n");
        ctrlStatus = IOM_ENOTIMPL;
    }
    VPIF_DEBUG("Exited ---> adv7343SetControl\n");
    return ctrlStatus;
}



/**
 *  \brief Power down ADV7343
 *
 *  \param  chan            [IN] ADV7343 channel parameters
 *  \param  powerDownEnable [IN] Flag to select power down or power up mode
 *          FALSE : enable the adv7343 encoder
 *          TRUE  : disable the adv7343 encoder
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 adv7343PowerDown(Adv7343_EncObj *chan, Bool powerDownEnable)
{
    Int32 pwrDnStatus = IOM_COMPLETED;
    Bool powerFlag;
    Uint8 pwrDnMode;

    assert(NULL != chan);
    VPIF_DEBUG("Entered ---> adv7343PowerDown\n");
    VPIF_DEBUG1("powerDownEnable = %d\n", powerDownEnable);

    /* Put _ADV7343 in power down mode */
    if (powerDownEnable == TRUE)
    {
        pwrDnMode = 0x03u;
    }
    else
    {
        pwrDnMode = ADV7343_POWER_MODE_REG_DEFAULT;
    }

    powerFlag = codecWriteReg(chan->edcCtrlBusType,
                    chan->edcCtrlDeviceAddr,
                    ADV7343_POWER_MODE_REG,
                    &pwrDnMode,
                    1u);
    if (powerFlag == TRUE)
    {
        DBG_PRINT_ERR("ADV7343: Error from adv7343PowerDown\n");
        pwrDnStatus = IOM_EBADIO;
    }
    else
    {
        pwrDnStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> adv7343PowerDown\n");

    return pwrDnStatus;
}



/**
 *  \brief Reset ADV7343
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *
 *  \return  Success    IOM_COMPLETED
 *           Error code, in case of error.
 */
static Int32 adv7343Reset(Adv7343_EncObj *chan)
{
    Int32 resetStatus = IOM_COMPLETED;
    Bool resetFlag;
    Uint8 softReset = 0;

    assert(NULL != chan);
    VPIF_DEBUG("Entered ---> adv7343Reset\n");

    /* software reset to ADV7343 */
    softReset = ADV7343_SOFT_RESET_DEFAULT;
    resetFlag = codecWriteReg(chan->edcCtrlBusType,
                    chan->edcCtrlDeviceAddr,
                    ADV7343_SOFT_RESET,
                    &softReset,
                    1u);
    if (resetFlag == TRUE)
    {
        DBG_PRINT_ERR("ADV7343: Error from adv7343Reset\n");
        resetStatus = IOM_EBADIO;
    }
    else
    {
        /* power-up ADV7343 */
        resetStatus = adv7343PowerDown(chan, FALSE);
    }

    VPIF_DEBUG("Exited ---> adv7343Reset\n");
    return resetStatus;
}

/* ========================================================================== */
/*                     ADV7343 GLOBAL FUNCTION DEFINITIONS                    */
/* ========================================================================== */
/**
 *  \brief  ADV7343 Close
 *
 *  \param  handle [IN]  External device channel handle
 *
 *  \return  Success    IOM_COMPLETED
 *           Error code, in case of error.
 */
static Int32 ADV7343_close(EDC_Handle handle)
{
    Adv7343_EncObj *closeDevObj;
    Int32 closeStatus = IOM_EBADARGS;
    Uint32 services = FVID_SLICE_VBI_SERVICES_NONE;

    VPIF_DEBUG("Entered ---> ADV7343_close\n");
    VPIF_DEBUG1("EDC_Handle = 0x%0.8x\n", handle);

    /* First Check if the Handle/params are correct */
    assert(NULL != handle);
    closeDevObj = (Adv7343_EncObj *)handle;
    assert(ADV7343_IIC_ADDR == closeDevObj->edcCtrlDeviceAddr);
    assert(Edc_ControlBusType_UNKNOWN != closeDevObj->edcCtrlBusType);

    if (closeDevObj->servicesSet != FVID_SLICE_VBI_SERVICES_NONE)
    {
        adv7343SetSliceVbiService(closeDevObj, &services);
    }
    /*Put ADV7343 in power down mode */
    closeStatus = adv7343PowerDown(closeDevObj, TRUE);
    if (IOM_COMPLETED == closeStatus)
    {
        /*close I2C channel*/
        closeStatus = deInitCodecInterface(closeDevObj->edcCtrlBusType);
    }
    memset(&adv7343Obj, 0x0, sizeof(Adv7343_EncObj));
    closeDevObj->edcCtrlBusType = Edc_ControlBusType_UNKNOWN;

    VPIF_DEBUG("Exited ---> ADV7343_close\n");
    return closeStatus;
}


/**
 *  \brief  ADV7343 Ioctls
 *
 *  \param  handle [IN]  External device channel handle
 *  \param  cmd    [IN]  IOCTL to be performed.
 *  \param  param  [IN]  Argument as required by Ioctl
 *
 *  \return  Success    IOM_COMPLETED
 *           Error code, in case of error.
 */
static Int32 ADV7343_ctrl(EDC_Handle handle, Uns cmd, Ptr param)
{
    Int32 advCtrlStatus = IOM_COMPLETED;
    Adv7343_EncObj *ctrlDevObj;
    Edc_RegData *regRw = NULL;
    Bool advCtrlFlag;
    Uns advDefCmd;

    VPIF_DEBUG("Entered ---> ADV7343_ctrl\n");
    VPIF_DEBUG2("EDC_Handle = 0x%0.8X, Arg = 0x%0.8X\n", handle, param);

    /* First Check if the Handle/params are correct */
    assert(NULL != handle);
    ctrlDevObj = (Adv7343_EncObj *)handle;
    assert(ADV7343_IIC_ADDR == ctrlDevObj->edcCtrlDeviceAddr);

    if (Edc_IOCTL_CONFIG == cmd)
    {
        VPIF_DEBUG("ADV7343: Command: Edc_IOCTL_CONFIG\n");
        assert(NULL != param);
        advCtrlStatus = adv7343Config(ctrlDevObj, (Adv7343_ConfParams *)param);
    }
    else if (Edc_IOCTL_RESET == cmd)
    {
        VPIF_DEBUG("ADV7343: Command: Edc_IOCTL_RESET\n");
        advCtrlStatus = adv7343Reset(ctrlDevObj);
    }
    else if (Edc_IOCTL_SET_REG == cmd)
    {
        VPIF_DEBUG("ADV7343: Command: Edc_IOCTL_SET_REG\n");
        assert(NULL != param);
        regRw = (Edc_RegData *)param;
        assert(NULL != regRw);
        advCtrlFlag = codecWriteReg(ctrlDevObj->edcCtrlBusType,
                          ctrlDevObj->edcCtrlDeviceAddr,
                          (Uint8)regRw->startReg,
                          regRw->value,
                          regRw->noRegToRW);
        if (advCtrlFlag == TRUE)
        {
            DBG_PRINT_ERR("ADV7343: Error from ADV7343_ctrl\n");
            advCtrlStatus = IOM_EBADIO;
        }
        else
        {
            advCtrlStatus = IOM_COMPLETED;
        }
    }
    else if (Edc_IOCTL_GET_REG == cmd)
    {
        VPIF_DEBUG("ADV7343: Command: Edc_IOCTL_GET_REG\n");
        assert(NULL != param);
        regRw = (Edc_RegData *)param;
        assert(NULL != regRw);
        advCtrlFlag = codecReadReg(ctrlDevObj->edcCtrlBusType,
                          ctrlDevObj->edcCtrlDeviceAddr,
                          (Uint8)regRw->startReg,
                          regRw->value,
                          regRw->noRegToRW);
        if (advCtrlFlag == TRUE)
        {
            DBG_PRINT_ERR("ADV7343: Error from ADV7343_ctrl\n");
            advCtrlStatus = IOM_EBADIO;
        }
        else
        {
            advCtrlStatus = IOM_COMPLETED;
        }
    }
    else if (Edc_IOCTL_WRITE_SLICE_VBI_DATA == cmd)
    {
        VPIF_DEBUG("ADV7343: Command: Edc_IOCTL_WRITE_SLICE_VBI_DATA\n");
        assert(NULL != param);
        advCtrlStatus = adv7343WriteVbiData(ctrlDevObj,
                            (FVID_SliceFrame *)param);
    }
    else
    {
        advDefCmd = (Uns)(cmd - Edc_IOCTL_CMD_MAX);
        VPIF_DEBUG("ADV7343: Command: default\n");
        if (Adv7343_IOCTL_SET_CONTROL == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Command: Adv7343_IOCTL_SET_CONTROL\n");
            assert(NULL != param);
            advCtrlStatus = adv7343SetControl(ctrlDevObj,
                                (Adv7343_Control *)param);
        }
        else if (Adv7343_IOCTL_SET_GAMMA == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Command: Adv7343_IOCTL_SET_GAMMA\n");
            assert(NULL != param);
            /* Call adv7343SetGammaParams to set gamma correction parameters */
            advCtrlStatus = adv7343SetGammaParams(ctrlDevObj,
                                (Adv7343_GammaParams *)param);
        }
        else if (Adv7343_IOCTL_POWERDOWN == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Command: Adv7343_IOCTL_POWERDOWN\n");
            advCtrlStatus = adv7343PowerDown(ctrlDevObj, TRUE);
        }
        else if (Adv7343_IOCTL_POWERUP == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Command: Adv7343_IOCTL_POWERUP\n");
            advCtrlStatus = adv7343PowerDown(ctrlDevObj, FALSE);
        }
        else if (Adv7343_IOCTL_ENABLE_COLORBAR == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Command: Adv7343_IOCTL_ENABLE_COLORBAR\n");
            assert(NULL != param);
            advCtrlStatus = adv7343EnableColorBars(ctrlDevObj, *(Bool *)param);
        }
        else if (Adv7343_IOCTL_SET_SLICE_VBI_SERVICE == advDefCmd)
        {
            VPIF_DEBUG("ADV7343: Cmd: Adv7343_IOCTL_SET_SLICE_VBI_SERVICE\n");
            assert(NULL != param);
            advCtrlStatus = adv7343SetSliceVbiService(ctrlDevObj,
                                (Uint32 *)param);
        }
        else
        {
            advCtrlStatus = IOM_EBADARGS;
        }
    }

    VPIF_DEBUG("Exited ---> ADV7343_ctrl\n");
    return advCtrlStatus;
}



/**
 *  \brief  ADV7343 device open
 *
 *  \param  devName [IN]  External device name string
 *  \param  optArg  [IN]  Optional argument (not used here)
 *
 *  \return  handle  EDC channel handle when success
 *                   NULL when unsuccessful
 */
static EDC_Handle ADV7343_open(String devName, Ptr optArg)
{
    Adv7343_EncObj *handle = NULL;
    Int8 adv7343Str[20] = "ADV7343";
    Int8 i2cDevice[20] = "\0";
    Int8 encName[20] = "\0";
    /** Default configuration of ADV7343 */
    static Adv7343_ConfParams adv7343Default =
    {
        Adv7343_AnalogFormat_SVIDEO,                /* AnalogFormat         */
        Adv7343_Std_AUTO,                           /* Mode                 */
        Adv7343_InputFormat_YCBCR422,               /* InputFormat          */
        FVID_SLICE_VBI_SERVICES_NONE               /* slice vbi service    */
    };


    VPIF_DEBUG("Entered ---> ADV7343_open\n");
    VPIF_DEBUG2("devName = %s, Arg = 0x%0.8X\n", devName, optArg);

    assert(NULL != devName);

    /* Check if the address is correct */
    if (devName[0] != '/')
    {
        handle = (Adv7343_EncObj *)NULL;
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
            /*sizeof string ("/i2c0") is 6
              sizeof string ("\i2c0") is 5
              sizeof string ("ADV7343") is 8*/
            devName++;
            /*only copy 7 chars into positions 0-6 for devName*/
            strncpy(encName, devName, 7u);
            /* position 7 gets the terminator */
            encName[7] = '\0';

            devName += 7;
            /* Compare devName with "ADV7343" */
            if (0 == strcmp(encName, adv7343Str))
            {
                strncpy(adv7343Obj.codecName, adv7343Str, 20u);
                /* check if already opened  */
                if (('/' == devName[0]) && (0 == adv7343Obj.edcCtrlDeviceAddr))
                {
                    devName++;
                    adv7343Obj.edcCtrlDeviceAddr = (Uint8)strtol(
                                                              devName + 2,
                                                              NULL,
                                                              16);
                    if (ADV7343_IIC_ADDR == adv7343Obj.edcCtrlDeviceAddr)
                    {
                        strncpy(adv7343Obj.edcCtrlBusName, i2cDevice, 20u);
                        if (0 == strcmp(i2cDevice, "/I2C0"))
                        {
                            adv7343Obj.edcCtrlBusType = Edc_ControlBusType_I2C;
                        }
                        else
                        {
                            adv7343Obj.edcCtrlBusType = \
                                Edc_ControlBusType_UNKNOWN;
                        }
                        /* assigning channel object structure */
                        handle = &adv7343Obj;
                    }
                    else
                    {
                        /* wrong i2c address */
                        handle = (Adv7343_EncObj *)NULL;
                        adv7343Obj.codecName[0] = '\0';
                    }

                }
                else
                {
                    /* no i2c address in the string or device already open*/
                    handle = (Adv7343_EncObj *)NULL;
                    adv7343Obj.codecName[0] = '\0';
                }
            }
            else
            {
                /* Encoder name incorrect */
                handle = (Adv7343_EncObj *)NULL;
            }
        }
        else
        {
            /* no encoder name */
            handle = (Adv7343_EncObj *)NULL;
        }
    }
    if (handle != NULL)
    {

	    if (optArg != NULL)
	    {
	        adv7343Obj.sliceCbFxn = ((Edc_Data *)optArg)->cbFxn;
	        adv7343Obj.sliceCbData = ((Edc_Data *)optArg)->cbData;
		}

        /* open I2C channel */
        if (IOM_COMPLETED != initCodecInterface(
                                (String)adv7343Obj.edcCtrlBusName))
        {
            handle = (Adv7343_EncObj *)NULL;
        }

        /* power on decoder */
        if (handle != NULL)
        {
            if (IOM_COMPLETED != adv7343Reset(handle))
            {
                    deInitCodecInterface(adv7343Obj.edcCtrlBusType);
                    handle = (Adv7343_EncObj *)NULL;
                    adv7343Obj.edcCtrlDeviceAddr = 0;
            }
            else
            {
                /* Configure ADV7343 for default video standard */
                if (IOM_COMPLETED != ADV7343_ctrl((EDC_Handle)handle,
                                    (Uns)Edc_IOCTL_CONFIG,
                                    (Ptr)&adv7343Default))
                {
                    deInitCodecInterface(adv7343Obj.edcCtrlBusType);
                    handle = (Adv7343_EncObj *)NULL;
                    adv7343Obj.edcCtrlDeviceAddr = 0;
                }
            }
        }
        else
        {
            DBG_PRINT_ERR("ADV7343: ERR ---> edcEncoder_open\n");
        }
    }

    VPIF_DEBUG("Exited ---> ADV7343_open\n");
    return (EDC_Handle)handle;
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
