/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/* Standard Linux headers */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <media/davinci/videohd.h>

#include <xdc/std.h>
#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/Capture.h>
#include <ti/sdo/dmai/ColorSpace.h>
#include <ti/sdo/dmai/BufferGfx.h>

#include "../priv/_VideoBuf.h"

#define MODULE_NAME     "Capture"

/* Number of retries finding a video source before failing */
#define NUM_IOCTL_RETRIES        300

/* The following string sequence should match Capture_Input enum */
static Char *captureInputString[Capture_Input_COUNT] = {
    "S-Video",
    "Composite",
    "Component"
};

static v4l2_std_id stds[VideoStd_COUNT] = { 0, 0, 0, 0, 0, V4L2_STD_NTSC,
    V4L2_STD_PAL, V4L2_STD_525P_60, V4L2_STD_625P_50, V4L2_STD_720P_60, 
    V4L2_STD_720P_50, 0, V4L2_STD_1080I_60, V4L2_STD_1080I_50, 
    0, 0, 0, V4L2_STD_1080P_60, V4L2_STD_1080P_50 };

typedef struct Capture_Object {
    Int                  fd;
    Int16                userAlloc;
    Int16                started;
    Int32                topOffset;
    BufTab_Handle        hBufTab;
    VideoStd_Type        videoStd;
    struct _VideoBufDesc *bufDescs;
    Int                  resizerFd;
    Int                  previewerFd;
} Capture_Object;

/******************************************************************************
 * cleanup
 ******************************************************************************/
static Int cleanup(Capture_Handle hCapture)
{
    BufTab_Handle         hBufTab    = hCapture->hBufTab;
    Int                   ret        = Dmai_EOK;
    Int8                 *capBufPtr;
    enum v4l2_buf_type    type;
    Uns                   bufIdx;
    Buffer_Handle         hCapBuf;

    if (hCapture->fd != -1) {
        if (hCapture->started) {
            /* Shut off the video capture */
            type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (ioctl(hCapture->fd, VIDIOC_STREAMOFF, &type) == -1) {
                Dmai_err1("VIDIOC_STREAMOFF failed (%s)\n", strerror(errno));
                ret = Dmai_EFAIL;
            }
        }

        if (hCapture->userAlloc == FALSE && hBufTab) {
            for (bufIdx = 0;
                 bufIdx < BufTab_getNumBufs(hBufTab);
                 bufIdx++) {

                hCapBuf = BufTab_getBuf(hBufTab, bufIdx);
                capBufPtr = Buffer_getUserPtr(hCapBuf);

                if (munmap(capBufPtr - hCapture->topOffset,
                           hCapture->bufDescs[bufIdx].v4l2buf.length) == -1) {
                    Dmai_err1("Failed to unmap capture buffer%d\n", bufIdx);
                    ret = Dmai_EFAIL;
                }
            }
        }

        if (close(hCapture->fd) == -1) {
            Dmai_err1("Failed to close capture device (%s)\n", strerror(errno));
            ret = Dmai_EIO;
        }

        if (hCapture->bufDescs) {
            free(hCapture->bufDescs);
        }
    }

    free(hCapture);

    return ret;
}

/******************************************************************************
 * Capture_create
 ******************************************************************************/
Capture_Handle Capture_create(BufTab_Handle hBufTab, Capture_Attrs *attrs)
{
    struct v4l2_capability      cap;
    struct v4l2_cropcap         cropCap;
    struct v4l2_crop            crop;
    struct v4l2_format          fmt;
    enum v4l2_buf_type          type;
    Capture_Handle              hCapture;
    VideoStd_Type               videoStd;
    Int32                       width, height;
    Uint32                      pixelFormat;

    assert(attrs);
    Dmai_clear(fmt);

    /* Allocate space for state object */
    hCapture = calloc(1, sizeof(Capture_Object));

    if (hCapture == NULL) {
        Dmai_err0("Failed to allocate space for Capture Object\n");
        return NULL;
    }

    /* User allocated buffers by default */
    hCapture->userAlloc = TRUE;

    /* Open video capture device */
    hCapture->fd = open(attrs->captureDevice, O_RDWR, 0);

    if (hCapture->fd == -1) {
        Dmai_err2("Cannot open %s (%s)\n", attrs->captureDevice,
                                           strerror(errno));
        cleanup(hCapture);
        return NULL;
    }

    /* See if an input is connected, and if so which standard */
    if (Capture_detectVideoStd(hCapture, &videoStd, attrs) < 0) {
        cleanup(hCapture);
        return NULL;
    }

    hCapture->videoStd = videoStd;

    if (VideoStd_getResolution(videoStd, &width, &height) < 0) {
        cleanup(hCapture);
        Dmai_err0("Failed to get resolution of capture video standard\n");
        return NULL;
    }

    /* Query for capture device capabilities */
    if (ioctl(hCapture->fd, VIDIOC_QUERYCAP, &cap) == -1) {
        cleanup(hCapture);
        if (errno == EINVAL) {
            Dmai_err1("%s is no V4L2 device\n", attrs->captureDevice);
            cleanup(hCapture);
            return NULL;
        }
        Dmai_err2("Failed VIDIOC_QUERYCAP on %s (%s)\n", attrs->captureDevice,
                                                       strerror(errno));
        cleanup(hCapture);
        return NULL;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        Dmai_err1("%s is not a video capture device\n", attrs->captureDevice);
        cleanup(hCapture);
        return NULL;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        Dmai_err1("%s does not support streaming i/o\n", attrs->captureDevice);
        cleanup(hCapture);
        return NULL;
    }

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(hCapture->fd, VIDIOC_G_FMT, &fmt) == -1) {
        Dmai_err2("Failed VIDIOC_G_FMT on %s (%s)\n", attrs->captureDevice,
                                                      strerror(errno));
        cleanup(hCapture);
        return NULL;
    }

    fmt.fmt.pix.width        = width;
    fmt.fmt.pix.height       = height; 
    fmt.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    switch(attrs->colorSpace) {
        case ColorSpace_UYVY: 
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
            break;
        case ColorSpace_YUV420PSEMI:
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV12;
            break;
        case ColorSpace_YUV422PSEMI:
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_NV16;
            break;
        default:
            Dmai_err1("Unsupported color format %g\n", attrs->colorSpace);            
            cleanup(hCapture);
            return NULL;
    }
    fmt.fmt.pix.bytesperline = BufferGfx_calcLineLength(fmt.fmt.pix.width, 
                                    attrs->colorSpace);
    fmt.fmt.pix.sizeimage    = BufferGfx_calcSize(attrs->videoStd, attrs->colorSpace);
    pixelFormat = fmt.fmt.pix.pixelformat;
    
    if ((videoStd == VideoStd_CIF) || (videoStd == VideoStd_SIF_PAL) || 
        (videoStd == VideoStd_SIF_NTSC) || (videoStd == VideoStd_D1_PAL) ||
        (videoStd == VideoStd_D1_NTSC) || (videoStd == VideoStd_1080I_30) ||
        (videoStd == VideoStd_1080I_25)) {
        fmt.fmt.pix.field        = V4L2_FIELD_INTERLACED;
    } else {
        fmt.fmt.pix.field        = V4L2_FIELD_NONE;
    }


    if (ioctl(hCapture->fd, VIDIOC_S_FMT, &fmt) == -1) {
        Dmai_err2("Failed VIDIOC_S_FMT on %s (%s)\n", attrs->captureDevice,
                                                      strerror(errno));
        cleanup(hCapture);
        return NULL;
    }

    if ((fmt.fmt.pix.width != width)  || (fmt.fmt.pix.height != height)) {
        Dmai_err4("Failed to set resolution %d x %d (%d x %d)\n", width,
                    height, fmt.fmt.pix.width, fmt.fmt.pix.height);
        cleanup(hCapture);
        return NULL;
    }

    if (pixelFormat != fmt.fmt.pix.pixelformat) {
        Dmai_err2("Pixel format 0x%x not supported. Received 0x%x\n", 
            pixelFormat, fmt.fmt.pix.pixelformat);
        cleanup(hCapture);
        return NULL;        
    }

    Dmai_dbg3("Video input connected size %dx%d pitch %d\n",
              fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.bytesperline);

    /* Query for video input cropping capability */

    if (attrs->cropWidth > 0 && attrs->cropHeight > 0) {

        cropCap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(hCapture->fd, VIDIOC_CROPCAP, &cropCap) == -1) {
            Dmai_err2("VIDIOC_CROPCAP failed on %s (%s)\n", attrs->captureDevice,
                                                            strerror(errno));
            cleanup(hCapture);
            return NULL;
        }

        if (attrs->cropX & 0x1) {
            Dmai_err1("Crop width (%ld) needs to be even\n", attrs->cropX);
            cleanup(hCapture);
            return NULL;
        }

        crop.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c.left   = attrs->cropX;
        crop.c.top    = attrs->cropY;
        crop.c.width  = attrs->cropWidth;
        crop.c.height = hCapture->topOffset ? attrs->cropHeight + 4 + 2 :
                                              attrs->cropHeight;

        Dmai_dbg4("Setting capture cropping at %dx%d size %dx%d\n",
                  crop.c.left, crop.c.top, crop.c.width, crop.c.height);

        /* Crop the image depending on requested image size */
        if (ioctl(hCapture->fd, VIDIOC_S_CROP, &crop) == -1) {
            Dmai_err2("VIDIOC_S_CROP failed on %s (%s)\n", attrs->captureDevice,
                                                           strerror(errno));
            cleanup(hCapture);
            return NULL;
        }
    }

    if (hBufTab == NULL) {
        hCapture->userAlloc = FALSE;

        /* The driver allocates the buffers */
        if (_Dmai_v4l2DriverAlloc(hCapture->fd,
                                  attrs->numBufs,
                                  V4L2_BUF_TYPE_VIDEO_CAPTURE,
                                  &hCapture->bufDescs,
                                  &hBufTab,
                                  hCapture->topOffset,
                                  attrs->colorSpace) < 0) {
            Dmai_err1("Failed to allocate capture driver buffers on %s\n",
                      attrs->captureDevice);
            cleanup(hCapture);
            return NULL;
        }
    }
    else {
        /* Make the driver use the user supplied buffers */
        if (_Dmai_v4l2UserAlloc(hCapture->fd,
                                attrs->numBufs,
                                V4L2_BUF_TYPE_VIDEO_CAPTURE,
                                &hCapture->bufDescs,
                                hBufTab,
                                0, attrs->colorSpace) < 0) {
            Dmai_err1("Failed to intialize capture driver buffers on %s\n",
                      attrs->captureDevice);
            cleanup(hCapture);
            return NULL;
        }
    }

    hCapture->hBufTab = hBufTab;

    /* Start the video streaming */
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(hCapture->fd, VIDIOC_STREAMON, &type) == -1) {
        Dmai_err2("VIDIOC_STREAMON failed on %s (%s)\n", attrs->captureDevice,
                                                         strerror(errno));
        cleanup(hCapture);
        return NULL;
    }

    hCapture->started = TRUE;

    return hCapture;
}

/******************************************************************************
 * Capture_delete
 ******************************************************************************/
Int Capture_delete(Capture_Handle hCapture)
{
    Int ret = Dmai_EOK;

    if (hCapture) {
        ret = cleanup(hCapture);
    }

    return ret;
}

/******************************************************************************
 * Capture_detectVideoStd
 ******************************************************************************/
Int Capture_detectVideoStd(Capture_Handle hCapture, VideoStd_Type *videoStdPtr,
                           Capture_Attrs *attrs)
{
    Int                failCount  =  0;
    Int                firstInput;
    Int                queryInput;
    struct v4l2_input  v4l2Input;
    v4l2_std_id        std;
    Int                input;
    Int                fd;
    Int                ret;

    assert(videoStdPtr);
    assert(attrs);

    /*
     * Initialize variables outside of variable declarations to suppress
     * "unused variable" warnings for platforms that don't use them.
     */
    Dmai_clear(v4l2Input);
    queryInput =  0;
    firstInput = -1;

    if (attrs->videoStd < 0 || attrs->videoStd > VideoStd_COUNT) {
        Dmai_err1("Invalid capture standard given (%d)\n", attrs->videoStd);
        return Dmai_EINVAL;
    }

    if (hCapture) {
        fd = hCapture->fd;
    }
    else {
        fd = open(attrs->captureDevice, O_RDWR, 0);

        if (fd == -1) {
            Dmai_err2("Cannot open %s (%s)\n", attrs->captureDevice,
                                               strerror(errno));
            return Dmai_EFAIL;
        }
    }

    if (attrs->decoderIdx < 0) {
        v4l2Input.type = V4L2_INPUT_TYPE_CAMERA;
        v4l2Input.index = 0;
        do {
            if (ioctl(fd, VIDIOC_ENUMINPUT, &v4l2Input) != 0) {
                Dmai_err2("%s input not found (%s)\n",
                          captureInputString[attrs->videoInput], strerror(errno));
                return Dmai_EFAIL;
            }
            v4l2Input.index++;        
        } while (strcmp((Char *) v4l2Input.name,
                            captureInputString[attrs->videoInput]) != 0);
        input = --v4l2Input.index;

        Dmai_dbg1("%s input selected\n", v4l2Input.name);

    } else {
        input = attrs->decoderIdx;
    }

    if (ioctl(fd, VIDIOC_S_INPUT, &input) == -1) {
        Dmai_err2("Failed to set video input to %d (%s)\n", 
            input, strerror(errno));
        return Dmai_EFAIL;
    }

    if (ioctl(fd, VIDIOC_G_INPUT, &queryInput) == -1) {
        Dmai_err0("Failed to retrieve video input setting\n");
        return Dmai_EFAIL;
    }

    if (input != queryInput) {
        Dmai_dbg2("Attempted to set video input to %d, but it still has a"
                  "setting of %d\n", input, queryInput);
        return Dmai_EFAIL;
    }

    if (attrs->videoStd != VideoStd_AUTO) {
        /* Force video standard */
        std = stds[attrs->videoStd];
    } else {
        Dmai_dbg0("Checking video standard\n");

        /* It might take a few tries to detect the signal */
        do {
            ret = ioctl(fd, VIDIOC_QUERYSTD, &std);

            if (ret == -1 && errno == EAGAIN) {
                usleep(1);
                failCount++;
            }
        } while (ret == -1 && errno == EAGAIN && failCount < NUM_IOCTL_RETRIES);

        if (ret == -1) {
            Dmai_err1("VIDIOC_QUERYSTD failed on %s. Video input connected?\n",
                      attrs->captureDevice);
            return Dmai_EFAIL;
        }
    }

    if(ioctl(fd, VIDIOC_S_STD, &std) == -1) {
        Dmai_err2("VIDIOC_S_STD failed on %s (%s)\n",
                  attrs->captureDevice, strerror(errno));
        return Dmai_EFAIL;
    }

    if (!hCapture) {
        close(fd);
    }

    if (std & V4L2_STD_NTSC) {
       *videoStdPtr = VideoStd_D1_NTSC;
    }
    else if (std & V4L2_STD_PAL) {
        *videoStdPtr = VideoStd_D1_PAL;
    }
    else if (std & V4L2_STD_525P_60) {
        *videoStdPtr = VideoStd_480P;
    }
    else if (std & V4L2_STD_625P_50) {
        *videoStdPtr = VideoStd_576P;
    }
    else if (std == V4L2_STD_720P_60) {
       *videoStdPtr = VideoStd_720P_60;
    }
    else if (std == V4L2_STD_720P_50) {
        *videoStdPtr = VideoStd_720P_50;
    }
#if 0
    else if (std == V4L2_STD_720P_30) {
        *videoStdPtr = VideoStd_720P_30;
    }
#endif
    else if (std == V4L2_STD_1080I_60) {
        *videoStdPtr = VideoStd_1080I_30;
    }
    else if (std == V4L2_STD_1080I_50) {
        *videoStdPtr = VideoStd_1080I_25;
    }
#if 0    
    else if (std == V4L2_STD_1080P_30) {
        *videoStdPtr = VideoStd_1080P_30;
    }
    else if (std == V4L2_STD_1080P_25) {
        *videoStdPtr = VideoStd_1080P_25;
    }
    else if (std == V4L2_STD_1080P_24) {
        *videoStdPtr = VideoStd_1080P_24;
    }
#endif
    else if (std == V4L2_STD_1080P_60) {
        *videoStdPtr = VideoStd_1080P_60;
    }
    else if (std == V4L2_STD_1080P_50) {
        *videoStdPtr = VideoStd_1080P_50;
    }        
    else {
        Dmai_err1("Unknown video standard on capture device %s\n",
                  attrs->captureDevice);
        return Dmai_EFAIL; 
    }

    attrs->videoStd = *videoStdPtr;
    Dmai_dbg2("Capture input set to %s:%d\n",
              captureInputString[attrs->videoInput], *videoStdPtr);

    return Dmai_EOK;
}

/******************************************************************************
 * Capture_getVideoStd
 ******************************************************************************/
VideoStd_Type Capture_getVideoStd(Capture_Handle hCapture)
{
    return hCapture->videoStd;
}

/******************************************************************************
 * Capture_getBufTab
 ******************************************************************************/
BufTab_Handle Capture_getBufTab(Capture_Handle hCapture)
{
    return hCapture->hBufTab;
}

/******************************************************************************
 * Capture_get
 ******************************************************************************/
Int Capture_get(Capture_Handle hCapture, Buffer_Handle *hBufPtr)
{
    struct v4l2_buffer v4l2buf;

    assert(hCapture);
    assert(hBufPtr);

    Dmai_clear(v4l2buf);
    v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2buf.memory = hCapture->userAlloc ? V4L2_MEMORY_USERPTR :
                                           V4L2_MEMORY_MMAP;

    /* Get a frame buffer with captured data */
    if (ioctl(hCapture->fd, VIDIOC_DQBUF, &v4l2buf) < 0) {
        Dmai_err1("VIDIOC_DQBUF failed (%s)\n", strerror(errno));
        return Dmai_EFAIL;
    }

    *hBufPtr = hCapture->bufDescs[v4l2buf.index].hBuf;
    hCapture->bufDescs[v4l2buf.index].used = TRUE;
    Buffer_setNumBytesUsed(*hBufPtr, v4l2buf.bytesused);

    return Dmai_EOK;
}

/******************************************************************************
 * Capture_put
 ******************************************************************************/
Int Capture_put(Capture_Handle hCapture, Buffer_Handle hBuf)
{
    Int idx;

    assert(hCapture);
    assert(hBuf);

    idx = getUsedIdx(hCapture->bufDescs, BufTab_getNumBufs(hCapture->hBufTab));

    if (idx < 0) {
        Dmai_err0("You must get a captured buffer before putting one\n");
        return Dmai_ENOMEM;
    }

    hCapture->bufDescs[idx].v4l2buf.m.userptr =
        (Int) Buffer_getUserPtr(hBuf);
    hCapture->bufDescs[idx].v4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    /* Issue captured frame buffer back to device driver */
    if (ioctl(hCapture->fd, VIDIOC_QBUF,
              &hCapture->bufDescs[idx].v4l2buf) == -1) {
        Dmai_err1("VIDIOC_QBUF failed (%s)\n", strerror(errno));
        return Dmai_EFAIL;
    }

    hCapture->bufDescs[idx].hBuf = hBuf;
    hCapture->bufDescs[idx].used = FALSE;

    return Dmai_EOK;
}

