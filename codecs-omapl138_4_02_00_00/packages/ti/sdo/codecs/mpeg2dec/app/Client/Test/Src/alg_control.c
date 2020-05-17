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
 *  ======== alg_control.c ========
 *  ALG_control implementation.  This is common to all implementations
 *  of the ALG module.
 */
#ifdef ENABLE_VC
#include <std.h>
#include <alg.h>
#include <ialg.h>
#else
#include <ti/bios/include/std.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/xdais/ialg.h>
#endif


#ifdef C6000
#pragma CODE_SECTION(ALG_control, ".externaltext")
#endif

/*
 *  ======== ALG_control ========
 */
Int ALG_control(ALG_Handle alg, IALG_Cmd cmd, IALG_Status *statusPtr)
{
    if (alg && alg->fxns->algControl) {
        return (alg->fxns->algControl(alg, cmd, statusPtr));
    }

    return (IALG_EFAIL);
}
