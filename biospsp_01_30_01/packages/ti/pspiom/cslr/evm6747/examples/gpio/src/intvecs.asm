; * ============================================================================
; * Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006, 2007
; *
; * Use of this software is controlled by the terms and conditions found
; * in the license agreement under which this software has been supplied.
; * ============================================================================

; * ============================================================================
; * @brief Interrupt vector file for C64x and C64x+.
; *
; * @file  intvecs.asm
; *
; * @verbatim
;   This file contains interrupt service table.  The CPU interrupt 4 vector
;   contains interrupt4 function.
;   @endverbatim
; *
; * ============================================================================

; Global symbols defined here and exported out of this file
   .global _intcVectorTable
   .global _c_int00
   .global _vector1
   .global _vector2
   .global _vector3
   .global _GPIO_input_isr
   .global _vector5
   .global _vector6
   .global _vector7
   .global _vector8
   .global _vector9
   .global _vector10
   .global _vector11


; This is a macro that instantiates one entry in the interrupt service table.
VEC_ENTRY .macro addr
    STW   B0,*--B15
    MVKL  addr,B0
    MVKH  addr,B0
    B     B0
    LDW   *B15++,B0
    NOP   2
    NOP
    NOP
   .endm

; This is a dummy interrupt service routine used to initialize the IST.
_vec_dummy:
  B    B3
  NOP  5

; This is the actual interrupt service table (IST).
 .sect ".vecs"
 .align 1024

_intcVectorTable:
_vector0:   VEC_ENTRY _c_int00      ;RESET
_vector1:   VEC_ENTRY _vec_dummy    ;NMI
_vector2:   VEC_ENTRY _vec_dummy    ;RSVD
_vector3:   VEC_ENTRY _vec_dummy    ;RSVD
_vector4:   VEC_ENTRY _GPIO_input_isr ;Interrupt4 ISR
_vector5:   VEC_ENTRY _vec_dummy
_vector6:   VEC_ENTRY _vec_dummy
_vector7:   VEC_ENTRY _vec_dummy
_vector8:   VEC_ENTRY _vec_dummy
_vector9:   VEC_ENTRY _vec_dummy
_vector10:  VEC_ENTRY _vec_dummy
_vector11:  VEC_ENTRY _vec_dummy


;* =============================================================================
;*   Automated Revision Information
;*   Changed: $Date: 2007-09-11 11:05:40 -0500 (Tue, 11 Sep 2007) $
;*   Revision: $Revision: 3960 $
;* =============================================================================


