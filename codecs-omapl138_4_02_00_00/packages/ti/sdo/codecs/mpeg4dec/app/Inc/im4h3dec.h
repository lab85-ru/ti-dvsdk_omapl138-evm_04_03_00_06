/*
//============================================================================
//
//    FILE NAME : IM4H3DEC.h
//
//    ALGORITHM : M4H3DEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : IM4H3DEC Interface Header
//
//    Component Wizard for eXpressDSP Version 1.31.00 Auto-Generated Component
//============================================================================ */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


#ifndef IM4H3DEC_
#define IM4H3DEC_

#ifdef MSVC
#include <std.h>
#include <xdas.h>
#include <ialg.h>
#include <ividdec2.h>
#else
#include <ti/bios/include/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ialg.h>
#include <ti/xdais/dm/ividdec2.h>
#endif


/** @enum M4H3DEC_TI_ERROR */
typedef enum {
  /* 00 */  M4H3DEC_TI_ERROR_Success = 0,
  /* 01 */  M4H3DEC_TI_ERROR_Failure ,
  /* 02 */  M4H3DEC_TI_ERROR_nonMpegStream,
  /* 03 */  M4H3DEC_TI_ERROR_nonVideoMpegStream,
  /* 04 */  M4H3DEC_TI_ERROR_unSupportedProfile,
  /* 05 */  M4H3DEC_TI_ERROR_invalidBitstreamAddress,
  /* 06 */  M4H3DEC_TI_ERROR_invalidVideoObjectLayerStartCode,
  /* 07 */  M4H3DEC_TI_ERROR_nullBitstreamAddress,
  /* 08 */  M4H3DEC_TI_ERROR_insufficientData,
  /* 09 */  M4H3DEC_TI_ERROR_unsupportedVOL_verid,
  /* 10 */  M4H3DEC_TI_ERROR_invalidAspectRatio,
  /* 11 */  M4H3DEC_TI_ERROR_invalidChromaFormat,
  /* 12 */  M4H3DEC_TI_ERROR_unsupportedVOLShape,
  /* 13 */  M4H3DEC_TI_ERROR_invalidVOPTimeIncrementResolution,
  /* 14 */  M4H3DEC_TI_ERROR_unsupportedFeatureInterlaced,
  /* 15 */  M4H3DEC_TI_ERROR_unsupportedFeatureOBMC,
  /* 16 */  M4H3DEC_TI_ERROR_unsupportedVideoDataPrecision, 
  /* 17 */  M4H3DEC_TI_ERROR_unsupportedObjectType,
  /* 18 */  M4H3DEC_TI_ERROR_unsupportedFirstQuantMethod,
  /* 19 */  M4H3DEC_TI_ERROR_unsupportedFeatureScalability,
  /* 20 */  M4H3DEC_TI_ERROR_invalidCallingOrder,
  /* 21 */  M4H3DEC_TI_ERROR_invalidVideoObjectSC,
  /* 22 */  M4H3DEC_TI_ERROR_invalidVOPSC,
  /* 23 */  M4H3DEC_TI_ERROR_invalidQuant,
  /* 24 */  M4H3DEC_TI_ERROR_invalidFcode,
  /* 25 */  M4H3DEC_TI_ERROR_invalidMBnumInVPH,
  /* 26 */  M4H3DEC_TI_ERROR_endOfSequence,
  /* 27 */  M4H3DEC_TI_ERROR_invalidGOBnum,
  /* 28 */  M4H3DEC_TI_ERROR_corruptedHeader,          
  /* 29 */  M4H3DEC_TI_ERROR_corruptedBitStream,       
  /* 30 */  M4H3DEC_TI_ERROR_unsupportedFeatureBFrames,
  /* 31 */  M4H3DEC_TI_ERROR_unsupportedFeatureSprite,
  /* 32 */  M4H3DEC_TI_ERROR_unsupportedFeatureQuarterPel,
  /* 33 */  M4H3DEC_TI_ERROR_exceededResolution,
  /* 34 */	M4H3DEC_TI_ERROR_unsupportedFeatureIntraDcVlcThreshold,
  /* 35 */  M4H3DEC_TI_ERROR_invalidValue,
  /* 36 */  M4H3DEC_TI_ERROR_stuffingInMB,
  /* 37 */  M4H3DEC_TI_ERROR_numMbRowsInVpExceeded,
  /* 38 */  M4H3DEC_TI_ERROR_cannotdecodempeg4,
  /* 39 */  M4H3DEC_TI_ERROR_incorrectWidthHeight,
  /* 40 */  M4H3DEC_TI_ERROR_insufficientMemory,
  /* 41 */  M4H3DEC_TI_ERROR_consumedAllInputBits,
  /* 42 */  M4H3DEC_TI_ERROR_noRefBufferToFlush,
  /* 43 */  M4H3DEC_TI_ERROR_missingFirstIFrame,
  /* 44 */  M4H3DEC_TI_ERROR_invalidDmvLength,
  /* 45 */  M4H3DEC_TI_ERROR_unsupportedNumberOfWarpingPointGMC,
  /* 46 */  M4H3DEC_TI_ERROR_invalidWarpPoint,
  /* 47 */  M4H3DEC_TI_ERROR_unsupportedFeatureInterlaceSprite,
  /* 48 */  M4H3DEC_TI_ERROR_unsupportedFeatureDPSprite,
  /* 49 */  M4H3DEC_TI_ERROR_invalidToolsSimpleObject,
  /* 50 */  M4H3DEC_TI_ERROR_nullPtr,
  /* 51 */  M4H3DEC_TI_ERROR_bufSizeError,
  /* 52 */  M4H3DEC_TI_ERROR_unsupportedFeature



} M4H3DEC_TI_ERROR;

/*
// ===========================================================================
// IM4H3DEC_Cmd
//
// The Cmd enumeration defines the control commands for the M4H3DEC
// control method.
*/

#define IM4H3DEC_GETSTATUS      XDM_GETSTATUS
#define IM4H3DEC_SETPARAMS      XDM_SETPARAMS
#define IM4H3DEC_RESET          XDM_RESET
#define IM4H3DEC_FLUSH          XDM_FLUSH
#define IM4H3DEC_SETDEFAULT     XDM_SETDEFAULT
#define IM4H3DEC_GETBUFINFO     XDM_GETBUFINFO
#define IM4H3DEC_GETVERSION     XDM_GETVERSION

/*
// ===========================================================================
// IM4H3DEC_Handle
//
// This handle is used to reference all M4H3DEC instance objects
*/
typedef struct IM4H3DEC_Obj *IM4H3DEC_Handle;

/*
// ===========================================================================
// IM4H3DEC_Obj
//
// This structure must be the first field of all M4H3DEC instance objects
*/
typedef struct IM4H3DEC_Obj {
    struct IM4H3DEC_Fxns *fxns;
} IM4H3DEC_Obj;



/*
// ===========================================================================
// IM4H3DEC_Params
//
// This structure defines the creation parameters for all M4H3DEC objects
*/
typedef struct IM4H3DEC_Params {
	IVIDDEC2_Params viddecParams;  
} IM4H3DEC_Params;

/*
// ===========================================================================
// IM4H3DEC_DynamicParams
//
// This structure defines the dynamic parameters for all M4H3DEC objects
*/
typedef struct IM4H3DEC_DynamicParams {
	IVIDDEC2_DynamicParams viddecDynamicParams;   
	XDAS_Int32 postDeblock; /*for deblocking postprocess*/
	XDAS_Int32 postDering; /*for deringing postprocess*/
	XDAS_Int32 errorConceal; /*for error concealment*/
	XDAS_UInt32	FrameLevelByteSwap; /*  Will do byte swap only */
	                                /*  for number of bytes of the frame to be decoded */
	XDAS_UInt32	useHighPrecIdctQp1; /*  0: Use Default Idct  
										1: Use High precision Idct for qp 1*/
} IM4H3DEC_DynamicParams;

/*
 *  ======== IM4H3DEC_InArgs ========
 *  This structure defines the input arguments for all M4H3DEC  instance 
 *  process function.
 */
typedef struct IM4H3DEC_InArgs {
	IVIDDEC2_InArgs viddecInArgs;
} IM4H3DEC_InArgs;

/*
 *  ======== IM4H3DEC_OutArgs ========
 *  This structure defines the Output arguments for all M4H3DEC  instance 
 *  process function.
 */
typedef struct IM4H3DEC_OutArgs {
	IVIDDEC2_OutArgs viddecOutArgs;  
    XDAS_Int32  profile;        /* 0: Simple Profile
								   1: Advance Simple Profile    */
} IM4H3DEC_OutArgs;

/*
// ===========================================================================
// IM4H3DEC_Status
//
// Status structure defines the parameters that can be changed or read
// during real-time operation of the alogrithm.
*/
typedef struct IM4H3DEC_Status {
	IVIDDEC2_Status  viddecStatus;
	XDAS_Int32 vopTimeIncrementResolution;
    XDAS_Int32 vopTimeIncrement;
	XDAS_Int32 actualHeight;
	XDAS_Int32 actualWidth;	
	XDAS_Int32 cloedGOV;	   /* Closed GOV flag in GOV header */
	XDAS_Int32 brokenLink;     /* Broken link flag in GOV header */

} IM4H3DEC_Status;

/* 
// ===========================================================================
// IM4H3DEC_PARAMS
//
// Default parameter values for M4H3DEC instance objects
*/
extern IM4H3DEC_Params IM4H3DEC_PARAMS;

/*
// ===========================================================================
// IM4H3DEC_Fxns
//
// This structure defines all of the operations on M4H3DEC objects
*/
typedef struct IM4H3DEC_Fxns {
	IVIDDEC2_Fxns	ividdec;
} IM4H3DEC_Fxns;


#endif	/* IM4H3DEC_ */
