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
 *  ======== mpeg2vdec_ti.h ========
 *  Vendor specific (TI) interface header for MPEG2VDEC algorithm.
 *
 *  Applications that use this interface enjoy type safety and
 *  and minimal overhead at the expense of being tied to a
 *  particular MPEG2VDEC implementation.
 *
 *  This header only contains declarations that are specific
 *  to this implementation.  Thus, applications that do not
 *  want to be tied to a particular implementation should never
 *  include this header (i.e., it should never directly
 *  reference anything defined in this header.)
 */
#ifndef MPEG2VDEC_TI_
#define MPEG2VDEC_TI_

#ifdef ENABLE_VC
#include <ialg.h>
#include <idma3.h>
#else
#include <ti/xdais/ialg.h>
#include <ti/xdais/idma3.h>
#endif
#include "impeg2vdec.h"


/*
 *  ======== MPEG2VDEC_TI_IDMA3 ========
 *  TI's implementation of M4H3 IDMA3 interface
 */
extern IDMA3_Fxns MPEG2VDEC_TI_IDMA3;

/*
 *  ======== MPEG2VDEC_TI_IALG ========
 *  TI's implementation of MPEG2VDEC's IALG interface
 */
extern IALG_Fxns MPEG2VDEC_TI_IALG; 

/*
 *  ======== MPEG2VDEC_TI_IMPEG2VDEC ========
 *  TI's implementation of MPEG2VDEC's IMPEG2VDEC interface
 */
extern IMPEG2VDEC_Fxns MPEG2VDEC_TI_IMPEG2VDEC; 

/*
 *  ======== Vendor specific methods  ========
 *  The remainder of this file illustrates how a vendor can
 *  extend an interface with custom operations.
 *
 *  The operations below simply provide a type safe interface 
 *  for the creation, deletion, and application of TI's MPEG2VDEC filters.
 *  However, other implementation specific operations can also
 *  be added.
 */

/*
 *  ======== MPEG2VDEC_TI_Handle ========
 */
typedef struct MPEG2VDEC_TI_Obj *MPEG2VDEC_TI_Handle;

/*
 *  ======== MPEG2VDEC_TI_Params ========
 *  We don't add any new parameters to the standard ones defined
 *  by IMPEG2VDEC.
 */
typedef IMPEG2VDEC_Params MPEG2VDEC_TI_Params;

/*
 *  ======== MPEG2VDEC_TI_PARAMS ========
 *  Define our defult parameters.
 */
#define MPEG2VDEC_TI_PARAMS   IMPEG2VDEC_PARAMS



#endif  /* MPEG2VDEC_TI_ */
