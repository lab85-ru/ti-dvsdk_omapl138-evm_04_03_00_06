/** ============================================================================
 *  @file   print.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/
 *
 *  @desc   Implementation of OS printf abstraction.
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


/*  ----------------------------------- OS Specific Headers         */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/spinlock.h>
#include <stdarg.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <asm/div64.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Debug                       */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component ID of this module.
 *  ============================================================================
 */
#define  COMPONENT_ID        ID_OSAL_PRINT

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_OSAL_PRINT, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   PRINT_IsInitialized
 *
 *  @desc   Flag to track initialization of the sub-component.
 *  ============================================================================
 */
STATIC Uint32  PRINT_IsInitialized  = FALSE   ;


/** ============================================================================
 *  @func   PRINT_Initialize
 *
 *  @desc   Initializes the PRINT sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRINT_Initialize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    PRINT_IsInitialized = TRUE ;

    return status ;
}


/** ============================================================================
 *  @func   PRINT_Finalize
 *
 *  @desc   Releases resources used by this sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PRINT_Finalize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    PRINT_IsInitialized = FALSE ;

    return status ;
}


/** ============================================================================
 *  @func   PRINT_Printf
 *
 *  @desc   Provides standard printf functionality abstraction.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
PRINT_Printf (Pstr format, ...)
{
    Uint32  bufCount     = 0 ;
    va_list args             ;
    char    buffer [512 ]    ;

    buffer[0] = '<';
    buffer[1] = '1';
    buffer[2] = '>';
    bufCount  =  3 ;

    va_start (args, format) ;
    vsprintf (buffer + bufCount, format, args);
    va_end   (args) ;

    printk (buffer) ;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
