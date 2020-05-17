/** ============================================================================
 *  @file   _mplist.h
 *
 *  @path   $(DSPLINK)/dsp/src/mplist/
 *
 *  @desc   Defines the internal interfaces and data structures for the API
 *          sub-component MPLIST.
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


#if !defined (_MPLIST_H)
#define _MPLIST_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <mplistdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   MPLIST_init
 *
 *  @desc   This function initialize the MPLIST component.
 *          This function does the system level initialization. This initializes
 *          all internal MPLIST structures.This function should be called even
 *          before any other MPLIST functions are called
 *
 *  @arg    procId
 *              ID of the processor with which the MPLIST component is shared.
 *  @arg    shAddr
 *              Address in shared memory for the MPLIST control structure.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EALLOC
 *              Memory allocation failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */

Int
MPLIST_init (IN Uint16 procId, IN Uint32 shAddr) ;


/** ============================================================================
 *  @func   MPLIST_exit
 *
 *  @desc   This function finalizes the MPLIST component.
 *
 *  @arg    procId
 *              ID of the processor with which the MPLIST component is shared.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EFREE
 *              Operation failed due to memory error.
 *          SYS_EINVAL
 *              Invalid Parameter passed.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */

Int
MPLIST_exit (IN Uint16 procId) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_MPLIST_H) */
