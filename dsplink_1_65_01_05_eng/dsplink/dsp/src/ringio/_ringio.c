/** ============================================================================
 *  @file   _ringio.c
 *
 *  @path   $(DSPLINK)/dsp/src/ringio/
 *
 *  @desc   Implementation of internal functions for API sub-component RING IO.
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <mem.h>
#include <sem.h>
#include <gbl.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <_mpcs.h>
#include <notify.h>
#include <ringiodefs.h>
#include <ringio.h>
#include <_ringio.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  FILEID
 *
 *  @desc   File identifier.
 *  ============================================================================
 */
#define FILEID FID__RINGIO_C

/*  ============================================================================
 *  @macro  RINGIO_MPCSOBJ_NAME
 *
 *  @desc   Name of the MPCS object used for protection of the RingIO region.
 *  ============================================================================
 */
#define  RINGIO_MPCSOBJ_NAME    MPCS_RESV_LOCKNAME"_RINGIO"


/** ============================================================================
 *  @name   RingIO_ctrlPtr
 *
 *  @desc   Pointer to RingIO Control region.
 *  ============================================================================
 */
extern RingIO_Ctrl * RingIO_ctrlPtr ;

/** ============================================================================
 *  @name   RingIO_mpcsObj
 *
 *  @desc   Handle to global MPCS object for protecting the RingIO region.
 *  ============================================================================
 */
extern MPCS_Handle   RingIO_mpcsObj ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_initInstance
 *
 *  @desc   This function initializes the RingIO instance identified by the
 *          control handle specified.
 *
 *  @arg    control
 *              Control handle that needs
 *
 *  @ret    None
 *
 *  @enter  control must be non-Null
 *
 *  @leave  None
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
Void
_RingIO_initInstance (RingIO_ControlHandle control) ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_getFirstAcquiredAttr
 *
 *  @desc   This function returns the first attribute that is acquired by
 *          the reader or the writer
 *
 *  @arg    client
 *              Client handle
 *
 *  @ret    attr
 *              The first attribute
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
RingIO_Attr *
_RingIO_getFirstAcquiredAttr (RingIO_Client * client);


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_getFirstAttr
 *
 *  @desc   This function returns the next attribute that can be acquired by
 *          the reader or the writer
 *
 *  @arg    client
 *              Client handle
 *
 *  @ret    attr
 *              The first attribute
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
RingIO_Attr *
_RingIO_getFirstAttr (RingIO_Client * client);


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_releaseAttribute
 *
 *  @desc   This function releases an attribute.
 *
 *  @arg    client
 *              Client handle
 *  @arg    attrBuf
 *              Attribute to be released.
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
_RingIO_releaseAttribute (RingIO_Client * client, RingIO_Attr * attrBuf) ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_getContiguousDataSize
 *
 *  @desc   This function returns the amount of contiguous data available for
 *          the reader to acquire. This is the minimum of
 *          1) the number of bytes available before encountering next attribute
 *          2) the number of bytes before encountering end of buffer
 *          3) total valid data available in the buffer
 *          This should be called only by the reader.
 *
 *  @arg    client
 *              Client handle
 *  @arg    size
 *              Placeholder for returning the size.
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
_RingIO_getContiguousDataSize (RingIO_Client * client, Uint32* size);


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_invalidateDataBuffer
 *
 *  @desc   This function invalidates the data buffer for
 *          in place buffer operations.
 *          This should be called only by the reader.
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
_RingIO_invalidateDataBuffer (RingIO_Client * client);


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_updateAttributes
 *
 *  @desc   This function updates the attribute's offset in early end scenarios.
 *
 *  @arg    client
 *              Client handle
 *  @arg    offset
 *              Attribute offset to be updated.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
Void
_RingIO_updateAttributes (RingIO_Client * client, Uint32 offset) ;


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_isOffsetInBetween
 *
 *  @desc   This function checks whether offset lies between start and end
 *          offsets. Note that this includes the start and end offsets
 *          themselves. Note that no check is made to verify whether the
 *          offset lies in the ring buffer.
 *
 *  @arg    offset
 *              Offset to be checked.
 *  @arg    start
 *              Start of the range.
 *  @arg    end
 *              End of the range.
 *  @arg    isFull
 *              Is Buffer full or start == end.
 *
 *  @ret    TRUE
 *              If the offset is within the range specified by start and end
 *              If start == end and isFull is TRUE i.e. buffer is full
 *          FALSE
 *              Otherwise
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
static
inline
Bool
_RingIO_isOffsetInBetween (Uint32 offset,
                           Uint32 start,
                           Uint32 end,
                           Bool isFull)

{
    Bool retVal = FALSE ;

    /* This condition checks when wraparound
     * has not occurred.
     */
    if (start < end) {
        if ( (offset >= start) && (offset <= end) ) {
            retVal = TRUE ;
        }
    }

    /* This condition checks when wraparound
     * has occurred.
     */
    else if (start > end) {
        if ((offset >= start) || (offset <= end)) {
            retVal = TRUE ;
        }
    }

    /* This condition implies that either buffer
     * is full or buffer is empty. The caller
     * knows whether buffer is full or empty and
     * passes it by means of the isFull flag.
     */
     else if (start == end) {
         if (isFull == TRUE) {
                retVal = TRUE ;
         }
     }

    return retVal ;
}


/** ============================================================================
 *  @func   RingIO_init
 *
 *  @desc   This function initialize the RingIO component.
 *          This function does the system level initialization. This initializes
 *          all internal RingIO structures.This function should be called even
 *          before any other RingIO functions are called
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_init (IN Uint16 procId, IN Uint32 shAddr)
{
    Int        status = RINGIO_SUCCESS ;
    MPCS_Attrs attrs ;

    RingIO_ctrlPtr = (RingIO_Ctrl *) shAddr ;

    HAL_cacheInv ((Void *) RingIO_ctrlPtr, sizeof (RingIO_Ctrl)) ;

    if (RingIO_ctrlPtr->isInitialized == FALSE) {
        RingIO_ctrlPtr->isInitialized = TRUE ;
        RingIO_ctrlPtr->dspId         = GBL_getProcId () ;
        memset ((Char *) RingIO_ctrlPtr->dspAddrEntry,
                0,
                RingIO_ctrlPtr->maxEntries * sizeof (RingIO_Entry)) ;

        attrs.poolId = (Uint16) MPCS_INVALID_ID ;
        status = MPCS_create (procId,
                              RINGIO_MPCSOBJ_NAME,
                              &(RingIO_ctrlPtr->lockObj),
                              &attrs) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }

        HAL_cacheWbInv ((Ptr) (RingIO_ctrlPtr), sizeof (RingIO_Ctrl)) ;
    }

    if (status == SYS_OK) {
        status = _MPCS_open (procId,
                             RINGIO_MPCSOBJ_NAME,
                             &RingIO_mpcsObj,
                             &(RingIO_ctrlPtr->lockObj)) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return MAP_STATUS (status) ;
}


/** ============================================================================
 *  @func   RingIO_exit
 *
 *  @desc   This function finalizes the RingIO module.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
RingIO_exit (IN Uint16 procId)
{
    Int status = SYS_OK ;

    if (RingIO_mpcsObj != NULL) {
        status = MPCS_close (procId, RingIO_mpcsObj) ;
        if (status == SYS_OK) {
            RingIO_mpcsObj = NULL ;
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

    RingIO_ctrlPtr = NULL ;

    return MAP_STATUS (status) ;
}


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_writerAcquire
 *
 *  @desc   This function acquires a buffer for the writer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerAcquire (RingIO_Client *  client,
                       RingIO_BufPtr * dataBuf,
                       Uint32 *        size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Uint32               storedEmptySize = 0 ;
    Uint32               bufSize ;
    Uint32               bytesTillEndOfBuffer ;
    Uint32               contiguousSpace ;


    DBC_require (client != NULL) ;

    DBC_require (client  != NULL) ;
    DBC_require (dataBuf != NULL) ;
    DBC_require (size    != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* If no free space is available, return failure */
    if (control->emptySize == 0) {
        *dataBuf = NULL ;
        *size    = 0 ;
        status   = RINGIO_EBUFFULL ;
    }
    else {
        /* Current size of the buffer */
        bufSize = GET_CUR_DATA_END(control) ;
        storedEmptySize = control->emptySize ;

        /*  Find total contiguous free space available. */
        bytesTillEndOfBuffer = (  bufSize
                                - ((  ADJUST_MODULO (client->acqStart,
                                                     client->acqSize,
                                                     bufSize)))) ;

        contiguousSpace = RINGIO_min (bytesTillEndOfBuffer, control->emptySize);

        /* If the requested free space is not available as a contiguous chunk */
        if (*size > contiguousSpace) {
            /*  If the contiguousBufFlag is set, we need to satisfy the
             *  request only if a contiguous data buffer of requested size
             *  is found. This can be done as follows:
             *  1) Check if an early end to the buffer can be set and then a
             *     contiguous data buffer can be provided from the start
             *     of buffer.
             *  2) If step 1 does not give a contiguous buffer return
             *     failure.
             */
            if (client->flags & RINGIO_NEED_EXACT_SIZE) {
                /*  Check if the data request can be satisfied from the
                 *  start of the buffer.
                 */
                if ((control->emptySize - contiguousSpace) < *size) {
                    /*  Return a failure as we cannot satisfy the contiguous
                     *  buffer requirement even by setting an early end
                     */
                    *size    = 0 ;
                    *dataBuf = (RingIO_BufPtr) NULL ;
                    status   = RINGIO_EFAILURE ;
                }
                else {
                    /* Mark early buf End */
                    control->curBufEnd -= contiguousSpace;

                    /* Reduce amount of empty size available due to early end */
                    control->emptySize -= contiguousSpace;
                }
            }
            else {
                /*  As a contiguous buffer flag is not set, just provide
                 *  whatever is available as a contiguous chunk
                 */
                if (*size > control->emptySize) {
                    status = RINGIO_EBUFFULL ;
                }
                else {
                    status = RINGIO_EBUFWRAP ;
                }

                *size = contiguousSpace ;
            }
        }

        /* Buffer is to be returned even if status is failure with
         * RINGIO_EBUFFULL or RINGIO_EBUFWRAP.
         */
        if (status != RINGIO_EFAILURE) {
            /* Calculate the current buffer size */
            bufSize = GET_CUR_DATA_END(control);

            if (client->acqStart == bufSize) {
                client->acqStart = 0 ;
            }

            /* Get the next acquire address */
            *dataBuf = (RingIO_BufPtr) (  (Uint32) client->pDataStart
                                        + ( ADJUST_MODULO (client->acqStart,
                                                           client->acqSize,
                                                           bufSize))) ;

            client->acqSize    += *size ;
            control->emptySize -= *size ;
        }
    }

    /* On acquire failure notification is enabled for all types. */
    if (   ((status == RINGIO_EFAILURE) || (status == RINGIO_EBUFFULL))
        && (client->notifyType != RINGIO_NOTIFICATION_NONE)) {
        client->notifyFlag = TRUE ;
    }
    else if (*size != 0) {
        /* This check is to disable notification in RINGIO_NOTIFICATION_ALWAYS
         * case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Empty Data was earlier above watermark and now falls below
         *    watermark.
         */
        if (   (client->notifyFlag == TRUE)
            && (client->notifyType == RINGIO_NOTIFICATION_ALWAYS)) {
            if (   (storedEmptySize >= client->notifyWaterMark)
                && (control->emptySize < client->notifyWaterMark)) {
                client->notifyFlag = FALSE ;
            }
        }
        /* This check is to enable notification in
         * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Empty Data Size falls below watermark.
         */
        else if ((client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE) &&
            (control->emptySize < client->notifyWaterMark) &&
            (client->notifyFlag == FALSE)){
                client->notifyFlag = TRUE ;
        }
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerAcquire
 *
 *  @desc   This function acquires a buffer for the reader.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerAcquire (RingIO_Client * client,
                       RingIO_BufPtr * dataBuf,
                       Uint32 *        size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Uint32               bufSize ;
    Uint32               contiguousSpace ;
    RingIO_Attr *        attrBuf ;
    Uint32               storedValidSize ;
    Uint32               nextAcqOffset ;
    Uint32               i ;
    Char *               top ;
    Char *               foot ;
    Char *               footStart ;
    Uint32               footSize ;

    DBC_require (client  != NULL) ;
    DBC_require (dataBuf != NULL) ;
    DBC_require (size    != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* Current size of the buffer */
    bufSize = GET_CUR_DATA_END(control) ;

    if (control->validSize == 0) {
        /* If no data is available, return failure */
        *dataBuf = NULL ;
        *size    = 0 ;
        if (control->validAttrSize == 0) {
            status = RINGIO_EBUFEMPTY ;
        }
        else {
            status = RINGIO_SPENDINGATTRIBUTE ;
        }
    }
    else {
        storedValidSize = control->validSize ;

        /* Initialize the nextAcqOffset */
        nextAcqOffset = ADJUST_MODULO (client->acqStart,
                                       client->acqSize,
                                       bufSize) ;

        /*  If attributes are present, and there is an attribute present at the
         *  current read offset, return RINGIO_SPENDINGATTRIBUTE
         */
        if (control->validAttrSize) {
            attrBuf = _RingIO_getFirstAttr (client) ;
            /* Handle the special wraparound case when there is an attribute at
             * the *very* end of the buffer
             */
            if (   ((nextAcqOffset == 0) && (attrBuf->offset == bufSize))
                || (attrBuf->offset == nextAcqOffset)) {
                *dataBuf = NULL ;
                *size    = 0 ;
                status   = RINGIO_SPENDINGATTRIBUTE ;
            }
        }

        if (status != RINGIO_SPENDINGATTRIBUTE) {
            status = _RingIO_getContiguousDataSize (client, &contiguousSpace) ;

            /* If requested data size is not available as a contiguous chunk */
            if (*size > contiguousSpace) {
                if (client->flags & RINGIO_NEED_EXACT_SIZE) {
                    /* Return a failure as we cannot satisfy the contiguous
                     * buffer requirement.
                     */
                    *size    = 0 ;
                    *dataBuf = (RingIO_BufPtr) NULL ;
                    status   = RINGIO_EFAILURE ;
                }
                else {
                    *size  = contiguousSpace ;
                    /* If attribute is present within, or at the end of the
                     * valid contiguous size available, return status code
                     * RINGIO_SPENDINGATTRIBUTE to indicate this.
                     */
                    if (status != RINGIO_SPENDINGATTRIBUTE) {
                        status = RINGIO_ENOTCONTIGUOUSDATA ;
                    }
                }
            }
            else {
                status = RINGIO_SUCCESS ;
            }

            /* Buffer is to be returned even if status is failure with
             * RINGIO_EBUFEMPTY.
             */
            if (status != RINGIO_EFAILURE) {
                /* Check if the buffer needs to be copied from top of
                 * the data buffer to the footer area
                 */
                if (*size > (bufSize - nextAcqOffset)) {
                    top  = (Char *) client->pDataStart ;
                    foot = (Char *) (  ((Uint32) client->pDataStart)
                                       + bufSize) ;
                    footStart = foot ;
                    footSize  = (*size - (bufSize - nextAcqOffset)) ;

                    if (client->flags & RINGIO_DATABUF_CACHEUSE) {
                        /* Invalidate the top from which data
                         * will be copied to foot buffer
                         */
                        HAL_cacheInv ((Ptr) top, footSize) ;
                        /* Invalidate the area before actual
                         * foot buffer start.
                         * This is to invalidate buffer
                         * before early end up to the cache line start.
                         * as complete cache line will
                         * be written back.
                         */
                        HAL_cacheInv ((Ptr)
                                  (  (Uint32) footStart
                                   - ((Uint32) footStart % CACHE_L2_LINESIZE)),
                                  ((Uint32) footStart % CACHE_L2_LINESIZE)) ;
                    }

                    for (i = 0 ; i < footSize ; i++) {
                        *foot++ = *top++ ;
                    }

                    if (client->flags & RINGIO_DATABUF_CACHEUSE) {
                        /* Align footStart to cache line boundary. */
                        footSize  =   footSize
                                    + ((Uint32) footStart % CACHE_L2_LINESIZE) ;
                        footStart = (Char *)
                                  (  (Uint32) footStart
                                   - ((Uint32) footStart % CACHE_L2_LINESIZE)) ;
                        HAL_cacheWbInv ((Ptr) footStart, footSize) ;
                    }
                }

                /* Reset acquire start in the case of wraparound.  */
                if (client->acqStart == bufSize) {
                    client->acqStart = 0 ;
                }

                /* Get the next acquire address */
                *dataBuf = (RingIO_BufPtr) (  (Uint32) client->pDataStart
                                            + nextAcqOffset) ;

                client->acqSize    += *size ;
                control->validSize -= *size ;

                if (client->flags & RINGIO_DATABUF_CACHEUSE) {
                    HAL_cacheInv ((Ptr) *dataBuf, *size) ;
                }
            }
        }
    }

    /* On acquire failure notification is enabled for all types. */
    if (   ((status == RINGIO_EFAILURE) || (status == RINGIO_EBUFEMPTY))
        && (client->notifyType != RINGIO_NOTIFICATION_NONE)) {
        client->notifyFlag = TRUE ;
    }
    else if (*size != 0) {
        /* This check is to disable notification in RINGIO_NOTIFICATION_ALWAYS
         * case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Valid Data was earlier above watermark and now falls below
         *    watermark.
         */
        if (   (client->notifyFlag == TRUE)
            && (client->notifyType == RINGIO_NOTIFICATION_ALWAYS)) {
            if (   (storedValidSize >= client->notifyWaterMark)
                && (control->validSize < client->notifyWaterMark)) {
                client->notifyFlag = FALSE ;
            }
        }
        /* This check is to enable notification in
         * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Acquire was successful
         * 2) Valid Data Size falls below watermark.
         */
        else if ((client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE) &&
            (control->validSize < client->notifyWaterMark) &&
            (client->notifyFlag == FALSE)){
            /* Enable  the notifyFlag of the client in HDWRFIFO_ONCE_ case
             * if the amount of data available is less than watermark.
             */
                client->notifyFlag = TRUE ;
        }
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerRelease
 *
 *  @desc   This function releases the data buffer acquired by a writer.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerRelease (RingIO_Client * client, Uint32 size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Bool                 isFull  = FALSE ;
    Uint32               bufSize ;
    Uint32               endBufOffset ;
    Uint32               temp ;
    RingIO_Attr *        attrBuf ;

    DBC_require (client != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* Make sure that we are not trying to release what we haven't acquired */
    if (size > client->acqSize) {
        status = RINGIO_EFAILURE ;
    }
    else {
        /* Get the current data buffer Size */
        bufSize = GET_CUR_DATA_END(control) ;

        /* Starting from the first attribute release all attributes with
         * offsets between client->acqStart and (client->acqStart + size)
         */
        if (client->pAttrStart != NULL) {
            endBufOffset = ADJUST_MODULO (client->acqStart, size, bufSize) ;

            /* Empty buffer size not done here as 'size' check in ringio.c
             * No extra check for (size == bufSize) as below condition
             * implies the same state.
             */
            if (client->acqStart == endBufOffset) {
                isFull = TRUE ;
            }

            for ( ; ; ) {
                attrBuf = _RingIO_getFirstAttr (client) ;

                /* Check if attribute is within size to be released.
                 * If offset is between acqStart and endBufOffset it
                 * must be released.
                 */
                if (    (attrBuf == NULL)
                    ||  ( !_RingIO_isOffsetInBetween (attrBuf->offset,
                                                      client->acqStart,
                                                      endBufOffset,
                                                      isFull))) {
                    break ;
                }

                _RingIO_releaseAttribute (client, attrBuf) ;
            }
        }

        if (client->flags & RINGIO_DATABUF_CACHEUSE) {
            temp = ((Uint32) client->pDataStart) + client->acqStart ;
            HAL_cacheWbInv ((Ptr) (temp), size) ;
        }

        client->acqStart    = ADJUST_MODULO (client->acqStart, size, bufSize) ;
        client->acqSize    -= size ;
        control->validSize += size ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerRelease
 *
 *  @desc   This function releases the data buffer acquired by a reader.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerRelease (RingIO_Client * client, Uint32 size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Bool                 isFull  = FALSE ;
    Uint32               bufSize ;
    Uint32               storedAcqStart ;
    Uint32               storedAttrAcqStart ;
    Uint32               endBufOffset ;
    RingIO_Attr *        attrBuf ;

    DBC_require (client != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* Make sure that we are not trying to release what we haven't acquired */
    if (size > client->acqSize) {
        status = RINGIO_EFAILURE ;
    }
    else {
        storedAcqStart = client->acqStart ;
        storedAttrAcqStart = client->acqAttrStart ;

        bufSize = GET_CUR_DATA_END(control) ;

        /* If attribute buffer is present, starting from the first attribute,
         * release all attributes with offsets between client->acqStart and
         * client->acqStart+size.
         */
        if (client->pAttrStart != NULL) {
            endBufOffset = ADJUST_MODULO (client->acqStart, size, bufSize) ;

            /* Empty buffer size not done here as 'size' check in ringio.c
             * No extra check for (size == bufSize) as below condition
             * implies the same state.
             */
            if (client->acqStart == endBufOffset) {
                isFull = TRUE ;
            }

            for ( ; ; ) {
                attrBuf = _RingIO_getFirstAcquiredAttr (client) ;
                if (   (attrBuf != NULL)
                    && (    (size == bufSize)
                        ||  (_RingIO_isOffsetInBetween (attrBuf->offset,
                                                        client->acqStart,
                                                        endBufOffset,
                                                        isFull)))) {
                            _RingIO_releaseAttribute (client, attrBuf) ;
                        }
                else {
                    break ;
                }
            }
        }

        if (client->flags & RINGIO_DATABUF_CACHEUSE) {
            _RingIO_invalidateDataBuffer (client) ;
        }

        /* Release the data buffer */
        client->acqStart    = ADJUST_MODULO (client->acqStart, size, bufSize) ;
        client->acqSize    -= size ;
        control->emptySize += size ;

        /* Reset the early end for the data buffer if reader has read past
         * the early end marker
         */
        if (    (GET_CUR_DATA_END(control) != control->dataBufSize)
             && (GET_CUR_DATA_END(control)) <= (storedAcqStart + size)) {
                /* Shift up all attributes set in the unused area (occurred due to
                 * early end).
                 */
                _RingIO_updateAttributes (client, GET_CUR_DATA_END(control)) ;

                control->emptySize += (control->dataBufEnd - control->curBufEnd) ;
                control->curBufEnd  = control->dataBufEnd ;
        }


        /* Reset the early end for attribute buffer if reader has read past
         * the early end marker
         */
        if (storedAttrAcqStart > client->acqAttrStart) {
            control->emptyAttrSize +=   control->phyAttrBufEnd
                                      - control->curAttrBufEnd ;
            control->curAttrBufEnd  = control->phyAttrBufEnd ;
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIO_initInstance (control) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerSetvAttribute
 *
 *  @desc   This function allows writer to set variable length attributes.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerSetvAttribute (RingIO_Client *  client,
                             Uint32           offset,
                             Uint16           type,
                             Uint32           param,
                             RingIO_BufPtr    pdata,
                             Uint32           size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    RingIO_Attr *        destAttrBuf ;
    Uint32               bytesTillEndOfBuffer ;
    Uint32               contiguousSpace ;
    Uint32               bufSize ;
    Uint32               attrBufSize ;
    Uint32               attrOffset ;

    DBC_require (client != NULL) ;

    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart == NULL) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;
    }

    if (status == SYS_OK) {
        /* Data Buffer current Size */
        bufSize = GET_CUR_DATA_END(control) ;
        if (    (control->validSize == (GET_CUR_DATA_END(control)))
            ||  (   (control->emptySize == 0)
                 && (offset             == client->acqSize))) {
            /* Do not allow setting an attribute when it falls into reader
             * region. The following scenarios cover this condition:
             * - The buffer is completely full. In this case, attribute can only
             *   be set at offset 0. But offset 0 falls into reader region.
             * - The buffer is completely acquired by the writer. Part or none
             *   of this buffer may be released. Writer is attempting to set an
             *   attribute at the end of its acquired range. In this case, end
             *   of writer buffer is the same as beginning of reader buffer.
             *
             * If the reader has acquired and released some data, resulting in
             * its moving further such that its acquire start is not at the
             * same location where writer may be able to set an attribute, the
             * above conditions do not hold true, and the attribute is allowed
             * to be set.
            */
            status = RINGIO_EWRONGSTATE ;
        }
        else if (client->acqSize) {
            /* Check if the offset provided is within acquired range. */
            if (offset > client->acqSize) {
                status = RINGIO_EFAILURE ;
            }
        }
        else {
            /*
             *  This is the case when no data has been acquired, but the
             *  writer wishes to set an attribute at the next buffer write
             *  location. To enable this we setup offset as 0.This makes
             *  sure that an attribute is set at an offset which corresponds
             *  to the next position which can be acquired next
                 */
                offset = 0 ;
            }

        if (status == SYS_OK) {
            attrBufSize = GET_CUR_ATTR_END(control) ;
            bytesTillEndOfBuffer = (    attrBufSize
                                   - (  ADJUST_MODULO (client->acqAttrStart,
                                                       client->acqAttrSize,
                                                       attrBufSize))) ;

            contiguousSpace = RINGIO_min (bytesTillEndOfBuffer,
                                          control->emptyAttrSize) ;

            if (contiguousSpace < (SIZEOF_ATTR(size))) {
                /* Check if the attribute can be placed at the beginning of
                 * buffer
                 */
                if (  (control->emptyAttrSize - contiguousSpace)
                    < (SIZEOF_ATTR(size))) {
                    status = RINGIO_EFAILURE ;
                }
                else {
                    control->curAttrBufEnd -= contiguousSpace ;
                    control->emptyAttrSize -= contiguousSpace ;
                }
            }
        }
    }

    if (status == SYS_OK) {
        /* Copy the attribute to next available location and update all
         * fields
         */
        /* Attr Buffer current size */
        attrBufSize = GET_CUR_ATTR_END(control) ;

        attrOffset  = ADJUST_MODULO (client->acqAttrStart,
                                     client->acqAttrSize,
                                     attrBufSize) ;
        destAttrBuf = (RingIO_Attr*)((Uint32) client->pAttrStart + attrOffset) ;

        destAttrBuf->offset     = ADJUST_MODULO (client->acqStart,
                                                 offset,
                                                 bufSize) ;
        destAttrBuf->prevoffset = control->prevAttrOffset ;
        destAttrBuf->size       = size ;
        destAttrBuf->type       = type ;
        destAttrBuf->param      = param ;

        /* Copy data contents for variable attribute */
        if (size != 0) {
            memcpy ((void *) ((Uint32) destAttrBuf + sizeof (RingIO_Attr)),
                    pdata,
                    size) ;
        }

        control->prevAttrOffset = attrOffset ;

        /* Now the attribute buffer has been acquired, and the
         * attribute has also been set. So update all the relevant fields
         */
        control->emptyAttrSize -= (SIZEOF_ATTR(size)) ;
        client->acqAttrSize    += (SIZEOF_ATTR(size)) ;

        /* If no data buffer has been acquired, the attribute can be
         * released now
         */
        if (client->acqSize == 0) {
            client->acqAttrStart = ADJUST_MODULO (client->acqAttrStart,
                                                  (SIZEOF_ATTR(size)),
                                                  attrBufSize) ;

            client->acqAttrSize     = 0 ;
            control->validAttrSize += (SIZEOF_ATTR(size)) ;
        }

        if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
            HAL_cacheWbInv ((Ptr) destAttrBuf, (SIZEOF_ATTR(size))) ;
        }
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerGetvAttribute
 *
 *  @desc   This function gets a variable length attribute
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerGetvAttribute (RingIO_Client * client,
                             Uint16 *        type,
                             Uint32 *        param,
                             RingIO_BufPtr   vptr,
                             Uint32 *        size)
{
    Int                  status      = RINGIO_SUCCESS ;
    RingIO_ControlHandle control     = NULL ;
    RingIO_Client *       otherClient = NULL ;
    RingIO_Attr *        attrBuf ;
    Uint32               bufSize ;

    DBC_require (client != NULL) ;

    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart == NULL) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        /* If there are no valid attributes, return failure */
        if (control->validAttrSize == 0) {
            status = RINGIO_EFAILURE ;
        }
    }

    if (status == SYS_OK) {
        /* Get the current buffer size */
        bufSize = GET_CUR_DATA_END(control) ;

        /*  Since we have already made sure that there are attributes,
         *  no need to check attrBuf is NULL
         */
        attrBuf = _RingIO_getFirstAttr (client) ;

        /* If there is an attribute at the current read offset */
        if (   (attrBuf->offset == (client->acqStart + client->acqSize))
            || ((ADJUST_MODULO (client->acqStart, client->acqSize, bufSize))
                == attrBuf->offset)) {
            /* Copy the attribute type and attribute param */
            *type  = attrBuf->type ;
            *param = attrBuf->param ;

            if (size == NULL) {
                /* If the user hasn't supplied a buffer, but the attribute
                 * present is a variable attribute, return error
                 */
                if (attrBuf->size) {
                    status = RINGIO_EVARIABLEATTRIBUTE ;
                }
            }
            else {
                /* If the user hasn't supplied a sufficient buffer for the
                 * payload return error, and return the correct size of the
                 * attr as well
                 */
                if (*size < attrBuf->size) {
                    *size = attrBuf->size ;
                    status = RINGIO_EFAILURE ;
                }
                else {
                    /* Copy the payload and its size */
                    *size = attrBuf->size ;
                    if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                        HAL_cacheInv (
                              ((Ptr) ((Uint32) attrBuf + sizeof (RingIO_Attr))),
                              attrBuf->size) ;
                    }

                    memcpy (vptr,
                            ((void *)((Uint32) attrBuf + sizeof (RingIO_Attr))),
                            *size) ;
                }
            }

            if (status == SYS_OK) {
                control->validAttrSize -= SIZEOF_ATTR(attrBuf->size) ;

                /* Free up the attribute buffer in case the client has not
                 * acquired anything
                 */
                if (client->acqSize == 0) {
                    control->emptyAttrSize +=  SIZEOF_ATTR(attrBuf->size) ;

                    client->acqAttrStart =   (  client->acqAttrStart
                                              + SIZEOF_ATTR(attrBuf->size) )
                                          %  (GET_CUR_ATTR_END(control)) ;
                    client->acqAttrSize = 0 ;
                }
                else {
                    client->acqAttrSize    += SIZEOF_ATTR(attrBuf->size) ;
                }

                /* We need to return RINGIO_SPENDINGATTRIBUTE if there are
                 * more attributes at the current read offset
                 */
                attrBuf = _RingIO_getFirstAttr (client) ;
                if (attrBuf != NULL) {
                    /* If there is an attribute at the current read offset */
                    if ( ADJUST_MODULO (client->acqStart,
                                        client->acqSize,
                                        bufSize)
                         == attrBuf->offset) {
                        status = RINGIO_SPENDINGATTRIBUTE ;
                    }
                    attrBuf->prevoffset = RINGIO_INVALID_OFFSET ;
                    if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                        HAL_cacheWbInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
                    }
                }
                else {
                    otherClient = &(control->writer) ;
                    if (otherClient->acqAttrSize == 0) {
                        control->prevAttrOffset = RINGIO_INVALID_OFFSET ;
                    }
                }
            }
        }
        else {
            /* We need to read more data before reading an attribute */
            status = RINGIO_EPENDINGDATA ;
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIO_initInstance (control) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerFlush
 *
 *  @desc   This function performs the flush operation from the writer side.
 *          This function discards any data from the bottom of the valid data
 *          buffer until the first unread attribute.
 *          This function also returns information associated with
 *          that attribute.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerFlush (RingIO_Client * client,
                     Bool            hardFlush,
                     Uint16 *        type,
                     Uint32 *        param,
                     Uint32 *        bytesFlushed)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    RingIO_Client *      otherClient = NULL ;
    Uint32               flushAttrSize  = 0 ;
    Uint32               flushSize      = 0 ;
    RingIO_Attr *        attrBuf ;
    Bool                 isFull  = FALSE ;
    Uint32               attrOffset ;
    Uint32               attrBufSize ;
    Uint32               bufSize ;
    Uint32               bytesTillEndOfBuffer ;
    Uint32               storedAcqStart ;
    Uint32               storedAcqAttrStart ;

    DBC_require (client != NULL) ;
    DBC_require (type   != NULL) ;
    DBC_require (param   != NULL) ;
    DBC_require (bytesFlushed  != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* In the case when hardFlush is false, writer can only flush data starting
     * from first attribute (as seen from the reader side). In case there are
     * no attributes, we should return failure.
     */
    if ((control->validAttrSize == 0) && (hardFlush == FALSE)) {
        *type = RINGIO_INVALID_ATTR ;
        status = RINGIO_EFAILURE ;
    }
    else {
        bufSize = GET_CUR_DATA_END(control) ;

        if (control->validAttrSize != 0) {
            /* We need to flush out all attributes starting from the last one,
             * till only one is left in the attribute buffer. The last set
             * attribute is present at prevAttrOffset. Note that prevAttrOffset
             * will be valid, as we have already checked that an attribute is
             * present in the attribute buffer
             */
            attrOffset = control->prevAttrOffset ;
            do {
                attrBuf = (RingIO_Attr *) (  (Uint32)client->pAttrStart
                                          + attrOffset) ;

                if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                    HAL_cacheInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
                }

                flushAttrSize += (SIZEOF_ATTR(attrBuf->size)) ;
                attrOffset     = attrBuf->prevoffset ;
            } while (attrBuf->prevoffset != RINGIO_INVALID_OFFSET) ;

            /* Now we have reached the first attribute that is readable by the
             * reader. We should remove all attributes till this one, and
             * return info from this attribute.
             */
            control->emptyAttrSize += flushAttrSize ;

            control->prevAttrOffset = RINGIO_INVALID_OFFSET ;
            attrBufSize = GET_CUR_ATTR_END(control) ;

            storedAcqAttrStart = client->acqAttrStart ;
            /* All the attributes have been removed now */
            client->acqAttrStart =    (  client->acqAttrStart
                                       - (flushAttrSize - client->acqAttrSize)
                                       + attrBufSize)
                                  %   attrBufSize ;

            /* Do isFull check before change in validAttrSize.  */
            if (   (client->acqAttrStart == storedAcqAttrStart)
                && (control->validAttrSize == attrBufSize)){
                isFull = TRUE ;
            }
            control->validAttrSize -= (flushAttrSize - client->acqAttrSize) ;
            client->acqAttrSize  = 0 ;

            /* Check if early end needs to be reset in attribute buffer. */
            if (    (control->curAttrBufEnd != control->phyAttrBufEnd)
                &&  (_RingIO_isOffsetInBetween (control->curAttrBufEnd,
                                                client->acqAttrStart,
                                                storedAcqAttrStart,
                                                isFull))) {
                /* Early end was set in the region to be flushed. The early end
                 * needs to be reset and the size added to emptyAttrSize.
                 */
                control->emptyAttrSize += (     control->phyAttrBufEnd
                                           -    control->curAttrBufEnd) ;
                control->curAttrBufEnd = control->phyAttrBufEnd ;
            }
        }

        /* Now we have to flush all the data starting attrBuf->offset
         * onwards in case hardFlush is false.
         */
        if (hardFlush == FALSE) {
            if (attrBuf->offset == client->acqStart) {
                /* Check if buffer is full */
                if (control->validSize == bufSize) {
                    flushSize = control->validSize ;
                }
                else {
                    flushSize = 0 ;
                }
            }
            else if (attrBuf->offset < client->acqStart) {
                flushSize = client->acqStart - attrBuf->offset ;
            }
            else {
                bytesTillEndOfBuffer = control->curBufEnd
                                       - attrBuf->offset
                                      + 1 ;
                flushSize = bytesTillEndOfBuffer + client->acqStart ;
            }
        }
        /* Flush all data in case hardFlush is TRUE */
        else {
            flushSize = control->validSize ;
        }

        control->emptySize +=  flushSize ;
        control->validSize -=  flushSize ;

        storedAcqStart = client->acqStart ;
        client->acqStart = (client->acqStart - flushSize + bufSize) % bufSize ;

        /* Return isFull is TRUE only when buffer is full */
        isFull = FALSE;
        if (   (flushSize == bufSize)
            && (client->acqStart == storedAcqStart)) {
            isFull = TRUE ;
        }

        /* Check if early end needs to be reset. */
        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIO_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd) ;
            control->curBufEnd = control->dataBufEnd ;
        }

        *bytesFlushed = flushSize ;

        /* Return the attribute info in case hardFlush is FALSE */
        if (hardFlush == FALSE) {
            *type  = attrBuf->type ;
            *param = attrBuf->param ;
        }
        else {
            *type  = RINGIO_INVALID_ATTR ;
            *param = (Uint32) 0 ;
        }

        if (flushSize != 0) {
            otherClient = &(control->reader) ;
            if (otherClient->isValid) {
                /* This check is to enable notification in
                 * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
                 * The pre-conditions are
                 * 1) Valid Data Size falls below watermark.
                 */
                if (   (otherClient->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE)
                    && (control->validSize < otherClient->notifyWaterMark)
                    && (otherClient->notifyFlag == FALSE)) {
                    otherClient->notifyFlag = TRUE ;
                }
            }
        }

        /* This check is to disable notification in
         * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Empty Data Size falls above watermark.
         */
        if (   (client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE)
                 && (control->emptySize >= client->notifyWaterMark)
                 && (client->notifyFlag == TRUE)){
            client->notifyFlag = FALSE ;
        }


        /* Reset buffer pointers if there is no data in buffer */
        _RingIO_initInstance (control) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerFlush
 *
 *  @desc   This function performs the flush operation from the reader side.
 *          This function discards any data from the top of the valid data
 *          buffer until the first unread attribute.
 *          This function also returns information associated with
 *          that attribute.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerFlush (RingIO_Client * client,
                     Bool            hardFlush,
                     Uint16 *        type,
                     Uint32 *        param,
                     Uint32 *        bytesFlushed)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Bool                 isFull  = FALSE ;
    Uint32               flushSize  = 0 ;
    RingIO_Attr *        attrBuf ;
    Uint32               flushAttrSize ;
    Uint32               bufSize ;
    Uint32               attrBufSize ;
    Uint32               storedAcqStart ;
    Uint32               storedAcqAttrStart ;

    DBC_require (client != NULL) ;
    DBC_require (type   != NULL) ;
    DBC_require (param  != NULL) ;
    DBC_require (bytesFlushed  != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    bufSize     = GET_CUR_DATA_END(control) ;
    attrBufSize = GET_CUR_ATTR_END(control) ;
    storedAcqStart  = client->acqStart ;
    storedAcqAttrStart = client->acqAttrStart ;

    /* If there are no attributes, flush out all valid data */
    if (control->validAttrSize == 0) {
        client->acqStart = ADJUST_MODULO (client->acqStart,
                                          control->validSize,
                                          bufSize) ;

        /* Return isFull is TRUE only when buffer is full */
        if (   (control->validSize == bufSize)
            && (client->acqStart == storedAcqStart)) {
            isFull = TRUE ;
        }

        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIO_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd) ;
            control->curBufEnd = control->dataBufEnd ;
        }

        control->emptySize += control->validSize ;
        *bytesFlushed       = control->validSize ;
        flushSize           = control->validSize ;
        control->validSize  = 0 ;

        *type = RINGIO_INVALID_ATTR ;
        *param              = (Uint32) 0 ;
    }
    else {
        if (hardFlush == TRUE) {
            flushSize = control->validSize ;
            flushAttrSize = control->validAttrSize ;
        }
        else {
            attrBuf = _RingIO_getFirstAttr (client) ;
            if (client->acqStart == attrBuf->offset) {
                /* Check if buffer is full */
                if (control->validSize == bufSize) {
                    flushSize = control->validSize ;
                }
                else {
                    flushSize = 0 ;
                }
            }
            else if (client->acqStart < attrBuf->offset) {
                flushSize = attrBuf->offset - client->acqStart ;
            }
            else {
                flushSize =   control->curBufEnd
                            - client->acqStart
                            + attrBuf->offset
                            + 1 ;
            }

            flushAttrSize = 0 ;
        }

        if (flushSize != 0) {
            client->acqStart    = ADJUST_MODULO (client->acqStart,
                                                 flushSize,
                                                 bufSize) ;
            control->validSize -= flushSize ;
            control->emptySize += flushSize ;
        }

        if (flushAttrSize != 0) {
            client->acqAttrStart =  ADJUST_MODULO (client->acqAttrStart,
                                                   flushAttrSize,
                                                   attrBufSize) ;
            control->validAttrSize -= flushAttrSize ;
            control->emptyAttrSize += flushAttrSize ;
        }

        /* Return the attribute info in case hardFlush is FALSE */
        if (hardFlush == FALSE) {
            *type  = attrBuf->type ;
            *param = attrBuf->param ;
            *bytesFlushed = flushSize;
        }
        else {
            *type  = RINGIO_INVALID_ATTR ;
            *param = (Uint32) 0 ;
            *bytesFlushed = flushSize;
        }

        /* Return isFull is TRUE only when buffer is full */
        isFull = FALSE;
        if (   (client->acqStart == storedAcqStart)
            && (flushSize == bufSize)){
            isFull = TRUE ;
        }

        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (_RingIO_isOffsetInBetween (control->curBufEnd,
                                            client->acqStart,
                                            storedAcqStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptySize.
             */
            control->emptySize += (control->dataBufEnd - control->curBufEnd) ;
            control->curBufEnd = control->dataBufEnd ;
        }

        /* Check isFull is FALSE when attribute buffer is full.
         * One condition is enough as this is in the else
         * clause for empty attribute buffer check. Only full
         * buffer condition needs to be checked here.
         */
        isFull = FALSE;
        if (client->acqAttrStart == storedAcqAttrStart) {
            isFull = TRUE ;
        }
        /* Check if early end needs to be reset in attribute buffer. */
        if (    (control->curAttrBufEnd != control->phyAttrBufEnd)
            &&  (_RingIO_isOffsetInBetween (control->curAttrBufEnd,
                                            client->acqAttrStart,
                                            storedAcqAttrStart,
                                            isFull))) {
            /* Early end was set in the region to be flushed. The early end
             * needs to be reset and the size added to emptyAttrSize.
             */
            control->emptyAttrSize += (     control->phyAttrBufEnd
                                       -    control->curAttrBufEnd) ;
            control->curAttrBufEnd = control->phyAttrBufEnd ;
        }

    }
    /* This check is to enable notification in
     * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
     * The pre-conditions are
     * 1) Valid Data Size falls below watermark.
     */
    if (flushSize != 0) {
        if ((client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE) &&
            (control->validSize < client->notifyWaterMark) &&
            (client->notifyFlag == FALSE)){
            client->notifyFlag = TRUE ;
        }
    }

    /* The notifyFlag for the writer client is not disabled here because
     * notification will be missed if the flag is disabled before sending
     * a notification. This will be disabled in function
     * _RingIO_checkNotifier which will be called as part of reader flush
     * transactions. This is valid for type
     * RINGIO_NOTIFICATION_HDWRFIFO_ONCE.
     */

    /* Reset buffer pointers if there is no data in buffer */
    _RingIO_initInstance (control) ;

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_writerCancel
 *
 *  @desc   This function cancels the previous acquire
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_writerCancel (RingIO_Client * client)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    RingIO_Attr *        attrBuf = NULL ;
    Uint32               bytesTillEndOfBuffer ;
    RingIO_BufPtr        dataBuf;


    DBC_require (client  != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    if (client->acqSize == 0) {
        /* Nothing acquired */
        status = RINGIO_EFAILURE;
    }
    else {
        /*
         * If any attributes had been set by the writer, update the
         * control structure appropriately to point to the
         * previous attribute buffer
         */
        attrBuf = _RingIO_getFirstAttr (client) ;
        if (attrBuf != NULL) {
            control->prevAttrOffset = attrBuf->prevoffset ;
        }

        /* Clear early end for data buffer, if set, in the following two cases:
         * 1. Early end is set in the acquired range to be cancelled.
         * 2. Client acqStart is at the start of data buffer. In this case, last
         *    acquire was larger than size remaining in buffer, hence early end
         *    was set. For next acquire, this may not be the case, so early end
         *    can be reset.
         * No check for isFull is needed here as this (client->acqStart
         * + client->acqSize)) is not a modulo operation and they can
         * never be equal at this point. The check for empty client->
         * acqSize = 0 is done earlier.
         */

        if (    (control->curBufEnd != control->dataBufEnd)
            &&  (    _RingIO_isOffsetInBetween (control->curBufEnd,
                                        client->acqStart,
                                        (client->acqStart + client->acqSize),
                                        FALSE)
                 ||  (client->acqStart == 0))) {
            /* If acqStart has moved to the start of data buffer and early end
             * is being cleared, the acqStart can be set back to early end
             * before resetting the early end.
             */
            if (client->acqStart == 0) {
                client->acqStart = GET_CUR_DATA_END(control) ;
                _RingIO_updateAttributes (client, 0) ;
            }
            control->emptySize += (control->dataBufEnd - control->curBufEnd) ;
            control->curBufEnd = control->dataBufEnd ;
        }

        bytesTillEndOfBuffer = control->curBufEnd - client->acqStart + 1 ;
        if (client->flags & RINGIO_DATABUF_CACHEUSE) {
            /* Invalidate cache */
            dataBuf = (RingIO_BufPtr) ((Uint32) client->pDataStart + client->acqStart) ;
            if (client->acqSize <= bytesTillEndOfBuffer) {
                HAL_cacheInv (dataBuf, client->acqSize) ;
            }
            else {
                /* Invalidate cache taking into consideration wraparound */
                HAL_cacheInv (dataBuf, bytesTillEndOfBuffer) ;
                HAL_cacheInv (client->pDataStart, (client->acqSize - bytesTillEndOfBuffer)) ;
            }
        }

        control->emptySize     += client->acqSize ;
        client->acqSize         = 0 ;

        /* Clear early end for attr buffer, if set, in the following two cases:
         * 1. Early end is set in the acquired range to be cancelled.
         * 2. Client acqAttrStart is at the start of data buffer. In this case,
         *    last acquire was larger than size remaining in buffer, hence early
         *    end was set. For next acquire, this may not be the case, so early
         *    end can be reset.
         * No check for isFull is needed here as this (client->acqStart
         * + client->acqSize)) is not a modulo operation and they can
         * never be equal at this point. The check for empty client->
         * acqSize = 0 is done earlier.
         */

        if (    (control->curAttrBufEnd != control->phyAttrBufEnd)
            &&  (    _RingIO_isOffsetInBetween (control->curAttrBufEnd,
                                   client->acqAttrStart,
                                   (client->acqAttrStart + client->acqAttrSize),
                                   FALSE)
                 ||  (client->acqAttrStart == 0))) {
            /* If acqAttrStart has moved to the start of data buffer and early
             * end is being cleared, the acqAttrStart can be set back to early
             * end before resetting the early end.
             */
            if (client->acqAttrStart == 0) {
                client->acqAttrStart = GET_CUR_ATTR_END (control) ;
            }
            control->emptyAttrSize += (     control->phyAttrBufEnd
                                       -    control->curAttrBufEnd) ;
            control->curAttrBufEnd = control->phyAttrBufEnd ;
        }

        control->emptyAttrSize += client->acqAttrSize ;
        client->acqAttrSize     = 0 ;

        /* This check is to disable notification in
         * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Empty Data Size falls above watermark.
         */
        if (   (client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE)
                 && (control->emptySize >= client->notifyWaterMark)
                 && (client->notifyFlag == TRUE)){
            client->notifyFlag = FALSE ;
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIO_initInstance (control) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_readerCancel
 *
 *  @desc   This function cancels the previous acquire
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_readerCancel (RingIO_Client * client)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;

    DBC_require (client  != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    if (client->acqSize == 0) {
        status = RINGIO_EFAILURE;
    }
    else {


        if (client->flags & RINGIO_DATABUF_CACHEUSE) {
            _RingIO_invalidateDataBuffer (client) ;
        }


        control->validSize     += client->acqSize ;
        control->validAttrSize += client->acqAttrSize ;
        client->acqSize         = 0 ;
        client->acqAttrSize     = 0 ;

        /* This check is to disable notification in
         * RINGIO_NOTIFICATION_HDWRFIFO_ONCE case:
         * The pre-conditions are
         * 1) Valid Data Size falls above watermark.
         */
        if (   (client->notifyType == RINGIO_NOTIFICATION_HDWRFIFO_ONCE)
                && (control->validSize >= client->notifyWaterMark)
                && (client->notifyFlag == TRUE)){
            client->notifyFlag = FALSE ;
        }

        /* Reset buffer pointers if there is no data in buffer */
        _RingIO_initInstance (control) ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_callNotifier
 *
 *  @desc   This function calls the notifier function registered by the
 *          client with the RingIO instance.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */

Int
_RingIO_callNotifier (RingIO_Client * client, RingIO_NotifyMsg msg)
{
    Int                  status      = RINGIO_SUCCESS ;
    RingIO_Client *      otherClient = NULL ;
    RingIO_ControlHandle control ;
    Uint32               entryId;

    DBC_require (client != NULL) ;

    /* Cache coherence is needed here, since this function is called outside
     * lock protection.
     */
    if (client->flags &  RINGIO_CONTROL_CACHEUSE) {
        HAL_cacheInv ((Ptr) (client), sizeof (RingIO_Client)) ;
    }
    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    status = _RingIO_acquireLock (client) ;
    if (status != SYS_OK) {
        SET_FAILURE_REASON (status) ;
    }
    else {
        if (client->flags &  RINGIO_CONTROL_CACHEUSE) {
            HAL_cacheInv ((Ptr) (control),
                          sizeof (RingIO_ControlStruct)) ;
        }
        status = _RingIO_releaseLock (client) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
        else {
            if (client->openMode == (Uint32) RINGIO_MODE_WRITE) {
                otherClient = &(control->reader) ;
            }
            else {
                otherClient = &(control->writer) ;
            }
            DBC_assert (otherClient != NULL) ;

            entryId  = ((Uint32) msg) << 16;
            entryId |= (control->entryId & 0xFFFF);

            /* Send notification only when other client
             * is valid and notification has been registered.
             */
            if (   (otherClient->isValid)
                && (otherClient->notifyFunc != NULL)) {
                if (control->transportType == (Uint32) RINGIO_TRANSPORT_GPP_DSP) {
                    status = NOTIFY_notify (otherClient->procId,
                            RingIO_ctrlPtr->ipsId,
                            RingIO_ctrlPtr->ipsEventNo,
                            entryId) ;
                }
                else {
                    otherClient->notifyFunc (otherClient,
                            otherClient->notifyParam,
                            msg) ;
                }
            }
            else {
                status = RINGIO_EFAILURE ;
            }
        }
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_checkNotifier
 *
 *  @desc   This function checks if conditions for invoking the notification
 *          callback are satisfied.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_checkNotifier (RingIO_Client * client)
{
    Int                  status  = RINGIO_EFAILURE ;
    RingIO_ControlHandle control = NULL ;
    RingIO_Client *      otherClient ;
    Uint32               currentBytes ;

    DBC_require (client != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    if (client->openMode == (Uint32) RINGIO_MODE_WRITE) {
        otherClient = &(control->reader);
        currentBytes = control->validSize;
    }
    else {
        otherClient = &(control->writer);
        currentBytes = control->emptySize;
    }

    if (otherClient->isValid) {
        switch (otherClient->notifyType) {
        case RINGIO_NOTIFICATION_ALWAYS:
            {
                if (   (otherClient->notifyFlag)
                    && (currentBytes >= otherClient->notifyWaterMark)) {
                    status = RINGIO_SUCCESS ;
                }
            }
            break ;

        case RINGIO_NOTIFICATION_ONCE:
        case RINGIO_NOTIFICATION_HDWRFIFO_ONCE:
            {
                if (   (otherClient->notifyFlag)
                    && (currentBytes >= otherClient->notifyWaterMark)) {
                    status = RINGIO_SUCCESS ;
                    otherClient->notifyFlag = FALSE ;
                }
            }
            break ;

         case RINGIO_NOTIFICATION_HDWRFIFO_ALWAYS:
            {
                if (currentBytes >= otherClient->notifyWaterMark) {
                    status = RINGIO_SUCCESS ;
                }
            }
            break;

         case RINGIO_NOTIFICATION_NONE:
            break ;
        }
    }
    else {
        status = RINGIO_EFAILURE ;
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_initInstance
 *
 *  @desc   This function initializes the RingIO instance identified by the
 *          control handle specified.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Void
_RingIO_initInstance (RingIO_ControlHandle control)
{
    if (   (control->emptySize == (GET_CUR_DATA_END(control)))
        && (control->validSize == 0)
        && (control->emptyAttrSize == (GET_CUR_ATTR_END(control)))
        && (control->validAttrSize == 0)) {
        control->validSize      = 0 ;
        control->emptySize      = (Uint32) control->dataBufEnd + 1 ;
        control->curBufEnd      = (Uint32) control->dataBufEnd ;

        control->validAttrSize  = 0 ;
        control->emptyAttrSize  = (Uint32) control->phyAttrBufEnd + 1 ;
        control->curAttrBufEnd  = (Uint32) control->phyAttrBufEnd ;

        control->prevAttrOffset = RINGIO_INVALID_OFFSET ;

        if (control->reader.isValid == TRUE) {
            control->reader.acqStart = 0 ;
            control->reader.acqSize  = 0 ;
            control->reader.acqAttrStart = 0 ;
            control->reader.acqAttrSize  = 0 ;
        }

        if (control->writer.isValid == TRUE) {
            control->writer.acqStart = 0 ;
            control->writer.acqSize  = 0 ;
            control->writer.acqAttrStart = 0 ;
            control->writer.acqAttrSize  = 0 ;
        }
    }
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_getFirstAcquiredAttr
 *
 *  @desc   This function returns the first attribute that is acquired by
 *          the reader or the writer
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
RingIO_Attr *
_RingIO_getFirstAcquiredAttr (RingIO_Client * client)
{
    RingIO_Attr *        attrBuf = NULL ;
    RingIO_ControlHandle control = NULL ;
    Uint32               attrBufSize ;


    DBC_require (client != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart != NULL) {
        if (client->acqAttrSize) {
            attrBufSize = GET_CUR_ATTR_END(control) ;
            attrBuf = (RingIO_Attr *)(  (Uint32) client->pAttrStart
                                      + ((client->acqAttrStart) % attrBufSize)) ;

            if (client->openMode == (Uint32) RINGIO_MODE_READ) {
                if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                    HAL_cacheInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
                }
            }
        }
    }

    return attrBuf ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_getFirstAttr
 *
 *  @desc   This function returns the next attribute that can be acquired by
 *          the reader or the writer
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
RingIO_Attr *
_RingIO_getFirstAttr (RingIO_Client * client)
{
    RingIO_Attr *        attrBuf = NULL ;
    RingIO_ControlHandle control = NULL ;
    Uint32               attrBufSize ;

    DBC_require (client != NULL) ;


    /* Check if attribute buffer has been provided. */
    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;
    attrBufSize = GET_CUR_ATTR_END(control) ;

    if (client->pAttrStart != NULL) {
        if (client->openMode == (Uint32) RINGIO_MODE_WRITE) {
            if (client->acqAttrSize != 0) {
                attrBuf = (RingIO_Attr *) (  (Uint32) client->pAttrStart
                                           + (  (client->acqAttrStart)
                                              % attrBufSize)) ;
            }
        }
        else {
            if (control->validAttrSize) {
                attrBuf = (RingIO_Attr *) (  (Uint32)client->pAttrStart
                                           + (  (  client->acqAttrStart
                                                 + client->acqAttrSize)
                                              % attrBufSize)) ;

                if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                    HAL_cacheInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
                }
            }
        }
    }

    return attrBuf ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_releaseAttribute
 *
 *  @desc   This function releases an attribute.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_releaseAttribute (RingIO_Client * client, RingIO_Attr * attrBuf)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;

    DBC_require (client    != NULL) ;
    DBC_require (attrBuf   != NULL) ;

    /* Check if attribute buffer has been provided. */
    if (client->pAttrStart == NULL) {
        status = RINGIO_EFAILURE ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        control = client->virtControlHandle ;
        DBC_assert (control != NULL) ;

        /* Check if reader has read all valid attributes so far. In this case,
         * when releasing the new attribute, its prevOffset needs to be set
         * to invalid.
         *
         * For wtiter this condition  implies that if it is the first attribute
         * to be released the attrbuf and there are no valid attributes except
         * current attribute that is to be released.
         *
         * For reader this condition  implies that  there are no valid
         * attributes . Setting prevOffset to RINGIO_INVALID_OFFSET
         * ( This is to allow for easier debugging).
         */
        if (control->validAttrSize == 0) {
            attrBuf->prevoffset = RINGIO_INVALID_OFFSET ;
        }

        client->acqAttrStart =  ((  client->acqAttrStart
                                  + (SIZEOF_ATTR (attrBuf->size)))
                                % (GET_CUR_ATTR_END(control))) ;

        client->acqAttrSize    -= (SIZEOF_ATTR(attrBuf->size)) ;

        if (client->openMode == (Uint32) RINGIO_MODE_WRITE) {
            control->validAttrSize += (SIZEOF_ATTR(attrBuf->size)) ;
        }
        else {
            control->emptyAttrSize += (SIZEOF_ATTR(attrBuf->size)) ;
        }

        if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
            HAL_cacheWbInv ((Ptr) attrBuf,
                            (SIZEOF_ATTR(attrBuf->size))) ;
        }
    }

    return status ;
}


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_getContiguousDataSize
 *
 *  @desc   This function returns the amount of contiguous data available for
 *          the reader to acquire. This is the minimum of
 *          1) the number of bytes available before encountering next attribute
 *          2) the number of bytes before encountering end of buffer
 *          3) total valid data available in the buffer
 *          This should be called only by the reader.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_getContiguousDataSize (RingIO_Client * client, Uint32 * size)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Uint32               bufSize = 0 ;
    RingIO_Attr *        attrBuf ;
    Uint32               bytesTillEndOfBuffer ;
    Uint32               nextAcqOffset ;
    Uint32               contiguousBytes ;
    Uint32               totalSpaceAvailable ;
    Uint32               bytesTillNextAttr ;

    DBC_require (client != NULL) ;
    DBC_require (size   != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;
    bufSize = GET_CUR_DATA_END(control) ;

    nextAcqOffset = ADJUST_MODULO (client->acqStart, client->acqSize, bufSize) ;
    bytesTillEndOfBuffer = control->curBufEnd - nextAcqOffset + 1 ;

    totalSpaceAvailable =  bytesTillEndOfBuffer ;

    if (control->footBufSize != 0) {
        totalSpaceAvailable +=  control->footBufSize
                              + control->dataBufSize
                              - (control->curBufEnd + 1) ;
    }

    contiguousBytes = RINGIO_min (totalSpaceAvailable, control->validSize) ;

    attrBuf = _RingIO_getFirstAttr (client) ;
    if (attrBuf == NULL) {
        *size = contiguousBytes ;
    }
    else {
        /*
         * Find number of valid bytes before next attribute
         * 1. Check if the attribute is *not* at the current acquire offset.
         * 2. Check if the buffer is wrapping around and that the attribute
         *    is set at the current 'end' (could be early-end) of the data
         *    buffer. In this case the attribute is *actually* set at end of
         *    buffer but due to early end it is logically set at offset '0'.
         */

        /* This is the wrap around case. Attribute is at the top of the buffer
         * and reader is still at the bottom of the buffer
         */
        if (nextAcqOffset > attrBuf->offset) {
            bytesTillNextAttr = attrBuf->offset + bytesTillEndOfBuffer ;
        }
        /* Attribute set at the logical end of the buffer */
        else if ((nextAcqOffset == 0) && (attrBuf->offset == bufSize)) {
            bytesTillNextAttr = 0;
        }
        /* Regular case when attribute buffer is after reader acquire offset */
        else {
            bytesTillNextAttr = attrBuf->offset - nextAcqOffset;
        }

        *size = RINGIO_min (contiguousBytes, bytesTillNextAttr) ;

        if (bytesTillNextAttr <= contiguousBytes) {
            status = RINGIO_SPENDINGATTRIBUTE ;
        }
    }

    return status ;
}


/*  ----------------------------------------------------------------------------
 *  @func   _RingIO_updateAttributes
 *
 *  @desc   This function updates the attribute's offset in early end scenarios.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Void
_RingIO_updateAttributes (RingIO_Client * client, Uint32 offset)
{
    RingIO_ControlHandle control = NULL ;
    RingIO_Client *      otherClient = NULL ;
    RingIO_Attr *        attrBuf ;
    Uint32               size  ;
    Uint32               attrBufSize ;

    DBC_require (client  != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;

    size = control->validAttrSize ;
    attrBufSize = GET_CUR_ATTR_END(control) ;

    /* Get the address of first released attribute, if any, that may need to be
     * updated.
     */
    if (size > 0) {
        if (client->openMode == (Uint32) RINGIO_MODE_WRITE) {
            /* For writer, the first released attribute is to be taken. It
             * is the first attribute that can be acquired by the reader.
             */
            otherClient = &(control->reader) ;
            attrBuf = (RingIO_Attr *) (  (Uint32) client->pAttrStart
                                       + (  (  otherClient->acqAttrStart
                                             + otherClient->acqAttrSize)
                                          % attrBufSize)) ;
        }
        else {
            /* For reader, the first released attribute is the first that can be
             * acquired by it.
             */
            attrBuf = (RingIO_Attr *) (  (Uint32) client->pAttrStart
                                       + (  (  client->acqAttrStart
                                             + client->acqAttrSize)
                                          % attrBufSize)) ;
        }
    }

    while (size > 0) {
        if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
            HAL_cacheInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
        }
        if (attrBuf->offset == offset) {
            if (offset == 0) {
                attrBuf->offset = (GET_CUR_DATA_END(control)) ;
            }
            else if (attrBuf->offset == GET_CUR_DATA_END(control)) {
                attrBuf->offset = 0 ;
            }

            if (client->flags & RINGIO_ATTRBUF_CACHEUSE) {
                HAL_cacheWbInv ((Ptr) attrBuf, sizeof (RingIO_Attr)) ;
            }
        }

        size -= (SIZEOF_ATTR(attrBuf->size)) ;
        attrBuf = (RingIO_Attr *) (  (Uint32) client->pAttrStart
                                   + (  (  (  ((Uint32) attrBuf )
                                            - ((Uint32) client->pAttrStart))
                                         + attrBuf->size
                                         + sizeof (RingIO_Attr))
                                      % attrBufSize)) ;
    }
}


/** ----------------------------------------------------------------------------
 *  @func   _RingIO_invalidateDataBuffer
 *
 *  @desc   This function invalidates the data buffer for
 *          in place buffer operations.
 *          This should be called only by the reader.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
Int
_RingIO_invalidateDataBuffer (RingIO_Client * client)
{
    Int                  status  = RINGIO_SUCCESS ;
    RingIO_ControlHandle control = NULL ;
    Uint32               bufSize = 0 ;
    Uint32               bytesTillEndOfBuffer ;
    Uint32               bytesFromTopOfBuffer ;
    Uint32               nextAcqOffset ;
    RingIO_BufPtr        dataBuf;

    DBC_require (client != NULL) ;

    control = client->virtControlHandle ;
    DBC_assert (control != NULL) ;
    bufSize = GET_CUR_DATA_END(control) ;

    nextAcqOffset = ADJUST_MODULO (client->acqStart, client->acqSize, bufSize) ;
    bytesFromTopOfBuffer = nextAcqOffset + 1 ;
    /*  Find total contiguous space available. */
    bytesTillEndOfBuffer = (  bufSize
                            - ((  ADJUST_MODULO (client->acqStart,
                                                 0,
                                                 bufSize)))) ;


    dataBuf = (RingIO_BufPtr) ((Uint32) client->pDataStart + client->acqStart) ;

    if (client->acqSize <= bytesTillEndOfBuffer) {
        HAL_cacheInv (dataBuf, client->acqSize) ;
    }
    else {
        /* Invalidate cache taking into consideration wraparound */
        HAL_cacheInv (dataBuf, bytesTillEndOfBuffer) ;
        HAL_cacheInv (client->pDataStart, bytesFromTopOfBuffer);
    }

    return status ;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
