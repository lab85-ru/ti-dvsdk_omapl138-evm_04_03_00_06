/** ============================================================================
 *  @file   user.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/
 *
 *  @desc   Implementation of user access.
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
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
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
 *  @desc   Identifier for sub-component.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_OSAL_USER

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_OSAL_USER, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @func   USER_copyFromUser
 *
 *  @desc   Copy user data to kernel buffer.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyFromUser (IN Pvoid dst, OUT Pvoid src, IN Uint32 numBytes)
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      copied = 0       ;

    TRC_3ENTER ("USER_copyFromUser", dst, src, numBytes) ;

    DBC_Require (src != NULL) ;
    DBC_Require (dst != NULL) ;

    copied = copy_from_user (dst, src, numBytes) ;
    if (copied > 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("USER_copyFromUser", status) ;

    return status ;
}


/** ============================================================================
 *  @func   USER_copyTOUser
 *
 *  @desc   Copy kernel data to user buffer.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
USER_copyToUser (IN Pvoid src, OUT Pvoid dst, IN Uint32 numBytes)
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      copied = 0       ;

    TRC_3ENTER ("USER_copyToUser", src, dst, numBytes) ;

    DBC_Require (src != NULL) ;
    DBC_Require (dst != NULL) ;

    copied = copy_to_user (dst, src, numBytes) ;
    if (copied > 0) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_0LEAVE ("USER_copyToUser") ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
