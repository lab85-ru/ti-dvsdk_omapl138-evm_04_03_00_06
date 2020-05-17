/*
 * gpp_to_dsp_stub.h
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
 
#ifndef _GPP_TO_DSP_STUB_H_
#define _GPP_TO_DSP_STUB_H_

// Get all public (user-callable) c6run declarations
#include "c6run.h"

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

#define RPC_BUFFER_TYPE_NONE          (0)
#define RPC_BUFFER_TYPE_INBUF         (1)
#define RPC_BUFFER_TYPE_OUTBUF        (2)
#define RPC_BUFFER_TYPE_INOUTBUF      (3)
#define RPC_BUFFER_TYPE_RAWBUF        (4)

// Init macro for when the function does not return anything
#define RPC_INIT_VOIDRETURN(F,RT)   \
    uint32_t *__C6RUN_msgPtr__; \
    bool __C6RUN_bGlobalInv__ = false; \
    void *__C6RUN_msg__; \
    C6RUN_init(); \
    __C6RUN_msgPtr__ = (uint32_t*) C6RUN_RPC_allocMsgBuff(&__C6RUN_msg__); \
    *((intptr_t *)__C6RUN_msgPtr__) = (intptr_t) FXN_ADDR_ ## F ## _stub; \
    __C6RUN_msgPtr__ += (sizeof(intptr_t)>>2);
// Init macros for when the function does return a value
#define RPC_INIT(F,RT) \
    RT __C6RUN_fxnResult__; \
    uint32_t *__C6RUN_msgPtr__,*__C6RUN_msgPtrOrig__; \
    bool __C6RUN_bGlobalInv__ = false; \
    void *__C6RUN_msg__; \
    C6RUN_init(); \
    __C6RUN_msgPtrOrig__ = __C6RUN_msgPtr__ = (uint32_t*) C6RUN_RPC_allocMsgBuff(&__C6RUN_msg__); \
    *((intptr_t *)__C6RUN_msgPtr__) = (intptr_t) FXN_ADDR_ ## F ## _stub; \
    __C6RUN_msgPtr__ += (sizeof(intptr_t)>>2);

// Macro for performing global cache operations for input buffers             
#define RPC_GLOBAL_CACHE_IN_OPS()   \
    do { \
      *__C6RUN_msgPtr__++=RPC_GLOBAL_CACHE_INVALIDATE; \
      C6RUN_CACHE_globalWb(); \
    } while(0)

// Macro for performing cache operations for individual input buffers
#define RPC_BUFFER_CACHE_IN_OPS(P, BT)  \
    do { \
      void *__C6RUN_ptr__; \
      size_t __C6RUN_size__; \
      __C6RUN_ptr__ = C6RUN_SHAREDMEM_lookupBuffer((void *)P,&__C6RUN_size__); \
      if (__C6RUN_ptr__ == NULL)\
      { \
        printf("%s: C6RUN_SHAREDMEM_lookupBuffer() failed for pointer 0x%08X\n",__FILE__,(intptr_t)P); \
        exit(1); \
      } \
      *__C6RUN_msgPtr__++ = (uint32_t) RPC_BUFFER_CACHE_INVALIDATE; \
      *__C6RUN_msgPtr__++ = (uint32_t) C6RUN_SHAREDMEM_local2Remote((void *)__C6RUN_ptr__); \
      *__C6RUN_msgPtr__++ = (uint32_t) __C6RUN_size__; \
      if (BT == RPC_BUFFER_TYPE_INBUF) \
      { \
        C6RUN_CACHE_wb(__C6RUN_ptr__,__C6RUN_size__); \
      } \
      else if (BT == RPC_BUFFER_TYPE_INOUTBUF) \
      { \
        C6RUN_CACHE_wbInv(__C6RUN_ptr__,__C6RUN_size__); \
      } \
    } while (0)
    
// Macro for performing global cache operations for output buffers
#define RPC_GLOBAL_CACHE_OUT_OPS() \
    do { \
      *__C6RUN_msgPtr__++=RPC_GLOBAL_CACHE_WRITEBACK; \
      __C6RUN_bGlobalInv__ = true; \
    } while(0)

// Macro for performing cache operations for individual output buffers
#define RPC_BUFFER_CACHE_OUT_OPS(P, BT) \
    do { \
      void *__C6RUN_ptr__; \
      size_t __C6RUN_size__; \
      __C6RUN_ptr__ = C6RUN_SHAREDMEM_lookupBuffer((void *)P,&__C6RUN_size__); \
      if (__C6RUN_ptr__ == NULL)\
      { \
        printf("%s: C6RUN_SHAREDMEM_lookupBuffer() failed for pointer 0x%08X\n",__FILE__,(intptr_t)P); \
        exit(1); \
      } \
      *__C6RUN_msgPtr__++ = (uint32_t) RPC_BUFFER_CACHE_WRITEBACK; \
      *__C6RUN_msgPtr__++ = (uint32_t) C6RUN_SHAREDMEM_local2Remote((void *)__C6RUN_ptr__); \
      *__C6RUN_msgPtr__++ = (uint32_t) __C6RUN_size__; \
      if (BT == RPC_BUFFER_TYPE_OUTBUF) \
      { \
        C6RUN_CACHE_inv(__C6RUN_ptr__,__C6RUN_size__); \
      } \
    } while (0)

// Macro to terminate cache operations part of message buffer
#define RPC_TERMINATE_CACHE_OPS()    \
    do { \
      *__C6RUN_msgPtr__++ = RPC_TERMINATE_CACHE_CMDS; \
    } while (0)
    
// Macro for packing in arguments for the remote procedure call
#define RPC_PARAM_PACK(V) \
    do { \
      size_t __C6RUN_size__ = ((sizeof(V) + 3) & (~3u)); \
      memcpy((void *)__C6RUN_msgPtr__,(void *)&V,sizeof(V)); \
      __C6RUN_msgPtr__ = __C6RUN_msgPtr__ + (__C6RUN_size__>>2); \
    } while(0)
    
#define RPC_PARAM_BUFFER_PACK(V) \
    do { \
      *__C6RUN_msgPtr__++ = (uint32_t) C6RUN_SHAREDMEM_local2Remote((void *)V); \
    } while(0)

#define RPC_PARAM_RAW_BUFFER_PACK(V) \
    do { \
      *p++ = (uint32_t) (V); \
    } while(0)


    
// Macro used for synchronous dispatching the remote procedure call
#define RPC_SYNC_DISPATCH() \
    do { \
      C6RUN_RPC_syncDispatch(__C6RUN_msg__); \
      if (__C6RUN_bGlobalInv__) \
      { \
        C6RUN_CACHE_globalInv(); \
      } \
    } while(0)

// Init macros for async send/begin
#define RPC_INIT_ASYNC_BEGIN(F) \
    uint32_t *__C6RUN_msgPtr__; \
    void *__C6RUN_msg__; \
    C6RUN_RPC_AsyncHandle __C6RUN_asyncHandle__; \
    C6RUN_init(); \
    __C6RUN_msgPtr__ = (uint32_t*) C6RUN_RPC_allocMsgBuff(&__C6RUN_msg__); \
    *__C6RUN_msgPtr__++ = (uint32_t) FXN_ADDR_ ## F ## _stub;

// Macro used to send/begin the async message
#define RPC_ASYNC_DISPATCH_BEGIN() \
    do { \
      C6RUN_RPC_asyncDispatchSend(__C6RUN_msg__, &__C6RUN_asyncHandle__); \
      return __C6RUN_asyncHandle__; \
    } while(0)
    
// Init macros for async recieve/end
#define RPC_INIT_ASYNC_END_VOIDRETURN(RT) \
    void *__C6RUN_msg__; \
    uint32_t *__C6RUN_msgPtr__; \
    __C6RUN_msg__ = (void *) C6RUN_RPC_asyncGetMsg(__C6RUN_asyncHandle__); \
    __C6RUN_msgPtr__ = (uint32_t *) C6RUN_RPC_getMsgBuffer(__C6RUN_msg__);

#define RPC_INIT_ASYNC_END(RT) \
    void *__C6RUN_msg__; \
    uint32_t *__C6RUN_msgPtr__,*__C6RUN_msgPtrOrig__; \
    RT __C6RUN_fxnResult__; \
    __C6RUN_msg__ = (void *) C6RUN_RPC_asyncGetMsg(__C6RUN_asyncHandle__); \
    __C6RUN_msgPtrOrig__ = __C6RUN_msgPtr__ = (uint32_t *) C6RUN_RPC_getMsgBuffer(__C6RUN_msg__);

// Macro used to receive/end the async response
#define RPC_ASYNC_DISPATCH_END() \
    do { \
      C6RUN_RPC_asyncDispatchRecv(__C6RUN_msg__, __C6RUN_asyncHandle__); \
    } while(0)
    
// Macros used for returning from stub
#define RPC_RESULT_UNPACK(V) \
    do { \
      int __C6RUN_size__ = ((sizeof(V) + 3) & (~3u)); \
      memcpy((void *)&V,(void *)__C6RUN_msgPtrOrig__,sizeof(V)); \
      __C6RUN_msgPtr__ = __C6RUN_msgPtr__ + (__C6RUN_size__>>2); \
    } while(0)
#define RPC_RETURN() \
    do { \
      C6RUN_RPC_freeMsgBuff(__C6RUN_msg__); \
      return __C6RUN_fxnResult__; \
    } while(0)
#define RPC_RETURN_VOIDRETURN() \
    do { \
      C6RUN_RPC_freeMsgBuff(__C6RUN_msg__); \
      return; \
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

// C6Run RPC routines
extern void     C6RUN_RPC_syncDispatch(void *msg);
extern void     C6RUN_RPC_asyncDispatchSend(void *msg, C6RUN_RPC_AsyncHandle *hAsync);
extern bool     C6RUN_RPC_asyncIsComplete(C6RUN_RPC_AsyncHandle hAsync);
extern void     C6RUN_RPC_asyncDispatchRecv(void *msg, C6RUN_RPC_AsyncHandle hAsync);
extern void*    C6RUN_RPC_asyncGetMsg( C6RUN_RPC_AsyncHandle hAsync );

extern void*    C6RUN_RPC_getMsgBuffer(void *msg);
extern void*    C6RUN_RPC_allocMsgBuff(void **msg);
extern void     C6RUN_RPC_freeMsgBuff(void *msg);

// C6Run_mem buffer lookup and translation routines
extern void    *C6RUN_SHAREDMEM_lookupBuffer(void *ptr, size_t *buffSize);
extern void    *C6RUN_SHAREDMEM_local2Remote(void *localPtr);
extern void    *C6RUN_SHAREDMEM_remote2Local(void *remotePtr);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif
    
#endif /* _GPP_TO_DSP_STUB_H_ */
