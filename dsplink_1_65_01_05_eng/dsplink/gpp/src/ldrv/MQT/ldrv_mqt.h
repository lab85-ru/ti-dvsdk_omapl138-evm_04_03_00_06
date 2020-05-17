/** ============================================================================
 *  @file   ldrv_mqt.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MQT/
 *
 *  @desc   Defines the interface and structures of LDRV MQT.
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


#if !defined (LDRV_MQT_H)
#define LDRV_MQT_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <msgqdefs.h>
#include <_loaderdefs.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   LDRV_MSGQ_TransportObj
 *
 *  @desc   Forward declaration of LDRV MQT transport object.
 *  ============================================================================
 */
typedef struct LDRV_MSGQ_TransportObj_tag LDRV_MSGQ_TransportObj ;

/** ============================================================================
 *  @name   LDRV_MSGQ_TransportHandle
 *
 *  @desc   Forward declaration of the handle to LDRV MQT transport object.
 *  ============================================================================
 */
typedef LDRV_MSGQ_TransportObj * LDRV_MSGQ_TransportHandle ;


/** ============================================================================
 *  @name   FnMqtInit
 *
 *  @desc   Signature of the function that performs MQT initialization.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    mqtId
 *              MQT Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the SHM DRV component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtInit) (IN  ProcessorId dspId,
                                 IN  Uint32      mqtId,
                                 OUT Uint32 *    shDspAddr) ;

/** ============================================================================
 *  @name   FnMqtExit
 *
 *  @desc   Signature of the function that performs MQT finalization.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    mqtId
 *              MQT Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtExit) (IN ProcessorId dspId, IN  Uint32 mqtId) ;

/** ============================================================================
 *  @name   FnMqtOpen
 *
 *  @desc   Signature of the function that opens the MQT and configures it
 *          according to the user attributes.
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MQT transport object.
 *  @arg    mqtAttrs
 *              Attributes for initialization of the MQT component.
 *
 *  @ret    DSP_SOK
 *              This component has been successfully opened.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtOpen) (IN LDRV_MSGQ_TransportHandle mqtHandle,
                                 IN Pvoid                     mqtAttrs) ;

/** ============================================================================
 *  @name   FnMqtClose
 *
 *  @desc   Signature of the function that closes the MQT, and cleans up its
 *          state object.
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MQT transport object.
 *
 *  @ret    DSP_SOK
 *              This component has been successfully closed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtClose) (IN LDRV_MSGQ_TransportHandle mqtHandle) ;

/** ============================================================================
 *  @name   FnMqtLocate
 *
 *  @desc   Signature of the function that locates the message queue identified
 *          by the specified MSGQ name.
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MQT transport object.
 *  @arg    queueName
 *              Name of the message queue to be located.
 *  @arg    sync
 *              Indicates whether the location is synchronous.
 *  @arg    msgqQueue
 *              If synchronous:  indicates the location to store the handle to
 *                               the located message queue.
 *              If asynchronous: indicates the message queue to be used to
 *                               receive the response message for location.
 *  @arg    attrs
 *              If synchronous:  indicates the optional attributes for
 *                               synchronous location of the MSGQ.
 *              If asynchronous: indicates the attributes for asynchronous
 *                               location of the MSGQ.
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
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtLocate) (IN     LDRV_MSGQ_TransportHandle mqtHandle,
                                   IN     Pstr                      queueName,
                                   IN     Bool                      sync,
                                   IN OUT MSGQ_Queue *               msgqQueue,
                                   IN     Pvoid                     attrs) ;

/** ============================================================================
 *  @name   FnMqtRelease
 *
 *  @desc   Signature of the function that releases the MSGQ identified by the
 *          MSGQ handle that was located earlier.
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MQT transport object.
 *  @arg    msgqQueue
 *              Handle to the message queue to be released.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtRelease) (IN LDRV_MSGQ_TransportHandle mqtHandle,
                                    IN MSGQ_Queue                msgqQueue) ;

/** ============================================================================
 *  @name   FnMqtPut
 *
 *  @desc   Signature of the function that sends a message to the specified
 *          MSGQ.
 *
 *  @arg    mqtHandle
 *              Handle to LDRV MQT transport object.
 *  @arg    msg
 *              Pointer to the message to be sent.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *  ============================================================================
 */
typedef DSP_STATUS (*FnMqtPut) (IN LDRV_MSGQ_TransportHandle mqtHandle,
                                IN MSGQ_Msg                  msg) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @name   FnMqtDebug
 *
 *  @desc   Signature of the MQT debug function.
 *
 *  @arg    mqtHandle
 *              Handle to LDRV MQT transport object.
 *
 *  @ret    None.
 *  ============================================================================
 */
typedef Void (*FnMqtDebug) (IN LDRV_MSGQ_TransportHandle mqtHandle) ;
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   MQT_Interface
 *
 *  @desc   This structure defines the function pointer table that must be
 *          implemented for every MQT in the system.
 *
 *  @field  init
 *              Pointer to MQT init function.
 *  @field  exit
 *              Pointer to MQT exit function.
 *  @field  open
 *              Pointer to MQT open function.
 *  @field  close
 *              Pointer to MQT close function.
 *  @field  locate
 *              Pointer to MQT function for locating a MSGQ.
 *  @field  release
 *              Pointer to MQT function for releasing a MSGQ.
 *  @field  put
 *              Pointer to MQT function for sending a message.
 *  @field  debug
 *              Pointer to MQT debug function.
 *  ============================================================================
 */
struct MQT_Interface_tag {
    FnMqtInit         init       ;
    FnMqtExit         exit       ;
    FnMqtOpen         open       ;
    FnMqtClose        close      ;
    FnMqtLocate       locate     ;
    FnMqtRelease      release    ;
    FnMqtPut          put        ;
#if defined (DDSP_DEBUG)
    FnMqtDebug        debug      ;
#endif /* defined (DDSP_DEBUG) */
} ;

/** ============================================================================
 *  @name   LDRV_MSGQ_TransportObj_tag
 *
 *  @desc   This structure defines the common attributes of the transport
 *          object. There is one instance of the transport object per MQT
 *          in the system.
 *
 *  @field  mqtInterface
 *              Pointer to the function table of the MQT represented by the
 *              transport object.
 *  @field  object
 *              Transport-specific object.
 *  @field  dspId
 *              Processor identifier.
 *  ============================================================================
 */
struct LDRV_MSGQ_TransportObj_tag {
    MQT_Interface *     mqtInterface ;
    Pvoid               object ;
    ProcessorId         dspId ;
} ;

/** ============================================================================
 *  @name   LDRV_MQT_Config
 *
 *  @desc   This structure defines the configuration information shared between
 *          the MQT instance on the GPP and DSP-sides.
 *
 *  @field  maxMsgSize
 *              Maximum size of message supported by MQT. May be -1 if there is
 *              no limit on maximum message size for the MQT.
 *  @field  ipsId
 *              ID of the IPS to be used (if any). A value of -1 indicates that
 *              no IPS is required by the MQT.
 *  @field  ipsEventNo
 *              IPS Event number associated with MQT (if any). A value of -1
 *              indicates that no IPS is required by the MQT.
 *  @field  arg1
 *              First optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  @field  arg2
 *              Second optional argument for this MQT.
 *              The significance of this argument is specific to the MQT.
 *  ============================================================================
 */
typedef struct LDRV_MQT_Config_tag {
    Uint32             maxMsgSize ;
    Uint32             ipsId ;
    Uint32             ipsEventNo ;
    Uint32             arg1 ;
    Uint32             arg2 ;
} LDRV_MQT_Config ;

/** ============================================================================
 *  @name   LDRV_MQT_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          LDRV_MQT component.
 *
 *  @field  dspAddr
 *              DSP address of the shared control structure for the MQT
 *              instance.
 *  @field  config
 *              Configuration structure for the MQT instance.
 *  @field  padding
 *              Padding for alignment.
 *  ============================================================================
 */
typedef struct LDRV_MQT_Ctrl_tag {
    Uint32             dspAddr ;
    LDRV_MQT_Config    config ;
    Uint16             padding [LDRV_MQT_CTRL_PADDING] ;
} LDRV_MQT_Ctrl ;


/** ============================================================================
 *  @func   LDRV_MQT_moduleInit
 *
 *  @desc   This function initializes the LDRV_MQT module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MQT_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_MQT_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MQT module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MQT_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_MQT_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the MQT component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MQT_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_MQT_exit
 *
 *  @desc   This function finalizes the LDRV MQT module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MQT_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MQT_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_locateLocal
 *
 *  @desc   This function locates a local message queue identified by
 *          the specified MSGQ name and returns a handle to the located message
 *          queue if found.
 *
 *  @arg    queueName
 *              Name of the message queue to be located.
 *  @arg    msgqQueue
 *              Location to store the handle to the located message queue.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTFOUND
 *              The specified message queue could not be located.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  queueName must be valid.
 *          msgqQueue must be a valid pointer.
 *
 *  @leave  On success, the msgqQueue returned must not be MSGQ_INVALIDMSGQ.
 *          On failure, the msgqQueue returned must be MSGQ_INVALIDMSGQ.
 *
 *  @see    MSGQ_Queue
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_locateLocal (IN Pstr queueName, OUT MSGQ_Queue * msgqQueue) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_sendErrorMsg
 *
 *  @desc   This function sends an asynchronous error message of a particular
 *          type to the user-defined error handler MSGQ.
 *
 *  @arg    errorType
 *              Type of the error.
 *  @arg    arg1
 *              First argument dependent on the error type.
 *  @arg    arg2
 *              Second argument dependent on the error type.
 *
 *  @ret    DSP_SOK
 *              The error message has been successfully sent.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  The component must be initialized.
 *
 *  @leave  None
 *
 *  @see    MSGQ_MqtError, MSGQ_AsyncErrorMsg, LDRV_MSGQ_setErrorHandler ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_sendErrorMsg (IN  MSGQ_MqtError  errorType,
                        IN  Pvoid          arg1,
                        IN  Pvoid          arg2) ;


/** ============================================================================
 *  @func   LDRV_MSGQ_notImpl
 *
 *  @desc   This function should be used in interface tables where some
 *          functions are not being implemented.
 *
 *  @arg    None
 *
 *  @ret    DSP_ENOTIMPL
 *              This function is not implemented.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MSGQ_notImpl (Void) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_MQT_H) */
