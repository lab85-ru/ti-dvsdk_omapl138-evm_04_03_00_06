/** ============================================================================
 *  @file   pmgr_chnl.c
 *
 *  @path   $(DSPLINK)/gpp/src/pmgr/
 *
 *  @desc   This file implements the functions of PMGR_CHNL subcomponent.
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


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_dsplink.h>
#include <procdefs.h>
#include <chnldefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Header                   */
#include <osal.h>

/*  ----------------------------------- Profiling                     */
#include <profile.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Processor Manager             */
#include <pmgr_proc.h>
#include <pmgr_chnl.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_chnl.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   COMPONENT_ID
 *
 *  @desc   Component ID of this module. That will be used by TRC macros.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_PMGR_CHNL

/** ============================================================================
 *  @name   SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason (defined in GEN).
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_PMGR_CHNL, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @name   PMGR_CHNL_OwnerArray
 *
 *  @desc   Array of channel owners.
 *  ============================================================================
 */
STATIC PrcsObject * PMGR_CHNL_OwnerArray [MAX_DSPS][MAX_CHANNELS] ;

/** ============================================================================
 *  @name   PMGR_CHNL_IsInitialized
 *
 *  @desc   Flag that checks if channels for a processor is initialized or not.
 *  ============================================================================
 */
STATIC Bool PMGR_CHNL_IsInitialized [MAX_DSPS] ;


/** ----------------------------------------------------------------------------
 *  @func   PMGR_CHNL_IsOwner
 *
 *  @desc   Checks that the calling client is the owner of the specified
 *          channel.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    chnlId
 *              Channel Id to open.
 *
 *  @ret    TRUE
 *              If the calling client has opened the channel.
 *          FALSE
 *              Otherwise.
 *
 *  @enter  procId must be valid.
 *          chnlId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
PMGR_CHNL_IsOwner (IN  ProcessorId procId,
                   IN     ChannelId   chnlId,
                   IN     Void *      optArgs) ;


/** ============================================================================
 *  @func   PMGR_CHNL_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_init (IN ProcessorId procId)
{
    DSP_STATUS   status = DSP_SOK ;
    Uint32       i                ;

    TRC_1ENTER ("PMGR_CHNL_init", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    status = LDRV_CHNL_init (procId) ;

    if (DSP_SUCCEEDED (status)) {
        for (i = 0 ; i < MAX_CHANNELS ; i++) {
            PMGR_CHNL_OwnerArray [procId][i] = NULL ;
        }
        PMGR_CHNL_IsInitialized [procId] = TRUE ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_exit
 *
 *  @desc   This function finalizes the PMGR CHNL module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_exit (IN ProcessorId   procId)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     i      = 0       ;

    TRC_1ENTER ("PMGR_CHNL_exit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;

    if (PMGR_CHNL_IsInitialized [procId]) {
        status = LDRV_CHNL_exit (procId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        for (i = 0 ; i < MAX_CHANNELS ; i++) {
            PMGR_CHNL_OwnerArray [procId][i] = NULL ;
        }

        PMGR_CHNL_IsInitialized [procId] = FALSE ;
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_create
 *
 *  @desc   Creates resources used for transferring data between GPP and DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_create (IN     ProcessorId     procId,
                  IN     ChannelId       chnlId,
                      IN ChannelAttrs *  attrs,
                      IN Void *          optArgs)
{
    DSP_STATUS   status     = DSP_SOK ;
    Bool         isAttached = FALSE   ;
    PrcsObject * prcsInfo   = NULL    ;
    ChannelAttrs ldrvAttrs            ;

    TRC_3ENTER ("PMGR_CHNL_create", procId, chnlId, attrs) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (attrs != NULL) ;

    if (PMGR_CHNL_OwnerArray [procId][chnlId] != NULL) {
        status = DSP_EALREADYCONNECTED ;
        SET_FAILURE_REASON ;
    }
    else {
        status = PRCS_Create (&prcsInfo, optArgs) ;

        if (DSP_SUCCEEDED (status)) {
            status = PMGR_PROC_isAttached (procId, prcsInfo, &isAttached) ;
            if (DSP_SUCCEEDED (status)) {
                if (isAttached == TRUE) {
                    PMGR_CHNL_OwnerArray [procId][chnlId] = prcsInfo ;
                    ldrvAttrs.endianism = attrs->endianism ;
                    ldrvAttrs.mode      = attrs->mode      ;
                    ldrvAttrs.size      = attrs->size      ;
                    status = LDRV_CHNL_open (procId, chnlId, &ldrvAttrs) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    status = DSP_EATTACHED ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            if (DSP_FAILED (status)) {
                PMGR_CHNL_OwnerArray [procId][chnlId] = NULL ;
                PRCS_Delete (prcsInfo) ;
                prcsInfo = NULL ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("PMGR_CHNL_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_delete
 *
 *  @desc   Releases channel resources used for transferring data between GPP
 *          and DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_delete (IN ProcessorId  procId,
                  IN     ChannelId    chnlId,
                  IN     Void *       optArgs)
{
    DSP_STATUS status  = DSP_SOK ;
    DSP_STATUS tmpStatus = DSP_SOK ;

    TRC_2ENTER ("PMGR_CHNL_delete", procId, chnlId) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) {
        status = LDRV_CHNL_close (procId, chnlId) ;
        tmpStatus = PRCS_Delete (PMGR_CHNL_OwnerArray [procId][chnlId]) ;
        PMGR_CHNL_OwnerArray [procId][chnlId] = NULL ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_allocateBuffer (IN  ProcessorId procId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN     Uint32      numBufs,
                          IN     Void *      optArgs)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_5ENTER ("PMGR_CHNL_allocateBuffer", procId, chnlId,
                                            bufArray, size, numBufs) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs <= MAX_ALLOC_BUFFERS) ;

    if (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) {
        status = LDRV_CHNL_allocateBuffer (procId,
                                           chnlId,
                                           bufArray,
                                           size,
                                           numBufs) ;
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_allocateBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by PMGR_CHNL_allocateBuffer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_freeBuffer (IN ProcessorId procId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN     Uint32      numBufs,
                      IN     Void *      optArgs)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_4ENTER ("PMGR_CHNL_freeBuffer", procId, chnlId, bufArray, numBufs) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs <= MAX_ALLOC_BUFFERS) ;

    if (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) {
        status = LDRV_CHNL_freeBuffer (procId, chnlId, bufArray, numBufs) ;
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_freeBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_issue
 *
 *  @desc   Issues an input or output request on a specified channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_issue (IN ProcessorId     procId,
                 IN ChannelId       chnlId,
                 IN     ChannelIOInfo * ioReq,
                 IN     Void *          optArgs)
{
    DSP_STATUS     status  = DSP_SOK ;
    LDRV_CHNL_IOInfo ioInfo            ;

    TRC_3ENTER ("PMGR_CHNL_issue", procId, chnlId, ioReq) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioReq != NULL) ;
    DBC_Require (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) ;

    ioInfo.buffer      = ioReq->buffer ;
    ioInfo.size        = ioReq->size   ;
    ioInfo.arg         = ioReq->arg    ;

    status =  LDRV_CHNL_addIORequest (procId, chnlId, &ioInfo) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_issue", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_reclaim
 *
 *  @desc   Gets the buffer back that has been issued to this channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_reclaim (IN     ProcessorId      procId,
                   IN     ChannelId        chnlId,
                   IN     Uint32           timeout,
                   IN OUT ChannelIOInfo *  ioReq,
                   IN     Void *           optArgs)
{
    DSP_STATUS     status  = DSP_SOK ;
    LDRV_CHNL_IOInfo ioInfo            ;

    TRC_4ENTER ("PMGR_CHNL_reclaim", procId, chnlId, timeout, ioReq) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioReq != NULL) ;
    DBC_Require (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) ;

    status = LDRV_CHNL_getIOCompletion (procId, chnlId,
                                        timeout, &ioInfo) ;

    if (DSP_SUCCEEDED (status)) {
        ioReq->buffer = ioInfo.buffer ;
        ioReq->size   = ioInfo.size   ;
        ioReq->arg    = ioInfo.arg    ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_reclaim", status) ;

    return status ;
}


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
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_idle (IN ProcessorId          procId,
                IN     ChannelId   chnlId,
                IN     Void *      optArgs)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_2ENTER ("PMGR_CHNL_idle", procId, chnlId) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) {
        status = LDRV_CHNL_idle (procId, chnlId, TRUE) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_flush
 *
 *  @desc   Discards all the requested buffers that are pending for transfer
 *          both in case of input mode channel as well as output mode channel.
 *          One must still have to call the PMGR_CHNL_reclaim to get back the
 *          discarded buffers.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_flush (IN ProcessorId          procId,
                 IN     ChannelId   chnlId,
                 IN     Void *      optArgs)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_2ENTER ("PMGR_CHNL_flush", procId, chnlId) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (PMGR_CHNL_IsOwner (procId, chnlId, optArgs)) {
        status = LDRV_CHNL_idle (procId, chnlId, FALSE) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EACCESSDENIED ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_flush", status) ;

    return status ;
}


/** ============================================================================
 *  @func   PMGR_CHNL_control
 *
 *  @desc   Provides a hook to perform device dependent control operations
 *          on channels.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_control (IN  ProcessorId  procId,
                   IN  ChannelId    chnlId,
                   IN  Int32        cmd,
                       Pvoid        arg)
{
    DSP_STATUS   status   = DSP_SOK ;

    TRC_4ENTER ("PMGR_CHNL_control", procId, chnlId, cmd, arg) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    status = DSP_ENOTIMPL ;

    TRC_1LEAVE ("PMGR_CHNL_control", status) ;

    return status ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   PMGR_CHNL_instrument
 *
 *  @desc   Gets the instrumentation information related to the specified
 *          channel's
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
PMGR_CHNL_instrument (IN  ProcessorId       procId,
                      IN  ChannelId         chnlId,
                      OUT CHNL_Instrument * retVal)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("PMGR_CHNL_instrument", procId, chnlId, retVal) ;

    DBC_Require (PMGR_CHNL_IsInitialized [procId]) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (retVal != NULL) ;

    status = LDRV_CHNL_instrument (procId, chnlId, retVal) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_instrument", status) ;

    return status ;
}
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   PMGR_CHNL_debug
 *
 *  @desc   Prints the current status of CHNL sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
PMGR_CHNL_debug (ProcessorId procId, ChannelId chnlId)
{
    TRC_0ENTER ("PMGR_CHNL_debug") ;

    LDRV_CHNL_debug (procId, chnlId) ;

    TRC_0LEAVE ("PMGR_CHNL_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @func   PMGR_CHNL_IsOwner
 *
 *  @desc   Checks that the calling client is the owner of the specified
 *          channel.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
PMGR_CHNL_IsOwner (IN  ProcessorId procId,
                   IN      ChannelId   chnlId,
                   IN      Void *      optArgs)
{
    DSP_STATUS   status    = DSP_SOK ;
    PrcsObject * prcsInfo  = NULL    ;
    Bool         isOwner   = FALSE   ;

    TRC_2ENTER ("PMGR_CHNL_IsOwner", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (PMGR_CHNL_OwnerArray [procId][chnlId] != NULL) {
        status = PRCS_Create (&prcsInfo, optArgs) ;

        if (DSP_SUCCEEDED (status)) {
            status = PRCS_IsEqual (prcsInfo,
                                   PMGR_CHNL_OwnerArray [procId][chnlId],
                                   &isOwner) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            status   = PRCS_Delete (prcsInfo) ;
            prcsInfo = NULL ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("PMGR_CHNL_IsOwner", isOwner) ;

    return isOwner ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

