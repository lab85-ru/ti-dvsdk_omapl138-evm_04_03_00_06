/*
 * rasterSample_main.c
 *
 * This file contains the test / demo code to demonstrate the LCDC Raster driver 
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

/** \file   rasterSample_io.c
 *
 *   \brief  This file contains sample code to demonstrate the
 *          LCDC Raster driver implemented for DSP/BIOS - OMAPL138
 *
 * (C) Copyright 2006, Texas Instruments, Inc
 *
 *  \author     PSG
 *  \version    0.1
 */
#include "std.h"
#include "stdio.h"
#include "string.h"
#include "log.h"
#include "tsk.h"
#include "ecm.h"
#include "iom.h"
#include "gio.h"
#include "sio.h"

#include "ti/pspiom/platforms/evmOMAPL138/Lcdraster_evmInit.h"
#include "ti/pspiom/lcdcraster/Raster.h"
#include "ti/pspiom/i2c/I2c.h"

/* Image Data File */
#include "rasterSample_image16bpp.h"

/*
 * External references
 */
extern LOG_Obj trace;

#define rasterBPP                            16u
/**< Default bits per pixel */

#define RASTER_SA_DEFAULT_FB_COUNT       5u
/**< Default number of frame buffers */
#define RASTER_SA_SECS_TO_FLIP        20u
/**< Time in seconds */

#define RASTER_NUM_IOP 10

#define BUFSIZE 36

static Uint32               frameBufferCount = 0;
static Uint32               fbCount          = RASTER_SA_DEFAULT_FB_COUNT;
static Uint32               paletteSize      = 32u;
static Uint32               imageSize        = ((480*272u*rasterBPP)/8u);
static Ptr                  palettePtr       = &paletteData16;
static Ptr                  imagePtr         = &imageData16;
static Uint32               bpp              = rasterBPP;

static Raster_FrameBuffer    rasterFB[RASTER_SA_DEFAULT_FB_COUNT];
#pragma DATA_ALIGN (fbufferArray0,32);
static char fbufferArray0[261200];
#pragma DATA_ALIGN (fbufferArray1,32);
static char fbufferArray1[261200];
#pragma DATA_ALIGN (fbufferArray2,32);
static char fbufferArray2[261200];
#pragma DATA_ALIGN (fbufferArray3,32);
static char fbufferArray3[261200];
#pragma DATA_ALIGN (fbufferArray4,32);
static char fbufferArray4[261200];

static Raster_FrameBuffer    *rasterFBPtr       = NULL;

static SIO_Handle rasterChanHandle;

const Raster_RasterConf rasterConf =
{
        Raster_Output_TFT_RIGHTALIGNED,                  /* outputFormat     */
        Raster_Intface_PARALLEL_16PIN,                   /* interface        */
        Raster_Panel_TFT,                                /* panel            */
        Raster_Display_COLOR,                            /* display          */
        Raster_FbBitsPerPixel_16BPP,                     /* bitsPP           */
        Raster_FBContent_PALETTE_DATA,                   /* fbContent        */
        Raster_DataOrder_LSBMSB,                         /* dataOrder        */
        FALSE,                                           /* nibbleMode       */
        /* RasterSubpanel Configuration */
        {
             FALSE,                                      /* enable           */
             Raster_SubpanelPos_HIGH,                    /* position         */
             24u,                                        /* lppt             */
             0x00                                        /* defaultData      */
        },
        /*RasterTiming2 Configuration */
        {
            FALSE,                                      /* iAcbias          */
            TRUE,                                       /* iPclk            */
            TRUE,                                       /* iLclk            */
            TRUE,                                       /* iFclk            */
            TRUE,                                       /* syncCtrl         */
            Raster_SyncEdge_RISING,                     /* syncEdge         */
            0,                                          /* acBiasInt        */
            255u                                        /* acbiasFreq       */
        },
        2,                                              /* fifoDmaDelay     */
        /* intMask Configuration         */
        (

            Raster_Intr_OFIFO_UNDERRUN_ENABLE |
            Raster_Intr_FRAMESYNC_LOST_ENABLE
        ),
        (2u-1u),                                    /* hFP              */
        (2u-1u),                                    /* hBP              */
        (41u-1u),                                   /* hSPW             */
        ((480u/16-1u)),                             /* pPL              */ /* Refer to section 3.9 and 3.10 of SPRUFM0*/
        (4u-1u),                                    /* vFP              */
        (4u-1u),                                    /* vBP              */
        (10u-1u),                                    /* vSPW             */
        (272u-1u)                                   /* lPP              */
};

/* channel params for the SIO stream */
Raster_ChanParams rasterChanParams =
{
    Raster_Controller_RASTER,   /* controller */
    (Ptr)&rasterConf,           /* chanConf   */
    0                           /* segId      */
};

static Void tskHeartBeat(Void);

/**
 * This task is used for the image flipping.
 * This is a generalized task so that any configuration can be supported.
 * Care has been taken to make this run for different BPPs as well.
 * This is an create-open-flip-close-delete task
 */
static Void rasterSampleFlipTask(Void)
{
    Uint32           i, j;
    Ptr              reclaimPtr;
    Int32            sioStatus;

    for(i = 0; i < fbCount; i++)
    {
        switch (i)
        {
            case 0:
                rasterFB[0].frameBufferPtr = &fbufferArray0[0];
                break;
            case 1:
                rasterFB[1].frameBufferPtr = &fbufferArray1[0];
                break;
            case 2:
                rasterFB[2].frameBufferPtr = &fbufferArray2[0];
                break;
            case 3:
                rasterFB[3].frameBufferPtr = &fbufferArray3[0];
                break;
            case 4:
                rasterFB[4].frameBufferPtr = &fbufferArray4[0];
                break;
        }
    }


    /* Populate frame buffer */
    for(i = 0; i < fbCount; i++)
    {
        memcpy(rasterFB[i].frameBufferPtr, palettePtr, paletteSize);

        memcpy((Uint8*)(rasterFB[i].frameBufferPtr)+paletteSize,
                        imagePtr,
                        imageSize);


        rasterFB[i].pitch = (480u*bpp)/8u;
        rasterFB[i].lines = 272u;
    }

    for(i = 0; i < fbCount-1; i++)
    {
        size_t size = sizeof(rasterFB[i]);
        sioStatus = SIO_issue(rasterChanHandle,&rasterFB[i],size,0);
    }

    rasterFBPtr = &rasterFB[fbCount-1];

    i = 0;
    j = 0;

    for(i=0; i<frameBufferCount; i++)
    {
       size_t size;

       sioStatus = SIO_reclaim(rasterChanHandle, &reclaimPtr, NULL);

       if(NULL == reclaimPtr)
       {
           LOG_printf(&trace,
            "\nError in recalim issued buffers, SIO_reclaim returned: %d\n",
                   sioStatus);
       }
       else
       {
            rasterFBPtr = reclaimPtr;

            memcpy(rasterFBPtr->frameBufferPtr, palettePtr, paletteSize);
            memcpy(((Uint8*)(rasterFBPtr->frameBufferPtr)+paletteSize),
                   imagePtr,
                   imageSize);

            rasterFBPtr->pitch    = (480u * bpp) / 8u;
            rasterFBPtr->lines    = 272u;

            if(272u == j)
            {
                j = 0;
            }

            memset((((Uint8*)(rasterFBPtr->frameBufferPtr)+paletteSize)
                    + (j*((480u/8u)*bpp))),
                   0x0fu,
                   ((480u/8u)*bpp));
            j++;

            size = sizeof(Raster_FrameBuffer);

            sioStatus = SIO_issue(rasterChanHandle, rasterFBPtr, size, 0);

            if(0 != sioStatus)
            {
                LOG_printf(&trace, "\r\nExchange: FAILED");
            }
        }
    }


    LOG_printf(&trace, "\nDisabling the raster");

    /* Disable raster before closing */
    SIO_ctrl(rasterChanHandle, Raster_IOCTL_RASTER_DISABLE, NULL);
    
    /* Reclaim all the issued buffers                                         */
    for(i = 0; i < fbCount-1 ; i++)
    {
        sioStatus = SIO_reclaim(rasterChanHandle,
                    &reclaimPtr,
                    NULL);
    }

    for(i = 0; i < fbCount; i++)
    {
        rasterFB[i].frameBufferPtr = NULL;
    }

    LOG_printf(&trace, "\nDeleting the RASTER channel");

    SIO_delete(rasterChanHandle);
}

/**
 * Flips a 16 rasterBPP image
 */
static Void rasterFlipTest(Void)
{
    palettePtr              = &paletteData16;
    imagePtr                = &imageData16;
    bpp                     = 16u;

    paletteSize             = 32u;
    imageSize               = ((480u*272u*bpp)/8u);
    frameBufferCount        = RASTER_SA_SECS_TO_FLIP * 60u;
    fbCount = 5u;

    LOG_printf(&trace, "\nrasterSampleFlipTask start");

    rasterSampleFlipTask();

    LOG_printf(&trace, "\nrasterSampleFlipTask done\n");

    return;
}


Void rasterSampleTask(Ptr arg1, Ptr arg2)
{
    SIO_Attrs       streamparams = SIO_ATTRS;

    arg1 = arg1;
    arg2 = arg2;

    /* call the EVM specific function to configure the LCDC                   */
    configureLcdraster();

    /*Intialize the stream parameters                                         */
    streamparams.nbufs = RASTER_NUM_IOP;
    streamparams.model = SIO_ISSUERECLAIM;

    /* Create a channel to the LCDC Raster driver                             */
    rasterChanHandle = SIO_create("/rasterDio",
                                  SIO_OUTPUT,
                                  BUFSIZE,
                                  &streamparams);

    if(NULL == rasterChanHandle)
    {
        LOG_printf(&trace, "ERROR: CHANNEL CREATION TO RASTER DRIVER FAILED\n");
        return;
    }

    /* Run RASTER sample application */
    rasterFlipTest();

    /* Start the Heart Beat Print */
    tskHeartBeat();

    return;
}

Void tskHeartBeat(Void)
{
    static volatile Uint32 counter = 0;

    while (counter < 0xFFFFFFFF)
    {
        TSK_sleep(1000u);    /* Add large delay */
        LOG_printf(&trace, "\n!!! HeartBeat");
        counter++;
    }
}

Void rasterSysErrHandler(char *s, int errNo, char* vargs)
{
    /* To avoid warnings                                                      */
    vargs = vargs;
    /* To print the BIOS error message                                        */
    LOG_printf(&trace,"\r\nError : %s\n\r",SYS_errors[errNo]);
    /* To print the custom error message                                      */
    LOG_printf(&trace,"\r\nError Message: %s",s);
}
/*!
*! Revision History
*! ================
*! 12-Dec-2008 connell created from IDriver LCDC Raster sample
*! */


