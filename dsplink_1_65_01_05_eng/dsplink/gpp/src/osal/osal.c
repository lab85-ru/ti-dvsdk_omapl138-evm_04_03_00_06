/** ============================================================================
 *  @file   osal.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/
 *
 *  @desc   Implementation of OS Adaptation layer initialization and
 *          finalization functions.
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
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_OSAL

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_OSAL, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   OSAL_InitializeCount
 *
 *  @desc   Number of times OSAL has been initialized.
 *  ============================================================================
 */
STATIC Uint32  OSAL_InitializeCount = 0   ;


/** ============================================================================
 *  @func   OSAL_Initialize
 *
 *  @desc   Initializes the OS Adaptation layer.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OSAL_Initialize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("OSAL_Initialize") ;

    if (OSAL_InitializeCount == 0) {
        status = MEM_Initialize   () ;
        if (DSP_SUCCEEDED (status)) {
            status = SYNC_Initialize  () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = KFILE_Initialize () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = ISR_Initialize   () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = DPC_Initialize   () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = PRCS_Initialize  () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = PRINT_Initialize  () ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            OSAL_InitializeCount++ ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        OSAL_InitializeCount++ ;
    }

    TRC_1LEAVE ("OSAL_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   OSAL_Finalize
 *
 *  @desc   Releases OS adaptation layer resources indicating that they would
 *          no longer be used.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OSAL_Finalize (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    TRC_0ENTER ("OSAL_Finalize") ;

    if (OSAL_InitializeCount == 1) {

        status = PRINT_Finalize  () ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        tmpStatus = PRCS_Finalize   () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = DPC_Finalize   () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = ISR_Finalize   () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = KFILE_Finalize () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = SYNC_Finalize  () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = MEM_Finalize   () ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    if (OSAL_InitializeCount > 0) {
        if (DSP_SUCCEEDED (status)) {
            OSAL_InitializeCount-- ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("OSAL_Finalize", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
