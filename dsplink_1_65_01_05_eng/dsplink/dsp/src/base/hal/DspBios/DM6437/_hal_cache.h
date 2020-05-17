/** ============================================================================
 *  @file   _hal_cache.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/hal/DspBios/DM6437/
 *
 *  @desc   Internal definitions for cache functionality of the Hardware
 *          Abstraction Layer Module.
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


#if !defined (_HAL_CACHE_H)
#define _HAL_CACHE_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <bcache.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  HAL_CACHE_INV
 *
 *  @desc   Invalidate cache contents for specified memory region.
 *  ============================================================================
 */
#define HAL_CACHE_INV(addr, size)      BCACHE_inv (addr, size, TRUE)

/** ============================================================================
 *  @macro  HAL_CACHE_WB
 *
 *  @desc   Write-back cache contents for specified memory region.
 *  ============================================================================
 */
#define HAL_CACHE_WB(addr, size)       BCACHE_wb (addr, size, TRUE)

/** ============================================================================
 *  @macro  HAL_CACHE_WBINV
 *
 *  @desc   Write-back and invalidate cache contents for specified memory region
 *  ============================================================================
 */
#define HAL_CACHE_WBINV(addr, size)    BCACHE_wbInv (addr, size, TRUE)

/** ============================================================================
 *  @macro  HAL_CACHE_WBALL
 *
 *  @desc   Write-back complete cache contents.
 *  ============================================================================
 */
#define HAL_CACHE_WBALL                BCACHE_wbAll ()

/** ============================================================================
 *  @macro  HAL_CACHE_WBINVALL
 *
 *  @desc   Write-back and invaliate complete cache contents.
 *  ============================================================================
 */
#define HAL_CACHE_WBINVALL             BCACHE_wbInvAll ()


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_HAL_CACHE_H) */
