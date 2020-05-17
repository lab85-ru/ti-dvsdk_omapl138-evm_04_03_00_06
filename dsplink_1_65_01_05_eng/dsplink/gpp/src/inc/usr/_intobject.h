/** ============================================================================
 *  @file   _intobject.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Declares an object that encapsulates the interrupt information
 *          reqiured by Linux.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated - http://www.ti.com/
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation version 2.
 *  
 *  This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 *  whether express or implied; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
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
