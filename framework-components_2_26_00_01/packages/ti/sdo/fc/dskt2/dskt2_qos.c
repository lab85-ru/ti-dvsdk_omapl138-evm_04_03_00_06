/* 
 * Copyright (c) 2010, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== dskt2_qos.c ========
 *  DSKT2 QOS scratch sharing support functions
 *
 */
#include <xdc/std.h>

#include <ti/sdo/fc/utils/dbc.h>
#include <ti/bios/include/mem.h>

#include "_dskt2.h"
#include "dskt2.h"
#include "dskt2_qos.h"

/*
 * Global private module reference.
 */
extern _DSKT2_ScratchBufferDesc *_DSKT2_scratchDescTbl[DSKT2_NUM_SHAREDHEAPS];

/*
 * ======== DSKT2_QOS_getStat ========
 *  DSKT2 QOS support function
 */

Void  DSKT2_QOS_getStat(Uns heapType, Uns mutexId, Uns *sizeSharedScratch,
    Uns *inHeap)
{
    Int scratchMutexId;
    Int heapId;
    Uns heapStart, heapEnd;
    Uns scratchGroupStart, scratchGroupEnd;
    _DSKT2_ScratchBufferDesc *scrBufDesc;

    DBC_require( (mutexId < DSKT2_NUM_SCRATCH_GROUPS)
                || (mutexId == DSKT2_QOS_ALL_SCRATCHGROUPS));
    GT_assert(ti_sdo_fc_dskt2_GTMask, (mutexId < DSKT2_NUM_SCRATCH_GROUPS)
            || (mutexId == DSKT2_QOS_ALL_SCRATCHGROUPS));

    DBC_require((heapType == DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS)
                || (heapType == DSKT2_QOS_SHARED_SCRATCH_DARAM)
                || (heapType == DSKT2_QOS_SHARED_SCRATCH_SARAM) );

    GT_assert(ti_sdo_fc_dskt2_GTMask,
            (heapType == DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS) || 
            (heapType == DSKT2_QOS_SHARED_SCRATCH_DARAM) || 
            (heapType == DSKT2_QOS_SHARED_SCRATCH_SARAM) );

    _DSKT2_init();

    switch (heapType) {

        case DSKT2_QOS_SHARED_SCRATCH_ALLHEAPS:
                heapStart = 0;
                heapEnd  = DSKT2_NUM_SHAREDHEAPS-1;
                break;

        case DSKT2_QOS_SHARED_SCRATCH_DARAM:
                heapStart = heapEnd = 0;
                *inHeap = _DSKT2_spaceToSeg(IALG_DARAM0);
                break;

        case DSKT2_QOS_SHARED_SCRATCH_SARAM:
                heapStart = heapEnd = 1;
                *inHeap = _DSKT2_spaceToSeg(IALG_SARAM0);
    }

    if (mutexId == DSKT2_QOS_ALL_SCRATCHGROUPS) {
        scratchGroupStart = 0;
        scratchGroupEnd =  DSKT2_NUM_SCRATCH_GROUPS;

    } else {

        scratchGroupStart = scratchGroupEnd = (Uns) mutexId;
    }

    *sizeSharedScratch = 0;

    for (heapId = heapStart; heapId <= heapEnd; heapId++) {

        for (scratchMutexId = scratchGroupStart;
                scratchMutexId <= scratchGroupEnd; scratchMutexId++) {

            scrBufDesc = &_DSKT2_scratchDescTbl[heapId][scratchMutexId];

            if ((scrBufDesc->size > 0) && (scrBufDesc->base != MEM_ILLEGAL )) {
                *sizeSharedScratch += scrBufDesc->size;

            }
        }
    }
}

/*
 *  @(#) ti.sdo.fc.dskt2; 1, 0, 4,300; 12-1-2010 17:24:36; /db/atree/library/trees/fc/fc-p01x/src/ xlibrary

 */

