/** ============================================================================
 *  @file   _mpcs.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
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


/*  ----------------------------------- C Libraries                 */
#include <string.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <gpptypes.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <_mpcs.h>
#include <mpcs.h>
#include <_mpcs_os.h>
#include <proc.h>
#include <pool.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_MPCS

/*  ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MPCS, __LINE__)


/** ============================================================================
 *  @name   MPCS_state
 *
 *  @desc   State object for the MPCS component containing all information
 *          required by it.
 *  ============================================================================
 */
MPCS_Object MPCS_State [MAX_DSPS] ;


/** ============================================================================
 *  @func   _MPCS_init
 *
 *  @desc   This function initialize the MPCS component.
 *
 *  @modif  MPCS_ctrlPtr
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_init (IN ProcessorId procId)
{
    DSP_STATUS    status = DSP_SOK ;
    MPCS_Object * mpcsState = NULL ;
    CMD_Args      args             ;

    TRC_1ENTER ("_MPCS_init", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if ((IS_VALID_PROCID (procId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.mpcsMapArgs.mpcsRegionArgs.procId = procId ;
        status = DRV_INVOKE (DRV_handle, CMD_MPCS_MAPREGION, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            mpcsState = &MPCS_State [procId] ;
            mpcsState->ctrlMemInfo = args.apiArgs.mpcsMapArgs.mpcsRegionArgs ;
            mpcsState->ctrlPtr     = (MPCS_Ctrl *)
                                      mpcsState->ctrlMemInfo.userAddr ;
            mpcsState->entryPtr    = (MPCS_Entry *)
                               ( (Pvoid) ((Uint32) (mpcsState->ctrlPtr)
                                + sizeof (MPCS_Ctrl))) ;

            status = _MPCS_open (procId,
                                 MPCS_RESV_LOCKNAME,
                                 &mpcsState->csObj,
                                 &(mpcsState->ctrlPtr->lockObj)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            if (DSP_FAILED (status)) {
                _MPCS_exit (procId) ;
            }
        }
    }

    TRC_1LEAVE ("_MPCS_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MPCS_exit
 *
 *  @desc   This function finalizes the MPCS component.
 *
 *  @modif  MPCS_ctrlPtr
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_exit (IN ProcessorId procId)
{
    DSP_STATUS    status    = DSP_SOK ;
    DSP_STATUS    tmpStatus = DSP_SOK ;
    MPCS_Object * mpcsState = NULL ;
    CMD_Args      args ;

    TRC_1ENTER ("_MPCS_exit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if ((IS_VALID_PROCID (procId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;
        if (mpcsState->ctrlPtr == NULL) {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = MPCS_close (procId, mpcsState->csObj) ;
        if (DSP_SUCCEEDED (status)) {
            mpcsState->csObj = NULL ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        args.apiArgs.mpcsMapArgs.mpcsRegionArgs.userAddr =
                                               mpcsState->ctrlMemInfo.userAddr ;
        args.apiArgs.mpcsMapArgs.mpcsRegionArgs.size     =
                                                  mpcsState->ctrlMemInfo.size  ;
        tmpStatus = DRV_INVOKE (DRV_handle, CMD_MPCS_UNMAPREGION, &args) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        mpcsState->ctrlPtr = NULL ;
        mpcsState->entryPtr = NULL ;
    }

    TRC_1LEAVE ("_MPCS_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MPCS_open
 *
 *  @desc   This function opens an MPCS object specified by its name and gets a
 *          handle to the object. This function allows the user to open an MPCS
 *          object by a special reserved name MPCS_RESV_LOCKNAME indicating that
 *          the object is not registered within the MPCS entries table.
 *          For such objects, the user already has the pointer to the MPCS
 *          shared object in its process space.
 *          Every process that needs to use the MPCS object must get a handle to
 *          the object by calling this API.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPCS_open (IN     ProcessorId    procId,
            IN     Pstr           name,
            OUT    MPCS_Handle *  mpcsHandle,
            IN     MPCS_ShObj *   mpcsShObj)
{
    DSP_STATUS    status     = DSP_SOK ;
    DSP_STATUS    tmpStatus  = DSP_SOK ;
    MPCS_ShObj *  mpcsObj    = NULL ;
    Bool          resvMpcs   = FALSE ;
    MPCS_Object * mpcsState  = NULL ;
    Uint32        mpcsId     = MPCS_INVALID_ID ;
    Uint32        i ;


    TRC_4ENTER ("_MPCS_open", procId, name, mpcsHandle, mpcsShObj) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (name       != NULL) ;
    DBC_Require (mpcsHandle != NULL) ;

    *mpcsHandle = NULL ;
    /* Check if the MPCS object to be opened is the reserved one used for
     * protection of MPCS region.
     */
    if (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) == 0) {
        DBC_Assert (mpcsShObj != NULL) ;
        if (mpcsShObj == NULL) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            mpcsObj = mpcsShObj ;
        }

        resvMpcs = TRUE ;
        mpcsId   = MPCS_INVALID_ID ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;

        tmpStatus = MPCS_enter (mpcsState->csObj) ;
        /* MPCS_csObj is not NULL at this point. */
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Find the MPCS entry corresponding to the specified name. */
        for (i = 0 ; i < mpcsState->ctrlPtr->maxEntries ; i++) {
            /* Check if the entry is valid. */
            if (mpcsState->entryPtr [i].physAddress != NULL) {
                if (    strncmp (name,
                                 mpcsState->entryPtr [i].name,
                                 DSP_MAX_STRLEN)
                    ==  0) {
                    mpcsId = i ;
                    break ;
                }
            }
        }

        if (i == mpcsState->ctrlPtr->maxEntries) {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Get the user address of the MPCS object. */
            status = POOL_translateAddr (
                        POOL_makePoolId(procId, mpcsState->entryPtr [i].poolId),
                                         (Pvoid *) &mpcsObj,
                                         AddrType_Usr,
                                         mpcsState->entryPtr [i].physAddress,
                                         AddrType_Dsp) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                POOL_invalidate (
                    POOL_makePoolId(procId, mpcsState->entryPtr [i].poolId),
                    mpcsObj,
                    sizeof (MPCS_ShObj)) ;
            }
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _MPCS_OS_open (procId, name, mpcsHandle, mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            ((MPCS_Obj *) *mpcsHandle)->mpcsId = mpcsId ;
            ((MPCS_Obj *) *mpcsHandle)->procId = procId ;
            if (mpcsId == MPCS_INVALID_ID) {
                mpcsObj->gppMpcsObj.mpcsEntryAddr = MPCS_INVALID_ID ;
            }
            else {
                mpcsObj->gppMpcsObj.mpcsEntryAddr =  (Uint32) (&mpcsState->entryPtr [mpcsId]) ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    if (resvMpcs == FALSE) {
        tmpStatus = MPCS_leave (mpcsState->csObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPCS_open", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
