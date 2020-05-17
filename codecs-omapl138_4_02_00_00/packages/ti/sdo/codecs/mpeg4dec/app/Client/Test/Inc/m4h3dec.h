/*
//============================================================================
//
//    FILE NAME : M4H3DEC.h
//
//    ALGORITHM : M4H3DEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : This header defines the interface used by clients of the
//                M4H3DEC module
//
//    Component Wizard for eXpressDSP Version 1.31.00 Auto-Generated Component
//============================================================================ */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef M4H3DEC_
#define M4H3DEC_

#include <alg.h>
#include <xdas.h>
#include "im4h3dec.h"

/*
// ===========================================================================
// control method commands
*/
#define M4H3DEC_GETSTATUS    IM4H3DEC_GETSTATUS
#define M4H3DEC_SETPARAMS    IM4H3DEC_SETPARAMS
#define M4H3DEC_RESET        IM4H3DEC_RESET
#define M4H3DEC_SETDEFAULT   IM4H3DEC_SETDEFAULT
#define M4H3DEC_FLUSH		 IM4H3DEC_FLUSH
#define M4H3DEC_GETBUFINFO   IM4H3DEC_GETBUFINFO
  
/*
// ===========================================================================
// M4H3DEC_Handle
//
// This pointer is used to reference all M4H3DEC instance objects
*/
typedef struct IM4H3DEC_Obj *M4H3DEC_Handle;

/*
// ===========================================================================
// M4H3DEC_Params
//
// This structure defines the creation parameters for all M4H3DEC objects
*/
typedef IM4H3DEC_Params M4H3DEC_Params;

/*
// ===========================================================================
// M4H3DEC_PARAMS
//
// This structure defines the default creation parameters for M4H3DEC objects
*/
#define M4H3DEC_PARAMS   IM4H3DEC_PARAMS

/*
// ===========================================================================
// M4H3DEC_Params
//
// This structure defines the dynamic parameters for all M4H3DEC objects
*/
typedef IM4H3DEC_DynamicParams M4H3DEC_DynamicParams;



/*
// ===========================================================================
// M4H3DEC_Status
//
// This structure defines the real-time parameters for M4H3DEC objects
*/
typedef struct IM4H3DEC_Status   M4H3DEC_Status;

/*
// ===========================================================================
// IM4H3DEC_InArgs
//
//  This structure defines the input arguments for all M4H3DEC  instance 
//  process function.
*/
typedef IM4H3DEC_InArgs M4H3DEC_InArgs;

/*
// ===========================================================================
// IM4H3DEC_OutArgs
//
//  This structure defines the Output arguments for all M4H3DEC  instance 
//  process function.
*/
typedef IM4H3DEC_OutArgs M4H3DEC_OutArgs;

/*
// ===========================================================================
// M4H3DEC_create
//
// Create an M4H3DEC instance object (using parameters specified by prms)
*/
extern M4H3DEC_Handle M4H3DEC_create(const IM4H3DEC_Fxns *fxns, const IVIDDEC2_Params *prms);


/*
// ===========================================================================
// M4H3DEC_delete
// Delete the M4H3DEC instance object specified by handle
*/
extern Void M4H3DEC_delete(M4H3DEC_Handle handle);

/*
// ===========================================================================
// M4H3DEC_control
*/
extern XDAS_Int32 M4H3DEC_control(M4H3DEC_Handle handle, IVIDDEC2_Cmd id, 
               IVIDDEC2_DynamicParams *params, IVIDDEC2_Status *status); 

/*
// ===========================================================================
// M4H3DEC_decodeFrame
*/
extern void M4H3DEC_decodeFrame(M4H3DEC_Handle handle, XDM_BufDesc *inBufs, 
               XDM_BufDesc *outBufs, IVIDDEC2_InArgs *inargs, 
               IVIDDEC2_OutArgs *outargs);
 
#endif	/* M4H3DEC_ */
