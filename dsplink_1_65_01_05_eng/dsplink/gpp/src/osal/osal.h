/** ============================================================================
 *  @file   osal.h
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Defines the interfaces for initializing and finalizing OSAL.
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


#if !defined (OSAL_H)
#define OSAL_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <dpc.h>
#include <isr.h>
#include <kfile.h>
#include <mem_os.h>
#include <mem.h>
#if defined (NOTIFY_COMPONENT)
#include <notify_knl.h>
#endif /* #if defined (NOTIFY_COMPONENT) */
#include <prcs.h>
#include <sync.h>
#include <drvdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   OSAL_Initialize
 *
 *  @desc   Initializes the OS Adaptation layer.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    OSAL_Finalize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OSAL_Initialize (Void) ;


/** ============================================================================
 *  @func   OSAL_Finalize
 *
 *  @desc   Releases OS adaptation layer resources indicating that they would
 *          no longer be used.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EFAIL
 *              General error from GPP-OS.
 *
 *  @enter  Subcomponent must be initialized.
 *
 *  @leave  None
 *
 *  @see    OSAL_Initialize
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OSAL_Finalize (Void) ;


#if defined (__cplusplus)
}
#endif


#endif /* !defined (OSAL_H) */
