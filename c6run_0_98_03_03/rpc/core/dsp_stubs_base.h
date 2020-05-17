/*
 * dsp_stubs_base.h
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * Copyright (C) 2010 Yaman Umuroglu 
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
 
/************************************************************************
*  Header file for DSP->GPP RPC definitions, needed for DSP side stubs  *
************************************************************************/


#ifndef _DSP_STUBS_BASE_H
#define _DSP_STUBS_BASE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rpc_common.h"

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/* DSP cache is enabled by default - disabling needs changes in platform.tci as well */
#define DSP_CACHE_ENABLED

/* Macros for data marshalling */

#define RPC_PARAM(type, v_ptr)  memcpy(p, v_ptr, sizeof(type)); p += sizeof(type);

/* Define macros according to whether the DSP cache is enabled (writebacks on/off) */

#ifdef DSP_CACHE_ENABLED

    #define RPC_INIT(fname, fsign)  char *p = rpc_request_buffer; \
                                    int wbinv_needed = 0; \
                                    unsigned int rpc_name_len = strlen(fname);  \
                                    unsigned int rpc_sign_len = strlen(fsign);  \
                                    memcpy(p, &rpc_name_len, sizeof(unsigned int)); \
                                    p += sizeof(unsigned int);  \
                                    memcpy(p, fname, rpc_name_len); p += rpc_name_len;  \
                                    memcpy(p, &rpc_sign_len, sizeof(unsigned int)); \
                                    p += sizeof(unsigned int);  \
                                    memcpy(p, fsign, rpc_sign_len); p += rpc_sign_len;

    #define RPC_PERFORM()           p[0] = 0; p++; \
                                    rpc_request_length = (unsigned int)( p - rpc_request_buffer ); \
                                    if(wbinv_needed) BCACHE_wbInvAll(); \
                                    rpc_perform();

    #define RPC_BUFFER(type, v_ptr) wbinv_needed = 1; \
                                    RPC_PARAM(type, v_ptr)                                
#else

    #define RPC_INIT(fname, fsign)  char *p = rpc_request_buffer; \
                                    unsigned int rpc_name_len = strlen(fname);  \
                                    unsigned int rpc_sign_len = strlen(fsign);  \
                                    memcpy(p, &rpc_name_len, sizeof(unsigned int)); \
                                    p += sizeof(unsigned int);  \
                                    memcpy(p, fname, rpc_name_len); p += rpc_name_len;  \
                                    memcpy(p, &rpc_sign_len, sizeof(unsigned int)); \
                                    p += sizeof(unsigned int);  \
                                    memcpy(p, fsign, rpc_sign_len); p += rpc_sign_len;

    #define RPC_PERFORM()           p[0] = 0; p++; \
                                    rpc_request_length = (unsigned int)( p - rpc_request_buffer ); \
                                    rpc_perform();

    #define RPC_BUFFER(type, v_ptr) RPC_PARAM(type, v_ptr)       
                         
#endif

/* Macro to unpack result from buffer by casting it to desired type */
#define RPC_GETRESULT(type)     *((type *) rpc_response_buffer)

/***********************************************************
* Global Typedef Declarations                              *
***********************************************************/

/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern char rpc_request_buffer[RPC_BUFSZ];
extern char rpc_response_buffer[RPC_RESPSZ];
extern unsigned int rpc_request_length;

/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern void rpc_perform();
extern void* rpc_malloc(size_t);
extern void rpc_free(void *);
extern unsigned int rpc_translate_address(unsigned int, unsigned int);
extern void BCACHE_wbInvAll();  /* exported from the DSP side lib */

/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_DSP_STUBS_BASE_H

