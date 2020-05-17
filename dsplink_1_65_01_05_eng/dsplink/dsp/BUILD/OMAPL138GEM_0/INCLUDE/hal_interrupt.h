/** ============================================================================
 *  @file   hal_interrupt.h
 *
 *  @path   $(DSPLINK)/dsp/src/base/hal/
 *
 *  @desc   This file declares interrupt-related functions of  HAL (Hardware
 *          Abstraction Layer).
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


#if !defined (HAL_INTERRUPT_H)
#define HAL_INTERRUPT_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   HAL_intInit
 *
 *  @desc   Initializes the HAL Interrupt module.
 *
 *  @arg    None
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    EMIF_init
 *  ============================================================================
 */
Void
HAL_intInit () ;


/** ============================================================================
 *  @func   HAL_intEnable
 *
 *  @desc   Enables GPP interrupt.
 *
 *  @arg    intId
 *              ID of the interrupt to be enabled.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intDisable ()
 *  ============================================================================
 */
Void
HAL_intEnable (Uint32 intId) ;


/** ============================================================================
 *  @func   HAL_intDisable
 *
 *  @desc   Disables GPP interrupt.
 *
 *  @arg    intId
 *              ID of the interrupt to be disabled.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intEnable ()
 *  ============================================================================
 */
Void
HAL_intDisable (Uint32 intId) ;


/** ============================================================================
 *  @func   HAL_intRegister
 *
 *  @desc   Register ISR for GPP interrupt.
 *
 *  @arg    intId
 *              ID of the interrupt to be registered.
 *  @arg    intVectorId
 *              Interrupt vector number to be used.
 *  @arg    func
 *              Function to register.
 *  @arg    arg
 *              Argument to function.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intInit ()
 *  ============================================================================
 */
Void
HAL_intRegister (Uint32 intId, Uint32 intVectorId, Fxn func, Ptr arg) ;


/** ============================================================================
 *  @func   HAL_intWaitClear
 *
 *  @desc   Wait for interrupt to be cleared.
 *
 *  @arg    intId
 *              ID of the interrupt to be cleared.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intSend ()
 *  ============================================================================
 */
Void
HAL_intWaitClear (Uint32 intId) ;


/** ============================================================================
 *  @func   HAL_intSend
 *
 *  @desc   Send interrupt to GPP.
 *
 *  @arg    intId
 *              ID of the interrupt to be sent.
 *  @arg    arg
 *              Argument for sending interrupt.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intClear (), HAL_intWaitClear ()
 *  ============================================================================
 */
Void
HAL_intSend (Uint32 intId, Uns arg) ;


/** ============================================================================
 *  @func   HAL_intClear
 *
 *  @desc   Clear interrupt.
 *
 *  @arg    intId
 *              ID of the interrupt to be cleared.
 *
 *  @ret    Value (if any) of the interrupt before it was cleared.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    HAL_intSend (),
 *  ============================================================================
 */
Uns
HAL_intClear (Uint32 intId) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (HAL_INTERRUPT_H) */

