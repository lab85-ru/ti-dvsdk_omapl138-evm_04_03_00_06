/** ============================================================================
 *  @file   ringiodefs.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Global definitions for RingIO module
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


#if !defined (RINGIODEFS_H)
#define RINGIODEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* Success code for RingIO component */
#define RINGIO_SUCCESS              (SYS_OK)
#define RINGIO_BASE                 (SYS_EUSER)

/* Indicates that either: 1) The amount of data requested could not be
                             serviced due to the presence of an attribute
                          2) During an attribute read if another is also present
                             at the same offset  */
#define RINGIO_SPENDINGATTRIBUTE    (RINGIO_BASE + 0x1)

/* Generic RingIO error code */
#define RINGIO_EFAILURE             (RINGIO_BASE + 0x2)

/* Indicates that the amount of data requested could not be serviced due to the
   ring buffer getting wrapped */
#define RINGIO_EBUFWRAP             (RINGIO_BASE + 0x3)

/* Indicates that there is no data in the buffer for reading */
#define RINGIO_EBUFEMPTY            (RINGIO_BASE + 0x4)

/* Indicates that the buffer is full */
#define RINGIO_EBUFFULL             (RINGIO_BASE + 0x5)

/* Indicates that there is no attribute at the current, but attributes are
   present at a future offset */
#define RINGIO_EPENDINGDATA         (RINGIO_BASE + 0x6)

/* Indicates that attibute get() failed, need to extract variable length message
   getv() */
#define RINGIO_EVARIABLEATTRIBUTE   (RINGIO_BASE + 0x7)

/* Indicates that the RingIO being created already exists */
#define RINGIO_EALREADYEXISTS       (RINGIO_BASE + 0x8)

/* Indicates that Valid data is present in the buffer but is not contiguous as
 * data has been copied from top to footer area.
 */
#define RINGIO_ENOTCONTIGUOUSDATA   (RINGIO_BASE + 0x9)

/* Indicates that the RingIO is in a wrong state */
#define RINGIO_EWRONGSTATE          (RINGIO_BASE + 0xA)

/** ============================================================================
 *  @macro  RINGIO_NAME_MAX_LEN
 *
 *  @desc   Maximum length of RingIO strings.
 *  ============================================================================
 */
#define RINGIO_NAME_MAX_LEN         DSP_MAX_STRLEN

/*  ============================================================================
 *  @const  RINGIO_INVALID_ATTR
 *
 *  @desc   This constant denotes an invalid attribute type.
 *          This should not be used for any valid attribute.
 *  ============================================================================
 */
#define RINGIO_INVALID_ATTR  (Uint16) 0xFFFF

/** ============================================================================
 *  @const  RINGIO_DATABUF_CACHEUSE, RINGIO_ATTRBUF_CACHEUSE,
 *          RINGIO_CONTROL_CACHEUSE, RINGIO_NEED_EXACT_SIZE
 *
 *  @desc   These constants denote the flags provided while opening the RingIO.
 *  ============================================================================
 */
#define RINGIO_DATABUF_CACHEUSE 0x1
#define RINGIO_ATTRBUF_CACHEUSE 0x2
#define RINGIO_CONTROL_CACHEUSE 0x4
#define RINGIO_NEED_EXACT_SIZE  0x8


/** ============================================================================
 *  @name   RingIO_NotifyType
 *
 *  @desc   Enumeration of RingIO notification types.
 *
 *  @field  RINGIO_NOTIFICATION_NONE
 *              No notification required.
 *  @field  RINGIO_NOTIFICATION_ALWAYS
 *              Notify whenever the other side sends data/frees up space.
 *  @field  RINGIO_NOTIFICATION_ONCE
 *              Notify when the other side sends data/frees up space. Once the
 *              notification is done, the notification is disabled until it is
 *              enabled again
 *  @field  RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS
 *              Notify whenever the other side sends data/frees up space.
 *              This notification is never disabled.
 *  @field  RINGIO_NOTIFICATION_HDWRFIFO_ONCE
 *              Notify when the other side sends data/frees up space. Once the
 *              notification is done, the notification is disabled until it is
 *              enabled again. The notification is enabled once the watermark
 *               is crossed and does not require buffer to get full/empty.
 *  ============================================================================
 */
typedef enum {
    RINGIO_NOTIFICATION_NONE = 0,
    RINGIO_NOTIFICATION_ALWAYS,
    RINGIO_NOTIFICATION_ONCE,
    RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS,
    RINGIO_NOTIFICATION_HDWRFIFO_ONCE
} RingIO_NotifyType ;

/** ============================================================================
 *  @name   RingIO_OpenMode
 *
 *  @desc   Enumeration of RingIO open modes.
 *
 *  @field  RINGIO_MODE_READ
 *              Reader mode.
 *  @field  RINGIO_MODE_WRITE
 *              Writer mode.
 *  ============================================================================
 */
typedef enum{
    RINGIO_MODE_READ = 0,
    RINGIO_MODE_WRITE
} RingIO_OpenMode ;

/** ============================================================================
 *  @name   RingIO_TransportType
 *
 *  @desc   Enumeration of RingIO transport types.
 *
 *  @field  RINGIO_TRANSPORT_DSP_DSP
 *              Intra-DSP transport.
 *  @field  RINGIO_TRANSPORT_GPP_DSP
 *              GPP<->DSP transport.
 *  ============================================================================
 */
typedef enum{
    RINGIO_TRANSPORT_DSP_DSP = 1,
    RINGIO_TRANSPORT_GPP_DSP
} RingIO_TransportType ;


/** ============================================================================
 *  @name   RingIO_Handle
 *
 *  @desc   This type is used for the handle to the RingIO Client structure.
 *  ============================================================================
 */
typedef Ptr                                     RingIO_Handle ;

/** ============================================================================
 *  @name   RingIO_ControlHandle
 *
 *  @desc   This type is used for the handle to the Shared Control structure.
 *  ============================================================================
 */
typedef struct  RingIO_ControlStruct_tag *      RingIO_ControlHandle ;

/** ============================================================================
 *  @name   RingIO_BufPtr
 *
 *  @desc   This type is used for the Buffer Pointer Type.
 *  ============================================================================
 */
typedef Ptr                                     RingIO_BufPtr ;

/** ============================================================================
 *  @name   RingIO_NotifyParam
 *
 *  @desc   This type is used for the Notification Parameter.
 *  ============================================================================
 */
typedef Ptr                                     RingIO_NotifyParam ;

/** ============================================================================
 *  @name   RingIO_NotifyMsg
 *
 *  @desc   This type is used for the Notification  Message.
 *  ============================================================================
 */
typedef Uint16  RingIO_NotifyMsg;

/** ============================================================================
 *  @name   RingIO_NotifyFunc
 *
 *  @desc   Signature of function used as the Notification function.
 *
 *  @arg    handle
 *              Handle to the RingIO.
 *  @arg    param
 *              Parameter used while registering the notification.
 *  @arg    msg
 *               Message passed along with notification.
 *
 *  @ret    None.
 *  ============================================================================
 */
typedef Void (* RingIO_NotifyFunc) (RingIO_Handle handle,
                                    RingIO_NotifyParam param,
                                    RingIO_NotifyMsg msg) ;


/** ============================================================================
 *  @name   RingIO_Attrs
 *
 *  @desc   This structure defines the RingIO Creation Parameters.
 *
 *  @field  transportType
 *              Transport type - This specifies whether the data transport is
 *              between DSP<-> DSP or DSP<->ARM.
 *  @field  ctrlPoolId
 *              Pool to be used to allocate memory for control structure.
 *  @field  dataPoolId
 *              Pool to be used to allocate memory for data buffer.
 *  @field  attrPoolId
 *              Pool to be used to allocate memory for attribute buffer.
 *  @field  lockPoolId
 *              Pool to be used to allocate memory for lock structure.
 *  @field  dataBufSize
 *              Data Buffer Size in bytes.
 *  @field  footBufSize
 *              Footer area for providing contiguous buffer to a reader.
 *  @field  attrBufSize
 *              Attribute buffer size in bytes.
 *  ============================================================================
 */
typedef struct RingIO_Attrs_tag {
    RingIO_TransportType   transportType ;
    Uint16                 ctrlPoolId  ;
    Uint16                 dataPoolId  ;
    Uint16                 attrPoolId  ;
    Uint16                 lockPoolId  ;
    Uint32                 dataBufSize ;
    Uint32                 footBufSize ;
    Uint32                 attrBufSize ;
} RingIO_Attrs ;

/** ============================================================================
 *  @name   RingIO_ControlStruct
 *
 *  @desc   Forward declaration of RingIO_ControlStruct.
 *  ============================================================================
 */
typedef struct RingIO_ControlStruct_tag RingIO_ControlStruct ;

/** ============================================================================
 *  @name   RingIO_Client
 *
 *  @desc   This structure defines the RingIO Shared memory client structure.
 *          The RingIO Reader or Writer state information is stored in this
 *          structure
 *
 *  @field  procId
 *              Processor Id where the client is executing
 *  @field  openMode
 *              Indicates whether the client is a reader or writer
 *  @field  pDataStart
 *              Virtual start address of the data buffer
 *  @field  pAttrStart
 *              Virtual start address of the attr buffer
 *  @field  acqStart
 *              Start offset of data buffer that has been acquired by the
 *              application.
 *  @field  acqSize
 *              Size of data that has been acquired
 *  @field  acqAttrStart
 *              Start offset of the acquired attribute buffer
 *  @field  acqAttrSize
 *              Size of attribute data that has been acquired
 *  @field  notifyType
 *              Notification type
 *  @field  notifyFunc
 *              Notification function for this client
 *  @field  notifyParam
 *              Parameter to the Notification function
 *  @field  notifyWaterMark
 *              Watermark that should be satisfied before notification is done
 *  @field  flags
 *              Counting bits from MSB to LSB 0....31
 *              B31 - Denotes whether the client uses cache for accessing the
 *                    data buffer (dataBufCacheUseFlag)
 *              B30 - Denotes whether the client uses cache for accessing the
 *                    attribute buffer (attrBufCacheUseFlag)
 *              B29 - Denotes whether the client uses cache for accessing the
 *                    control struct (controlStructCacheFlag)
 *              B28 - Denotes whether requests for acquire buffer should always
 *                    be satisfied. If a contiguous request could not be
 *                    satisfied because of the buffer wrap, we need to mark an
 *                    early end and provide only a contiguous buffer
 *                    (contiguousBufFlag)
 *  @field  virtControlHandle
 *              Handle to the Control structure. Apps do not have direct access
 *              to the control structure. The Control structure can only be
 *              accessed through the client handle
 *  @field  virtLockHandle
 *              Virtual (GPP) address of the lock that should be used to protect
 *              the Control structure from multiple accesses.
 *  @field  isValid
 *              Indicates whether the Client is initialized
 *  @field  notifyFlag
 *              Denotes whether notification needs to be done or not
 *  @field  refCount
 *              Denotes whether ringio has been re-opened or not
 *  @field  padding
 *              Padding.
 *  ============================================================================
 */
typedef struct RingIO_Client_tag {
    Uint32                 procId ;
    Uint32                 openMode ;
    RingIO_BufPtr          pDataStart ;
    RingIO_BufPtr          pAttrStart ;
    Uint32                 acqStart ;
    Uint32                 acqSize ;
    Uint32                 acqAttrStart ;
    Uint32                 acqAttrSize ;
    Uint32                 notifyType ;
    RingIO_NotifyFunc      notifyFunc ;
    RingIO_NotifyParam     notifyParam ;
    Uint32                 notifyWaterMark ;
    Uint32                 flags ;
    RingIO_ControlStruct * virtControlHandle ;
    Void *                 virtLockHandle;
    Uint32                 isValid ;
    Uint32                 refCount ;
    Uint16                 notifyFlag ;
    ADD_PADDING            (padding, RINGIO_CLIENT_PADDING)
} RingIO_Client ;

/** ============================================================================
 *  @name   RingIO_ControlStruct
 *
 *  @desc   This structure defines the RingIO Control Structure. This structure
 *          is stored in shared memory and is accessible by all clients. The
 *          control structure supports a single reader and a single writer for
 *          the ring buffer.
 *
 *  @field  procId 
 *              Processor Identifier.                     
 *  @field  entryId
 *              ID of the RingIO within the entry array.
 *  @field  transportType
 *              Transport type - This specifies whether the data transport is
 *              between DSP<-> DSP or DSP<->ARM.
 *  @field  phyBufStart
 *              Physical start address of the data buffer.
 *  @field  phyBufEnd
 *              Total size of the Data buffer (offset from phyBufStart)
 *  @field  curBufEnd
 *              Current buffer size. This may be <= dataBufEnd (offset from
 *              phyBufStart)
 *  @field  dataBufEnd
 *              Size of the main data buffer (offset from phyBufStart)
 *  @field  dataBufSize
 *              Size of the data buffer
 *  @field  footBufSize
 *              Size of the foot buffer used for providing linear buffer to a
 *              reader
 *  @field  validSize
 *              Amount of valid data available in the data buffer. Valid Data is
 *              the total data that is readable by the reader using an acquire
 *              call. This  does not include the size of the data buffer already
 *              acquired by the reader
 *  @field  emptySize
 *              Amount of empty space in the data buffer. This does not include
                the empty space already acquired by the writer
 *  @field  phyAttrStart
 *              Physical start address of the attr buffer
 *  @field  phyAttrBufEnd
 *              Total Size of the attribute buffer (offset)
 *  @field  curAttrBufEnd
 *              Current Attr buffer size. This may be <= the phyAttrBufEnd
 *              (offset)
 *  @field  validAttrSize
 *              Amount of valid attribute bytes available in the attribute
 *              buffer. The valid attribute bytes does not include the attribute
 *              bytes already acquired by the reader
 *  @field  emptyAttrSize
 *              Amount of empty space in the attr buffer. This does not include
 *              the empty attr space already acquired by the writer
 *  @field  prevAttrOffset
 *              Offset of the most recent attribute
 *  @field  phyLockHandle
 *              Physical (DSP) address of the lock that should be used to
 *              protect the Control structure from multiple accesses.
 *  @field  padding
 *              Padding.
 *  @field  writer
 *              Writer state information
 *  @field  reader
 *              Reader state information
 *  ============================================================================
 */
struct RingIO_ControlStruct_tag {
    Uint32                procId ;
    Uint32                entryId;
    Uint32                transportType;
    RingIO_BufPtr         phyBufStart;
    Uint32                phyBufEnd;
    Uint32                curBufEnd;
    Uint32                dataBufEnd;
    Uint32                dataBufSize;
    Uint32                footBufSize;
    Uint32                validSize;
    Uint32                emptySize;
    RingIO_BufPtr         phyAttrStart;
    Uint32                phyAttrBufEnd;
    Uint32                curAttrBufEnd;
    Uint32                validAttrSize;
    Uint32                emptyAttrSize;
    Int32                 prevAttrOffset;
    Void *                phyLockHandle;
    ADD_PADDING           (padding, RINGIO_CONTROLSTRUCT_PADDING)
    RingIO_Client         writer;
    RingIO_Client         reader;
} ;

/** ============================================================================
 *  @name   RingIO_Entry
 *
 *  @desc   Entry structure for the RingIO data transport
 *
 *  @field  name
 *              System wide unique identifier for the RingIO
 *  @field  phyControl
 *              Physical (DSP) address of the Control structure for the RingIO.
 *  @field  virtControl
 *              Virtual (GPP) address of the Control structure for the RingIO.
 *  @field  ownerProcId
 *              Creator's processor ID of this ringio.
 *  @field  ctrlPoolId
 *              Pool to be used to allocate memory for control structure.
 *  @field  dataPoolId
 *              Pool to be used to allocate memory for data buffer.
 *  @field  attrPoolId
 *              Pool to be used to allocate memory for attribute buffer.
 *  @field  lockPoolId
 *              Pool to be used to allocate memory for lock structure.
 *  @field  padding
 *              Padding.
 *  ============================================================================
 */
typedef struct RingIO_Entry_tag {
    Ptr          phyControl ;
    Ptr          virtControl ;
    Char         name [RINGIO_NAME_MAX_LEN] ;
    Uint16       ownerProcId ;
    Uint16       ctrlPoolId ;
    Uint16       dataPoolId ;
    Uint16       attrPoolId ;
    Uint16       lockPoolId ;
    ADD_PADDING  (padding, RINGIO_ENTRY_PADDING)
} RingIO_Entry ;

/** ============================================================================
 *  @name   RingIO_Ctrl
 *
 *  @desc   This structure defines the control structure required by the
 *          RINGIO component. It contains information about all RINGIO objects
 *          shared between the GPP and a specific DSP.
 *
 *  @field  isInitialized
 *              Flag to indicate if this region was initialized
 *  @field  dspId
 *              ID of the DSP with which the RingIO Region is shared
 *  @field  maxEntries
 *              Maximum number of RingIO instances supported by the RingIO.
 *  @field  ipsId
 *              ID of the IPS to be used.
 *  @field  ipsEventNo
 *              IPS Event number associated with the RingIO.
 *  @field  dspAddrEntry
 *              Pointer to array in DSP address space of RINGIO objects that can
 *              be created.
 *  @field  padding
 *              Padding.
 *  @field  lockObj
 *              Lock used to protect the shared RingIO_Ctrl from multiple
 *              simultaneous accesses.
 *  ============================================================================
 */
typedef struct RingIO_Ctrl_tag {
    Uint32         isInitialized ;
    Uint32         dspId ;
    Uint32         maxEntries ;
    Uint32         ipsId ;
    Uint32         ipsEventNo ;
    RingIO_Entry * dspAddrEntry ;
    ADD_PADDING    (padding, RINGIO_CTRL_PADDING)
    MPCS_ShObj     lockObj ;
} RingIO_Ctrl ;


/** ============================================================================
 *  @macro  GET_ENTRY_ID
 *
 *  @desc   Gets the entry id of the RingIO
 *  ============================================================================
 */
#define GET_ENTRY_ID(client)   ((client)->virtControlHandle->entryId)

/** ============================================================================
 *  @macro  GET_LOCK_POOL_ID
 *
 *  @desc   Gets the pool id from which the MPCS lock object structure has been
 *          allocated.
 *  ============================================================================
 */
#define GET_LOCK_POOL_ID(entryId)   (RingIO_ctrlPtr->entry [entryId].lockPoolId)

/** ============================================================================
 *  @macro  GET_CTRL_POOL_ID
 *
 *  @desc   Gets the pool id from which the RingIO control structure has been
 *          validated.
 *  ============================================================================
 */
#define GET_CTRL_POOL_ID(entryId) (RingIO_ctrlPtr->entry [entryId].ctrlPoolId)

/** ============================================================================
 *  @macro  GET_DATA_POOL_ID
 *
 *  @desc   Gets the pool id from which the RingIO data buffer has been
 *          allocated.
 *  ============================================================================
 */
#define GET_DATA_POOL_ID(entryId)   (RingIO_ctrlPtr->entry [entryId].dataPoolId)

/** ============================================================================
 *  @macro  GET_ATTR_POOL_ID
 *
 *  @desc   Gets the pool id from which the RingIO attr buffer has been
 *          allocated.
 *  ============================================================================
 */
#define GET_ATTR_POOL_ID(entryId)   (RingIO_ctrlPtr->entry [entryId].attrPoolId)

/** ============================================================================
 *  @macro  IS_WRITER
 *
 *  @desc   Check if client is a writer.
 *  ============================================================================
 */
#define IS_WRITER(client)            ((client)->openMode == RINGIO_MODE_WRITE)

/** ============================================================================
 *  @macro  IS_READER
 *
 *  @desc   Check if client is a reader.
 *  ============================================================================
 */
#define IS_READER(client)           ((client)->openMode == RINGIO_MODE_READ)

/** ============================================================================
 *  @macro  GET_CUR_ATTR_END
 *
 *  @desc   Get Current attr buffer end.
 *  ============================================================================
 */
#define GET_CUR_ATTR_END(control)   ((control)->curAttrBufEnd + 1)

/** ============================================================================
 *  @macro  GET_CUR_DATA_END
 *
 *  @desc   Get Current data buffer end.
 *  ============================================================================
 */
#define GET_CUR_DATA_END(control)   ((control)->curBufEnd + 1)

/** ============================================================================
 *  @macro  ADJUST_MODULO
 *
 *  @desc   Modulo operation
 *  ============================================================================
 */
#define ADJUST_MODULO(start, size, bufSize)   (((start) + (size)) % bufSize)

/** ============================================================================
 *  @macro  SIZEOF_ATTR
 *
 *  @desc   Size of RingIO attributes
 *  ============================================================================
 */
#define SIZEOF_ATTR(size)                  (sizeof (RingIO_Attr) + (size))

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* if !defined (RINGIODEFS_H) */
