/*
 * c6run.h
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

#ifndef _C6RUN_H_
#define _C6RUN_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/ 


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/

typedef struct _C6RUN_ASYNC_STRUCT_ C6RUN_RPC_AsyncStruct,*C6RUN_RPC_AsyncHandle;


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern volatile bool  C6RUN_initialized;
extern volatile int   C6RUN_retVal;


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

// Setup and terminate functions for C6Run
extern int      C6RUN_init (void);
extern void     C6RUN_exit (void);


// Memory buffer management routines
extern void    *C6RUN_MEM_malloc(size_t size);
extern void    *C6RUN_MEM_calloc(size_t nelem, size_t elsize);
extern void    *C6RUN_MEM_realloc(void *oldPtr,size_t size);
extern void    *C6RUN_MEM_memalign(size_t alignment, size_t size);
extern void     C6RUN_MEM_free(void* ptr);


// Cache management routines
extern void     C6RUN_CACHE_globalInv( void );
extern void     C6RUN_CACHE_globalWb( void );
extern void     C6RUN_CACHE_globalWbInv( void );
extern void     C6RUN_CACHE_inv(void *ptr,size_t size);
extern void     C6RUN_CACHE_wb(void *ptr,size_t size);
extern void     C6RUN_CACHE_wbInv(void *ptr,size_t size);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_C6RUN_H_
