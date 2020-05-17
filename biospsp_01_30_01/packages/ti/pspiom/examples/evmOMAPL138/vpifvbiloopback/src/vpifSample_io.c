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
 *          This application tests the loop-back of video data with VBI enabled.
 *          The application capture the BT.656 video data and the VBI data and
 *          displays it using VPIF.
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
#include "ti/pspiom/platforms/evmOMAPL138/Vpif_evmInit.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/Tvp5147.h"
#include "ti/pspiom/platforms/evmOMAPL138/vpifedc/Adv7343.h"

/* ========================================================================== */
/*                          MACRO DEFINITIONS                                 */
/* ========================================================================== */
/* Different video modes of operation */
#define MODE_NTSC                   (1)/* 720 x 480 aka NTSC */
#define MODE_PAL                    (2)/* 720 x 576 aka PAL */

/* Different connector types */
#define CONN_SVIDEO                 (1)
#define CONN_COMPOSITE              (2)

/* Different storage types - valid only for interlaced mode of operation*/
#define STORAGE_FIELD               (1)
#define STORAGE_FRAME               (2)

#define NUM_FRAME_BUFFERS           (3u)
/* Please make sure the Device(DVD) is in the following VIDEO_MODE mode */
#define VIDEO_MODE                  (MODE_NTSC)

#define VIDEO_STORAGE               (STORAGE_FRAME)
#define MAXLOOPCOUNT                (500u)

/* ========================================================================== */
/*                          EXTERNAL REFERENCES                               */
/* ========================================================================== */
extern far LOG_Obj    trace;

/* Structure to store each driver channel information                         */
typedef struct ChannelInfo_t
{
    FVID_Handle chanHandle;     /* Channel handle                             */
    FVID_Frame *frame;          /* Current FVID frame buffer pointer          */
}ChannelInfo;

/* ========================================================================== */
/*                          GLOBAL REFERENCES                                 */
/* ========================================================================== */

/* ========================================================================== */
/*                       LOCAL FUNCTION PROTOTYPES                            */
/* ========================================================================== */
static Void runVpifSample(Void);

static Void tskHeartBit(Void);

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
 *         device. This function opens one channel for the capture in desired
 *         input mode and video standard. The display channel is also opened
 *         similarly. The capture channel captures frame form the input source
 *         and gives it to the display device for outputting on output source.
 *
 *  \return None
 */
static Void runVpifSample(Void)
{
    Vpif_CapChanParams  vCapParamsChan;
    Vpif_DisChanParams  vDisParamsChan;
    /* Structure containing display and capture channel information */
    ChannelInfo         capChInfo;
    ChannelInfo         disChInfo;
    Int32               status = IOM_COMPLETED;
    /* Setup Capture Channel 0 -> Composite
       Use this capture driver name string for proper driver creation */
    Int8                *vpifCapStrings = "/VPIF0/0/I2C0/TVP5147_1/0x5D";
    /*  Use this Display driver name string for proper driver creation */
    Int8                *vpifDisStrings = "/VPIF0/2/I2C0/ADV7343/0x2A";
    Int32               bufCount, counter;
    Vpif_StdInfo        capParams, disParams;
    Uint32              dispheight, dispwidth;
    Uint32              sizeimage;
    Uint32              capSlice = FVID_SLICE_VBI_SERVICES_CC_NTSC;

    assert(NULL != vpifCapStrings);
    assert(NULL != vpifDisStrings);

    /*Channel 0 -> Composite only and Channel 1 -> SVideo only*/
    /* Configure VPIF Input Video Clocks */
    configureVpif0(EvmInit_VpifChannel_0, FALSE);

    /*Channel 2 -> SVideo/Composite*/
    /* Configure VPIF Output Video Clocks - 27 MHz */
    configureVpif0(EvmInit_VpifChannel_2, FALSE);

    /* Create and configure capture drivers */
    vCapParamsChan.capEdcTbl = &TVP5147_Fxns;
    vCapParamsChan.capChannelIoMode = Vpif_IoMode_CAP;
    vCapParamsChan.capFbParams.frmBufAlignment = 128u;
    vCapParamsChan.capFbParams.frmBufSegId = 0;/* Create from system heap*/
    #if !(VIDEO_MODE == MODE_NTSC)
        vCapParamsChan.capStdMode = Vpif_VideoMode_PAL;
    #else
        vCapParamsChan.capStdMode = Vpif_VideoMode_NTSC;
    #endif /* #if !(VIDEO_MODE == MODE_NTSC) */
    #if !(VIDEO_STORAGE == STORAGE_FRAME)
        vCapParamsChan.capStorageMode = Vpif_SdramStorage_FIELD;
    #else
        vCapParamsChan.capStorageMode = Vpif_SdramStorage_FRAME;
    #endif /* #if !(VIDEO_STORAGE == STORAGE_FRAME) */
    vCapParamsChan.capVideoParams = NULL;
    vCapParamsChan.capVbiService = Vpif_VbiServiceType_SLICE_VBI;
    vCapParamsChan.capVbiSliceService = FVID_SLICE_VBI_SERVICES_CC_NTSC;
    capChInfo.chanHandle = FVID_create(vpifCapStrings,
                               IOM_INPUT,
                               &status,
                               (Ptr)&vCapParamsChan,
                               NULL);
    if ((IOM_COMPLETED != status) || (NULL == capChInfo.chanHandle))
    {
        LOG_printf(&trace, "Failed to create capture channels");
    }

    /* Create and configure display driver */
    vDisParamsChan.dispEdcTbl = &ADV7343_Fxns;
    vDisParamsChan.dispChannelIoMode = Vpif_IoMode_DIS;
    vDisParamsChan.dispFbParams.frmBufAlignment = 128u;
    vDisParamsChan.dispFbParams.frmBufSegId = 0;/* Create from system heap*/
    #if !(VIDEO_MODE == MODE_NTSC)
        vDisParamsChan.dispStdMode = Vpif_VideoMode_PAL;
    #else
        vDisParamsChan.dispStdMode = Vpif_VideoMode_NTSC;
    #endif /* #if !(VIDEO_MODE == MODE_NTSC) */
    #if !(VIDEO_STORAGE == STORAGE_FRAME)
        vDisParamsChan.dispStorageMode = Vpif_SdramStorage_FIELD;
    #else
        vDisParamsChan.dispStorageMode = Vpif_SdramStorage_FRAME;
    #endif /* #if !(VIDEO_STORAGE == STORAGE_FRAME) */
    vDisParamsChan.dispVideoParams = NULL;
    vDisParamsChan.dispVbiService = Vpif_VbiServiceType_SLICE_VBI;
    vDisParamsChan.dispVbiSliceService = FVID_SLICE_VBI_SERVICES_CC_NTSC;
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
            LOG_printf(&trace, "Failed to create display channels");
        }
    }
    else
    {
        LOG_printf(&trace, " Failed to open - Capture channel");
    }
    /*
     *  It is necessary for applications to know about the channel
     *  characteristics that are set by the driver for proper handling of
     * channel. These are : width, height, pitch and image size
     */
    if (IOM_COMPLETED == status)
    {
        status = FVID_control(capChInfo.chanHandle,
                     Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO,
                     &capParams);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to get capture channel info");
        }
        status = FVID_control(disChInfo.chanHandle,
                     Vpif_IOCTL_CMD_GET_CHANNEL_STD_INFO,
                     &disParams);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to get display channel info");
        }
    }
    else
    {
        LOG_printf(&trace, "Failed to open - Display channel");
    }

    if ((capParams.stdMode == disParams.stdMode) &&
        (capParams.activePixels == disParams.activePixels) &&
        (capParams.activeLines == disParams.activeLines) &&
        (capParams.framePerSec == disParams.framePerSec) &&
        (capParams.stdFrameFormat == disParams.stdFrameFormat) &&
        (capParams.stdVbiService == disParams.stdVbiService) &&
        (capParams.sdramStorage == disParams.sdramStorage))
    {
        LOG_printf(&trace, "Parameters same - good to go");
    }
    else
    {
        LOG_printf(&trace, "Parameters not same - not good to go");
        status = IOM_EBADARGS;
    }

    dispheight = capParams.activeLines;   /*height*/
    dispwidth = capParams.activePixels;   /*width or bytesperline*/
    sizeimage = dispheight * dispwidth;   /*sizeimage*/

    /* Allocate and Queue buffers for display & capture channel */
    if (IOM_COMPLETED == status)
    {
        for (bufCount = 0; bufCount < NUM_FRAME_BUFFERS; bufCount++)
        {
            /* Allocate Frame buffer for capture driver */
            status = FVID_allocBuffer(capChInfo.chanHandle, &(capChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, "Failed to allocate buffer for capture");
            }
            else
            {
                LOG_printf(&trace, "Cap: Alloc frame->frame.spFrm.y1 = 0x%x",
                    capChInfo.frame->frame.spFrm.y1);
                /*
                   After mapping each buffer, it is a good idea to first "zero"
                   them out. Here it is being set to a mid grey-scale
                   Y=0x80, Cb=0x80, Cr=0x80
                 */
                memset((Uint8 *)capChInfo.frame->frame.spFrm.y1,
                    0x80,
                    sizeimage);
                memset((Uint8 *)capChInfo.frame->frame.spFrm.c1,
                    0x80,
                    sizeimage);
                capChInfo.frame->vbiFrm.s1->fvidSliceServiceId = \
                    FVID_SLICE_VBI_SERVICES_CC_NTSC;    /**< Type of Slice service */
                capChInfo.frame->vbiFrm.s1->fvidField = 0; /*0: first field */
                capChInfo.frame->vbiFrm.s2->fvidSliceServiceId = \
                    FVID_SLICE_VBI_SERVICES_CC_NTSC;    /**< Type of Slice service */
                capChInfo.frame->vbiFrm.s2->fvidField = 1; /*1: second field */
                /* Queue the frame buffers for capture */
                status = FVID_queue(capChInfo.chanHandle, &(capChInfo.frame));
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, "Failed to Queue capture buffer");
                }
            }

            if (IOM_COMPLETED == status)
            {
                /* Allocate Frame buffer for display driver */
                status = FVID_allocBuffer(disChInfo.chanHandle,
                             &(disChInfo.frame));
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, "Failed to allocate buffer for display");
                }
                else
                {
                    LOG_printf(&trace, "Dis: Alloc frame->frame.spFrm.y1"
                        " = 0x%x", disChInfo.frame->frame.spFrm.y1);
                    /*
                     * After mapping each buffer, it is a good idea to first
                     * "zero" them out. Here it is being set to a mid grey-scale
                     * Y=0x80, Cb=0x80, Cr=0x80
                     */
                    memset((Uint8 *)disChInfo.frame->frame.spFrm.y1,
                        0x80,
                        disParams.activeLines*disParams.activePixels);
                    memset((Uint8 *)disChInfo.frame->frame.spFrm.c1,
                        0x80,
                        disParams.activeLines*disParams.activePixels);
                    disChInfo.frame->vbiFrm.s1->fvidSliceServiceId = \
                        FVID_SLICE_VBI_SERVICES_CC_NTSC;
                    disChInfo.frame->vbiFrm.s1->fvidField = 0; /*0: first field */
                    disChInfo.frame->vbiFrm.s2->fvidSliceServiceId = \
                        FVID_SLICE_VBI_SERVICES_CC_NTSC;
                    disChInfo.frame->vbiFrm.s2->fvidField = 1; /*1: second field */
                    /* Queue the buffers for display */
                    status = FVID_queue(disChInfo.chanHandle,
                                 &(disChInfo.frame));
                    if (IOM_COMPLETED != status)
                    {
                        LOG_printf(&trace, "Failed to Queue display buffer");
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
        LOG_printf(&trace, "Failed to get params - Display channel");
    }

    /* Configure external video encoders (ADV7343)                            */
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
                FVID_SLICE_VBI_SERVICES_CC_NTSC     /* slice vbi service */
            };
            /* Configure ADV7343 */
            status = FVID_control(disChInfo.chanHandle,
                         Vpif_IOCTL_CMD_MAX + Edc_IOCTL_CONFIG,
                         (Ptr)&vDisParamsEncoder);
        }
    }
    else
    {
        LOG_printf(&trace, "Failed to allocate/queue the buffers");
    }

    if (IOM_COMPLETED == status)
    {
        /* Configure ADV7343 */
        status = FVID_control(capChInfo.chanHandle,
                     (Vpif_IOCTL_CMD_MAX + Edc_IOCTL_CMD_MAX +
                      Tvp5147_IOCTL_SET_SLICE_VBI_SERVICE),
                     (Ptr)&capSlice);
        if (1 == status) /* Returns number of slice services set */
        {
            status = IOM_COMPLETED;
        }
    }

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
            LOG_printf(&trace, "Failed to start display channel device");
        }
        status = FVID_control(capChInfo.chanHandle, Vpif_IOCTL_CMD_START, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to start capture channel device");
        }
    }
    else
    {
        LOG_printf(&trace, "Failed to configure enc/dec");
    }

    /* Request a frame buffer from display & capture driver */
    if (IOM_COMPLETED == status)
    {
        /* Capture buffer will return the latest captured buffer */
        status = FVID_dequeue(capChInfo.chanHandle, &(capChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to dequeue capture channel device");
        }
        LOG_printf(&trace, "DQ Cap vpifFrm.y1 = 0x%x",
            capChInfo.frame->frame.spFrm.y1);

        /* Display buffer will return a free buffer */
        status = FVID_dequeue(disChInfo.chanHandle, &(disChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to dequeue display channel device");
        }
        LOG_printf(&trace, "DQ Dis vpifFrm.y1 = 0x%x",
            disChInfo.frame->frame.spFrm.y1);

    }
    else
    {
        LOG_printf(&trace, "Failed to start display/capture driver");
    }


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
            BCACHE_inv(capChInfo.frame->frame.spFrm.y1,
                (sizeimage * 2),
                TRUE);
            /* Give the old capture frame buffer back to driver and get the
               recently captured frame buffer */
            status = FVID_exchange(capChInfo.chanHandle, &(capChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, "%d Error in exchanging capture buffer", status);
            }
            else
            {
                /* Flush and invalidate the processed buffer so that the DMA
                   reads the processed data */
                BCACHE_wbInv(capChInfo.frame->frame.spFrm.y1,
                    (sizeimage * 2),
                    TRUE);

                /* Note: We are directly exchanging the capture buffer to
                   display channel instead of copying to display buffer.
                   Give the captured frame buffer to display driver and get a
                   free frame buffer for next capture */
                status = FVID_exchange(disChInfo.chanHandle,
                             &(capChInfo.frame));
                LOG_printf(&trace, "EX vpifFrm.y1 = 0x%x",
                    capChInfo.frame->frame.spFrm.y1);
                if (IOM_COMPLETED != status)
                {
                    LOG_printf(&trace, "%d Error in exchanging display buffer", status);
                }
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
            LOG_printf(&trace, "Error in stopping display operation");
        }
        /* Free the buffer owned by application */
        status = FVID_freeBuffer(disChInfo.chanHandle, &(disChInfo.frame));
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "IOM_COMPLETED != status for free buff");
        }
        /* Dequeue buffers from driver and free them */
        for (bufCount = 0; bufCount < (NUM_FRAME_BUFFERS - 1u); bufCount++)
        {
            status = FVID_dequeue(disChInfo.chanHandle, &(disChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, "IOM_COMPLETED != status for DQ");
            }
            LOG_printf(&trace, "DQ disp vpifFrm.y1 = 0x%x",
                disChInfo.frame->frame.spFrm.y1);
            status = FVID_freeBuffer(disChInfo.chanHandle, &(disChInfo.frame));
            if (IOM_COMPLETED != status)
            {
                LOG_printf(&trace, "IOM_COMPLETED != status for free buff");
            }
        }
        status = FVID_control(capChInfo.chanHandle, Vpif_IOCTL_CMD_STOP, NULL);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Error in stopping capture operation");
        }
        status = FVID_freeBuffer(capChInfo.chanHandle, &(capChInfo.frame));
        /* Dequeue buffers from driver and free them */
        for (bufCount = 0; bufCount < (NUM_FRAME_BUFFERS - 1u); bufCount++)
        {
            status = FVID_dequeue(capChInfo.chanHandle, &(capChInfo.frame));
            LOG_printf(&trace, "DQ cap vpifFrm.y1 = 0x%x",
                capChInfo.frame->frame.spFrm.y1);
            status = FVID_freeBuffer(capChInfo.chanHandle, &(capChInfo.frame));
        }
    }
    else
    {
        LOG_printf(&trace, "Failed to alloc or exchange buffers");
    }


    /* Delete capture and display channel */
    if (IOM_COMPLETED == status)
    {
        /* Delete display drivers */
        status = FVID_delete(disChInfo.chanHandle);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to delete display channel");
        }
        status = FVID_delete(capChInfo.chanHandle);
        if (IOM_COMPLETED != status)
        {
            LOG_printf(&trace, "Failed to delete capture channel");
        }
        if (IOM_COMPLETED == status)
        {
            LOG_printf(&trace, "Application closed successfully!!");
        }
    }
    else
    {
        LOG_printf(&trace, "Failed to stop display/capture channel");
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
    LOG_printf(&trace, "VPIF_SAMPLE :Starting Sample Application VBI LOOPBACK mode");
    runVpifSample();
    LOG_printf(&trace, "VPIF_SAMPLE :Sample Application Ends VBI LOOPBACK mode");

    /* Start the Heart Beat Print */
   tskHeartBit();
}

/******************************************************************************/
/* End of file                                                                */
/******************************************************************************/
