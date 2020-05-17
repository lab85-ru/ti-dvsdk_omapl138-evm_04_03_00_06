/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
/*
 *  ======== std.h ========
 *
 */

#ifndef STD_

#include "tistdtypes.h"

/*
 *  #include <tistdtypes.h> must be before '#define STD_' to be compatible
 *  with older versions of <tistdtypes.h> file which had conditional logic
 *  based on STD_.  This protects against possible include path inconsistencies
 *  where user might find older <tistdtypes.h> before BIOS's <tistdtypes.h>.
 */

#define STD_


#ifdef _TMS320C28X
#define _28_ 1
#ifdef __LARGE_MODEL__
#define _28L_ 1
#define _28l_ 1         /* deprecated, recommend use _28L_ */
#endif // __LARGE_MODEL__
#endif // _TMS320C28X

#if defined(__TMS320C28XX_FPU32__) || defined(__TMS320C28XX_FPU64__)
#define _28FP_ 1
#endif

#ifdef _TMS320C5XX
#define _54_ 1
#endif

#ifdef __TMS320C55X__ 
#define _55_ 1
#ifdef __LARGE_MODEL__
#define _55L_ 1
#define _55l_ 1         /* deprecated, recommend use _55L_ */
#endif
#ifdef __HUGE_MODEL__
#define _55H_ 1
#define _VCORE3_ 1
#endif
#endif

#ifdef __TMS320C55X_PLUS_BYTE__
#define _55_  1
#define _55H_ 1
#define _55Pb_ 1
#define _55P_ 1
#define _VCORE3_ 1
#endif // __TMS320C55X_PLUS_BYTE__ 

//#ifdef __TMS320C55X_PLUS_WORD__
//#define _55_  1
//#define       _55H_ 1
//#define       _55Pw_ 1
//#define       _55P_ 1
//#define _VCORE3_ 1
//#endif // __TMS320C55X_PLUS_WORD__

#ifdef _TMS320C6200
#define _62_ 1
#define _6x_ 1
#endif

#ifdef _TMS320C6400
#define _64_ 1
#define _6x_ 1
#endif

#ifdef _TMS320C6400_PLUS
#define _64P_ 1
#define _6x_ 1
#endif

#ifdef _TMS320C6700
#define _67_ 1
#define _6x_ 1
#endif

#ifdef _TMS320C6700_PLUS
#define _67P_ 1
#define _6x_ 1
#endif

#ifdef _TMS320C6740
#define _674_ 1
#define _6x_ 1
#endif

/*
 *  ======== _TI_ ========
 *  _TI_ is defined for all TI targets
 */
#if defined(_54_) || defined(_55_) || defined (_6x_) || defined (_28_)
#define _TI_    1
#endif

/*
 *  ======== _FLOAT_ ========
 *  _FLOAT_ is defined for all targets that natively support floating point
 */
#if defined(_67_) || defined(_67P_) || defined(_674_)
#define _FLOAT_ 1
#else
#define _FIXED_ 1
#endif


/*
 *  8, 16, 32-bit type definitions
 *
 *  Sm* - 8-bit type
 *  Md* - 16-bit type
 *  Lg* - 32-bit type
 *
 *  *Int - signed type
 *  *Uns - unsigned type
 *  *Bits - unsigned type (bit-maps)
 */
typedef char SmInt;             /* SMSIZE-bit signed integer */
typedef short MdInt;            /* MDSIZE-bit signed integer */
#if defined(_6x_)
typedef int LgInt;              /* LGSIZE-bit signed integer */
#else
typedef long LgInt;             /* LGSIZE-bit signed integer */
#endif

typedef unsigned char SmUns;    /* SMSIZE-bit unsigned integer */
typedef unsigned short MdUns;   /* MDSIZE-bit unsigned integer */
#if defined(_6x_)
typedef unsigned LgUns;         /* LGSIZE-bit unsigned integer */
#else
typedef unsigned long LgUns;    /* LGSIZE-bit unsigned integer */
#endif

typedef unsigned char SmBits;   /* SMSIZE-bit bit string */
typedef unsigned short MdBits;  /* MDSIZE-bit bit string */
#if defined(_6x_)
typedef unsigned LgBits;        /* LGSIZE-bit bit string */
#else
typedef unsigned long LgBits;   /* LGSIZE-bit bit string */
#endif

/* avoid conflicting with xdc/std.h */
#ifndef xdc_std__include

typedef long int Long;
typedef short int Short;
typedef SmBits Byte;            /* smallest unit of addressable store */

#define Void void

/* Arg should be size of Ptr */
#if defined(_54_) || defined(_6x_)
typedef Int Arg;
#elif defined(_55_) || defined(_28_)
typedef void *Arg;
#else
/* Other ISAs not supported */
#error <std.h> types not supported for this target
#endif

typedef Int (*Fxn)();           /* generic function type */

typedef float Float;

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define FALSE ((Bool)0)
#define TRUE  ((Bool)1)
#endif

#endif /* xdc_std__include */

/*
 * These macros are used to cast 'Arg' types to 'Int' or 'Ptr'.
 * These macros were added for the 55x since Arg is not the same
 * size as Int and Ptr in 55x large model.
 */
#if defined(_28L_) || defined(_55L_) || defined(_55H_)
#define ArgToInt(A)     ((Int)((long)(A) & 0xffff))
#define ArgToPtr(A)     ((Ptr)(A))
#else
#define ArgToInt(A)     ((Int)(A))
#define ArgToPtr(A)     ((Ptr)(A))
#endif

#endif /* STD_ */
