/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/

/*
 *  ======== msgqConfig.c ========
 */

#include <std.h>

/* DSP/BIOS interface files */
#include <msgq.h>
#include <pool.h>

/* DSP/BIOS Link interface files */
#include <zcpy_mqt.h>
#include <sma_pool.h>

/* Common header file shared between GPP and DSP */
#include "../common/common.h"

/* The number of pools allocators configured */
#define NUMALLOCATORS 1

/* The number of local message queues. */
#define NUMMSGQUEUES   6

/* MGSQ parameters for the array of pools */
static ZCPYMQT_Params  mqtParams = {APP_POOL_ID} ;  // POOL ID Number

/* Shared memory allocator params */
static SMAPOOL_Params smaPoolParams = {
    0,            /* POOL ID      */
    TRUE          /* Exact Match  */
};

/* Array of local message queues */
static MSGQ_Obj msgQueues[NUMMSGQUEUES];

/* Array of Transports */
static MSGQ_TransportObj transports[MAX_PROCESSORS] = {
    MSGQ_NOTRANSPORT,    /* Local processor 0 transport  */
    {
        ZCPYMQT_init,    /* Remote processor 1 transport */
        &ZCPYMQT_FXNS,
        &mqtParams,
        NULL,
        ID_GPP
    }
};

/* MSGQ config required structure */
MSGQ_Config MSGQ_config = {
    msgQueues,         /* Array of message queues      */
    transports,        /* Array of transports          */
    NUMMSGQUEUES,      /* # of message queues in array */
    MAX_PROCESSORS,    /* # of transports in array     */
    0,                 /* 1st uninitialized msg queue  */
    MSGQ_INVALIDMSGQ,  /* no error handler queue       */
    POOL_INVALIDID     /* allocator id for errors      */
};

/* Array of pools */
static POOL_Obj allocators[NUMALLOCATORS] = {
    {
        SMAPOOL_init,                  /* Allocator init function       */
        (POOL_Fxns *)&SMAPOOL_FXNS,    /* Allocator interface functions */
        &smaPoolParams,                /* Allocator configuration       */
        NULL
    }
};

POOL_Config  POOL_config = {allocators, NUMALLOCATORS};
