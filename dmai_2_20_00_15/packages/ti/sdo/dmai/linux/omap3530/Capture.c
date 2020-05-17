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
#include <linux/videodev.h>
#include <linux/videodev2.h>

#include <xdc/std.h>
#include <ti/sdo/dmai/Cpu.h>
#include <ti/sdo/dmai/Dmai.h>
#include <ti/sdo/dmai/BufTab.h>
#include <ti/sdo/dmai/Capture.h>
#include <ti/sdo/dmai/VideoStd.h>
#include <ti/sdo/dmai/ColorSpace.h>
#include <ti/sdo/dmai/BufferGfx.h>

#include <linux/media.h>
#include <linux/v4l2-mediabus.h>
#include <linux/v4l2-subdev.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>



#include "priv/_VideoBuf.h"

#define MODULE_NAME     "Capture"

#define CAPTURE_DEVICE		"/dev/video2"
#define DISPLAY_DEVICE		"/dev/video7"
#define MEDIA_DEVICE		"/dev/media0"

/* Media entity names */
#define ENTITY_VIDEO_CCDC_OUT_NAME	"OMAP3 ISP CCDC output"
#define ENTITY_CCDC_NAME		"OMAP3 ISP CCDC"
#define ENTITY_TVP514X_NAME		"tvp514x 3-005c"
#define ENTITY_MT9T111_NAME		"mt9t111 2-003c"

#define NUM_BUFFERS     	3
#define CAPTURE_MAX_BUFFER	NUM_BUFFERS
#define DISPLAY_MAX_BUFFER	NUM_BUFFERS
#define CAPTURE_NAME		"Capture"
#define DISPLAY_NAME		"Display"
#define MEDIA_NAME		"Media"

/* number of frames to be captured and displayed */
#define MAXLOOPCOUNT		1000

#define DEF_PIX_FMT		V4L2_PIX_FMT_UYVY
#define IMG_WIDTH_PAL_NTSC	720
#define IMG_HEIGHT_NTSC		480
#define IMG_HEIGHT_PAL		574

#define IMG_WIDTH_VGA		640
#define IMG_HEIGHT_VGA		480



/* Function declaration */
static int  open_video_dev(const char *dev, int *capture_fd);

static Char *captureInputString[Capture_Input_COUNT] = {
    "SVIDEO",
    "COMPOSITE",
    "COMPONENT"
};

typedef struct Capture_Object {
    Int                  fd;
    Int16                userAlloc;
    Int16                started;
    Int32                topOffset;
    BufTab_Handle        hBufTab;
    VideoStd_Type        videoStd;
    struct _VideoBufDesc *bufDescs;
} Capture_Object;

/* structure used to store information of the buffers */
struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

struct media_dev {
	/* Media device */
	int media_fd;
	/* either tvp5146 or mt9t111 */
	int input_source;
	/* All entities */
	struct media_entity_desc entity[20];
	/* Entities we do care about */
	int video;	/* Streaming entity */
	int ccdc;
	int tvp5146;
	int mt9t111;

	/* Total number of entities */
	unsigned int num_entities;
};

struct capture_dev {
	int capture_fd;

	struct v4l2_format capture_fmt;
	struct v4l2_buffer capture_buf;

	int tvp_input;	/* Only applicable for tvp5146 */

	unsigned int num_bufs;
	/* Width and height for current input */
	unsigned int width;
	unsigned int height;
};

struct display_dev {
	int display_fd;

	struct v4l2_format display_fmt;
	struct v4l2_buffer display_buf;

	unsigned int num_bufs;
	/* Width and height for current input */
	unsigned int width;
	unsigned int height;
};

/* capture_buff_info and display_buff_info stores buffer information of capture
   and display respectively. */
static struct buf_info capture_buff_info[CAPTURE_MAX_BUFFER];

static int media_device_open(struct media_dev *media)
{
	/* Open the Media device */
	media->media_fd = open((const char *) MEDIA_DEVICE, O_RDWR);
	if (media->media_fd  <= 0) {
		printf("Cannot open = %s device\n", MEDIA_DEVICE);
		return -1;
	}
	printf("\n%s: Opened Media Device\n", MEDIA_NAME);
	return 0;
}

static void media_device_close(int media_fd)
{
	/* Close the Media device */
	close(media_fd);
}

static int enumerate_all_entities(struct media_dev *media)
{
	int ret, index;

	printf("Enumerating media entities\n");
	index = 0;
	do {
		memset(&media->entity[index], 0, sizeof(struct media_entity_desc));
		media->entity[index].id = index | MEDIA_ENTITY_ID_FLAG_NEXT;
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_ENTITIES, &media->entity[index]);
		if (ret < 0) {
			break;
		} else {
			if (!strcmp(media->entity[index].name, ENTITY_VIDEO_CCDC_OUT_NAME))
				media->video =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_TVP514X_NAME))
				media->tvp5146 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_MT9T111_NAME))
				media->mt9t111 =  media->entity[index].id;
			else if (!strcmp(media->entity[index].name, ENTITY_CCDC_NAME))
				media->ccdc =  media->entity[index].id;
			printf("[%d]:%s\n", media->entity[index].id, media->entity[index].name);
		}
		index++;
	} while (ret == 0);

	if ((ret < 0) && (index <= 0)) {
		printf("Failed to enumerate entities OR no entity registered - %d\n",
				ret);
		return ret;
	}
	media->num_entities = index;
	printf("Total number of entities: %d\n", media->num_entities);

	return 0;
}


static int reset_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i;

	/* Open the Media device */
	ret  = media_device_open(media);
	if (ret < 0)
		return ret;

	printf("Resetting all links...\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			for(i = 0; i < media->entity[index].links; i++) {
				link.source.entity = links.links->source.entity;
				link.source.index = links.links->source.index;
				link.source.flags = MEDIA_PAD_FLAG_OUTPUT;
				link.sink.entity = links.links->sink.entity;
				link.sink.index = links.links->sink.index;
				link.sink.flags = MEDIA_PAD_FLAG_INPUT;
				link.flags = (link.flags & ~MEDIA_LINK_FLAG_ENABLED) |
					(link.flags & MEDIA_LINK_FLAG_IMMUTABLE);
				ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
				if(ret)
					break;
				links.links++;
			}
		}
	}
	media_device_close(media->media_fd);
	return 0;
}

static int setup_media_links(struct media_dev *media)
{
	struct media_link_desc link;
	struct media_links_enum links;
	int ret, index, i, input;

	printf("Enumerating links/pads for entities\n");
	for(index = 0; index < media->num_entities; index++) {
		links.entity = media->entity[index].id;
		links.pads = malloc(sizeof( struct media_pad_desc) * media->entity[index].pads);
		links.links = malloc(sizeof(struct media_link_desc) * media->entity[index].links);
		ret = ioctl(media->media_fd, MEDIA_IOC_ENUM_LINKS, &links);
		if (ret < 0) {
			printf("Error while enumeration links/pads - %d\n", ret);
			break;
		} else {
			if(media->entity[index].pads)
				printf("pads for entity %d=", media->entity[index].id);
			for(i = 0 ; i < media->entity[index].pads; i++) {
				printf("(%d %s) ", links.pads->index,
						(links.pads->flags & MEDIA_PAD_FLAG_INPUT) ?
						"INPUT" : "OUTPUT");
				links.pads++;
			}
			printf("\n");
			for(i = 0; i < media->entity[index].links; i++) {
				printf("[%d:%d]===>[%d:%d]",
						links.links->source.entity,
						links.links->source.index,
						links.links->sink.entity,
						links.links->sink.index);
				if(links.links->flags & MEDIA_LINK_FLAG_ENABLED)
					printf("\tACTIVE\n");
				else
					printf("\tINACTIVE \n");
				links.links++;
			}
			printf("\n");
		}
	}

	if (media->input_source != 0)
		input = media->mt9t111;
	else
		input = media->tvp5146;

	printf("Enabling link [tvp5146]===>[ccdc]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LINK_FLAG_ENABLED;
	link.source.entity = input;
	link.source.index = 0;	/* Only 1 pad */
	link.source.flags = MEDIA_PAD_FLAG_OUTPUT;

	link.sink.entity = media->ccdc;
	link.sink.index = 0; /* Sink pad of CCDC, 0 */
	link.sink.flags = MEDIA_PAD_FLAG_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret) {
		printf("failed to enable link between tvp514x and ccdc\n");
		return ret;
	} else {
		printf("[tvp514x]===>[ccdc]\tenabled\n");
	}
	/* Enable 'ccdc===>memory' link */
	printf("Enabling link [ccdc]===>[video_node]\n");
	memset(&link, 0, sizeof(link));

	link.flags |=  MEDIA_LINK_FLAG_ENABLED;
	link.source.entity = media->ccdc;
	link.source.index = 1; /* Source pad of CCDC: 1 */
	link.source.flags = MEDIA_PAD_FLAG_OUTPUT;

	link.sink.entity = media->video;
	link.sink.index = 0;
	link.sink.flags = MEDIA_PAD_FLAG_INPUT;

	ret = ioctl(media->media_fd, MEDIA_IOC_SETUP_LINK, &link);
	if(ret)
		printf("failed to enable link between ccdc and video node\n");
	else
		printf("[ccdc]===>[video_node]\tenabled\n");

	return ret;
}

static int set_subdev_format(struct media_dev *media,
				struct capture_dev *capture)
{
	struct v4l2_subdev_format fmt;
	int tvp_fd, ccdc_fd, ret;
	char subdev[20];

	/* TODO: Should be having some mechanism to select subdev */
	ccdc_fd = open("/dev/v4l-subdev2", O_RDWR);
	if(ccdc_fd == -1) {
		printf("failed to open %s\n", "/dev/v4l-subdev2");
		return -1;
	}
	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 0;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;
	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 1;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;
	ret = ioctl(ccdc_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return ret;
	}

#if 0
	/* Default to tvp5146 */
	if (media->input_source != 0)
		strcpy(subdev, "/dev/v4l-subdev8");
	else
		strcpy(subdev, "/dev/v4l-subdev9");
#endif
		strcpy(subdev, "/dev/v4l-subdev8");

	tvp_fd = open(subdev, O_RDWR);
	if(tvp_fd == -1) {
		printf("failed to open %s\n", subdev);
		return -1;
	}

	memset(&fmt, 0, sizeof(fmt));
	fmt.pad = 0;
	fmt.which = V4L2_SUBDEV_FORMAT_ACTIVE;
	fmt.format.code = V4L2_MBUS_FMT_UYVY8_2X8;
	fmt.format.width = capture->width;
	fmt.format.height = capture->height;
	fmt.format.colorspace = V4L2_COLORSPACE_SMPTE170M;
	fmt.format.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(tvp_fd, VIDIOC_SUBDEV_S_FMT, &fmt);
	if(ret) {
		printf("failed to set format on pad %x\n", fmt.pad);
		return ret;
	}

	printf("successfully format is set on all pad [WxH] - [%dx%d]\n",
			capture->width, capture->height);
	return ret;
}


static int  open_video_dev(const char *dev, int *capture_fd)
{
	/* Open the capture device */
	*capture_fd  = open((const char *) dev, O_RDWR);
	if (*capture_fd  <= 0) {
		printf("Cannot open = %s device\n", dev);
		return -1;
	}
	return 0;
}

static void close_video_dev(int capture_fd)
{
	/* close the device */
	close(capture_fd);
}

/*
 * Currently only return width and height.
 */
static int get_current_capture_format(int input_src, struct capture_dev *capture)
{
	struct v4l2_input input;
	v4l2_std_id std_id;
	struct v4l2_standard standard;
	int index;

	/* In case of MT9T111 we only support VGA resoltion */
	if (input_src != 0) {
		capture->width = IMG_WIDTH_VGA;
		capture->height = IMG_HEIGHT_VGA;
		return 0;
	}

	/* Get any active input */
	if (ioctl(capture->capture_fd, VIDIOC_G_INPUT, &index) < 0) {
		perror("VIDIOC_G_INPUT");
	}

	/* Enumerate input to get the name of the input detected */
	memset(&input, 0, sizeof(struct v4l2_input));
	input.index = index;
	if (ioctl(capture->capture_fd, VIDIOC_ENUMINPUT, &input) < 0) {
		perror("VIDIOC_ENUMINPUT");
	}

	printf("%s: Current Input: %s\n", CAPTURE_NAME, input.name);

	index = capture->tvp_input;

	if (ioctl(capture->capture_fd, VIDIOC_S_INPUT, &index) < 0) {
		perror("VIDIOC_S_INPUT");
	}
	memset(&input, 0, sizeof(struct v4l2_input));
	input.index = index;
	if (ioctl(capture->capture_fd, VIDIOC_ENUMINPUT, &input) < 0) {
		perror("VIDIOC_ENUMINPUT");
	}
	printf("%s: Input changed to: %s\n", CAPTURE_NAME, input.name);


	/* Detect the standard in the input detected */
	if (ioctl(capture->capture_fd, VIDIOC_QUERYSTD, &std_id) < 0) {
		perror("VIDIOC_QUERYSTD");
	}

	/* Get the standard*/
	if (ioctl(capture->capture_fd, VIDIOC_G_STD, &std_id) < 0) {
		/* Note when VIDIOC_ENUMSTD always returns EINVAL this
		   is no video device or it falls under the USB exception,
		   and VIDIOC_G_STD returning EINVAL is no error. */
		perror("VIDIOC_G_STD");
	}
	memset(&standard, 0, sizeof(standard));
	standard.index = 0;
	while (1) {
		if (ioctl(capture->capture_fd, VIDIOC_ENUMSTD, &standard) < 0) {
			perror("VIDIOC_ENUMSTD");
		}

		/* Store the name of the standard */
		if (standard.id & std_id) {
			printf("%s: Current standard: %s\n",
					CAPTURE_NAME, standard.name);
			break;
		}
		standard.index++;
	}

	capture->width = IMG_WIDTH_PAL_NTSC;
	if (!strcmp((char *)standard.name, "PAL"))
		capture->height = IMG_HEIGHT_PAL;
	else
		capture->height = IMG_HEIGHT_NTSC;

	return 0;
}

/*
 * This function initializes capture device. It selects an active input
 * and detects the standard on that input. It then allocates buffers in the
 * driver's memory space and mmaps them in the application space.
 */
static int capture_prepare_streaming(struct capture_dev *capture)
{
	struct v4l2_capability capability;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_format *fmt = &capture->capture_fmt;
	int ret, i, j;

	/* Check if the device is capable of streaming */
	ret = ioctl(capture->capture_fd, VIDIOC_QUERYCAP, &capability);
	if (ret < 0) {
		perror("VIDIOC_QUERYCAP");
		return ret;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING) {
		printf("%s: Capable of streaming\n", CAPTURE_NAME);
	} else {
		printf("%s: Not capable of streaming\n", CAPTURE_NAME);
		return -1;
	}

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture->capture_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}

	fmt->fmt.pix.width = capture->width;
	fmt->fmt.pix.height = capture->height;
	fmt->fmt.pix.pixelformat = DEF_PIX_FMT;

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture->capture_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT");
		return ret;
	}

	ret = ioctl(capture->capture_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT");
		return ret;
	}

	if (fmt->fmt.pix.pixelformat != DEF_PIX_FMT) {
		printf("%s: Requested pixel format not supported\n", CAPTURE_NAME);
		return -1;
	}

	printf("%s: Init done successfully\n\n", CAPTURE_NAME);
	return 0;

ERROR:
	for (j = 0; j < capture->num_bufs; j++)
		munmap(capture_buff_info[j].start,
				capture_buff_info[j].length);
	return -1;
}



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

        if (close(hCapture->fd) == -1) {
            Dmai_err1("Failed to close capture device (%s)\n", strerror(errno));
            ret = Dmai_EIO;
        }

        if (hCapture->userAlloc == FALSE && hBufTab) {
            for (bufIdx = 0;
                 bufIdx < BufTab_getNumBufs(hBufTab);
                 bufIdx++) {

                hCapBuf = BufTab_getBuf(hBufTab, bufIdx);
                capBufPtr = Buffer_getUserPtr(hCapBuf);

                if (munmap(capBufPtr - hCapture->topOffset,
                           Buffer_getSize(hCapBuf)) == -1) {
                    Dmai_err1("Failed to unmap capture buffer%d\n", bufIdx);
                    ret = Dmai_EFAIL;
                }
            }
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

        int i = 0, ret = 0, a, c, index;
        unsigned int loop_cnt = MAXLOOPCOUNT;

        struct media_dev media;
        struct capture_dev capture;
        struct display_dev display;
        struct timeval before, after, result;
    Capture_Handle              hCapture;

    assert(attrs);

    /* Allocate space for state object */
    hCapture = calloc(1, sizeof(Capture_Object));

    if (hCapture == NULL) {
        Dmai_err0("Failed to allocate space for Capture Object\n");
        return NULL;
    }

    /* User allocated buffers by default */
    hCapture->userAlloc = TRUE;

        memset(&media, 0, sizeof(struct media_dev));
        memset(&capture, 0, sizeof(struct capture_dev));
        memset(&display, 0, sizeof(struct display_dev));

        /* Setup default init for all devices */
        /* Media */
        media.input_source = 0; /* default to tvp5146 */
        /* Capture */
        capture.num_bufs = CAPTURE_MAX_BUFFER;

        for(i = 0; i < capture.num_bufs; i++) {
                capture_buff_info[i].start = NULL;
    }

	/*
	 * Initialization section
	 * In case of Media-Controller compliant device: Setup Links
	 * Initialize capture and display devices.
	 * Here one capture channel is opened and input and standard is
	 * detected on that channel.
	 * Display channel is opened with the same standard that is detected at
	 * capture channel.
	 * */
	/* Open the Media device */
	ret  = media_device_open(&media);
	if (ret < 0)
		return ret;

	ret = enumerate_all_entities(&media);
	if (ret < 0)
            return NULL;
	/* Setup Links */
	ret = setup_media_links(&media);
	if (ret < 0)
        return NULL;

	media_device_close(media.media_fd);

	/* Open the capture device */
	ret = open_video_dev((const char *)CAPTURE_DEVICE, &capture.capture_fd);
	if (ret < 0)
        return NULL;

        hCapture->fd = capture.capture_fd;
        if (hCapture->fd == -1) {
            Dmai_err2("Cannot open %s (%s)\n", attrs->captureDevice,
                                                      strerror(errno));
        cleanup(hCapture);
        return NULL;
    }


	ret = get_current_capture_format(media.input_source, &capture);
	if (ret < 0)
        return NULL;

	/*
	 * Now set the detected format at each pad
	 */
	ret = set_subdev_format(&media, &capture);
	if (ret < 0)
           return NULL;	

	ret = capture_prepare_streaming(&capture);
	if(ret < 0)
            return NULL;

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
                                0, attrs->colorSpace, TRUE) < 0) {
            Dmai_err1("Failed to intialize capture driver buffers on %s\n",
                      attrs->captureDevice);
            cleanup(hCapture);
            return NULL;
        }
    }

    hCapture->hBufTab = hBufTab;

	/* run section
	 * STEP2:
	 * Here display and capture channels are started for streaming. After
	 * this capture device will start capture frames into enqueued
	 * buffers and display device will start displaying buffers from
	 * the qneueued buffers */

	/* Start Streaming. on capture device */
	a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(capture.capture_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON");
    }
	printf("%s: Stream on...\n", CAPTURE_NAME);

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
    v4l2_std_id             id;
    struct v4l2_input       input;
    Int                     index;
    Int                     fd;
    struct v4l2_standard    std;

    assert(videoStdPtr);
    assert(attrs);

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

    /* Get any active input */
    if (ioctl(fd, VIDIOC_G_INPUT, &index) < 0) {
        Dmai_err0("VIDIOC_G_INPUT");
        return Dmai_EFAIL;
    }

    /* Display available video input */
    Dmai_dbg0("Available video input:\n");
    for (index=0;; index++) {

        input.index = index;
        if (ioctl(fd, VIDIOC_ENUMINPUT, &input) < 0) {
            if (errno == EINVAL || errno == ENOTTY)
                break;
        }

        Dmai_dbg1(" name=%s\n", input.name);
    }

    /* Set video input */
    switch (attrs->videoInput) {

        case Capture_Input_SVIDEO:
                Dmai_dbg0("Setting video input to SVIDEO\n");
                index = 1;
                break;
        case Capture_Input_COMPOSITE:
                Dmai_dbg0("Setting video input to COMPOSITE\n");
                index = 0;
                break;
        default:
                Dmai_err0("Unknown video input\n");
                return Dmai_EFAIL;
                break;
    }
        
    if (ioctl(fd, VIDIOC_S_INPUT, &index) < 0) {
        Dmai_err2("Failed VIDIOC_S_INPUT to index %d (%s)\n", 
                                    index, strerror(errno));
        return Dmai_EFAIL;
    }
 
    /* Display available video standards */
    Dmai_dbg0("Available video standard:\n");
    for (index=0;; index++) {

        std.frameperiod.numerator = 1;
        std.frameperiod.denominator = 0;
        std.index = index;

        if (ioctl(fd, VIDIOC_ENUMSTD, &std) < 0) {
            if (errno == EINVAL || errno == ENOTTY)
                break;
        }
        Dmai_dbg3(" name=%s, fps=%d/%d\n", std.name, 
                    std.frameperiod.denominator, std.frameperiod.numerator);
    }
    /* Detect the standard in the input detected */ 
    if (ioctl(fd, VIDIOC_QUERYSTD, &id) < 0) {
        Dmai_err0("VIDIOC_QUERYSTD");
        return Dmai_EFAIL;
    }

    /* Get current video standard */
    if (ioctl(fd, VIDIOC_G_STD, &id) < 0) {
        Dmai_err0("Failed VIDIOC_G_STD\n");
        return Dmai_EFAIL;
    }

    if (!hCapture) {
        close(fd);
    }

#if 0
    if (id & V4L2_STD_NTSC) {
       *videoStdPtr = VideoStd_D1_NTSC;
        Dmai_dbg0("Found NTSC std input\n");
    }
    else if (id & V4L2_STD_PAL) {
        *videoStdPtr = VideoStd_D1_PAL;
        Dmai_dbg0("Found PAL std input\n");
    }
    else if (id & V4L2_STD_525_60) {
        *videoStdPtr = VideoStd_480P;
        Dmai_dbg0("Found 525_60 std input\n");
    }
    else if (id & V4L2_STD_625_50) {
        *videoStdPtr = VideoStd_576P;
        Dmai_dbg0("Found 625_50 std input\n");
    }
    else {
        Dmai_err1("Unknown video standard on capture device %s\n",
                  attrs->captureDevice);
        return Dmai_EFAIL; 
    }
#endif


    //TODO: Currently supporting only D1 NTSC standard
    *videoStdPtr = VideoStd_D1_NTSC; 
    attrs->videoStd = *videoStdPtr;
    Dmai_dbg2("Capture input set to %s.  Standard: %d\n",
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

    /*
     * The used Flag as part of bufdesc is with respect to the use of buffer 
     * by the application. If Used = TRUE then the index is available for
     * storing new buffer information. This is set to FALSE once it contains
     * valid information about the buffer.
     */
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

