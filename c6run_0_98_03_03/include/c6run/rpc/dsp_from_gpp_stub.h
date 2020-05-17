/*
 * dsp_from_gpp_stub.h
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
 
#ifndef _DSP_FROM_GPP_STUB_H_
#define _DSP_FROM_GPP_STUB_H_

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

#define RPC_TERMINATE_CACHE_CMDS      (0)
#define RPC_GLOBAL_CACHE_WRITEBACK    (1)
#define RPC_GLOBAL_CACHE_INVALIDATE   (2)
#define RPC_BUFFER_CACHE_WRITEBACK    (3)
#define RPC_BUFFER_CACHE_INVALIDATE   (4)

/* Init macros for when the function does not return anything */
#define RPC_INIT_VOIDRETURN(RT) \
    uint32_t *__C6RUN_msgPtr__; \
    __C6RUN_msgPtr__ = (uint32_t*) msgBuffer; \
    __C6RUN_msgPtr__++;
/* Init macros for when the function does return a value */
#define RPC_INIT(RT) \
    RT __C6RUN_fxnResult__; \
    uint32_t *__C6RUN_msgPtr__,*__C6RUN_msgPtrOrig__; \
    __C6RUN_msgPtrOrig__ = __C6RUN_msgPtr__ = (uint32_t*) msgBuffer; \
    __C6RUN_msgPtr__++;

/* Macro to process any input cache commands in message */
#define RPC_PROCESS_CACHE_IN_OPS() \
    do { \
      uint32_t currVal = *__C6RUN_msgPtr__++; \
      while(currVal != RPC_TERMINATE_CACHE_CMDS) \
      { \
        if (RPC_BUFFER_CACHE_INVALIDATE==currVal) \
        { \
          void *__C6RUN_ptr__   = (void *) *__C6RUN_msgPtr__++; \
          size_t __C6RUN_size__ = (size_t) *__C6RUN_msgPtr__++; \
          C6RUN_CACHE_inv(__C6RUN_ptr__,__C6RUN_size__); \
        } \
        else if (RPC_GLOBAL_CACHE_INVALIDATE==currVal) \
        { \
          C6RUN_CACHE_globalInv(); \
        } \
        currVal = *__C6RUN_msgPtr__++; \
      } \
    } while(0)

/* Macro to declare argument variables */
#define RPC_ARG_DECLARATION(V,T) T V;

/* Macro to unpack function arguments from the message */
#define RPC_PARAM_UNPACK(V) \
    do { \
      size_t __C6RUN_size__ = ((sizeof(V) + 3) & (~3u)); \
      memcpy((void *)&V,(void *)__C6RUN_msgPtr__,sizeof(V)); \
      __C6RUN_msgPtr__ = __C6RUN_msgPtr__ + (__C6RUN_size__ >> 2); \
    } while (0)

/* Macro to process any output cache commands in message */
#define RPC_PROCESS_CACHE_OUT_OPS() \
    do { \
      uint32_t currVal = *__C6RUN_msgPtr__++; \
      while(currVal != RPC_TERMINATE_CACHE_CMDS) \
      { \
        if (RPC_BUFFER_CACHE_WRITEBACK==currVal) \
        { \
          void *__C6RUN_ptr__   = (void *) *__C6RUN_msgPtr__++; \
          size_t __C6RUN_size__ = (size_t) *__C6RUN_msgPtr__++; \
          C6RUN_CACHE_wb(__C6RUN_ptr__,__C6RUN_size__); \
        } \
        else if (RPC_GLOBAL_CACHE_WRITEBACK==currVal) \
        { \
          C6RUN_CACHE_globalWb(); \
        } \
        currVal = *__C6RUN_msgPtr__++; \
      } \
    } while(0)

/* Macro to insert result data into the return message */
#define RPC_RESULT_PACK(V) \
    do { \
      size_t __C6RUN_size__ = ((sizeof(V) + 3) & (~3u)); \
      memcpy((void *)__C6RUN_msgPtrOrig__,(void *)&V,sizeof(V)); \
      __C6RUN_msgPtrOrig__ += (__C6RUN_size__ >> 2); \
    } while(0)


/***********************************************************
* Global Struct/Enum/Typedef Declarations                  *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

// C6RUN_CACHE routines
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
    
#endif /* _DSP_FROM_GPP_STUB_H_ */
