/*
//============================================================================
//
//    FILE NAME : h264vdec.h
//
//    ALGORITHM : H264VDEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : This header defines the interface used by clients of 
//               the H264VDEC module IH264VDEC Interface Header 
//
//============================================================================
*/
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef H264VDEC_
#define H264VDEC_

#include <ih264vdec.h>
//#include <ialg.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
// ===========================================================================
// H264VDEC_Handle
//
// This pointer is used to reference all MP4VE instance objects
*/
typedef struct IH264VDEC_Obj *H264VDEC_Handle;


/*
// ===========================================================================
// H264VDEC_Params
//
// This structure defines the creation parameters for all MP4VE objects
*/
typedef IH264VDEC_Params H264VDEC_Params;


/*
// ===========================================================================
// H264VDEC_PARAMS
//
// This structure defines the default creation parameters for MP4VE objects
*/
#define H264VDEC_PARAMS   IH264VDEC_PARAMS

/*
// ===========================================================================
// H264VDEC_DynamicParams
//
// This structure defines the creation parameters for all MP4VE objects
*/
typedef IH264VDEC_DynamicParams H264VDEC_DynamicParams;
/*
// ===========================================================================
// H264VDEC_Status
//
// This structure defines the real-time parameters for MP4VE objects
*/
typedef struct IH264VDEC_Status   H264VDEC_Status;

/*
// ===========================================================================
// H264VDEC_OutArgs
//
// This structure defines input arguments for all MP4VE objects
*/
typedef IH264VDEC_OutArgs H264VDEC_OutArgs;
/*
// ===========================================================================
// H264VDEC_InArgs
//
// This structure defines the real-time input arguments for MP4VE objects
*/
typedef struct IH264VDEC_InArgs   H264VDEC_InArgs;


/*
// ===========================================================================
// H264VDEC_Cmd
//
// This typedef defines the control commands MP4VE objects
*/
typedef IH264VDEC_Cmd   H264VDEC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define H264VDEC_GETSTATUS    IH264VDEC_GETSTATUS
#define H264VDEC_SETPARAMS    IH264VDEC_SETPARAMS
#define H264VDEC_PREPROCESS   IH264VDEC_PREPROCESS
#define H264VDEC_RESET        IH264VDEC_RESET
#define H264VDEC_FLUSH        IH264VDEC_FLUSH
#define H264VDEC_SETDEFAULT   IH264VDEC_SETDEFAULT
#define H264VDEC_GETBUFINFO   IH264VDEC_GETBUFINFO
#define H264VDEC_GETVERSION   IH264VDEC_GETVERSION

#ifdef __cplusplus
}
#endif

#endif    /* H264VDEC_ */
/* ======================================================================== */ 
/* End of file : h264vdec.h                                                 */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */ 
