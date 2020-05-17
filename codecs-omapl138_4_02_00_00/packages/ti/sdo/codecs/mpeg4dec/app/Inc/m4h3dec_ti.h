/*
//============================================================================
//
//    FILE NAME : M4H3DEC_TI.h
//
//    ALGORITHM : M4H3DEC
//
//    VENDOR    : TI
//
//    TARGET DSP: C64x
//
//    PURPOSE   : Interface for the M4H3DEC_TI module; TI's implementation
//                of the IM4H3DEC interface.
//============================================================================ */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2005 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef M4H3DEC_TI_
#define M4H3DEC_TI_

#include "im4h3dec.h"


#ifdef MSVC
#include <ialg.h>
#include <idma3.h> 
#else
#include <ti/xdais/ialg.h>
#include <ti/xdais/idma3.h>
#endif

/*
 *  ======== FCPY_TI_IDMA3 ========
 *  TI's implementation of M4H3 IDMA3 interface
 */
extern far IDMA3_Fxns M4H3DEC_TI_IDMA3;


/*
//============================================================================
// M4H3DEC_TI_IALG
//
// TI's implementation of the IALG interface for M4H3DEC
*/
extern far IALG_Fxns M4H3DEC_TI_IALG;

/*
//============================================================================
// M4H3DEC_TI_IM4H3DEC
//
// TI's implementation of the IM4H3DEC interface
*/
extern IM4H3DEC_Fxns M4H3DEC_TI_IM4H3DEC;

/*
//============================================================================
// M4H3DEC_TI_init
//
// Initialize the M4H3DEC_TI module as a whole
*/
extern Void M4H3DEC_TI_init(Void);

/*
//============================================================================
// M4H3DEC_TI_exit
//
// Exit the M4H3DEC_TI module as a whole
*/
extern Void M4H3DEC_TI_exit(Void);


extern XDAS_Int32 M4H3DEC_TI_decodeFrame(IVIDDEC2_Handle handle, XDM1_BufDesc *inBufs, 
               XDM_BufDesc *outBufs, IVIDDEC2_InArgs *inargs, 
               IVIDDEC2_OutArgs *outargs);

extern XDAS_Int32 M4H3DEC_TI_control(IVIDDEC2_Handle handle, IVIDDEC2_Cmd id, 
               IVIDDEC2_DynamicParams *params, IVIDDEC2_Status *status);


#endif	/* M4H3DEC_TI_ */
