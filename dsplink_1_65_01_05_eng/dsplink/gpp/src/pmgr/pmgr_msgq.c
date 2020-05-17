/** ============================================================================
 *  @file   pmgr_msgq.c
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   This file implements the functions of PMGR_MSGQ subcomponent.
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
#include <msgqdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <osal.h>

/*  ----------------------------------- Profiling                     */
#include <profile.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Processor Manager             */
#include <pmgr_proc.h>
#include <pmgr_msgq.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_pool.h>
#include <ldrv_mqt.h>
#include <ldrv_msgq.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_PMGR_MSGQ

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason (defined in GEN).
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_PMGR_MSGQ, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @name   PMGR_MSGQ_Object
 *
 *  @desc   This structure defines the PMGR_MSGQ object, which contains all
 *          state information required by the PMGR_MSGQ component.
 *
 *  @field  maxMsgqs
 *              Maximum number of message queues on the GPP.
 *  @field  msgqOwner
 *              Array of MSGQ owners.
 *  @field  refCount
 *              Reference count for transport open.
 *  ----------------------------------------------------------------------------
 */
typedef struct PMGR_MSGQ_Object_tag {
    Uint32          maxMsgqs ;
    PrcsObject **   msgqOwner ;
    Uint32          refCount  [MAX_DSPS] ;
} PMGR_MSGQ_Object ;


/*  ----------------------------------------------------------------------------
 *  @name   PMGR_MSGQ_State
 *
 *  @desc   State object for the PMGR_MSGQ component containing all information
 *          required by it.
 *  ----------------------------------------------------------------------------
 */
STATIC PMGR_MSGQ_Object PMGR_MSGQ_State ;

/** ============================================================================
 *  @name   PMGR_MSGQ_IsInitialized
 *
 *  @desc   Flag to keep track of initialization of this subcomponent.
 *  ============================================================================
 */
STATIC Bool PMGR_MSGQ_IsInitialized = FALSE ;

/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;


/** ----------------------------------------------------------------------------
 *  @func   PMGR_MSGQ_isOwner
 *
 *  @desc   Checks if the calling client is the owner.
 *
 *  @arg    prcsInfo
 *              Pointer to the PRCS object with which the PRCS information of
 *              the current client is to be compared.
 *  @arg    optArgs
 *              Optional arguments
 *
 *
 *  @ret    TRUE
 *              If the calling client has created the MSGQ.
 *          FALSE
 *              Otherwise.
 *
 *  @enter  msgqId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
PMGR_MSGQ_isOwner (IN PrcsObject * prcsInfo, IN     Void * optArgs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_setup
 *
 *  @desc   This function initializes the MSGQ component.
 *
 *  @modif  PMGR_MSGQ_IsInitialized, PMGR_MSGQ_State
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_setup (Void)
{
    DSP_STATUS    status  = DSP_SOK ;
    LINKCFG_Gpp * gppObj ;
    Uint16        i ;

    TRC_0ENTER ("PMGR_MSGQ_setup") ;

    DBC_Require (PMGR_MSGQ_IsInitialized == FALSE) ;

    gppObj = LDRV_LinkCfgPtr->gppObject ;

    /* Initialize the state object.*/
    PMGR_MSGQ_State.maxMsgqs = gppObj->maxMsgqs ;

    for (i = 0 ; i < MAX_DSPS ; i++) {
        PMGR_MSGQ_State.refCount [i] = 0 ;
    }

    status = MEM_Calloc ((Void **) &(PMGR_MSGQ_State.msgqOwner),
                         (sizeof (PrcsObject *) * PMGR_MSGQ_State.maxMsgqs),
                         MEM_DEFAULT) ;
    if (DSP_SUCCEEDED (status)) {
        status = LDRV_MSGQ_setup () ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            FREE_PTR (PMGR_MSGQ_State.msgqOwner) ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        PMGR_MSGQ_IsInitialized = TRUE ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_setup", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_destroy
 *
 *  @desc   This function finalizes the MSGQ component.
 *
 *  @modif  PMGR_MSGQ_IsInitialized, PMGR_MSGQ_State
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_destroy (Void)
{
    DSP_STATUS  status    = DSP_SOK ;
    DSP_STATUS  tmpStatus = DSP_SOK ;
    Uint16      i         = 0 ;

    TRC_0ENTER ("PMGR_MSGQ_destroy") ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;

    if (PMGR_MSGQ_IsInitialized == TRUE) {
        PMGR_MSGQ_IsInitialized = FALSE ;

        if (PMGR_MSGQ_State.msgqOwner != NULL) {
            for (i = 0 ; i < PMGR_MSGQ_State.maxMsgqs ; i++) {
                if (PMGR_MSGQ_State.msgqOwner [i] != NULL) {
                    tmpStatus = PRCS_Delete (PMGR_MSGQ_State.msgqOwner [i]) ;
                    PMGR_MSGQ_State.msgqOwner [i] = NULL ;
                    if ((DSP_FAILED (tmpStatus)) && (DSP_SUCCEEDED (status))) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
            tmpStatus = FREE_PTR (PMGR_MSGQ_State.msgqOwner) ;
            if ((DSP_FAILED (tmpStatus)) && (DSP_SUCCEEDED (status))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }

        for (i = 0 ; i < MAX_DSPS ; i++) {
            PMGR_MSGQ_State.refCount [i] = 0 ;
        }

        tmpStatus = LDRV_MSGQ_destroy () ;
        if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_destroy", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_transportOpen
 *
 *  @desc   This function initializes the transport associated with the
 *          specified processor.
 *          Note: The Processor ID here refers to a DSP, since transports are
 *                only associated with DSPs. The IDs for the DSPs start from 0
 *                onwards.
 *
 *  @modif  PMGR_MSGQ_State.refCount [procId]
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs)
{
    DSP_STATUS   status   = DSP_SOK ;
    Bool         toOpen   = FALSE ;

    TRC_2ENTER ("PMGR_MSGQ_transportOpen", procId, attrs) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (attrs != NULL) ;

    SYNC_ProtectionStart () ;
    if (PMGR_MSGQ_State.refCount [procId] == 0) {
        toOpen = TRUE ;
        status = DSP_SALREADYOPENED ;
    }
    PMGR_MSGQ_State.refCount [procId]++ ;
    SYNC_ProtectionEnd () ;

    if (toOpen == TRUE) {
        status = LDRV_MSGQ_transportOpen (procId, attrs) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            PMGR_MSGQ_State.refCount [procId]-- ;
        }
    }

    TRC_1LEAVE ("PMGR_MSGQ_transportOpen", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @modif  PMGR_MSGQ_State.refCount [procId]
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_transportClose (IN  ProcessorId procId)
{
    DSP_STATUS  status    = DSP_SOK ;
    DSP_STATUS  tmpStatus = DSP_SOK ;
    Bool        toClose   = FALSE ;

    TRC_1ENTER ("PMGR_MSGQ_transportClose", procId) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;

    SYNC_ProtectionStart () ;
    if (PMGR_MSGQ_State.refCount [procId] == 0) {
        status = DSP_EOPENED ;
        SET_FAILURE_REASON ;
    }
    else {
        PMGR_MSGQ_State.refCount [procId]-- ;
        if (PMGR_MSGQ_State.refCount [procId] == 0) {
            toClose = TRUE ;
            status = DSP_SCLOSED ;
        }
    }
    SYNC_ProtectionEnd () ;

    if (DSP_SUCCEEDED (status)) {
        if (toClose == TRUE) {
            tmpStatus = LDRV_MSGQ_transportClose (procId) ;
            if ((DSP_FAILED (tmpStatus)) && (DSP_SUCCEEDED (status))) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("PMGR_MSGQ_transportClose", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_open
 *
 *  @desc   This function opens the message queue to be used for receiving
 *          messages, identified through the specified message queue name.
 *
 *  @modif  PMGR_MSGQ_State.msgqOwner
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_open (IN     Pstr        queueName,
                OUT    MSGQ_Queue * msgqQueue,
                IN     MSGQ_Attrs * attrs,
                IN     Void *       optArgs)
{
    DSP_STATUS   status     = DSP_SOK ;
    PrcsObject * prcsInfo   = NULL    ;
    MSGQ_Id       msgqId ;

    TRC_3ENTER ("PMGR_MSGQ_open", queueName, msgqQueue, attrs) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (msgqQueue != NULL) ;

    status = LDRV_MSGQ_open (queueName, msgqQueue, attrs) ;
    if (DSP_SUCCEEDED (status)) {
        msgqId = (MSGQ_Id) *msgqQueue ;
        if (PMGR_MSGQ_State.msgqOwner [msgqId] == NULL) {
            status = PRCS_Create (&prcsInfo, optArgs) ;
            if (DSP_SUCCEEDED (status)) {
                PMGR_MSGQ_State.msgqOwner [msgqId] = prcsInfo ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
            /* Message queue already opened */
            status = DSP_EALREADYEXISTS ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @modif  PMGR_MSGQ_State.msgqOwner
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_close (IN MSGQ_Queue msgqQueue, IN     Void * optArgs)
{
    DSP_STATUS   status    = DSP_SOK ;
    DSP_STATUS   tmpStatus = DSP_SOK ;
    MSGQ_Id       msgqId    = (MSGQ_Id) msgqQueue ;

    TRC_1ENTER ("PMGR_MSGQ_close", msgqQueue) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if ( PMGR_MSGQ_State.msgqOwner != NULL) {
        if (PMGR_MSGQ_State.msgqOwner [msgqId] != NULL) {
            if (PMGR_MSGQ_isOwner (PMGR_MSGQ_State.msgqOwner [msgqId], optArgs)
                == TRUE) {
                status = LDRV_MSGQ_close (msgqQueue) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                tmpStatus = PRCS_Delete (PMGR_MSGQ_State.msgqOwner [msgqId]) ;
                PMGR_MSGQ_State.msgqOwner [msgqId] = NULL ;
                if ((DSP_FAILED (tmpStatus)) && (DSP_SUCCEEDED (status))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                /* Non-owner client may not close the message queue */
                status = DSP_EACCESSDENIED ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            /* Message queue not opened */
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        /* PMGR_MSGQ_State.msgqOwner is not valid */
         status = DSP_EINVALIDARG ;
         SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_locate
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
PMGR_MSGQ_locate (IN     Pstr              queueName,
                  OUT    MSGQ_Queue *       msgqQueue,
                  IN     MSGQ_LocateAttrs * attrs)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_3ENTER ("PMGR_MSGQ_locate", queueName, msgqQueue, attrs) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (queueName != NULL) ;
    DBC_Require (msgqQueue != NULL) ;

    status = LDRV_MSGQ_locate (queueName, msgqQueue, attrs) ;
    if ((DSP_FAILED (status)) && (status != DSP_ENOTFOUND)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_locate", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_locateAsync
 *
 *  @desc   This function asynchronously locates the message queue identified by
 *          the specified MSGQ name. An attempt is made to asynchronously
 *          locate the message queue. If found, an MSGQ_AsyncLocateMsg message is
 *          sent to the specified reply message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_locateAsync (IN Pstr                    queueName,
                       IN MSGQ_Queue               replyQueue,
                       IN MSGQ_LocateAsyncAttrs *  attrs)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_3ENTER ("PMGR_MSGQ_locateAsync", queueName, replyQueue, attrs) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (queueName != NULL) ;
    DBC_Require (IS_VALID_MSGQ (replyQueue)) ;
    DBC_Require (attrs     != NULL) ;

    status = LDRV_MSGQ_locateAsync (queueName, replyQueue, attrs) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_locateAsync", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_release
 *
 *  @desc   This function releases the message queue identified by the MSGQ
 *          handle that was located earlier.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_release (IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_1ENTER ("PMGR_MSGQ_release", msgqQueue) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    status = LDRV_MSGQ_release (msgqQueue) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_release", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_alloc
 *
 *  @desc   This function allocates a message, and returns the pointer to the
 *          user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_3ENTER ("PMGR_MSGQ_alloc", poolId, size, msg) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (msg != NULL) ;
    DBC_Require (size >= sizeof (MSGQ_MsgHeader)) ;

    status = LDRV_MSGQ_alloc (poolId, size, msg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_free
 *
 *  @desc   This function frees a message.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_free (IN MSGQ_Msg msg)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_1ENTER ("PMGR_MSGQ_free", msg) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (msg != NULL) ;

    status = LDRV_MSGQ_free (msg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("PMGR_MSGQ_put", msgqQueue, msg) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    status = LDRV_MSGQ_put (msgqQueue, msg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_put", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_get
 *
 *  @desc   This function receives a message on the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_3ENTER ("PMGR_MSGQ_get", msgqQueue, timeout, msg) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    /* Ownership validation is not being done here for efficiency */
    status = LDRV_MSGQ_get (msgqQueue, timeout, msg) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_get", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_count
 *
 *  @desc   This API returns the count of the number of messages in a local
 *          message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("PMGR_MSGQ_count", msgqQueue, count) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (count != NULL) ;

    /* Ownership validation is not being done here for efficiency */
    status = LDRV_MSGQ_count (msgqQueue, count) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_count", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_MSGQ_setErrorHandler
 *
 *  @desc   This API allows the user to designate a MSGQ as an error-handler
 *          MSGQ to receive asynchronous error messages from the transports.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("PMGR_MSGQ_setErrorHandler", errorQueue, poolId) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;

    status = LDRV_MSGQ_setErrorHandler (errorQueue, poolId) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_setErrorHandler", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_MSGQ_instrument
 *
 *  @desc   This function gets the instrumentation information related to the
 *          specified message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("PMGR_MSGQ_instrument", msgqQueue, retVal) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (retVal != NULL) ;

    status = LDRV_MSGQ_instrument (msgqQueue, retVal) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_instrument", status) ;

    return status ;
}
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_MSGQ_debug
 *
 *  @desc   This function prints the current status of the MSGQ subcomponent.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
PMGR_MSGQ_debug (IN MSGQ_Queue msgqQueue)
{
    MSGQ_Id  msgqId = (MSGQ_Id) msgqQueue ;
    TRC_1ENTER ("PMGR_MSGQ_debug", msgqQueue) ;

    DBC_Require (PMGR_MSGQ_IsInitialized == TRUE) ;
    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if (PMGR_MSGQ_IsInitialized == TRUE) {
        if (PMGR_MSGQ_State.msgqOwner [msgqId] != NULL) {
            TRC_2PRINT (TRC_LEVEL4,
                        "MSGQ = [0x%x] owned by [0x%x] \n",
                        msgqQueue,
                        PMGR_MSGQ_State.msgqOwner [msgqId]) ;
        }
        else {
            TRC_1PRINT (TRC_LEVEL4, "MSGQ = [0x%x] not created \n", msgqQueue) ;
        }
    }
    else {
        TRC_0PRINT (TRC_LEVEL4, "MSGQ component not yet initialized \n") ;
    }

    LDRV_MSGQ_debug (msgqQueue) ;

    TRC_0LEAVE ("PMGR_MSGQ_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @func   PMGR_MSGQ_isOwner
 *
 *  @desc   Checks if the calling client is the owner.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
PMGR_MSGQ_isOwner (PrcsObject * prcsInfo, IN     Void * optArgs)
{
    DSP_STATUS   status      = DSP_SOK ;
    DSP_STATUS   tmpStatus   = DSP_SOK ;
    PrcsObject * curPrcsInfo = NULL    ;
    Bool         isOwner     = FALSE   ;

    TRC_1ENTER ("PMGR_MSGQ_isOwner", prcsInfo) ;

    DBC_Require (prcsInfo != NULL) ;

    if (prcsInfo != NULL) {
        status = PRCS_Create (&curPrcsInfo, optArgs) ;

        if (DSP_SUCCEEDED (status)) {
            status = PRCS_IsSameContext (curPrcsInfo,
                                         prcsInfo,
                                         &isOwner) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            tmpStatus = PRCS_Delete (curPrcsInfo) ;
            curPrcsInfo = NULL ;
            if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_MSGQ_isOwner", isOwner) ;

    return isOwner ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

