/** ============================================================================
 *  @file   ldrv_chnl.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Link driver's channel module interface.
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


#if !defined (LDRV_CHNL_H)
#define LDRV_CHNL_H


/*  ----------------------------------- Link Driver Headers         */
#include <dsplink.h>
#include <list.h>
#include <chnldefs.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  IO Completion State flags.
 *
 *  @desc   Status of completion.
 *  ============================================================================
 */
#define LDRV_CHNL_IOCSTATE_COMPLETE     0x0000u  /* IO Completed.             */
#define LDRV_CHNL_IOCSTATE_PENDING      0x0002u  /* IO is pending             */
#define LDRV_CHNL_IOCSTATE_CANCELED     0x0004u  /* IO was cancelled          */
#define LDRV_CHNL_IOCSTATE_TIMEOUT      0x0008u  /* Wait for IOC timed out.   */
#define LDRV_CHNL_IOCSTATE_INPROGRESS   0x0010u  /* IO is in progress between
                                                  * GPP and DSP
                                                  */
#define LDRV_CHNL_IOCSTATE_EOS          0x8000u  /* End Of Stream reached.    */


/** ============================================================================
 *  @name   CHNL_State
 *
 *  @desc   Channel State type.
 *
 *  @field  ChannelState_Ready
 *              Indicates channel is ready.
 *  @field  ChannelState_Idled
 *              Indicates channel is idled.
 *  @field  ChannelState_EOS
 *              Indicates channel is in End of Stream state.
 *  @field  ChannelState_Closed
 *              Indicates channel is in closed state.
 *  ============================================================================
 */
typedef enum  {
    ChannelState_Ready  = 0x01,
    ChannelState_Idled  = 0x02,
    ChannelState_EOS    = 0x04,
    ChannelState_Closed = 0x08
} CHNL_State ;

/** ============================================================================
 *  @name   IO_State
 *
 *  @desc   Completion state of IO on a channel.
 *
 *  @field  IOState_Completed
 *              Indicates completion of IO for an IO request on a channel.
 *  @field  IOState_NotCompleted
 *              Indicates non-completion of IO for an IO request on a channel.
 *  ============================================================================
 */
typedef enum  {
    IOState_Completed    = 1,
    IOState_NotCompleted = 2
} IO_State ;


/** ============================================================================
 *  @name   LDRV_CHNL_IOInfo
 *
 *  @desc   LDRV Channel IO information structure.
 *
 *  @field  buffer
 *              Pointer to data buffer.
 *  @field  size
 *              Size of the data buffer.
 *  @field  arg
 *              Argument to be sent or received.
 *  @field  completionStatus
 *              Completion status of this IO request.
 *  ============================================================================
 */
typedef struct LDRVChnlIOInfo_tag {
    Pvoid               buffer            ;
    Uint32              size              ;
    Uint32              arg               ;
    IO_State            completionStatus  ;
} LDRV_CHNL_IOInfo ;

/** ============================================================================
 *  @name   LDRV_CHNL_Irp
 *
 *  @desc   CHIRP (Channel Input/output Request Packet) data structure.
 *
 *  @field  link
 *              List element header needed for this structure.
 *  @field  buffer
 *              Buffer to be filled/emptied.
 *  @field  arg
 *              Issue reclaim argument.
 *  @field  size
 *              Buffer length.
 *  @field  iocStatus
 *              Status of IO Completion.
 *  @field  chnlId
 *              ChannelID
 *  ============================================================================
 */
typedef struct LDRVChnlIRP_tag {
    ListElement         link          ;
    Uint32              buffer        ;
    Uint32              arg           ;
    Uint32              size          ;
    Uint32              iocStatus     ;
    ChannelId           chnlId        ;
} LDRV_CHNL_Irp ;


/** ============================================================================
 *  @func   LDRV_CHNL_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    procId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_CHNL_exit
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_init (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_exit
 *
 *  @desc   This function finalizes the LDRV CHNL module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    procId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_CHNL_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_exit (IN ProcessorId procId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_open
 *
 *  @desc   Opens and prepares a channel for use.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    attrs
 *              Channel attributes.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *          attrs must be a valid pointer.
 *
 *  @leave  Channel must be in Idled state in case of success.
 *
 *  @see    LDRV_CHNL_close
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_open (IN ProcessorId      procId,
                IN ChannelId        chnlId,
                IN ChannelAttrs *   attrs) ;


/** ============================================================================
 *  @func   LDRV_CHNL_close
 *
 *  @desc   Closes the channel. No I/O can be performed on a closed channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_CHNL_open
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_close (IN ProcessorId procId,
                 IN ChannelId   chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_allocateBuffer
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
 *
 *  @enter  Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    LDRV_CHNL_init, LDRV_CHNL_Create, LDRV_CHNL_freeBuffer
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_allocateBuffer (IN  ProcessorId procId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN  Uint32      numBufs) ;


/** ============================================================================
 *  @func   LDRV_CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by LDRV_CHNL_allocateBuffer.
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
 *
 *  @enter  Processor and  channel ids must be valid.
 *          bufArray must be valid.
 *          numBufs must be less than maximum limit.
 *
 *  @leave  None
 *
 *  @see    LDRV_CHNL_init, LDRV_CHNL_Create, LDRV_CHNL_allocateBuffer
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_freeBuffer (IN ProcessorId procId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN Uint32      numBufs) ;


/** ============================================================================
 *  @func   LDRV_CHNL_addIORequest
 *
 *  @desc   Adds an IO request to a channel. An IO request may be a request
 *          for transferring a buffer from GPP side to DSP side or vice-versa.
 *          The direction of data transfer is decided by the mode of channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    ioInfo
 *              Information of IO to be performed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *          ioInfo must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    LDRV_CHNL_getIOCompletion
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_addIORequest (IN ProcessorId      procId,
                        IN ChannelId        chnlId,
                        IN LDRV_CHNL_IOInfo * ioInfo) ;


/** ============================================================================
 *  @func   LDRV_CHNL_getIOCompletion
 *
 *  @desc   Waits for a specified amount of time for an I/O completion event
 *          on a channel.
 *          Upon successful completion, a buffer is returned as part of ioInfo
 *          structure. A filled buffer is returned in case of an 'Input' channel
 *          and an empty buffer is returned in case of an 'Output' channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    timeout
 *              Timeout for waiting.
 *  @arg    ioInfo
 *              Information of completed request.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *          CHNL_E_NOIOC
 *              Timeout parameter was "NO_WAIT", yet no I/O completions were
 *              queued.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *          ioInfo must be a valid pointer.
 *
 *  @leave  buffer returned as part of ioInfo is non-NULL in case of
 *          successful completion, otherwise it is NULL.
 *
 *  @see    LDRV_CHNL_addIORequest
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_getIOCompletion (IN  ProcessorId      procId,
                           IN  ChannelId        chnlId,
                           IN  Uint32           timeout,
                           OUT LDRV_CHNL_IOInfo * ioInfo) ;


/** ============================================================================
 *  @func   LDRV_CHNL_addIOCompletion
 *
 *  @desc   Notification for the completion of IO.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    chirp
 *              CHIRP on which completion is done.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *          chirp must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_addIOCompletion (IN ProcessorId   procId,
                           IN ChannelId     chnlId,
                           IN LDRV_CHNL_Irp * chirp) ;


/** ============================================================================
 *  @func   LDRV_CHNL_idle
 *
 *  @desc   In case of input mode channel this function discards all pending
 *          input requests from the channel. In case of output mode channel,
 *          action of this function depends upon the flush parameter and is as
 *          follows:
 *          If flush is TRUE this function will block till all output buffers
 *          are transferred to the DSP.
 *          If flush is FALSE this function will discard all the output
 *          requests pending on this channel without blocking.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    flush
 *              Boolean parameter tells whether to block or not for output mode
 *              channels.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Memory error occurred.
 *          DSP_EINVALIDARG
 *              Invaild arguments.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *          flush argument must be valid.
 *
 *  @leave  IO request list is empty in case of successful completion.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_idle (IN ProcessorId  procId,
                IN ChannelId    chnlId,
                IN Bool         flush) ;


/** ============================================================================
 *  @func   LDRV_CHNL_control
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
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None
 *
 *  @see    PMGR_CHNL_init
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_control (IN  ProcessorId    procId,
                   IN  ChannelId      chnlId,
                   IN  Int32          cmd,
                       Pvoid          arg) ;


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelMode
 *
 *  @desc   Gets the channel mode.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    mode
 *              Channel mode.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
ChannelMode
LDRV_CHNL_getChannelMode (IN ProcessorId   procId,
                          IN ChannelId     chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelState
 *
 *  @desc   Gets the channel state.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    Channel state.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
CHNL_State
LDRV_CHNL_getChannelState (IN ProcessorId   procId,
                           IN ChannelId     chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_setChannelState
 *
 *  @desc   Sets the channel state.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *  @arg    state
 *              State of the channel to be set.
 *
 *  @ret    None.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHNL_setChannelState (IN ProcessorId   procId,
                           IN ChannelId     chnlId,
                           IN CHNL_State  state) ;


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelEndianism
 *
 *  @desc   Gets channel endianism information.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    endianism information
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Endianism
LDRV_CHNL_getChannelEndianism (IN ProcessorId   procId,
                               IN ChannelId     chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_hasMoreChirps
 *
 *  @desc   Returns TRUE if the channel has more chirps in the IO request queue.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    TRUE
 *              Channel has more chirps in IO request queue.
 *          FALSE
 *              Channel does not have more chirps available in the queue.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Bool
LDRV_CHNL_hasMoreChirps (IN ProcessorId   procId,
                         IN ChannelId     chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_nextRequestChirp
 *
 *  @desc   Returns TRUE if the channel has more chirps in the IO request queue.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    TRUE
 *              Channel has more chirps in IO request queue.
 *          FALSE
 *              Channel does not have more chirps available in the queue.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHNL_nextRequestChirp (IN ProcessorId   procId,
                            IN ChannelId     chnlId) ;


/** ============================================================================
 *  @func   LDRV_CHNL_getRequestChirp
 *
 *  @desc   Gets a chirp from request queue of a channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    CHIRP from requested queue.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHNL_getRequestChirp (IN ProcessorId   procId,
                           IN ChannelId     chnlId) ;


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_CHNL_instrument
 *
 *  @desc   Gets the instrumentation information related to the specified
 *          channel.
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
 *  @enter  procId must be valid.
 *          chirp must be valid.
 *          retVal must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_instrument (IN  ProcessorId       procId,
                      IN  ChannelId         chnlId,
                      OUT CHNL_Instrument * retVal) ;
#endif /* defined (DDSP_PROFILE) */


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_CHNL_debug
 *
 *  @desc   Prints out debug information of CHNL module. It will print all
 *          the important data structures and variable of this module.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Identifier.
 *
 *  @ret    None.
 *
 *  @enter  procId must be valid.
 *          chirp must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHNL_debug (ProcessorId procId, ChannelId chnlId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_CHNL_H) */
