/** ============================================================================
 *  @file   pmgr_msgq.h
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          PMGR_MSGQ.
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


#if !defined (PMGR_MSGQ_H)
#define  PMGR_MSGQ_H

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <msgqdefs.h>

/*  ----------------------------------- Profiling                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   PMGR_MSGQ_setup
 *
 *  @desc   This function initializes the MSGQ component.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must not be initialized before calling this
 *          function.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_setup ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_setup (Void) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_destroy
 *
 *  @desc   This function finalizes the MSGQ component.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_destroy ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_destroy (Void) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_transportOpen
 *
 *  @desc   This function initializes the transport associated with the
 *          specified processor.
 *
 *  @arg    procId
 *              ID of the Processor for which the transport is to be opened.
 *  @arg    attrs
 *              Attributes for initialization of the transport.
 *              The structure of the expected attributes is specific to a
 *              transport.
 *
 *  @ret    DSP_SOK
 *              The MQT component has been successfully opened.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          attrs must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_transportOpen ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @arg    procId
 *              ID of the Processor for which the transport is to be closed.
 *
 *  @ret    DSP_SOK
 *              The MQT component has been successfully closed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          Client must be the owner of the transport.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_transportClose ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_transportClose (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_open
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
 *  @arg    optArgs
 *              Optional arguments
 *
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTFOUND
 *              Attempt to open more than number of message queues configured.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_open (IN     Pstr        queueName,
                OUT    MSGQ_Queue * msgqQueue,
                IN     MSGQ_Attrs * attrs,
                IN     Void *       optArgs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @arg    msgqQueue
 *              Handle to the message queue to be closed.
 *  @arg    optArgs
 *              Optional arguments
 *
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EACCESSDENIED
 *              Access denied. Only the client who had successfully opened the
 *              message queue may call this function.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgqQueue must be valid.
 *          Client must be the owner of the MSGQ.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_close (IN MSGQ_Queue msgqQueue, IN     Void * optArgs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_locate
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
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_locate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_locate (IN     Pstr              queueName,
                  OUT    MSGQ_Queue *       msgqQueue,
                  IN     MSGQ_LocateAttrs * attrs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_locateAsync
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
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          queueName must be valid.
 *          replyQueue must be valid.
 *          attrs must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_locateAsync ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_locateAsync (IN Pstr                    queueName,
                       IN MSGQ_Queue               replyQueue,
                       IN MSGQ_LocateAsyncAttrs *  attrs) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_release
 *
 *  @desc   This function releases the message queue identified by the MSGQ
 *          handle that was located earlier.
 *
 *  @arg    msgqQueue
 *              Handle to the message queue to be released.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTFOUND
 *              The message queue has not been previously located.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_release ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_release (IN MSGQ_Queue msgqQueue) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_alloc
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
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msg must be a valid pointer.
 *          size must be greater than size of MSGQ_MsgHeader.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_alloc ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_free
 *
 *  @desc   This function frees a message.
 *
 *  @arg    msg
 *              Pointer to the message to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_free ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_free (IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_put
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
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgqQueue must be valid.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_put ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_get
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
 *          DSP_ETIMEOUT
 *              Timeout occurred while receiving the message.
 *          DSP_ENOTCOMPLETE
 *               Operation not complete when WAIT_NONE was specified as timeout.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgQueue must be valid.
 *          msg must be a valid pointer.
 *          Client must be the owner of msgqId MSGQ.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_get ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_count
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
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgQueue must be valid.
 *          count must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_count ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count) ;


/** ============================================================================
 *  @func   PMGR_MSGQ_setErrorHandler
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
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_setErrorHandler ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_MSGQ_instrument
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
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgqQueue must be valid.
 *          retVal must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_instrument ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
PMGR_MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_MSGQ_debug
 *
 *  @desc   This function prints the current status of the MSGQ subcomponent.
 *
 *  @arg    msgqQueue
 *              Handle to the message queue.
 *
 *  @ret    None
 *
 *  @enter  PMGR MSGQ component must be initialized before calling this
 *          function.
 *          msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_debug ()
 *  ============================================================================
 */
EXPORT_API
Void
PMGR_MSGQ_debug (IN MSGQ_Queue msgqQueue) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PMGR_MSGQ_H) */
