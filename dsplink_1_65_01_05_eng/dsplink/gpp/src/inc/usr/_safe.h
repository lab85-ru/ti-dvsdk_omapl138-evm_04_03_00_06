/** ============================================================================
 *  @file   _safe.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Contains safe programming macros
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


#if !defined (_SAFE_H)
#define _SAFE_H

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  IS_OBJECT_VALID
 *
 *  @desc   Checks validity of object by comparing against it's signature.
 *  ============================================================================
 */
#define IS_OBJECT_VALID(obj, sign)                                  \
    (((obj != NULL) && ((obj)->signature == sign)) ? TRUE : FALSE)


/** ============================================================================
 *  @macro  IS_RANGE_VALID
 *
 *  @desc   Checks if a value lies in given range.
 *  ============================================================================
 */
#define IS_RANGE_VALID(x,min,max) (((x) < (max)) && ((x) >= (min)))

/** ============================================================================
 *  @macro  MIN
 *
 *  @desc   Returns minumum of the two arguments
 *  ============================================================================
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_SAFE_H) */
