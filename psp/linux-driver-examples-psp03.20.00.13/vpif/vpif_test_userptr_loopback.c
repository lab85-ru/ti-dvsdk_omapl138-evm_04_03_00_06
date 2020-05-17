/*
 * vpif_userptr_loopback.c
 *
 * This is the sample to show DavinciHD capture and display functionality using
 * user pointer buffers. In this example, buffers are allocated using MMAP in
 * display driver and the mmapped address is used as USERPTR for USERPTR IO on
 * capture side.
 *
 * Flow: Capture -> Display
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
 /******************************************************************************
  Header File Inclusion
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/videodev2.h>
/******************************************************************************
 Macros
 NUM_BUFFER     : Changing the following will result different number of
		  instances of buf_info structure.
 BUFFER_HEIGHT	: This indicates maximum height of the image which is
		  captured or displayed.
 BUFFER_PITCH	: This indicates maximum widht of the image which is
		  captured or displayed.
 CAPTURE_DEVICE : This indicates device to be used for capture
 DISPLAY_DEVICE : This indicates device to be used for display
 MAXLOOPCOUNT	: This indicates number of frames to be captured and displayed
 OUTPUTPATH	: This indicates absolute path of the sysfs entry for
		  controlling output
 STDPATH	: This indicates absolute path of the sysfs entry for
		  controlling standard
 ******************************************************************************/
#define NUM_BUFFERS	4
#define BUFFER_PITCH	720
#define BUFFER_HEIGHT	576
#define CAPTURE_DEVICE0	"/dev/video0"
#define CAPTURE_DEVICE1	"/dev/video1"
#define DISPLAY_DEVICE	"/dev/video2"
#define MAXLOOPCOUNT	500
#define CAPTURE_INPUT0 "Composite"
#define CAPTURE_INPUT1 "S-Video"
#define CLEAR(x)	memset(&(x), 0, sizeof(x))
#define ALIGN(x, y)     (((x + (y - 1)) / y) * y)

/******************************************************************************
 Declaration
	Following structure is used to store information of the buffers which
	are allocated using VDCE driver and mmaped in the user space.
 ******************************************************************************/
struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

/******************************************************************************
 Globals
	Following variables stores file descriptors returned when opening
	capture, display and vdce device.
	capture_buff_info and display_buff_info stores mmaped buffer
	information of capture and display respectively.
	numbuffers is used to store number of buffers actually allocated by
	the driver.
	outputidx is used to store index of the output to be selected in
	display depending on the input detecting on the capture.
	capture_std is used to store detected standard.
 ******************************************************************************/

static struct buf_info capture_buff_info[NUM_BUFFERS];
static struct buf_info display_buff_info[NUM_BUFFERS];

/******************************************************************************
				Function Definitions
 ******************************************************************************/
static int initCapture(int *, int *, struct v4l2_format *);
static int startCapture(int *, struct v4l2_buffer *, int);
static int stopCapture(int *);
static int releaseCapture(int *, int *);
static int initDisplay(int *, int *, struct v4l2_format *fmt);
static int releaseDisplay(int *, int);
static int startDisplay(int *);
static int stopDisplay(int *);
static int allocateBuffers(int *, int);

v4l2_std_id input_std_id = V4L2_STD_NTSC;
char *input_device;
char *input_name;

/* print frame number */
int print_fn = 1;
/* stress test */
int stress_test;
#define BYTESPERLINE 720

/* We trick this as PAL buffer size since on the display side
 * we are using MMAP and we want to align on the cbcr offset
 * But user ptr doesn't have this restriction as such
 */
int sizeimage = BYTESPERLINE*576*2;
int kernel_buf_size = BUFFER_PITCH * BUFFER_HEIGHT * 2;

int app_main(void);

/*=====================initCapture========================*/
/* This function initializes capture device. It detects   *
 * first input connected on the channel-0 and detects the *
 * standard on that input. It, then, enqueues all the     *
 * buffers in the driver's incoming queue.		  */
static int initCapture(int *capture_fd, int *numbuffers,
					struct v4l2_format *fmt)
{
	int mode = O_RDWR, ret, i;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_buffer buf;
	struct v4l2_input input;
	int input_idx;
	struct v4l2_standard standard;
	v4l2_std_id std;
	int found = 0;

	/* Open the channel-0 capture device */
	*capture_fd = open((const char *)input_device, mode);
	if (capture_fd <= 0) {
		printf("Cannot open = %s device\n", input_device);
		return -1;
	}

	/* Enumerate inputs to get the name of the input chosen by user*/
	input.index = 0;
	while (1) {
		ret = ioctl(*capture_fd, VIDIOC_ENUMINPUT, &input);
		if (ret < 0) {
			perror("VIDIOC_ENUMINPUT\n");
			return -1;
		}
		if (!strcmp(input.name, input_name)) {
			found = 1;
			break;
		}
	}

	if (!found) {
		printf("Input %s not found\n", input_name);
		return -1;
	}

	/* Set input
	 * VIDIOC_G_INPUT ioctl detects the inputs connected. It returns
	 * error if no inputs are connected. Otherwise it returns index of
	 * the input connected. */

	ret = ioctl(*capture_fd, VIDIOC_S_INPUT, &input.index);
	if (ret < 0) {
		perror("VIDIOC_G_INPUT\n");
		return -1;
	}

	/* check if input is selected correctly */
	ret = ioctl(*capture_fd, VIDIOC_G_INPUT, &input_idx);
	if (ret < 0) {
		perror("VIDIOC_G_INPUT\n");
		return -1;
	}

	if (input_idx != input.index) {
		printf("Couldn't set input correctly\n");
		return -1;
	}
	/* Set the user selected standard */

	found = 0;
	/* Enumerate standard to get the name of the standard detected */
	standard.index = 0;
	do {
		ret = ioctl(*capture_fd, VIDIOC_ENUMSTD, &standard);
		if (ret < 0) {
			perror("VIDIOC_ENUM_STD failed\n");
			return -1;
		}
		printf("standard.name = %s\n", standard.name);
		if (standard.id & input_std_id) {
			printf("Found standard support in the driver\n");
			found = 1;
			break;
		}
		standard.index++;
	} while (1);

	if (!found) {
		printf("Driver doesn't support the standard\n");
		return -1;
	}

	ret == ioctl(*capture_fd, VIDIOC_S_STD, &input_std_id);
	if (ret < 0) {
		perror("VIDIOC_S_STD failed\n");
		return -1;
	}

	/* Detect the standard in the input detected */
	ret = ioctl(*capture_fd, VIDIOC_QUERYSTD, &std);

	if (ret < 0) {
		perror("VIDIOC_QUERYSTD\n");
		return -1;
	}

	if (std & V4L2_STD_NTSC)
		printf("Input video standard is NTSC.\n");
	else if (std & V4L2_STD_PAL)
		printf("Input video standard is PAL.\n");
	else if (std & V4L2_STD_PAL_M)
		printf("Input video standard is PAL-M.\n");
	else if (std & V4L2_STD_PAL_N)
		printf("Input video standard is PAL-N.\n");
	else if (std & V4L2_STD_SECAM)
		printf("Input video standard is SECAM.\n");
	else if (std & V4L2_STD_PAL_60)
		printf("Input video standard to PAL60.\n");
	else
		return -1;

	/* Set format */
	CLEAR(*fmt);
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt->fmt.pix.bytesperline = BYTESPERLINE;
	fmt->fmt.pix.sizeimage = sizeimage;
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_NV16;
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(*capture_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT failed\n");
		return -1;
	}

	CLEAR(*fmt);
	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(*capture_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT failed\n");
		return -1;
	}

	/* Buffer allocation
	 * Informing the driver that user pointer buffer exchange
	 * mechanism will be used.
	 * HERE count = number of buffer to be allocated.
	 * type = type of device for which buffers are to be allocated.
	 * memory = type of the buffers requested i.e. driver allocated or
	 * user pointer */
	reqbuf.count = *numbuffers;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_USERPTR;

	ret = ioctl(*capture_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("cannot allocate memory\n");
		return -1;
	}
	/* Store the number of buffers actually allocated */
	*numbuffers = reqbuf.count;

	return 0;
}

/*=====================startCapture========================*/
/* This function starts streaming on the capture device	   */
static int startCapture(int *capture_fd, struct v4l2_buffer *buf, int stream_on)
{
	int a = V4L2_BUF_TYPE_VIDEO_CAPTURE, ret;


	printf("startCapture, stream_on = %d\n", stream_on);
	printf("startCapture, buf->index = %d\n", buf->index);
	printf("startCapture, buf->m.userptr = %p\n", buf->m.userptr);
	ret = ioctl(*capture_fd, VIDIOC_QBUF, buf);
	if (ret < 0) {
		perror("VIDIOC_QBUF failed\n");
		return -1;
	}

	if (stream_on) {
		/* Here type of device to be streamed on is
		 * required to be passed
		 */
		ret = ioctl(*capture_fd, VIDIOC_STREAMON, &a);
		if (ret < 0) {
			perror("VIDIOC_STREAMON\n");
			return -1;
		}
	}
	return 0;
}

/*=====================stopCapture========================*/
/* This function stops streaming on the capture device	  */
static int stopCapture(int *capture_fd)
{
	int ret, a = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	/* Here type of device to be streamed off is required to be passed */
	ret = ioctl(*capture_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		return -1;
	}
	return 0;
}

/*=====================releaseCapture========================*/
/* This function un-maps all the mmapped buffers of capture  *
 * and closes the capture file handle			     */
static int releaseCapture(int *capture_fd, int *numbuffers)
{
	close(*capture_fd);
	*capture_fd = 0;
	return 0;
}

/*=====================initDisplay========================*/
/* This function initializes display device. It sets      *
 * output and standard on channel-2. These output and     *
 * standard are same as those used in capture device. *
 * It, then, enqueues all the buffers in the driver's     *
 * incoming queue.*/
static int initDisplay(int *display_fd, int *numbuffers,
						struct v4l2_format *fmt)
{
	int mode = O_RDWR;
	struct v4l2_buffer buf;
	int ret, i = 0;
	struct v4l2_requestbuffers reqbuf;
	struct v4l2_output output;
	int temp_output;
	struct v4l2_standard standard;
	v4l2_std_id std_id;
	int found = 0;

	/* Open the channel-2 display device */
	*display_fd = open((const char *)DISPLAY_DEVICE, mode);
	if (*display_fd <= -1) {
		printf("Cannot open = %s\n", DISPLAY_DEVICE);
		return -1;
	}

	/* Enumerate outputs */
	output.type = V4L2_OUTPUT_TYPE_ANALOG;
	output.index = 0;
	while ((ret = ioctl(*display_fd, VIDIOC_ENUMOUTPUT, &output) == 0)) {
		printf("output.name = %s\n", output.name);
		if (!strcmp(output.name, input_name)) {
			found = 1;
			break;
		}
		output.index++;
	}

	if (!found) {
		printf("Unable to find output name matching input name\n",
								input_name);
		return -1;
	}

	/* Set output */
	ret = ioctl(*display_fd, VIDIOC_S_OUTPUT, &output.index);
	if (ret < 0) {
		perror("VIDIOC_S_OUTPUT failed\n");
		return -1;
	}

	ret = ioctl(*display_fd, VIDIOC_G_OUTPUT, &temp_output);
	if (ret < 0) {
		perror("VIDIOC_S_OUTPUT failed\n");
		return -1;
	}

	if (temp_output != output.index) {
		printf("Couldn't set output index %d at display\n",
							output.index);
		return -1;
	}

	/* Set standard */
	found = 0;
	/* Enumerate standard to get the name of the standard detected */
	standard.index = 0;
	do {
		ret = ioctl(*display_fd, VIDIOC_ENUMSTD, &standard);
		if (ret < 0) {
			perror("VIDIOC_ENUM_STD failed\n");
			return -1;
		}

		if (standard.id & input_std_id) {
			printf("Found standard support in the driver\n");
			found = 1;
			break;
		}
		standard.index++;
	} while (1);

	ret == ioctl(*display_fd, VIDIOC_S_STD, &input_std_id);
	if (ret < 0) {
		perror("VIDIOC_S_STD failed\n");
		return -1;
	}

	printf("Set output standard to match with input capture standard\n");

	printf("Setting display format\n");

	/* Set format */
	CLEAR(*fmt);
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	fmt->fmt.pix.bytesperline = BYTESPERLINE;
	fmt->fmt.pix.sizeimage = sizeimage;
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_NV16;
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(*display_fd, VIDIOC_S_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_S_FMT failed\n");
		return -1;
	}

	CLEAR(*fmt);
	fmt->type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*display_fd, VIDIOC_G_FMT, fmt);
	if (ret < 0) {
		perror("VIDIOC_G_FMT\n");
		return -1;
	}

	/* Buffer allocation. Use MMAP at the display and use the
	 * mmapped address as ptr at the capture side
	 */
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.count = *numbuffers;
	reqbuf.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(*display_fd, VIDIOC_REQBUFS, &reqbuf);
	if (ret < 0) {
		perror("cannot allocate memory\n");
		return -1;
	}
	/* Store the numbfer of buffers allocated */
	*numbuffers = reqbuf.count;

	/* It is better to zero all the members of buffer structure */
	memset(&buf, 0, sizeof(buf));

	/* Mmap the buffers
	 * To access driver allocated buffer in application space, they have
	 * to be mmapped in the application space using mmap system call */
	for (i = 0; i < reqbuf.count; i++) {
		/* Query physical address of the buffers */
		buf.index = i;
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(*display_fd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QUERYCAP\n");
			return -1;
		}

		/* Mmap the buffers in application space */
		display_buff_info[i].length = buf.length;
		display_buff_info[i].index = i;
		display_buff_info[i].start =
		    mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
			 MAP_SHARED, *display_fd, buf.m.offset);

		if ((unsigned int)display_buff_info[i].start == MAP_SHARED) {
			printf("Cannot mmap = %d buffer\n", i);
			return -1;

		}
		/* It is better to zero buffers */
		memset(display_buff_info[i].start, 0x80,
		       display_buff_info[i].length);

		printf("display_buff_info[%d].length = %d\n", i,
						display_buff_info[i].length);
		printf("display_buff_info[%d].index = %d\n", i,
						display_buff_info[i].index);
		printf("display_buff_info[%d].start = %p\n", i,
						display_buff_info[i].start);
	}

	/* Enqueue buffers
	 * Before starting streaming, all the buffers needs to be en-queued
	 * in the driver incoming queue. */
	/* Enqueue buffers */
	for (i = 0; i < reqbuf.count; i++) {
		buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(*display_fd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			perror("VIDIOC_QBUF\n");
			return -1;
		}
	}

	return 0;
}

/*=====================releaseDisplay========================*/
/* This function un-maps all the mmapped buffers for display *
 * and closes the capture file handle			     */
static int releaseDisplay(int *display_fd, int numbuffers)
{
	int i;
	for (i = 0; i < numbuffers; i++) {
		munmap(display_buff_info[i].start, display_buff_info[i].length);
		display_buff_info[i].start = NULL;
	}
	close(*display_fd);
	*display_fd = 0;
	return 0;
}

/*=====================startDisplay========================*/
/* This function starts streaming on the display device	   */
static int startDisplay(int *display_fd)
{
	int a = 0, ret;
	/* Here type of device to be streamed on is required to be passed */
	a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*display_fd, VIDIOC_STREAMON, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMON\n");
		return -1;
	}
	return 0;
}

/*=====================stopDisplay========================*/
/* This function stops streaming on the display device	  */
static int stopDisplay(int *display_fd)
{
	int ret, a = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*display_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("display:VIDIOC_STREAMOFF\n");
		return -1;
	}
	return 0;
}

/*=====================app_main===========================*/
int app_main()
{
	int i = 0;
	int counter = 0;
	int ret = 0;
	struct v4l2_buffer buf1, buf2;
	unsigned long temp;
	struct v4l2_format capture_fmt;
	struct v4l2_format display_fmt;
	int capture_fd, display_fd, vdce_fd;
	int capture_numbuffers = NUM_BUFFERS, display_numbuffers = NUM_BUFFERS;
	char outputname[15];
	char stdname[15];

	for (i = 0; i < NUM_BUFFERS; i++)
		display_buff_info[i].start = NULL;

	/* open display channel */
	ret = initDisplay(&display_fd, &display_numbuffers, &display_fmt);
	if (ret < 0) {
		printf("Error in opening display device\n");
		return ret;
	}

	/* STEP2:
	 * Initialization section
	 * Initialize capture and display devices.
	 * Here one capture channel is opened and input and standard is
	 * detected on thatchannel.
	 * Display channel is opened with the same standard that is detected at
	 * capture channel.
	 * */

	/* open capture channel 0 or 1*/
	ret = initCapture(&capture_fd, &capture_numbuffers, &capture_fmt);
	if (ret < 0) {
		printf("Error in opening capture device for channel 0\n");
		return ret;
	}

	/* STEP3:
	 * Here display and capture channels are started for streaming. After
	 * this capture device will start capture frames into enqueued
	 * buffers and display device will start displaying buffers from
	 * the qneueued buffers */

	/* start display */
	ret = startDisplay(&display_fd);
	if (ret < 0) {
		printf("Error in starting display\n");
		return ret;
	}



	/* One buffer is dequeued from display and capture channels.
	 * Capture buffer will be exchanged with display buffer.
	 * All two buffers are put back to respective channels.
	 * This sequence is repeated in loop.
	 * After completion of this loop, channels are stopped.
	 * */
	memset(&buf1, 0, sizeof(buf1));
	memset(&buf2, 0, sizeof(buf2));

	buf1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf1.memory = V4L2_MEMORY_USERPTR;

	buf2.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf2.memory = V4L2_MEMORY_MMAP;

	while (1) {

		/* DQ a buffer from the display */
		ret = ioctl(display_fd, VIDIOC_DQBUF, &buf2);
		if (ret < 0) {
			perror("capture VIDIOC_DQBUF\n");
			return -1;
		}

		buf1.length = sizeimage;
		buf1.index = buf2.index;
		buf1.m.userptr =
			(unsigned long)display_buff_info[buf2.index].start;

		if (counter < 2) {
			if (startCapture(&capture_fd, &buf1, counter) < 0) {
				printf("startCapture failed\n");
				return -1;
			}
			counter++;
			continue;
		} else {
			ret = ioctl(capture_fd, VIDIOC_QBUF, &buf1);
			if (ret < 0) {
				perror("capture VIDIOC_QBUF\n");
				return -1;
			}
		}

		/* Get filled buffer from capture and
		 * queue it back to display */
		ret = ioctl(capture_fd, VIDIOC_DQBUF, &buf1);
		if (ret < 0) {
			perror("capture VIDIOC_DQBUF failed\n");
			return -1;
		}

		buf2.index = buf1.index;
		ret = ioctl(display_fd, VIDIOC_QBUF, &buf2);
		if (ret < 0) {
			perror("display VIDIOC_QBUF failed\n");
			return -1;
		}

		counter++;
		if (print_fn)
			printf("time:%lu    frame:%u\n",
				(unsigned long)time(NULL), counter);

		if (!stress_test && counter >= MAXLOOPCOUNT)
			break;

	}

	/* stop capturing for channel 0 */
	ret = stopCapture(&capture_fd);
	if (ret < 0) {
		printf("Error in stopping capturing for channel 0\n");
		return ret;
	}

	/* stop display */
	ret = stopDisplay(&display_fd);
	if (ret < 0) {
		printf("Error in stopping display\n");
		return ret;
	}

	/* close capture channel 0 */
	ret = releaseCapture(&capture_fd, &capture_numbuffers);
	if (ret < 0) {
		printf("Error in closing capture device\n");
		return ret;
	}
	/* Free section
	 * Here channels for capture and display are close.
	 * */
	/* open display channel */
	ret = releaseDisplay(&display_fd, display_numbuffers);
	if (ret < 0) {
		printf("Error in closing display device\n");
		return ret;
	}
	return ret;
}

void menu()
{
	printf("vpif_userptr_loopback -c <channel> -s <stress test>");
	printf(" -p <print frame number>, -m <ntsc or pal>\n");
}

int main(int argc, char *argv[])
{
	int ret = 0, d, index;
	char shortoptions[] = "s:c:p:m:";
	/* 0 or 1 */
	static int channel_no;
	/* 0 - NTSC, 1 - PAL */
	static int input_std = V4L2_STD_NTSC;

	input_device = CAPTURE_DEVICE0;
	input_name = CAPTURE_INPUT0;
	stress_test = 0;

	for (;;) {
		d = getopt_long(argc, argv, shortoptions, (void *)NULL, &index);
		if (-1 == d)
			break;
		switch (d) {
		case 'm':
			input_std = atoi(optarg);
			if (input_std) {
				input_std_id = V4L2_STD_PAL;
				sizeimage = 720*576*2;
			}
			break;
		case 's':
			stress_test = atoi(optarg);
			break;
		case 'p':
			print_fn = atoi(optarg);
			break;
		case 'c':
			channel_no = atoi(optarg);
			if (channel_no == 1) {
				input_device = CAPTURE_DEVICE1;
				input_name = CAPTURE_INPUT1;
			}
			break;
		default:
			menu();
			exit(1);
		}
	}

	app_main();
	return 0;
}
