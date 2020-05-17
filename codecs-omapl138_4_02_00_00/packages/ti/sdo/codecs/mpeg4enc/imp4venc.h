/*
//============================================================================
//    FILE NAME : imp4venc.h
//    ALGORITHM : MP4VENC
//    VENDOR    : TI
//    TARGET DSP: C64x+
//    PURPOSE   : This is the top level driver file that exercises the MPEG4
//               Encoder code
//============================================================================
*/
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
#ifndef IMP4VENC_
#define IMP4VENC_

#ifdef SETUP_VC
#include <std.h>
#include <ialg.h>
#include <ividenc1.h> //XDM1.0 Migration
#else
#include <ti/bios/include/std.h> // Take from Bios
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividenc1.h> //XDM1.0 Migration
#endif


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

//!< control method commands
#define IMP4VENC_GETSTATUS      XDM_GETSTATUS
#define IMP4VENC_SETPARAMS      XDM_SETPARAMS
#define IMP4VENC_RESET          XDM_RESET
#define IMP4VENC_FLUSH          XDM_FLUSH
#define IMP4VENC_SETDEFAULT     XDM_SETDEFAULT
#define IMP4VENC_GETBUFINFO     XDM_GETBUFINFO
#define IMP4VENC_GETVERSION     XDM_GETVERSION

/*
 *  ======== IMP4VENC_Handle ========
 *  This handle is used to reference all MPEG4VENC instance objects
 */
typedef struct IMP4VENC_Obj *IMP4VENC_Handle;

/*
 *  ======== IMP4VENC_Obj ========
 *  This structure must be the first field of all MP4VENC instance objects
 */
typedef struct IMP4VENC_Obj {

	struct IMP4VENC_Fxns *fxns;

} IMP4VENC_Obj;


/*
// ===========================================================================
// IMP4VE_Params
//
// This structure defines the creation parameters for all MP4VE objects
*/
typedef struct IMP4VE_Params {

    IVIDENC1_Params videncParams; /* must be followed for all video encoders */

	// CreateParams specific to MP4
	XDAS_Int32 encodeMode;			/* Encoding mode, 0: H.263 mode, 1 : MPEG4 mode */
	XDAS_Int32 levelIdc; 		    /* Profile level indication for MPEG4 */
    XDAS_Int32 numFrames;			/* Number of frames to be encoded. This parameter is used only for VM4 rate control inside encoder */
	XDAS_Int32 rcAlgo;          	/* Rate Control Method, 0: disable rate control, 1-TM5,  2-modified TMN5, 4-PLR3, 5-VM4, 6 : mod TM5 */
    XDAS_Int32 vbvBufferSize;  	    /* vbv_buffer_size (in multiples of 16 kbit) */
 	XDAS_Int32 useVOS;            	/* VOS header insertion, 0 = off, 1 = on */
	XDAS_Int32 useGOV;            	/* GOV header insertion, 0 = off, 1 = on */
	XDAS_Int32 useDataPartition;    /* Data partioning, 0 = off , 1 = on */
	XDAS_Int32 useRVLC;   			/* RVLC, 0 = off , 1 = on */
	XDAS_Int32 maxDelay;			/* max delay for rate control interms of ms, set it to 1000 for 1 second delay */
	XDAS_Int32 enableSCD;   /* Scene Change detector  0 = off, 1=on */
} IMP4VENC_Params;

/*
 *  ======== IMP4VENC_PARAMS ========
 *  Default parameter values for MPEG4VENC instance objects
 */
extern IMP4VENC_Params IMP4VENC_PARAMS;

/*
// ===========================================================================
// IMP4VE_DynamicParams
//
// This structure defines the run time parameters for all MP4VE objects
*/
typedef struct IMP4VE_DynamicParams {

	IVIDENC1_DynamicParams videncDynamicParams; /* must be followed for all video encoders */

	// DynamicParams specific to MP4
	XDAS_Int32 resyncInterval;      /* Insert resync marker (RM) after given specified bits, 0 means do not insert */
	XDAS_Int32 hecInterval;  		/* Insert HEC after given specified packets insertion, 0 means do not insert   */
	XDAS_Int32 airRate;          	/* Adaptive Intra refresh of n MBs per frame */
	XDAS_Int32 mirRate;      		/* Mandatory Intra refresh rate: at least 1 in 132 time MB coded */
    XDAS_Int32 qpIntra;           	/* default QP for I frame : QPI,  range 1 to 31*/
	XDAS_Int32 qpInter;           	/* default QP for P frame, range 1 to 31  */
    XDAS_Int32 fCode;             	/* f_code(as per MPEG4 spec)(max MV length): 1<<f_code-1, set to 1 in case of H.263 */
	XDAS_Int32 useHpi;         	    /* Half pixel Interpolation, 0 : off, 1 = onn */
	XDAS_Int32 useAcPred;           /* AC prediction Flag, 0 : off, 1 = on */
	XDAS_Int32 lastFrame;           /* Last frame flag, 0 : NOT Last frame, 1 = Last Frame */
	XDAS_Int32 MVDataEnable;        /* Flag to enable Motion vector access*/
	XDAS_Int32 useUMV;              /* UMV Flag, 0 : off, 1 = on */

	XDAS_Int32 qpMax;
	XDAS_Int32 qpMin;
} IMP4VENC_DynamicParams;

extern IMP4VENC_DynamicParams IMP4VENC_DYNAMICPARAMS;

/*
// ===========================================================================
// IMP4VE_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/

typedef struct IMP4VENC_Status {

   IVIDENC1_Status  videncStatus;	/* must be followed for video encoder */
   XDAS_Int32 mvDataSize;   //XDM1.0
   XDAS_Int32 numPackets; /* To get total number of packets in one frame */

	/* TODO : add on  algorithm & vendor specific status here */

} IMP4VENC_Status;

/*
 *  ======== IMP4VENC_InArgs ========
 *  This structure defines the runtime input arguments for IMP4VENC::process
 */

typedef struct IMP4VENC_InArgs {

	IVIDENC1_InArgs videncInArgs;

} IMP4VENC_InArgs;

/*
 *  ======== IMP4VENC_OutArgs ========
 *  This structure defines the run time output arguments for IMP4VENC::process
 *  function.
 */

typedef struct IMP4VENC_OutArgs {

	IVIDENC1_OutArgs videncOutArgs;

} IMP4VENC_OutArgs;

/*
 *  ======== IMP4VENC_Cmd ========
 *  This structure defines the control commands for the IMP4VENC module.
 */
typedef IVIDENC1_Cmd IMP4VENC_Cmd;

/*
 *  ======== IMP4VENC_Fxns ========
 *  This structure defines all of the operations on IMP4VENC objects.
 */

typedef struct IMP4VENC_Fxns {

	  IVIDENC1_Fxns	ividenc;    // IMP4VE extends IVIDENC object.

} IMP4VENC_Fxns;

/*
 * MPEG4 Encoder Specific Error Code bits
 * Bits (0 -7) of the error word are reserved for any algorithm specific
 * error. Now Bit 0 is used for the Output bitstream buffer overflow.
 */
typedef enum 
{
  IMP4VENC_OUTBUFOVERFLOW = 0 // Sets whe Output Buffer overflow occurs          
  
} IMP4VENC_ErrorBit;
/* 
 * New extended error bit showing output buffer overflow error
 */
#define IMP4VENC_ISOUTBUFOVERFLOW(x)     (((x) >> IMP4VENC_OUTBUFOVERFLOW) & 0x1)

/* 
 * Check for new extended error bit showing output buffer overflow error
 */
#define IMP4VENC_SETOUTBUFOVERFLOW(x)   ((x) |=(0x1 << IMP4VENC_OUTBUFOVERFLOW))

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif    /* IMP4VENC_ */

/* ======================================================================== */
/* End of file : imp4venc.h                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

