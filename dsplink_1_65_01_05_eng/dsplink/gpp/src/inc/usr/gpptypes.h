/** ============================================================================
 *  @file   gpptypes.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines the type system for DSP/BIOS Link
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


#if !defined (GPPTYPES_H)
#define GPPTYPES_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#if defined  xdc_target__
#include <xdc/std.h>
#endif

/** ============================================================================
 *  @macro  IN/OUT/OPTIONAL/CONST
 *
 *  @desc   Argument specification syntax
 *  ============================================================================
 */
#define IN                              /* The argument is INPUT  only */
#define OUT                             /* The argument is OUTPUT only */
#define CONST   const

/** ============================================================================
 *  @macro  USES
 *
 *  @desc   Empty macro to indicate header file dependency
 *  ============================================================================
 */
#define USES(filename)


/** ============================================================================
 *  @macro  Data types
 *
 *  @desc   Basic data types
 *  ============================================================================
 */
#if defined  xdc_target__
#else
typedef signed char         Int8 ;      /*  8 bit value */
#endif


#if defined  xdc_target__
#else
typedef signed short int    Int16 ;     /* 16 bit value */
#endif
#if defined  xdc_target__
#else
typedef signed long  int    Int32 ;     /* 32 bit value */
#endif

#if defined  xdc_target__
#else
typedef unsigned char       Uint8 ;     /*  8 bit value */
#endif
#if defined  xdc_target__
#else
typedef unsigned short int  Uint16 ;    /* 16 bit value */
#endif
#if defined  xdc_target__
#else
typedef unsigned long  int  Uint32 ;    /* 32 bit value */
#endif

typedef float               Real32 ;    /* 32 bit value */
typedef double              Real64 ;    /* 64 bit value */

#if defined  xdc_target__
#else
typedef short int           Bool ;      /* 16 bit value */
#endif

typedef char                Char8 ;     /*  8 bit value */
typedef short               Char16 ;    /* 16 bit value */

typedef unsigned char       Uchar8 ;    /*  8 bit value */
typedef unsigned short      Uchar16 ;   /* 16 bit value */

/* TBD resolve this with hal_psc.c */
#if defined  xdc_target__
#else
typedef int                 Int ;
#endif

#if defined  xdc_target__
#else
#define Void                void
#endif
typedef void *              Pvoid ;

typedef Char8 *             Pstr ;
typedef Uchar8 *            Pustr ;


/** ============================================================================
 *  @const  TRUE/FALSE
 *
 *  @desc   Boolean constants
 *  ============================================================================
 */
#if !defined (FALSE)
#define FALSE   0u
#endif

#if !defined (TRUE)
#define TRUE    1u
#endif


/** ============================================================================
 *  @const  NULL
 *
 *  @desc   Definition is language specific
 *  ============================================================================
 */
#if !defined (NULL)

#if defined (__cplusplus)
#define NULL    0u
#else  /* defined (__cplusplus) */
#define NULL ((void *)0)
#endif /* defined (__cplusplus) */

#endif /* !defined (NULL) */


/** ============================================================================
 *  @const  NULL_CHAR
 *
 *  @desc   String terminator.
 *  ============================================================================
 */
#define NULL_CHAR '\0'


/** ============================================================================
 *  @macro  REG8/REG16/REG32
 *
 *  @desc   Macros to access register fields.
 *  ============================================================================
 */
#define REG8(A)         (*(volatile Char8  *) (A))
#define REG16(A)        (*(volatile Uint16 *) (A))
#define REG32(A)        (*(volatile Uint32 *) (A))


/** ============================================================================
 *  @macro  DSP/BIOS Link specific types
 *
 *  @desc   These types are used across DSP/BIOS Link.
 *  ============================================================================
 */
typedef Uint32     ProcessorId ;
typedef Uint32     ChannelId ;


/** ============================================================================
 *  @name   PoolId
 *
 *  @desc   This type is used for identifying the different pools used by
 *          DSPLINK.
 *  ============================================================================
 */
typedef Uint16     PoolId ;


/** ============================================================================
 *  @macro  OS Specific standard definitions
 *
 *  @desc   Free for OEMs to add their own generic stuff, if they so desire
 *  ============================================================================
 */
#if defined (OS_WINCE)

#endif  /* defined (OS_WINCE) */


#if defined (OS_NUCLEUS)

#endif  /* defined (OS_NUCLEUS) */


#if defined (OS_LINUX)

#endif  /* defined (OS_LINUX) */


/** ============================================================================
 *  @macro  Calling convention
 *
 *  @desc   Definition of CDECL, DLLIMPORT, DLLEXPORT can be defined by
 *          OEM for his compiler
 *  ============================================================================
 */
#define STATIC          static
#define EXTERN          extern


#if defined (OS_WINCE)
/*  ------------------------------------------- WINCE               */
#define CDECL           
#define DLLIMPORT       __declspec (dllimport)
#define DLLEXPORT       __declspec (dllexport)
/*  ------------------------------------------- WINCE               */
#endif  /* defined (OS_WINCE) */


#if defined (OS_NUCLEUS)
/*  ------------------------------------------- NUCLEUS             */
#define CDECL
#define DLLIMPORT
#define DLLEXPORT
/*  ------------------------------------------- NUCLEUS             */
#endif  /* defined (OS_NUCLEUS) */

#if defined (OS_LINUX)
/*  ------------------------------------------- LINUX               */
#define CDECL
#define DLLIMPORT
#define DLLEXPORT
/*  ------------------------------------------- LINUX               */
#endif  /* defined (OS_LINUX) */


#if defined (OS_PROS)
/*  ------------------------------------------- PROS                */
#define CDECL
#define DLLIMPORT
#define DLLEXPORT
/*  ------------------------------------------- PROS                */
#endif  /* defined (OS_PROS) */

/* Derived calling conventions */
#define NORMAL_API      CDECL
#define IMPORT_API      DLLIMPORT
#define EXPORT_API      DLLEXPORT


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif  /* !defined (GPPTYPES_H) */
