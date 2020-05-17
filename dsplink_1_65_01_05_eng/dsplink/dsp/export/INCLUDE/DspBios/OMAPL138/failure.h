/** ============================================================================
 *  @file   failure.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Header file for failure handler module.
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


#if !defined (FAILURE_H)
#define FAILURE_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


#if defined (_DEBUG)
/*  ============================================================================
 *  @name   CCS_BREAKPOINT
 *
 *  @desc   If USE_CCS_BREAKPOINT is defined the breakpoint instruction
 *          specific to CCS is used.
 *  ============================================================================
 */
#if (defined(USE_CCS_BREAKPOINT) && defined( _55_ ))

#define CCS_BREAKPOINT  \
        asm(" ;\n .if (.MNEMONIC)\n ESTOP_1\n .else\n ESTOP_1()\n .endif\n NOP")

#endif /* (defined(USE_CCS_BREAKPOINT) && defined( _55_ )) */


/** ============================================================================
 *  @name   FID_XXX
 *
 *  @desc   File ids for files in LINK IOM driver.
 *  ============================================================================
 */
#define FID_BASE                (0x100)
#define FID_APP_C               (FID_BASE + 0x1)
#define FID_HAL_C               (FID_BASE + 0x2)
#define FID_DRV_C               (FID_BASE + 0x3)
#define FID_SHMDRV_C            (FID_BASE + 0x4)
#define FID_DSPLINKIPS_C        (FID_BASE + 0x5)
#define FID_IPS_C               (FID_BASE + 0x6)
#define FID_DSPLINKPOOL_C       (FID_BASE + 0x7)
#define FID_SMAPOOL_C           (FID_BASE + 0x8)
#define FID_MPCS_C              (FID_BASE + 0x9)
#define FID__MPCS_C             (FID_BASE + 0xA)
#define FID_MPLIST_C            (FID_BASE + 0xB)
#define FID__MPLIST_C           (FID_BASE + 0xC)
#define FID_DSPLINKMQT_C        (FID_BASE + 0xD)
#define FID_ZCPYMQT_C           (FID_BASE + 0xE)
#define FID_DSPLINKDATA_C       (FID_BASE + 0xF)
#define FID_ZCPYDATA_C          (FID_BASE + 0x10)
#define FID_NOTIFY_C            (FID_BASE + 0x11)
#define FID__NOTIFY_C           (FID_BASE + 0x12)
#define FID_RINGIO_C            (FID_BASE + 0x13)
#define FID__RINGIO_C           (FID_BASE + 0x14)
#define FID_DSPLINKMSQ_C        (FID_BASE + 0x15)

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets the failure reason.
 *  ============================================================================
 */
#define SET_FAILURE_REASON(code)  SetReason (FILEID, __LINE__, code)


/** ============================================================================
 *  @func   SetReason
 *
 *  @desc   Sets the reason of failure.
 *          The reason is set for first failure only.
 *
 *  @arg    fileId
 *              An ID representing a file.
 *          lineNo
 *              Line number from where this function is called.
 *          failureCode
 *              Failure code.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Void SetReason (int fileId, int lineNo, int failureCode) ;

#else  /* if defined (_DEBUG) */

#define SET_FAILURE_REASON(code)

#endif /* if defined (_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (FAILURE_H) */
