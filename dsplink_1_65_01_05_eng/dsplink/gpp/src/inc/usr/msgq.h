/** ============================================================================
 *  @file   msgq.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines the interfaces and data structures for the API sub-component
 *          MSGQ.
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


#if !defined (MSGQ_H)
#define  MSGQ_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <msgqdefs.h>

/*  ----------------------------------- Profiling                       */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  MSGQ_getMsgId
 *
 *  @desc   This macro returns the message ID of the specified message.
 *  ============================================================================
 */
#define MSGQ_getMsgId(msg) (((MSGQ_Msg) (msg))->msgId)

/** ============================================================================
 *  @macro  MSGQ_getMsgSize
 *
 *  @desc   This macro returns the size of the specified message.
 *  ============================================================================
 */
#define MSGQ_getMsgSize(msg) (((MSGQ_Msg) (msg))->size)

/** ============================================================================
 *  @macro  MSGQ_setMsgId
 *
 *  @desc   This macro sets the message ID in the specified message.
 *  ============================================================================
 */
#define MSGQ_setMsgId(msg, id) ((MSGQ_Msg) (msg))->msgId = id

/** ============================================================================
 *  @macro  MSGQ_getDstQueue
 *
 *  @desc   This macro returns the MSGQ_Queue handle of the destination message
 *          queue for the specified message.
 *  ============================================================================
 */
#define MSGQ_getDstQueue(msg) ((Uint32) ID_GPP << 16u) | ((MSGQ_Msg) (msg))->dstId

/** ============================================================================
 *  @macro  MSGQ_setSrcQueue
 *
 *  @desc   This macro sets the source message queue in the specified message.
 *  ============================================================================
 */
#define MSGQ_setSrcQueue(msg, msgq) ((MSGQ_Msg) (msg))->srcId = (MSGQ_Id) msgq ; \
                                    ((MSGQ_Msg) (msg))->srcProcId = (Uint16)     \
                                     (msgq >> 16u)

/** ============================================================================
 *  @macro  MSGQ_isLocalQueue
 *
 *  @desc   This macro checks whether the specified queue is a local queue.
 *  ============================================================================
 */
#define MSGQ_isLocalQueue(msgq) ((msgq >> 16u) == ID_GPP)


/** ============================================================================
 *  @func   MSGQ_transportOpen
 *
 *  @desc   This function initializes the transport associated with the
 *          specified processor. The transport corresponding to the processor ID
 *          specified in the call should be configured in the CFG. This function
 *          should be called only once as part of system initialization after
 *          the client has called PROC_Attach the first time. Subsequent
 *          tasks / threads should not call this function again as it would
 *          result in unpredictable behavior.
 *
 *  @arg    procId
 *              ID of the Processor for which the transport is to be opened.
 *  @arg    attrs
 *              Attributes for initialization of the transport.
 *              The structure of the expected attributes is specific to a
 *              transport.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SALREADYOPENED
 *              The MSGQ transport for the specified processor has already been
 *              opened.
 *          DSP_EATTACHED
 *              This process has not attached to the specified processor.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *          DSP_EACCESSDENIED
 *              Access to the DSP is denied.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  attrs must be valid.
 *          procId must be valid.
 *          PROC_attach has been successful.
 *
 *  @leave  None
 *
 *  @see    MSGQ_transportClose
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs) ;


/** ============================================================================
 *  @func   MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @arg    procId
 *              ID of the Processor for which the transport is to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SCLOSED
 *              The final process has closed the MSGQ transport.
 *          DSP_EATTACHED
 *              This process has not attached to the specified processor.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *          DSP_EOPENED
 *              The MSGQ transport was not opened.
 *          DSP_EACCESSDENIED
 *              The MSGQ transport was not opened in this process.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          All MSGQ operations should be complete.
 *
 *  @leave  None
 *
 *  @see    MSGQ_transportOpen
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_transportClose (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   MSGQ_open
 *
 *  @desc   This function opens the message queue to be used for receiving
 *          messages, identified through the specified message queue name.
 *
 *  @arg    queueName
 *              Name of the message queue to be opened.
 *  @arg    msgqQueue
 *              Location to store the handle to the message queue.
 *  @arg    attrs
 *              Optional attributes for creation of the MSGQ.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ENOTFOUND
 *              Attempt to open more than number of message queues configured.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *          PROC_attach has been successful.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_Attrs, MSGQ_close (), MSGQ_locate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_open (IN     Pstr         queueName,
           OUT    MSGQ_Queue * msgqQueue,
           IN     MSGQ_Attrs * attrs) ;


/** ============================================================================
 *  @func   MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @arg    msgqQueue
 *              Handle to the message queue to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgqQueue must be valid.
 *          All MSGQ operations are complete.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_close (IN MSGQ_Queue msgqQueue) ;


/** ============================================================================
 *  @func   MSGQ_locate
 *
 *  @desc   This function synchronously locates the message queue identified by
 *          the specified MSGQ name and returns a handle to the located message
 *          queue.
 *
 *  @arg    queueName
 *              Name of the message queue to be located.
 *  @arg    msgqQueue
 *              Location to store the handle to the located message queue.
 *  @arg    attrs
 *              Optional attributes for location of the MSGQ.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ENOTFOUND
 *              The specified message queue could not be located.
 *          DSP_ETIMEOUT
 *              Timeout occurred while locating the message.
 *          DSP_ENOTCOMPLETE
 *               Operation not complete when WAIT_NONE was specified as timeout.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *          Corresponding MSGQ has been opened successfully.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_LocateAttrs, MSGQ_put (), MSGQ_release ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_locate (IN     Pstr               queueName,
             OUT    MSGQ_Queue *       msgqQueue,
             IN     MSGQ_LocateAttrs * attrs) ;


/** ============================================================================
 *  @func   MSGQ_locateAsync
 *
 *  @desc   This function asynchronously locates the message queue identified by
 *          the specified MSGQ name. An attempt is made to asynchronously
 *          locate the message queue. If found, an MSGQ_AsyncLocateMsg message is
 *          sent to the specified reply message queue.
 *
 *  @arg    queueName
 *              Name of the message queue to be located.
 *  @arg    replyQueue
 *              Message queue to be used to receive the response message for
 *              asynchronous location.
 *  @arg    attrs
 *              Attributes for asynchronous location of the MSGQ.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  queueName must be valid.
 *          replyQueue must be valid.
 *          attrs must be valid.
 *          Corresponding MSGQ has been opened successfully.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_LocateAsyncAttrs, MSGQ_put (), MSGQ_release ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_locateAsync (IN Pstr                     queueName,
                  IN MSGQ_Queue               replyQueue,
                  IN MSGQ_LocateAsyncAttrs *  attrs) ;


/** ============================================================================
 *  @func   MSGQ_release
 *
 *  @desc   This function releases the message queue identified by the MSGQ
 *          handle that was located earlier.
 *
 *  @arg    msgqQueue
 *              Handle to the message queue to be released.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_ENOTFOUND
 *              The message queue has not been previously located.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgqQueue must be valid.
 *          MSGQ_locate has been successful.
 *          All MSGQ operations pertaining to that MSGQ are complete.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_locate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_release (IN MSGQ_Queue msgqQueue) ;


/** ============================================================================
 *  @func   MSGQ_alloc
 *
 *  @desc   This function allocates a message, and returns the pointer to the
 *          user.
 *
 *  @arg    poolId
 *              ID of the Pool to be used for allocating this message.
 *  @arg    size
 *              Size of the message to be allocated.
 *  @arg    msg
 *              Location to receive the allocated message.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msg must be a valid pointer.
 *          size must be greater than size of MSGQ_MsgHeader.
 *          POOL has been configured according to application specification.
 *
 *  @leave  None
 *
 *  @see    MSGQ_MsgHeader, MSGQ_put ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   MSGQ_free
 *
 *  @desc   This function frees a message.
 *
 *  @arg    msg
 *              Pointer to the message to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msg must be valid.
 *          MSGQ_alloc must be successful.
 *
 *  @leave  None
 *
 *  @see    MSGQ_MsgHeader, MSGQ_get ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_free (IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   MSGQ_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @arg    msgqQueue
 *              Handle to the destination MSGQ.
 *  @arg    msg
 *              Pointer to the message to be sent to the destination MSGQ.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgqQueue must be valid.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_MsgHeader, MSGQ_get ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   MSGQ_get
 *
 *  @desc   This function receives a message on the specified MSGQ.
 *
 *  @arg    msgqQueue
 *              Handle to the MSGQ on which the message is to be received.
 *  @arg    timeout
 *              Timeout value to wait for the message (in milliseconds).
 *  @arg    msg
 *              Location to receive the message.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *          DSP_ETIMEOUT
 *              Timeout occurred while receiving the message.
 *          DSP_ENOTCOMPLETE
 *               Operation not complete when WAIT_NONE was specified as timeout.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgQueue must be valid.
 *          msgqQueue must be a local queue.
 *          msg must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_MsgHeader, MSGQ_put ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   MSGQ_getSrcQueue
 *
 *  @desc   This function returns a handle to the source message queue of a
 *          message to be used for replying to the message.
 *
 *  @arg    msg
 *              Message, whose source MSGQ handle is to be returned.
 *  @arg    msgqQueue
 *              Location to retrieve the handle to the source MSGQ.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *          DSP_ENOTFOUND
 *              Source information has not been provided by the sender.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msg must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_MsgHeader
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_getSrcQueue (IN MSGQ_Msg msg, OUT MSGQ_Queue * msgqQueue) ;


/** ============================================================================
 *  @func   MSGQ_count
 *
 *  @desc   This API returns the count of the number of messages in a local
 *          message queue.
 *
 *  @arg    msgqQueue
 *              Handle to the MSGQ for which the count is to be retrieved.
 *  @arg    count
 *              Location to receive the message count.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgqQueue must be valid.
 *          msgqQueue must be a local queue.
 *          count must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count) ;


/** ============================================================================
 *  @func   MSGQ_setErrorHandler
 *
 *  @desc   This API allows the user to designate a MSGQ as an error-handler
 *          MSGQ to receive asynchronous error messages from the transports.
 *
 *  @arg    errorQueue
 *              Handle to the message queue to receive the error messages.
 *  @arg    poolId
 *              ID indicating the pool to be used for allocating the error
 *              messages.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  MSGQ_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MSGQ_AsyncErrorMsg
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   MSGQ_instrument
 *
 *  @desc   This function gets the instrumentation information related to the
 *          specified message queue.
 *
 *  @arg    msgqQueue
 *              Handle to the message queue.
 *  @arg    retVal
 *              Location to retrieve the instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  msgqQueue must be valid.
 *          retVal must be a valid pointer.
 *          Profiling has been selected in the configuration.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Instrument
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   MSGQ_debug
 *
 *  @desc   This function prints the current status of the MSGQ subcomponent.
 *
 *  @arg    msgqQueue
 *              Handle to the message queue.
 *
 *  @ret    None
 *
 *  @enter  msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Void
MSGQ_debug (IN MSGQ_Queue msgqQueue) ;
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @deprecated The deprecated API MSGQ_GetMsgId has been replaced
 *              with MSGQ_getMsgId.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_GetMsgId              MSGQ_getMsgId

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_GetMsgSize has been replaced
 *              with MSGQ_getMsgSize.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_GetMsgSize            MSGQ_getMsgSize

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_SetMsgId has been replaced
 *              with MSGQ_setMsgId.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_SetMsgId              MSGQ_setMsgId

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_GetDstQueue has been replaced
 *              with MSGQ_getDstQueue.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_GetDstQueue           MSGQ_getDstQueue

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_SetSrcQueue has been replaced
 *              with MSGQ_setSrcQueue.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_SetSrcQueue           MSGQ_setSrcQueue

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_IsLocalQueue has been replaced
 *              with MSGQ_isLocalQueue.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_IsLocalQueue          MSGQ_isLocalQueue

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_TransportOpen has been replaced
 *              with MSGQ_transportOpen.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_TransportOpen         MSGQ_transportOpen

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_TransportClose has been replaced
 *              with MSGQ_transportClose.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_TransportClose        MSGQ_transportClose

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Open has been replaced
 *              with MSGQ_open.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Open                  MSGQ_open

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Close has been replaced
 *              with MSGQ_close.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Close                 MSGQ_close

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Locate has been replaced
 *              with MSGQ_locate.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Locate                MSGQ_locate

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_LocateAsync has been replaced
 *              with MSGQ_locateAsync.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_LocateAsync           MSGQ_locateAsync

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Release has been replaced
 *              with MSGQ_release.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Release               MSGQ_release

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Alloc has been replaced
 *              with MSGQ_alloc.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Alloc                 MSGQ_alloc

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Free has been replaced
 *              with MSGQ_free.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Free                  MSGQ_free

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Put has been replaced
 *              with MSGQ_put.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Put                   MSGQ_put

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Get has been replaced
 *              with MSGQ_get.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Get                   MSGQ_get

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_GetSrcQueue has been replaced
 *              with MSGQ_getSrcQueue.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_GetSrcQueue           MSGQ_getSrcQueue

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Count has been replaced
 *              with MSGQ_count.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Count                 MSGQ_count

/** ============================================================================
 *  @deprecated The deprecated API MSGQ_SetErrorHandler has been replaced
 *              with MSGQ_setErrorHandler.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_SetErrorHandler       MSGQ_setErrorHandler

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @deprecated The deprecated API MSGQ_Debug has been replaced
 *              with MSGQ_debug.
 *              This has been done to follow DSP/BIOS codinf guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define MSGQ_Debug                 MSGQ_debug
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MSGQ_H) */
