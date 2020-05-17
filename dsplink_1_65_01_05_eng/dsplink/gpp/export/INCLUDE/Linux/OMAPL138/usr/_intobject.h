/** ============================================================================
 *  @file   _intobject.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Declares an object that encapsulates the interrupt information
 *          reqiured by Linux.
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


#if !defined (_INTOBJECT_H)
#define _INTOBJECT_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <gpptypes.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   IPS_checkIntGen
 *
 *  @desc   This function implements check routine to check the interrupt
 *          generation. In case of shared interrupts, other devices can also
 *          generate same ints.
 *
 *  @modif  None.
 *  ============================================================================
 */
typedef Bool (*INT_checkGen) (IN Pvoid refData) ;


/** ============================================================================
 *  @name   InterruptObject
 *
 *  @desc   Object encapsulating the OS dependent interrupt information.
 *
 *  @field  dspId
 *              Processor identifier
 *  @field  intId
 *              Interrupt identifier
 *  @field  shared
 *              Tells whether the interrupt is shared with other devices.
 *  @field  checkFunc
 *              Function to check for interrupt generating device.
 *  @field  param
 *              Parameter to be passed to check function.
 *  ============================================================================
 */
typedef struct InterruptObject_tag {
    ProcessorId  dspId     ;
    Int32        intId     ;
    Bool         shared    ;
    INT_checkGen checkFunc ;
    Pvoid        param     ;
} InterruptObject ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (_INTOBJECT_H) */
