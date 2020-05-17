/** ============================================================================
 *  @file   dbc.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Design by Contract
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


#if !defined (DBC_H)
#define DBC_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @macro  DBC_PRINTF
 *
 *  @desc   This macro expands to the print function. It makes the DBC
 *          macros portable across OSes.
 *  ============================================================================
 */
#define  DBC_PRINTF     PRINT_Printf


#if defined (DDSP_DEBUG)

/** ============================================================================
 *  @macro  DBC_Assert
 *
 *  @desc   Assert on expression.
 *  ============================================================================
 */
#define DBC_Assert(exp)                                                        \
        if (!(exp)) {                                                          \
            DBC_PRINTF ("Assertion failed ("#exp"). File : "__FILE__           \
                        " Line : %d\n", __LINE__) ;                            \
        }

/** ============================================================================
 *  @macro  DBC_Require
 *
 *  @desc   Function Precondition.
 *  ============================================================================
 */
#define DBC_Require    DBC_Assert

/** ============================================================================
 *  @macro  DBC_Ensure
 *
 *  @desc   Function Postcondition.
 *  ============================================================================
 */
#define DBC_Ensure     DBC_Assert

#else /* defined (DDSP_DEBUG) */

/*  ============================================================================
 *  @macro  DBC_Assert/DBC_Require/DBC_Ensure
 *
 *  @desc   Asserts defined out.
 *  ============================================================================
 */
#define DBC_Assert(exp)
#define DBC_Require(exp)
#define DBC_Ensure(exp)

#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (DBC_H) */
