/** ============================================================================
 *  @file   chnl.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   This file implements the functions of CHNL API subcomponent.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <procdefs.h>
#include <chnldefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Profiling                       */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Header                   */
#include <drvdefs.h>
#include <drv_api.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_CHNL

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_CHNL, __LINE__)

/** ============================================================================
 *  @macro  IS_VALID_CHNL_ATTRS
 *
 *  @desc   Check for validity of channel attributes.
 *  ============================================================================
 */
#define IS_VALID_CHNL_ATTRS(attrs)                                  \
             (   ((attrs) == NULL)                                    \
              || (   (   ((attrs)->endianism == Endianism_Default)    \
                      || ((attrs)->endianism == Endianism_Big)        \
                      || ((attrs)->endianism == Endianism_Little))    \
                  && (   ((attrs)->mode == ChannelMode_Input)         \
                      || ((attrs)->mode == ChannelMode_Output))       \
                  && (   ((attrs)->size == ChannelDataSize_16bits)    \
                      || ((attrs)->size == ChannelDataSize_32bits))))



/** ============================================================================
 *  @func   CHNL_create
 *
 *  @desc   Creates resources used for transferring data between GPP and DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_create (IN ProcessorId     procId,
             IN ChannelId       chnlId,
             IN ChannelAttrs *  attrs)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_3ENTER ("CHNL_create", procId, chnlId, attrs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (IS_VALID_CHNL_ATTRS (attrs)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (IS_VALID_CHNL_ATTRS (attrs)      == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlCreateArgs.procId = procId ;
        args.apiArgs.chnlCreateArgs.chnlId = chnlId ;
        args.apiArgs.chnlCreateArgs.attrs  = attrs ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_CREATE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_delete
 *
 *  @desc   Releases channel resources used for transferring data between GPP
 *          and DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_delete (IN ProcessorId  procId,
             IN ChannelId    chnlId)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("CHNL_delete", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlDeleteArgs.procId = procId ;
        args.apiArgs.chnlDeleteArgs.chnlId = chnlId ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_DELETE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_allocateBuffer (IN  ProcessorId procId,
                     IN  ChannelId   chnlId,
                     OUT Char8 **    bufArray,
                     IN  Uint32      size,
                     IN  Uint32      numBufs)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_5ENTER ("CHNL_allocateBuffer",
                procId,
                chnlId,
                size,
                bufArray,
                numBufs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (size     != 0) ;
    DBC_Require (numBufs  <= MAX_ALLOC_BUFFERS) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (bufArray == NULL)
        || (size     == 0)
        || (numBufs == 0 )
        || (numBufs  > MAX_ALLOC_BUFFERS)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlAllocateBufferArgs.procId   = procId   ;
        args.apiArgs.chnlAllocateBufferArgs.chnlId   = chnlId   ;
        args.apiArgs.chnlAllocateBufferArgs.bufArray = bufArray ;
        args.apiArgs.chnlAllocateBufferArgs.size     = size     ;
        args.apiArgs.chnlAllocateBufferArgs.numBufs  = numBufs  ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_ALLOCATEBUFFER, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_allocateBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by CHNL_allocateBuffer.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_freeBuffer (IN ProcessorId procId,
                 IN ChannelId   chnlId,
                 IN Char8 **    bufArray,
                 IN Uint32      numBufs)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_4ENTER ("CHNL_freeBuffer", procId, chnlId, bufArray, numBufs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs <= MAX_ALLOC_BUFFERS) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (bufArray == NULL)
        || (numBufs == 0)
        || (numBufs > MAX_ALLOC_BUFFERS)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlFreeBufferArgs.procId   = procId   ;
        args.apiArgs.chnlFreeBufferArgs.chnlId   = chnlId   ;
        args.apiArgs.chnlFreeBufferArgs.bufArray = bufArray ;
        args.apiArgs.chnlFreeBufferArgs.numBufs  = numBufs  ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_FREEBUFFER, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_freeBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_issue
 *
 *  @desc   Issues an input or output request on a specified channel.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_issue (IN ProcessorId     procId,
            IN ChannelId       chnlId,
            IN ChannelIOInfo * ioReq)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_3ENTER ("CHNL_issue", procId, chnlId, ioReq) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioReq != NULL) ;
    DBC_Require ((ioReq != NULL) && (ioReq->buffer != NULL)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (ioReq == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (ioReq->buffer == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlIssueArgs.procId = procId ;
        args.apiArgs.chnlIssueArgs.chnlId = chnlId ;
        args.apiArgs.chnlIssueArgs.ioReq  = ioReq  ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_ISSUE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_issue", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_reclaim
 *
 *  @desc   Gets the buffer back that has been issued to this channel.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_reclaim (IN     ProcessorId      procId,
              IN     ChannelId        chnlId,
              IN     Uint32           timeout,
              IN OUT ChannelIOInfo *  ioReq)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_4ENTER ("CHNL_reclaim", procId, chnlId, timeout, ioReq) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioReq != NULL) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (ioReq == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlReclaimArgs.procId = procId   ;
        args.apiArgs.chnlReclaimArgs.chnlId = chnlId   ;
        args.apiArgs.chnlReclaimArgs.timeout = timeout ;
        args.apiArgs.chnlReclaimArgs.ioReq  = ioReq    ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_RECLAIM, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_reclaim", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_idle
 *
 *  @desc   If the channel is an input stream this function resets the channel
 *          and causes any currently buffered input data to be discarded.  If
 *          the channel is an output channel, this function causes any
 *          currently queued buffers to be transferred through the channel. It
 *          causes the client to wait for as long as it takes for the data to
 *          be transferred through the channel.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_idle (IN ProcessorId          procId,
           IN ChannelId            chnlId)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("CHNL_idle", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlIdleArgs.procId = procId   ;
        args.apiArgs.chnlIdleArgs.chnlId = chnlId   ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_IDLE, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_flush
 *
 *  @desc   Discards all the requested buffers that are pending for transfer
 *          both in case of input mode channel as well as output mode channel.
 *          One must still have to call the CHNL_reclaim to get back the
 *          discarded buffers.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_flush (IN ProcessorId procId,
            IN ChannelId   chnlId)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_2ENTER ("CHNL_flush", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlFlushArgs.procId = procId   ;
        args.apiArgs.chnlFlushArgs.chnlId = chnlId   ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_FLUSH, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_flush", status) ;

    return status ;
}


/** ============================================================================
 *  @func   CHNL_control
 *
 *  @desc   Provides a hook to perform device dependent control operations
 *          on channels.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_control (IN  ProcessorId  procId,
              IN  ChannelId    chnlId,
              IN  Int32        cmd,
                  Pvoid        arg)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_4ENTER ("CHNL_control", procId, chnlId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlControlArgs.procId = procId ;
        args.apiArgs.chnlControlArgs.chnlId = chnlId ;
        args.apiArgs.chnlControlArgs.cmd    = cmd    ;
        args.apiArgs.chnlControlArgs.arg    = arg    ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_CONTROL, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_control", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   CHNL_instrument
 *
 *  @desc   Gets the instrumentation information related to PROC's
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_instrument (IN  ProcessorId       procId,
                 IN  ChannelId         chnlId,
                 OUT CHNL_Instrument * retVal)
{
    DSP_STATUS status = DSP_SOK ;
    CMD_Args   args             ;

    TRC_3ENTER ("CHNL_instrument", procId, chnlId, retVal) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (retVal != NULL) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)
        || (retVal == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlInstrumentArgs.procId    = procId ;
        args.apiArgs.chnlInstrumentArgs.chnlId    = chnlId ;
        args.apiArgs.chnlInstrumentArgs.chnlStats = retVal ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_INSTRUMENT, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("CHNL_instrument", status) ;

    return status ;
}
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   CHNL_debug
 *
 *  @desc   Prints the current status of CHNL component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
CHNL_debug (IN ProcessorId procId,
            IN ChannelId   chnlId)
{
    DSP_STATUS  status = DSP_SOK ;
    CMD_Args    args             ;

    TRC_0ENTER ("CHNL_debug") ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (   (IS_VALID_PROCID (procId)         == FALSE)
        || (IS_VALID_CHNLID (procId, chnlId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        args.apiArgs.chnlDebugArgs.procId = procId ;
        args.apiArgs.chnlDebugArgs.chnlId = chnlId ;

        status = DRV_INVOKE (DRV_handle, CMD_CHNL_DEBUG, &args) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_0LEAVE ("CHNL_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
