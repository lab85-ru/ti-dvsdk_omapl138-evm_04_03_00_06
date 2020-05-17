/*
//============================================================================
//
//    FILE NAME : h264vdec_ti.h
//
//    ALGORITHM : H264VDEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : Interface for the H264VDEC_TI module; TI's implementation
//                of the IH264VDEC interface.
//
//============================================================================
*/
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef H264VDEC_TI_
#define H264VDEC_TI_

#include <ih264vdec.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== H264VDEC_TI_IALG ========
 *  TI's implementation of the IALG interface for H264VDEC
 */
extern IALG_Fxns H264VDEC_TI_IALG;

/*
 *  ======== H264VDEC_TI_IH264VDEC ========
 *  TI's implementation of the IH264VDEC interface
 */
extern IH264VDEC_Fxns H264VDEC_TI_IH264VDEC;


/*
 *  ======== H264VDEC_TI_IDMA3 ========
 *  TI's implementation of H264VDec's IDMA3 interface
 */
extern IDMA3_Fxns H264VDEC_TI_IDMA3;


extern void H264VDEC_TI_exit(void);

extern void H264VDEC_TI_init(IALG_Handle handle);

extern XDAS_Int32 H264VDEC_TI_control(IVIDDEC2_Handle handle,
									  IVIDDEC2_Cmd cmd, 
									  IVIDDEC2_DynamicParams *params,
									  IVIDDEC2_Status *status);

extern XDAS_Int32 H264VDEC_TI_decode(IVIDDEC2_Handle handle,
									 XDM1_BufDesc *Input,
									 XDM_BufDesc *Output,
									 IVIDDEC2_InArgs *inargs,
									 IVIDDEC2_OutArgs *outargs);

#ifdef __cplusplus
}
#endif

#endif	/* H264VDEC_TI_ */
/* ======================================================================== */
/* End of file : h264vdec_ti.h                                              */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

