/** ============================================================================
 *  @file   pmgr_chnl.h
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   Defines the interfaces and data structures for the sub-component
 *          PMGR_CHNL.
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


#if !defined (PMGR_CHNL_H)
#define  PMGR_CHNL_H

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <chnldefs.h>

/*  ----------------------------------- Profiling                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   PMGR_CHNL_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    procId
 *              Processor Id.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Processor Id must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_exit, PMGR_CHNL_create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_init (IN ProcessorId    procId) ;


/** ============================================================================
 *  @func   PMGR_CHNL_exit
 *
 *  @desc   This function finalizes the PMGR CHNL module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    procId
 *              Processor Id.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor Id must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init, PMGR_CHNL_create, PMGR_CHNL_Destroy
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_exit (IN ProcessorId    procId) ;


/** ============================================================================
 *  @func   PMGR_CHNL_create
 *
 *  @desc   Creates resources used for transferring data between GPP and DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Id to open.
 *  @arg    attrs
 *              Channel attributes.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          Attributes must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_create (IN ProcessorId    procId,
                  IN ChannelId      chnlId,
                  IN     ChannelAttrs * attrs,
                  IN     Void *         optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_delete
 *
 *  @desc   Releases channel resources used for transferring data between GPP
 *          and DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processors should have been initialized.
 *          Processor and  channel ids should be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_delete (IN ProcessorId    procId,
                  IN     ChannelId   chnlId,
                  IN     Void *      optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_allocateBuffer
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
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init, PMGR_CHNL_create, PMGR_CHNL_freeBuffer
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_allocateBuffer (IN  ProcessorId procId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN     Uint32      numBufs,
                          IN     Void *      optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by PMGR_CHNL_allocateBuffer.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    bufArray
 *              Pointer to the array of buffers to freed.
 *  @arg    numBufs
 *              Number of buffers to be freed.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init, PMGR_CHNL_create, PMGR_CHNL_allocateBuffer
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_freeBuffer (IN ProcessorId procId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN     Uint32      numBufs,
                      IN     Void *      optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_issue
 *
 *  @desc   Issues an input or output request on a specified channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    ioReq
 *              Information regarding IO.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          ioReq must be valid pointer.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_reclaim
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_issue (IN ProcessorId      procId,
                 IN ChannelId        chnlId,
                 IN     ChannelIOInfo *  ioReq,
                 IN     Void *           optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_reclaim
 *
 *  @desc   Gets the buffer back that has been issued to this channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    timeout
 *              Timeout value for this operation.
 *  @arg    ioReq
 *              Information needed for doing reclaim.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *          CHNL_E_NOIOC
 *              Timeout parameter was "NO_WAIT", yet no I/O completions were
 *              queued.
 *
 *  @enter  Channels for specified processors must be initialized.
 *          Processor and  channel ids must be valid.
 *          ioReq must be valid pointer.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_issue
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_reclaim (IN     ProcessorId       procId,
                   IN     ChannelId         chnlId,
                   IN     Uint32            timeout,
                   IN OUT ChannelIOInfo *   ioReq,
                   IN     Void *            optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_idle
 *
 *  @desc   If the channel is an input stream this function resets the channel
 *          and causes any currently buffered input data to be discarded.  If
 *          the channel is an output channel, this function causes any
 *          currently queued buffers to be transferred through the channel. It
 *          causes the client to wait for as long as it takes for the data to
 *          be transferred through the channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init, PMGR_CHNL_create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_idle (IN ProcessorId  procId,
                IN     ChannelId    chnlId,
                IN     Void *       optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_flush
 *
 *  @desc   Discards all the requested buffers that are pending for transfer
 *          both in case of input mode channel as well as output mode channel.
 *          One must still have to call the PMGR_CHNL_reclaim to get back the
 *          discarded buffers.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    optArgs
 *              Optional arguments
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *          DSP_EMEMORY
 *              Operation failed due to memory error.
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init, PMGR_CHNL_create, PMGR_CHNL_issue
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_flush (IN ProcessorId         procId,
                 IN     ChannelId   chnlId,
                 IN     Void *      optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_control
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
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_control (IN  ProcessorId    procId,
                   IN  ChannelId      chnlId,
                   IN  Int32          cmd,
                       Pvoid          arg) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_CHNL_instrument
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
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_instrument (IN  ProcessorId       procId,
                      IN  ChannelId         chnlId,
                      OUT CHNL_Instrument * retVal) ;
#endif /* #if defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_CHNL_debug
 *
 *  @desc   Prints the current status of CHNL subcomponent.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    None
 *
 *  @enter  Channels for specified processor must be initialized.
 *          Processor and  channel ids must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init
 *  ============================================================================
 */
NORMAL_API
Void
PMGR_CHNL_debug (ProcessorId procId, ChannelId chnlId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (PMGR_CHNL_H) */

