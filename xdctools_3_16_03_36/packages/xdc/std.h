/* --COPYRIGHT--,ESD
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
 *  This program and the accompanying materials are made available under the 
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at 
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * --/COPYRIGHT--*/
/*
 *  ======== std.h ========
 *
 *! Revision History
 *! ================
 *! 06-Feb-2008 nitya	added floatToArg and argToFloat
 *! 28-Jul-2005 kw      updated per new RTSC types
 *! 02-Sep-2005 rf      added xdc_VaList
 */

#ifndef xdc_std__include
#define xdc_std__include

#include <stdarg.h>
#include <stddef.h>

/* macros to simplify "stringification" and computed includes */
#define xdc__stringify(a) #a
#define xdc__local_include(a) xdc__stringify(a.h)
#define xdc__system_include(m) <m.h>

/* TitleCase standard types */

#define xdc_Void        void

typedef char            xdc_Char;
typedef unsigned char   xdc_UChar;
typedef short           xdc_Short;
typedef unsigned short  xdc_UShort;
typedef int             xdc_Int;
typedef unsigned int    xdc_UInt;
typedef long            xdc_Long;
typedef unsigned long   xdc_ULong;
typedef float           xdc_Float;
typedef double          xdc_Double;
typedef long double     xdc_LDouble;
typedef size_t          xdc_SizeT;
typedef va_list         xdc_VaList;

/* Generic Extended Types */

typedef unsigned short  xdc_Bool;
typedef void            *xdc_Ptr;       /* data pointer */
typedef char            *xdc_String;    /* null terminated string */

/* we intentionally omit arguments from Fxn to indicate that it can have
 * any (or none).  Unfortunately this causes gcc to emit warnings when
 * -Wstrict-prototypes is used.  Newer gcc's support a pragma that
 * may work around this:
 *
 *    #pragma GCC diagnostic ignored "-Wstrict-prototype"
 */
typedef int             (*xdc_Fxn)();   /* function pointer */

/*
 * Import the target-specific std.h
 */
#ifdef xdc_target_types__
#define xdc_target__ <xdc_target_types__>
#endif
#ifdef xdc_target__
#include xdc_target__
#else
/*
 * 'xdc_target_types__' must be defined to name a target-specific header
 * file (e.g., ti/targets/std.h) that has definitions for the basic types:
 * xdc_Int8, xdc_Int16, ...
 *
 * For example, to build for a target in the ti.targets package you should
 * add the following option to your compiler's command line:
 *    -Dxdc_target_types__=ti/targets/std.h
 */
#error xdc_target_types__ must be defined to name a target-specific header containing definitions of xdc_Int8, xdc_Int16, ...

/* the following definitions are required to keep the compiler from
 * complaining about references to these types in the rest of this header;
 * some compilers do not stop parsing this file after the #error above.
 */
typedef int xdc_IArg;
typedef unsigned int xdc_UArg;
typedef signed char xdc_Int8;
typedef unsigned char xdc_UInt8;
typedef short xdc_Int16;
typedef unsigned short xdc_UInt16;
typedef int xdc_Int32;
typedef unsigned int xdc_UInt32;
#endif

/* Each modules' internal header file defines 'module' as 
 * xdc__MODOBJADDR__(Module__state__V), where Module__state__V is the actual
 * object where the module state is kept. For most targets, the default macro
 * given here results in the construct '(&Module__state__V)->field', when the
 * expression 'module->field' is used. Compilers then generate the code that
 * doesn't dereference a pointer, but puts the address of the field in the
 * code.
 * The targets that need to do something different can define
 * xdc__MODOBJADDR__ in std.h for their target package.
 */
#ifndef xdc__MODOBJADDR__
#define xdc__MODOBJADDR__(symbol) &symbol
#endif

/* Long Long Types */

#ifdef xdc__LONGLONG__
typedef long long               xdc_LLong;
typedef unsigned long long      xdc_ULLong;

#else

#ifndef xdc__INT64__
/* If the target doesn't support "long long" or a 64-bit integral type, we
 * simply use "long".  This is done to ensure that the type LLong always
 * exists, it's at least as long as a "long", and it's 64-bits wide whenever
 * possible.
 */
typedef long                    xdc_LLong;
typedef unsigned long           xdc_ULLong;
#endif

#endif

/* Arg to Ptr and Fxn conversion operators
 *
 * Individual targets may override these definitions in the event
 * that compilers issue warnings about shortening of an Arg to a pointer,
 * for example.
 */
#ifndef xdc__ARGTOPTR
static inline xdc_Ptr xdc_iargToPtr(xdc_IArg a) { return ((xdc_Ptr)a); }
static inline xdc_Ptr xdc_uargToPtr(xdc_UArg a) { return ((xdc_Ptr)a); }
#endif

#ifndef xdc__ARGTOFXN
static inline xdc_Fxn xdc_iargToFxn(xdc_IArg a) { return ((xdc_Fxn)a); }
static inline xdc_Fxn xdc_uargToFxn(xdc_UArg a) { return ((xdc_Fxn)a); }
#endif

/*
 * functions to convert a single precision float to an arg
 * Here assumption is that sizeof(Float) <= sizeof(IArg);
 */
typedef union xdc_FloatData {
    xdc_Float f;
    xdc_IArg  a;
} xdc_FloatData;

static inline xdc_IArg xdc_floatToArg(xdc_Float f)
{
     xdc_FloatData u;
     u.f = f;

     return (u.a);
}

static inline xdc_Float xdc_argToFloat(xdc_IArg a)
{
     xdc_FloatData u;
     u.a = a;

     return (u.f);
}

/* restrict keyword */
#ifndef xdc__RESTRICT__
#define restrict
#endif

/* Unprefixed Aliases */

#ifndef xdc__nolocalnames

#define iargToPtr(a) xdc_iargToPtr(a)
#define uargToPtr(a) xdc_uargToPtr(a)
#define iargToFxn(a) xdc_iargToFxn(a)
#define uargToFxn(a) xdc_uargToFxn(a)
#define floatToArg(a) xdc_floatToArg(a)
#define argToFloat(a) xdc_argToFloat(a)

#define Void xdc_Void

typedef xdc_Char        Char;
typedef xdc_UChar       UChar;
typedef xdc_Short       Short;
typedef xdc_UShort      UShort;
typedef xdc_Int         Int;
typedef xdc_UInt        UInt;
typedef xdc_Long        Long;
typedef xdc_ULong       ULong;
typedef xdc_LLong       LLong;
typedef xdc_ULLong      ULLong;
typedef xdc_Float       Float;
typedef xdc_Double      Double;
typedef xdc_LDouble     LDouble;
typedef xdc_SizeT       SizeT;
typedef xdc_VaList      VaList;

typedef xdc_IArg        IArg;
typedef xdc_UArg        UArg;
typedef xdc_Bool        Bool;
typedef xdc_Int8        Int8;
typedef xdc_Int16       Int16;
typedef xdc_Int32       Int32;
typedef xdc_Fxn         Fxn;
typedef xdc_Ptr         Ptr;
typedef xdc_String      String;

typedef xdc_UInt8       UInt8;
typedef xdc_UInt16      UInt16;
typedef xdc_UInt32      UInt32;

/* DEPRECATED Aliases */
#ifndef xdc__strict
#define _TI_STD_TYPES

/* xdc_Arg is defined only in ti/targets/std.h; use IArg and UArg instead */
#ifdef xdc__ARG__
typedef xdc_Arg         Arg;
#endif

typedef xdc_UInt8       Uint8;
typedef xdc_UInt16      Uint16;
typedef xdc_UInt32      Uint32;
typedef xdc_UInt        Uns;
#endif

/*
 *  ======== optional types ========
 *  The following types are not always defined for all targets
 */
#ifdef xdc__INT64__
typedef xdc_Int64       Int64;
typedef xdc_UInt64      UInt64;
#endif

/* The following exact size types are not required by C99 and may not be
 * supported by some compiler/processor environments.  For greater
 * portability, use the IntN or UIntN types above.
 */
#ifdef xdc__BITS8__
typedef xdc_Bits8       Bits8;
#endif

#ifdef xdc__BITS16__
typedef xdc_Bits16      Bits16;
#endif

#ifdef xdc__BITS32__
typedef xdc_Bits32      Bits32;
#endif

#ifdef xdc__BITS64__
typedef xdc_Bits64      Bits64;
#endif

#endif /* xdc__nolocalnames */

/* Standard Constants */

#undef NULL
#define NULL 0

#undef FALSE
#define FALSE 0

#undef TRUE
#define TRUE 1

/* Declaration Qualifiers */

#ifndef __FAR__
#define __FAR__
#endif

/* Code-Section Directive */

/*
 *  ======== xdc__CODESECT ========
 *  targets can optionally #define xdc__CODESECT in their specific
 *  std.h files.  This directive is placed in front of all
 *  "extern" function declarations, and specifies a section-name in
 *  which to place this function.  This approach
 *  will give folks more control on combining/organizing groups of
 *  related functions into a single named sub-section (e.g.,
 *  "init-code")  If this macro is not defined by the target, an
 *  empty definition is used instead.
 */ 
#ifndef xdc__CODESECT
#define xdc__CODESECT(fn, sn)
#endif


/*
 *  ======== xdc__META ========
 *  Embed unreferenced string in the current file
 *
 *  Strings emebdded via xdc__META can be placed in a section that is
 *  _not_ loaded on the target but are, nevertheless, part of the
 *  executable and available to loaders.
 *
 *  Different targets may define this macro in a way that places these
 *  strings in an output section that is not loaded (and therefore does
 *  not takeup space on the target).  Unless the target provides a
 *  definition of xdc__META, the definition below simply defines
 *  as string constant in the current file.
 */
#ifndef xdc__META
#define xdc__META(n,s) __FAR__ const char (n)[] = {s}
#endif

#endif /* xdc_std__include */
/*
 *  @(#) xdc; 1, 1, 1,239; 2-22-2010 10:32:54; /db/ztree/library/trees/xdc/xdc-u17x/src/packages/
 */

