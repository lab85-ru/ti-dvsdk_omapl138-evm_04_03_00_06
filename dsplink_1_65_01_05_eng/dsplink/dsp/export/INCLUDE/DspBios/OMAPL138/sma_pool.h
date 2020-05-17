/** ============================================================================
 *  @file   sma_pool.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Defines the exported interface of the SMAPOOL - Shared Memory
 *          Allocator POOL.
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


#if !defined (SMA_POOL_H)
#define SMA_POOL_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <pool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   SMAPOOL_Params
 *
 *  @desc   Parameters required for initialization of the SMA pool.
 *
 *  @field  poolId
 *              ID of the pool being opened.
 *  @field  exactMatchReq
 *              Flag indicating while allocating /freeing exact size match has
 *              to be performed for buffers.
 *  ============================================================================
 */
typedef struct SMAPOOL_Params_tag {
    Uint16     poolId ;
    Bool       exactMatchReq ;
} SMAPOOL_Params ;

/** ============================================================================
 *  @name   SMAPOOL_FXNS
 *
 *  @desc   Declaration of the function table interface for the SMA pool.
 *  ============================================================================
 */
extern const POOL_Fxns SMAPOOL_FXNS;


/** ============================================================================
 *  @func   SMAPOOL_init
 *
 *  @desc   Performs global initialization of the SMA pool.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    SMAPOOL_exit
 *  ============================================================================
 */
Void SMAPOOL_init () ;


/** ============================================================================
 *  @func   SMAPOOL_exit
 *
 *  @desc   Performs global finalization of the SMA pool.
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
 *  @see    SMAPOOL_init ()
 *  ============================================================================
 */
#define SMAPOOL_exit    FXN_F_nop


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SMA_POOL_H) */
