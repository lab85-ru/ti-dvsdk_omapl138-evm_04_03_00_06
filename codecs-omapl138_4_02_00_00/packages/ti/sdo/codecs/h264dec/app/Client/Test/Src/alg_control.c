/* 
 *  Copyright 2008
 *  Texas Instruments Incorporated
 *
 *  All rights reserved.  Property of Texas Instruments Incorporated
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 * 
 */
/*
 *  ======== alg_control.c ========
 *  ALG_control implementation.  This is common to all implementations
 *  of the ALG module.
 */

#include <ti/bios/include/std.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/xdais/ialg.h>


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
/*
 *  @(#) ti.sdo.fc.utils.api; 1, 0, 0,183; 9-10-2008 22:26:48; /db/atree/library/trees/fc-f28x/src/
 */

