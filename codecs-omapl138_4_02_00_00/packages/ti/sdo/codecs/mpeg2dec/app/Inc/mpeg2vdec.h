/*
 *  Copyright 1999 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  U.S. Patent Nos. 5,283,900  5,392,448
 */

/*
 * (C) Copyright 2000	 Texas Instruments Inc.  All rights reserved.
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
 *  ======== mpeg2vdec.h ========
 *  This header defines all types, constants, and functions used by 
 *  applications that use the MPEG2VDEC algorithm.
 *
 *  Applications that use this interface enjoy type safety and
 *  the ability to incorporate multiple implementations of the MPEG2VDEC
 *  algorithm in a single application at the expense of some
 *  additional indirection.
 */
#ifndef MPEG2VDEC_
#define MPEG2VDEC_

#ifdef ENABLE_VC
#include <alg.h>
#include <ialg.h>
#else
#include <ti/sdo/fc/utils/api/alg.h> 
#include <ti/xdais/ialg.h>
#endif
#include "impeg2vdec.h"



#define	  MPEG2DEC_GETSTATUS		IMPEG2VDEC_GETSTATUS 
#define   MPEG2DEC_SETPARAMS		IMPEG2VDEC_SETPARAMS
#define   MPEG2DEC_RESET			IMPEG2VDEC_RESET
#define   MPEG2DEC_SETDEFAULT		IMPEG2VDEC_SETDEFAULT
#define   MPEG2DEC_FLUSH			IMPEG2VDEC_FLUSH 
#define   MPEG2DEC_GETBUFINFO		IMPEG2VDEC_GETBUFINFO
#define   MPEG2DEC_GETVERSION		IMPEG2VDEC_GETVERSION
/*
 *  ======== MPEG2VDEC_Handle ========
 *  MPEG2VDEC algorithm instance handle
 */
typedef struct IMPEG2VDEC_Obj *MPEG2VDEC_Handle;

/*
 *  ======== MPEG2VDEC_Params ========
 *  MPEG2VDEC algorithm instance creation parameters
 */
typedef struct IMPEG2VDEC_Params MPEG2VDEC_Params;

/*
 *  ======== MPEG2VDEC_PARAMS ========
 *  Default instance parameters
 */
#define MPEG2VDEC_PARAMS IMPEG2VDEC_PARAMS
/*
 *  ======== MPEG2VDEC_apply ========
 *  Apply a MPEG2VDEC filter to the input array and place results in the
 *  output array.
 */
/*extern XDAS_Int32 MPEG2VDEC_apply(MPEG2VDEC_Handle handle, XDM_BufDesc *inBufs, 
               XDM_BufDesc *outBufs, IVIDDEC_InArgs *inargs, 
               IVIDDEC_OutArgs *outargs);*/
extern XDAS_Int32 MPEG2VDEC_apply(MPEG2VDEC_Handle handle, XDM1_BufDesc *inBufs, 
               XDM_BufDesc *outBufs, IVIDDEC2_InArgs *inargs, 
               IVIDDEC2_OutArgs *outargs);//XDM1.0

/*
 *  ======== MPEG2VDEC_create ========
 *  Create an instance of a MPEG2VDEC object.
 */
extern MPEG2VDEC_Handle MPEG2VDEC_create(const IMPEG2VDEC_Fxns *fxns, const MPEG2VDEC_Params *prms);

/*
 *  ======== MPEG2VDEC_delete ========
 *  Delete a MPEG2VDEC instance object
 */
extern Void MPEG2VDEC_delete(MPEG2VDEC_Handle handle);

#endif  /* MPEG2VDEC_ */
