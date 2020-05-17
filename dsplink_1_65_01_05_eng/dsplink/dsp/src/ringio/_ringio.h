/** ============================================================================
 *  @file   _ringio.h
 *
 *  @path   $(DSPLINK)/dsp/src/ringio/
 *
 *  @desc   Defines the internal interfaces and data structures for the API
 *          sub-component RING IO.
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


#if !defined (_RINGIO_H)
#define _RINGIO_H


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sem.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <ringiodefs.h>
#include <mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  min
 *
 *  @desc   This macro returns the minimum of two values.
 *  ============================================================================
 */
#define RINGIO_min(a,b) ((a)<(b)?(a):(b))

/** ============================================================================
 *  @const  RINGIO_INVALID_ID
 *
 *  @desc   This constant an invalid index in the ring buffer.
 *  ============================================================================
 */
#define RINGIO_INVALID_OFFSET   -1

/** ============================================================================
 *  @macro  MAP_STATUS
 *
 *  @desc   Map the status code to ensure that RINGIO_SUCCESS is returned from
 *          all APIs for success.
 *  ============================================================================
 */
#define MAP_STATUS(status)    ((status) == SYS_OK) ? RINGIO_SUCCESS : (status)


/** ============================================================================
 *  @name   RingIO_Attr
 *
 *  @desc   Private structure of RingIO Attribute buffer
 *
 *  @field  offset
 *              Offset into data buffer
 *  @field  prevoffset
 *              Offset to previous attribute
 *  @field  type
 *              Attribute type
 *  @field  size
 *              Size of the attribute payload following this field
 *  @field  param
 *              Optional Parameter
 *  ============================================================================
 */
typedef struct RingIO_Attr_tag {
    Uint32    offset;
    Int32     prevoffset;
    Uint16    type;
    Uint16    size;
    Uint32    param;
} RingIO_Attr ;


/** ============================================================================
 *  @func   RingIO_init
 *
 *  @desc   This function initialize the RingIO component.
 *          This function does the system level initialization. This initializes
 *          all internal RingIO structures. This function must be called
 *          before any other RingIO functions are called
 *
 *  @arg    procId
 *              ID of the processor with which the RingIO component is shared.
 *  @arg    shAddr
 *              Address in shared memory for the RingIO control structure.
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
 *  @see    RingIO_exit ()
 *  ============================================================================
 */
Int
RingIO_init (IN Uint16 procId, IN Uint32 shAddr) ;


/** ============================================================================
 *  @func   RingIO_exit
 *
 *  @desc   This function finalizes the RingIO module.
 *
 *  @arg    procId
 *              ID of the processor with which the RingIO component is shared.
 *
 *  @ret    RINGIO_SUCCESS
 *              Operation successfully completed.
 *          RINGIO_EFAILURE
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    RingIO_init ()
 *  ============================================================================
 */
Int
RingIO_exit (IN Uint16 procId) ;


/*  ----------------------------------------------------------------------------
 *  @name   _RingIO_acquireLock
 *
 *  @desc   Gain access to a shared memory control structure through mutex lock
 *
 *  @arg    client
 *              Pointer to the client structure for the RingIO.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *          SYS_EBUSY
 *              SEM_pend failure when timeout occurs.
 *
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    _RingIO_releaseLock
 *  ----------------------------------------------------------------------------
 */
static
inline
Int
_RingIO_acquireLock (RingIO_Client * client)
{
    Int status     = SYS_OK ;
    Bool semStatus = TRUE ;

    if (client->virtControlHandle->transportType == RINGIO_TRANSPORT_GPP_DSP) {
        /* GPP  <--> DSP transport */
        status = MPCS_enter ((MPCS_Handle) (client->virtLockHandle))   ;
    }
    else {
        /* DSP  <--> DSP transport: the object provided must be a semaphore */
        semStatus = SEM_pend ((SEM_Handle) (client->virtLockHandle),
                              SYS_FOREVER) ;
        if (semStatus == FALSE) {
            status = SYS_EBUSY ;
        }
    }
    return status ;
}


/** ----------------------------------------------------------------------------
 *  @name   _RingIO_releaseLock
 *
 *  @desc   Relinquish access to a shared memory control structure through
 *          mutex lock
 *
 *  @arg    client
 *              Pointer to the client structure for the RingIO.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid argument.
 *
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    _RingIO_releaseLock
 *  ----------------------------------------------------------------------------
 */
static
inline
Int
_RingIO_releaseLock (RingIO_Client * client)
{
    Int status = SYS_OK ;

    if (client->virtControlHandle->transportType == RINGIO_TRANSPORT_GPP_DSP) {
        /* GPP  <--> DSP transport */
        status = MPCS_leave ((MPCS_Handle) (client->virtLockHandle)) ;
    }
    else {
        /* DSP  <--> DSP transport: the object provided must be a semaphore */
        SEM_post ((SEM_Handle) (client->virtLockHandle)) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerAcquire
 *
 *  @desc   This function acquires a buffer for the writer.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    dataBuf
 *              Placeholder for returning the data buffer pointer
 *  @arg    size
 *              Size of the data buffer to be acquired (IN)
 *              Size of the data buffer actually acquired (OUT)
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerAcquire (IN     RingIO_Client*  client,
                       OUT    RingIO_BufPtr * dataBuf,
                       IN OUT Uint32 *        size);


/** ============================================================================
 *  @func   _RingIO_readerAcquire
 *
 *  @desc   This function acquires a buffer for the reader.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    dataBuf
 *              Placeholder for returning the data buffer pointer
 *  @arg    size
 *              Size of the data buffer to be acquired (IN)
 *              Size of the data buffer actually acquired (OUT)
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerAcquire (IN     RingIO_Client * client,
                       OUT    RingIO_BufPtr * dataBuf,
                       IN OUT Uint32 *        size);


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerRelease
 *
 *  @desc   This function releases the data buffer acquired by a writer.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    size
 *              Size of the writer buffer to be released
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerRelease (IN  RingIO_Client* client, IN  Uint32 size);


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerRelease
 *
 *  @desc   This function releases the data buffer acquired by a reader.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    size
 *              Size of the reader buffer to be released
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerRelease (IN  RingIO_Client * client, IN  Uint32 size);


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerSetvAttribute
 *
 *  @desc   This function allows writer to set variable length attributes.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    offset
 *              Offset at which the variable length has to be set
 *  @arg    type
 *              Type of the attribute
 *  @arg    param
 *              (Optional) Parameter associated with the attribute
 *  @arg    pdata
 *              Buffer pointer containing the variable length attribute
 *              information
 *  @arg    size
 *              Size of the attribute buffer
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerSetvAttribute (IN  RingIO_Client *  client,
                             IN  Uint32           offset,
                             IN  Uint16           type,
                             IN  Uint32           param,
                             IN  RingIO_BufPtr    pdata,
                             IN  Uint32           size);


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerGetvAttribute
 *
 *  @desc   This function gets a variable length attribute
 *
 *  @arg    client
 *              Client handle.
 *  @arg    type
 *              Placeholder for returning the type of the attribute
 *  @arg    param
 *              Placeholder for returning the (optional) parameter
 *              associated with the attribute
 *  @arg    vptr
 *              Placeholder for returning the buffer pointer containing
 *              the variable length attribute information
 *  @arg    size
 *              Size of the attribute buffer
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerGetvAttribute (IN  RingIO_Client*  client,
                             OUT Uint16 *        type,
                             OUT Uint32 *        param,
                             OUT RingIO_BufPtr   vptr,
                             OUT Uint32 *        size);

/** ----------------------------------------------------------------------------
 *  @func   _RingIO_writerFlush
 *
 *  @desc   This function performs the flush operation from the writer side.
 *          This function is used to discard any committed data from the RingIO
 *          instance. Refer to the description in the user level header
 *          ringio.h for description.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    hardFlush
 *              Mode in which the flush operation discards committed data
 *              and attributes
 *  @arg    type
 *              Type of the attribute until which the flush operation was
 *              carried out.
 *  @arg    param
 *              Parameter associated with the attribute.
 *  @arg    bytesFlushed
 *               The size of the buffer flushed.
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerFlush (RingIO_Client * client,
                     Bool            hardFlush,
                     Uint16 *        type,
                     Uint32 *        param,
                     Uint32 *        bytesFlushed) ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_readerFlush
 *
 *  @desc   This function performs the flush operation from the reader side.
 *          This function is used to discard any committed data from the RingIO
 *          instance. Refer to the description in the user level header
 *          ringio.h for description.
 *
 *  @arg    client
 *              Client handle.
 *  @arg    hardFlush
 *              Mode in which the flush operation discards committed data
 *              and attributes
 *  @arg    type
 *              Type of the attribute until which the flush operation was
 *              carried out.
 *  @arg    param
 *              Parameter associated with the attribute.
 *  @arg    bytesFlushed
 *               The size of the buffer flushed.
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerFlush (RingIO_Client * client,
                     Bool            hardFlush,
                     Uint16 *        type,
                     Uint32 *        param,
                     Uint32 *        bytesFlushed) ;

/** ----------------------------------------------------------------------------
 *  @func   _RingIO_callNotifier
 *
 *  @desc   This function calls the notifier function registered by the
 *          client with the RingIO instance.
 *
 *  @arg    client
 *              Client handle
 *  @arg    msg
 *              Message to be sent along with the notification.
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *          <tbd: other errors>
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_callNotifier (RingIO_Client * client, RingIO_NotifyMsg msg);

/** ----------------------------------------------------------------------------
 *  @func   _RingIO_writerCancel
 *
 *  @desc   This function cancels the acquired buffers for a writer
 *
 *  @arg    client
 *              Client handle.
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerCancel (IN RingIO_Client * client) ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_readerCancel
 *
 *  @desc   This function cancels the acquired buffers for a reader
 *
 *  @arg    client
 *              Client handle.
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerCancel (IN RingIO_Client * client) ;

/** ----------------------------------------------------------------------------
 *  @func   _RingIO_checkNotifier
 *
 *  @desc   This function checks if conditions for invoking the notification
 *          callback are satisfied.
 *
 *  @arg    client
 *              Client handle
 *
 *  @ret    RINGIO_SUCCESS
 *              Successful completion
 *          RINGIO_EFAILURE
 *              Generic failure
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_checkNotifier (IN  RingIO_Client * client);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (_RINGIO_H) */
