/** ============================================================================
 *  @file   msgq.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   This file implements the functions of MSGQ API subcomponent.
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

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>

/*  ----------------------------------- Profiling                     */
#include <profile.h>
#if defined (DDSP_PROFILE)
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Header                   */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                      */
#include <msgq.h>
#include <_proc.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_MSGQ

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MSGQ, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   PROC_stateObj
 *
 *  @desc   Extern declaration of the state object for the PROC user-level
 *          component containing internal state information required by it.
 *  ============================================================================
 */
extern PROC_Object PROC_stateObj ;


/** ============================================================================
 *  @func   MSGQ_transportOpen
 *
 *  @desc   This function initializes the transport associated with the
 *          specified processor.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs)
{
    DSP_STATUS  status      = DSP_SOK ;
    DSP_STATUS  tmpStatus   = DSP_SOK ;
    Bool        csObjExists = FALSE ;
    CMD_Args    args ;

    TRC_2ENTER ("MSGQ_transportOpen", procId, attrs) ;

    DBC_Require (attrs != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    if ((attrs == NULL) || ((IS_VALID_PROCID (procId) == FALSE))) {
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
                                  PROC_stateObj.curStatus.mqtIsOpened [procId])
                ==  TRUE) {
                /* Check if MSGQ transport has been opened in this process. */
                status = DSP_EALREADYOPENED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.msgqTransportOpenArgs.procId = procId ;
                args.apiArgs.msgqTransportOpenArgs.attrs  = attrs ;

                status = DRV_INVOKE (DRV_handle, CMD_MSGQ_TRANSPORTOPEN, &args);
                if (DSP_SUCCEEDED (status)) {
                    DRV_SET_CURSTATUS (
                                PROC_stateObj.curStatus.mqtIsOpened [procId]) ;
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

    TRC_1LEAVE ("MSGQ_transportOpen", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_transportClose (IN  ProcessorId procId)
{
    DSP_STATUS  status      = DSP_SOK ;
    DSP_STATUS  tmpStatus   = DSP_SOK ;
    Bool        csObjExists = FALSE ;
    CMD_Args    args ;

    TRC_1ENTER ("MSGQ_transportClose", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    if ((IS_VALID_PROCID (procId) == FALSE)) {
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
                                  PROC_stateObj.curStatus.mqtIsOpened [procId])
                ==  FALSE) {
                /* Check if MSGQ transport has been opened in this process. */
                status = DSP_EACCESSDENIED ;
                SET_FAILURE_REASON ;
            }
            else {
                args.apiArgs.msgqTransportCloseArgs.procId = procId ;

                status = DRV_INVOKE (DRV_handle,
                                     CMD_MSGQ_TRANSPORTCLOSE,
                                     &args) ;
                DRV_RESET_CURSTATUS (
                                 PROC_stateObj.curStatus.mqtIsOpened [procId]) ;
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

    TRC_1LEAVE ("MSGQ_transportClose", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_open
 *
 *  @desc   This function opens the message queue to be used for receiving
 *          messages, identified through the specified message queue name.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_open (IN     Pstr         queueName,
           OUT    MSGQ_Queue * msgqQueue,
           IN     MSGQ_Attrs * attrs)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;
    Uint32      length           ;

    TRC_3ENTER ("MSGQ_open", queueName, msgqQueue, attrs) ;

    DBC_Require (msgqQueue != NULL) ;

    if (msgqQueue == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (queueName != NULL){
        length = strlen (queueName) ;
            if (length >= DSP_MAX_STRLEN) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
        }
        if (DSP_SUCCEEDED(status)) {
            args.apiArgs.msgqOpenArgs.queueName = queueName ;
            args.apiArgs.msgqOpenArgs.msgqQueue = msgqQueue ;
            args.apiArgs.msgqOpenArgs.attrs     = attrs ;

            status = DRV_INVOKE (DRV_handle, CMD_MSGQ_OPEN, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("MSGQ_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_close (IN  MSGQ_Queue msgqQueue)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_1ENTER ("MSGQ_close", msgqQueue) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if ((IS_VALID_MSGQ (msgqQueue) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqCloseArgs.msgqQueue = msgqQueue ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_CLOSE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_locate
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
MSGQ_locate (IN     Pstr               queueName,
             OUT    MSGQ_Queue *       msgqQueue,
             IN     MSGQ_LocateAttrs * attrs)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;
    Uint32      length           ;

    TRC_3ENTER ("MSGQ_locate", queueName, msgqQueue, attrs) ;

    DBC_Require (queueName != NULL) ;
    DBC_Require (msgqQueue != NULL) ;

    if ((queueName == NULL) || (msgqQueue == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        length = strlen (queueName) ;
        if (length >= DSP_MAX_STRLEN) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            args.apiArgs.msgqLocateArgs.queueName = queueName ;
            args.apiArgs.msgqLocateArgs.msgqQueue = msgqQueue ;
            args.apiArgs.msgqLocateArgs.attrs     = attrs  ;

            status = DRV_INVOKE (DRV_handle, CMD_MSGQ_LOCATE, &args) ;
            if ((DSP_FAILED (status)) && (status != DSP_ENOTFOUND)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("MSGQ_locate", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_locateAsync
 *
 *  @desc   This function asynchronously locates the message queue identified by
 *          the specified MSGQ name. An attempt is made to asynchronously
 *          locate the message queue. If found, an MsgqAsyncLocateMsg message is
 *          sent to the specified reply message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_locateAsync (IN Pstr                     queueName,
                  IN MSGQ_Queue               replyQueue,
                  IN MSGQ_LocateAsyncAttrs *  attrs)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;
    Uint32      length           ;

    TRC_3ENTER ("MSGQ_locateAsync", queueName, replyQueue, attrs) ;

    DBC_Require (queueName != NULL) ;
    DBC_Require (IS_VALID_MSGQ (replyQueue)) ;
    DBC_Require (attrs     != NULL) ;

    if (    (queueName  == NULL)
        ||  ((IS_VALID_MSGQ (replyQueue) == FALSE))
        ||  (attrs      == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        length = strlen (queueName) ;
        if (length >= DSP_MAX_STRLEN) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            args.apiArgs.msgqLocateAsyncArgs.queueName  = queueName ;
            args.apiArgs.msgqLocateAsyncArgs.replyQueue = replyQueue ;
            args.apiArgs.msgqLocateAsyncArgs.attrs      = attrs  ;

            status = DRV_INVOKE (DRV_handle, CMD_MSGQ_LOCATEASYNC, &args) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("MSGQ_locateAsync", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_release
 *
 *  @desc   This function releases the message queue identified by the MSGQ
 *          handle that was located earlier.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_release (IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_1ENTER ("MSGQ_release", msgqQueue) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if ((IS_VALID_MSGQ (msgqQueue) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqReleaseArgs.msgqQueue = msgqQueue ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_RELEASE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_release", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_alloc
 *
 *  @desc   This function allocates a message, and returns the pointer to the
 *          user.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_3ENTER ("MSGQ_alloc", poolId, size, msg) ;

    DBC_Require (msg   != NULL) ;
    DBC_Require (size  >= sizeof (MSGQ_MsgHeader)) ;

    if (    (msg == NULL)
        ||  (size < sizeof (MSGQ_MsgHeader))) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqAllocArgs.poolId   = poolId ;
        args.apiArgs.msgqAllocArgs.size     = size ;
        args.apiArgs.msgqAllocArgs.msgAddr  = (Uint32) NULL ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_ALLOC, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            *msg = (MsgqMsg) args.apiArgs.msgqAllocArgs.msgAddr ;
        }
    }

    TRC_1LEAVE ("MSGQ_alloc", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_free
 *
 *  @desc   This function frees a message.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_free (IN  MSGQ_Msg msg)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_1ENTER ("MSGQ_free", msg) ;

    DBC_Require (msg != NULL) ;

    if (msg == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqFreeArgs.msg = msg ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_FREE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_free", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_2ENTER ("MSGQ_put", msgqQueue, msg) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    if (((IS_VALID_MSGQ (msgqQueue) == FALSE)) || (msg == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqPutArgs.msgqQueue  = msgqQueue ;
        args.apiArgs.msgqPutArgs.msg        = msg ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_PUT, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_put", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_get
 *
 *  @desc   This function receives a message on the specified MSGQ.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_3ENTER ("MSGQ_get", msgqQueue, timeout, msg) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (MSGQ_isLocalQueue (msgqQueue)) ;
    DBC_Require (msg != NULL) ;

    if (    ((IS_VALID_MSGQ (msgqQueue) == FALSE))
        ||  ((MSGQ_isLocalQueue (msgqQueue) == FALSE))
        ||  (msg == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqGetArgs.msgqQueue  = msgqQueue ;
        args.apiArgs.msgqGetArgs.timeout    = timeout ;
        args.apiArgs.msgqGetArgs.msgAddr    = (Uint32) NULL ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_GET, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            *msg = (MsgqMsg) args.apiArgs.msgqGetArgs.msgAddr ;
        }
    }

    TRC_1LEAVE ("MSGQ_get", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_getSrcQueue
 *
 *  @desc   This function returns a handle to the source message queue of a
 *          message to be used for replying to the message.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_getSrcQueue (IN MSGQ_Msg msg, OUT MSGQ_Queue * msgqQueue)
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_2ENTER ("MSGQ_getSrcQueue", msg, msgqQueue) ;

    DBC_Require (msg        != NULL) ;
    DBC_Require (msgqQueue  != NULL) ;

    if ((msg == NULL) || (msgqQueue == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* If the srcId is invalid there is nothing can be done. */
        if (msg->srcId == MSGQ_INVALIDMSGQ) {
            status = DSP_ENOTFOUND ; /* Not an error condition */
        }
        else {
            *msgqQueue = (MSGQ_Queue) ((((Uint32) msg->srcProcId) << 16) | msg->srcId) ;
        }
    }

    TRC_1LEAVE ("MSGQ_getSrcQueue", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_count
 *
 *  @desc   This API returns the count of the number of messages in a local
 *          message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_2ENTER ("MSGQ_count", msgqQueue, count) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (MSGQ_isLocalQueue (msgqQueue)) ;
    DBC_Require (count != NULL) ;

    if (    ((IS_VALID_MSGQ (msgqQueue) == FALSE))
        ||  ((MSGQ_isLocalQueue (msgqQueue) == FALSE))
        ||  (count == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqCountArgs.msgqQueue = msgqQueue ;
        args.apiArgs.msgqCountArgs.count     = count ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_COUNT, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_count", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MSGQ_setErrorHandler
 *
 *  @desc   This API allows the user to designate a MSGQ as an error-handler
 *          MSGQ to receive asynchronous error messages from the transports.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_2ENTER ("MSGQ_setErrorHandler", errorQueue, poolId) ;

    args.apiArgs.msgqSetErrorHandlerArgs.errorQueue = errorQueue ;
    args.apiArgs.msgqSetErrorHandlerArgs.poolId     = poolId ;

    status = DRV_INVOKE (DRV_handle, CMD_MSGQ_SETERRORHANDLER, &args) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("MSGQ_setErrorHandler", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   MSGQ_instrument
 *
 *  @desc   This function gets the instrumentation information related to the
 *          specified message queue.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_2ENTER ("MSGQ_instrument", msgqQueue, retVal) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;
    DBC_Require (retVal != NULL) ;

    if (((IS_VALID_MSGQ (msgqQueue) == FALSE)) || (retVal == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqInstrumentArgs.msgqQueue   = msgqQueue ;
        args.apiArgs.msgqInstrumentArgs.retVal      = retVal ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_INSTRUMENT, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_instrument", status) ;

    return status ;
}
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   MSGQ_debug
 *
 *  @desc   This function prints the current status of the MSGQ subcomponent.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
MSGQ_debug (IN MSGQ_Queue msgqQueue)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_1ENTER ("MSGQ_debug", msgqQueue) ;

    DBC_Require (IS_VALID_MSGQ (msgqQueue)) ;

    if ((IS_VALID_MSGQ (msgqQueue) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.msgqDebugArgs.msgqQueue = msgqQueue ;

        status = DRV_INVOKE (DRV_handle, CMD_MSGQ_DEBUG, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MSGQ_debug", status) ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
