/****************************************************************************/
/*==========================================================================*/
/*    FILE NAME : mp4venc_ti.h												*/
/*    ALGORITHM : MP4VENC													*/
/*    VENDOR    : TI														*/
/*    TARGET DSP: C64x+														*/
/*    PURPOSE   : This is the top level driver file that exercises the MPEG4*/
/*               Encoder code												*/
/*==========================================================================*/
/****************************************************************************/
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef MP4VENC_TI_H
#define MP4VENC_TI_H

#ifdef SETUP_VC
#include <xdm.h> /*XDM1.0*/
#include <ivideo.h> /*XDM1.0*/
#include <ialg.h> 
#include <idma3.h>
#else
#include <ti/xdais/dm/xdm.h> /*XDM1.0 */
#include <ti/xdais/dm/ivideo.h> /*XDM1.0*/
#include <ti/xdais/ialg.h> 
#include <ti/xdais/idma3.h>
#endif

#include <imp4venc.h>

/*
 *  ======== FCPY_TI_IDMA3 ========
 *  TI's implementation of MP4ENC IDMA3 interface
 */
extern IDMA3_Fxns MP4VENC_TI_IDMA3;
/*
 *  ======== MP4VENC_TI_IALG ========
 *  TI's implementation of the IALG interface for MP4VENC
 */
extern IALG_Fxns MP4VENC_TI_IALG;

/*
 *  ======== MP4VENC_TI_IMP4VENC ========
 *  TI's implementation of the IMP4VENC interface
 */
extern IMP4VENC_Fxns MP4VENC_TI_IMP4VENC;

extern void MPEG4ENC_TI_init(void);

extern void MPEG4ENC_TI_exit(void);

extern void MP4VENC_TI_activate(IALG_Handle);

extern XDAS_Int32 MP4VENC_TI_alloc(const IALG_Params *,
                              struct IALG_Fxns **,
                              IALG_MemRec *);

extern XDAS_Int32 MP4VENC_TI_control(IMP4VENC_Handle handle, IVIDENC1_Cmd cmd,
                    IMP4VENC_DynamicParams *params, IMP4VENC_Status *status);

extern void MP4VENC_TI_deactivate(IALG_Handle);

extern XDAS_Int32 MP4VENC_TI_free(IALG_Handle, IALG_MemRec *);

extern XDAS_Int32 MP4VENC_TI_initObj(IALG_Handle,
                                const IALG_MemRec *,
                                IALG_Handle,
                                const IALG_Params *);

extern void MP4VENC_TI_moved(IALG_Handle,
                               const IALG_MemRec *,
                               IALG_Handle,
                               const IALG_Params *);

extern XDAS_UInt32 MP4VENC_TI_numAlloc(Void);

extern XDAS_Int32 MP4VENC_TI_encode(IMP4VENC_Handle handle, IVIDEO1_BufDescIn *InBufs,
            XDM_BufDesc *OutBufs, IMP4VENC_InArgs *Inargs,
            IMP4VENC_OutArgs *Outargs);

#endif	/* MP4VENC_TI_ */

/* ======================================================================== */
/* End of file : mp4venc_ti.h                                             */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2006 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

