/** ============================================================================
 *  @file   ldrv_msgq.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines the interface and structures of LDRV MSGQ driver.
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


#if !defined (LDRV_MSGQ_H)
#define LDRV_MSGQ_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <msgqdefs.h>
#include <ldrv_msgq.h>
#include <ldrv_mqt.h>

/*  ----------------------------------- Profiling                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- Generic Functions             */
#include<list.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LDRV_MSGQ_Object
 *
 *  @desc   Forward declaration of LDRV MSGQ object.
 *  ============================================================================
 */
typedef struct LDRV_MSGQ_Object_tag LDRV_MSGQ_Object ;

/** ============================================================================
 *  @name   LDRV_MSGQ_Handle
 *
 *  @desc   Forward declaration of the handle to LDRV MSGQ object.
 *  ============================================================================
 */
typedef LDRV_MSGQ_Object * LDRV_MSGQ_Handle ;


/** ============================================================================
 *  @name   LDRV_MSGQ_Object
 *
 *  @desc   This structure defines the MSGQ object. It includes all information
 *          specific to a particular MSGQ.
 *
 *  @field  name
 *              System-wide unique message queue name.
 *  @field  msgqQueue
 *              Message queue handle.
 *  @field  queue
 *              Queue of received messages.
 *  @field  ntfyHandle
 *              Pointer to the notification object for the message queue.
 *  @field  pend
 *              Function to be used to wait to receive a message.
 *  @field  post
 *              Function to be used to indicate arrival of a message.
 *  @field  defaultNtfyHandle
 *              Indicates whether the notify handle in the message queue object
 *              was created internally.
 *  @field  procStats
 *              Instrumentation information for the Message Queue.
 *              Defined only if profiling is enabled.
 *  ============================================================================
 */
struct LDRV_MSGQ_Object_tag {
    Char8            name [DSP_MAX_STRLEN] ;
    MSGQ_Queue       msgqQueue ;
    List *           queue ;
    Pvoid            ntfyHandle ;
    MsgqPend         pend ;
    MsgqPost         post ;
    Bool             defaultNtfyHandle ;
#if defined (DDSP_PROFILE)
    MSGQ_Stats       msgqStats ;
#endif /* if defined (DDSP_PROFILE) */
} ;

/** ============================================================================
 *  @name   LDRV_MSGQ_State
 *
 *  @desc   This structure defines the MSGQ state object. It includes all global
 *          information required by the MSGQ component.
 *
 *  @field  msgqHandles
 *              Array of handles to message queue objects.
 *  @field  maxMsgqs
 *              Maximum number of message queues on the GPP.
 *  @field  numDsps
 *              Number of DSPs in the system.
 *  @field  transports
 *              Array of transport objects, one for every processor in the
 *              system.
 *  @field  doDspCtrl
 *              Indicates whether power control of the DSPs should be done
 *              within DSPLINK.
 *  @field  errorQueue
 *              Handle to the MSGQ registered by the user as an error handler.
 *              If no error handler MSGQ has been registered by the user, the
 *              value of this field is MSGQ_INVALIDMSGQ.
 *  @field  errorPoolId
 *              ID of the Pool to be used for allocating the asynchronous
 *              error messages, if the user has registered an error handler
 *              MSGQ. If no error handler MSGQ has been registered by the user,
 *              the value of this field is POOL_INVALIDID
 *  @field  lock
 *              Lock for using MSGQ resources.
 *  ============================================================================
 */
typedef struct LDRV_MSGQ_State_tag {
    LDRV_MSGQ_Handle *       msgqHandles ;
    Uint16                   maxMsgqs ;
    Uint16                   numDsps ;
    LDRV_MSGQ_TransportObj * transports ;
    DSP_BootMode             doDspCtrl [MAX_DSPS] ;
    MSGQ_Queue               errorQueue ;
    PoolId                   errorPoolId ;
    Pvoid                    lock        ;
} LDRV_MSGQ_State ;


/** ============================================================================
 *  @func   LDRV_MSGQ_setup
 *
 *  @desc   This function initializes the MSGQ component.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must not be initialized.
 *
 *  @leave  The LDRV_MSGQ component must be initialized upon successful
 *          completion otherwise it must be uninitialized.
 *
 *  @see    LDRV_MSGQ_destroy
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_setup (Void) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_destroy
 *
 *  @desc   This function finalizes the MSGQ component.
 *
 *  @arg    None
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  The LDRV_MSGQ component must be uninitialized.
 *
 *  @see    LDRV_MSGQ_setup
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_destroy (Void) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_transportOpen
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
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          procId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MsgqTransportAttrs
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_transportOpen (IN  ProcessorId procId, IN  Pvoid attrs) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_transportClose
 *
 *  @desc   This function finalizes the transport associated with the
 *          specified processor.
 *
 *  @arg    procId
 *              ID of the Processor for which the transport is to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          procId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MSGQ_allocatorOpen
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_transportClose (IN  ProcessorId procId) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_open
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
 *          DSP_ENOTFOUND
 *              Attempt to open more than number of message queues configured.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  On success, the msgqQueue returned must not be MSGQ_INVALIDMSGQ.
 *          On failure, the msgqQueue returned must be MSGQ_INVALIDMSGQ.
 *
 *  @see    MSGQ_Queue, MSGQ_Attrs, LDRV_MSGQ_close (), LDRV_MSGQ_locate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_open (IN     Pstr        queueName,
                OUT    MSGQ_Queue * msgqQueue,
                IN     MSGQ_Attrs * attrs) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_close
 *
 *  @desc   This function closes the message queue identified by the specified
 *          MSGQ handle
 *
 *  @arg    msgqQueue
 *              Handle to the message queue to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msgqQueue must be valid.
 *
 *  @leave  None.
 *
 *  @see    MSGQ_Queue, LDRV_MSGQ_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_close (IN MSGQ_Queue msgqQueue) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_locate
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  On success, the msgqQueue returned must not be MSGQ_INVALIDMSGQ.
 *          On failure, the msgqQueue returned must be MSGQ_INVALIDMSGQ.
 *
 *  @see    MSGQ_Queue, MSGQ_LocateAttrs, LDRV_MSGQ_put (), LDRV_MSGQ_release ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locate (IN     Pstr              queueName,
                  OUT    MSGQ_Queue *       msgqQueue,
                  IN     MSGQ_LocateAttrs * attrs) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_locateAsync
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          queueName must be valid.
 *          replyQueue must be valid.
 *          attrs must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_LocateAsyncAttrs,
 *          LDRV_MSGQ_put (), LDRV_MSGQ_release ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locateAsync (IN Pstr                    queueName,
                       IN MSGQ_Queue               replyQueue,
                       IN MSGQ_LocateAsyncAttrs *  attrs) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_release
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, LDRV_MSGQ_locate ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_release (IN MSGQ_Queue msgqQueue) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_alloc
 *
 *  @desc   This function allocates a message, and returns the pointer to the
 *          user.
 *
 *  @arg    poolId
 *              ID of the Pool to be used for allocating this message.
 *  @arg    size
 *              Size (in bytes) of the message to be allocated.
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msg must be a valid pointer.
 *          size must be greater than size of MSGQ_MsgHeader.
 *
 *  @leave  On success, the msg returned must not be NULL.
 *          On failure, the msg returned must be NULL.
 *
 *  @see    MSGQ_MsgHeader, LDRV_MSGQ_put (), LDRV_MSGQ_free ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_alloc (IN PoolId poolId, IN Uint16 size, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_free
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_MsgHeader, LDRV_MSGQ_get (), LDRV_MSGQ_alloc ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_free (IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_put
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
 *  @ret    DSP_ENOTFOUND
 *              The message queue does not exist.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msgqQueue must be valid.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue, MSGQ_MsgHeader, LDRV_MSGQ_get ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_put (IN MSGQ_Queue msgqQueue, IN MSGQ_Msg msg) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_get
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msgQueue must be valid.
 *          msg must be a valid pointer.
 *
 *  @leave  On success, the msg returned must not be NULL.
 *          On failure, the msg returned must be NULL.
 *
 *  @see    MSGQ_Queue, MSGQ_MsgHeader, LDRV_MSGQ_put ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_get (IN MSGQ_Queue msgqQueue, IN Uint32 timeout, OUT MSGQ_Msg * msg) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_count
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
 *  @enter  The LDRV_MSGQ component must be initialized before calling this
 *          function.
 *          msgQueue must be valid.
 *          count must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Queue
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_count (IN MSGQ_Queue msgqQueue, OUT Uint16 * count) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_setErrorHandler
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          errorQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    MSGQ_AsyncErrorMsg, LDRV_MSGQ_sendErrorMsg ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_setErrorHandler (IN MSGQ_Queue errorQueue, IN PoolId poolId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_MSGQ_instrument
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
 *  @enter  The LDRV_MSGQ component must be initialized.
 *          msgqQueue must be valid.
 *          retVal must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MSGQ_Instrument
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_instrument (IN MSGQ_Queue msgqQueue, OUT MSGQ_Instrument * retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_MSGQ_debug
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
LDRV_MSGQ_debug (IN MSGQ_Queue msgqQueue) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_MSGQ_H) */
