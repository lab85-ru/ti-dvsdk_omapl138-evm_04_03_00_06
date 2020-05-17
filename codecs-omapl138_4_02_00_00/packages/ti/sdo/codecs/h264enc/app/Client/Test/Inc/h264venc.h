/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

/*!
********************************************************************************
  @file     h264venc.h                                                          
  @brief    This header defines the interface used by clients of                
            the H264VENC module IH264VENC Interface Header                      
  @author   Pramod Kumar Swami                                                  
  @version  1.0 - Nov 05,2004                                                   
  @version  1.1 - Dec 06,2005 -> XMI changes incorporated                       
********************************************************************************
*/

#ifndef H264VENC_ //--{
#define H264VENC_

#include <alg.h>
#include <ti/xdais/ialg.h>
#include <ih264venc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== H264VENC_Handle ========
 *  This pointer is used to reference all H264VENC instance objects
 */
typedef struct IH264VENC_Obj *H264VENC_Handle;

/*
 *  ======== H264VENC_Params ========
 *  This structure defines the creation parameters for all H264VENC objects
 */
typedef IH264VENC_Params H264VENC_Params;

/*
 *  ======== H264VENC_PARAMS ========
 *  This structure defines the default creation parameters for H264VENC objects
 */
#define H264VENC_PARAMS   IH264VENC_PARAMS

/*
// ===========================================================================
// H264VENC_DynamicParams
//
// This structure defines the creation parameters for all H264VENC objects
*/
typedef IH264VENC_DynamicParams H264VENC_DynamicParams;

#define H264VENC_DYNAMICPARAMS H264VENC_TI_DYNAMICPARAMS
/*
// ===========================================================================
// H264VENC_Status
//
// This structure defines the real-time parameters for H264VENC objects
*/
typedef IH264VENC_Status   H264VENC_Status;

/*
// ===========================================================================
// H264VENC_OutArgs
//
// This structure defines input arguments for all H264VENC objects
*/
typedef IH264VENC_OutArgs H264VENC_OutArgs;
/*
// ===========================================================================
// H264VENC_InArgs
//
// This structure defines the real-time input arguments for H264VENC objects
*/
typedef IH264VENC_InArgs   H264VENC_InArgs;


/*
// ===========================================================================
// H264VENC_Cmd
//
// This typedef defines the control commands H264VENC objects
*/
typedef IH264VENC_Cmd   H264VENC_Cmd;

/*
// ===========================================================================
// control method commands
*/
#define H264VENC_GETSTATUS    IH264VENC_GETSTATUS
#define H264VENC_SETPARAMS    IH264VENC_SETPARAMS
#define H264VENC_PREPROCESS   IH264VENC_PREPROCESS
#define H264VENC_RESET        IH264VENC_RESET
#define H264VENC_FLUSH        IH264VENC_FLUSH
#define H264VENC_SETDEFAULT   IH264VENC_SETDEFAULT
#define H264VENC_GETBUFINFO   IH264VENC_GETBUFINFO


/*
 *  ======== H264VENC_create ========
 *  Create an H264VENC instance object (using parameters specified by prms)
 */
extern H264VENC_Handle H264VENC_create(const IH264VENC_Fxns *fxns, 
                                         const H264VENC_Params *prms);

/*
// ===========================================================================
// H264VENC_control
//
// Get, set, and change the parameters of the MP4VE function (using parameters specified by status).
*/
extern Int H264VENC_control(H264VENC_Handle handle, H264VENC_Cmd cmd, H264VENC_DynamicParams *params, H264VENC_Status *status);

/*
// ===========================================================================
// H264VENC_delete
// Delete the MP4VE instance object specified by handle
*/
extern Void H264VENC_delete(H264VENC_Handle handle);

/*
// ===========================================================================
// H264VENC_encode
*/
extern Int H264VENC_encode(H264VENC_Handle handle, IVIDEO1_BufDescIn *inptr, XDM_BufDesc *outptr, H264VENC_InArgs *inarg, H264VENC_OutArgs *outarg);


/*
 *  ======== H263ENC_init ========
 */
extern void H264VENC_init(void);

/*
 *  ======== H263ENC_exit ========
 */
extern void H264VENC_exit(void);


#ifdef __cplusplus
}
#endif


#endif    //H264VENC_ --}
/* ======================================================================== */ 
/* End of file : H264venc.h                                                */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */ 
