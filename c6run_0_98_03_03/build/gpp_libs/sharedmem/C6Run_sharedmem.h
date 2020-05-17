/*
 * C6Run_sharedmem.h
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#ifndef _C6RUN_SHAREDMEM_H_
#define _C6RUN_SHAREDMEM_H_

/*  ----------------------------------- Standard C Headers            */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/ 

// Constants
#define NUM_TABLE_BINS_BITS           (12)
#define MIN_ALIGNMENT_BITS            (3)
#define C6RUN_SHAREDMEM_MALLOC_MAGIC        (0x43365231)
#define C6RUN_SHAREDMEM_MEMALIGN_MAGIC      (0x43365232)

// Derived Constants
#define NUM_TABLE_BINS                      (0x1 << NUM_TABLE_BINS_BITS)
#define MIN_ALIGNMENT                       (0x1 << MIN_ALIGNMENT_BITS)

// Macro functions
#define SET_MALLOC_MAGIC(P)                 (C6RUN_SHAREDMEM_MALLOC_MAGIC ^ ((uint32_t)P))
#define IS_MALLOC_MAGIC(M,P)                ((((uint32_t)M) ^ ((uint32_t)P)) == C6RUN_SHAREDMEM_MALLOC_MAGIC)
#define SET_MEMALIGN_MAGIC(P)               (C6RUN_SHAREDMEM_MEMALIGN_MAGIC ^ ((uint32_t)P))
#define IS_MEMALIGN_MAGIC(M,P)              ((((uint32_t)M) ^ ((uint32_t)P)) == C6RUN_SHAREDMEM_MEMALIGN_MAGIC)

#define GET_BIN_NUMBER(P)                   (((((uint32_t) P) - ((uint32_t)C6RUN_SHAREDMEM_localCmemBaseAddr)) >> C6RUN_SHAREDMEM_binBitShift) & (NUM_TABLE_BINS - 1))

#define C6RUN_SHAREDMEM_ALIGN(S)            ((((uint32_t)S) + (MIN_ALIGNMENT - 1)) & (~(MIN_ALIGNMENT - 1)))

#define BUF2PTR(B)                          ((void *)((C6RUN_SHAREDMEM_Buffer *)B + 1))
#define PTR2BUF(P)                          ((pC6RUN_SHAREDMEM_Buffer)((C6RUN_SHAREDMEM_Buffer *)P - 1))

#define GET_BUF_SIZE(B)                     ((size_t) (((size_t *)B)[-1] & 0xFFFFFFFC) - (2 * sizeof(size_t)))

#define C6RUN_SHAREDMEM_MALLOC_LOCK()       pthread_mutex_lock(&buffTableMutex)
#define C6RUN_SHAREDMEM_MALLOC_UNLOCK()     pthread_mutex_unlock(&buffTableMutex)

#if (0)
  #define FAST_BUFFER_LOOKUP
#endif


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/

// C6RUN_Buffer
typedef struct _C6RUN_SHAREDMEM_BUFFER_
{
  void       *next;
  uint32_t    magic;
}
C6RUN_SHAREDMEM_Buffer, *pC6RUN_SHAREDMEM_Buffer;


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern volatile bool  C6RUN_SHAREDMEM_initialized;

extern void          *C6RUN_SHAREDMEM_localBaseAddr;
extern void          *C6RUN_SHAREDMEM_remoteBaseAddr;

extern void          *C6RUN_SHAREDMEM_localCmemBaseAddr;
extern void          *C6RUN_SHAREDMEM_localCmemEndAddr;
extern void          *C6RUN_SHAREDMEM_localCodeBaseAddr;
extern void          *C6RUN_SHAREDMEM_localCodeEndAddr;

extern int32_t        C6RUN_SHAREDMEM_binBitShift;
extern uint32_t       C6RUN_SHAREDMEM_binSize;

extern pthread_mutex_t buffTableMutex;


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

// Memory subsystem initialization routines
extern int32_t                  C6RUN_SHAREDMEM_init (void);
extern void                     C6RUN_SHAREDMEM_exit (void);

// Memory helper routines
extern pC6RUN_SHAREDMEM_Buffer  C6RUN_SHAREDMEM_getBuf(void * ptr);
extern void                     C6RUN_SHAREDMEM_insertInBin(pC6RUN_SHAREDMEM_Buffer buf, void *ptr, size_t size);
extern void                     C6RUN_SHAREDMEM_removeFromBin(pC6RUN_SHAREDMEM_Buffer buf, void *ptr, size_t size);

extern void                    *C6RUN_SHAREDMEM_lookupBuffer(void *ptr, size_t *buffSize);
extern void                     C6RUN_SHAREDMEM_printStats(void);
extern void                    *C6RUN_SHAREDMEM_local2Remote(void *localPtr);
extern void                    *C6RUN_SHAREDMEM_remote2Local(void *remotePtr);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_C6RUN_SHAREDMEM_H_