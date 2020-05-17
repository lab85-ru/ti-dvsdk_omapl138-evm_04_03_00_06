/** ============================================================================
 *  @file   proc.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Implementation of API sub-component PROC.
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
#include <_dsplink.h>
#include <linkcfgdefs.h>
#include <procdefs.h>
#if defined (MPLIST_COMPONENT)
#include <mplistdefs.h>
#include <_mplist.h>
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (RINGIO_COMPONENT)
#include <ringio.h>
#include <_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

#if defined (MPCS_COMPONENT)
#include <_mpcs.h>
#endif /* if defined (MPCS_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
#include <_notify.h>
#endif /* if defined (NOTIFY_COMPONENT) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <proc.h>

#if defined (NOTIFY_COMPONENT)
#include <_notify.h>
#endif /* if defined (NOTIFY_COMPONENT) */

#include <_proc.h>
#include <_idm_usr.h>
#include <_sync_usr.h>
#include <_mem_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_PROC

/** ============================================================================
 *  @macro  COMPONENT_ID_KEY
 *
 *  @desc   Component Subcomponent ID key used with SYNC.
 *  ============================================================================
 */
#define  COMPONENT_ID_KEY   "ID_API_PROC"

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_PROC, __LINE__)


/** ============================================================================
 *  @name   DRV_handle
 *
 *  @desc   OS specific Link Driver object
 *  ============================================================================
 */
DRV_Object   * DRV_handle = NULL ;

/** ============================================================================
 *  @name   LINKCFG_config
 *
 *  @desc   Declaration of the global structure containing DSPLINK configuration
 *          information for the driver on the user-side.
 *  ============================================================================
 */
extern LINKCFG_Object   LINKCFG_config ;

/** ============================================================================
 *  @name   PROC_linkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver on the user-side.
 *  ============================================================================
 */
LINKCFG_Object * PROC_linkCfgPtr = NULL ;

#if !defined (OS_PROS) && !defined(OS_WINCE)
/** ============================================================================
 *  @name   DSPLINK_isSignal
 *
 *  @desc   Global flag to indicate whether execution is in signal context.
 *          Driver on the user-side.
 *  ============================================================================
 */
extern Bool DSPLINK_isSignal ;
#endif

/*  ----------------------------------------------------------------------------
 *  @name   PROC_stateObj
 *
 *  @desc   State object for the PROC user-level component containing internal
 *          state information required by it.
 *  ----------------------------------------------------------------------------
 */
PROC_Object PROC_stateObj =
{
    NULL,
    {
        FALSE
    }
} ;


/** ----------------------------------------------------------------------------
 *  @func   PROC_resetCurStatus
 *
 *  @desc   Reset all CurStatus values for this process.
 *
 *  @arg    None
 *
 *  @ret    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
PROC_resetCurStatus (Void) ;


/** ============================================================================
 *  @func   PROC_setup
 *
 *  @desc   Sets up the necessary data structures for the PROC sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_setup (IN LINKCFG_Object * linkCfg)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;
    CMD_Args   args ;

    TRC_1ENTER ("PROC_setup", linkCfg) ;

    if (DRV_CHECK_CURSTATUS (PROC_stateObj.curStatus.isSetup) == TRUE) {
        status = DSP_EALREADYSETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Set the configuration if given by the user, otherwise use the default
         * configuration.
         */
        if (linkCfg != NULL) {
            PROC_linkCfgPtr = linkCfg ;
        }
        else {
            PROC_linkCfgPtr = &LINKCFG_config ;
        }

        status = DRV_INITIALIZE (&DRV_handle) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            tmpStatus = DRV_STARTUP_INIT (DRV_handle) ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = DRV_PROTECT_INIT (DRV_handle) ;
        if (DSP_SUCCEEDED (status)) {
            status = DRV_PROTECT_ENTER (DRV_handle) ;
            if (DSP_SUCCEEDED (status)) {
                args.apiArgs.procSetupArgs.linkCfg = PROC_linkCfgPtr ;
                status = DRV_INVOKE (DRV_handle, CMD_PROC_SETUP, &args) ;

                /* Check if it is the first call to setup. Subsequent calls
                 * return DSP_SAREADYSETUP.
                 */
                if (status == DSP_SOK) {
                    tmpStatus = _MEM_USR_init () ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        tmpStatus = _IDM_USR_init () ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                        else {
                            tmpStatus = _SYNC_USR_init () ;
                            if (    DSP_SUCCEEDED (status)
                                &&  DSP_FAILED (tmpStatus)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
                    }

                    if (DSP_FAILED (status)) {
                        _SYNC_USR_exit () ;
                        _IDM_USR_exit () ;
                        _MEM_USR_exit () ;
                    }
                }
                else {
                    _SYNC_USR_stateObjInit () ;
                }

                if (DSP_SUCCEEDED (status)) {
                    tmpStatus = _SYNC_USR_createCS (COMPONENT_ID_KEY,
                                                    &PROC_stateObj.syncCsObj) ;

                    if (DSP_SUCCEEDED (status) &&  DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }

                tmpStatus = DRV_PROTECT_LEAVE (DRV_handle) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                if (DSP_FAILED (status)) {
                    tmpStatus = DRV_PROTECT_EXIT (DRV_handle) ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            DRV_SET_CURSTATUS (PROC_stateObj.curStatus.isSetup) ;
            PROC_resetCurStatus () ;
        }
    }

    TRC_1LEAVE ("PROC_setup", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_destroy
 *
 *  @desc   Destroys the data structures for the PROC component,
 *          allocated earlier by a call to PROC_setup ().
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_destroy ()
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
    Bool       lastDestroy = FALSE ;
    CMD_Args   args ;

    TRC_0ENTER ("PROC_destroy") ;

    /* Check if PROC has been setup in this process. */
    if (DRV_CHECK_CURSTATUS (PROC_stateObj.curStatus.isSetup) == FALSE) {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }
    else {
        if (DRV_handle == NULL) {
            status = DRV_INITIALIZE (&DRV_handle) ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = DRV_PROTECT_ENTER (DRV_handle) ;
        if (DSP_SUCCEEDED (status)) {
            /* Check if it is the last call to destroy. */
            status = DRV_INVOKE (DRV_handle, CMD_PROC_ISLASTDESTROY, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                lastDestroy = args.apiArgs.procIsLastDestroyArgs.lastDestroy ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        tmpStatus = _SYNC_USR_deleteCS (&PROC_stateObj.syncCsObj) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        if (lastDestroy == TRUE) {
            tmpStatus = _SYNC_USR_exit () ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = _IDM_USR_exit () ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = _MEM_USR_exit () ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        tmpStatus = DRV_INVOKE (DRV_handle, CMD_PROC_DESTROY, &args) ;
        /* If earlier calls succeeded, return status of CMD_PROC_DESTROY. */
        if (DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
        }
        if (DSP_FAILED (tmpStatus)) {
            SET_FAILURE_REASON ;
        }

        tmpStatus = DRV_PROTECT_LEAVE (DRV_handle) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        /* There is a very small window of lack of protection here. */
        if (lastDestroy == TRUE) {
            tmpStatus = DRV_PROTECT_EXIT (DRV_handle) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.isSetup) ;

        tmpStatus = DRV_FINALIZE (DRV_handle) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
        DRV_handle = NULL ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PROC_destroy", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_attach
 *
 *  @desc   Attaches the client to the specified DSP and also
 *          initializes the DSP (if required).
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_attach (IN  ProcessorId    procId,
                 PROC_Attrs *   attr)
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
#if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
             || (MPLIST_COMPONENT) || (RINGIO_COMPONENT)
    Bool       firstAttach = FALSE ;
#endif /* if defined      (NOTIFY_COMPONENT) || (MPCS_COMPONENT)
                       || (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */
    CMD_Args   args                ;

    TRC_2ENTER ("PROC_attach", procId, attr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* PROC_attach may be directly called in multi-application scenario. */
        status = DRV_INITIALIZE (&DRV_handle) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            tmpStatus = DRV_STARTUP_INIT (DRV_handle) ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (PROC_stateObj.syncCsObj == NULL) {
                status = _SYNC_USR_stateObjInit () ;
                if (DSP_SUCCEEDED (status)) {
                    status = _SYNC_USR_createCS (COMPONENT_ID_KEY,
                                                 &PROC_stateObj.syncCsObj) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (DSP_SUCCEEDED (status)) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            if (DSP_SUCCEEDED (status)) {
                if (PROC_linkCfgPtr == NULL) {
                    PROC_linkCfgPtr = &LINKCFG_config ;
                }
                /* Update the PROC_linkCfgPtr->dspConfigs [procId] */
                if (attr != NULL) {
                    if (attr->dspCfgPtr != NULL) {
                        PROC_linkCfgPtr->dspConfigs [procId] =
                                                              attr->dspCfgPtr ;
                    }
                }

                args.apiArgs.procAttachArgs.procId = procId ;
                args.apiArgs.procAttachArgs.attr   = attr   ;

                status = DRV_INVOKE (DRV_handle, CMD_PROC_ATTACH, &args) ;

#if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
             || (MPLIST_COMPONENT) || (RINGIO_COMPONENT)

                /* Check if it is the first call to attach. Subsequent calls
                 * return DSP_SALREADYATTACHED.
                 */
                if (status == DSP_SOK) {
                    firstAttach = TRUE ;
                }

#endif /* #if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
                       || (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */

                if (DSP_SUCCEEDED (status)) {
                    /* Check if this is being called from a forked process.
                     * In this case, these APIs have not been called in this
                     * process.
                     */
                    if (    DRV_CHECK_CURSTATUS(PROC_stateObj.curStatus.isSetup)
                        ==  FALSE) {
                        DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.isSetup) ;
                        PROC_resetCurStatus () ;
                    }

#if defined (NOTIFY_COMPONENT)
                    tmpStatus = _NOTIFY_init (procId) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
#endif /* if defined (NOTIFY_COMPONENT) */

#if defined (MPCS_COMPONENT)
                    if (DSP_SUCCEEDED (status)) {
                        tmpStatus = _MPCS_init (procId) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (MPLIST_COMPONENT) || (RINGIO_COMPONENT)
                /* If it is the first process attaching to the DSP, initialize
                 * the MPLIST and RingIO modules.
                     */
                    if (firstAttach == TRUE) {
#if defined (MPLIST_COMPONENT)
                        if (DSP_SUCCEEDED (status)) {
                            tmpStatus = _MPLIST_moduleInit (procId) ;
                            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (RINGIO_COMPONENT)
                        if (DSP_SUCCEEDED (status)) {
                            tmpStatus = _RingIO_moduleInit (procId) ;
                            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
#endif /* if defined (RINGIO_COMPONENT) */
                    }
#endif /* #if defined (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */

#if defined (MPLIST_COMPONENT)
                    if (DSP_SUCCEEDED (status)) {
                        tmpStatus = _MPLIST_init (procId) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (RINGIO_COMPONENT)
                    if (DSP_SUCCEEDED (status)) {
                        tmpStatus = _RingIO_init (procId) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
#endif /* if defined (RINGIO_COMPONENT) */

                    if (DSP_SUCCEEDED (status)) {
                        DRV_SET_CURSTATUS (
                                  PROC_stateObj.curStatus.isAttached [procId]) ;
                    }
                }

                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_attach", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_detach
 *
 *  @desc   Detaches the client from specified processor.
 *          If the caller is the owner of the processor, this function releases
 *          all the resources that this component uses and puts the DSP in an
 *          unusable state (from application perspective).
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_detach (IN  ProcessorId   procId)
{
    DSP_STATUS status     = DSP_SOK ;
    DSP_STATUS tmpStatus  = DSP_SOK ;
#if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
             || (MPLIST_COMPONENT) || (RINGIO_COMPONENT)
    Bool       lastDetach = FALSE ;
#endif /* if defined      (NOTIFY_COMPONENT) || (MPCS_COMPONENT)
                       || (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */
    CMD_Args   args                ;

    TRC_1ENTER ("PROC_detach", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Check if PROC has been attached in this process for specified DSP. */
        if (    DRV_CHECK_CURSTATUS (PROC_stateObj.curStatus.isAttached[procId])
            ==  FALSE) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
        else {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            if (DSP_SUCCEEDED (status)) {
                /* Check if it is the last call to detach. */
                args.apiArgs.procIsLastDetachArgs.procId = procId ;
                status = DRV_INVOKE (DRV_handle, CMD_PROC_ISLASTDETACH, &args) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
#if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
             || (MPLIST_COMPONENT) || (RINGIO_COMPONENT)
                else {
                    lastDetach = args.apiArgs.procIsLastDetachArgs.lastDetach ;
                }
#endif /* #if defined     (NOTIFY_COMPONENT) || (MPCS_COMPONENT)  \
                       || (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */

#if defined (RINGIO_COMPONENT)
                tmpStatus = _RingIO_exit (procId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MPLIST_COMPONENT)
                tmpStatus = _MPLIST_exit (procId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
#endif /* if defined (MPLIST_COMPONENT) */

#if defined (MPLIST_COMPONENT) || (RINGIO_COMPONENT)
                /* If it is the last process detaching from the DSP, finalize the
                 * MPLIST and RingIO modules.
                 */
                if (lastDetach == TRUE) {
#if defined (RINGIO_COMPONENT)
                    tmpStatus = _RingIO_moduleExit (procId) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (MPLIST_COMPONENT)
                    tmpStatus = _MPLIST_moduleExit (procId) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
#endif /* if defined (MPLIST_COMPONENT) */
                }
#endif /* #if defined (MPLIST_COMPONENT) || (RINGIO_COMPONENT) */

#if defined (MPCS_COMPONENT)
                tmpStatus = _MPCS_exit (procId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
#endif /* if defined (MPCS_COMPONENT) */

#if defined (NOTIFY_COMPONENT)
                tmpStatus = _NOTIFY_exit (procId) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
#endif /* if defined (NOTIFY_COMPONENT) */

                args.apiArgs.procDetachArgs.procId = procId ;

                tmpStatus = DRV_INVOKE (DRV_handle, CMD_PROC_DETACH, &args) ;
                /* If earlier calls succeeded, return CMD_PROC_DETACH status */
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
                if (DSP_FAILED (tmpStatus)) {
                    SET_FAILURE_REASON ;
                }

                DRV_RESET_CURSTATUS(PROC_stateObj.curStatus.isAttached[procId]);

                tmpStatus = DRV_FINALIZE (DRV_handle) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
                if (tmpStatus == DSP_SFINALIZED) {
                    DRV_handle = NULL ;
                }

                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("PROC_detach", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_getState
 *
 *  @desc   Gets the current status of DSP by querying the Link Driver.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_getState (IN   ProcessorId  procId,
               OUT  PROC_State * procState)
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
    Bool       notAttached = FALSE   ;
    CMD_Args   args                  ;

    TRC_2ENTER ("PROC_getState", procId, status) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (procState != NULL ) ;

    if ((IS_VALID_PROCID (procId) == FALSE) || (procState == NULL )) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (DRV_handle == NULL) {
            status = DRV_INITIALIZE (&DRV_handle) ;
            notAttached = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (notAttached == FALSE) {
                status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            }

            if (DSP_SUCCEEDED (status)) {
                args.apiArgs.procGetStateArgs.procId    = procId ;
                args.apiArgs.procGetStateArgs.procState = procState ;

                status = DRV_INVOKE (DRV_handle, CMD_PROC_GETSTATE, &args) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }

                if (notAttached == FALSE) {
                    tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (notAttached == TRUE) {
            tmpStatus = DRV_FINALIZE (DRV_handle) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
            DRV_handle = NULL ;
        }
    }

    TRC_1LEAVE ("PROC_getState", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_load
 *
 *  @desc   Loads the specified DSP executable on the target DSP.
 *          It ensures that the caller owns the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_load (IN   ProcessorId  procId,
           IN   Char8 *      imagePath,
           IN   Uint32       argc,
           IN   Char8 **     argv)
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
    Bool       csObjExists = FALSE ;
    CMD_Args   args ;

    TRC_4ENTER ("PROC_load", procId, imagePath, argc, argv) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (imagePath != NULL ) ;
    DBC_Require (   ((argc != 0) && (argv != NULL))
                 || ((argc == 0) && (argv == NULL))) ;

    if (   ((IS_VALID_PROCID (procId)) == FALSE)
        || (imagePath == NULL)
        || (((argc == 0) && (argv != NULL)))
        || (((argc != 0) && (argv == NULL)))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (PROC_stateObj.syncCsObj != NULL) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            csObjExists = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (    DRV_CHECK_CURSTATUS (
                                PROC_stateObj.curStatus.isAttached [procId])
                ==  FALSE) {
                status = DSP_EATTACHED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.procLoadArgs.procId    = procId    ;
                args.apiArgs.procLoadArgs.imagePath = imagePath ;
                args.apiArgs.procLoadArgs.argc      = argc      ;
                args.apiArgs.procLoadArgs.argv      = argv      ;

                status = DRV_INVOKE (DRV_handle, CMD_PROC_LOAD, &args) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (csObjExists == TRUE) {
                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("PROC_load", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_loadSection
 *
 *  @desc   Loads the specified section of DSP executable onto the target DSP.
 *          It ensures that the client owns the DSP.
 *          Currently this API is not implemented.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_loadSection (IN    ProcessorId  procId,
                  IN    Char8 *      imagePath,
                  IN    Uint32       sectID)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("PROC_loadSection", procId, imagePath, sectID) ;

    status = DSP_ENOTIMPL ;
    SET_FAILURE_REASON ;

    TRC_1LEAVE ("PROC_loadSection", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_read
 *
 *  @desc   This function allows GPP side applications to read from the DSP
 *          memory space.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_read (IN     ProcessorId    procId,
           IN     Uint32         dspAddr,
           IN     Uint32         numBytes,
           IN OUT Pvoid          buffer)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_4ENTER ("PROC_read", procId, dspAddr, numBytes, buffer) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;

    if (   ((IS_VALID_PROCID (procId)) == FALSE)
        || (numBytes == 0)
        || (buffer == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.procReadArgs.procId   = procId   ;
        args.apiArgs.procReadArgs.dspAddr  = dspAddr  ;
        args.apiArgs.procReadArgs.numBytes = numBytes ;
        args.apiArgs.procReadArgs.buffer   = buffer   ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_READ, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_write
 *
 *  @desc   This function allows GPP side applications to write to the DSP
 *          memory space
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_write (IN ProcessorId    procId,
            IN Uint32         dspAddr,
            IN Uint32         numBytes,
            IN Pvoid          buffer)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_4ENTER ("PROC_write", procId, dspAddr, numBytes, buffer) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;

    if (   ((IS_VALID_PROCID (procId)) == FALSE)
        || (numBytes == 0)
        || (buffer == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.procWriteArgs.procId   = procId   ;
        args.apiArgs.procWriteArgs.dspAddr  = dspAddr  ;
        args.apiArgs.procWriteArgs.numBytes = numBytes ;
        args.apiArgs.procWriteArgs.buffer   = buffer   ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_WRITE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_write", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_start
 *
 *  @desc   Starts execution of the loaded code on DSP from the starting
 *          point specified in the DSP executable loaded earlier by call to
 *          PROC_load ().
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_start (IN  ProcessorId  procId)
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
    Bool       csObjExists = FALSE ;
    CMD_Args   args ;

    TRC_1ENTER ("PROC_start", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (PROC_stateObj.syncCsObj != NULL) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            csObjExists = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (    DRV_CHECK_CURSTATUS (
                                PROC_stateObj.curStatus.isAttached [procId])
                ==  FALSE) {
                status = DSP_EATTACHED ;
                SET_FAILURE_REASON ;
            }
            else if (   DRV_CHECK_CURSTATUS (
                                PROC_stateObj.curStatus.isStarted [procId])
                     == TRUE) {
                /* Check if this DSP has been started in this process. */
                status = DSP_EALREADYSTARTED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.procStartArgs.procId = procId ;

                status = DRV_INVOKE (DRV_handle, CMD_PROC_START, &args) ;
                if (DSP_SUCCEEDED (status)) {
                    DRV_SET_CURSTATUS (
                                  PROC_stateObj.curStatus.isStarted [procId]) ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            if (csObjExists == TRUE) {
                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("PROC_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_stop
 *
 *  @desc   Stops the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_stop (IN  ProcessorId  procId)
{
    DSP_STATUS status      = DSP_SOK ;
    DSP_STATUS tmpStatus   = DSP_SOK ;
    Bool       csObjExists = FALSE ;
    CMD_Args   args ;

    TRC_1ENTER ("PROC_stop", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (PROC_stateObj.syncCsObj != NULL) {
            status = _SYNC_USR_enterCS (PROC_stateObj.syncCsObj) ;
            csObjExists = TRUE ;
        }

        if (DSP_SUCCEEDED (status)) {
            if (   DRV_CHECK_CURSTATUS (
                                PROC_stateObj.curStatus.isAttached [procId])
                == FALSE) {
                status = DSP_EATTACHED ;
                SET_FAILURE_REASON ;
            }
            else if (    DRV_CHECK_CURSTATUS (
                                    PROC_stateObj.curStatus.isStarted [procId])
                ==  FALSE) {
                /* Check if this DSP has been started in this process. */
                status = DSP_EACCESSDENIED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.procStopArgs.procId = procId ;

                status = DRV_INVOKE (DRV_handle, CMD_PROC_STOP, &args) ;
                DRV_RESET_CURSTATUS(PROC_stateObj.curStatus.isStarted [procId]);
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (csObjExists == TRUE) {
                tmpStatus = _SYNC_USR_leaveCS (PROC_stateObj.syncCsObj) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("PROC_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_GetSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_GetSymbolAddress (IN   ProcessorId   procId,
                       IN   Char8 *       symbolName,
                       OUT  Uint32 *      dspAddr)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_3ENTER ("PROC_GetSymbolAddress", procId, symbolName, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (symbolName != NULL) ;
    DBC_Require (dspAddr    != NULL) ;

    if (    ((IS_VALID_PROCID (procId)) == FALSE)
        ||  (symbolName == NULL)
        ||  (dspAddr    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.procGetSymbolAddressArgs.procId     = procId ;
        args.apiArgs.procGetSymbolAddressArgs.symbolName = symbolName ;
        args.apiArgs.procGetSymbolAddressArgs.dspAddr    = dspAddr ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_GETSYMBOLADDRESS, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_GetSymbolAddress", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations on
 *          the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_control (IN  ProcessorId procId,
              IN  Int32       cmd,
                  Pvoid       arg)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_3ENTER ("PROC_control", procId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId) == FALSE) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.procControlArgs.procId = procId ;
        args.apiArgs.procControlArgs.cmd    = cmd ;
        args.apiArgs.procControlArgs.arg    = arg ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_CONTROL, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_control", status) ;

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   PROC_resetCurStatus
 *
 *  @desc   Reset all CurStatus values for this process.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
PROC_resetCurStatus (Void)
{
    Uint32 i ;
#if defined (POOL_COMPONENT)
    Uint32 j ;
#endif /* if defined (POOL_COMPONENT) */

    TRC_0ENTER ("PROC_resetCurStatus") ;

    for (i = 0 ; i < MAX_DSPS ; i++) {
        DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.isAttached  [i]) ;
        DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.isStarted   [i]) ;
#if defined (MSGQ_COMPONENT)
        DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.mqtIsOpened [i]) ;
#endif /* if defined (MSGQ_COMPONENT) */
#if defined (POOL_COMPONENT)
        for (j = 0 ; j < MAX_POOLENTRIES ; j++) {
            DRV_RESET_CURSTATUS (PROC_stateObj.curStatus.poolIsOpened [i][j]) ;
        }
#endif /* if defined (POOL_COMPONENT) */
    }

    TRC_0LEAVE ("PROC_resetCurStatus") ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PROC_instrument
 *
 *  @desc   Gets the instrumentation data associated with PROC sub-component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_instrument (IN ProcessorId procId, OUT PROC_Instrument * retVal)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("PROC_instrument", procId, retVal) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (retVal != NULL) ;

    if ((IS_VALID_PROCID (procId) == FALSE) || (retVal == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.procInstrumentArgs.procId    = procId ;
        args.apiArgs.procInstrumentArgs.procStats = retVal ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_INSTRUMENT, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PROC_instrument", status) ;

    return status ;
}
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PROC_debug
 *
 *  @desc   Prints the debug information summarizing the current status
 *          of the PROC component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
PROC_debug (IN ProcessorId procId)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_0ENTER ("PROC_debug") ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (IS_VALID_PROCID (procId)) {
        args.apiArgs.procDebugArgs.procId = procId ;

        status = DRV_INVOKE (DRV_handle, CMD_PROC_DEBUG, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }

    TRC_0LEAVE ("PROC_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  To be deprecated: Backward Compatibility definitions for the DSP/BIOS LINK
 *                    interface.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PROC_Setup (Void)
{
    return PROC_setup (NULL) ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
