/** ============================================================================
 *  @file   _mem_usr.h
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          MEM for user-side.
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


#if !defined (_MEM_USR_H)
#define _MEM_USR_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   _MEM_USR_init
 *
 *  @desc   Initializes the MEM USR subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_init (Void) ;


/** ============================================================================
 *  @func   _MEM_USR_exit
 *
 *  @desc   This function frees up all resources used by the SYNC USR
 *          subcomponent.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_exit (Void) ;


/** ============================================================================
 *  @func   _MEM_USR_alloc
 *
 *  @desc   Allocates the specified number of bytes.
 *
 *  @arg    ptr
 *              Location where pointer to allocated memory will be returned.
 *  @arg    bytes
 *              Number of bytes to allocate.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  bytes must be greater than 0.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_alloc (OUT Void ** ptr, IN Uint32 bytes) ;


/** ============================================================================
 *  @func   _MEM_USR_calloc
 *
 *  @desc   Allocates the specified number of bytes and clears them by filling
 *          it with 0s.
 *
 *  @arg    ptr
 *              Location where pointer to allocated memory will be returned.
 *  @arg    bytes
 *              Number of bytes to allocate.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  bytes must be greater than 0.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_calloc (OUT Void ** ptr, IN Uint32 bytes) ;


/** ============================================================================
 *  @func   _MEM_USR_free
 *
 *  @desc   Frees up the allocated chunk of memory.
 *
 *  @arg    ptr
 *              Pointer to pointer to start of memory to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  ptr must be a valid pointer.
 *
 *  @leave  *ptr is reset to NULL on success.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_MEM_USR_free (IN Void ** ptr) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (_MEM_USR_H) */
