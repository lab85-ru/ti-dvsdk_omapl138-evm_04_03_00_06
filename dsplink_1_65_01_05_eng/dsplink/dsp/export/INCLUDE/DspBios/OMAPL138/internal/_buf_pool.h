/** ============================================================================
 *  @file   _buf_pool.h
 *
 *  @path   $(DSPLINK)/dsp/src/pools/
 *
 *  @desc   Internal declarations for the buffer pool.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#ifndef _BUF_POOL_
#define _BUF_POOL_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <buf.h>
#include <pool.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ----------------------------------------------------------------------------
 *  @name   BUFPOOL_Obj
 *
 *  @desc   Buffer object for the pool, representing a BUF for a specific size.
 *
 *  @field  bufSize
 *              Size (in MADUs) of the buffers in the buffer pool.
 *  @field  bufList
 *              List of buffers in the buffer pool.
 *  ----------------------------------------------------------------------------
 */
typedef struct BUFPOOL_Obj_tag {
    Uint32      bufSize ;
    BUF_Handle  bufList ;
} BUFPOOL_Obj, *BUFPOOL_Handle ;

/** ----------------------------------------------------------------------------
 *  @name   BUFPOOL_State
 *
 *  @desc   BUFPOOL state object. One instance of this object is created each
 *          time BUFPOOL_open () is called.
 *
 *  @field  numBufObjs
 *              Number of buffer objects configured in the pool.
 *  @field  bufObjs
 *              Array of buffer objects for various buffer sizes.
 *  ----------------------------------------------------------------------------
 */
typedef struct BUFPOOL_State_tag {
    Uint32          numBufObjs ;
    BUFPOOL_Handle  bufObjs ;
} BUFPOOL_State ;


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* _BUF_POOL_ */
