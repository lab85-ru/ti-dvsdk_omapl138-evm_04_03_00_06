/** ============================================================================
 *  @file   buf_pool.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines the exported interface of the buffer pool.
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


#ifndef BUF_POOL_
#define BUF_POOL_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <pool.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @name   BUFPOOL_Params
 *
 *  @desc   Parameters required for initialization of the buffer pool.
 *
 *  @field  numBufObjs
 *              Number of entries in the arrays of buffer sizes and numbers in
 *              the params structure.
 *  @field  bufSizes
 *              Array of sizes of the buffers in the buffer pools.
 *              The sizes are in MADUs.
 *              This array is of size numBufObjs.
 *  @field  numBuffers
 *              Array of number of buffers in all buffer pools.
 *              This array is of size numBufObjs.
 *  @field  segId
 *              Segment ID for allocation of the buffers.
 *  ============================================================================
 */
typedef struct BUFPOOL_Params_tag {
    Uint32     numBufObjs ;
    Uint32 *   bufSizes ;
    Uint32 *   numBuffers ;
    Uint16     segId ;
} BUFPOOL_Params ;


/** ============================================================================
 *  @name   BUFPOOL_FXNS
 *
 *  @desc   Declaration of the function table interface for the buffer pool.
 *  ============================================================================
 */
extern const POOL_Fxns BUFPOOL_FXNS ;


/** ============================================================================
 *  @func   BUFPOOL_init
 *
 *  @desc   Performs global initialization of the buffer pool.
 *          This function currently is not required to perform any
 *          initialization and hence is defined to the NOP function.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    BUFPOOL_exit ()
 *  ============================================================================
 */
#define BUFPOOL_init    FXN_F_nop


/** ============================================================================
 *  @func   BUFPOOL_exit
 *
 *  @desc   Performs global finalization of the buffer pool.
 *          This function currently is not required to perform any
 *          finalization and hence is defined to the NOP function.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    BUFPOOL_init ()
 *  ============================================================================
 */
#define BUFPOOL_exit    FXN_F_nop


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* BUF_POOL_ */
