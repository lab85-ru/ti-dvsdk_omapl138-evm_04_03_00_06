/*
  *  Copyright 2006 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */
/*
 *  ======== ijpegdec.h ========
 *  IJPEGDEC Interface Header
 */
#ifndef IJPEGDEC_
#define IJPEGDEC_

#include <ti/bios/include/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>                        
#include "ti/xdais/dm/iimgdec1.h"

typedef enum

{
	JPEGDEC_DEFAULT_CHROMA_FORMAT = 0
}Chroma_format;



/* JPEG decoder Error status messages */
typedef enum{
    
    
	JPEGDEC_SUCCESS = 0,					/*Successful decoding*/

	
	JPEGDEC_ERROR_UNSUPPORTED_FORMAT = 1,    /*SOI not found*/

	/* I/P image is progressive , but progressiveDecFlag of Dynamicparams 
	   is not set So heap required for decoding progressive image is not 
	   allocated */

	JPEGDEC_ERROR_PROG_MEM_ALOC = 2,	
	
	JPEGDEC_ERROR_NOT_BASELINE_INTL = 3,	/* Image is not sequential */
 
	JPEGDEC_ERROR_DISPLAY_WIDTH= 4,			/* Invalid display width (displaywidth < imagewidth)*/
	
	JPEGDEC_ERROR_VLD= 5,					/* Error in VLD */

	JPEGDEC_ERROR_SCAN_FREQ = 6,			/* Inavlid Scan frequency */

	JPEGDEC_ERROR_RST_MARKER = 7,			/* Missing Restart Marker */

	JPEGDEC_ERROR_MISSING_MARKER = 8,		/* Missing either SOS, SOF , DHT or DQT marker */

	JPEGDEC_ERROR_BAD_MARKER_LENGTH= 9,		/* Invalid marker length         */
 
	JPEGDEC_END_OF_IMAGE= 10,				/* We reached end of picture     */

	JPEGDEC_ERROR_BAD_DQT= 11,				/* Error in Quantization table   */
 
	JPEGDEC_ERROR_DHT_ERROR= 12,			/* Error in huffman table  */

    JPEGDEC_ERROR_BAD_DRI_LEN= 13,			/* Bad DRI length */

	JPEGDEC_ERROR_SOS_NO_SOF= 14,			/* Invalid JPEG file structure: SOS before SOF */

	JPEGDEC_ERROR_SOF_DUPLICATE= 15,		/* Invalid JPEG file structure: two SOF markers */
 
	JPEGDEC_ERROR_BAD_SOF_DATA = 16,		/* Bad SOF Marker length or component  */

	JPEGDEC_ERROR_SOS_COMP_ID= 17,			/* Invalid component ID in SOS Marker*/
 
	JPEGDEC_ERROR_BAD_SOS_LEN= 18,			/* Invalid length of SOS or Bad component numbers*/
 
	JPEGDEC_ERROR_SOS_INVALID= 19,			/* SOS marker is invalid */
 
	JPEGDEC_ERROR_BAD_PROGRESSION= 20,		/* Invalid progressive parameters */

	JPEGDEC_ERROR_BAD_PRECISION= 21,		/* sample precision != 8           */

	JPEGDEC_ERROR_COMPONENT_COUNT= 22,		/* Too many or few color components in the scan or frame */
	
	JPEGDEC_ERROR_BAD_MCU_SIZE= 23,			/* Sampling factors too large for interleaved scan */

	JPEGDEC_ERROR_EMPTY_IMAGE= 24,			/* Image width | height | numcomponents invalid*/
 
	JPEGDEC_ERROR_IMAGE_SIZE= 25,		    /* Input Image size is greater than Maximum set size. 
	                                           Resizing factor will be calculated to fit the image 
											   in a given output buffer size & give a resized output image*/

	JPEGDEC_ERROR_CORRUPTED_BITSTREAM = 26, /* Corrupted Bit stream */

	JPEGDEC_ERROR_OUTSIZE= 27,				/* Too big Image, Output buffer size is too Small */

	JPEGDEC_ERROR_DCTBUFSIZE = 28,          /* Error in DCT buffer size */
 
	JPEGDEC_ERROR_NULL_PTR = 29,		    /* NULL pointer */
     
	JPEGDEC_ERROR_INPUT_PARAMETER = 30,		/* Error occured in the interface parameter */

	JPEGDEC_ERROR_MAX_SCAN = 31,			/* Exceed Maximum number of scans */

	JPEGDEC_ERROR_INVALID_numAU = 32,

	JPEGDEC_ERROR_INVALID_mcu_multiplication = 33,

	JPEGDEC_ERROR_INVALID_Xlength_Ylength = 34



}IJPEGDEC_ErrorStatus;

/* JPEG warnings */
#define WARN_JPGD_BAD_PROGRESSION  0x2500   /* Inconsistent progression sequence, AC coeff without DC coeff being decoded */
#define WARN_JPGD_APPx_MARKER_SKIP 0x2501   /* Decoding of APP0 marker not supported. It will be skipped.*/

/*JPEG default values */

#define DEFAULT_BITSTREAM_SIZE 1024*1536
#define DEFAULT_HEIGHT   1600
#define DEFAULT_WIDTH    2048
#define DEFAULT_SCAN	15

#define JPEGDEC_YUV420    XDM_YUV_420P
#define JPEGDEC_YUV422    XDM_YUV_422P
#define JPEGDEC_YUV444    XDM_YUV_444P
#define JPEGDEC_YUV411    XDM_YUV_411P
#define JPEGDEC_YUVGRAY   XDM_GRAY
#define JPEGDEC_YUV422ILE XDM_YUV_422ILE

/*
 *  ======== IJPEGDEC_Cmd ========
 *  The Cmd enumeration defines the control commands for the JPEGDEC
 *  control method.
 */

typedef IIMGDEC1_Cmd IJPEGDEC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define IJPEGDEC_GETSTATUS      XDM_GETSTATUS
#define IJPEGDEC_SETPARAMS      XDM_SETPARAMS
#define IJPEGDEC_RESET          XDM_RESET
#define IJPEGDEC_FLUSH          XDM_FLUSH
#define IJPEGDEC_SETDEFAULT     XDM_SETDEFAULT
#define IJPEGDEC_GETBUFINFO     XDM_GETBUFINFO

/*
 *  ======== IJPEGDEC_Handle ========
 *  This handle is used to reference all JPEG_DEC instance objects
 */
typedef struct IJPEGDEC_Obj *IJPEGDEC_Handle;

/*
 *  ======== IJPEGDEC_Obj ========
 *  This structure must be the first field of all JPEG_DEC instance objects
 */
typedef struct IJPEGDEC_Obj {
    struct IJPEGDEC_Fxns *fxns;
} IJPEGDEC_Obj;

/*
 *  ======== IJPEGDEC_Params ========
 *  This structure defines the creation parameters for all JPEG_DEC objects
 */
typedef struct IJPEGDEC_Params {
    /* -------------------------------------------------------------------- */
    /* Global Memory Pool                                                   */
    /* -------------------------------------------------------------------- */
	
	IIMGDEC1_Params imgdecParams;
  
    XDAS_Int32	progressiveDecFlag; 	 /* Set this flag to 1 if progressive 
									      decoding is required, in additional 
									      to baseline sequential mode */
	XDAS_Int32	outImgRes;              
										  /* Set the output image resolution 
										  0 : Always Even Image Resolution
										  1 : Actual Image Resolution
										  */
				  
 
} IJPEGDEC_Params;

/*
 *  ======== IJPEGDEC_Params ========
 *  This structure defines the Dynamic parameters for all JPEG_DEC objects
 */
typedef struct IJPEGDEC_DynamicParams 
{
	
	IIMGDEC1_DynamicParams	imgdecDynamicParams;
	XDAS_Int32				frame_numbytes; /* total number of bytes in a frame used for sectional decoding*/
	XDAS_Int32				progDisplay;	
											/* Set the display option for progressive mode:           */
											/* By Seting progDisplay to 1, The output buffer contains */
											/* the partially (progressively)decoded image after each  */
											/* scan is decoded.                                       */
											/* By Seting progDisplay to 0, The output buffer contains */
											/* the decoded image only after all the scans are decoded */    
	
	
    XDAS_Int32				resizeOption;		/* Set the resizing option:					*/
												/* 0: No resizing							*/
												/* 1: resize the o/p image by 1/2 */
												/* 2: resize the o/p image by 1/4 */
												/* 3: resize the o/p image by 1/8 */
												/* 4: upscale the o/p image by 2   */
												/* 5: upscale the o/p image by 4   */
												/* 6: upscale the o/p image by 8   */
	
    XDAS_Int32				RGB_Format;			/* Set the output RGB format				*/
												/* 0: BGR24									*/
												/* 1: BGR32									*/
												/* 2: RGB16									*/
	XDAS_Int32				numMCU_row;			/* number of rows to be decoded in sectional decoding*/
    XDAS_Int32				x_org;				/* start point of the X-axis in subregion*/
	XDAS_Int32				y_org;				/* start point of the Y-axis in subregion*/
	XDAS_Int32				x_length;			/* X-length of the  subregion */
	XDAS_Int32				y_length;			/* Y-length of the  subregion */
	XDAS_UInt8				alpha_rgb;			/* 0=>Default alpha value to fill rgb32*/
	
	
} IJPEGDEC_DynamicParams;

/*
 *  ======== IJPEGDEC_Status ========
 *  This structure defines the status parameters for all JPEG_DEC objects
 *  Presently IJPEGDEC_Status is same as IJPEGDEC_Output structure
 */
typedef struct IJPEGDEC_Status {
	IIMGDEC1_Status		imgdecStatus;
	XDAS_Int32			mode;           /* 0: baseline sequential, 1: progressive*/
	XDAS_Int32			imageHeight;    /* Actual height of the image*/
	//XDAS_Int32        stride[3];      /* stride values for Y,U and V components */
	XDAS_Int32			decImageSize;   /* size of the decoded image in bytes*/
	XDAS_Int32			lastMCU;        /* 1: last MCU in the frame, 0: not last*/
	XDAS_Int32			hSampleFreq;    /* chroma Component ,Horizontal Sampling frequency*/
	XDAS_Int32			vSampleFreq;    /* chroma Component ,Vertical Sampling frequency*/
	XDAS_Int8			end_of_seq;                  /* end of seq flag */
	XDAS_Int8			end_of_scan;                 /* end of scan flag */
	XDAS_Int32			bytesgenerated[3]; /* Slice decoded Image after each call */
	
} IJPEGDEC_Status;

/*
 *  ======== IJPEGDEC_InArgs ========
 *  This structure defines the runtime input arguments for IJPEGDEC::process
 */

typedef struct IJPEGDEC_InArgs{
    	
	IIMGDEC1_InArgs imgdecInArgs;
	
} IJPEGDEC_InArgs;

/*
 *  ======== IJPEGDEC_OutArgs ========
 *  This structure defines the run time output arguments for IJPEGDEC::process
 *  function.
 */

typedef struct IJPEGDEC_OutArgs{    	
	IIMGDEC1_OutArgs imgdecOutArgs;    
	
} IJPEGDEC_OutArgs;


/*
 *  ======== IJPEGDEC_PARAMS ========
 *  Default parameter values for JPEG_DEC instance objects
 */
extern IJPEGDEC_Params IJPEGDEC_PARAMS;
extern IJPEGDEC_DynamicParams IJPEGDEC_DYNAMICPARAMS;

/*
 *  ======== IJPEGDEC_Fxns ========
 *  This structure defines all of the operations on JPEG_DEC objects
 */
typedef struct IJPEGDEC_Fxns {
    	IIMGDEC1_Fxns iimgdec; 
} IJPEGDEC_Fxns;

#endif	/* IJPEGDEC_ */
