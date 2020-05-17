/** ============================================================================
 *  @file   _mplist.c
 *
 *  @path   $(DSPLINK)/dsp/src/mplist/
 *
 *  @desc   Implementation of internal functions for API sub-component MPLIST.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */

/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <gbl.h>
#include <mem.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <mplistdefs.h>
#include <mplist.h>
#include <_mplist.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define     FILEID                FID__MPLIST_C

/*  ============================================================================
 *  @macro  MPLIST_MPCSOBJ_NAME
 *
 *  @desc   Name of the MPCS object used for protection of the MPLIST region.
 *  ============================================================================
 */
#define     MPLIST_MPCSOBJ_NAME   MPCS_RESV_LOCKNAME"_MPLIST"

/** ============================================================================
 *  @name   MPLIST_ctrlPtr
 *
 *  @desc   Pointer to MPLIST Control region.
 *  ============================================================================
 */
MPLIST_Ctrl * MPLIST_ctrlPtr  = NULL ;

/** ============================================================================
 *  @name   MPLIST_mpcsObj
 *
 *  @desc   Handle to global MPCS object for protecting the MPLIST region.
 *  ============================================================================
 */
MPCS_Handle   MPLIST_mpcsObj  = NULL ;


/** ============================================================================
 *  @func   _MPLIST_init
 *
 *  @desc   This function initialize the MPLIST component.
 *          This function does the system level initialization. This initializes
 *          all internal MPLIST structures.This function should be called even
 *          before any other MPLIST functions are called
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPLIST_init (IN Uint16 procId, IN Uint32 shAddr)
{
    Int        status = SYS_OK ;
    MPCS_Attrs attrs ;

    MPLIST_ctrlPtr = (MPLIST_Ctrl *) shAddr ;

    HAL_cacheInv ((Void *) MPLIST_ctrlPtr, sizeof (MPLIST_Ctrl)) ;

    if (MPLIST_ctrlPtr->isInitialized == FALSE) {
        MPLIST_ctrlPtr->isInitialized = TRUE ;
        MPLIST_ctrlPtr->dspId         = GBL_getProcId () ;
        memset ((Char *) MPLIST_ctrlPtr->dspAddrEntry,
                0,
                MPLIST_ctrlPtr->maxEntries * sizeof (MPLIST_Entry)) ;

        attrs.poolId = (Uint16) MPCS_INVALID_ID ;
        status = MPCS_create (procId,
                              MPLIST_MPCSOBJ_NAME,
                              &(MPLIST_ctrlPtr->lockObj),
                              &attrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }

        HAL_cacheWbInv ((Ptr) (MPLIST_ctrlPtr), sizeof (MPLIST_ctrlPtr)) ;
    }

    if (status == SYS_OK) {
        status = _MPCS_open (procId,
                             MPLIST_MPCSOBJ_NAME,
                             &MPLIST_mpcsObj,
                             &(MPLIST_ctrlPtr->lockObj)) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   MPLIST_exit
 *
 *  @desc   This function finalizes the MPLIST component.
 *
 *  @modif
 *  ----------------------------------------------------------------------------
 */
Int
MPLIST_exit (IN Uint16 procId)
{
    Int status = SYS_OK ;

    /* Close the multi processor critical section object */
    if (MPLIST_mpcsObj != NULL) {
        status = MPCS_close (procId, MPLIST_mpcsObj) ;
        MPLIST_mpcsObj = NULL ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
