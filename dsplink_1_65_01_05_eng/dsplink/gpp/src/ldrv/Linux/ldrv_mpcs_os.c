/** ============================================================================
 *  @file   ldrv_mpcs_os.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/Linux/
 *
 *  @desc   Implementation of OS-specific functionality for LDRV.
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

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <print.h>
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Link Driver                 */
#include <ldrv_mpcs_os.h>


#if defined (__cplusplus)
extern "C" {
#endif

/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_MPCS_OS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_MPCS_OS, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */



/** ============================================================================
 *  @func   LDRV_MPCS_OS_moduleInit
 *
 *  @desc   This function initializes the LDRV_MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_OS_moduleInit (Void)
{
    DSP_STATUS         status = DSP_SOK ;

    TRC_0ENTER ("LDRV_MPCS_OS_moduleInit") ;

    TRC_1LEAVE ("LDRV_MPCS_OS_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_OS_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_OS_moduleExit (Void)
{
    DSP_STATUS status    = DSP_SOK ;

    TRC_0ENTER ("LDRV_MPCS_OS_moduleExit") ;

    TRC_1LEAVE ("LDRV_MPCS_OS_moduleExit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_OS_create
 *
 *  @desc   This function creates and initializes an instance of the MPCS
 *          object.
 *          The memory for the object may or may not be provided by the user.
 *          If provided by the user, the memory for the object must be shared
 *          across the processors using the MPCS. It must also already be mapped
 *          into user space for OSes supporting user/kernel separation.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_create (IN     ProcessorId    dspId,
                     IN     MPCS_ShObj *   mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("LDRV_MPCS_OS_create", dspId, mpcsObj) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (mpcsObj != NULL) ;

    mpcsObj->gppMpcsObj.localLock     = (Uint32) SYNC_SpinLockCreate () ;

    TRC_1LEAVE ("LDRV_MPCS_OS_create", status) ;

    return status ;
}



/** ============================================================================
 *  @func   LDRV_MPCS_OS_delete
 *
 *  @desc   This function deletes the instance of the MPCS object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_delete (IN     ProcessorId  dspId,
                     IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("LDRV_MPCS_OS_delete", dspId, mpcsObj) ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (mpcsObj != NULL) ;

#if defined (GPP_SWI_MODE)
        mpcsObj->gppMpcsObj.priority      = 0 ;
        mpcsObj->dspMpcsObj.priority      = 0 ;
#endif

    if (mpcsObj->gppMpcsObj.localLock != (Uint32) NULL) {
        SYNC_SpinLockDelete ((Pvoid) mpcsObj->gppMpcsObj.localLock) ;
        mpcsObj->gppMpcsObj.localLock = (Uint32) NULL ;
    }


    TRC_1LEAVE ("LDRV_MPCS_OS_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_OS_enter
 *
 *  @desc   This function enters the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_enter (IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LDRV_MPCS_OS_enter", mpcsObj) ;

    DBC_Require (mpcsObj != NULL) ;

    SYNC_SpinLockStartEx ((Pvoid) mpcsObj->gppMpcsObj.localLock) ;

    TRC_1LEAVE ("LDRV_MPCS_OS_enter", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MPCS_OS_leave
 *
 *  @desc   This function leaves the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_leave (IN     MPCS_ShObj * mpcsObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LDRV_MPCS_OS_leave", mpcsObj) ;

    DBC_Require (mpcsObj != NULL) ;

    SYNC_SpinLockEndEx ((Pvoid) mpcsObj->gppMpcsObj.localLock, (Uint32) NULL) ;

    TRC_1LEAVE ("LDRV_MPCS_OS_leave", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
