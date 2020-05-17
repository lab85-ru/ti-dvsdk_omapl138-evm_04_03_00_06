/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *
 */

/*
 *  ======== MSP430_large_code.h ========
 *  DO NOT MODIFY: This header is generated from stddef.xdt
 *
 *  This header contains target-specific definitions of target-independent
 *  macros required by the ITarget interface.  These definitions allow C/C++
 *  sources to portably compile for multiple targets (using #ifdef ...).
 */
#ifndef xdc_target_macros_include__
#define xdc_target_macros_include__    1

/*
 *  ======== ti_targets_MSP430_large_code ========
 *  This macro identifies the specific target being used.  This macro should
 *  probably be avoided in portable sources.
 */
#define ti_targets_MSP430_large_code  1

/*
 *  ======== xdc_target__isaCompatible_* macros ========
 *  The following definitions enable clients to conditionally compile for any
 *  compatible subset of the actual target ISA.
 */
#define xdc_target__isaCompatible_430  1

/*
 *  ======== xdc_target__isa_430 ========
 *  This macro identifies the specific target ISA for which we are being
 *  compiled.
 */
#define xdc_target__isa_430  1

/*
 *  ======== xdc_target__{big|little}Endian ========
 *  The following macro enables clients to portably compile for big or little
 *  endian targets.
 */
#define xdc_target__littleEndian   1

/*
 *  ======== xdc_target__{...}Code ========
 *  The following macro enables clients to portably compile for target specific
 *  code models; e.g., near, far, ...
 */
#define xdc_target__largeCode  1

/*
 *  ======== xdc_target__os_undefined ========
 *  The following macro enables clients to portably compile for target specific
 *  OS; e.g., Linux, Solaris, Windows, undefined.
 */
#define xdc_target__os_undefined    1

/*
 *  ======== xdc_target__sizeof_ ========
 *  The following macros enable clients to portably determine type sizes
 *  within #ifdef blocks; sizeof() can't be used and the definitions in
 *  stdint.h are not available to C++ clients (unless the special macro
 *  __STDC_LIMIT_MACROS is defined).
 */
#define  xdc_target__sizeof_IArg 4
#define  xdc_target__sizeof_Char 1
#define  xdc_target__sizeof_Double 4
#define  xdc_target__sizeof_Float 4
#define  xdc_target__sizeof_Fxn 4
#define  xdc_target__sizeof_Int 2
#define  xdc_target__sizeof_Int8 1
#define  xdc_target__sizeof_Int16 2
#define  xdc_target__sizeof_Int32 4
#define  xdc_target__sizeof_Long 4
#define  xdc_target__sizeof_LDouble 4
#define  xdc_target__sizeof_LLong 4
#define  xdc_target__sizeof_Ptr 2
#define  xdc_target__sizeof_Short 2
#define  xdc_target__sizeof_Size 2

/*
 *  ======== xdc_target__alignof_ ========
 *  The following macros enable clients to portably determine type alignment
 *  within #ifdef blocks; even if provided by the compiler, alignof() can't
 *  be used in pre-processor statements.
 */
#define  xdc_target__alignof_IArg 2
#define  xdc_target__alignof_Char 1
#define  xdc_target__alignof_Double 2
#define  xdc_target__alignof_Float 2
#define  xdc_target__alignof_Fxn 2
#define  xdc_target__alignof_Int 2
#define  xdc_target__alignof_Int8 1
#define  xdc_target__alignof_Int16 2
#define  xdc_target__alignof_Int32 2
#define  xdc_target__alignof_Long 2
#define  xdc_target__alignof_LDouble 2
#define  xdc_target__alignof_LLong 2
#define  xdc_target__alignof_Ptr 2
#define  xdc_target__alignof_Short 2
#define  xdc_target__alignof_Size 2

/*
 *  ======== xdc_target__bitsPerChar ========
 *  The number of bits in a char.  This macro allow one to determine the
 *  precise number of bits in any of the standard types (whose sizes are
 *  expressed as a number of chars).
 */
#define  xdc_target__bitsPerChar 8

#endif  /* end multi-include guard */
/*
 *  @(#) ti.targets; 1, 0, 3,385; 2-24-2010 16:24:15; /db/ztree/library/trees/xdctargets/xdctargets-b36x/src/
 */

