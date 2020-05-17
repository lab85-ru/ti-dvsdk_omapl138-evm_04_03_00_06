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
 *  ======== rmp.c ========
 *  Description:
 *
 *  This memory manager provides general heap management one memory segment.
 *  
 *  Notes:
 *
 *  Memory blocks are allocated from the end of the first free memory
 *  block large enough to satisfy the request.
 */

#include <xdc/std.h>
#include <ti/sdo/utils/trace/gt.h>
#include <stdlib.h>     /* For malloc etc */
extern GT_Mask ti_sdo_fc_dman3_GTMask;

//#include <ti/sdo/fc/utils///DBC.h>

#include "rmm.h"
#include "rmmp.h"
#include "_rmmp.h"

#ifdef xdc_target__os_Linux
#include "sharedMem.h"
#endif

#define OFFSET(base, address) ((UInt32)address - (UInt32)base)
#define RMMHANDLE(handle)  (RMM_Handle)((UInt32)(handle) + handle->rmmOffset) 
#define ALIGN32(x) ((((UInt32)(x) + 3) / 4) * 4)

/*  extern unsigned int RMMP_MAXINSTANCES;*/

static unsigned int shmAddrs[RMMP_MAXINSTANCES];
static Int shmIndex[RMMP_MAXINSTANCES];
static Int shmCount = 0;

static void setInternalState(void * addr); 
static Void *shmAlloc(Int segid, UInt32 size, UInt32 align);
static Bool shmFree(Int segid, Void *addr, UInt32 size);

/*
 *  ======== RMP_alloc ========
 */
Bool RMP_alloc(RMP_Handle rmp, UInt32 size, UInt32 *addr)
{
    Bool                status;

    /*
     * Call RMM_create  
     */
    status = RMM_alloc(RMMHANDLE(rmp), size, addr); 

    return (status);
}

/*
 *  ======== RMP_check ========
 */
Void RMP_check(RMP_Handle rmp, RMP_Stat *stat)
{
    RMM_check(RMMHANDLE(rmp), (RMM_Stat *)stat);
}

/*
 *  ======== RMP_create ========
 */
RMP_Handle RMP_create(RMP_Attrs *attrs)
{
    RMP_Obj         *handle = NULL;
    Int             shmId = -1;
    unsigned int    shmSize = 0;
    Void            * addr = NULL;
    RMM_Attrs       rmmAttrs;

    /* Create a shared memory object RMP_Obj using the key and the lock 
    Size of this will depend numResources and RMM_Obj etc  */
   
    GT_assert(CURTRACE, shmCount < RMMP_MAXINSTANCES); 

    shmSize = sizeof(RMP_Obj) + sizeof(RMM_Obj) + sizeof(UInt32); 
    shmSize = ALIGN32(shmSize);
    shmSize += (attrs->maxFreeListBlocks * sizeof(RMM_Header));
    shmSize = ALIGN32(shmSize);

#ifdef xdc_target__os_Linux
    addr = SHM_getObj(attrs->lock, shmSize, (key_t)attrs->key, 
            (SHM_InitFxn)&setInternalState, &shmId);
#else
    addr = (void *)malloc(shmSize);
    if (NULL != addr) {
        setInternalState(addr);
        shmId = 0;
    }
#endif

    if (shmId != -1) {

        handle = (RMP_Obj *)addr;

        if (0 == handle->refCount) {

            /* RMM_create has to happen */ 
            rmmAttrs.segid = shmCount;          /* Use shmCount as segId */ 
            rmmAttrs.base = attrs->base;
            rmmAttrs.length = attrs->length;
            rmmAttrs.maxFreeListBlocks = attrs->maxFreeListBlocks;
            /* My alloc fxn will use the sharedMem address to increment an 
               offset pointer */ 

            rmmAttrs.allocFxn = shmAlloc;
            rmmAttrs.freeFxn = shmFree;

            /* Store the shmCount in the  SHM_Obj part of the handle */
            handle->shmCount = shmCount;
            shmCount++;
        }

        /* Successful acquisition of shared Mem object, so store in array 
           for use by allocFxn */ 
        shmAddrs[handle->shmCount] = (unsigned int)addr;
        shmIndex[handle->shmCount] = sizeof(RMP_Obj); /* Change this to work for
                the allocFxn */ 

        if (handle->refCount == 0) {
            /* Whatever the RMM_create returns, store that as an offset from 
               shmBase */
            handle->rmmOffset = OFFSET(handle, RMM_create(&rmmAttrs));
        }
        handle->refCount++;

        /* Store the sharedMemId in the RMP_Obj */ 
        handle->sharedMemId = shmId;
    }

    return (handle);
}

/*
 *  ======== RMP_delete ========
 */
/* ARGSUSED */
Void RMP_delete(LockMP_Handle lock, RMP_Handle handle)
{
#ifdef xdc_target__os_Linux
    Int shmId = handle->sharedMemId;                   /* Shared Mem Id */
#endif
    Void * base = (Void *)(shmAddrs[handle->shmCount]);    /* Base Address */ 

    /* Decrease the refCount */
    handle->refCount--;   

    /* If the referenceCount is zero, call the RMM delete */ 
    if (0 == handle->refCount) {
        RMM_delete(RMMHANDLE(handle));
    }

#ifdef xdc_target__os_Linux
    /* Free the shared memory segment */
    SHM_freeObj(lock, base, shmId);
#else
    free(base);
#endif
}

/*
 *  ======== RMP_exit ========
 */
Void RMP_exit(Void)
{
}

/*
 *  ======== RMP_free ========
 */
/* ARGSUSED */
Bool RMP_free(RMP_Handle rmp, UInt32 addr, UInt32 size)
{
    Bool status = TRUE;

    status = RMM_free(RMMHANDLE(rmp), addr, size); 

    return (status);
}

/*
 *  ======== RMP_init ========
 */
Void RMP_init(Void)
{
}

static void setInternalState(void * addr) 
{

    RMP_Obj * rmmp = (RMP_Obj *)addr;
    /* In the init function of the object, call RMM_create */ 
    rmmp->refCount = 0;
}

/* ARGSUSED */
static Void *shmAlloc(Int segid, UInt32 size, UInt32 align)
{

    unsigned int baseAddr = shmAddrs[segid];        /* We stored the shmCount
                                                       in the segId */  
    Int index  = shmIndex[segid]; 
    Void * returnAddr = NULL; 
    
    returnAddr = (void *)(baseAddr + index);  
    shmIndex[segid] += size;

    return (returnAddr);
}

/* ARGSUSED */
static Bool shmFree(Int segid, Void *addr, UInt32 size)
{
    return (TRUE);
}

/*
 *  @(#) ti.sdo.fc.utils; 1, 0, 2,286; 12-1-2010 17:26:08; /db/atree/library/trees/fc/fc-p01x/src/ xlibrary

 */

