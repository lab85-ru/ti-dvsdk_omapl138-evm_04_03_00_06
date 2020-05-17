/*
 * Tvp5147.c
 *
 * This file contains Driver Layer Interface implementation for Tvp5147 decoder
 * driver. Tvp5147 decoder driver provides Driver Layer Interface to do
 * operations on the Tvp5147 decoder like open, close and control commands for
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
 *  \file   Tvp5147.c
 *
 *  \brief  This file contains interface functions for TVP5147 decoder
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
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/Tvp5147.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/src/Tvp5147Local.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/src/CodecInterfaceLocal.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/**\brief No of retries if lost lock detect is found */
#define LOCK_RETRY_COUNT                (5)
/**\brief Delay between the check for lost lock detect */
#define LOCK_RETRY_DELAY                (200u)
/**\brief Macro defining all the vbi silce services supported by TVP5147 */
#define TVP5147_ALL_VBI_SERVICES        (FVID_SLICE_VBI_SERVICES_WSS_PAL | \
                                         FVID_SLICE_VBI_SERVICES_CGMS_NTSC | \
                                         FVID_SLICE_VBI_SERVICES_CC_NTSC)

/* ========================================================================== */
/*                        LOCAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
static Int32 tvp5147SetSliceVbiService(Tvp5147_DecObj *chan, Uint32 *services);

static Int32 tvp5147ReadVbiData(Tvp5147_DecObj *chan, FVID_SliceFrame data[]);

static Tvp5147_Std tvp5147GetCurrentStd(Tvp5147_DecObj *chan);

static Void tvp5147Detect(Tvp5147_DecObj *chan);

static Int32 tvp5147SetDefaultValues(Tvp5147_DecObj *chan);

static Int32 tvp5147PowerDown(Tvp5147_DecObj *chan, Bool powerDownEnable);

static Int32 tvp5147Reset(Tvp5147_DecObj *chan);

static Int32 tvp5147SetControl(Tvp5147_DecObj *chan, Tvp5147_Control *ctrl);

static Int32 tvp5147Config(Tvp5147_DecObj *chan, Tvp5147_ConfParams *params);

static Void tvp5147WaitMsec(Uint32 msec);

static Int32 tvp5147QueryStd(Tvp5147_DecObj *chan, Tvp5147_AnalogFormat fmt);

static Int32 TVP5147_close(EDC_Handle edcHandle);

static Int32 TVP5147_ctrl(EDC_Handle edcHandle, Uns cmd, Ptr arg);

static EDC_Handle TVP5147_open(String devName, Ptr optArg);

/* ========================================================================== */
/*                            FUNCTION TABLE                                  */
/* ========================================================================== */
/** TVP5147 EDC function table */
EDC_Fxns TVP5147_Fxns =
{
    &TVP5147_open,
    &TVP5147_close,
    &TVP5147_ctrl
};

/* ========================================================================== */
/*                            LOCAL VARIABLES                                 */
/* ========================================================================== */
/** Local structure array to contain channel objects, codec bus interface
 * and channel open count
 */
static Tvp5147_DecObj decObj[NUM_DECODERS] =
{
    {"\0", Edc_ControlBusType_UNKNOWN, "\0", 0, -1, 0, 0, Tvp5147_Std_INVALID},
    /*CONN_SVIDEO*/
    {"\0", Edc_ControlBusType_UNKNOWN, "\0", 0, -1, 0, 0, Tvp5147_Std_INVALID}
    /*CONN_COMPOSITE*/
};

/* ========================================================================== */
/*                      LOCAL FUNCTION DEFINITIONS                            */
/* ========================================================================== */
/**
 *  \brief  This function is used to set sliced vbi services
 *
 *  \param  chan        [IN]  Handle of the Channel Object
 *  \param  services    [IN]  Pointer to the slice service
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 *
 */
static Int32 tvp5147SetSliceVbiService(Tvp5147_DecObj *chan, Uint32 *services)
{
    Int32 setVbiServStatus = IOM_COMPLETED;
    Uint8 val, setVbiNumServices = 0;
    Uint32 i, j, k;
    Bool vbiServiceFlag;
    Tvp5147_ServiceReg tvp5147SliceRegs[TVP5147_VBI_NUM_SERVICES] =
    {
        {
            FVID_SLICE_VBI_SERVICES_CC_NTSC,
            0x15, /* Line number 21*/
            19,/* Line start*/
            23,/* Line end*/
            {
                {/* field 0*/
                    {0x00, 0x06, 0x80},/*Line address 1*/
                    0x01/*Field 1[Bit 3:0] and CC (US, Europe, Japan, China)*/
                },
                {/* field 1*/
                    {0x02, 0x06, 0x80},/*Line address 2*/
                    0x09/*Field 2[Bit 3:1] and CC (US, Europe, Japan, China) */
                }
            },
            Tvp5147_Std_NTSC720
        },
        {
            FVID_SLICE_VBI_SERVICES_WSS_PAL,
            0x17,/* Line number 23*/
            21,/* Line start*/
            25,/* Line end*/
            {
                {/* field 0*/
                    {0x04, 0x06, 0x80},/*Line address 3*/
                    0x02/*WSS (525, 625)*/
                },
                {/* field 1*/
                    {0x04, 0x06, 0x80},/*Line address 3*/
                    0x02/*WSS (525, 625)*/
                }
            },
            Tvp5147_Std_PAL720
        },
        {
            FVID_SLICE_VBI_SERVICES_CGMS_NTSC,
            0x14, /* Line number 20*/
            18,/* Line start*/
            22,/* Line end*/
            {
                {/* field 0*/
                    {0x08, 0x06, 0x80},/*Line address 5*/
                    0x02/*WSS (525, 625)*/
                },
                {/* field 1*/
                    {0x06, 0x06, 0x80},/*Line address 4*/
                    0x02/*WSS (525, 625)*/
                }
            },
            Tvp5147_Std_NTSC720
        }
    };

    assert(NULL != chan);
    assert(NULL != services);

    VPIF_DEBUG("Entered ---> tvp5147SetSliceVbiService\n");

    if (((*services) | TVP5147_ALL_VBI_SERVICES) != TVP5147_ALL_VBI_SERVICES)
    {
        DBG_PRINT_ERR("TVP5147M1: Invalid VBI service\n");
        setVbiServStatus = IOM_EBADARGS;
    }

    if (IOM_COMPLETED == setVbiServStatus)
    {
        for (i = 0; i < TVP5147_VBI_NUM_SERVICES; i++)
        {

            if ((0 != ((*services) & tvp5147SliceRegs[i].service)) &&
                (tvp5147SliceRegs[i].std != chan->currentVideoStd))
            {
                DBG_PRINT_ERR("TVP5147M1: Invalid service for this standard\n");
                setVbiServStatus = IOM_EBADMODE;
                break;
            }
            /* Configure for both fields*/
            for (j = 0; j < 2; j++)
            {
                for (k = 0; k < 3; k++)
                {
                    /* write the address of Line Address Register*/
                    vbiServiceFlag = codecWriteReg(chan->edcCtrlBusType,
                                 chan->edcCtrlDeviceAddr,
                                 (Uint8)(TVP5147_VBUS_ADDRESS_ACCESS1 + k),
                                 &tvp5147SliceRegs[i].field[j].fifoLineAddr[k],
                                 1u);
                    if (vbiServiceFlag == TRUE)
                    {
                        DBG_PRINT_ERR("TVP5147: Error in SetSliceVbiService\n");
                    }
                }

                if (0 != ((*services) & tvp5147SliceRegs[i].service))
                {
                    setVbiNumServices++;
                    /*Write to Line address [7:0]: Line number to be processed
                    by a VDP set by a line mode register*/
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                          chan->edcCtrlDeviceAddr,
                                          TVP5147_VBUS_DATA_ACCESS_AUTO_INCR,
                                          &tvp5147SliceRegs[i].lineNo,
                                          1u);
                    /*write to Line mode register [7:0]:*/
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                      chan->edcCtrlDeviceAddr,
                                      TVP5147_VBUS_DATA_ACCESS_AUTO_INCR,
                                      &tvp5147SliceRegs[i].field[j].fifoModeVal,
                                      1u);
                    /*
                    VDP line start [7:0]: Set the VDP line starting address
                    This register must be set properly before enabling the
                    line mode registers. The VDP processor works only the
                    VBI region set by this register and the VDP line stop
                    register.
                    */
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                          chan->edcCtrlDeviceAddr,
                                          TVP5147_VDP_LINE_START,
                                          &tvp5147SliceRegs[i].lineStart,
                                          1u);
                    /*VDP line stop [7:0]: Set the VDP stop line address*/
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                          chan->edcCtrlDeviceAddr,
                                          TVP5147_VDP_LINE_STOP,
                                          &tvp5147SliceRegs[i].lineEnd,
                                          1u);
                }
                else
                {
                    /* (default line mode = FFh, address = 00h) */
                    val = TVP5147_LINE_ADDRESS_DEFAULT;
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                          chan->edcCtrlDeviceAddr,
                                          TVP5147_VBUS_DATA_ACCESS_AUTO_INCR,
                                          &val,
                                          1u);
                    val = TVP5147_LINE_MODE_DEFAULT;
                    vbiServiceFlag |= codecWriteReg(chan->edcCtrlBusType,
                                          chan->edcCtrlDeviceAddr,
                                          TVP5147_VBUS_DATA_ACCESS_AUTO_INCR,
                                          &val,
                                          1u);
                }
                if (vbiServiceFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147: Error from SetSliceVbiService\n");
                    setVbiServStatus = IOM_EBADIO;
                    break;
                }
            }
        }
    }

    if (IOM_COMPLETED == setVbiServStatus)
    {
        chan->servicesSet = *services;
        chan->numServices = setVbiNumServices / 2u;
        setVbiServStatus = (Int32)chan->numServices;
    }
    else
    {
        chan->numServices = 0;
        chan->servicesSet = 0;
    }
    VPIF_DEBUG("Exited ---> tvp5147SetSliceVbiService\n");
    return setVbiServStatus;
}



/**
 *  \brief  This function is used to write the vbi data to the decoder device
 *
 *  \param  chan        [IN]  Handle of the Channel Object
 *  \param  data        [OUT]  Array containing Slice VBI data from TVP5147
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 *
 */
static Int32 tvp5147ReadVbiData(Tvp5147_DecObj *chan, FVID_SliceFrame data[])
{
    Int32 readVbiStatus = IOM_EBADARGS;
    Uint8 i = 0, j, k;
    Uint8 value;
    Uint8 tvpFld = 0;
    Uint8 readVbiNumServices;
    Bool readVbiFlag = FALSE;
    Tvp5147_ServiceDataReg tvpServicesRegs[TVP5147_VBI_NUM_SERVICES] =
    {
        {
            FVID_SLICE_VBI_SERVICES_WSS_PAL,
            {
                {
                    {0x20, 0x05, 0x80}
                    /*Subaddress(80 0520h) of VDP WSS Data Register for
                    field 0 */
                },
                {
                    {0x24, 0x05, 0x80}
                    /*Subaddress(80 0524h) of VDP WSS Data Register for
                    field 1 */
                }
            },
            2/* Bytes to read */
        },
        {
            FVID_SLICE_VBI_SERVICES_CC_NTSC,
            {
                {
                    {0x1C, 0x05, 0x80}
                    /*Subaddress(80 051Ch) of VDP Closed Caption Data Register
                    for field 0 */
                },
                {
                    {0x1E, 0x05, 0x80}
                    /*Subaddress(80 051Eh) of VDP Closed Caption Data Register
                    for field 1 */
                }
            },
            2/* Bytes to read */
        },
        {
            FVID_SLICE_VBI_SERVICES_CGMS_NTSC,
            {
                {
                    {0x20, 0x05, 0x80}
                    /*Subaddress(80 0520h) of VDP WSS Data Register for
                    field 0 */
                },
                {
                    {0x24, 0x05, 0x80}
                    /*Subaddress(80 0524h) of VDP WSS Data Register for
                    field 1 */
                }
            },
            3/* Bytes to read */
        }
    };

    if ((NULL != chan) && (NULL != data))
    {
        readVbiStatus = IOM_COMPLETED;
        readVbiNumServices = chan->numServices;
        for (i = 0; i < readVbiNumServices; i++)
        {
            if (FVID_SLICE_VBI_SERVICES_NONE != data[i].fvidSliceServiceId)
            {
                if ((data[i].fvidSliceServiceId | TVP5147_ALL_VBI_SERVICES) !=
                    TVP5147_ALL_VBI_SERVICES)
                {
                    readVbiStatus = IOM_EBADARGS;
                    break;
                }

                for (j = 0; j < TVP5147_VBI_NUM_SERVICES; j++)
                {
                    if (tvpServicesRegs[j].tvpSliceService ==
                        (tvpServicesRegs[j].tvpSliceService &
                        data[i].fvidSliceServiceId))
                    {
                        for (k = 0; k < 3; k++)
                        {
                            tvpFld = data[i].fvidField;
                            readVbiFlag |= codecWriteReg(chan->edcCtrlBusType,
                              chan->edcCtrlDeviceAddr,
                              (Uint8)(TVP5147_VBUS_ADDRESS_ACCESS1 + k),
                              &tvpServicesRegs[j].tvpSliceField[tvpFld].addr[k],
                              1u);
                        }
                        for (k = 0; k < tvpServicesRegs[j].bytestoread; k++)
                        {
                            readVbiFlag |= codecReadReg(chan->edcCtrlBusType,
                                           chan->edcCtrlDeviceAddr,
                                           TVP5147_VBUS_DATA_ACCESS_AUTO_INCR,
                                           &value,
                                           1u);
                            data[i].fvidData[k] = value;
                        }
                    }
                    if (readVbiFlag == TRUE)
                    {
                        readVbiStatus = IOM_EBADIO;
                        break;
                    }
                }
            }
        }
    }
    return readVbiStatus;
}



/**
 * \brief  Delay function for TVP5147 device interface. Wait in a software
 *         loop for 'msec' milliseconds
 *
 * \param  msec  [IN]  Number of milli seconds required in the delay loop.
 *
 * \return None.
 */
static Void tvp5147WaitMsec(Uint32 msec)
{
    Uint32 delay = 1000u * msec * 28u;
    volatile Uint32 i;
    for (i = 0; i < delay; i++)
    {
        ;
    }
}


/**
 *  \brief  This function get the current standard detected by TVP5147
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *
 *  \return TVP video standard.
 *
 */
static Tvp5147_Std tvp5147GetCurrentStd(Tvp5147_DecObj *chan)
{
    Uint32 tvpGetNumBytes = 1u;
    Uint8 tvpGetStd, tvpGetStatus;
    Uint8 tvpGetAddrI2c = 0;
    Bool tvpGetFlag;
    Tvp5147_Std tvpGetMode;

    assert(NULL != chan);
    tvpGetAddrI2c = chan->edcCtrlDeviceAddr;

    VPIF_DEBUG("Entered ---> tvp5147GetCurrentStd\n");

    tvpGetFlag = codecReadReg(chan->edcCtrlBusType,
                     tvpGetAddrI2c,
                     TVP5147_VIDEO_STD,
                     &tvpGetStd,
                     tvpGetNumBytes);


    if ((tvpGetStd & VIDEO_STD_MASK) == VIDEO_STD_AUTO_SWITCH_BIT)
    {
        /* use the standard status register */
        tvpGetFlag |= codecReadReg(chan->edcCtrlBusType,
                          tvpGetAddrI2c,
                          TVP5147_VIDEO_STD_STATUS,
                          &tvpGetStatus,
                          tvpGetNumBytes);
        if (TRUE == tvpGetFlag)
        {
            DBG_PRINT_ERR("TVP5147M1: Standard detection failed\n");
        }
    }
    else
    {
        tvpGetStatus = tvpGetStd;/* use the standard register itself */
    }

    switch (tvpGetStatus & VIDEO_STD_MASK)
    {
        case VIDEO_STD_NTSC_MJ_BIT:
            tvpGetMode = Tvp5147_Std_NTSC720;
            VPIF_INFO("TVP5147M1: Standard detected is STD_NTSC_MJ\n");
            break;

        case VIDEO_STD_PAL_BDGHIN_BIT:
            tvpGetMode = Tvp5147_Std_PAL720;
            VPIF_INFO("TVP5147M1: Standard detected is STD_PAL_BDGHIN\n");
            break;

        default:
            tvpGetMode = Tvp5147_Std_INVALID;
            VPIF_INFO("TVP5147M1: Standard detected is STD_INVALID\n");
            break;
    }
    return tvpGetMode;
}

/**
 *  \brief  This function checks the input detected by TVP5147
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *  \param  fmt    [IN]  Analog input format to detect
 *
 *  \return IOM_COMPLETED, If success.
 *          Error code,    If no active input is detected
 *
 */
static Int32 tvp5147QueryStd(Tvp5147_DecObj *chan, Tvp5147_AnalogFormat fmt)
{
    Int32 queryStatus = IOM_COMPLETED;
    Uint32 queryNumBytes = 1u;
    Uint8 lockStatus = 0xFFu;
    Uint8 queryLockMask;
    Uint8 configAddrI2c = 0;
    Bool queryFlag;
    Tvp5147_Std mode;

    assert(NULL != chan);
    configAddrI2c = chan->edcCtrlDeviceAddr;

    VPIF_DEBUG("Entered ---> tvp5147QueryStd.\n");

    mode = tvp5147GetCurrentStd(chan);
    chan->currentVideoStd = mode;
    if (Tvp5147_Std_INVALID == mode)
    {
        DBG_PRINT_ERR("TVP5147M1: Standard not detected\n");
    }

    /* check whether signal is locked */
    /* check lock status */
    queryFlag = codecReadReg(chan->edcCtrlBusType,
                    configAddrI2c,
                    TVP5147_STATUS1,
                    &lockStatus,
                    queryNumBytes);
    if (Tvp5147_AnalogFormat_SVIDEO == fmt)
    {
        queryLockMask = STATUS_HORZ_SYNC_LOCK_BIT | STATUS_VIRT_SYNC_LOCK_BIT;
    }
    else if (Tvp5147_AnalogFormat_COMPOSITE == fmt)
    {
        queryLockMask = STATUS_CLR_SUBCAR_LOCK_BIT |
                        STATUS_HORZ_SYNC_LOCK_BIT |
                        STATUS_VIRT_SYNC_LOCK_BIT;
    }
    else
    {
        DBG_PRINT_ERR("TVP5147M1: Analog format not supported\n");
    }

    if (queryLockMask != (lockStatus & queryLockMask))
    {
        /* No input detected */
        DBG_PRINT_ERR("TVP5147M1: Error in lock. No input detected\n");
    }

    if (TRUE == queryFlag)
    {
        DBG_PRINT_ERR("TVP5147M1: Error from tvp5147Config\n");
        queryStatus = IOM_EBADIO;
    }
    else
    {
        queryStatus = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> tvp5147QueryStd\n");
    return queryStatus;
}



/**
 *  \brief  Configure registers of TVP5147
 *
 *  \param  chan    [IN]  TVP5147 channel parameters
 *  \param  params  [IN]  TVP5147 configuration parameter structure pointer
 *
 *  \return IOM_COMPLETED on successful operation
 *          Error code, in case of error.
 */
static Int32 tvp5147Config(Tvp5147_DecObj *chan, Tvp5147_ConfParams *params)
{
    Uint8 confAddrI2c = 0, confVal, confStatus, outputSel, confOutput1;
    Uint32 tryCount = LOCK_RETRY_COUNT;
    Int32 configStatus = IOM_COMPLETED;
    Uint8 analogFmt, configMode;
    Uint32 confNumBytes = 1u;
    Tvp5147_Std currentStd;
    Uint8 lockMask;
    Bool configFlag;

    assert(NULL != chan);
    assert(NULL != params);

    VPIF_DEBUG("Entered ---> tvp5147Config\n");
    VPIF_DEBUG2("Tvp5147_DecObj = 0x%0.8X , Tvp5147_ConfParams = 0x%0.8X\n",
               (Int32)chan,
               params);
    VPIF_DEBUG2("Std = %d, AnalogFormat = %d, ", params->tvpMode,
                params->tvpAnaFmt);
    VPIF_DEBUG1("InputFormat = %d", params->tvpOutFmt);
    VPIF_DEBUG1("Slice Vbi Service = %d\n", params->tvpServices);

    confAddrI2c = chan->edcCtrlDeviceAddr;

    tvp5147SetDefaultValues(chan);
    assert(Tvp5147_Std_INVALID != params->tvpMode);

    /* Got valid Std. Call set standard to set default standard */
    configMode = (Uint8)params->tvpMode;

    /* setup the requested video standard */
    confVal = (configMode & VIDEO_STD_MASK);
    configFlag = codecWriteReg(chan->edcCtrlBusType,
                     confAddrI2c,
                     TVP5147_VIDEO_STD,
                     &confVal,
                     confNumBytes);

    /* if auto switch mode, enable all modes for auto switch */
    if ((configMode & VIDEO_STD_MASK) == VIDEO_STD_AUTO_SWITCH_BIT)
    {
        confVal = TVP5147_AUTOSWITCH_MASK_VALUE;
        configFlag |= codecWriteReg(chan->edcCtrlBusType,
                          confAddrI2c,
                          TVP5147_AUTOSWITCH_MASK,
                          &confVal,
                          confNumBytes);
    }

    if (Tvp5147_AnalogFormat_SVIDEO == params->tvpAnaFmt)
    {
        analogFmt = TVP5147_SVIDEO_INPUT;
        lockMask = STATUS_HORZ_SYNC_LOCK_BIT |
            STATUS_VIRT_SYNC_LOCK_BIT;


    }
    else
    {
        /*tvpAnaFmt should be only Tvp5147_AnalogFormat_COMPOSITE here because
        of enum */
        analogFmt = TVP5147_COMPOSITE_INPUT;
        lockMask = STATUS_CLR_SUBCAR_LOCK_BIT |
            STATUS_HORZ_SYNC_LOCK_BIT |
            STATUS_VIRT_SYNC_LOCK_BIT;
    }

    configFlag |= codecWriteReg(chan->edcCtrlBusType,
                      confAddrI2c,
                      TVP5147_INPUT_SEL,
                      &analogFmt,
                      confNumBytes);

    if (Tvp5147_OutputFormat_YCBCR422 == params->tvpOutFmt)
    {
        outputSel = TVP5147_OUTPUT1_DEFAULT;
        configFlag |= codecReadReg(chan->edcCtrlBusType,
                          confAddrI2c,
                          TVP5147_OUTPUT_FORMATTER1,
                          &confOutput1,
                          confNumBytes);
        outputSel |= (confOutput1 & 0x7);
        configFlag |= codecWriteReg(chan->edcCtrlBusType,
                          confAddrI2c,
                          TVP5147_OUTPUT_FORMATTER1,
                          &outputSel,
                          confNumBytes);
    }

    /* Clear status */
    tvp5147WaitMsec(200u);
    confVal = 0x01u;
    configFlag |= codecWriteReg(chan->edcCtrlBusType,
                      confAddrI2c,
                      TVP5147_CLEAR_LOST_LOCK,
                      &confVal,
                      confNumBytes);

    while (tryCount-- > 0)
    {
        /* Allow decoder to sync up with new input */
        /* wait here so that if lock is lost, it can be detected */
        tvp5147WaitMsec(LOCK_RETRY_DELAY);

        /* get the current standard for future reference */
        currentStd = tvp5147GetCurrentStd(chan);
        chan->currentVideoStd = currentStd;
        if (currentStd != Tvp5147_Std_INVALID)
        {
            configFlag |= codecReadReg(chan->edcCtrlBusType,
                              confAddrI2c,
                              TVP5147_STATUS1,
                              &confStatus,
                              confNumBytes);

            if (TRUE == configFlag)
            {
                DBG_PRINT_ERR("TVP5147M1: Error reading status1 reg\n");
            }
            if (lockMask == (confStatus & lockMask))
            {
                if ((STATUS_CLR_SUBCAR_LOCK_BIT | STATUS_HORZ_SYNC_LOCK_BIT |
                     STATUS_VIRT_SYNC_LOCK_BIT) == lockMask)
                {
                    VPIF_INFO("TVP5147M1: Composite Input detected\n");
                }
                if ((STATUS_HORZ_SYNC_LOCK_BIT | STATUS_VIRT_SYNC_LOCK_BIT) ==
                    lockMask)
                {
                    VPIF_INFO("TVP5147M1: Svideo Input detected\n");
                }
                break;/* Input detected */
            }
        }
    }

    if ((currentStd == Tvp5147_Std_INVALID) || (tryCount <= 0))
    {
        DBG_PRINT_ERR("TVP5147M1: Input detection(c/s) failed and standard ");
        DBG_PRINT_ERR("is wrong(N/P)\n");
    }

    if (TRUE == configFlag)
    {
        DBG_PRINT_ERR("TVP5147M1: Error from tvp5147Config\n");
        configStatus = IOM_EBADIO;
    }
    else
    {
        /* get the current standard for future reference */
        configStatus = tvp5147QueryStd(chan, params->tvpAnaFmt);
    }

    if (IOM_COMPLETED == configStatus)
    {
        configStatus = tvp5147SetSliceVbiService(chan, &(params->tvpServices));
        if (configStatus >= 0)
        {
            VPIF_INFO1("TVP5147M1: Slice set for %d\n", configStatus);
            configStatus = IOM_COMPLETED;
        }
        else
        {
            configStatus = IOM_EBADIO;
        }
    }

    VPIF_DEBUG("Exited ---> tvp5147Config\n");

    return configStatus;
}


/**
 *  \brief This function initialize the decoder device TVP5147 in default setup
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *
 *  \return  Success    IOM_COMPLETED
 *           Error      IOM_EBADIO
 */
static Int32 tvp5147SetDefaultValues(Tvp5147_DecObj *chan)
{
    Uint8 setDefAddrI2c = 0, i, setDefVal;
    Uint32 setDefNumBytes = 1u;
    Bool setDefFlag;
    Int32 retVal = IOM_COMPLETED;

    Uint8 tvp5147AfterResetReg[][2] =
    {
        {TVP5147_VBUS_ADDRESS_ACCESS1, 0x02},
        {TVP5147_VBUS_ADDRESS_ACCESS2, 0x00},
        {TVP5147_VBUS_ADDRESS_ACCESS3, 0x80},
        {TVP5147_VBUS_DATA_ACCESS_NO_VBUS_ADDR_INCR, 0x01},
        {TVP5147_VBUS_ADDRESS_ACCESS1, 0x60},
        {TVP5147_VBUS_ADDRESS_ACCESS2, 0x00},
        {TVP5147_VBUS_ADDRESS_ACCESS3, 0xB0},
        {TVP5147_VBUS_DATA_ACCESS_NO_VBUS_ADDR_INCR, 0x01},
        {TVP5147_VBUS_ADDRESS_ACCESS1, 0x16},
        {TVP5147_VBUS_ADDRESS_ACCESS2, 0x00},
        {TVP5147_VBUS_ADDRESS_ACCESS3, 0xA0},
        {TVP5147_VBUS_DATA_ACCESS_NO_VBUS_ADDR_INCR, 0x16},
        {TVP5147_VBUS_ADDRESS_ACCESS1, 0x60},
        {TVP5147_VBUS_ADDRESS_ACCESS2, 0x00},
        {TVP5147_VBUS_ADDRESS_ACCESS3, 0xB0},
        {TVP5147_VBUS_DATA_ACCESS_NO_VBUS_ADDR_INCR, 0x00}
    };

    VPIF_DEBUG("Entered ---> tvp5147PowerDown\n");
    VPIF_DEBUG1("Tvp5147_DecObj = 0x%0.8X\n", chan);

    assert(NULL != chan);
    setDefAddrI2c = chan->edcCtrlDeviceAddr;

    for (i = 0; i < 16; i++)
    {
        setDefFlag = codecWriteReg(chan->edcCtrlBusType,
                         setDefAddrI2c,
                         tvp5147AfterResetReg[i][0],
                         &tvp5147AfterResetReg[i][1],
                         setDefNumBytes);
    }

    setDefVal = TVP5147_COMPOSITE_INPUT;/* Composite selected */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                      setDefAddrI2c,
                      TVP5147_INPUT_SEL,
                      &setDefVal,
                      setDefNumBytes);

    setDefVal = TVP5147_AFE_GAIN_CTRL_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                      setDefAddrI2c,
                      TVP5147_AFE_GAIN_CTRL,
                      &setDefVal,
                      setDefNumBytes);

    setDefVal = VIDEO_STD_AUTO_SWITCH_BIT;/* Auto mode */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                      setDefAddrI2c,
                      TVP5147_VIDEO_STD,
                      &setDefVal,
                      setDefNumBytes);

    setDefVal = TVP5147_COLOR_KILLER_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_COLOR_KILLER,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_LUMA_CONTROL1_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_LUMA_CONTROL1,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_LUMA_CONTROL2_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_LUMA_CONTROL2,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_LUMA_CONTROL3_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_LUMA_CONTROL3,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_BRIGHTNESS_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_BRIGHTNESS,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_CONTRAST_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_CONTRAST,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_SATURATION_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_SATURATION,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_HUE_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_HUE,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_CHROMA_CONTROL1_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_CHROMA_CONTROL1,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_CHROMA_CONTROL2_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_CHROMA_CONTROL2,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x80u;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_COMP_PR_SATURATION,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x80u;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_COMP_Y_CONTRAST,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x80u;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_COMP_PB_SATURATION,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x80u;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_COMP_Y_BRIGHTNESS,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x00;/* HS, VS active high */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_SYNC_CONTROL,
                                &setDefVal,
                                setDefNumBytes);

    /* Configuration for 10-bit BT656 mode */
    setDefVal = TVP5147_OUTPUT1_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER1,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_OUTPUT2_DEFAULT;/* Enable clk output & data(YCbCr) */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER2,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_OUTPUT3_DEFAULT;/* Enable AVID & FLD */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER3,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_OUTPUT4_DEFAULT;/* Enable VS & HS */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER4,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_OUTPUT5_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER5,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = TVP5147_OUTPUT6_DEFAULT;
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_OUTPUT_FORMATTER6,
                                &setDefVal,
                                setDefNumBytes);

    setDefVal = 0x01u;/* Clear status */
    setDefFlag |= codecWriteReg(chan->edcCtrlBusType,
                                setDefAddrI2c,
                                TVP5147_CLEAR_LOST_LOCK,
                                &setDefVal,
                                setDefNumBytes);

    if (TRUE == setDefFlag)
    {
        DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetDefaultValues\n");
        retVal = IOM_EBADIO;
    }
    else
    {
        retVal = IOM_COMPLETED;
    }

    return retVal;
}


/**
 *  \brief  This function is used to set control parameters of TVP5147
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *  \param  ctrl   [IN]  Control structure
 *
 *  \return IOM_COMPLETED, If the requested control is supported, sets the
 *                         control's current value in HW.
 *          Error code,    In case of error or if the control is not supported.
 *
 */
static Int32 tvp5147SetControl(Tvp5147_DecObj *chan, Tvp5147_Control *ctrl)
{
    Int32 ctrlStatus = IOM_COMPLETED;
    Int ctrlValue;
    Bool ctrlFlag;
    Uint8 writeValue;

    VPIF_DEBUG("Entered ---> tvp5147SetControl\n");
    VPIF_DEBUG2("Edc_VideoType = %d, Tvp5147_ControlId = %d ",
               ctrl->tvpVidtype,
               ctrl->tvpCtrlId);

    /* check for null pointer */
    assert(NULL != ctrl);
    assert(NULL != chan);

    ctrlValue = (Int)ctrl->tvpValue;

    if (Tvp5147_ControlId_AUTO_GAIN == ctrl->tvpCtrlId)
    {
        if (ctrlValue == 1)
        {
            ctrlValue = 0x0F;
        }
        else if (ctrlValue == 0)
        {
            ctrlValue = 0x0C;
        }
        else
        {
            ctrlStatus = IOM_EBADARGS;
        }
    }
    else
    {
        if (255 < ctrlValue)
        {
            /* ctrlValue is always > 0 so no check made*/
            ctrlStatus = IOM_EBADARGS;
        }
    }
    writeValue = (Uint8)ctrlValue;
    if (IOM_COMPLETED == ctrlStatus)
    {
        if (Edc_VideoType_SD == ctrl->tvpVidtype)
        {
            if (Tvp5147_ControlId_BRIGHTNESS == ctrl->tvpCtrlId)
            {
                ctrlFlag = codecWriteReg(chan->edcCtrlBusType,
                               chan->edcCtrlDeviceAddr,
                               TVP5147_BRIGHTNESS,
                               &writeValue,
                               1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
            else if (Tvp5147_ControlId_HUE == ctrl->tvpCtrlId)
            {
                ctrlFlag = codecWriteReg(chan->edcCtrlBusType,
                               chan->edcCtrlDeviceAddr,
                               TVP5147_HUE,
                               &writeValue,
                               1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
            else if (Tvp5147_ControlId_AUTO_GAIN == ctrl->tvpCtrlId)
            {
                ctrlFlag = codecWriteReg(chan->edcCtrlBusType,
                               chan->edcCtrlDeviceAddr,
                               TVP5147_AFE_GAIN_CTRL,
                               &writeValue,
                               1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
            else if (Tvp5147_ControlId_CONTRAST == ctrl->tvpCtrlId)
            {
                ctrlFlag = codecWriteReg(chan->edcCtrlBusType,
                               chan->edcCtrlDeviceAddr,
                               TVP5147_CONTRAST,
                               &writeValue,
                               1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetControl\n");
                    ctrlStatus = IOM_EBADIO;
                }
                else
                {
                    ctrlStatus = IOM_COMPLETED;
                }
            }
            else
            {
                /* tvpCtrlId should be Tvp5147_ControlId_SATURATION only if
                this condition occurs because of enum*/
                ctrlFlag = codecWriteReg(chan->edcCtrlBusType,
                               chan->edcCtrlDeviceAddr,
                               TVP5147_SATURATION,
                               &writeValue,
                               1u);
                if (ctrlFlag == TRUE)
                {
                    DBG_PRINT_ERR("TVP5147M1: Error from tvp5147SetControl\n");
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
            DBG_PRINT_ERR("TVP5147M1: Param Type not supported\n");
            ctrlStatus = IOM_ENOTIMPL;
        }
    }
    VPIF_DEBUG("Exited ---> tvp5147SetControl\n");
    return ctrlStatus;
}


/**
 *  \brief  Detect if an tvp5147 is present, and if so which revision.
 *          A device is considered to be detected if the chip ID (LSB and MSB)
 *          registers match the expected values. Any value of the rom version
 *          register is accepted.
 *
 *  \param  chan   [IN]  Handle of the Channel Object
 *
 *  \return None
 *
 */
static Void tvp5147Detect(Tvp5147_DecObj *chan)
{
    Uint8 chipIdMsb, chipIdLsb, romVer;
    Bool dFlag;

    assert(NULL != chan);
    dFlag = codecReadReg(chan->edcCtrlBusType,
                chan->edcCtrlDeviceAddr,
                TVP5147_CHIP_ID_MSB,
                &chipIdMsb,
                1u);

    dFlag |= codecReadReg(chan->edcCtrlBusType,
                 chan->edcCtrlDeviceAddr,
                 TVP5147_CHIP_ID_LSB,
                 &chipIdLsb,
                 1u);

    dFlag |= codecReadReg(chan->edcCtrlBusType,
                 chan->edcCtrlDeviceAddr,
                 TVP5147_ROM_VERSION,
                 &romVer,
                 1u);
    if (TRUE == dFlag)
    {
        DBG_PRINT_ERR("TVP5147M1: Error from tvp5147PowerDown\n");
    }

    VPIF_INFO3("TVP5147M1: Chip id detected MSB: 0x%x LSB: 0x%x rom version: "
        "0x%x\n", chipIdMsb, chipIdLsb, romVer);
    if ((chipIdMsb != TVP5147_CHIP_ID_MSB_VALUE) ||
        (chipIdLsb != TVP5147_CHIP_ID_LSB_VALUE))
    {
        /* Didn't read the values as expected, so this must not be an TVP5147.*/
        DBG_PRINT_ERR2("TVP5147M1: Chip id mismatch MSB:0x%x lsb:0x%x\n",
            chipIdMsb, chipIdLsb);
    }
}

/**
 *  \brief  Power Up or Power Down TVP5147. Sets devices power state to
 *          requested state, if possible.
 *
 *  \param  chan             [IN] TVP5147 channel parameters
 *  \param  powerDownEnable  [IN] Flag to power-up or power-down TVP5147
 *
 *  \return IOM_COMPLETED on successful operation
 *          IOM_EBADIO if error occurred
 */
static Int32 tvp5147PowerDown(Tvp5147_DecObj *chan, Bool powerDownEnable)
{
    Uint8 pwrDnAddrI2c = 0;
    Uint32 pwrDnNumBytes = 1u;
    Uint8 enableDecoder;
    Bool pwrDnFlag;
    Int32 retVal = IOM_COMPLETED;

    VPIF_DEBUG("Entered ---> tvp5147PowerDown\n");
    VPIF_DEBUG2("Tvp5147_DecObj = 0x%0.8X, powerDownEnable = %d \n",
               chan,
               powerDownEnable);

    assert(NULL != chan);
    pwrDnAddrI2c = chan->edcCtrlDeviceAddr;

    /* Check for Power Up */
    if (FALSE == powerDownEnable)
    {
        /* Power Up Sequence */
        /* Put decoder in power down, then normal operating mode to
         reset device.*/
        enableDecoder = TVP5147_OPERATION_MODE_RESET;
        /* Reset TVP5147 */
        pwrDnFlag = codecWriteReg(chan->edcCtrlBusType,
                        pwrDnAddrI2c,
                        TVP5147_OPERATION_MODE,
                        &enableDecoder,
                        pwrDnNumBytes);
        tvp5147WaitMsec(100u);
        enableDecoder = TVP5147_OPERATION_MODE_DEFAULT;
        /*Put _tvp5147 in normal power mode */
        pwrDnFlag |= codecWriteReg(chan->edcCtrlBusType,
                         pwrDnAddrI2c,
                         TVP5147_OPERATION_MODE,
                         &enableDecoder,
                         pwrDnNumBytes);
        tvp5147WaitMsec(100u);
        /* Detect the sensor is not already detected */
        tvp5147Detect(chan);
    }
    else
    {
        /* Power Down Sequence */
        enableDecoder = TVP5147_OPERATION_MODE_RESET;
        /* Reset TVP5147 */
        pwrDnFlag = codecWriteReg(chan->edcCtrlBusType,
                        pwrDnAddrI2c,
                        TVP5147_OPERATION_MODE,
                        &enableDecoder,
                        pwrDnNumBytes);
        /* Disable mux for TVP5147 decoder data path, if any */
    }
    if (TRUE == pwrDnFlag)
    {
        DBG_PRINT_ERR("TVP5147M1: Error from tvp5147PowerDown\n");
        retVal = IOM_EBADIO;
    }
    else
    {
        retVal = IOM_COMPLETED;
    }
    VPIF_DEBUG("Exited ---> tvp5147PowerDown\n");
    return retVal;
}



/**
 *  \brief  Reset TVP5147
 *
 *  \param  chan  [IN]  TVP5147 channel parameters
 *
 *  \return IOM_COMPLETED on successful operation
 *          IOM_EBADIO if error occurred
 */
static Int32 tvp5147Reset(Tvp5147_DecObj *chan)
{
    /* Reset 5147 */
    return tvp5147PowerDown(chan, FALSE);
}

/* ========================================================================== */
/*                     TVP5147 GLOBAL FUNCTION DEFINITIONS                    */
/* ========================================================================== */
/**
 *  \brief  TVP5147 Close
 *
 *  \param  edcHandle [IN]  External device channel handle
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 */
static Int32 TVP5147_close(EDC_Handle edcHandle)
{
    Int32 retVal = IOM_COMPLETED;
    Tvp5147_DecObj *chan;

    VPIF_DEBUG("Entered ---> TVP5147_close\n");
    VPIF_DEBUG1("EDC_Handle = 0x%0.8X\n", edcHandle);
    assert(NULL != edcHandle);
    chan = (Tvp5147_DecObj *)edcHandle;

    /* First Check if the Handle/params are correct */
    if (0 == chan->decNum)
    {
        assert(TVP5147_0_IIC_ADDR == chan->edcCtrlDeviceAddr);
    }
    else if (1 == chan->decNum)
    {
        assert(TVP5147_1_IIC_ADDR == chan->edcCtrlDeviceAddr);
    }
    else
    {
        assert(FALSE);
    }
    assert(Edc_ControlBusType_UNKNOWN != chan->edcCtrlBusType);
    assert(NULL != edcHandle);
    /* Put _TVP5147 in power down mode */
    retVal = TVP5147_ctrl(chan,
                 (Uns)(Edc_IOCTL_CMD_MAX + Tvp5147_IOCTL_POWERDOWN),
                 NULL);
    if (IOM_COMPLETED == retVal)
    {
        /*close I2C channel*/
        retVal = deInitCodecInterface(chan->edcCtrlBusType);
    }
    memset(&decObj[chan->decNum], 0x0, sizeof(Tvp5147_DecObj));
    chan->edcCtrlBusType = Edc_ControlBusType_UNKNOWN;
    chan->decNum = -1;

    VPIF_DEBUG("Exited ---> TVP5147_close\n");
    return retVal;
}



/**
 *  \brief  TVP5147 Ioctls
 *
 *  \param  edcHandle [IN]  External device channel handle
 *  \param  cmd       [IN]  IOCTL to be performed (mostly enum)
 *  \param  arg       [IN]  Argument as required by Ioctl
 *
 *  \return IOM_COMPLETED, if success.
 *          Error code, in case of error.
 */
static Int32 TVP5147_ctrl(EDC_Handle edcHandle, Uns cmd, Ptr arg)
{
    Int32 retVal = IOM_COMPLETED;
    Tvp5147_DecObj *tvpCtrlChan;
    Edc_RegData *regRw = NULL;
    Bool tvpCtrlFlag;
    Uns tvpDefCmd;

    VPIF_DEBUG("Entered ---> TVP5147_ctrl\n");
    VPIF_DEBUG2("EDC_Handle = 0x%0.8X, Arg = 0x%0.8X\n", edcHandle, arg);

    assert(NULL != edcHandle);
    tvpCtrlChan = (Tvp5147_DecObj *)edcHandle;
    if (Edc_IOCTL_CONFIG == cmd)
    {
        VPIF_DEBUG("TVP5147M1: Command: Edc_IOCTL_CONFIG\n");
        assert(NULL != arg);
        retVal = tvp5147Config(tvpCtrlChan, (Tvp5147_ConfParams *)arg);
    }
    else if (Edc_IOCTL_RESET == cmd)
    {
        VPIF_DEBUG("TVP5147M1: Command: Edc_IOCTL_RESET\n");
        retVal = tvp5147Reset(tvpCtrlChan);
    }
    else if (Edc_IOCTL_SET_REG == cmd)
    {
        VPIF_DEBUG("TVP5147M1: Command: Edc_IOCTL_SET_REG\n");
        assert(NULL != arg);
        regRw = (Edc_RegData *)arg;
        assert(NULL != regRw);
        tvpCtrlFlag = codecWriteReg(tvpCtrlChan->edcCtrlBusType,
                          tvpCtrlChan->edcCtrlDeviceAddr,
                          (Uint8)regRw->startReg,
                          regRw->value,
                          regRw->noRegToRW);
        if (TRUE == tvpCtrlFlag)
        {
            DBG_PRINT_ERR("TVP5147M1: Error from TVP5147_ctrl\n");
            retVal = IOM_EBADIO;
        }
        else
        {
            retVal = IOM_COMPLETED;
        }
    }
    else if (Edc_IOCTL_GET_REG == cmd)
    {
        VPIF_DEBUG("TVP5147M1: Command: Edc_IOCTL_GET_REG\n");
        assert(NULL != arg);
        regRw = (Edc_RegData *)arg;
        assert(NULL != regRw);
        tvpCtrlFlag = codecReadReg(tvpCtrlChan->edcCtrlBusType,
                          tvpCtrlChan->edcCtrlDeviceAddr,
                          (Uint8)regRw->startReg,
                          regRw->value,
                          regRw->noRegToRW);
        if (TRUE == tvpCtrlFlag)
        {
            DBG_PRINT_ERR("TVP5147M1: Error from TVP5147_ctrl\n");
            retVal = IOM_EBADIO;
        }
        else
        {
            retVal = IOM_COMPLETED;
        }
    }
    else if (Edc_IOCTL_READ_SLICE_VBI_DATA == cmd)
    {
        VPIF_DEBUG("TVP5147M1: Command: Edc_IOCTL_READ_SLICE_VBI_DATA\n");
        assert(NULL != arg);
        retVal = tvp5147ReadVbiData(tvpCtrlChan, (FVID_SliceFrame *)arg);
    }
    else
    {
        tvpDefCmd = (Uns)(cmd - Edc_IOCTL_CMD_MAX);
        VPIF_DEBUG("TVP5147M1: Command: default\n");
        if (Tvp5147_IOCTL_SET_CONTROL == tvpDefCmd)
        {
            VPIF_DEBUG("TVP5147M1: Command: Tvp5147_IOCTL_SET_CONTROL\n");
            assert(NULL != arg);
            retVal = tvp5147SetControl(tvpCtrlChan, (Tvp5147_Control *)arg);
        }
        else if (Tvp5147_IOCTL_POWERDOWN == tvpDefCmd)
        {
            VPIF_DEBUG("TVP5147M1: Command: Tvp5147_IOCTL_POWERDOWN\n");
            retVal = tvp5147PowerDown(tvpCtrlChan, TRUE);
        }
        else if (Tvp5147_IOCTL_POWERUP == tvpDefCmd)
        {
            VPIF_DEBUG("TVP5147M1: Command: Tvp5147_IOCTL_POWERUP\n");
            retVal = tvp5147PowerDown(tvpCtrlChan, FALSE);
        }
        else if (Tvp5147_IOCTL_SET_SLICE_VBI_SERVICE == tvpDefCmd)
        {
            VPIF_DEBUG("TVP5147M1: Cmd: Tvp5147_IOCTL_SET_SLICE_VBI_SERVICE\n");
            assert(NULL != arg);
            retVal = tvp5147SetSliceVbiService(tvpCtrlChan, (Uint32 *)arg);
        }
        else
        {
            DBG_PRINT_ERR1("TVP5147M1: Command: default(0x%0.8X)\n", cmd);
            retVal = IOM_EBADARGS;
        }
    }

    VPIF_DEBUG("Exited ---> TVP5147_ctrl\n");
    return retVal;
}



/**
 *  \brief  TVP5147 device open
 *
 *  \param  devName [IN]  External device name
 *  \param  optArg  [IN]  Optional argument (not used here)
 *
 *  \return  handle  EDC channel handle when success
 *                   NULL when unsuccessful
 */
static EDC_Handle TVP5147_open(String devName, Ptr optArg)
{
    Tvp5147_DecObj *handle = NULL;
    Int8 tvp5147Str0[20] = "TVP5147_0";
    Int8 tvp5147Str1[20] = "TVP5147_1";
    Int8 i2cDevice[20] = "\0";
    Int8 decName[20] = "\0";
    Int32 chanNum = 0;

    static Tvp5147_ConfParams TVP5147_default0 =
    {
        Tvp5147_AnalogFormat_SVIDEO,
        /**< only SVIDEO input is connected to the TVP5147 instance 0*/
        Tvp5147_Std_AUTO,
        /**< Auto standard detection is default */
        Tvp5147_OutputFormat_YCBCR422,
        FVID_SLICE_VBI_SERVICES_NONE
        /**< slice vbi service default : NONE   */
    };

    static Tvp5147_ConfParams TVP5147_default1 =
    {
        Tvp5147_AnalogFormat_COMPOSITE,
        /**< only Composite input is connected to the TVP5147 instance 1 */
        Tvp5147_Std_AUTO,
        /**< Auto standard detection is default */
        Tvp5147_OutputFormat_YCBCR422,
        FVID_SLICE_VBI_SERVICES_NONE
        /**< slice vbi service default : NONE   */
    };

    VPIF_DEBUG("Entered ---> TVP5147_open\n");
    VPIF_DEBUG2("devName = %s, Arg = 0x%0.8X\n", devName, optArg);

    assert(NULL != devName);
    optArg = optArg; /* to remove compilation warning */
    /* Check if the address is correct */
    if (devName[0] != '/')
    {
        handle = (Tvp5147_DecObj *)NULL;
    }
    else
    {
        /*only copy 5 chars into positions 0-4 for /I2C0*/
        strncpy(i2cDevice, devName, 5u);
        /* position 5 gets the terminator */
        i2cDevice[5] = '\0';

        devName += 5;
        /* Check if the address is correct */
        if ('/' == devName[0])
        {
            devName++;
            /*only copy 9 chars into positions 0-8 for devName*/
            strncpy(decName, devName, 9u);
            /* position 9 gets the terminator */
            decName[9] = '\0';

            devName += 9;
            /* Compare devName with "TVP5147" */
            if (0 == strcmp(decName, tvp5147Str0))
            {
                chanNum = 0;
                strncpy(decObj[chanNum].codecName, tvp5147Str0, 20u);
            }
            if (0 == strcmp(decName, tvp5147Str1))
            {
                chanNum = 1;
                strncpy(decObj[chanNum].codecName, tvp5147Str1, 20u);
            }
            decObj[chanNum].decNum = chanNum;
            /* check if already opened  */
            if (('/' == devName[0]) && (0 == decObj[chanNum].edcCtrlDeviceAddr))
            {
                devName++;
                decObj[chanNum].edcCtrlDeviceAddr = (Uint8)strtol(devName + 2,
                                                               NULL,
                                                               16);
                if (((chanNum == 0) &&
                    (TVP5147_0_IIC_ADDR == decObj[chanNum].edcCtrlDeviceAddr)) ||
                    ((chanNum == 1) &&
                    (TVP5147_1_IIC_ADDR == decObj[chanNum].edcCtrlDeviceAddr)))
                {
                    strncpy(decObj[chanNum].edcCtrlBusName, i2cDevice, 20u);
                    if (0 == strcmp(i2cDevice, "/I2C0"))
                    {
                        decObj[chanNum].edcCtrlBusType = Edc_ControlBusType_I2C;
                    }
                    else
                    {
                        decObj[chanNum].edcCtrlBusType = \
                            Edc_ControlBusType_UNKNOWN;
                    }
                    /* assigning channel object structure */
                    handle = &decObj[chanNum];
                }
                else
                {
                    /* wrong i2c address */
                    handle = (Tvp5147_DecObj *)NULL;
                    decObj[chanNum].codecName[0] = '\0';
                }
            }
            else
            {
                /* no i2c address in the string or device already opened*/
                handle = (Tvp5147_DecObj *)NULL;
                decObj[chanNum].codecName[0] = '\0';
            }
        }
        else
        {
            /* No decoder name*/
            handle = (Tvp5147_DecObj *)NULL;
        }
    }
    if (handle != NULL)
    {
        /* open I2C channel */
        if (IOM_COMPLETED != initCodecInterface(
                                 (String)decObj[chanNum].edcCtrlBusName))
        {
            handle = (Tvp5147_DecObj *)NULL;
        }

        /* power on decoder */
        if (handle != NULL)
        {

            if (IOM_COMPLETED != TVP5147_ctrl(handle,
                                     (Uns)Tvp5147_IOCTL_POWERUP,
                                     NULL))
            {
                deInitCodecInterface(decObj[chanNum].edcCtrlBusType);
                handle = (Tvp5147_DecObj *)NULL;
            }
            else
            {
                /* Configure TVP5147 to default mode */
                if (chanNum == 0)
                {
                    if (IOM_COMPLETED != TVP5147_ctrl(handle,
                                             (Uns)Edc_IOCTL_CONFIG,
                                             (Ptr)&TVP5147_default0))
                    {
                        handle = (Tvp5147_DecObj *)NULL;
                    }
                }
                if (chanNum == 1)
                {
                    if (IOM_COMPLETED != TVP5147_ctrl(handle,
                                             (Uns)Edc_IOCTL_CONFIG,
                                             (Ptr)&TVP5147_default1))
                    {
                        handle = (Tvp5147_DecObj *)NULL;
                    }
                }
            }
        }
        else
        {
            DBG_PRINT_ERR("ERR ---> edcDecoder_open\n");
        }
    }

    VPIF_DEBUG("Exited ---> TVP5147_open\n");
    return (EDC_Handle)handle;
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
