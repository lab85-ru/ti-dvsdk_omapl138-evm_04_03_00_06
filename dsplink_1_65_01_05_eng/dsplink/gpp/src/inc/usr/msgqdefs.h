/** ============================================================================
 *  @file   msgqdefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines data types and structures used by DSP/BIOS(tm) Link for
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


#if !defined (MSGQDEFS_H)
#define MSGQDEFS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <gpptypes.h>
#include <errbase.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   MSGQ_MqtError
 *
 *  @desc   This type is used for identifying types of MQT async error messages.
 *  ============================================================================
 */
typedef Int16    MSGQ_MqtError ;

/** ============================================================================
 *  @name   MSGQ_Id
 *
 *  @desc   This type is used for identifying a message queue on a specific
 *          processor.
 *  ============================================================================
 */
typedef Uint16   MSGQ_Id ;

/** ============================================================================
 *  @name   MSGQ_Queue
 *
 *  @desc   This type is used for identifying a message queue across processors.
 *  ============================================================================
 */
typedef Uint32   MSGQ_Queue ;


/** ============================================================================
 *  @const  MSGQ_INVALIDMSGQ
 *
 *  @desc   This constant denotes an invalid message queue.
 *  ============================================================================
 */
#define MSGQ_INVALIDMSGQ      (Uint16) 0xFFFFu

/** ============================================================================
 *  @const  MSGQ_INVALIDPROCID
 *
 *  @desc   This constant denotes an invalid processor ID.
 *  ============================================================================
 */
#define MSGQ_INVALIDPROCID    (Uint16) 0xFFFFu

/** ============================================================================
 *  @const  MSGQ_INTERNAL_ID_START
 *
 *  @desc   This constant defines the start of internal MSGQ message ID range.
 *  ============================================================================
 */
#define MSGQ_INTERNALIDSSTART (Uint16) 0xFF00u

/** ============================================================================
 *  @const  MSGQ_ASYNCLOCATEMSGID
 *
 *  @desc   This constant defines the asynchronous locate message ID.
 *  ============================================================================
 */
#define MSGQ_ASYNCLOCATEMSGID (Uint16) 0xFF00u

/** ============================================================================
 *  @const  MSGQ_ASYNCERRORMSGID
 *
 *  @desc   This constant defines the asynchronous error message ID.
 *  ============================================================================
 */
#define MSGQ_ASYNCERRORMSGID (Uint16) 0xFF01u

/** ============================================================================
 *  @const  MSGQ_INTERNALIDSEND
 *
 *  @desc   This constant defines the end of internal MSGQ message ID range.
 *  ============================================================================
 */
#define MSGQ_INTERNALIDSEND (Uint16) 0xFF7fu

/** ============================================================================
 *  @const  MSGQ_MQTMSGIDSSTART
 *
 *  @desc   This constant defines the start of transport message ID range.
 *  ============================================================================
 */
#define MSGQ_MQTMSGIDSSTART (Uint16) 0xFF80u

/** ============================================================================
 *  @const  MSGQ_MQTMSGIDSEND
 *
 *  @desc   This constant defines the end of transport message ID range.
 *  ============================================================================
 */
#define MSGQ_MQTMSGIDSEND (Uint16) 0xFFFEu

/** ============================================================================
 *  @const  MSGQ_INVALIDMSGID
 *
 *  @desc   This constant is used to denote no message ID value.
 *  ============================================================================
 */
#define MSGQ_INVALIDMSGID (Uint16) 0xFFFFu

/** ============================================================================
 *  @const  MSGQ_MQTERROREXIT
 *
 *  @desc   In an asynchronous error message, this value as the error type
 *          indicates that remote MQT has called exit.
 *  ============================================================================
 */
#define MSGQ_MQTERROREXIT  (MSGQ_MqtError) -1l

/** ============================================================================
 *  @const  MSGQ_MQTFAILEDPUT
 *
 *  @desc   In an asynchronous error message, this value as the error type
 *          indicates that the transport failed to send a message to the remote
 *          processor.
 *  ============================================================================
 */
#define MSGQ_MQTFAILEDPUT  (MSGQ_MqtError) -2l

/** ============================================================================
 *  @const  MSG_HEADER_RESERVED_SIZE
 *
 *  @desc   This macro defines the size of the reserved field of message header.
 *  ============================================================================
 */
#define MSG_HEADER_RESERVED_SIZE    2u

/** ============================================================================
 *  @macro  IS_VALID_MSGQ
 *
 *  @desc   Is the MSGQ valid.
 *  ============================================================================
 */
#define IS_VALID_MSGQ(msgq)        (msgq != MSGQ_INVALIDMSGQ)


/** ============================================================================
 *  @name   MsgqPend
 *
 *  @desc   Defines the types for the pend function.
 *
 *  @arg    notifyHandle
 *              Pointer to the notification object for the message queue.
 *  @arg    timeout
 *              Timeout Value.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          SYNC_E_FAIL
 *              General error from GPP-OS.
 *          DSP_ETIMEOUT
 *              Timeout occured while performing operation.
 *          DSP_EPOINTER
 *              Invalid pointer passed.
 *  ============================================================================
 */
typedef DSP_STATUS (*MsgqPend) (Pvoid notifyHandle, Uint32 timeout) ;


/** ============================================================================
 *  @name   MsgqPost
 *
 *  @desc   Defines the types for the post function.
 *
 *  @arg    notifyHandle
 *              Pointer to the notification object for the message queue.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          SYNC_E_FAIL
 *              General error from GPP-OS.
 *          DSP_EPOINTER
 *              Invalid pointer passed.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *  ============================================================================
 */
typedef DSP_STATUS (*MsgqPost) (Pvoid notifyHandle) ;


/** ============================================================================
 *  @name   MSGQ_Attrs
 *
 *  @desc   This structure defines the attributes required during opening of
 *          the MSGQ.
 *
 *  @field  notifyHandle
 *              Pointer to the notification object for the message queue.
 *  @field  pend
 *              Function to be used to wait to receive a message.
 *  @field  post
 *              Function to be used to indicate arrival of a message.
 *  ============================================================================
 */
typedef struct MSGQ_Attrs_tag {
    Pvoid    notifyHandle ;
    MsgqPend pend ;
    MsgqPost post ;
} MSGQ_Attrs ;

/** ============================================================================
 *  @name   PCPYMQT_Attrs
 *
 *  @desc   This structure defines the attributes for initialization of the
 *          PCPY MQT.
 *
 *  @field  poolId
 *              Pool ID used for allocating control messages.
 *              This pool is also used in case the ID within the message
 *              received from the DSP is invalid.
 *  ============================================================================
 */
typedef struct PCPYMQT_Attrs_tag {
    PoolId    poolId ;
} PCPYMQT_Attrs ;

/** ============================================================================
 *  @name   ZCPYMQT_Attrs
 *
 *  @desc   This structure defines the attributes for initialization of the
 *          ZCPY MQT.
 *
 *  @field  poolId
 *              Pool ID used for allocating control messages.
 *  ============================================================================
 */
typedef struct ZCPYMQT_Attrs_tag {
    PoolId    poolId ;
} ZCPYMQT_Attrs ;

/** ============================================================================
 *  @name   DCPYMQT_Attrs
 *
 *  @desc   This structure defines the attributes for initialization of the
 *          DCPY MQT.
 *
 *  @field  poolId
 *              Pool ID used for allocating control messages.
 *  ============================================================================
 */
typedef struct DCPYMQT_Attrs_tag {
    PoolId    poolId ;
} DCPYMQT_Attrs ;

/** ============================================================================
 *  @name   MSGQ_LocateAttrs
 *
 *  @desc   This structure defines the attributes required during synchronous
 *          location of a MSGQ.
 *
 *  @field  timeout
 *              Timeout value in milliseconds for the locate calls.
 *  ============================================================================
 */
typedef struct MSGQ_LocateAttrs_tag {
    Uint32     timeout ;
} MSGQ_LocateAttrs ;

/** ============================================================================
 *  @name   MSGQ_LocateAsyncAttrs
 *
 *  @desc   This structure defines the attributes required during asynchronous
 *          location of a MSGQ.
 *
 *  @field  poolId
 *              ID of the pool to be used for allocating async locate messages.
 *  @field  arg
 *              User-defined argument returned with an async locate message.
 *  ============================================================================
 */
typedef struct MSGQ_LocateAsyncAttrs_tag {
    PoolId     poolId ;
    Pvoid      arg ;
} MSGQ_LocateAsyncAttrs ;

/** ============================================================================
 *  @name   MSGQ_MsgHeader
 *
 *  @desc   This structure defines the format of the message header that must be
 *          the first field of any message.
 *
 *  @field  reserved
 *              Reserved for use by the MQT. The MQT typically uses them as a
 *              link for queuing the messages.
 *  @field  srcProcId
 *              Processor Id for the source message queue
 *  @field  poolId
 *              ID of the Pool used for allocating this message.
 *  @field  size
 *              Size of the message including the header.
 *  @field  dstId
 *              ID of the destination message queue.
 *  @field  srcId
 *              ID of the source message queue for reply.
 *  @field  msgId
 *              User-specified message ID.
 *  ============================================================================
 */
typedef struct MSGQ_MsgHeader_tag {
    Uint32    reserved [MSG_HEADER_RESERVED_SIZE] ;
    Uint16    srcProcId    ;
    Uint16    poolId       ;
    Uint16    size         ;
    Uint16    dstId        ;
    Uint16    srcId        ;
    Uint16    msgId        ;
} MSGQ_MsgHeader ;

/** ============================================================================
 *  @name   MSGQ_AsyncLocateMsg
 *
 *  @desc   This structure defines the asynchronous locate message format.
 *
 *  @field  header
 *              Fixed message header required for all messages.
 *  @field  msgqQueue
 *              Reply message queue specified during MSGQ_locateAsync ().
 *  @field  arg
 *              User-defined argument specified as part of the
 *              MSGQ_LocateAsyncAttrs
 *  ============================================================================
 */
typedef struct MSGQ_AsyncLocateMsg_tag {
    MSGQ_MsgHeader  header ;
    MSGQ_Queue      msgqQueue ;
    Pvoid          arg ;
} MSGQ_AsyncLocateMsg ;

/** ============================================================================
 *  @name   MSGQ_AsyncErrorMsg
 *
 *  @desc   This structure defines the asynchronous error message format.
 *
 *  @field  header
 *              Fixed message header required for all messages.
 *  @field  errorType
 *              Type of error.
 *  @field  arg1
 *              First argument dependent on the error type.
 *              MSGQ_MQTERROREXIT : Processor ID of the transport.
 *              MSGQ_MQTFAILEDPUT : Handle of the destination message queue on
 *                                  which the put failed.
 *  @field  arg2
 *              Second argument dependent on the error type.
 *              MSGQ_MQTERROREXIT : Not used.
 *              MSGQ_MQTFAILEDPUT : Status of the MSGQ_put () call that failed.
 *  ============================================================================
 */
typedef struct MSGQ_AsyncErrorMsg_tag {
    MSGQ_MsgHeader  header ;
    MSGQ_MqtError   errorType ;
    Pvoid          arg1 ;
    Pvoid          arg2 ;
} MSGQ_AsyncErrorMsg ;

/** ============================================================================
 *  @name   MSGQ_Msg
 *
 *  @desc   This is the type of any buffer used for messaging.
 *  ============================================================================
 */
typedef MSGQ_MsgHeader * MSGQ_Msg ;

/** ============================================================================
 *  @name   MQT_Interface
 *
 *  @desc   Forward declaration of MQT interface table.
 *  ============================================================================
 */
typedef struct MQT_Interface_tag MQT_Interface ;


/** ============================================================================
 *  @deprecated The deprecated data structure MsgqMqtError has been replaced
 *              with MSGQ_MqtError
 *
 *  ============================================================================
 */
#define MsgqMqtError               MSGQ_MqtError

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqId has been replaced
 *              with MSGQ_Id
 *
 *  ============================================================================
 */
#define MsgqId                     MSGQ_Id

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqQueue has been replaced
 *              with MSGQ_Queue
 *
 *  ============================================================================
 */
#define MsgqQueue                  MSGQ_Queue

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqAttrs has been replaced
 *              with MSGQ_Attrs
 *
 *  ============================================================================
 */
#define MsgqAttrs                  MSGQ_Attrs

/** ============================================================================
 *  @deprecated The deprecated data structure PcpyMqtAttrs has been replaced
 *              with PCPYMQT_Attrs
 *
 *  ============================================================================
 */
#define PcpyMqtAttrs               PCPYMQT_Attrs

/** ============================================================================
 *  @deprecated The deprecated data structure ZcpyMqtAttrs has been replaced
 *              with ZCPYMQT_Attrs
 *
 *  ============================================================================
 */
#define ZcpyMqtAttrs               ZCPYMQT_Attrs

/** ============================================================================
 *  @deprecated The deprecated data structure DcpyMqtAttrs has been replaced
 *              with DCPYMQT_Attrs
 *
 *  ============================================================================
 */
#define DcpyMqtAttrs               DCPYMQT_Attrs

/** ============================================================================
 *  @deprecated The deprecated data structure MSGQ_LocateAttrs has been replaced
 *              with MSGQ_LocateAttrs
 *
 *  ============================================================================
 */
#define MsgqLocateAttrs            MSGQ_LocateAttrs

/** ============================================================================
 *  @deprecated The deprecated data structure MSGQ_LocateAsyncAttrs has been
 *              replaced with MSGQ_LocateAsyncAttrs
 *
 *  ============================================================================
 */
#define MsgqLocateAsyncAttrs       MSGQ_LocateAsyncAttrs

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqMsgHeader has been replaced
 *              with MSGQ_MsgHeader
 *
 *  ============================================================================
 */
#define MsgqMsgHeader              MSGQ_MsgHeader

/** ============================================================================
 *  @deprecated The deprecated data structure MSGQ_AsyncLocateMsg has been
 *              replaced with MSGQ_AsyncLocateMsg
 *
 *  ============================================================================
 */
#define MsgqAsyncLocateMsg         MSGQ_AsyncLocateMsg

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqAsyncErrorMsg has been
 *              replaced with MSGQ_AsyncErrorMsg
 *
 *  ============================================================================
 */
#define MsgqAsyncErrorMsg          MSGQ_AsyncErrorMsg

/** ============================================================================
 *  @deprecated The deprecated data structure MsgqMsg has been replaced
 *              with MSGQ_Msg
 *
 *  ============================================================================
 */
#define MsgqMsg                    MSGQ_Msg

/** ============================================================================
 *  @deprecated The deprecated data structure MqtInterface has been replaced
 *              with MQT_Interface
 *
 *  ============================================================================
 */
#define MqtInterface               MQT_Interface


#if defined (__cplusplus)
}
#endif


#endif /* !defined (MSGQDEFS_H) */
