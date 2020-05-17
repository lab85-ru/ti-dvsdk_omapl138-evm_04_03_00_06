/** ============================================================================
 *  @file   pmgr_proc.c
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   Implementation of sub-component PMGR_PROC.
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
#include <procdefs.h>
#include <loaderdefs.h>
#include <linkcfgdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- Processor Manager           */
#include <pmgr_proc.h>
#if defined (CHNL_COMPONENT)
#include <pmgr_chnl.h>
#endif /* if defined (CHNL_COMPONENT) */
#if defined (MSGQ_COMPONENT)
#include <pmgr_msgq.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- Link Driver                 */
#include <cfgmap.h>
#if defined (POOL_COMPONENT)
#include <ldrv_pool.h>
#endif /* if defined (POOL_COMPONENT) */
#if defined (RINGIO_COMPONENT)
#include <ldrv_ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (MPLIST_COMPONENT)
#include <ldrv_mplist.h>
#endif /* if defined (MPLIST_COMPONENT) */

#include <ldrv.h>
#include <ldrv_proc.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_PMGR_PROC

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_PMGR_PROC, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @name   PMGR_SetupRefCount
 *
 *  @desc   Reference count indicating the number of times PROC_setup is called.
 *  ============================================================================
 */
STATIC Bool PMGR_SetupRefCount = 0 ;

/** ============================================================================
 *  @name   PMGR_ClientInfo
 *
 *  @desc   An element that holds process info and that can be manipulated using
 *          LIST.
 *
 *  @field  listElement
 *              Structure that allows it to be used by LIST.
 *  @field  prcsInfo
 *              Placeholder for process information.
 *  ============================================================================
 */
typedef struct PMGR_ClientInfo_tag {
    ListElement   listElement ;
    PrcsObject  * prcsInfo ;
} PMGR_ClientInfo ;


/** ============================================================================
 *  @name   PMGR_PROC_SetupObj
 *
 *  @desc   Object containing information regarding setup of this subcomponent.
 *
 *  @field  signature
 *              Signature of this object.
 *  @field  mutex
 *              Critical section object to ensure mutual exclusion.
 *  ============================================================================
 */
typedef struct PMGR_PROC_SetupObj_tag {
    Uint32         signature ;
    SyncCsObject * mutex [MAX_DSPS] ;
} PMGR_PROC_SetupObj ;


/** ============================================================================
 *  @name   PMGR_PROC_Object
 *
 *  @desc   Object containing information maintained by this subcomponent.
 *
 *  @field  signature
 *              Signature of this object.
 *  @field  clients
 *              List of clients that have attached to the processor.
 *  @field  entryPoint
 *              Entry point of the executable loaded on target processor.
 *  @field  objCtx
 *              Pointer to context object for the loader.
 *  @field  loaderIntf
 *              Interface table for the loader.
 *  @field  attachRefCount
 *              PROC_attach/PROC_detach reference count.
 *  @field  isLoaded
 *              Indicates whether the processor has been loaded.
 *  @field  startRefCount
 *              PROC_start/PROC_stop reference count.
 *  ============================================================================
 */
typedef struct PMGR_PROC_Object_tag {
    Uint32             signature ;
    List *             clients ;
    Uint32             entryPoint ;
    Void *             objCtx ;
    LOADER_Interface * loaderIntf ;
    Uint32             attachRefCount ;
    Bool               isLoaded ;
    Uint32             startRefCount ;
} PMGR_PROC_Object ;


/** ============================================================================
 *  @name   PMGR_SetupObj
 *
 *  @desc   Object containing information regarding setup of this subcomponent.
 *  ============================================================================
 */
STATIC PMGR_PROC_SetupObj PMGR_SetupObj ;

/** ============================================================================
 *  @name   PMGR_ProcObj
 *
 *  @desc   Array of PMGR_PROC_Object for each processor
 *  ============================================================================
 */
STATIC PMGR_PROC_Object   PMGR_ProcObj [MAX_DSPS] ;

/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;

/** ============================================================================
 *  @name   CFGMAP_Config
 *
 *  @desc   Global structure containing DSPLINK configuration mapping
 *          information for DSP/BIOS LINK.
 *  ============================================================================
 */
extern CFGMAP_Object * CFGMAP_Config [] ;


/** ============================================================================
 *  @func   PMGR_PROC_setup
 *
 *  @desc   Sets up the necessary data structures for the PMGR_PROC
 *          sub-component.
 *          Also stores the client who called this function. This information
 *          is used in PROC_destroy ().
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_setup (IN LINKCFG_Object * linkCfg)
{
    DSP_STATUS status  = DSP_SOK ;
    Uint32     i       = 0 ;
    Bool       toSetup = FALSE ;

    DBC_Require (linkCfg != NULL) ;

    SYNC_ProtectionStart () ;
    if (PMGR_SetupRefCount == 0) {
        toSetup = TRUE ;
    }
    else {
        status = DSP_SALREADYSETUP ;
    }
    PMGR_SetupRefCount++ ;
    SYNC_ProtectionEnd () ;

    TRC_0ENTER ("PMGR_PROC_setup") ;
    TRC_1PRINT (TRC_LEVEL1, "    toSetup : %d\n", toSetup) ;

    /*  ------------------------------------------------------------------------
     *  First caller sets up the DSPLINK driver.
     *  ------------------------------------------------------------------------
     */
    if (toSetup == TRUE) {
        status = OSAL_Initialize () ;

        PMGR_SetupObj.signature = SIGN_PROC_SETUP ;

        for (i = 0 ; i < MAX_DSPS ; i++) {
            PMGR_SetupObj.mutex [i] = NULL ;

            PMGR_ProcObj [i].signature      = SIGN_NULL ;
            PMGR_ProcObj [i].clients        = NULL ;
            PMGR_ProcObj [i].loaderIntf     = NULL ;
            PMGR_ProcObj [i].objCtx         = NULL ;
            PMGR_ProcObj [i].attachRefCount = 0 ;
            PMGR_ProcObj [i].isLoaded       = FALSE ;
            PMGR_ProcObj [i].startRefCount  = 0 ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = LDRV_moduleInit (linkCfg) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
#if defined (MSGQ_COMPONENT)
            else {
                status = PMGR_MSGQ_setup () ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
#endif /* if defined (MSGQ_COMPONENT) */

            for (i = 0 ; (i < MAX_DSPS) && DSP_SUCCEEDED (status) ; i++) {
                status = SYNC_CreateCS (&(PMGR_SetupObj.mutex [i])) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_FAILED (status)) {
            PMGR_PROC_destroy () ;
        }
    }

    TRC_1LEAVE ("PMGR_PROC_setup", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_destroy
 *
 *  @desc   Destroys the data structures for the PMGR_PROC component,
 *          allocated earlier by a call to PROC_setup ().
 *          This function ensures that the calling client has performed
 *          PROC_setup ().
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_destroy (Void)
{
    DSP_STATUS   status    = DSP_SOK ;
    DSP_STATUS   tmpStatus = DSP_SOK ;
    Uint32       i         = 0 ;
    Bool         toDestroy = FALSE ;

    TRC_0ENTER ("PMGR_PROC_destroy") ;

    SYNC_ProtectionStart () ;
    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        PMGR_SetupRefCount-- ;
        if (PMGR_SetupRefCount == 0) {
            toDestroy = TRUE ;
            status = DSP_SDESTROYED ;
        }
    }
    SYNC_ProtectionEnd () ;

    /*  ------------------------------------------------------------------------
     *  Last caller finalizes the DSPLINK driver.
     *  ------------------------------------------------------------------------
     */
    if (toDestroy == TRUE) {
        for (i = 0 ; i < MAX_DSPS ; i++) {
            if (PMGR_SetupObj.mutex [i] != NULL) {
                tmpStatus = SYNC_DeleteCS (PMGR_SetupObj.mutex [i]) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
                PMGR_SetupObj.mutex [i]         = NULL ;
                PMGR_ProcObj [i].signature      = SIGN_NULL ;
                PMGR_ProcObj [i].clients        = NULL ;
                PMGR_ProcObj [i].loaderIntf     = NULL ;
                PMGR_ProcObj [i].objCtx         = NULL ;
                PMGR_ProcObj [i].attachRefCount = 0 ;
                PMGR_ProcObj [i].isLoaded       = FALSE ;
                PMGR_ProcObj [i].startRefCount  = 0 ;
            }
        }

#if defined (MSGQ_COMPONENT)
        tmpStatus = PMGR_MSGQ_destroy () ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
#endif /* if defined (MSGQ_COMPONENT) */

        tmpStatus = LDRV_moduleExit () ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = OSAL_Finalize () ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        PMGR_SetupObj.signature = SIGN_NULL ;
    }

    TRC_1LEAVE ("PMGR_PROC_destroy", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_attach
 *
 *  @desc   Attaches the client to the specified DSP and also
 *          initializes the DSP (if required).
 *          The first caller to this function actually initializes the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_attach (IN  ProcessorId   procId,
                  IN     PROC_Attrs *  attr,
                  IN     Void *        optArgs)
{
    DSP_STATUS        status     = DSP_SOK ;
    DSP_STATUS        tmpStatus  = DSP_SOK ;
    PMGR_ClientInfo * client     = NULL    ;
    PrcsObject *      prcsInfo   = NULL    ;
    Bool              toAttach   = FALSE   ;
    Bool              isAttached = FALSE   ;

    TRC_2ENTER ("PMGR_PROC_attach", procId, attr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    DBC_Assert (PMGR_SetupRefCount != 0) ;

    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_ProtectionStart () ;
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            toAttach = TRUE ;
        }
        PMGR_ProcObj [procId].attachRefCount++ ;
        SYNC_ProtectionEnd () ;

        status = PRCS_Create (&prcsInfo, optArgs) ;
        if (DSP_SUCCEEDED (status)) {
            if (toAttach == TRUE) {
                status = SYNC_EnterCS (PMGR_SetupObj.mutex [procId]) ;
                if (DSP_SUCCEEDED (status)) {
                    /*  --------------------------------------------------------
                     *  This is the first attach for this processor.
                     *  Create the client list and initialize the DSP.
                     *  --------------------------------------------------------
                     */
                    status = LIST_Create (&(PMGR_ProcObj [procId].clients)) ;

                    if (DSP_SUCCEEDED (status)) {
                        /* User wants load DSP with new values. */
                        if (attr != NULL) {
                            status = LDRV_init (procId, attr->dspCfgPtr) ;
                        }
                        else {
                            /* Use the values provided at setup time. */
                            status = LDRV_init (procId, NULL) ;
                        }

                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }

                    if (DSP_SUCCEEDED (status)) {
                        status = LDRV_PROC_init (procId) ;
                        if (DSP_SUCCEEDED (status)) {
                            PMGR_ProcObj [procId].objCtx     = NULL      ;
                            PMGR_ProcObj [procId].loaderIntf = NULL      ;
                            PMGR_ProcObj [procId].signature  = SIGN_PROC ;
                        }
                        else {
                            LIST_Delete (PMGR_ProcObj [procId].clients) ;
                            PMGR_ProcObj [procId].clients = NULL ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }

#if defined (CHNL_COMPONENT)
                    if (DSP_SUCCEEDED (status)) {
                        status = PMGR_CHNL_init (procId) ;
                        if (DSP_FAILED (status)) {
                            LIST_Delete (PMGR_ProcObj [procId].clients) ;
                            PMGR_ProcObj [procId].clients   = NULL ;
                            PMGR_ProcObj [procId].signature = SIGN_NULL ;
                            SET_FAILURE_REASON ;
                        }
                    }
#endif  /* if defined (CHNL_COMPONENT) */

                    tmpStatus = SYNC_LeaveCS (PMGR_SetupObj.mutex [procId]) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }

            }
            else {
                /* Check if caller is already attached to the DSP. */
                status = PMGR_PROC_isAttached (procId,
                                               prcsInfo,
                                               &isAttached) ;
                if (DSP_SUCCEEDED (status)) {
                    if (isAttached == TRUE) {
                        status = DSP_EALREADYCONNECTED ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status)) {
                status = SYNC_EnterCS (PMGR_SetupObj.mutex [procId]) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Add information about attached client to client list. */
                    status = MEM_Alloc ((Void **) &client,
                                        sizeof (PMGR_ClientInfo),
                                        MEM_DEFAULT) ;

                    if (DSP_SUCCEEDED (status)) {
                        DBC_Assert (client != NULL) ;
                        DBC_Assert (prcsInfo != NULL) ;

                        client->prcsInfo = prcsInfo ;
                        status = LIST_InitializeElement (
                                                &(client->listElement)) ;
                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }

                    if (DSP_SUCCEEDED (status)) {
                        status = LIST_PutTail (PMGR_ProcObj [procId].clients,
                                               &(client->listElement)) ;

                        if (DSP_FAILED (status)) {
                            /* Overwriting the failure from LIST to prevent
                             * confusion with upper layer error codes.
                             */
                            status = DSP_EFAIL ;
                            FREE_PTR (client) ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        FREE_PTR (client) ;
                    }

                    tmpStatus = SYNC_LeaveCS (PMGR_SetupObj.mutex [procId]) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

#if defined (NOTIFY_COMPONENT)
            if (DSP_SUCCEEDED (status)) {
                UEVENT_AddNewProcess () ;
            }
#endif /* #if defined (NOTIFY_COMPONENT)*/
            if (DSP_FAILED (status) && (toAttach == TRUE)) {
                LDRV_exit (procId) ;
            }

            if (DSP_FAILED (status)) {
                PRCS_Delete (prcsInfo) ;
                prcsInfo = NULL ;
                SYNC_ProtectionStart () ;
                PMGR_ProcObj [procId].attachRefCount-- ;
                SYNC_ProtectionEnd () ;
            }
            else {
                if (toAttach == FALSE) {
                    /* Return status indicating that at least one GPP process
                     * is already attached to the DSP.
                     */
                    status = DSP_SALREADYATTACHED ;
                }
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_attach", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_detach
 *
 *  @desc   Detaches the client from specified processor.
 *          If the caller is the owner of the processor, this function releases
 *          all the resources that this component uses and puts the DSP in an
 *          unusable state (from application perspective).
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_detach (IN ProcessorId procId, IN     Void * optArgs)
{
    DSP_STATUS        status     = DSP_SOK ;
    DSP_STATUS        tmpStatus  = DSP_SOK ;
    PrcsObject *      client     = NULL ;
    Bool              isEqual    = FALSE ;
    PMGR_ClientInfo * clientInfo = NULL ;
    PMGR_ClientInfo * nextClient = NULL ;
    Bool              toDetach   = FALSE ;

    TRC_1ENTER ("PMGR_PROC_detach", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_ProtectionStart () ;
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            status = DSP_EATTACHED ;
            SET_FAILURE_REASON ;
        }
        else {
            PMGR_ProcObj [procId].attachRefCount-- ;
            if (PMGR_ProcObj [procId].attachRefCount == 0) {
                toDetach = TRUE ;
                status = DSP_SDETACHED ;
            }
        }
        SYNC_ProtectionEnd () ;
    }

    if (DSP_SUCCEEDED (status)) {
        status = SYNC_EnterCS (PMGR_SetupObj.mutex [procId]) ;
        DBC_Assert (DSP_SUCCEEDED (status)) ;

        if (toDetach == TRUE) {
            /*  ----------------------------------------------------------------
             *  Force detach and clear out all allocated resources for this
             *  processor.
             *  ----------------------------------------------------------------
             */
#if defined (CHNL_COMPONENT)
            status = PMGR_CHNL_exit (procId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
#endif /* if defined (CHNL_COMPONENT) */

            tmpStatus = LDRV_PROC_exit (procId) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            while (  (!LIST_IsEmpty (PMGR_ProcObj [procId].clients))
                   && DSP_SUCCEEDED (status)) {
                status = LIST_GetHead (PMGR_ProcObj [procId].clients,
                                       (ListElement **) &clientInfo) ;
                if (DSP_SUCCEEDED (status)) {
                    status = PRCS_Delete (clientInfo->prcsInfo) ;
                    clientInfo->prcsInfo = NULL ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }

                    tmpStatus = FREE_PTR (clientInfo) ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
            }

            tmpStatus = LDRV_exit (procId) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }

            tmpStatus = LIST_Delete (PMGR_ProcObj [procId].clients) ;
            PMGR_ProcObj [procId].clients = NULL ;

            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }

            if (PMGR_ProcObj [procId].loaderIntf != NULL) {
                /* Check if object context from previous load exists.
                 * Finalize it if it exists.
                 */
                if (PMGR_ProcObj [procId].objCtx != NULL) {
                    tmpStatus = (*(PMGR_ProcObj [procId].loaderIntf->exit))(
                                         PMGR_ProcObj [procId].objCtx) ;
                    PMGR_ProcObj [procId].objCtx = NULL ;
                    if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
                PMGR_ProcObj [procId].loaderIntf = NULL ;
            }
            PMGR_ProcObj [procId].signature = SIGN_NULL ;
        }
        else {
            /*  ----------------------------------------------------------------
             *  This is not the last call to detach for this processor.
             *  Find the element in the client list and then remove it.
             *  ----------------------------------------------------------------
             */
            status = PRCS_Create (&client, optArgs) ;
            if (DSP_SUCCEEDED (status)) {
                status = LIST_First (PMGR_ProcObj [procId].clients,
                                     (ListElement **) &clientInfo) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;

                while (   (clientInfo != NULL)
                       && (isEqual    == FALSE)
                       && DSP_SUCCEEDED (status)) {
                    status = PRCS_IsSameContext (clientInfo->prcsInfo,
                                           client,
                                           &isEqual) ;

                    if (DSP_SUCCEEDED (status)) {
                        if (isEqual == TRUE) {
                            status = LIST_RemoveElement (
                                               PMGR_ProcObj [procId].clients,
                                               (ListElement *) clientInfo) ;

                            if (DSP_FAILED (status)) {
                                status = DSP_EFAIL ;
                                SET_FAILURE_REASON ;
                            }

                            tmpStatus = PRCS_Delete (clientInfo->prcsInfo) ;
                            clientInfo->prcsInfo = NULL ;
                            if (   DSP_FAILED (tmpStatus)
                                && DSP_SUCCEEDED (status)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }

                            tmpStatus = FREE_PTR (clientInfo) ;
                            if (   DSP_FAILED (tmpStatus)
                                && DSP_SUCCEEDED (status)) {
                                status = tmpStatus ;
                                SET_FAILURE_REASON ;
                            }
                        }
                        else {
                            status = LIST_Next(PMGR_ProcObj [procId].clients,
                                                (ListElement *)  clientInfo,
                                                (ListElement **) &nextClient) ;

                            if (DSP_SUCCEEDED (status)) {
                                clientInfo = nextClient ;
                            }
                            else {
                                status = DSP_EFAIL ;
                                SET_FAILURE_REASON ;
                            }
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }

                if (DSP_FAILED (status)) {
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
                else {
                    if (isEqual == FALSE) {
                        status = DSP_EATTACHED ;
                        SET_FAILURE_REASON ;
                    }
                }

                tmpStatus = PRCS_Delete (client) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }

        tmpStatus = SYNC_LeaveCS (PMGR_SetupObj.mutex [procId]) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

#if defined (NOTIFY_COMPONENT)
        if (status != DSP_EATTACHED) {
            UEVENT_RemoveUserProcess () ;
        }
#endif /* #if defined (NOTIFY_COMPONENT) */

        if (DSP_SUCCEEDED (status)) {
            if (toDetach == TRUE) {
                /* Return status indicating the last GPP process is detaching
                 * from the DSP.
                 */
                status = DSP_SDETACHED ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_detach", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_getState
 *
 *  @desc   Gets the current status of processor.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_getState (IN   ProcessorId     procId,
                    OUT  PROC_State *    procState)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("PMGR_PROC_getState", procId, status) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (procState != NULL ) ;

    /* Check if the PROC component has been setup. */
    if (PMGR_SetupRefCount == 0) {
        *procState = ProcState_Unknown ;
    }
    else {
        /* Check if any process has attached to the DSP. */
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            *procState = ProcState_Setup ;
        }
        else {
            /* Get the DSP state by querying the Link Driver. */
            status = LDRV_PROC_getState (procId, procState) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_getState", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_load
 *
 *  @desc   Loads the specified DSP executable on the target DSP.
 *          It ensures that the caller owns the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_load (IN   ProcessorId  procId,
                IN   Char8 *      imagePath,
                IN   Uint32       argc,
                IN   Char8 **     argv)
{
    DSP_STATUS         status       = DSP_SOK ;
    LOADER_Interface * loaderIntf   = NULL ;
    KFILE_Interface *  kfileIntf    = NULL ;
    Bool               toLoad       = FALSE ;
    LINKCFG_Dsp *      dspObj ;
    LINKCFG_DspConfig * dspCfg ;
    LoaderObject       loaderObj ;
    LoaderInitArgs     loaderArgs ;

    TRC_4ENTER ("PMGR_PROC_load", procId, imagePath, argc, argv) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (imagePath != NULL ) ;
    DBC_Require (   ((argc != 0) && (argv != NULL))
                 || ((argc == 0) && (argv == NULL))) ;

    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_ProtectionStart () ;
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            status = DSP_EATTACHED ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the DSP has not been loaded yet.*/
            if (PMGR_ProcObj [procId].isLoaded == FALSE) {
                toLoad = TRUE ;
                PMGR_ProcObj [procId].isLoaded = TRUE ;
            }
            else {
                status = DSP_SALREADYLOADED ;
            }
        }
        SYNC_ProtectionEnd () ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (toLoad == TRUE) {
            if (PMGR_ProcObj [procId].loaderIntf != NULL) {
                loaderIntf = PMGR_ProcObj [procId].loaderIntf ;
                DBC_Assert (loaderIntf != NULL) ;

                /* Check if object context from previous load exists.
                 * Finalize it if it exists.
                 */
                if (PMGR_ProcObj [procId].objCtx != NULL) {
                    status = (*(loaderIntf->exit)) (
                                         PMGR_ProcObj [procId].objCtx) ;
                    PMGR_ProcObj [procId].objCtx = NULL ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Get the loader interface from LDRV_PROC. */
                LDRV_PROC_getLoader (procId, &loaderIntf, &kfileIntf) ;
                /* Do not check for NULL kfileIntf, since it is possible that
                 * some loaders do not need to use KFILE.
                 */
                if (loaderIntf == NULL) {
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                PMGR_ProcObj [procId].loaderIntf = loaderIntf ;

                dspCfg    = LDRV_LinkCfgPtr->dspConfigs [procId] ;
                dspObj    = dspCfg->dspObject ;
                loaderArgs.dspArch     = (DspArch) dspObj->dspArch ;
                loaderArgs.kfileIntf   = kfileIntf ;

                status = (*(loaderIntf->init)) (procId,
                                                imagePath,
                                                &loaderArgs,
                                                &PMGR_ProcObj [procId].objCtx) ;
                if (DSP_SUCCEEDED (status)) {
                    loaderObj.objCtx = PMGR_ProcObj [procId].objCtx ;
                    loaderObj.endian = (Endianism) dspObj->endian ;
                    loaderObj.maduSize      = dspObj->maduSize ;
                    loaderObj.fnWriteDspMem = &LDRV_PROC_write ;
                    loaderObj.fnAddrConvert = &LDRV_PROC_addrConvert ;
                    status = (*(loaderIntf->load)) (
                                          procId,
                                          &loaderObj,
                                          argc,
                                          argv,
                                          &(PMGR_ProcObj [procId].entryPoint)) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                        if (status == DSP_ERANGE) {
                            PRINT_Printf (
                                     "Error: DSP-side memory map does not "
                                     "match configuration.\n"
                                     "Compare DSP-side TCF/MAP file with "
                                     "/dsplink/config/all/CFG_<PLATFORM>.c\n") ;
                        }
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                status = LDRV_PROC_setState (procId, ProcState_Loaded) ;

                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SYNC_ProtectionStart () ;
                /* Reset state to not loaded. */
                PMGR_ProcObj [procId].objCtx = NULL ;
                PMGR_ProcObj [procId].isLoaded = FALSE ;
                SYNC_ProtectionEnd () ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_load", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_loadSection
 *
 *  @desc   Loads the specified section of DSP executable onto the target DSP.
 *          It ensures that the client owns the DSP.
 *          Currently this function is not implemented.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_loadSection (IN    ProcessorId  procId,
                       IN    Char8 *      imagePath,
                       IN    Uint32       sectID)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("PMGR_PROC_loadSection", procId, imagePath, sectID) ;

    status = DSP_ENOTIMPL ;

    TRC_1LEAVE ("PMGR_PROC_loadSection", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_read
 *
 *  @desc   This function allows GPP side applications to read from the DSP
 *          memory space.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_read (IN     ProcessorId    procId,
                IN     Uint32         dspAddr,
                IN     Uint32         numBytes,
                IN OUT Pvoid          buffer)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("PMGR_PROC_read", procId, dspAddr, numBytes, buffer) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;

    status = LDRV_PROC_read (procId,
                             dspAddr,
                             Endianism_Default,
                             numBytes,
                             buffer) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_read", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_write
 *
 *  @desc   This function allows GPP side applications to write to the DSP
 *          memory space
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_write (IN ProcessorId    procId,
                 IN Uint32         dspAddr,
                 IN Uint32         numBytes,
                 IN Pvoid          buffer)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("PMGR_PROC_write", procId, dspAddr, numBytes, buffer) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (numBytes != 0) ;
    DBC_Require (buffer != NULL) ;

    status = LDRV_PROC_write (procId,
                              dspAddr,
                              Endianism_Default,
                              numBytes,
                              buffer) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_write", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_start
 *
 *  @desc   Starts execution of the loaded code on DSP from the starting
 *          point specified in the DSP executable loaded earlier by call to
 *          PROC_load ().
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_start (IN  ProcessorId  procId)
{
    DSP_STATUS   status    = DSP_SOK ;
    Bool         toStart   = FALSE ;

    TRC_1ENTER ("PMGR_PROC_start", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_ProtectionStart () ;
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            status = DSP_EATTACHED ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Check if the DSP has not been started yet.*/
            if (PMGR_ProcObj [procId].startRefCount == 0) {
                toStart = TRUE ;
            }
            else {
                status = DSP_SALREADYSTARTED ;
            }
            PMGR_ProcObj [procId].startRefCount++ ;
        }
        SYNC_ProtectionEnd () ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (toStart == TRUE) {
            status = LDRV_PROC_start (procId,
                                      PMGR_ProcObj [procId].entryPoint) ;
            if (DSP_FAILED (status)) {
                SYNC_ProtectionStart () ;
                PMGR_ProcObj [procId].startRefCount-- ;
                SYNC_ProtectionEnd () ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_start", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_stop
 *
 *  @desc   Stops the specified DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_stop (IN  ProcessorId  procId)
{
    DSP_STATUS   status    = DSP_SOK ;
    DSP_STATUS   tmpStatus = DSP_SOK ;
    Bool         toStop    = FALSE   ;

    TRC_1ENTER ("PMGR_PROC_stop", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (PMGR_SetupRefCount == 0) {
        status = DSP_ESETUP ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_ProtectionStart () ;
        if (PMGR_ProcObj [procId].attachRefCount == 0) {
            status = DSP_EATTACHED ;
            SET_FAILURE_REASON ;
        }
        else {
            if (PMGR_ProcObj [procId].startRefCount == 0) {
                status = DSP_ESTARTED ;
                SET_FAILURE_REASON ;
            }
            else {
                PMGR_ProcObj [procId].startRefCount-- ;
                if (PMGR_ProcObj [procId].startRefCount == 0) {
                    toStop = TRUE ;
                    status = DSP_SSTOPPED ;
                }
            }
        }
        SYNC_ProtectionEnd () ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (toStop == TRUE) {
            tmpStatus = LDRV_PROC_stop (procId) ;
            if (DSP_SUCCEEDED (tmpStatus)) {
                /* Indicate that processor needs to be re-loaded. */
                PMGR_ProcObj [procId].isLoaded = FALSE ;
            }
            else {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_PROC_stop", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_getSymbolAddress
 *
 *  @desc   Gets the DSP address corresponding to a symbol within a DSP
 *          executable currently loaded on the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_PROC_getSymbolAddress (IN   ProcessorId   procId,
                            IN   Char8 *       symbolName,
                            OUT  Uint32 *      dspAddr)
{
    DSP_STATUS        status    = DSP_SOK ;
    PROC_State         procState ;
    LOADER_Interface * loaderIntf ;

    TRC_3ENTER ("PMGR_PROC_getSymbolAddress", procId, symbolName, dspAddr) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (symbolName != NULL) ;
    DBC_Require (dspAddr != NULL) ;

    status = LDRV_PROC_getState (procId, &procState) ;
    if (DSP_SUCCEEDED (status)) {
        /* Ensure that DSP is currently loaded with a valid DSP executable. */
        if (    (procState != ProcState_Loaded)
            &&  (procState != ProcState_Started)
            &&  (procState != ProcState_Stopped)) {
            status = DSP_EWRONGSTATE ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Get the symbol address corresponding to the symbol name. */
            if ((PMGR_ProcObj [procId].loaderIntf) != NULL) {
                loaderIntf = PMGR_ProcObj [procId].loaderIntf ;
                DBC_Assert (loaderIntf != NULL) ;
                if (loaderIntf != NULL) {
                    if (PMGR_ProcObj [procId].objCtx != NULL) {
                        status = (*(loaderIntf->getSymbolAddress)) (
                                                   procId,
                                                   PMGR_ProcObj [procId].objCtx,
                                                   symbolName,
                                                   dspAddr) ;
                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                }
                else {
                    status = DSP_EWRONGSTATE ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_getSymbolAddress", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_control
 *
 *  @desc   Provides a hook to perform device dependent control operations on
 *          the DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_control (IN  ProcessorId procId,
                   IN  Int32       cmd,
                       Pvoid       arg)
{
    DSP_STATUS    status  = DSP_SOK ;

    TRC_3ENTER ("PMGR_PROC_control", procId, cmd, arg) ;

    status = LDRV_PROC_control (procId, cmd, arg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_control", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_isAttached
 *
 *  @desc   Function to check whether the client identified by the specified
 *          'client' object is attached to the specified processor.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_isAttached (IN  ProcessorId  procId,
                      IN  PrcsObject * client,
                      OUT Bool *       isAttached)
{
    DSP_STATUS        status     = DSP_SOK ;
    DSP_STATUS        tmpStatus  = DSP_SOK ;
    Bool              isEqual    = FALSE   ;
    PMGR_ClientInfo * clientInfo = NULL    ;
    PMGR_ClientInfo * nextClient = NULL    ;

    TRC_3ENTER ("PMGR_PROC_isAttached", procId, client, isAttached) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (client != NULL) ;
    DBC_Require (isAttached != NULL) ;

    status = SYNC_EnterCS (PMGR_SetupObj.mutex [procId]) ;

    if (    (PMGR_ProcObj [procId].attachRefCount == 0)
        &&  (DSP_SUCCEEDED (status))) {
        *isAttached = FALSE ;
    }
    else if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        status = LIST_First (PMGR_ProcObj [procId].clients,
                             (ListElement **) &clientInfo) ;

        if (DSP_FAILED (status)) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }

        while (     (clientInfo != NULL)
               &&   (isEqual == FALSE)
               &&   DSP_SUCCEEDED (status)) {
            status = PRCS_IsSameContext (clientInfo->prcsInfo,
                                         client,
                                         &isEqual) ;

            if (DSP_SUCCEEDED (status) && (isEqual == FALSE)) {
                status = LIST_Next (PMGR_ProcObj [procId].clients,
                                    (ListElement *)  clientInfo,
                                    (ListElement **) &nextClient) ;

                if (DSP_SUCCEEDED (status)) {
                    clientInfo = nextClient ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        *isAttached = isEqual ;
    }

    tmpStatus = SYNC_LeaveCS (PMGR_SetupObj.mutex [procId]) ;
    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
        status = tmpStatus ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_isAttached", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_PROC_getSetupRefCount
 *
 *  @desc   Returns the current reference count for PROC_setup/PROC_destroy.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Uint32
PMGR_PROC_getSetupRefCount (Void)
{
    Uint32 refCount ;

    TRC_0ENTER ("PMGR_PROC_getSetupRefCount") ;

    refCount = PMGR_SetupRefCount ;

    TRC_1PRINT (TRC_LEVEL2, "    PMGR_PROC_getSetupRefCount [%d]\n", refCount) ;
    TRC_0LEAVE ("PMGR_PROC_getSetupRefCount") ;

    return refCount ;
}


/** ============================================================================
 *  @func   PMGR_PROC_getAttachRefCount
 *
 *  @desc   Returns the current reference count for PROC_attach/PROC_detach.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
Uint32
PMGR_PROC_getAttachRefCount (IN Uint32 procId)
{
    Uint32 refCount ;

    TRC_1ENTER ("PMGR_PROC_getAttachRefCount", procId) ;

    refCount = PMGR_ProcObj [procId].attachRefCount ;

    TRC_1PRINT (TRC_LEVEL2, "    PMGR_PROC_getAttachRefCount [%d]\n", refCount);
    TRC_0LEAVE ("PMGR_PROC_getAttachRefCount") ;

    return refCount ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_PROC_instrument
 *
 *  @desc   Gets the instrumentation data associated with PMGR_PROC
 *          sub-component.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_PROC_instrument (IN ProcessorId procId, OUT PROC_Instrument * retVal)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("PMGR_PROC_instrument", retVal) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (retVal != NULL) ;

    status = LDRV_PROC_instrument (procId, retVal) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_PROC_instrument () [0x%x]\n",
                status) ;

    return status ;
}
#endif

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_PROC_debug
 *
 *  @desc   Prints the debug information summarizing the current status
 *          of the PMGR_PROC component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
PMGR_PROC_debug (IN ProcessorId procId)
{
    DSP_STATUS        status      = DSP_SOK ;
    PMGR_ClientInfo * clientInfo  = NULL    ;
    PMGR_ClientInfo * nextClient  = NULL    ;
    Uint32            clientCount = 0       ;

    TRC_1ENTER ("PMGR_PROC_debug", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    TRC_1PRINT (TRC_LEVEL4, "Processor Id : %d\n", procId) ;

    TRC_1PRINT (TRC_LEVEL4, "PMGR_SetupRefCount: %d\n", PMGR_SetupRefCount) ;

    if (PMGR_SetupRefCount != 0) {
        TRC_1PRINT (TRC_LEVEL4,
                    "   Attach count      = %d\n",
                    (PMGR_ProcObj [procId].attachRefCount)) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "   Start count       = %d\n",
                    (PMGR_ProcObj [procId].startRefCount)) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "   entryPoint        = [0x%x]\n",
                    PMGR_ProcObj [procId].entryPoint) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "   Loader objCtx     = [0x%x]\n",
                    PMGR_ProcObj [procId].objCtx) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "   Loader loaderIntf = [0x%x]\n",
                    PMGR_ProcObj [procId].loaderIntf) ;

        TRC_1PRINT (TRC_LEVEL4,
                    "   Client : [0x%x]\n",
                    PMGR_ProcObj [procId].clients) ;

        if (PMGR_ProcObj [procId].clients != NULL) {
            status = LIST_First (PMGR_ProcObj [procId].clients,
                                 (ListElement **) &clientInfo) ;
            if (DSP_SUCCEEDED (status)) {
                while (clientInfo != NULL) {
                    clientCount++ ;

                    TRC_2PRINT (TRC_LEVEL4,
                                "        client [%d] = [0x%x]\n",
                                clientCount, &(clientInfo->prcsInfo)) ;

                    status = LIST_Next (PMGR_ProcObj [procId].clients,
                                        (ListElement *) clientInfo,
                                        (ListElement **) &nextClient) ;
                    if (DSP_FAILED (status)) {
                        clientInfo = NULL ;
                    }
                    else {
                        clientInfo = nextClient ;
                    }
                }
            }
            else {
                TRC_0PRINT (TRC_LEVEL4, "Client list is empty\n") ;
            }

            if (DSP_SUCCEEDED (status)) {
                LDRV_PROC_debug (procId) ;
            }
        }
    }
    else {
        TRC_0PRINT (TRC_LEVEL4, "Processor is not yet setup\n") ;
    }

    TRC_1LEAVE ("PMGR_PROC_debug", status) ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
