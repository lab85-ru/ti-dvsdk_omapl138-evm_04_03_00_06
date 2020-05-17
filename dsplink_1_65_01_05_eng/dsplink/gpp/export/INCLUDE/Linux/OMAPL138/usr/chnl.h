/** ============================================================================
 *  @file   chnl.h
 *
 *  @path   $(DSPLINK)/gpp/inc/usr/
 *
 *  @desc   Defines the interfaces and data structures for the API sub-component
 *          CHNL.
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


#if !defined (CHNL_H)
#define  CHNL_H


/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>
#include <chnldefs.h>

/*  ----------------------------------- Profiling                       */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   CHNL_create
 *
 *  @desc   Creates resources used for transferring data between GPP and DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Id to open.
 *  @arg    attrs
 *              Channel attributes - if NULL, default attributes are applied.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          Attributes must be valid.
 *          PROC_load  has been successful.
 *          POOL_open i.e. POOL configuration has been successful.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_create (IN ProcessorId      procId,
             IN ChannelId        chnlId,
             IN ChannelAttrs *   attrs) ;


/** ============================================================================
 *  @func   CHNL_delete
 *
 *  @desc   Releases channel resources used for transferring data between GPP
 *          and DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid Parameter passed.
 *
 *  @enter  Channels for specified processors should have been initialized.
 *          Processor and  channel ids should be valid.
 *          All CHNL operations are complete.
 *
 *  @leave  None
 *
 *  @see    CHNL_create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_delete (IN ProcessorId    procId,
             IN ChannelId      chnlId) ;


/** ============================================================================
 *  @func   CHNL_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    size
 *              Size of each buffer.
 *  @arg    bufArray
 *              Pointer to receive array of allocated buffers.
 *  @arg    numBufs
 *              Number of buffers to allocate.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          size must not be zero.
 *          numBufs must be less than/equal to MAX_ALLOC_BUFFERS.
 *          CHNL_create has been successful.
 *          POOL_open i.e. POOL configuration has been successful.
 *
 *  @leave  None
 *
 *  @see    CHNL_create, CHNL_freeBuffer
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_allocateBuffer (IN  ProcessorId procId,
                     IN  ChannelId   chnlId,
                     OUT Char8 **    bufArray,
                     IN  Uint32      size,
                     IN  Uint32      numBufs) ;


/** ============================================================================
 *  @func   CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by CHNL_allocateBuffer.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    bufArray
 *              Pointer to the array of buffers to freed.
 *  @arg    numBufs
 *              Number of buffers to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than/equal to MAX_ALLOC_BUFFERS.
 *          All data transfer to DSP using CHNL is complete.
 *
 *  @leave  None
 *
 *  @see    CHNL_create, CHNL_allocateBuffer
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_freeBuffer (IN ProcessorId procId,
                 IN ChannelId   chnlId,
                 IN Char8 **    bufArray,
                 IN Uint32      numBufs) ;


/** ============================================================================
 *  @func   CHNL_issue
 *
 *  @desc   Issues an input or output request on a specified channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    ioReq
 *              Information regarding IO.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          ioReq must be valid.
 *          CHNL_create, CHNL_allocateBuffer has been successful.
 *          Corresponding data stream on dsp side has been created.
 *          Data buffer is prime'd for data transfer
 *
 *  @leave  Corresponding data reclaim on gpp/dsp side is done.
 *
 *  @see    CHNL_reclaim
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_issue (IN ProcessorId      procId,
            IN ChannelId        chnlId,
            IN ChannelIOInfo *  ioReq) ;


/** ============================================================================
 *  @func   CHNL_reclaim
 *
 *  @desc   Gets the buffer back that has been issued to this channel.
 *          This call blocks for specified timeout value ranging from NO_WAIT
 *          to WAIT_FOREVER.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    timeout
 *              Timeout value for this operation.
 *              Unit of timeout is OS dependent.
 *  @arg    ioReq
 *              Information needed for doing reclaim.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *          CHNL_E_NOIOC
 *              Timeout parameter was "NO_WAIT", yet no I/O completions were
 *              queued.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          CCHNL_issue has been successful.
 *
 *  @leave  None
 *
 *  @see    CHNL_issue
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_reclaim (IN     ProcessorId       procId,
              IN     ChannelId         chnlId,
              IN     Uint32            timeout,
              IN OUT ChannelIOInfo *   ioReq) ;


/** ============================================================================
 *  @func   CHNL_idle
 *
 *  @desc   In the input mode channel, this function resets the channel
 *          and causes any currently buffered input data to be discarded.
 *
 *          In the output mode channel, this function causes any currently
 *          queued buffers to be transferred through the channel. It causes
 *          the client to wait for as long as it takes for the data to be
 *          transferred through the channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *          CHNL_create, CHNL_allocateBuffer, CHNL_open has been successful.
 *
 *  @leave  None
 *
 *  @see    CHNL_create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_idle (IN ProcessorId  procId,
           IN ChannelId    chnlId) ;


/** ============================================================================
 *  @func   CHNL_flush
 *
 *  @desc   Discards all the requested buffers that are pending for transfer
 *          both in case of input mode channel as well as output mode channel.
 *          One must still have to call the CHNL_reclaim to get back the
 *          discarded buffers.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *          CHNL_issue has been successful.
 *
 *  @leave  None
 *
 *  @see    CHNL_create, CHNL_issue
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_flush (IN ProcessorId         procId,
            IN ChannelId           chnlId) ;


/** ============================================================================
 *  @func   CHNL_control
 *
 *  @desc   Provides a hook to perform device dependent control operations
 *          on channels.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    cmd
 *              Command id.
 *  @arg    arg
 *              Optional argument for the specified command.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          DSP_EINVALIDARG
 *              Invalid parameter passed.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *          CHNL_create, CHNL_allocateBuffer has been successful.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_control (IN  ProcessorId    procId,
              IN  ChannelId      chnlId,
              IN  Int32          cmd,
                  Pvoid          arg) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   CHNL_instrument
 *
 *  @desc   Gets the instrumentation information related to CHNL's
 *
 *  @arg    procId
 *              Identifier for processor.
 *  @arg    chnlId
 *              Identifier for channel for which instrumentation information
 *              is to be obtained.
 *  @arg    retVal
 *              OUT argument to contain the instrumentation information.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              retVal is invalid.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *          retVal must be a valid pointer.
 *          CHNL_create has been successful.
 *          Profiling has been selected in the configuration.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
CHNL_instrument (IN  ProcessorId       procId,
                 IN  ChannelId         chnlId,
                 OUT CHNL_Instrument * retVal) ;
#endif /* #if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   CHNL_debug
 *
 *  @desc   Prints the current status of CHNL subcomponent.
 *
 *  @arg    procId
 *              Identifier for processor.
 *  @arg    chnlId
 *              Identifier for channel.
 *
 *  @ret    None
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *          CHNL_create has been successful.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Void
CHNL_debug (IN ProcessorId procId,
            IN ChannelId   chnlId) ;
#endif /* defined (DDSP_DEBUG) */


/** ============================================================================
 *  @deprecated The deprecated API CHNL_Create has been replaced
 *              with CHNL_create.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Create                CHNL_create

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Delete has been replaced
 *              with CHNL_delete.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Delete                CHNL_delete

/** ============================================================================
 *  @deprecated The deprecated API CHNL_AllocateBuffer has been replaced
 *              with CHNL_allocateBuffer.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_AllocateBuffer        CHNL_allocateBuffer

/** ============================================================================
 *  @deprecated The deprecated API CHNL_FreeBuffer has been replaced
 *              with CHNL_freeBuffer.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_FreeBuffer            CHNL_freeBuffer

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Issue has been replaced
 *              with CHNL_issue.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Issue                 CHNL_issue

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Reclaim has been replaced
 *              with CHNL_reclaim.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Reclaim               CHNL_reclaim

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Idle has been replaced
 *              with CHNL_idle.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Idle                  CHNL_idle

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Flush has been replaced
 *              with CHNL_flush.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Flush                 CHNL_flush

/** ============================================================================
 *  @deprecated The deprecated API CHNL_Control has been replaced
 *              with CHNL_control.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Control               CHNL_control

#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @deprecated The deprecated API CHNL_Debug has been replaced
 *              with CHNL_debug.
 *              This has been done to follow DSP/BIOS coding guidelines and
 *              have a uniform naming convention for the API's on both GPP
 *              as well as DSP.
 *
 *  ============================================================================
 */
#define CHNL_Debug                 CHNL_debug
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (CHNL_H) */
