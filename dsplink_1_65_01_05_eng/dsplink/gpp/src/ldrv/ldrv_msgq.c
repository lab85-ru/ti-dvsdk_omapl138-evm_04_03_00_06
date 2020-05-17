/** ============================================================================
 *  @file   ldrv_msgq.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Implements the LDRV MSGQ driver.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <linkcfgdefs.h>
#include <msgqdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Generic Function              */
#include<gen_utils.h>
#include <list.h>

/*  ----------------------------------- User API                      */
#include <msgq.h>

/*  ----------------------------------- Link Driver                   */
#include <cfgmap.h>
#include <ldrv.h>
#include <ldrv_mqt.h>
#include <ldrv_proc.h>
#include <ldrv_msgq.h>
#include <ldrv_pool.h>

/*  ----------------------------------- LOG  Headers                  */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_LDRV_MSGQ


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_MSGQ, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ----------------------------------------------------------------------------
 *  @name   LDRV_MSGQ_StateObj
 *
 *  @desc   This is the global state object for LDRV MSGQ component.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRV_MSGQ_State LDRV_MSGQ_StateObj ;


/*  ----------------------------------------------------------------------------
 *  @name   LDRV_MSGQ_IsInitialized
 *
 *  @desc   Flag for tracking initialization of component.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool   LDRV_MSGQ_IsInitialized = FALSE ;


/** ============================================================================
 *  @func   LDRV_MSGQ_setup
 *
 *  @desc   This function initializes the MSGQ component.
 *
 *  @modif  LDRV_MSGQ_IsInitialized, LDRV_MSGQ_StateObj
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_setup (Void)
{
    DSP_STATUS                status       = DSP_SOK ;
    LINKCFG_Gpp *             gppObj ;

    TRC_0ENTER ("LDRV_MSGQ_setup") ;

    DBC_Require (LDRV_MSGQ_IsInitialized == FALSE) ;

    if (LDRV_MSGQ_IsInitialized == FALSE) {
        gppObj = LDRV_LinkCfgPtr->gppObject ;

        /*  Initialize the msgQueues and transports fields.  */
        LDRV_MSGQ_StateObj.msgqHandles = NULL ;
        LDRV_MSGQ_StateObj.transports  = NULL ;

        /*  --------------------------------------------------------------------
         *  Initialize the global state variable.
         *  --------------------------------------------------------------------
         */
        LDRV_MSGQ_StateObj.maxMsgqs     = gppObj->maxMsgqs ;
        LDRV_MSGQ_StateObj.numDsps      = MAX_DSPS ;
        LDRV_MSGQ_StateObj.errorQueue   = (MSGQ_Queue) MSGQ_INVALIDMSGQ ;
        LDRV_MSGQ_StateObj.errorPoolId  = POOL_INVALIDID ;
        
		/* Create the lock */
		LDRV_MSGQ_StateObj.lock = SYNC_SpinLockCreate () ;

        /*  --------------------------------------------------------------------
         *  Allocate memory for the message queue handles.
         *  --------------------------------------------------------------------
         */
        status = MEM_Calloc ((Void **) &(LDRV_MSGQ_StateObj.msgqHandles),
                             (  LDRV_MSGQ_StateObj.maxMsgqs
                              * sizeof (LDRV_MSGQ_Handle)),
                             MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            LDRV_MSGQ_IsInitialized = TRUE ;
        }

        /*  --------------------------------------------------------------------
         *  Allocate memory for the transport array and fill its fields.
         *  --------------------------------------------------------------------
         */
        if (DSP_SUCCEEDED (status)) {
            status = MEM_Alloc ((Void **) &(LDRV_MSGQ_StateObj.transports),
                                (  LDRV_MSGQ_StateObj.numDsps
                                 * sizeof (LDRV_MSGQ_TransportObj)),
                                MEM_DEFAULT) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_FAILED (status)) {
            LDRV_MSGQ_IsInitialized = FALSE ;
            if (LDRV_MSGQ_StateObj.msgqHandles != NULL) {
                FREE_PTR (LDRV_MSGQ_StateObj.msgqHandles) ;
            }
            if (LDRV_MSGQ_StateObj.transports != NULL) {
                FREE_PTR (LDRV_MSGQ_StateObj.transports) ;
            }
        }
    }

    DBC_Ensure (   (   (LDRV_MSGQ_IsInitialized == TRUE)
                    && (DSP_SUCCEEDED (status)))
                || (   (LDRV_MSGQ_IsInitialized == FALSE)
                    && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_setup", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_destroy
 *
 *  @desc   This function finalizes the MSGQ component.
 *
 *  @modif  LDRV_MSGQ_IsInitialized, LDRV_MSGQ_StateObj
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_destroy (Void)
{
    DSP_STATUS status    = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    TRC_0ENTER ("LDRV_MSGQ_destroy") ;

    if (LDRV_MSGQ_IsInitialized == TRUE) {
        /*  --------------------------------------------------------------------
         *  Free memory allocated for any open message queue handles.
         *  --------------------------------------------------------------------
         */
        if (LDRV_MSGQ_StateObj.msgqHandles != NULL) {
            tmpStatus = FREE_PTR (LDRV_MSGQ_StateObj.msgqHandles) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        /*  --------------------------------------------------------------------
         *  Free memory allocated for the transports.
         *  --------------------------------------------------------------------
         */
        if (LDRV_MSGQ_StateObj.transports != NULL) {
            tmpStatus = FREE_PTR (LDRV_MSGQ_StateObj.transports) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

		/* Delete the lock */
		SYNC_SpinLockDelete (LDRV_MSGQ_StateObj.lock) ;
		
        LDRV_MSGQ_IsInitialized = FALSE ;
    }

    DBC_Ensure (LDRV_MSGQ_IsInitialized == FALSE) ;

    TRC_1LEAVE ("LDRV_MSGQ_destroy", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_transportOpen
 *
 *  @desc   This function initializes the transport associated with the
 *          specified processor.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs)
{
    DSP_STATUS                 status = DSP_SOK ;
    MQT_Interface *            mqtInterface = NULL ;
    LDRV_MSGQ_TransportHandle  mqtHandle ;
    LINKCFG_Dsp *              dspObj ;
    LINKCFG_DspConfig *        dspCfg ;
    LINKCFG_LinkDrv *          linkDrv ;
    Int32                      strCmpResult ;
    Uint16                     j ;

    TRC_2ENTER ("LDRV_MSGQ_transportOpen", procId, attrs) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (procId < LDRV_MSGQ_StateObj.numDsps) ;

    if (DSP_SUCCEEDED (status)) {
        dspCfg  = LDRV_LinkCfgPtr->dspConfigs [procId] ;
        dspObj  = dspCfg->dspObject ;
        linkDrv = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]);

        LDRV_MSGQ_StateObj.doDspCtrl [procId] = dspObj->doDspCtrl ;

        /*  ----------------------------------------------------------------
         *  Get the kernel configuration information for the MQT.
         *  ----------------------------------------------------------------
         */
        for (j = 0 ;
             (j < CFGMAP_Config [procId]->numMqts) && DSP_SUCCEEDED (status) ;
             j++) {
            status = GEN_Strcmp (
                      dspCfg->mqtObjects [linkDrv->mqtId].name,
                      CFGMAP_Config [procId]->mqtObjects [j].name,
                      &strCmpResult) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            if (strCmpResult == 0) {
                mqtInterface = CFGMAP_Config [procId]->mqtObjects [j].interface;
                DBC_Assert (mqtInterface != NULL) ;
                break ;
            }
        }

        if (j == CFGMAP_Config [procId]->numMqts) {
            /* Configured MQT is not available. */
            PRINT_Printf ("Configuration error:"
                    " Incorrect MQT name specified [%s]\n",
                    dspCfg->mqtObjects [linkDrv->mqtId].name) ;
            status = DSP_ECONFIG ;
            SET_FAILURE_REASON ;
        }
        else if (DSP_SUCCEEDED (status)) {
            mqtHandle = &(LDRV_MSGQ_StateObj.transports [procId]) ;
            mqtHandle->dspId        = procId ;
            mqtHandle->object       = NULL ;
            mqtHandle->mqtInterface = mqtInterface ;
        }
        else {
            /* Added for MISRAC compliance */
        }
    }

    if (DSP_SUCCEEDED (status)) {
    mqtHandle    = &(LDRV_MSGQ_StateObj.transports [procId]) ;
    DBC_Assert (mqtHandle != NULL) ;

    if (mqtHandle->mqtInterface != NULL) {
        mqtInterface = mqtHandle->mqtInterface ;
        /* Call the open function for the corresponding transport. */
        status = mqtInterface->open (mqtHandle, attrs) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_MSGQ_transportOpen", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_transportClose (IN  ProcessorId procId)
{
    DSP_STATUS                 status    = DSP_SOK ;
    MQT_Interface *            mqtInterface ;
    LDRV_MSGQ_TransportHandle  mqtHandle ;

    TRC_1ENTER ("LDRV_MSGQ_transportClose", procId) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (procId < LDRV_MSGQ_StateObj.numDsps) ;

    if (LDRV_MSGQ_StateObj.transports != NULL) {
        mqtHandle = &(LDRV_MSGQ_StateObj.transports [procId]) ;
        if (mqtHandle != NULL) {
            if (mqtHandle->mqtInterface != NULL) {
                if (mqtHandle->mqtInterface != NULL) {
                    mqtInterface = mqtHandle->mqtInterface ;
                    /* Call the close function for the corresponding transport. */
                    status = mqtInterface->close (mqtHandle) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LDRV_MSGQ_transportClose", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_open
 *
 *  @desc   This function opens the message queue to be used for receiving
 *          messages, identified through the specified message queue name.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_open (IN     Pstr         queueName,
                OUT    MSGQ_Queue * msgqQueue,
                IN     MSGQ_Attrs * attrs)
{
    DSP_STATUS        status       = DSP_SOK ;
    LDRV_MSGQ_Handle  msgqHandle   = NULL ;
    Bool              found        = FALSE ;
    MSGQ_Id           msgqId       = (MSGQ_Id) MSGQ_INVALIDMSGQ ;
    Uint32            irqFlags ;
    SyncAttrs         attr ;
    MSGQ_Attrs        msgqAttrs ;
    Uint16            i ;
    Int32             cmpResult ;

    TRC_3ENTER ("LDRV_MSGQ_open", queueName, msgqQueue, attrs) ;

    DBC_Require (msgqQueue != NULL) ;
    
    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;

    if (attrs == NULL) {
        msgqAttrs.notifyHandle = NULL ;
        msgqAttrs.pend         = NULL ;
        msgqAttrs.post         = NULL ;
        attrs = &msgqAttrs ;
    }

    /*  Search the local message queue array to find a free slot in it. */
    for (i = 0 ;
         (   (i < LDRV_MSGQ_StateObj.maxMsgqs)
          && (DSP_SUCCEEDED(status))) ;
         i++) {
        irqFlags = SYNC_SpinLockStartEx (LDRV_MSGQ_StateObj.lock) ;

        if ( (queueName!=NULL) &&  (LDRV_MSGQ_StateObj.msgqHandles [i] != NULL)
            && (LDRV_MSGQ_StateObj.msgqHandles [i] !=
               (LDRV_MSGQ_Handle) (TRUE))) {
            status = GEN_Strcmp (queueName,
                    LDRV_MSGQ_StateObj.msgqHandles [i]->name,
                    &cmpResult) ;
            if (DSP_SUCCEEDED (status)) {
                if (cmpResult == 0) {
                    status = DSP_EALREADYEXISTS ;
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (   (LDRV_MSGQ_StateObj.msgqHandles [i] == NULL)
            && (msgqId == (MSGQ_Id) MSGQ_INVALIDMSGQ)
            && (DSP_SUCCEEDED(status))) {
            /* Block the slot obtained. */
            LDRV_MSGQ_StateObj.msgqHandles [i] = (LDRV_MSGQ_Handle) TRUE ;
            msgqId = i ;
            found = TRUE ;
        }
        SYNC_SpinLockEndEx (LDRV_MSGQ_StateObj.lock, irqFlags) ;
    }


    /* Existing matching name was not found, but all slots are full. */
    if (DSP_SUCCEEDED(status) && (found == FALSE)) {
        status = DSP_ENOTFOUND ;
        SET_FAILURE_REASON ;
    }

    /*   Create and initialize the MSGQ object.  */
    if (DSP_SUCCEEDED(status)) {
            status = MEM_Calloc ((Void **) &msgqHandle,
                                 (sizeof (LDRV_MSGQ_Object)),
                                 MEM_DEFAULT) ;
            if (DSP_SUCCEEDED (status)) {
                msgqHandle->msgqQueue = (msgqId) | ((Uint32) ID_GPP << 16) ;
                if (queueName != NULL) {
                    status = GEN_Strcpyn ((Char8 *) msgqHandle->name,
                                          (Char8 *) queueName,
                                          DSP_MAX_STRLEN) ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                /*  Create the list for the local queue.  */
                status = LIST_Create (&(msgqHandle->queue)) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Create a semaphore if a notifyHandle is not provided by
                     * the user.
                     */
                    if (attrs->notifyHandle == NULL) {
                        attr.flag = SyncSemType_Binary ;
                        msgqHandle->defaultNtfyHandle = TRUE ;
                        status = SYNC_CreateSEM (
                             (SyncSemObject **) ((Pvoid) &(msgqHandle->ntfyHandle)),
                             &attr) ;
                        if (DSP_SUCCEEDED (status)) {
                                msgqHandle->pend =
                                    (MsgqPend) ((Uint32) &SYNC_WaitSEM) ;
                                msgqHandle->post =
                                    (MsgqPost) ((Uint32) &SYNC_SignalSEM) ;

                            /* Return the MSGQ_Queue handle */
                            *msgqQueue = msgqHandle->msgqQueue ;
                        }
                        else {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        msgqHandle->defaultNtfyHandle = FALSE ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                LDRV_MSGQ_StateObj.msgqHandles [msgqId] = msgqHandle ;
        }
    }

    /*  Cleanup on failure. */
    if (DSP_FAILED (status)) {
        if (msgqHandle != NULL) {
            LDRV_MSGQ_close (msgqHandle->msgqQueue) ;
        }
        *msgqQueue = (MSGQ_Queue) MSGQ_INVALIDMSGQ ;
    }

    DBC_Ensure (   (   (*msgqQueue != (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_SUCCEEDED (status)))
                || (   (*msgqQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_close (IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS        status      = DSP_SOK ;
    DSP_STATUS        tmpStatus   = DSP_SOK ;
    LDRV_MSGQ_Handle  msgqHandle ;
    MSGQ_Msg          msg ;
    MSGQ_Id           msgqId ;

    TRC_1ENTER ("LDRV_MSGQ_close", msgqQueue) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    DBC_Assert (LDRV_MSGQ_StateObj.msgqHandles != NULL) ;
    msgqId = (MSGQ_Id) msgqQueue ;
    if (LDRV_MSGQ_StateObj.msgqHandles != NULL) {
        msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [msgqId] ;
        if (msgqHandle != NULL) {
            /* First clear the msgqHandle in the array. */
            LDRV_MSGQ_StateObj.msgqHandles [msgqId] = NULL ;

            /*  Delete the semaphore if allocated internally. */
            if (msgqHandle->defaultNtfyHandle == TRUE) {
                if (msgqHandle->ntfyHandle != NULL) {
                    status = SYNC_DeleteSEM ((SyncSemObject *)
                                             msgqHandle->ntfyHandle) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }

            /*  Delete the message list. */
            if (msgqHandle->queue != NULL) {
                /* Clear out any existing messages. */
                SYNC_ProtectionStart () ;
                while (   (LIST_IsEmpty (msgqHandle->queue) != TRUE)
                       && (DSP_SUCCEEDED (tmpStatus))) {
                    tmpStatus = LIST_GetHead (msgqHandle->queue,
                                              (ListElement **) ((Pvoid) &msg)) ;
                    if ((DSP_SUCCEEDED (tmpStatus)) && (msg != NULL)) {
                        tmpStatus = LDRV_MSGQ_free (msg) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }
                    else if (DSP_SUCCEEDED (status) && (DSP_FAILED (tmpStatus))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* Added for MISRAC compliance */
                    }
                }
                SYNC_ProtectionEnd () ;

                tmpStatus = LIST_Delete (msgqHandle->queue) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }

            tmpStatus = FREE_PTR (msgqHandle) ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MSGQ_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_locate
 *
 *  @desc   This function synchronously locates the message queue identified by
 *          the specified MSGQ name and returns a handle to the located message
 *          queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locate (IN     Pstr              queueName,
                  OUT    MSGQ_Queue *       msgqQueue,
                  IN     MSGQ_LocateAttrs * attrs)
{
    DSP_STATUS                 status    = DSP_SOK ;
    DSP_STATUS                 tmpStatus = DSP_SOK ;
    LDRV_MSGQ_TransportHandle  mqtHandle ;
    MSGQ_LocateAttrs           locateAttrs ;
    Uint16                     i ;

    TRC_3ENTER ("LDRV_MSGQ_locate", queueName, msgqQueue, attrs) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (queueName != NULL) ;
    DBC_Require (msgqQueue != NULL) ;

    /*  Search the local message queues first. */
    status = LDRV_MSGQ_locateLocal (queueName, msgqQueue) ;
    /*  If message queue does not exist locally, status will not be DSP_SOK.
     *  In that case, pass on the locate call to all remote transports till the
     *  message queue is found.
     */

    if (attrs == NULL) {
        locateAttrs.timeout = WAIT_FOREVER ; /* Default timeout */
        attrs = &locateAttrs ;
    }

    for (i = 0 ;
         (status == DSP_ENOTFOUND) && (i < LDRV_MSGQ_StateObj.numDsps) ;
         i++) {
        mqtHandle = &(LDRV_MSGQ_StateObj.transports [i]) ;

        /*  Since the locate is sync, the transport will either:
         *  - Return DSP_SOK and fill in msgqQueue if located.
         *    In this case, return success and the msgqQueue
         *  - Return DSP_ENOTFOUND if not located.
         *    In this case, continue searching
         *  - Return error code due to occurrence of some other error.
         *    In this case, exit with error.
         */
        tmpStatus = LDRV_PROC_isStarted (i) ;

        /* Check if DSP has been started */
        if (DSP_SUCCEEDED (tmpStatus)) {
            /* Check if MSGQ_transportOpen has been called for
             * specified MQT.
             */
            if (mqtHandle->object != NULL) {
                status = mqtHandle->mqtInterface->locate (mqtHandle,
                                                          queueName,
                                                          TRUE,
                                                          msgqQueue,
                                                          (Pvoid) attrs) ;
            }
        }

        if (DSP_FAILED (status) && (status != DSP_ENOTFOUND)) {
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (   (   (*msgqQueue != (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_SUCCEEDED (status)))
                || (   (*msgqQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_locate", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_locateAsync
 *
 *  @desc   This function asynchronously locates the message queue identified by
 *          the specified MSGQ name.
 *          An attempt is made to asynchronously locate the message queue. If
 *          found, an MSGQ_AsyncLocateMsg message is sent to the specified reply
 *          message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locateAsync (IN Pstr                    queueName,
                       IN MSGQ_Queue               replyQueue,
                       IN MSGQ_LocateAsyncAttrs *  attrs)
{
    DSP_STATUS                status      = DSP_SOK ;
    DSP_STATUS                tmpStatus   = DSP_SOK ;
    MSGQ_AsyncLocateMsg *     msg ;
    MSGQ_Queue                locatedQueue;
    LDRV_MSGQ_TransportHandle mqtHandle ;
    Uint16                    i ;

    TRC_3ENTER ("LDRV_MSGQ_locateAsync", queueName, replyQueue, attrs) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (queueName  != NULL) ;
    DBC_Require (IS_VALID_MSGQ (replyQueue)) ;
    DBC_Require (attrs      != NULL) ;

    /*  Search the local message queues first. */
    status = LDRV_MSGQ_locateLocal (queueName, &locatedQueue) ;
    if (DSP_SUCCEEDED (status)) {
        /*  If the message queue exists locally, allocate an async locate
         *  message and send to the specified reply Queue.
         */
        status = LDRV_MSGQ_alloc (attrs->poolId,
                                  sizeof (MSGQ_AsyncLocateMsg),
                                  (MSGQ_Msg *) ((Pvoid) &msg)) ;

        if (DSP_SUCCEEDED (status)) {
            /* Fill in the fields of the message. */
            msg->msgqQueue = locatedQueue ;
            msg->arg       = attrs->arg ;
            MSGQ_setMsgId ((MSGQ_Msg) msg, MSGQ_ASYNCLOCATEMSGID) ;

            status = LDRV_MSGQ_put (replyQueue, (MSGQ_Msg) msg) ;
            if (DSP_FAILED (status)) {
                LDRV_MSGQ_free ((MSGQ_Msg) msg) ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_SOK ; /* Start afresh. */

        /*  If message queue does not exist locally, status will not be DSP_SOK.
         *  In that case, pass on the locate call to all remote transports till
         *  the message queue is found.
         */
        for (i = 0 ; i < LDRV_MSGQ_StateObj.numDsps ; i++) {
            mqtHandle = &(LDRV_MSGQ_StateObj.transports [i]) ;

            /*  Since the locate is async, a failure code from the transport
             *  indicates that async locate could not be initiated.
             *  In either case, continue searching for the queue in other
             *  transports.
             *  Also, check if MSGQ_transportOpen has been called for
             *  specified MQT.
             */
            if (mqtHandle->object != NULL) {
                tmpStatus = mqtHandle->mqtInterface->locate (mqtHandle,
                                                             queueName,
                                                             FALSE,
                                                             &replyQueue,
                                                             (Pvoid) attrs) ;
                if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("LDRV_MSGQ_locateAsync", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_release
 *
 *  @desc   This function releases the message queue identified by the MSGQ
 *          handle that was located earlier.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_release (IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS                status  = DSP_SOK ;
    LDRV_MSGQ_TransportHandle mqtHandle ;
    Uint16                    procId ;

    TRC_1ENTER ("LDRV_MSGQ_release", msgqQueue) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    procId = msgqQueue >> 16 ;

    /*  Nothing needs to be done for local message queue release.
     *  For remote message queue, call the release function of the corresponding
     *  transport.
     */
    if (!IS_GPPID (procId)) {
        mqtHandle = &(LDRV_MSGQ_StateObj.transports [procId]) ;
        status = mqtHandle->mqtInterface->release (mqtHandle, msgqQueue) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LDRV_MSGQ_release", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_alloc
 *
 *  @desc   This function allocates a message, and returns the pointer to the
 *          user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("LDRV_MSGQ_alloc", poolId, size, msg) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (msg != NULL) ;
    DBC_Require (size >= sizeof (MSGQ_MsgHeader)) ;

    status = LDRV_POOL_alloc (poolId, (Pvoid *) msg, size) ;
    if (DSP_SUCCEEDED (status)) {
        /*  Fill in the MSGQ_MsgHeader fields. */
        (*msg)->size        = size ;
        (*msg)->poolId      = poolId ;
        (*msg)->srcId       = MSGQ_INVALIDMSGQ ;
        (*msg)->msgId       = MSGQ_INVALIDMSGID ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   ((*msg != NULL) && (DSP_SUCCEEDED (status)))
                || ((*msg == NULL) && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_free
 *
 *  @desc   This function frees a message.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_free (IN  MSGQ_Msg msg)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LDRV_MSGQ_free", msg) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (msg != NULL) ;

    status = LDRV_POOL_free (msg->poolId, msg, msg->size) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_MSGQ_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg)
{
    DSP_STATUS                status  = DSP_SOK ;
    LDRV_MSGQ_TransportHandle mqtHandle ;
    LDRV_MSGQ_Handle          msgqHandle ;
    Uint16                    procId ;
    Uint32                    irqFlags ;

    TRC_2ENTER ("LDRV_MSGQ_put", msgqQueue, msg) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    msg->dstId  = (MSGQ_Id) msgqQueue ;
    procId      = msgqQueue >> 16 ;

    if (IS_GPPID (procId)) {
        irqFlags = SYNC_SpinLockStartEx (LDRV_MSGQ_StateObj.lock) ;
        /* Place the message on the message list */
        msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [(MSGQ_Id) msgqQueue] ;

        /* Check if the message queue exists to ensure that an asynchronous
         * late message arrival from the DSP does not crash the GPP.
         */
        if ((msgqHandle != NULL) && (msgqHandle != (LDRV_MSGQ_Handle) TRUE)) {
            status = LIST_PutTail (msgqHandle->queue, (ListElement *) msg) ;
            SYNC_SpinLockEndEx (LDRV_MSGQ_StateObj.lock, irqFlags) ;
            if (DSP_SUCCEEDED (status)) {
                /* Notify the reader. */
                status = msgqHandle->post (msgqHandle->ntfyHandle) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

#if defined (DDSP_PROFILE)
            if (DSP_SUCCEEDED (status)) {
                msgqHandle->msgqStats.msgqData.queued++ ;
            }
#endif /* defined (DDSP_PROFILE) */
        }
        else {
            SYNC_SpinLockEndEx (LDRV_MSGQ_StateObj.lock, irqFlags) ;
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        if (LDRV_MSGQ_StateObj.doDspCtrl [procId] == DSP_BootMode_Boot_NoPwr) {
            status = LDRV_PROC_isStarted (procId) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }

        if (DSP_SUCCEEDED (status)) {
            /* Call the transport associated with this message queue */
            mqtHandle = &(LDRV_MSGQ_StateObj.transports [procId]) ;
            status = mqtHandle->mqtInterface->put (mqtHandle, msg) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_MSGQ_put", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_get
 *
 *  @desc   This function receives a message on the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg)
{
    DSP_STATUS        status  = DSP_SOK ;
    LDRV_MSGQ_Handle  msgqHandle ;
    Uint32            irqFlags ;

    TRC_3ENTER ("LDRV_MSGQ_get", msgqQueue, timeout, msg) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [(MSGQ_Id) msgqQueue] ;
    DBC_Assert (msgqHandle != NULL) ;

    *msg = NULL ;

    while ((DSP_SUCCEEDED (status)) && (*msg == NULL)) {
        /*  --------------------------------------------------------------------
         *  Get the first element of the queue in msg. If the queue is empty,
         *  NULL will be returned which is not an error condition.
         *  --------------------------------------------------------------------
         */
        irqFlags = SYNC_SpinLockStartEx (LDRV_MSGQ_StateObj.lock) ;
        status = LIST_GetHead (msgqHandle->queue,
                               (ListElement **) ((Pvoid) msg)) ;
        SYNC_SpinLockEndEx (LDRV_MSGQ_StateObj.lock, irqFlags) ;

        /*  --------------------------------------------------------------------
         *  Wait on the semaphore till the time a message comes in this local
         *  queue. This semaphore will be posted by MSGQ_put.
         *  If the semaphore was already posted, below call returns immediately.
         *  However, the list could still be empty, because the message might
         *  have been taken away by a previous call. In this case, try to get
         *  the message once more (while loop), and wait once more if a message
         *  was not available.
         *  --------------------------------------------------------------------
         */
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else if (*msg == NULL) {
            status = msgqHandle->pend (msgqHandle->ntfyHandle, timeout) ;
            if ((timeout == SYNC_NOWAIT) && (status == SYNC_E_FAIL)) {
                status = DSP_ENOTCOMPLETE ;
                SET_FAILURE_REASON ;
            }
            else if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                /* Added for MISRAC compliance */
            }
        }
        else {
            /* Added for MISRAC compliance */
        }
    }

#if defined (DDSP_PROFILE)
    if (DSP_SUCCEEDED (status)) {
        msgqHandle->msgqStats.msgqData.transferred++ ;
    }
#endif /* defined (DDSP_PROFILE) */

    DBC_Ensure (   ((*msg != NULL) && (DSP_SUCCEEDED (status)))
                || ((*msg == NULL) && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_get", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_count
 *
 *  @desc   This API returns the count of the number of messages in a local
 *          message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count)
{
    DSP_STATUS        status  = DSP_SOK ;
    LDRV_MSGQ_Handle  msgqHandle ;
    MSGQ_Msg          msg ;
    MSGQ_Msg          nextMsg ;

    TRC_2ENTER ("LDRV_MSGQ_count", msgqQueue, count) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (count != NULL) ;

    msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [(MSGQ_Id) msgqQueue] ;
    DBC_Assert (msgqHandle != NULL) ;

    *count = 0 ;

    status = LIST_First (msgqHandle->queue,
                         (ListElement **) ((Pvoid) &msg)) ;
    /* No check needed since parameters are assured non-NULL */
    DBC_Assert (DSP_SUCCEEDED (status)) ;

    SYNC_ProtectionStart () ;

    while ((msg != NULL) && (DSP_SUCCEEDED (status))) {
        (*count)++ ;
        status = LIST_Next (msgqHandle->queue,
                            (ListElement *)  ((Pvoid) msg),
                            (ListElement **) ((Pvoid) &nextMsg)) ;
        if (DSP_SUCCEEDED (status)) {
            msg = nextMsg ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    SYNC_ProtectionEnd () ;

    TRC_1PRINT (TRC_LEVEL4, "MSGQ count: [%d]", *count) ;

    TRC_1LEAVE ("LDRV_MSGQ_count", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_setErrorHandler
 *
 *  @desc   This API allows the user to designate a MSGQ as an error-handler
 *          MSGQ to receive asynchronous error messages from the transports.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId)
{
    DSP_STATUS  status  = DSP_SOK ;
    Uint32      irqFlags ;

    TRC_2ENTER ("LDRV_MSGQ_setErrorHandler", errorQueue, poolId) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (   IS_VALID_MSGQ (errorQueue)
                 || (errorQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)) ;

    /* Protection to ensure that this code does not conflict with
     * LDRV_MSGQ_sendErrorMsg ().
     */
    irqFlags = SYNC_SpinLockStartEx (LDRV_MSGQ_StateObj.lock) ;
    LDRV_MSGQ_StateObj.errorQueue     = errorQueue ;
    LDRV_MSGQ_StateObj.errorPoolId    = poolId ;
    SYNC_SpinLockEndEx (LDRV_MSGQ_StateObj.lock, irqFlags) ;

    TRC_1LEAVE ("LDRV_MSGQ_setErrorHandler", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_sendErrorMsg
 *
 *  @desc   This function sends an asynchronous error message of a particular
 *          type to the user-defined error handler MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_sendErrorMsg (IN  MSGQ_MqtError  errorType,
                        IN  Pvoid          arg1,
                        IN  Pvoid          arg2)
{
    DSP_STATUS           status      = DSP_SOK ;
    MSGQ_AsyncErrorMsg * errorMsg    = NULL ;

    TRC_3ENTER ("LDRV_MSGQ_sendErrorMsg", errorType, arg1, arg2) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (arg1 != NULL) ;
    DBC_Require (arg2 != NULL) ;

    /* If the error handler has not been set-up, just return to the caller. */
    if (LDRV_MSGQ_StateObj.errorQueue != (MSGQ_Queue) MSGQ_INVALIDMSGQ) {
        status = LDRV_MSGQ_alloc (LDRV_MSGQ_StateObj.errorPoolId,
                                  sizeof (MSGQ_AsyncErrorMsg),
                                  (MSGQ_Msg *) ((Pvoid) &errorMsg));
        if (DSP_SUCCEEDED (status)) {
            /* Fill in the fields of the message. */
            errorMsg->errorType = errorType ;
            errorMsg->arg1      = arg1 ;
            errorMsg->arg2      = arg2 ;

            MSGQ_setMsgId ((MSGQ_Msg) errorMsg, MSGQ_ASYNCERRORMSGID) ;

            /* Send the message to the user specified error handler. */
            status = LDRV_MSGQ_put (LDRV_MSGQ_StateObj.errorQueue,
                                    (MSGQ_Msg) errorMsg) ;
            /* If put fails, free the message. */
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
                LDRV_MSGQ_free ((MSGQ_Msg) errorMsg) ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  Not setting error in this case as its a valid condition that user does
     *  not want to receive error notifications.
     *  ------------------------------------------------------------------------
     */

    TRC_1LEAVE ("LDRV_MSGQ_sendErrorMsg", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_locateLocal
 *
 *  @desc   This function locates a local message queue identified by
 *          the specified MSGQ name and returns a handle to the located message
 *          queue if found.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locateLocal (IN Pstr queueName, OUT MSGQ_Queue * msgqQueue)
{
    DSP_STATUS        status  = DSP_SOK ;
    LDRV_MSGQ_Handle  msgqHandle ;
    Int32             cmpResult ;
    Uint16            i ;

    TRC_2ENTER ("LDRV_MSGQ_locateLocal", queueName, msgqQueue) ;

    DBC_Require (queueName != NULL) ;
    DBC_Require (msgqQueue != NULL) ;

    *msgqQueue = (MSGQ_Queue) MSGQ_INVALIDMSGQ ;
    /* Loop through the local message queues. */
    for (i = 0 ;
            (i < LDRV_MSGQ_StateObj.maxMsgqs)
         && (*msgqQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)
         ; i++) {
        /* Check the message queue if it is opened and the name matches. */
        msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [i] ;
        if ((msgqHandle != NULL) && (msgqHandle != (LDRV_MSGQ_Handle) (TRUE))) {
            status = GEN_Strcmp ((Char8 *) msgqHandle->name,
                                 (Char8 *) queueName,
                                 &cmpResult) ;
            if (DSP_SUCCEEDED (status)) {
                /* Found a match */
                if (cmpResult == 0) {
                    *msgqQueue = msgqHandle->msgqQueue ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (    (*msgqQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)
        &&  DSP_SUCCEEDED (status)) {
        status = DSP_ENOTFOUND ; /* Not an error value */
    }

    DBC_Ensure (   (   (*msgqQueue != (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_SUCCEEDED (status)))
                || (   (*msgqQueue == (MSGQ_Queue) MSGQ_INVALIDMSGQ)
                    && (DSP_FAILED (status)))) ;

    TRC_1LEAVE ("LDRV_MSGQ_locateLocal", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_MSGQ_NotImpl
 *
 *  @desc   This function should be used in interface tables where some
 *          functions are not being implemented.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_NotImpl (Void)
{
    return DSP_ENOTIMPL ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_MSGQ_instrument
 *
 *  @desc   This function gets the instrumentation information related to the
 *          specified message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal)
{
    DSP_STATUS       status = DSP_SOK ;
    LDRV_MSGQ_Handle msgqHandle ;

    TRC_2ENTER ("LDRV_MSGQ_instrument", msgqQueue, retVal) ;

    DBC_Require (LDRV_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (retVal    != NULL) ;

    if (LDRV_MSGQ_IsInitialized == TRUE) {
        msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [(MSGQ_Id) msgqQueue] ;
        *retVal = msgqHandle->msgqStats.msgqData ;
    }
    else {
        TRC_0PRINT (TRC_LEVEL4, "MSGQ component is not yet initialized.\n") ;
    }

    TRC_1LEAVE ("LDRV_MSGQ_instrument", status) ;

    return status ;
}
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_MSGQ_debug
 *
 *  @desc   This function prints the current status of the MSGQ subcomponent.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MSGQ_debug (IN MSGQ_Queue msgqQueue)
{
    LDRV_MSGQ_TransportHandle mqtHandle ;
    Uint16                    procId ;
#if defined (TRACE_ENABLE)
    LDRV_MSGQ_Handle          msgqHandle ;
#endif /* if defined (TRACE_ENABLE) */

    TRC_1ENTER ("LDRV_MSGQ_debug", msgqQueue) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if (LDRV_MSGQ_IsInitialized == TRUE) {
        /*  --------------------------------------------------------------------
         *  Print the LDRV MSGQ component state information.
         *  --------------------------------------------------------------------
         */
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;
        TRC_0PRINT (TRC_LEVEL4, "MSGQ component state information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "msgqHandles    = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.msgqHandles) ;
        TRC_1PRINT (TRC_LEVEL4, "maxMsgqs       = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.maxMsgqs) ;
        TRC_1PRINT (TRC_LEVEL4, "numDsps        = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.numDsps) ;
        TRC_1PRINT (TRC_LEVEL4, "transports     = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.transports) ;
        TRC_1PRINT (TRC_LEVEL4, "errorQueue     = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.errorQueue) ;
        TRC_1PRINT (TRC_LEVEL4, "errorPoolId    = 0x[%x].\n",
                                LDRV_MSGQ_StateObj.errorPoolId) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  Print the Message Queue information
         *  --------------------------------------------------------------------
         */
#if defined (TRACE_ENABLE)
        msgqHandle = LDRV_MSGQ_StateObj.msgqHandles [(MSGQ_Id) msgqQueue] ;
#endif /* if defined (TRACE_ENABLE) */
        TRC_0PRINT (TRC_LEVEL4, "Message queue information:\n") ;
        TRC_1PRINT (TRC_LEVEL4, "name           = [%s]\n", &msgqHandle->name) ;
        TRC_1PRINT (TRC_LEVEL4, "msgqQueue      = [0x%x]\n",
                                &msgqHandle->msgqQueue) ;
        TRC_1PRINT (TRC_LEVEL4, "queue          = [0x%x]\n",
                                &msgqHandle->queue) ;
        TRC_1PRINT (TRC_LEVEL4, "ntfyHandle     = [0x%x]\n",
                                &msgqHandle->ntfyHandle) ;
        TRC_0PRINT (TRC_LEVEL4, "========================================.\n") ;

        /*  --------------------------------------------------------------------
         *  If the message queue is not local, extract the handle of the
         *  transport and call the debug function of the transport.
         *  --------------------------------------------------------------------
         */
        procId = msgqQueue >> 16 ;
        if (!IS_GPPID (procId)) {
            mqtHandle = &(LDRV_MSGQ_StateObj.transports [procId]) ;
            mqtHandle->mqtInterface->debug (mqtHandle) ;
        }
    }
    else {
        TRC_0PRINT (TRC_LEVEL4, "MSGQ component not yet initialized.\n") ;
    }

    TRC_0LEAVE ("LDRV_MSGQ_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
