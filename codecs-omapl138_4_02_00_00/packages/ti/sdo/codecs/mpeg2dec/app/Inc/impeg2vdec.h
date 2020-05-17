/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */

/*
 * (C) Copyright 2000     Texas Instruments Inc.  All rights reserved.
 * Exclusive property of the Multimedia DSP Branch, Tsukuba Research &
 * Development Center of Texas Instruments Japan Limited.  Any handling,
 * use, disclosure, reproduction, duplication, transmission, or storage
 * of any part of this work by any means is subject to restrictions and
 * prior written permission set forth in TI's program license agreements
 * and associated software documentation.
 *
 * This copyright notice, restricted rights legend, or other proprietary
 * markings must be reproduced without modification in any authorized
 * copies of any part of this work.  Removal or modification of any part
 * of this notice is prohibited.
 *
 */

/* "@(#) DAIS 1.00 07-29-99 (dais-a09)" */
/*
 *  ======== impeg2vdec.h ========
 *  This header defines all types, constants, and functions shared by all
 *  implementations of the MPEG2VDEC algorithm.
 */
#ifndef IMPEG2VDEC_
#define IMPEG2VDEC_

#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec2.h>


/** @enum MPEG2DEC_ERROR */
typedef enum {
  /* 00 */  MPEG2VDEC_ERROR_Success = 0,
  /* 01 */  MPEG2VDEC_ERROR_Failure,
  /* 02 */  MPEG2VDEC_ERROR_bitstream_Overrun,
  /* 03 */  MPEG2VDEC_ERROR_unsupportedpictureSpatialScalableExtension,
  /* 04 */  MPEG2VDEC_ERROR_unSupportedpictureTemporalScalableExtension,
  /* 05 */  MPEG2VDEC_ERROR_errorneousBitstream,
  /* 06 */  MPEG2VDEC_ERROR_sliceVerticalPosition,
  /* 07 */  MPEG2VDEC_ERROR_corruptedHeader,
  /* 08 */  MPEG2VDEC_ERROR_unsupportedInput,
  /* 09 */  MPEG2VDEC_ERROR_incorrectWidthHeight,
  /* 10 */  MPEG2VDEC_ERROR_nullOutputBufPtrs,
  /* 11 */  MPEG2VDEC_ERROR_brokenLinkSet,
  /* 12 */  MPEG2VDEC_ERROR_droppedFrame,
  /* 13 */  MPEG2VDEC_ERROR_noValidReferences,
  /* 14 */  MPEG2VDEC_ERROR_noRefBufferToFlush,
  /* 15 */  MPEG2VDEC_ERROR_insufficientUserDataBuffer,
  /* 16 */  MPEG2VDEC_ERROR_nullDisplayHdrPtrs,
  /* 17 */  MPEG2VDEC_ERROR_nullInputBufPtr,
  /* 18 */  MPEG2VDEC_ERROR_insufficientoutBufSize,
  /* 19 */  MPEG2VDEC_ERROR_nullPointer,
  /* 20 */  MPEG2VDEC_ERROR_invalidStructSize,
  /* 21 */  MPEG2VDEC_ERROR_invalidDisplayHdrSizes
} MPEG2VDEC_ERROR;


/*
 *  ======== IMPEG2VDEC_Obj ========
 *  Every implementation of IMPEG2VDEC *must* declare this structure as
 *  the first member of the implementation's object.
 */
typedef struct IMPEG2VDEC_Obj {
    struct IMPEG2VDEC_Fxns *fxns;
} IMPEG2VDEC_Obj;

/*
 *  ======== IMPEG2VDEC_Handle ========
 *  This type is a pointer to an implementation's instance object.
 */
typedef struct IMPEG2VDEC_Obj *IMPEG2VDEC_Handle;

#define MPEG2VDEC_FUNC_START_PARA     2
#define MPEG2VDEC_FUNC_DECODE_FRAME   3


/* picture structure enum*/
typedef enum ePicStrFormat {
   MPEG2VDEC_TOP_FIELD=1, MPEG2VDEC_BOTTOM_FIELD, MPEG2VDEC_FRAME_PICTURE
} ePicStrFormat;

#define    IMPEG2VDEC_GETSTATUS     XDM_GETSTATUS
#define    IMPEG2VDEC_SETPARAMS     XDM_SETPARAMS
#define    IMPEG2VDEC_RESET         XDM_RESET
#define    IMPEG2VDEC_SETDEFAULT    XDM_SETDEFAULT
#define    IMPEG2VDEC_FLUSH         XDM_FLUSH
#define    IMPEG2VDEC_GETBUFINFO    XDM_GETBUFINFO
#define    IMPEG2VDEC_GETVERSION    XDM_GETVERSION//XDM1.0


/*
 *  ======== IMPEG2VDEC_Params ========
 *  This structure defines the parameters necessary to create an
 *  instance of a MPEG2VDEC object.
 *
 *  Every implementation of IMPEG2VDEC *must* declare this structure as
 *  the first member of the implementation's parameter structure.
 */
typedef struct IMPEG2VDEC_Params {
    IVIDDEC2_Params viddecParams;//XDM1.0
} IMPEG2VDEC_Params;

/*
 *  ======== IMPEG2VDEC_PARAMS ========
 *  Default instance creation parameters (defined in impeg2vdec.c)
 */
extern IMPEG2VDEC_Params IMPEG2VDEC_PARAMS;

/*
// ===========================================================================
// IMPEG2VDEC_DynamicParams
//
// This structure defines the dynamic parameters for all MPEG2DEC objects
*/
typedef struct IMPEG2VDEC_DynamicParams {
    IVIDDEC2_DynamicParams viddecDynamicParams; //XDM1.0
    XDAS_Int32  ppNone;               //This will generate 4:2:0 planar output
   	XDAS_UInt32 dyna_chroma_format;	  /* If ON, chroma format can be modified at the format level */
    XDAS_UInt32 displayFieldReorder;  /* To display the fields always in the order TOP-BOTTOM */
   	XDAS_UInt32	frameLevelByteSwap;	  /* Byte swap is done inside the decoder only for the current
                                                                    frame number of bytes */
    XDAS_UInt32 skip_B_frame;		  /* If ON, the decode skips B frames */
    XDAS_UInt32 goto_next_I_frame;    /* If ON, decoder seeks to the next I frame */
	XDAS_UInt32 skip_curr_frame;	  /* If ON, current frame is skipped */
    XDAS_UInt32 seek_frame_end;	  	  /* This flag seeks to end of currenf frame */
	XDAS_UInt32	getDisplayHdrInfo;	  /* If ON, the structures described below are updated */
	XDAS_UInt32 reverse_play;		  /* If ON, decoder starts in reverse play mode */
	XDAS_UInt32 robustness_level;	  /* Indicates level of robustness of the decoder */
	XDAS_UInt32 no_delay_display;     /* If ON, the first I frame is given back imm */
} IMPEG2VDEC_DynamicParams;



typedef struct IMPEG2VDEC_Sequence_Display_Extension {
  XDAS_Int32 video_format;		   /* Indicates representation of the pictures */
  XDAS_Int32 display_hor_size;	   /* Display horizontal size */
  XDAS_Int32 display_ver_size;     /* Display vertical size */
} IMPEG2VDEC_Sequence_Display_Extension;


typedef struct IMPEG2VDEC_Picture_Display_Extension {
  XDAS_Int32 *frm_cen_hor_offset;	/* Horizontal offset in units of 1/16th sample */
  XDAS_Int32 *frm_cen_ver_offset;   /* Vetical offset in units of 1/16th sample */
  XDAS_Int32 frm_cen_offsets;		/* number of frame center offsets present */
} IMPEG2VDEC_Picture_Display_Extension;


/*
 *  ======== IMPEG2VDEC_InArgs ========
 *  This structure defines the input arguments for all MPEG2DEC  instance
 *  process function.
 */
typedef struct IMPEG2VDEC_InArgs {

    IVIDDEC2_InArgs viddecInArgs;     //XDM1.0 Should be the first argument

} IMPEG2VDEC_InArgs;

/*
 *  ======== IMPEG2VDEC_OutArgs ========
 *  This structure defines the Output arguments for all MPEG2DEC  instance
 *  process function.
 */
typedef struct IMPEG2VDEC_OutArgs {

    IVIDDEC2_OutArgs viddecOutArgs; //XDM1.0     /*Should be the first argument*/


} IMPEG2VDEC_OutArgs;


typedef struct IMPEG2VDEC_Buffer_Entry {
	XDAS_UInt32 inBuf_address;			   /* Address of the accepted buffer */
	XDAS_Int32  inputID;				   /* Input ID of the accepted buffer */
} IMPEG2VDEC_Buffer_Entry;


/*
// ===========================================================================
// IMPEG2VDEC_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct IMPEG2VDEC_Status {

    IVIDDEC2_Status viddecStatus;//XDM1.0   /* Should be the first argument */
    IVIDEO1_BufDesc displayBufinfo; /* decoder fills this structure to denote
                                     the buffer information for current displayable
                                     frame. */
    XDAS_Int32     outputID;       /* same as outputID of IVIDDEC_OutArgs */
    IMPEG2VDEC_Buffer_Entry   acceptedBufs[XDM_MAX_IO_BUFFERS]; /* Indicates accepted buffer (Y) addresses that are going to
									be output by the decoder in display order at some point of time */


    XDAS_UInt32 valid_buff_entries;  /* Valid number of structures in the array "acceptedBufs" */
    XDAS_Int32 non_vcl_length_at_start;
    XDAS_Int32 frame_num;				   /* Indicates frame number in the display order */
    XDAS_UInt32 frameIdentifier;	       /* timestamp of the frame returned by the decoder */

    ePicStrFormat pict_struct;             /* indicates pict structure:see the enum definition above*/
                                           /* This also has some build information */
    XDAS_Int32 video_format;		   /* Indicates representation of the pictures */
    XDAS_Int32 display_hor_size;	   /* Display horizontal size */
    XDAS_Int32 display_ver_size;     /* Display vertical size */
    XDAS_Int32 actualWidth;           /* Actual decoded width */
    XDAS_Int32 actualHeight;          /* Actual decoded height */

    XDAS_Int8 new_user_data;			   /* Set to 1 if user data is parsed from the current frame */
    XDAS_Int8 new_seq_display;			   /* Set to 1 if sequence display extension is parsed from the current frame */
    XDAS_Int8 new_pict_display;			   /* Set to 1 if picture display extension is parsed from the current frame */
    XDAS_Int8 is_mpeg2;                    /* Signals mpeg2 streams, will be set to 0 for mpeg1 stream*/
    XDAS_Int8 end_of_seq;                  /* end of seq flag, may be used for displaying last few frames stored in buffer */
    XDAS_Int8 stepSize;                    /* quantization step size used in the frame. */
    XDAS_Int8 progressive_frame;           /* Indicates progressive/interlace frame.*/
    XDAS_Int8 progressive_sequence;         /* Indicates progressive/interlace sequence.*/
    XDAS_Int8 closed_gop;				   /* Indicates B-pictures are encoded using only backward
        										  prediction or intra coding */
    XDAS_Int8 broken_link;				   /* Indicates B-pictures cannot be correctly decoded */
    XDAS_Int8 aspect_ratio;				   /* Provides aspect ratio information */
    XDAS_Int8 inbuf_status;                /* Status of the display buffer passed by the application */

} IMPEG2VDEC_Status;


/*
 *  ======== IMPEG2VDEC_Fxns ========
 *  All implementation's of MPEG2VDEC must declare and statically
 *  initialize a constant variable of this type.
 *
 *  By convention the name of the variable is MPEG2VDEC_<vendor>_IMPEG2VDEC, where
 *  <vendor> is the vendor name.
 */
typedef struct IMPEG2VDEC_Fxns {
    IVIDDEC2_Fxns    ividdec;//XDM1.0
} IMPEG2VDEC_Fxns;

#endif  /* IMPEG2VDEC_ */
