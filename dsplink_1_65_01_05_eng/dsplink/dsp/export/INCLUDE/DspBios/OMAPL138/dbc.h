/** ============================================================================
 *  @file   dbc.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   DSP-side Design By Contract support.
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


#if !defined (DBC_H)
#define DBC_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


#if defined (_DEBUG)


/*  ----------------------------------- DSP/BIOS Headers            */
#include <sys.h>
#include <failure.h>


/* If DSPLINK_FAILURE_STOP is defined, stop the execution at the point of
 * failure.
 */
#if defined (DSPLINK_FAILURE_STOP)
#if (defined(USE_CCS_BREAKPOINT) && defined( _55_ ))
/** ============================================================================
 *  @macro  DBC_assert
 *
 *  @desc   Asserts a condition.
 *          If USE_CCS_BREAKPOINT is defined the breakpoint instruction
 *          specific to CCS is used.
 *  ============================================================================
 */
#define DBC_assert(expr)    for (; !( expr ); )  \
                                CCS_BREAKPOINT ;

#else /* (defined(USE_CCS_BREAKPOINT) && defined( _55_ )) */

/* Halt the processing at point of failed assertion. */
#define DBC_assert(c)                                           \
    if (!(c)) {                                                 \
        SYS_abort ("Assertion Failed: file=%s, line=%d.\n",     \
                   __FILE__,                                    \
                   __LINE__); }
#endif /* (defined(USE_CCS_BREAKPOINT) && defined( _55_ )) */

#else /* if defined (DSPLINK_FAILURE_STOP) */

    /* Simply log an error. */
#define DBC_assert(c)                                            \
    if (!(c)) {                                                  \
        SYS_error ("Assertion Failed: file=%s, line=%d.\n",      \
                   SYS_EINVAL,                                   \
                   __FILE__,                                     \
                   __LINE__); }

#endif /* if defined (DSPLINK_FAILURE_STOP) */


/** ============================================================================
 *  @macro  DBC_require
 *
 *  @desc   Requirement assertion.
 *  ============================================================================
 */
#define DBC_require DBC_assert

/** ============================================================================
 *  @macro  DBC_ensure
 *
 *  @desc   Ensure assertion.
 *  ============================================================================
 */
#define DBC_ensure  DBC_assert

#else /* if defined (_DEBUG) */

/** ============================================================================
 *  @macro  DBC_xxx
 *
 *  @desc   Compiled out in case of DBC_ASSERTS is not set.
 *  ============================================================================
 */
#define DBC_assert(c)
#define DBC_require(c)
#define DBC_ensure(c)


#endif /* if defined (_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (DBC_H) */
