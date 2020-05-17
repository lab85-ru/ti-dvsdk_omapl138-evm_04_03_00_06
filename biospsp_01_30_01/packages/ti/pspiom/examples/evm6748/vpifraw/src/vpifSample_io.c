/*
 * vpifSample_io.c
 *
 * This file contains the test / demo code to demonstrate the VPIF driver
 * functionality on DSP/BIOS 5.
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
 *  \file   vpifSample_io.c
 *
 *  \brief  This file contains the test / demo code to demonstrate the
 *          VPIF driver implemented for DSP/BIOS
 *
 *          This application tests the RAW mode capture. This application will
 *          configure an MT9T001 image sensor to output data on the VPIF data
 *          bus and config the VPIF for raw capture receive mode.
 *          This application takes the RAW data in 8 bit mode and not 10 bit
 *          mode. This application converts this RAW 8bit Bayer data to YUV
 *          4:2:2 data and displays it using VPIF. The type of processing uses
 *          only 1/4 of the captured resolution, i.e., 1/2 the number of
 *          pixels / line and 1/2 the lines.
 *
 */

/* ========================================================================== */
/*                          INCLUDE FILES                                     */
/* ========================================================================== */
#include <std.h>
#include <string.h>
#include <assert.h>
#include <log.h>
#include <tsk.h>
#include <bcache.h>

#include "ti/pspiom/vpif/Fvid.h"
#include "ti/pspiom/vpif/Vpif.h"
#include "ti/pspiom/vpif/Edc.h"
#include "ti/pspiom/platforms/evm6748/Vpif_evmInit.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/Mt9t001.h"
#include "ti/pspiom/platforms/evm6748/vpifedc/Adv7343.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/* Different video modes of operation */
#define MODE_NTSC                   (1)/* 720 x 480 aka NTSC */
#define MODE_PAL                    (2)/* 720 x 576 aka PAL */

/* Different connector types */
#define CONN_SVIDEO                 (1)
#define CONN_COMPOSITE              (2)

#define NUM_FRAME_BUFFERS           (3u)

/* Please make sure the Device is in the following DISPLAY_CONNECTOR mode and
 the cable is connected*/
#define DISPLAY_CONNECTOR           (CONN_COMPOSITE)
#define MAXLOOPCOUNT                (500u)

#define SELECT_TEST_PATTERN         0
#define SET_GLOBAL_GAIN             0
#define CONFIG_MT9T001              0

/* ========================================================================== */
/*                          EXTERNAL REFERENCES                               */
/* ========================================================================== */
extern far LOG_Obj    trace;

/* Heap ID defined in the BIOS configuration file.
   This memory segment is used to allocate video frame buffers.               */
extern Int32 EXTERNALHEAP;

/* Structure to store each driver channel information                         */
typedef struct ChannelInfo_t
{
    FVID_Handle chanHandle;     /* Channel handle                             */
    FVID_Frame *frame;          /* Current FVID frame buffer pointer          */
}ChannelInfo;

/* Function to convert 8 bit bayer pattern data to YUV422                     */
extern Void bayer8Bit2Yuv422(Uint8 *srcBuf,
                             Int32 numlinesCap,
                             Int32 lineszCap,
                             Int32 numlinesDis,
                             Int32 lineszDis,
                             Uint8 *destY,
                             Uint8 *destC);

/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */
static Void tskHeartBit(Void);

static Void runVpifSample(Void);

/* ========================================================================== */
/*                       GLOBAL FUNCTION PROTOTYPES                           */
/* ========================================================================== */
Void vpifSampleTask(Void);

/* ========================================================================== */
/*                        LOCAL FUNCTION DEFINITIONS                          */
/* ========================================================================== */
/**
 *  \fn    Void tskHeartBit(Void)
 *
 *  \brief Function to print a debug message to screen.
 *
 *  \param  None
 *
 *  \return None
 */
static Void tskHeartBit(Void)
{
    static Uint32 count = 0;

    while (count < 0xFFFFFFFF)
    {
        TSK_sleep(1000u);    /* Add large delay */
        LOG_printf(&trace, " VPIF_SAMPLE: !!! PSP HrtBt %d", count);
        count++;
    }
}

/* ========================================================================== */
/*                        GLOBAL FUNCTION DEFINITIONS                         */
/* ========================================================================== */
/**
 *  \fn    Void runVpifSample(Void)
 *
 *  \brief Function which creates the capture and display channel for the VPIF
 *         device. This function opens one channel for the raw capture with
 *         sensor connected in desired resolution. The display channel is opened
 *         with BT standard. The capture channel captures raw frame from the
 *         sensor. The frame capture is in Bayer format. This is then converted
 *         into RGB888 and then to YUV422 format. This is then given to the
 *         display device for outputting on output source.
 *
 *  \return None
 */
Void runVpifSample(Void)
{
    Vpif_CapChanParams  vRawParamsChan;
    Vpif_DisChanParams  vDisParamsChan;
    /* Structure containing display and raw channel information */
    ChannelInfo         rawChInfo;
    ChannelInfo         disChInfo;
    Int32               status = IOM_COMPLETED;
    /* Setup Capture Channel 0 -> Raw
       Use this raw driver name string for proper driver creation */
    Int8                *vpifRawStrings = "/VPIF0/0/I2C0/MT9T001/0x5D";
    /* Use this Display driver name string for proper driver creation */
    Int8                *vpifDisStrings = "/VPIF0/2/I2C0/ADV7343/0x2A";
    Int32               bufCount, counter;
    Vpif_StdInfo        rawParams, disParams;

    assert(NULL != vpifRawStrings);
    assert(NULL != vpifDisStrings);

    /* Configure VPIF for the both capture channel as it is used for RAW
    capture and channel 2 for display*/

    /* Configure VPIF Input Video Clocks */
    configureVpif0(EvmInit_VpifChannel_BOTHCAPCH, FALSE);
    /* Configure VPIF Output Video Clocks - 27 MHz */
    configureVpif0(EvmInit_VpifChannel_2, FALSE);

    /* Create and configure capture drivers for RAW operation */
    vRawParamsChan.capEdcTbl = &MT9T001_Fxns;
    vRawParamsChan.capChannelIoMode = Vpif_IoMode_RAW_CAP;
    vRawParamsChan.capFbParams.frmBufAlignment = 128u;
    vRawParamsChan.capFbParams.frmBufSegId = (Int32)EXTERNALHEAP;
    vRawParamsChan.capStdMode = Vpif_VideoMode_RAW_480P;
    /* Note that here two bits of data is skipped from sensor */
    vRawParamsChan.capDataSize = Vpif_RawCaptureDataWidth_8BITS;
    vRawParamsChan.capFieldPol = Vpif_RawCapturePinPol_SAME;
    vRawParamsChan.capVPixPol = Vpif_RawCapturePinPol_SAME;
    vRawParamsChan.capHPixPol = Vpif_RawCapturePinPol_SAME;
    vRawParamsChan.capStorageMode = Vpif_SdramStorage_FRAME;
    vRawParamsChan.capVideoParams = NULL;
    vRawParamsChan.capVbiService = Vpif_VbiServiceType_NONE;
    rawChInfo.chanHandle = FVID_create(vpifRawStrings,
                               IOM_INPUT,
                               &status,
                               (Ptr)&vRawParamsChan,
                               NULL);
    if ((IOM_COMPLETED != status) || (NULL == rawChInfo.chanHandle))
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to create raw"
            " capture channel");
    }

    /* Create and configure display driver */
    vDisParamsChan.dispEdcTbl = &ADV7343_Fxns;
    vDisParamsChan.dispChannelIoMode = Vpif_IoMode_DIS;
    vDisParamsChan.dispFbParams.frmBufAlignment = 128u;
    vDisParamsChan.dispFbParams.frmBufSegId = (Int32)EXTERNALHEAP;
    vDisParamsChan.dispStdMode = Vpif_VideoMode_NTSC;
    vDisParamsChan.dispStorageMode = Vpif_SdramStorage_FRAME;
    vDisParamsChan.dispVideoParams = NULL;
    vDisParamsChan.dispVbiService = Vpif_VbiServiceType_NONE;
    vDisParamsChan.dispHVbiParams = NULL;
    vDisParamsChan.dispVVbiParams = NULL;
    if (IOM_COMPLETED == status)
    {
        disChInfo.chanHandle = FVID_create(vpifDisStrings,
                                   IOM_OUTPUT,
                                   &status,
                                   (Ptr)&vDisParamsChan,
                                   NULL);

        if ((IOM_COMPLETED != status) || (NULL == disChInfo.chanHandle))
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to create"
                " display channel");
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to open Capture channel");
    }

    /*
     *  It is necessary for applications to know about the channel
     *  characteristics that are set by the driver for proper handling of
     *  channel. These are: width and height for RAW mode
     */
    if (IOM_COMPLETED == status)
    {
        status = FVID_control(rawChInfo.chanHandle,
                     Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO,
                     &rawParams);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to get raw capture"
                " channel info");
        }
        status = FVID_control(disChInfo.chanHandle,
                     Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO,
                     &disParams);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to get display"
                " channel info");
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to open Display channel");
    }

    if ((rawParams.activePixels == disParams.activePixels) && /*width or bytesperline*/
        (rawParams.activeLines == disParams.activeLines))  /*height*/
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Parameters same - good to go");
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Parameters not same."
            " Do take care of buffers");
    }

    /* To select test pattern from MT9T001 to VPIF */
    #if (SELECT_TEST_PATTERN)
    {
        Edc_RegData regval;
        Uint8 val[2];

        regval.startReg = 0x07u;
        regval.noRegToRW = 1u;
        val[0] = 0x00;/*MSB*/
        val[1] = 0x42;/*LSB => Use Test Data*/
        regval.value = &val;
        status = FVID_control(rawChInfo.chanHandle,
                     Vpif_IOCTL_CMD_MAX + Edc_IOCTL_SET_REG,
                     (Ptr)&regval);

        regval.startReg = 0x32u;
        regval.noRegToRW = 1u;
        /*
         * Test Data will be inserted for even columns, and the inverse will be
         * inserted for odd columns.
         * 554(value) AA55(pattern)
         * AA8(value) 55AA(pattern)
         * 000(value) FF00(pattern)
         * FFC(value) 00FF(pattern)
         */
        val[0] = 0x0F;/*MSB*/
        val[1] = 0xF0;/*LSB*/
        regval.value = &val;
        status = FVID_control(rawChInfo.chanHandle,
                     Vpif_IOCTL_CMD_MAX + Edc_IOCTL_SET_REG,
                     (Ptr)&regval);
    }
    #endif

    /* Allocate and Queue buffers for display & capture channel */
    if (IOM_COMPLETED == status)
    {
        for (bufCount = 0; bufCount < NUM_FRAME_BUFFERS; bufCount++)
        {
            /* Allocate Frame buffer for capture driver */
            status = FVID_allocBuffer(rawChInfo.chanHandle, &(rawChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to allocate"
                    " buffer for capture");
            }
            else
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: Raw: Alloc "
                    "frame->frame.rpFrm.buf = 0x%x",
                    rawChInfo.frame->frame.rpFrm.buf);
                /* Queue the frame buffers for capture */
                status = FVID_queue(rawChInfo.chanHandle, &(rawChInfo.frame));
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to Queue"
                        " capture buffer");
                }
            }

            if (IOM_COMPLETED == status)
            {
                /* Allocate Frame buffer for display driver */
                status = FVID_allocBuffer(disChInfo.chanHandle,
                             &(disChInfo.frame));
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to allocate"
                        " buffer for display");
                }
                else
                {
                    LOG_printf(&trace, " VPIF_RAW_SAMPLE: Dis: Alloc"
                        " frame->frame.spFrm.y1 = 0x%x",
                        disChInfo.frame->frame.spFrm.y1);
                    /* Queue the buffers for display */
                    status = FVID_queue(disChInfo.chanHandle,
                                 &(disChInfo.frame));
                    if (IOM_COMPLETED != status)
                    {
                        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to Queue"
                            " display buffer");
                    }
                }
            }
            if (IOM_COMPLETED != status)
            {
                break;
            }
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to get params "
            "Display channel");
    }

    /* Configure external video encoders (ADV7343)                            */
    #if !(DISPLAY_CONNECTOR == CONN_SVIDEO)
        if (IOM_COMPLETED == status)
        {
            /* Only need to configure if the connection is composite. Since
               svideo is the default mode of connection for EDC driver */
            {
                Adv7343_ConfParams vDisParamsEncoder =
                {
                    Adv7343_AnalogFormat_COMPOSITE,   /* AnalogFormat      */
                    Adv7343_Std_AUTO,                 /* Video std         */
                    Adv7343_InputFormat_YCBCR422,     /* InputFormat       */
                    FVID_SLICE_VBI_SERVICES_NONE     /* slice vbi service */
                };
                /* Configure ADV7343 */
                status = FVID_control(disChInfo.chanHandle,
                             Vpif_IOCTL_CMD_MAX + Edc_IOCTL_CONFIG,
                             (Ptr)&vDisParamsEncoder);
            }
        }
        else
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to allocate/queue"
                " the buffers");
        }
    #endif /* #if !(DISPLAY_CONNECTOR == CONN_SVIDEO) */
    /* Configure external sensor (MT9T001) to different resolution SVGA */
    #if (CONFIG_MT9T001)
        if (IOM_COMPLETED == status)
        {
            /* Only need to configure if the video std is 576P. Since 480P
               is the default mode of standard for EDC driver */
            {
                Mt9t001_ConfParams config;
                config.fmtParams = NULL;
                config.stdFormat = MT9T001_MODE_SVGA;
                /* Enable the different std on MT9T001 */
                status = FVID_control(rawChInfo.chanHandle,
                             Vpif_IOCTL_CMD_MAX + Edc_IOCTL_CONFIG,
                             (Ptr)&config);
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to config"
                        "MT9T001");
                }
            }
        }
        else
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to allocate/queue"
                "the buffers");
        }
    #endif /* #if (CONFIG_MT9T001) */

    #if (SET_GLOBAL_GAIN) /* Set global gain for MT9T001 */
        {
            Edc_RegData regval;
            Uint8 val[2];

            regval.startReg = 0x35u;
            regval.noRegToRW = 1u;
            val[0] = 0x00;/*MSB*/
            val[1] = 0x3F;/*LSB => Use analog gain */
            regval.value = &val;
            status = FVID_control(rawChInfo.chanHandle,
                         Vpif_IOCTL_CMD_MAX + Edc_IOCTL_SET_REG,
                         (Ptr)&regval);
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to set "
                    "global gain");
            }
        }
    #endif

    /*
     * Here display and capture channels are started for streaming. After
     * this capture device will start capturing frames into enqueued
     * buffers and display device will start displaying buffers from
     * the enqueued buffers
     */

    /* Start display and capture operations */
    if (IOM_COMPLETED == status)
    {
        status = FVID_control(disChInfo.chanHandle, Vpif_IOCTL_CMD_START, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to start display"
                " channel device");
        }
        status = FVID_control(rawChInfo.chanHandle, Vpif_IOCTL_CMD_START, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to start capture"
                " channel device");
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to configure enc/dec");
    }

    /* Request a frame buffer from display & capture driver */
    if (IOM_COMPLETED == status)
    {
        /* Capture buffer will return the latest captured buffer */
        status = FVID_dequeue(rawChInfo.chanHandle, &(rawChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to dequeue capture"
                " channel device");
        }
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: DQ Raw rpFrm.buf = 0x%x",
            rawChInfo.frame->frame.rpFrm.buf);

        /* Display buffer will return a free buffer */
        status = FVID_dequeue(disChInfo.chanHandle, &(disChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to dequeue display"
                " channel device");
        }
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: DQ Dis vpifFrm.y1 = 0x%x",
            disChInfo.frame->frame.spFrm.y1);

    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to start display/"
            "capture driver");
    }

    /***************************************************/
    /* copy data from capture buffer to display buffer */
    /***************************************************/
    /*
     * Convert Bayer pattern source data to RGB888 and then
     * convert RGB data to YUV4:2:2 as needed by the display device for VPIF
     */
    bayer8Bit2Yuv422((Uint8 *)rawChInfo.frame->frame.rpFrm.buf,
        rawParams.activeLines,
        rawParams.activePixels,
        disParams.activeLines,
        disParams.activePixels,
        disChInfo.frame->frame.spFrm.y1,
        disChInfo.frame->frame.spFrm.c1);

    /*
     * One buffer is dequeued from display and capture channels.
     * Capture buffer will be exchanged with display buffer.
     * All two buffers are put back to respective channels.
     * This sequence is repeated in loop.
     * After completion of this loop, channels are stopped.
     */

    /*
     * This is a running loop where the buffer is
     * DEQUEUED  <-----|
     * PROCESSED    |
     * & QUEUED -------|
     */
    if (IOM_COMPLETED == status)
    {
        counter = 0;
        while (counter < MAXLOOPCOUNT)
        {
            /* Invalidate the buffer before giving to capture driver */
            BCACHE_inv(rawChInfo.frame->frame.rpFrm.buf,
                (rawParams.activeLines * rawParams.activePixels),/* Since 8 bit*/
                TRUE);
            /* Give the old capture frame buffer back to driver and get the
               recently captured frame buffer */
            status = FVID_exchange(rawChInfo.chanHandle, &(rawChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: Error in exchanging"
                    " capture buffer");
                break;
            }
            else
            {
                /* Flush and invalidate the processed buffer so that the DMA
                   reads the processed data */
                BCACHE_wbInv(disChInfo.frame->frame.spFrm.y1,
                    (disParams.activeLines * disParams.activePixels * 2),
                    TRUE);

                /* Note: We are directly exchanging the capture buffer to
                   display channel instead of copying to display buffer.
                   Give the captured frame buffer to display driver and get a
                   free frame buffer for next capture */
                status = FVID_exchange(disChInfo.chanHandle,
                    &(disChInfo.frame));
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: EX vpifFrm.y1 = 0x%x",
                    disChInfo.frame->frame.spFrm.y1);
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, " VPIF_RAW_SAMPLE: Error in exchanging"
                        " display buffer");
                }
                /***************************************************/
                /* copy data from capture buffer to display buffer */
                /***************************************************/
                /*
                 * Convert Bayer pattern source data to RGB888 and then convert
                 * RGB data to YUV4:2:2 as needed by the display device for VPIF
                 */
                bayer8Bit2Yuv422((Uint8 *)rawChInfo.frame->frame.rpFrm.buf,
                    rawParams.activeLines,
                    rawParams.activePixels,
                    disParams.activeLines,
                    disParams.activePixels,
                    disChInfo.frame->frame.spFrm.y1,
                    disChInfo.frame->frame.spFrm.c1);
            }
            if (IOM_COMPLETED != status)
            {
                break;
            }
            else
            {
                counter++;
            }
        }
    }

    /* Stop capture and display operation */
    if (IOM_COMPLETED == status)
    {
        /* Stop display operation */
        status = FVID_control(disChInfo.chanHandle, Vpif_IOCTL_CMD_STOP, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Error in stopping display"
                " operation");
        }
        /* Free the buffer owned by application */
        status = FVID_freeBuffer(disChInfo.chanHandle, &(disChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: IOM_COMPLETED != status for"
                " free buff");
        }
        /* Dequeue buffers from driver and free them */
        for (bufCount = 0; bufCount < (NUM_FRAME_BUFFERS - 1u); bufCount++)
        {
            status = FVID_dequeue(disChInfo.chanHandle, &(disChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: IOM_COMPLETED != status"
                    " for DQ");
            }
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: DQ vpifFrm.y1 = 0x%x",
                disChInfo.frame->frame.spFrm.y1);
            status = FVID_freeBuffer(disChInfo.chanHandle, &(disChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, " VPIF_RAW_SAMPLE: IOM_COMPLETED != status"
                    " for free buff");
            }
        }
        status = FVID_control(rawChInfo.chanHandle, Vpif_IOCTL_CMD_STOP, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Error in stopping capture"
                " operation");
        }
        status = FVID_freeBuffer(rawChInfo.chanHandle, &(rawChInfo.frame));
        /* Dequeue buffers from driver and free them */
        for (bufCount = 0; bufCount < (NUM_FRAME_BUFFERS - 1u); bufCount++)
        {
            status = FVID_dequeue(rawChInfo.chanHandle, &(rawChInfo.frame));
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: DQ vpifFrm.y1 = 0x%x",
                rawChInfo.frame->frame.spFrm.y1);
            status = FVID_freeBuffer(rawChInfo.chanHandle, &(rawChInfo.frame));
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to alloc or exchange"
            " buffers");
    }


    /* Delete capture and display channel */
    if (IOM_COMPLETED == status)
    {
        /* Delete display drivers */
        status = FVID_delete(disChInfo.chanHandle);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to delete display"
                " channel");
        }
        status = FVID_delete(rawChInfo.chanHandle);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to delete capture"
                " channel");
        }
        if (IOM_COMPLETED == status)
        {
            LOG_printf(&trace, " VPIF_RAW_SAMPLE: Application closed"
                " successfully!!");
        }
    }
    else
    {
        LOG_printf(&trace, " VPIF_RAW_SAMPLE: Failed to stop display/"
            "capture channel");
    }

}


/**
 *  \fn    Void vpifSampleTask(Void)
 *
 *  \brief Entry point for the "vpifSampleTask" task.
 *
 *   This function is the main entry point for the VPIF sample application's
 *   task. It calls the VPIF sample applcation to demostrate the usage of the
 *   VPIF driver
 *
 *  \param  None
 *
 *  \return None
 */
Void vpifSampleTask(Void)
{
    LOG_printf(&trace, "VPIF_SAMPLE :Starting Sample Application RAW LOOPBACK mode");
    runVpifSample();
    LOG_printf(&trace, "VPIF_SAMPLE :Sample Application Ends RAW LOOPBACK mode");

    /* Start the Heart Beat Print */
   tskHeartBit();
}


/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
