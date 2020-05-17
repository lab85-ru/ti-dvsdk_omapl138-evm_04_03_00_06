/** ============================================================================
 *  @file   zcpy_mqt.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MQT/
 *
 *  @desc   Declarations for the Zero-Copy Message Queue transport for GPP-DSP
 *          communication.
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


#if !defined (ZCPY_MQT_H)
#define ZCPY_MQT_H

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <msgqdefs.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <mpcs.h>
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_mqt.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  ZCPYMQT_CTRLCMD_LOCATE
 *
 *  @desc   Defines the control command msg ID for location of a remote MSGQ.
 *  ============================================================================
 */
#define ZCPYMQT_CTRLCMD_LOCATE    MSGQ_MQTMSGIDSSTART

/** ============================================================================
 *  @const  ZCPYMQT_CTRLCMD_LOCATEACK
 *
 *  @desc   Defines the control command msg ID for acknowledgement for location
 *          of a remote MSGQ.
 *  ============================================================================
 */
#define ZCPYMQT_CTRLCMD_LOCATEACK (MSGQ_MQTMSGIDSSTART + 1u)

/** ============================================================================
 *  @const  ZCPYMQT_LIST_PADLENGTH
 *
 *  @desc   Padding required for DSP L2 cache line alignment within SHMIPS
 *          control objects for aligning a LIST element.
 *  ============================================================================
 */
#define ZCPYMQT_LIST_PADLENGTH  ((CACHE_L2_LINESIZE - sizeof (List)) / 2u)


/** ============================================================================
 *  @name   ZCPYMQT_Ctrl
 *
 *  @desc   Defines the ZCPYMQT object, which contains all the control structure
 *          shared between GPP and DSP..
 *
 *  @field  isOpen
 *              Value indicating whether GPP has opened its transport or not.
 *  @field  padding
 *              Padding field for isOpen member.
 *  @field  toDspList
 *              Holds the list of buffers to be sent to the DSP.
 *  @field  toDspPadding
 *              Padding for alignment.
 *  @field  fmDspList
 *              Holds the list of buffers to be received from the DSP.
 *  @field  fmDspPadding
 *              Padding for alignment.
 *  @field  csToDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the toDspList.
 *  @field  csFmDspList
 *              Shared critical section object for protection of operations by
 *              the two processors on the fmDspList.
 *  ============================================================================
 */
typedef struct ZCPYMQT_Ctrl_tag {
    volatile Uint16      isOpen    ;
    ADD_PADDING (padding, DSPLINK_16BIT_PADDING)

    volatile List       toDspList ;
             Uint16     toDspPadding [ZCPYMQT_LIST_PADLENGTH] ;
    volatile List       fmDspList ;
             Uint16     fmDspPadding [ZCPYMQT_LIST_PADLENGTH] ;
    volatile MPCS_ShObj csToDspList ;
    volatile MPCS_ShObj csFmDspList ;
} ZCPYMQT_Ctrl ;

/** ============================================================================
 *  @name   ZCPYMQT_CtrlMsg
 *
 *  @desc   Defines the format of the control messages that are sent between the
 *          ZCPY MQTs on different processors.
 *          This structure is common between the GPP and the DSP, and must be
 *          maintained as the same on both. To ensure this, padding must be
 *          added as required within the structure.
 *
 *  @field  msgHeader
 *              Fixed message header required for all messages.
 *  @field  ctrlMsg
 *              Defines the format of the different control messages.
 *              locateMsg:    semHandle   -> Semaphore handle for sync locate
 *                            replyQueue  -> Reply MSGQ handle for async locate
 *                            arg         -> User-defined value passed to locate
 *                            poolId      -> Pool ID to alloc async response msg
 *                            padding     -> Padding for alignment.
 *                            msgqName    -> Name of the MSGQ to be located on
 *                                           the remote processor.
 *              locateAckMsg: semHandle   -> Semaphore handle for sync locate
 *                            replyQueue  -> Reply MSGQ handle for async locate
 *                            arg         -> User-defined value passed to locate
 *                            poolId      -> Pool ID to alloc async response msg
 *                            padding     -> Padding for alignment.
 *                            msgqQueue   -> Handle to the MSGQ located on the
 *                                           remote processor.
 *  ============================================================================
 */
typedef struct ZCPYMQT_CtrlMsg_tag {
    MSGQ_MsgHeader msgHeader ;
    union {
        struct {
            Uint32      semHandle ;
            Uint32      replyQueue;
            Uint32      arg;
            Uint16      poolId;
            Uint16      padding;
            Uint16      msgqName [DSP_MAX_STRLEN] ;
       } locateMsg ;

        struct {
            Uint32      semHandle ;
            Uint32      replyQueue;
            Uint32      arg;
            Uint16      poolId;
            Uint16      padding;
            Uint32      msgqQueue ;
        } locateAckMsg ;
    } ctrlMsg ;
} ZCPYMQT_CtrlMsg ;


/** ============================================================================
 *  @func   ZCPYMQT_init
 *
 *  @desc   This function initializes the ZCPY MQT.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    mqtId
 *              MQT Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the ZCPY MQT component.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_init (IN  ProcessorId dspId,
              IN  Uint32      mqtId,
              OUT Uint32 *    shDspAddr) ;


/** ============================================================================
 *  @func   ZCPYMQT_exit
 *
 *  @desc   This function finalizes the ZCPY MQT.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    mqtId
 *              MQT Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be a valid processor ID.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_exit (IN ProcessorId dspId,
              IN Uint32      mqtId) ;


/** ============================================================================
 *  @func   ZCPYMQT_open
 *
 *  @desc   This function opens the ZCPY MQT and configures it according to
 *          the user attributes.
 *
 *  @arg    mqtHandle
 *              Handle to the transport object.
 *  @arg    mqtAttrs
 *              Attributes required for initialization of the MQT component.
 *
 *  @ret    DSP_SOK
 *              The ZCPY MQT has been successfully opened.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mqtHandle must be valid.
 *          mqtAttrs must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_Object, ZCPYMQT_Attrs, ZCPYMQT_close ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_open (IN  LDRV_MSGQ_TransportHandle mqtHandle, IN  Pvoid mqtAttrs) ;


/** ============================================================================
 *  @func   ZCPYMQT_close
 *
 *  @desc   This function closes the ZCPY MQT, and cleans up its state object.
 *
 *  @arg    mqtHandle
 *              Handle to the transport object.
 *
 *  @ret    DSP_SOK
 *              The ZCPY MQT has been successfully closed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mqtHandle must be valid.
 *
 *  @leave  The object field in the MSGQ transport object is set to NULL.
 *
 *  @see    ZCPYMQT_Object, ZCPYMQT_open ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_close (IN  LDRV_MSGQ_TransportHandle mqtHandle) ;


/** ============================================================================
 *  @func   ZCPYMQT_locate
 *
 *  @desc   This function locates the message queue identified by the specified
 *          MSGQ name
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MSGQ transport object.
 *  @arg    queueName
 *              Name of the message queue to be located.
 *  @arg    sync
 *              Indicates whether the location is synchronous.
 *  @arg    msgqQueue
 *              If synchronous:  indicates the location to store the handle to
 *                               the located message queue.
 *              If asynchronous: indicates the message queue to be used to
 *                               receive the response message for location.
 *  @arg    locateAttrs
 *              If synchronous:  indicates the attributes for synchronous
 *                               location of the MSGQ.
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
 *
 *  @enter  mqtHandle must be valid.
 *          queueName must be valid.
 *          msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_release ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_locate (IN     LDRV_MSGQ_TransportHandle mqtHandle,
                IN     Pstr                      queueName,
                IN     Bool                      sync,
                IN OUT MSGQ_Queue *              msgqQueue,
                IN     Pvoid                     locateAttrs) ;


/** ============================================================================
 *  @func   ZCPYMQT_release
 *
 *  @desc   This function releases the MSGQ identified by the MSGQ handle that
 *          was located earlier.
 *
 *  @arg    mqtHandle
 *              Handle to the LDRV MSGQ transport object.
 *  @arg    msgqQueue
 *              Handle to the message queue to be released.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mqtHandle must be valid.
 *          msgqQueue must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_locate ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_release (IN LDRV_MSGQ_TransportHandle mqtHandle,
                 IN MSGQ_Queue                msgqQueue) ;


/** ============================================================================
 *  @func   ZCPYMQT_put
 *
 *  @desc   This function sends a message to the specified MSGQ.
 *
 *  @arg    mqtHandle
 *              Handle to LDRV MSGQ transport object.
 *  @arg    msg
 *              Pointer to the message to be sent.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mqtHandle must be valid.
 *          msg must be valid.
 *
 *  @leave  None
 *
 *  @see    ZCPYMQT_putCallback
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
ZCPYMQT_put (IN LDRV_MSGQ_TransportHandle mqtHandle,
             IN MSGQ_Msg                  msg) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   ZCPYMQT_debug
 *
 *  @desc   This function prints debug information about the MQT.
 *
 *  @arg    mqtHandle
 *              Handle to LDRV MSGQ transport object.
 *
 *  @ret    None.
 *
 *  @enter  mqtHandle must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
ZCPYMQT_debug (IN LDRV_MSGQ_TransportHandle mqtHandle) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (ZCPY_MQT_H) */
