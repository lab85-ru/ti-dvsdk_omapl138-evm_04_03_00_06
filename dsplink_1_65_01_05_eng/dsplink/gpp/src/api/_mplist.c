/** ============================================================================
 *  @file   _mplist.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <mplistdefs.h>
#include <linkcfgdefs.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>
#include <_mpcs.h>
#include <_mplist.h>

/*  ----------------------------------- C Libraries                 */
#include <string.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*  ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_MPLIST

/*  ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MPLIST, __LINE__)

/*  ============================================================================
 *  @macro  MPLIST_MPCSOBJ_NAME
 *
 *  @desc   Name of the MPCS object used for protection of the MPLIST region.
 *  ============================================================================
 */
#define  MPLIST_MPCSOBJ_NAME   MPCS_RESV_LOCKNAME"_MPLIST"


/** ============================================================================
 *  @name   MPLIST_state
 *
 *  @desc   State object for the MPLIST component containing all information
 *          required by it.
 *  ============================================================================
 */
MPLIST_Object MPLIST_State [MAX_DSPS] ;


/** ============================================================================
 *  @name   PROC_linkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver on the user-side.
 *  ============================================================================
 */
extern LINKCFG_Object * PROC_linkCfgPtr ;


/** ============================================================================
 *  @func   _MPLIST_moduleInit
 *
 *  @desc   This function initializes the MPLIST module.
 *          This function does the system level initialization for the MPLIST
 *          module. It initializes and allocates all resources common to all
 *          applications using the MPLIST module.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPLIST_moduleInit (IN ProcessorId procId)
{
    DSP_STATUS           status    = DSP_SOK ;
    DSP_STATUS           tmpStatus = DSP_SOK ;
    CMD_Args             args ;
    MPCS_Attrs           mpcsAttrs ;
    LINKCFG_MpList *     mplistObject ;
    LINKCFG_Dsp *        dspObj ;
    LINKCFG_LinkDrv *    linkDrv ;
    MPLIST_Ctrl *        ctrlPtr ;
    MPLIST_Entry *       entryPtr ;
    LINKCFG_DspConfig *  dspConfig ;

    TRC_1ENTER ("_MPLIST_moduleInit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.mplistArgs.mplistRegionArgs.procId = procId ;
        status = DRV_INVOKE (DRV_handle, CMD_MPLIST_MAPREGION, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            dspConfig = PROC_linkCfgPtr->dspConfigs [procId] ;
            dspObj    = dspConfig->dspObject ;
            linkDrv   = &(dspConfig->linkDrvObjects [dspObj->linkDrvId]) ;
            mplistObject =
                    &(dspConfig->mplistObjects [linkDrv->mplistTableId]) ;

            ctrlPtr  = (MPLIST_Ctrl *)
                             args.apiArgs.mplistArgs.mplistRegionArgs.userAddr ;
            entryPtr = (MPLIST_Entry *)
                                 ((Uint32) (ctrlPtr) + sizeof (MPLIST_Ctrl)) ;

            ctrlPtr->isInitialized = (Uint32) TRUE ;
            ctrlPtr->dspId         = procId ;
            ctrlPtr->maxEntries    = mplistObject->maxEntries ;
            ctrlPtr->ipsId         = mplistObject->ipsId ;
            ctrlPtr->ipsEventNo    = mplistObject->ipsEventNo ;

            memset ((Char8 *) entryPtr,
                    0,
                    (ctrlPtr->maxEntries * sizeof (MPLIST_Entry))) ;

            mpcsAttrs.poolId = POOL_INVALIDID ;
            status = MPCS_create (procId,
                                  MPLIST_MPCSOBJ_NAME,
                                  &(ctrlPtr->lockObj),
                                  &mpcsAttrs) ;

            tmpStatus = DRV_INVOKE (DRV_handle, CMD_MPLIST_UNMAPREGION, &args) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                _MPLIST_moduleExit (procId) ;
            }
        }
    }

    TRC_1LEAVE ("_MPLIST_moduleInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _MPLIST_moduleExit
 *
 *  @desc   This function finalizes the MPLIST module.
 *          This function does the system level finalization for the MPLIST
 *          module. It finalizes and allocates all resources common to all
 *          applications using the MPLIST module.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
_MPLIST_moduleExit (IN ProcessorId procId)
{
    DSP_STATUS     status    = DSP_SOK ;
    DSP_STATUS     tmpStatus = DSP_SOK ;
    MPLIST_Ctrl *  ctrlPtr ;
    MPLIST_Entry * entryPtr ;
    CMD_Args       args ;

    TRC_1ENTER ("_MPLIST_moduleExit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Map the MPLIST region to be able to finalize it. */
        args.apiArgs.mplistArgs.mplistRegionArgs.procId = procId ;
        status = DRV_INVOKE (DRV_handle, CMD_MPLIST_MAPREGION, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            tmpStatus = MPCS_delete (procId, MPLIST_MPCSOBJ_NAME) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            ctrlPtr  = (MPLIST_Ctrl *)
                             args.apiArgs.mplistArgs.mplistRegionArgs.userAddr ;
            entryPtr = (MPLIST_Entry *)
                              ((Uint32) (ctrlPtr) + sizeof (MPLIST_Ctrl)) ;
            memset ((Char8 *) entryPtr,
                    0,
                    (ctrlPtr->maxEntries * sizeof (MPLIST_Entry))) ;
            ctrlPtr->isInitialized = FALSE ;

            tmpStatus = DRV_INVOKE (DRV_handle, CMD_MPLIST_UNMAPREGION, &args) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("_MPLIST_moduleExit", status) ;

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _MPLIST_init
 *
 *  @desc   This function initializes the MPLIST component for the calling
 *          process.
 *
 *  @modif  MPLIST_ctrlPtr, MPLIST_entryPtr, MPLIST_ctrlMemInfo
 *  ----------------------------------------------------------------------------
 */
EXPORT_API
DSP_STATUS
_MPLIST_init (IN ProcessorId procId)
{
    DSP_STATUS      status = DSP_SOK ;
    MPLIST_Object * mplistState = NULL ;
    CMD_Args        args ;

    TRC_1ENTER ("_MPLIST_init", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.mplistArgs.mplistRegionArgs.procId = procId ;
        status = DRV_INVOKE (DRV_handle, CMD_MPLIST_MAPREGION, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            mplistState = &MPLIST_State [procId] ;
            mplistState->ctrlMemInfo =args.apiArgs.mplistArgs.mplistRegionArgs ;
            mplistState->ctrlPtr = (MPLIST_Ctrl *)
                                    MPLIST_State [procId].ctrlMemInfo.userAddr ;
            mplistState->entryPtr =(MPLIST_Entry *)
                                              (  (Uint32) (mplistState->ctrlPtr)
                                               + sizeof (MPLIST_Ctrl)) ;

            /* Open the MPCS object in every process to get a handle to the
             * MPCS object.
             */
            status = _MPCS_open (procId,
                                 MPLIST_MPCSOBJ_NAME,
                                 &mplistState->mpcsObj,
                                 &(mplistState->ctrlPtr->lockObj)) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                _MPLIST_exit (procId) ;
            }
        }
    }

    TRC_1LEAVE ("_MPLIST_init", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   _MPLIST_exit
 *
 *  @desc   This function finalizes the MPLIST component for the calling
 *          process.
 *
 *  @modif  MPLIST_ctrlPtr, MPLIST_entryPtr
 *  ----------------------------------------------------------------------------
 */
EXPORT_API
DSP_STATUS
_MPLIST_exit (IN ProcessorId procId)
{
    DSP_STATUS      status       = DSP_SOK ;
    DSP_STATUS      tmpStatus    = DSP_SOK ;
    MPLIST_Object * mplistState  = NULL    ;
    CMD_Args        args ;

    TRC_1ENTER ("_MPLIST_exit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistState = &MPLIST_State [procId] ;
        if (mplistState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        if (mplistState->mpcsObj != NULL) {
            status = MPCS_close (procId, mplistState->mpcsObj) ;
            mplistState->mpcsObj = NULL ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        args.apiArgs.mplistArgs.mplistRegionArgs.userAddr =
                                            mplistState->ctrlMemInfo.userAddr ;
        args.apiArgs.mplistArgs.mplistRegionArgs.size     =
                                               mplistState->ctrlMemInfo.size  ;
        tmpStatus = DRV_INVOKE (DRV_handle, CMD_MPLIST_UNMAPREGION, &args) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        mplistState->ctrlPtr = NULL ;
        mplistState->entryPtr = NULL ;
    }

    TRC_1LEAVE ("_MPLIST_exit", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
