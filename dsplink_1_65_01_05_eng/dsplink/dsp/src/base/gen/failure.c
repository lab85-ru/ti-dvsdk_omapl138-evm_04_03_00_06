/** ============================================================================
 *  @file   failure.c
 *
 *  @path   $(DSPLINK)/dsp/src/base/gen/
 *
 *  @desc   This file implements failure handler functions. They also provide
 *          support for debugging the DSP side application.
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sys.h>
#include <failure.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


#if defined(_DEBUG)


/** ============================================================================
 *  @name   FAIL_XXX
 *
 *  @desc   Contains information regaring first failure that is set otherwise
 *          if no failure is set all varibles should be zero.
 *  ============================================================================
 */
Int   FAIL_Set = 0 ;
Int   FAIL_File ;
Int   FAIL_Line ;
Int   FAIL_Code ;


/** ============================================================================
 *  @func   SetReason
 *
 *  @desc   Failure handler function for failures in LINK.
 *
 *  @modif  None
 *  ============================================================================
 */
Void SetReason (Int fileId, Int lineNo, Int failureCode)
{
    /* Set the failure for the first time */
    if (FAIL_Set == 0)
    {
        FAIL_Set  = 1;
        FAIL_File = fileId;
        FAIL_Line = lineNo;
        FAIL_Code = failureCode;
    }

    /* Log the error print */
    SYS_error ("Failure [0x%x] in [0x%x] at line %d\n",
               failureCode,
               failureCode,
               fileId,
               lineNo) ;

    /* If DSPLINK_FAILURE_STOP is defined, stop the execution at the point of
     * failure.
     */
#if defined (DSPLINK_FAILURE_STOP)
#if (defined(USE_CCS_BREAKPOINT) && defined( _55_ ))
    CCS_BREAKPOINT ;
#else
    /* A forever while loop.
     * It is not simple while (1) to sneak through the compiler optimizations.
     */
    {
        volatile Int i = 1 ;
        while (i) {
            /* Added for MISRAC compliance. */
        }
    }
#endif /* (defined(USE_CCS_BREAKPOINT) && defined( _55_ )) */
#endif /* if defined (DSPLINK_FAILURE_STOP) */
}


#endif /* if defined(_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
