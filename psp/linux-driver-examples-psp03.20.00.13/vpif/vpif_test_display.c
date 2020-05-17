/*
 * vpif_display.c
 *
 * This is the sample to show OMAP-L138 display functionality using
 * memory mapped buffers. It accepts standard and output type as parameters.
 * Colour bars are displayed in the output device in the selected standard
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
 MAX_BUFFER     : Changing the following will result different number of
		  instances of buf_info structure.
 BUFFER_HEIGHT	: This indicates maximum height of the image which is
		  captured or displayed.
 BUFFER_PITCH	: This indicates maximum widht of the image which is
		  captured or displayed.
 DISPLAY_DEVICE : This indicates device to be used for display
 MAXLOOPCOUNT	: This indicates number of frames to be captured and displayed
 ******************************************************************************/
#define MAX_BUFFER	3
#define DISPLAY_DEVICE	"/dev/video2"
#define BUFFER_PITCH	720
#define BUFFER_HEIGHT	576
#define MAXLOOPCOUNT	500
#define DISPLAY_OUTPUT0 "Composite"
#define DISPLAY_OUTPUT1 "S-Video"
#define CLEAR(x)	memset(&(x), 0, sizeof(x))
#define ALIGN(x, y)     (((x + (y - 1)) / y) * y)

/******************************************************************************
Declaration
	Following structure is used to store information of the buffers which
	are mmapped.
 ******************************************************************************/
struct buf_info {
	int index;
	unsigned int length;
	char *start;
};

/******************************************************************************
Globals
	Following variables stores file descriptors returned when opening
	capture and display device.
	capture_buff_info and display_buff_info stores mmaped buffer
	information of capture and display respectively.
	capture_numbuffers, display_numbuffers is used to store number of
	buffers actually allocated by the driver.
	outputidx is used to store index of the output to be selected in
	display depending on the input detecting on the capture.
	capture_std used to store detected standard.
 ******************************************************************************/
static struct buf_info display_buff_info[MAX_BUFFER];
/******************************************************************************
			Function Definitions
 ******************************************************************************/
static int initDisplay(int *display_fd, int *numbuffers,
						struct v4l2_format *fmt);
static int releaseDisplay(int *, int);
static int startDisplay(int *);
static int stopDisplay(int *);
static void *getDisplayBuffer(int *);
static int putDisplayBuffer(int *display_fd, int numbuffers, void *addr);

v4l2_std_id output_std_id = V4L2_STD_NTSC;
char *outputname;

/* print frame number */
int print_fn = 1;

FILE *file_fp = NULL;

#define BYTESPERLINE 720

int sizeimage = BYTESPERLINE * 480 * 2;
int dispheight, disppitch, dispwidth;
int kernel_buf_size = ALIGN(BUFFER_PITCH * BUFFER_HEIGHT * 2, 4096);

int app_main();
void fill_lines(void);
void color_bar(unsigned char *addr, int pitch, int h, int size, int order);

/*=====================initDisplay========================*/
/* This function initializes display device. It sets      *
 * output and standard on channel-2. These output and     *
 * standard are same as those detected in capture device. *
 * It, then, allocates buffers in the driver's memory     *
 * space and mmaps them in the application space	  */
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
	if (*display_fd <= 0) {
		printf("Cannot open %s\n", DISPLAY_DEVICE);
		return -1;
	}

	/* Enumerate outputs */
	output.type = V4L2_OUTPUT_TYPE_ANALOG;
	output.index = 0;
	while ((ret = ioctl(*display_fd, VIDIOC_ENUMOUTPUT, &output) == 0)) {
		printf("output.name = %s\n", output.name);
		if (!strcmp(output.name, outputname)) {
			found = 1;
			break;
		}
		output.index++;
	}

	if (!found) {
		printf("Unable to find output name matching input name\n",
								outputname);
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


		if (standard.id & output_std_id) {
			printf("Found standard support in the driver\n");
			found = 1;
			break;
		}
		standard.index++;
	} while (1);

	ret == ioctl(*display_fd, VIDIOC_S_STD, &output_std_id);
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

	dispheight = fmt->fmt.pix.height;
	disppitch = fmt->fmt.pix.bytesperline;
	dispwidth = fmt->fmt.pix.width;
	sizeimage = fmt->fmt.pix.sizeimage;

	/* Buffer allocation
	 * Buffer can be allocated either from display driver or
	 * user pointer can be used
	 */
	/* Request for 3 buffers. As far as Physically contiguous
	 * memory is available, driver can allocate as many buffers as
	 * possible. If memory is not available, it returns number of
	 * buffers it has allocated in count member of reqbuf.
	 * HERE count = number of buffer to be allocated.
	 * type = type of device for which buffers are to be allocated.
	 * memory = type of the buffers requested i.e. driver allocated or
	 * user pointer */
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

	/* Fill up the lines array with appropriate colors values */
	fill_lines();

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
		/* Fill up the buffers with the values. */
		color_bar(display_buff_info[i].start, disppitch, dispheight,
			  sizeimage, 0);

	}

	return 0;
}

/*=====================releaseDisplay========================*/
/* This function un-maps all the mmapped buffers of display  *
 * and closes the display file handle			     */
static int releaseDisplay(int *display_fd, int numbuffers)
{
	int i;
	/* Un-map buffers */
	for (i = 0; i < numbuffers; i++) {
		munmap(display_buff_info[i].start, display_buff_info[i].length);
		display_buff_info[i].start = NULL;
	}
	/* Close the file handle */
	close(*display_fd);
	*display_fd = 0;
	return 0;
}

/*=====================startDisplay========================*/
/* This function starts streaming on the display device	   */
static int startDisplay(int *display_fd)
{
	int a = V4L2_BUF_TYPE_VIDEO_OUTPUT, ret;
	/* Here type of device to be streamed on is required to be passed */
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
	/* Here type of device to be streamed off is required to be passed */
	ret = ioctl(*display_fd, VIDIOC_STREAMOFF, &a);
	if (ret < 0) {
		perror("VIDIOC_STREAMOFF\n");
		return -1;
	}
	return 0;
}

/*=====================getDisplayBuffer====================*/
/* This function de-queues displayed empty buffer from the *
 * display device's outgoing queue. 			   */
static void *getDisplayBuffer(int *display_fd)
{
	int ret;
	struct v4l2_buffer buf;
	/* It is better to zero members of v4l2_buffer structure */
	memset(&buf, 0, sizeof(buf));
	/* Dequeue buffer
	 * VIDIOC_DQBUF ioctl de-queues a displayed empty buffer from driver.
	 * This call can be blocking or non blocking. For blocking call, it
	 * blocks untill an empty buffer is available. For non-blocking call,
	 * it returns instantaneously with success or error depending on
	 * empty buffer is available or not. */
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	ret = ioctl(*display_fd, VIDIOC_DQBUF, &buf);
	if (ret < 0) {
		perror("VIDIOC_DQBUF\n");
		return NULL;
	}
	return display_buff_info[buf.index].start;
}

/*=====================putDisplayBuffer====================*/
/* This function en-queues a buffer, which contains frame  *
 * to be displayed, into the display device's incoming     *
 * queue.						   */
static int putDisplayBuffer(int *display_fd, int numbuffers, void *addr)
{
	struct v4l2_buffer buf;
	int i, index = 0;
	int ret;
	if (addr == NULL)
		return -1;

	/* It is better to zero members of v4l2_buffer structure */
	memset(&buf, 0, sizeof(buf));

	/* Find index of the buffer whose address is passed as the argument */
	for (i = 0; i < numbuffers; i++) {
		if (addr == display_buff_info[i].start) {
			index = display_buff_info[i].index;
			break;
		}
	}

	if (i == numbuffers)
		return -1;

	/* Enqueue the buffer */
	buf.m.offset = (unsigned long)addr;
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = index;
	ret = ioctl(*display_fd, VIDIOC_QBUF, &buf);
	if (ret < 0) {
		perror("VIDIOC_QBUF\n");
	}
	return ret;
}

/* Following array keeps track of Y and C data for 4 colors of 240 pixels.
 * First index is for colors, second index is for Y and C data and third
 * index is for pixels. These values are used in filling up the buffers.
 */
unsigned char lines[4][2][240];

/*
 * Following function fills the lines array will Y and CbCr values for 4 colors.
 * These values are used in filling up the display buffer.
 */
void fill_lines(void)
{
	unsigned char CVal[4][2] = { {0x5A, 0xF0}, {0x36, 0x22},
	{0xF0, 0x6E}, {0x10, 0x92}
	};

	int i, j, k;
	/* Copy Y data for all 4 colors in the array */
	memset(lines[0][0], 0x51, 240);
	memset(lines[1][0], 0x91, 240);
	memset(lines[2][0], 0x29, 240);
	memset(lines[3][0], 0xD2, 240);
	/* Copy C data for all 4 Colors in the array */
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 2; j++) {
			for (k = 0 + j; k < 240; k += 2)
				lines[i][1][k] = CVal[i][j];
		}
	}
}

void color_bar(unsigned char *addr, int pitch, int h, int size, int order)
{
	unsigned char *ptrY = addr;
	unsigned char *ptrC = addr + pitch * (size / (pitch * 2));
	unsigned char *tempY, *tempC;
	int i, j;

	/* Calculate the starting offset from where Y and C data should
	 * should start. */
	tempY = ptrY + pitch * 160 + 240 + order * pitch;
	tempC = ptrC + pitch * 160 + 240 + order * pitch;
	/* Fill all the colors in the buffer */
	for (j = 0; j < 4; j++) {
		for (i = 0; i < 40; i++) {
			memcpy(tempY, lines[j][0], 240);
			memcpy(tempC, lines[j][1], 240);
			tempY += pitch;
			tempC += pitch;
			if (tempY > (ptrY + pitch * 320 + 240 + pitch)) {
				tempY = ptrY + pitch * 160 + 240;
				tempC = ptrC + pitch * 160 + 240;
			}
		}
	}

}

/*=====================app_main===========================*/
int app_main()
{
	int i = 0;
	void *displaybuffer;
	int counter = 0;
	int ret = 0;
	struct v4l2_format display_fmt;
	int display_chroma_offset;
	int display_fd;
	char stdname[15];
	int display_numbuffers = MAX_BUFFER;

	for (i = 0; i < MAX_BUFFER; i++) {
		display_buff_info[i].start = NULL;
	}

	/* STEP1:
	 * Initialization section
	 * Initialize display devices.
	 * Display channel is opened with the same standard that is detected at
	 * capture channel. same output name as input
	 * */

	/* open display channel */
	ret = initDisplay(&display_fd, &display_numbuffers, &display_fmt);
	if (ret < 0) {
		printf("Error in opening display device\n");
		return ret;
	}
	printf(" Display initialized\n");
	/* run section
	 * STEP2:
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
	printf(" Display started \n");

	while (1) {
		/* get display buffer */
		displaybuffer = getDisplayBuffer(&display_fd);
		if (NULL == displaybuffer) {
			printf("Error in get display buffer\n");
			return ret;
		}


		/* Process it
		   In this example, the "processing" is putting a horizontally
		   moving color bars with changing starting line of display.
		 */
		color_bar(displaybuffer, disppitch, dispheight,
			  sizeimage, counter % 160);

		/* put output buffer into display queue */
		ret = putDisplayBuffer(&display_fd, display_numbuffers,
				       displaybuffer);
		if (ret < 0) {
			printf("Error in put display buffer\n");
			return ret;
		}

		counter++;

		if (print_fn)
			printf("time:%lu    frame:%u\n", (unsigned long)time(NULL), counter);

		if (counter >= MAXLOOPCOUNT)
			break;
	}

	/* stop display */
	ret = stopDisplay(&display_fd);
	if (ret < 0) {
		printf("Error in stopping display\n");
		return ret;
	}

	/* Free section
	 * Here channel for display is closed.
	 * */
	ret = releaseDisplay(&display_fd, display_numbuffers);
	if (ret < 0) {
		printf("Error in closing display device\n");
		return ret;
	}
	return ret;
}

void menu()
{
	printf("display -o <output number> -p <print frame");
	printf(" number, -m <ntsc or pal>\n");
}

/******************************************************************************
					Program Main
*******************************************************************************/
int main(int argc, char *argv[])
{
	int ret = 0, d, index;
	char shortoptions[] = "p:m:o:";
	/* 0 or 1 */
	static int output_no;
	/* 0 - NTSC, 1 - PAL */
	static int output_std = V4L2_STD_NTSC;
	outputname = DISPLAY_OUTPUT0;

	for (;;) {
		d = getopt_long(argc, argv, shortoptions, (void *)NULL, &index);
		if (-1 == d)
			break;
		switch (d) {
		case 'm':
			output_std = atoi(optarg);
			if (output_std) {
				output_std_id = V4L2_STD_PAL;
				sizeimage = BYTESPERLINE * 576 * 2;
			}
			break;
		case 'p':
			print_fn = atoi(optarg);
			break;
		case 'o':
			output_no = atoi(optarg);
			if (output_no == 1) {
				outputname = DISPLAY_OUTPUT1;
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
