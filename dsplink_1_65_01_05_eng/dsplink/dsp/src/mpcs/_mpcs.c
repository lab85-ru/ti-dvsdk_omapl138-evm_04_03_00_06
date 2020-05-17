/** ============================================================================
 *  @file   _mpcs.c
 *
 *  @path   $(DSPLINK)/dsp/src/mpcs/
 *
 *  @desc   Implementation of internal functions for API sub-component MPCS.
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
#include <sys.h>
#include <gbl.h>
#include <string.h>
#if defined (DSP_TSK_MODE)
#include <lck.h>
#endif /* if defined(DSP_TSK_MODE) */

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <_mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID                FID__MPCS_C

/** ============================================================================
 *  @name   MPCS_ctrlPtr
 *
 *  @desc   Pointer to MPCS Control region.
 *  ============================================================================
 */
MPCS_Ctrl * MPCS_ctrlPtr    = NULL ;

/** ============================================================================
 *  @name   MPCS_csObj
 *
 *  @desc   Handle to global MPCS protecting the MPCS region.
 *  ============================================================================
 */
MPCS_Handle MPCS_csObj      = NULL ;

#if defined (DSP_TSK_MODE)
/** ============================================================================
 *  @name   MPCS_GlobalLock
 *
 *  @desc   Global Lock for MPCS critical section protection
 *  ============================================================================
 */
extern Uint32 MPCS_GlobalLock;
#endif

/** ============================================================================
 *  @func   MPCS_init
 *
 *  @desc   This function initializes the MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPCS_init (IN Uint16 procId, IN Uint32 shAddr)
{
    Int        status = SYS_OK ;
    MPCS_Attrs attrs ;

    MPCS_ctrlPtr = (MPCS_Ctrl *) shAddr ;

    HAL_cacheInv ((Void *) MPCS_ctrlPtr, sizeof (MPCS_Ctrl)) ;

    if (MPCS_ctrlPtr->isInitialized == FALSE) {
        MPCS_ctrlPtr->isInitialized = TRUE ;
        memset ((Char *) MPCS_ctrlPtr->dspAddrEntry,
                0,
                MPCS_ctrlPtr->maxEntries * sizeof (MPCS_Entry)) ;
        MPCS_ctrlPtr->dspId = GBL_getProcId () ;

        attrs.poolId = (Uint16) MPCS_INVALID_ID ;
        status = MPCS_create (procId,
                              MPCS_RESV_LOCKNAME,
                              &(MPCS_ctrlPtr->lockObj),
                              &attrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }

        HAL_cacheWbInv ((Ptr) (MPCS_ctrlPtr), sizeof (MPCS_Ctrl)) ;
    }

    if (status == SYS_OK) {
        status = _MPCS_open (procId,
                             MPCS_RESV_LOCKNAME,
                             &MPCS_csObj,
                             &(MPCS_ctrlPtr->lockObj)) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_exit
 *
 *  @desc   This function finalizes the MPCS module.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPCS_exit (IN Uint16 procId)
{
    Int status = SYS_OK ;

    status = MPCS_close (procId, MPCS_csObj) ;
    if (status == SYS_OK) {
        MPCS_csObj = NULL ;
    }
    else {
        SET_FAILURE_REASON (status) ;
    }

    MPCS_ctrlPtr = NULL ;

    return status ;
}


/** ============================================================================
 *  @func   _MPCS_open
 *
 *  @desc   This function opens an MPCS object specified by its name and gets a
 *          handle to the object. This function allows the user to open an MPCS
 *          object by a special reserved name prefix MPCS_RESV_LOCKNAME
 *          indicating that the object is not registered within the MPCS entries
 *          table.
 *          For such objects, the user already has the pointer to the MPCS
 *          shared object in its process space.
 *          Every process that needs to use the MPCS object must get a handle to
 *          the object by calling this API.
 *
 *  @modif  None
 *  ============================================================================
 */
Int
_MPCS_open (IN     Uint16         procId,
            IN     Char *         name,
            OUT    MPCS_Handle *  mpcsHandle,
            IN     MPCS_ShObj *   mpcsShObj)
{
    Int          status     = SYS_OK ;
    Int          tmpStatus  = SYS_OK ;
    Uint32       i ;

    (Void) procId ; /* To avoid compiler warning. */

    DBC_require (name       != NULL) ;
    DBC_require (mpcsHandle != NULL) ;

    *mpcsHandle = NULL ;

    /* Check if the MPCS object to be opened is the reserved one used for
     * protection of MPCS region.
     */
    if (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) == 0) {
        DBC_assert (mpcsShObj != NULL) ;
        if (mpcsShObj == NULL) {
            status = SYS_EINVAL ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            *mpcsHandle = mpcsShObj ;
#if defined (DSP_TSK_MODE)
            HAL_cacheInv ((Ptr) *mpcsHandle, sizeof (MPCS_ShObj)) ;
            if (MPCS_GlobalLock == NULL) {
                MPCS_GlobalLock = (Uint32) LCK_create (NULL);
            }
            if((*mpcsHandle)->dspMpcsObj.localLock == NULL) {
                (*mpcsHandle)->dspMpcsObj.localLock = MPCS_GlobalLock;
                if ((*mpcsHandle)->dspMpcsObj.localLock == NULL) {
                    status = SYS_EALLOC ;
                }
                else {
                    (*mpcsHandle)->dspMpcsObj.localFlag = MPCS_SEMCREATED_OPEN ;
                    HAL_cacheWbInv ((Ptr) &((*mpcsHandle)->dspMpcsObj),
                                (sizeof (MPCS_ProcObj) + sizeof(Uint32))) ;
                }
            }
#endif /* if defined(DSP_TSK_MODE) */
        }
    }
    else {
        tmpStatus = MPCS_enter (MPCS_csObj) ;
        /* MPCS_csObj is not NULL at this point. */
        DBC_assert (tmpStatus == SYS_OK) ;

        /* Find the MPCS entry corresponding to the specified name. */
        for (i = 0 ; i < MPCS_ctrlPtr->maxEntries ; i++) {
            HAL_cacheInv ((Ptr) &(MPCS_ctrlPtr->dspAddrEntry [i]),
                          sizeof (MPCS_Entry)) ;
            /* Check if the entry is valid. */
            if (MPCS_ctrlPtr->dspAddrEntry [i].physAddress != NULL) {
                if (strncmp (name, MPCS_ctrlPtr->dspAddrEntry [i].name,
                            DSP_MAX_STRLEN) == 0) {
                    break ;
                }
            }
        }

        if (i == MPCS_ctrlPtr->maxEntries) {
            status = SYS_ENOTFOUND ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            *mpcsHandle = (MPCS_ShObj *)
                                 MPCS_ctrlPtr->dspAddrEntry [i].physAddress ;
            HAL_cacheInv ((Ptr) *mpcsHandle, sizeof (MPCS_ShObj)) ;
            (*mpcsHandle)->dspMpcsObj.mpcsEntryAddr = (Uint32)
                &(MPCS_ctrlPtr->dspAddrEntry [i]);
            HAL_cacheWbInv ((Ptr) &((*mpcsHandle)->dspMpcsObj),
                           (sizeof (MPCS_ProcObj))) ;

#if defined (DSP_TSK_MODE)
            if (MPCS_GlobalLock == NULL) {
                MPCS_GlobalLock = (Uint32) LCK_create (NULL);
            }
            if((*mpcsHandle)->dspMpcsObj.localLock == NULL) {
                (*mpcsHandle)->dspMpcsObj.localLock = MPCS_GlobalLock;
                if ((*mpcsHandle)->dspMpcsObj.localLock == NULL) {
                    status = SYS_EALLOC ;
                }
                else {
                    (*mpcsHandle)->dspMpcsObj.localFlag = MPCS_SEMCREATED_OPEN ;
                    HAL_cacheWbInv ((Ptr) &((*mpcsHandle)->dspMpcsObj),
                                (sizeof (MPCS_ProcObj) + sizeof(Uint32))) ;
                }
            }
#endif /* if defined(DSP_TSK_MODE) */
        }
        tmpStatus = MPCS_leave (MPCS_csObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
