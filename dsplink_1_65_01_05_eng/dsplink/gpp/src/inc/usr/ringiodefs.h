/** ============================================================================
 *  @file   ringiodefs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Global definitions for RingIO module
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


#if !defined (RINGIODEFS_H)
#define RINGIODEFS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <archdefs.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  RINGIO_INVALID_ATTR
 *
 *  @desc   This constant denotes an invalid attribute type.
 *          This should not be used for any valid attribute.
 *  ============================================================================
 */
#define RINGIO_INVALID_ATTR    (Uint16) 0xFFFFl

/** ============================================================================
 *  @const  RINGIO_DATABUF_CACHEUSE, RINGIO_ATTRBUF_CACHEUSE,
 *          RINGIO_ATTRBUF_CACHEUSE, RINGIO_NEED_EXACT_SIZE
 *
 *  @desc   These constants denote the flags provided while opening the RingIO.
 *  ============================================================================
 */
#define RINGIO_DATABUF_CACHEUSE 0x1u
#define RINGIO_ATTRBUF_CACHEUSE 0x2u
#define RINGIO_CONTROL_CACHEUSE 0x4u
#define RINGIO_NEED_EXACT_SIZE  0x8u

/** ============================================================================
 *  @macro  RINGIO_NAME_MAX_LEN
 *
 *  @desc   Maximum length of RingIO strings.
 *  ============================================================================
 */
#define RINGIO_NAME_MAX_LEN  32u


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
    RINGIO_MODE_READ = 0u,
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
    RINGIO_TRANSPORT_DSP_DSP = 1u,
    RINGIO_TRANSPORT_GPP_DSP
} RingIO_TransportType ;

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
 *
 *  ============================================================================
 */
typedef enum {
    RINGIO_NOTIFICATION_NONE = 0u,
    RINGIO_NOTIFICATION_ALWAYS,
    RINGIO_NOTIFICATION_ONCE,
    RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS,
    RINGIO_NOTIFICATION_HDWRFIFO_ONCE

} RingIO_NotifyType ;


/** ============================================================================
 *  @name   RingIO_Handle
 *
 *  @desc   This type is used for the handle to the RingIO Client structure.
 *  ============================================================================
 */
typedef Void * RingIO_Handle ;

/** ============================================================================
 *  @name   RingIO_ControlHandle
 *
 *  @desc   This type is used for the handle to the Shared Control structure.
 *  ============================================================================
 */
typedef struct RingIO_ControlStruct_tag * RingIO_ControlHandle ;

/** ============================================================================
 *  @name   RingIO_BufPtr
 *
 *  @desc   This type is used for the Buffer Pointer Type.
 *  ============================================================================
 */
typedef Void * RingIO_BufPtr ;

/** ============================================================================
 *  @name   RingIO_NotifyParam
 *
 *  @desc   This type is used for the Notification Parameter.
 *  ============================================================================
 */
typedef Void * RingIO_NotifyParam ;

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
    PoolId                 ctrlPoolId  ;
    PoolId                 dataPoolId  ;
    PoolId                 attrPoolId  ;
    PoolId                 lockPoolId  ;
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
typedef struct RingIO_ControlStruct_tag RingIO_ControlStruct;

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
 *              Denotes whether RingIO has been re-opened or not
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
 *              ID of DSP processor.
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
    RingIO_Client         writer ;
    RingIO_Client         reader ;
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
    Pvoid        phyControl ;
    Pvoid        virtControl ;
    Char8        name [RINGIO_NAME_MAX_LEN] ;
    PoolId       ownerProcId ;
    PoolId       ctrlPoolId ;
    PoolId       dataPoolId ;
    PoolId       attrPoolId ;
    PoolId       lockPoolId ;
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
 *  @deprecated The deprecated data structure RingIOEntry has been replaced
 *              with RingIOEntry
 *
 *  ============================================================================
 */
#define RingIOEntry                RingIO_Entry

/** ============================================================================
 *  @deprecated The deprecated data structure RingIORegion has been replaced
 *              with RingIO_Ctrl
 *
 *  ============================================================================
 */
#define RingIORegion               RingIO_Ctrl

/** ============================================================================
 *  @deprecated The deprecated API RingIO_SendNotify has been replaced
 *              with RingIO_sendNotify
 *
 *  ============================================================================
 */
#define RingIO_SendNotify          RingIO_sendNotify

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* if !defined (RINGIODEFS_H) */
