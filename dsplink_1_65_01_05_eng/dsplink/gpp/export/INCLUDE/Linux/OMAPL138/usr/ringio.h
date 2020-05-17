/** ============================================================================
 *  @file   ringio.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines the interfaces and data structures for the API sub-component
 *          RING IO.
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


#if !defined (RINGIO_H)
#define RINGIO_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <ringiodefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   RingIO_getValidSize
 *
 *  @desc   Returns the current valid data size
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current valid data size>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Uint32
RingIO_getValidSize (IN RingIO_Handle handle );

/** ============================================================================
 *  @func   RingIO_getEmptySize
 *
 *  @desc   Returns the current empty buffer size
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current empty buffer size>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Uint32
RingIO_getEmptySize (IN RingIO_Handle handle );

/** ============================================================================
 *  @func   RingIO_getValidAttrSize
 *
 *  @desc   Returns the current valid attribute size
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current valid attribute size>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Uint32
RingIO_getValidAttrSize (IN RingIO_Handle handle );

/** ============================================================================
 *  @func   RingIO_getEmptyAttrSize
 *
 *  @desc   Returns the current empty attribute buffer size
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current empty attribute buffer size>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
Uint32
RingIO_getEmptyAttrSize (IN RingIO_Handle handle );

/** ============================================================================
 *  @func   RingIO_getAcquiredOffset
 *
 *  @desc   Returns the current acquire offset for the client
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <client's current acquired offset>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
#define RingIO_getAcquiredOffset(client)                \
                    (((RingIO_Client *) client)->acqStart)

/** ============================================================================
 *  @func   RingIO_getAcquiredSize
 *
 *  @desc   Returns the size of buffer currently acquired
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current acquired buffer size>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle).
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
#define RingIO_getAcquiredSize(client)                  \
                    (((RingIO_Client *) client)->acqSize)

/** ============================================================================
 *  @func   RingIO_getWatermark
 *
 *  @desc   Returns the current watermark level specified by the client
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    <current watermark level set by the client>
 *
 *  @enter  client must be a valid pointer(i.e valid RingIO Handle)
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
#define RingIO_getWatermark(client)                     \
                    (((RingIO_Client *) client)->notifyWaterMark)


/** ============================================================================
 *  @deprecated   RingIO_init
 *
 *  @desc   This function initializes the RingIO component.
 *          This function has been deprecated and is provided for backward
 *          compatibility.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
#define RingIO_init(procId) RINGIO_SUCCESS


/** ============================================================================
 *  @deprecated   RingIO_exit
 *
 *  @desc   This function finalizes the RingIO component.
 *          This function has been deprecated and is provided for backward
 *          compatibility.
 *
 *  @arg    procId
 *              Processor Id
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
#define RingIO_exit(procId) RINGIO_SUCCESS


/** ============================================================================
 *  @func   RingIO_create
 *
 *  @desc   This function creates a RingIO instance in Shared memory using the
 *          creation params specified.
 *
 *  @arg    procId
 *              Processor Identifier (Not available in legacy suuport)
 *  @arg    name
 *              Unique name identifying the RingIO instance.
 *  @arg    attrs
 *              Pointer to the RingIO creation attributes.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EACCESSDENIED
 *              The RINGIO component has not been initialized.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          RINGIO_EALREADYEXISTS
 *              The specified RINGIO name is already in use.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  RINGIO name must be valid.
 *          RINGIO creation attributes must be a valid.
 *          PROC_attach has been successful.
 *          Application specific POOL configuration is complete.
 *
 *
 *  @leave  None.
 *
 *  @see    RINGIO_delete.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_create (IN Char8 *         name,
               IN RingIO_Attrs *  attrs) ;
#else
RingIO_create (IN ProcessorId     procId,
               IN Char8 *         name,
               IN RingIO_Attrs *  attrs) ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */


/** ============================================================================
 *  @func   RingIO_delete
 *
 *  @desc   This function deletes a RingIO channel.
 *
 *  @arg    procId
 *              Processor Identifier (Not available in legacy suuport)
 *  @arg    name
 *              Name of the RingIO channel to be deleted.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EALREADYEXISTS
 *              The specified RINGIO name is already in use.
 *          DSP_EACCESSDENIED
 *              The RINGIO component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  RINGIO name must be valid.
 *          All RINGIO operations are complete.
 *
 *  @leave  None.
 *
 *  @see    RINGIO_create.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
#if defined (DSPLINK_LEGACY_SUPPORT)
RingIO_delete (IN Char8 * name) ;
#else
RingIO_delete (IN ProcessorId procId, IN Char8 * name) ;
#endif /* if defined (DSPLINK_LEGACY_SUPPORT) */


/** ============================================================================
 *  @func   RingIO_open
 *
 *  @desc   This function opens a RingIO channel.
 *          This function is used to open an RingIO Channel either for reading
 *          or writing. Only one reader and one writer can be opened on a RingIO
 *
 *  @arg    name
 *              Name of the RingIO channel to be opened.
 *  @arg    openMode
 *              Mode with which the RingIO channel is to be opened
 *              (Reader/Writer).
 *  @arg    flags
 *              Cache usage and other notifification flags.
 *
 *  @ret    Handle to the RingIO client structure.
 *              Operation successfully completed.
 *          NULL
 *              General failure, name not found.
 *
 *  @enter  RINGIO name must be valid.
 *          openMode must be either RINGIO_MODE_READ or RINGIO_MODE_WRITE
 *          RINGIO_create has been successful.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
RingIO_Handle
RingIO_open (IN Char8 *         name,
             IN RingIO_OpenMode openMode,
             IN Uint32          flags) ;


/** ============================================================================
 *  @func   RingIO_close
 *
 *  @desc   This function closes a RingIO instance.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EACCESSDENIED
 *              The RINGIO component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  handle must be valid.
 *          All RINGIO operations are complete.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_close (IN RingIO_Handle handle) ;


/** ============================================================================
 *  @func   RingIO_acquire
 *
 *  @desc   This function acquires a data buffer from RingIO for reading or
 *          writing, depending on the mode in which the client (represented
 *          by the handle) has been opened.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    dataBuf
 *              Location to store the pointer to the acquired data buffer.
 *  @arg    size
 *              Pointer to the size of data buffer acquired. Holds the size of
 *              buffer to be acquired. Returns the size of buffer actually
 *              acquired.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_SPENDINGATTRIBUTE
 *              No data buffer could be acquired because an attribute was
 *              present at the current read offset.
 *          RINGIO_EBUFFULL
 *              Requested size of data buffer could not be returned because
 *              the total available size is less than requested size. A smaller
 *              sized buffer may have been returned, if available.
 *          RINGIO_EBUFWRAP
 *              Requested size of data buffer could not be returned because the
 *              available contiguous size till end of buffer is less than
 *              requested size. A smaller sized buffer may have been returned,
 *              if available.
 *          RINGIO_EFAILURE
 *              General failure.
 *          RINGIO_ENOTCONTIGUOUSDATA
 *              Valid data is present but is not contiguous because the data
 *              has been acquired using the foot buffer.
 *
 *  @enter  handle must be valid.
 *          dataBuf must be a valid pointer.
 *          size must be a valid pointer.
 *          RINGIO_open (for reading/writing) has been successful.
 *
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_acquire (IN     RingIO_Handle   handle,
                OUT    RingIO_BufPtr * dataBuf,
                IN OUT Uint32 *        size) ;


/** ============================================================================
 *  @func   RingIO_release
 *
 *  @desc   This function releases a data buffer to RingIO.
 *          This function releases an acquired buffer or part of it.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    size
 *              Size of data buffer to be released.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  RINGIO_acquire has been successful.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_release (IN RingIO_Handle handle, IN Uint32 size) ;


/** ============================================================================
 *  @func   RingIO_cancel
 *
 *  @desc   This function cancels any data buffers acquired by reader or writer.
 *          In the case of writer, all attributes that are set since the first
 *          acquire are removed. In the case of reader, all attributes that were
 *          obtained since the first acquired are re-instated in the RingIO
 *          instance.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *
 *  @enter  handle must be valid.
 *          RINGIO_acquire has been successful.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_cancel (IN RingIO_Handle handle) ;


/** ============================================================================
 *  @func   RingIO_getvAttribute
 *
 *  @desc   This function gets an attribute with a variable-sized payload from
 *          the attribute buffer.
 *          If an attribute is present, the attribute type, the optional
 *          parameter, a pointer to the optional payload and the payload size
 *          are returned.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    type
 *              Location to receive the user-defined type of attribute.
 *  @arg    param
 *              Location to receive an optional parameter which depends on the
 *              attribute type.
 *  @arg    vptr
 *              Pointer to buffer to receive the optional payload.
 *  @arg    size
 *              Location with the size of the variable attribute. On return,
 *              this stores the actual size of the payload.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_SPENDINGATTRIBUTE
 *              Additional attributes are present at the current read offset.
 *          RINGIO_EVARIABLEATTRIBUTE
 *              No buffer has been provided to receive the variable attribute
 *              payload.
 *          RINGIO_EPENDINGDATA
 *              More data must be read before reading the attribute.
 *          RINGIO_EFAILURE
 *              No valid attributes are present, or general failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  handle must be a valid pointer.
 *          type must be valid.
 *          param must be valid.
 *          RingIO_setvAttribute has been successful.
 *
 *  @leave  None.
 *
 *  @see    RingIO_getAttribute ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_getvAttribute (IN     RingIO_Handle handle,
                      OUT    Uint16 *      type,
                      OUT    Uint32 *      param,
                      IN     RingIO_BufPtr vptr,
                      IN OUT Uint32 *      size) ;


/** ============================================================================
 *  @func   RingIO_setvAttribute
 *
 *  @desc   This function sets an attribute with a variable sized payload at the
 *          offset provided in the acquired data buffer.
 *          If the offset is not in the range of the acquired data buffer, the
 *          attribute is not set, and an error is returned. One exception to
 *          this rule is when no data buffer has been acquired. In this case an
 *          attribute is set at the next data buffer offset that can be acquired
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    offset
 *              Offset in the acquired data buffer to which the attribute
 *              corresponds
 *  @arg    type
 *              User-defined type of attribute.
 *  @arg    param
 *              Optional parameter which depends on the attribute type.
 *  @arg    pdata
 *              Pointer to attribute payload buffer.
 *  @arg    size
 *              Size of the attribute payload.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          RINGIO_EWRONGSTATE
 *              Failure to set attribute when data buffer is full with valid
 *              data and buffer is wrapped around.
 *
 *  @enter  handle must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    RingIO_setAttribute (), RingIO_getvAttribute ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_setvAttribute (IN     RingIO_Handle handle,
                      IN     Uint32        offset,
                      IN     Uint16        type,
                      IN     Uint32        param,
                      IN     RingIO_BufPtr pdata,
                      IN     Uint32        size) ;


/** ============================================================================
 *  @func   RingIO_getAttribute
 *
 *  @desc   This function gets a fixed-size attribute from the attribute buffer.
 *          If an attribute is present, the attribute type and a related
 *          parameter are returned.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    type
 *              Location to receive the user-defined type of attribute.
 *  @arg    param
 *              Location to receive an optional parameter which depends on the
 *              attribute type.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_SPENDINGATTRIBUTE
 *              Additional attributes are present at the current read offset.
 *          RINGIO_EVARIABLEATTRIBUTE
 *              A variable atrtibute exists. The application must call
 *              RingIO_getvAttribute () to get the variable attribute.
 *          RINGIO_EPENDINGDATA
 *              More data must be read before reading the attribute.
 *          RINGIO_EFAILURE
 *              No valid attributes are present, or general failure.
 *
 *  @enter  handle must be a valid pointer.
 *          type must be valid.
 *          param must be valid.
 *          RingIO_setAttribute has been successful.
 *
 *  @leave  None.
 *
 *  @see    RingIO_getvAttribute ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_getAttribute (IN  RingIO_Handle handle,
                     OUT Uint16 *      type,
                     OUT Uint32*       param) ;


/** ============================================================================
 *  @func   RingIO_setAttribute
 *
 *  @desc   This function sets a fixed-size attribute at the offset provided in
 *          the acquired data buffer.
 *          If the offset is not in the range of the acquired data buffer, the
 *          attribute is not set, and an error is returned. One exception to
 *          this rule is when no data buffer has been acquired. In this case an
 *          attribute is set at the next data buffer offset that can be acquired
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    offset
 *              Offset in the acquired data buffer to which the attribute
 *              corresponds
 *  @arg    type
 *              User-defined type of attribute.
 *  @arg    param
 *              Optional parameter which depends on the attribute type.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  handle must be valid.
 *
 *  @leave  None.
 *
 *  @see    RingIO_setvAttribute (), RingIO_getAttribute ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_setAttribute (IN     RingIO_Handle handle,
                     IN     Uint32        offset,
                     IN     Uint16        type,
                     IN     Uint32        param) ;


/** ============================================================================
 *  @func   RingIO_flush
 *
 *  @desc   This function flushes the data buffer.
 *          This function is used to flush the data from the RingIO.
 *          Behavior of this function depends on the value of hardFlush argument
 *          When hardFlush is false:
 *          If function is called for the writer, all the valid data in buffer
 *          after the first attribute location will be discarded. In case there
 *          are no attributes, no data will be cleared from the buffer. Note
 *          that this does not include the data that has been already acquired
 *          by the reader. Note that the attribute will also be cleared from the
 *          attribute buffer.
 *          For the reader, all the data till the next attribute location will
 *          be discarded. And if there is no attribute in the buffer, all valid
 *          data will get discarded. Note that the attribute will remain the
 *          attribute buffer. This is different from the behavior mentioned for
 *          the writer.
 *          When hardFlush is true:
 *          If function is called from the writer, all committed data and
 *          attributes that is not acquired by reader are removed from
 *          the RingIO instance. The writer pointer is moved to point to
 *          reader's head pointer
 *          If function is called from the reader, all data and attributes
 *          that can be subsequently acquired from the reader are removed.
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  @arg    hardFlush
 *              Mode in which the flush operation discards committed data
 *              and attributes
 *  @arg    type
 *              Location to receive the User-defined type of attribute.
 *  @arg    param
 *              Location to receive an optional parameter which depends on the
 *              attribute type.
 *  @arg    bytesFlushed
 *              Location to recieve the size of the buffer flushed.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  handle must be a valid pointer.
 *          type must be valid.
 *          param must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_flush (IN  RingIO_Handle handle,
              IN  Bool          hardFlush,
              OUT Uint16 *      type,
              OUT Uint32 *      param,
              OUT Uint32 *      bytesFlushed) ;


/** ============================================================================
 *  @func   RingIO_setNotifier
 *
 *  @desc   This function sets Notification parameters for the RingIO Client.
 *          Both the reader and writer can set their notification mechanism
 *          using this function
 *
 *  @arg    handle
 *              Handle to the RingIO client.
 *  @arg    notifyType
 *              Type of notification.
 *  @arg    notifyWatermark
 *              Watermark for notification
 *  @arg    notifyFunc
 *              Function to call when notification is required
 *  @arg    notifyParam
 *              Pointer to the notification parameter. The type of the pointer
 *              and its size depends on the notification function
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *
 *  @enter  RingIO_open for both reader and writer has been successful.
 *          No DSP/BIOS Link API should be called from a callback function
 *          registered through the RingIO or NOTIFY module. On DSP-side or
 *          on operating systems such as PrOS, the callback functions are
 *          run from ISR context and must not perform any operations that
 *          may take a lock or block, which is done by most DSPLink APIs.
 *          Minimum functionality must be used in the callback functions,
 *          most often limited to posting a semaphore on which application
 *          is waiting, posting SWI etc.
 *
 *  @leave  None.
 *
 *  @see    RingIO_NotifyType, RingIO_NotifyFunc, RingIO_NotifyParam
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_setNotifier (IN  RingIO_Handle      handle,
                    IN  RingIO_NotifyType  notifyType,
                    IN  Uint32             notifyWatermark,
                    IN  RingIO_NotifyFunc  notifyFunc,
                    IN  RingIO_NotifyParam notifyParam) ;

/** ============================================================================
 *  @func   RingIO_sendNotify
 *
 *  @desc   Send a notification to the other client manually
 *
 *  @arg    handle
 *              Handle to the RingIO Client.
 *  arg     msg
 *              Message to be sent along with notification.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RingIO_sendNotify (IN RingIO_Handle handle, IN RingIO_NotifyMsg msg) ;

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (RINGIO_H) */
