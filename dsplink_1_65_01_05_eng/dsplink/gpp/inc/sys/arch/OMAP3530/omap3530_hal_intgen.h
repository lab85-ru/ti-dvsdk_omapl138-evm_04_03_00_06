/** ============================================================================
 *  @file   omap3530_hal_intgen.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/OMAP3530/
 *
 *  @desc   Declares necessary functions for Interrupt Handling.
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


#if !defined (OMAP3530_HAL_INTGEN_H)
#define OMAP3530_HAL_INTGEN_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <omap3530_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   OMAP3530_halIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *  @arg    cmd.
 *              Command.
 *  @arg    intId.
 *              Interrupt Identifier.
 *  @arg    arg.
 *              Command specific arguments.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *            DSP_EINVALIDARG
 *              Unsupported interrupt control command.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_halIntCtrl (IN         Pvoid          halObj,
                      IN         DSP_IntCtrlCmd cmd,
                      IN         Uint32         intId,
                      IN OUT     Pvoid          arg) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP3530_HAL_INTGEN_H) */
