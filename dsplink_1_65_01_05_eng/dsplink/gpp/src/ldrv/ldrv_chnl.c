/** ============================================================================
 *  @file   ldrv_chnl.c
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Link driver's channel module implementation.
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
#include <linkcfgdefs.h>
#include <chnldefs.h>

/*  ----------------------------------- Trace & Debug                 */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                     */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Header                   */
#include <mem.h>
#include <sync.h>

/*  ----------------------------------- Generic Function              */
#include <list.h>

/*  ----------------------------------- Link Driver                   */
#include <ldrv_proc.h>
#include <ldrv.h>
#include <ldrv_chnl.h>
#include <ldrv_data.h>
#include <ldrv_chirps.h>

/*  ----------------------------------- LOG  Headers                 */
#if defined (LOG_COMPONENT)
#include <linklog.h>
#endif /* if defined (LOG_COMPONENT) */


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component ID of this module.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_LDRV_CHNL

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_LDRV_CHNL, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @macro  IS_CHNL_READY
 *
 *  @desc   Is the channel ready.
 *  ============================================================================
 */
#define IS_CHNL_READY(chnlObj)         \
       (((chnlObj)->chnlState & ChannelState_Ready) == ChannelState_Ready)

/** ============================================================================
 *  @macro  IS_CHNL_EOS
 *
 *  @desc   Has the channel reached EOS.
 *  ============================================================================
 */
#define IS_CHNL_EOS(chnlObj)         \
       (((chnlObj)->chnlState & ChannelState_EOS) == ChannelState_EOS)

/** ============================================================================
 *  @macro  IS_CHNL_IDLE
 *
 *  @desc   Is channel in idle state.
 *  ============================================================================
 */
#define IS_CHNL_IDLE(chnlObj)         \
       (((chnlObj)->chnlState & ChannelState_Idled) == ChannelState_Idled)

/** ============================================================================
 *  @macro  IS_INPUT_CHNL
 *
 *  @desc   Is this channel a input channel.
 *  ============================================================================
 */
#define IS_INPUT_CHNL(chnlObj) (((chnlObj)->attrs).mode == ChannelMode_Input)

/** ============================================================================
 *  @macro  IS_OUTPUT_CHNL
 *
 *  @desc   Is this channel a output channel.
 *  ============================================================================
 */
#define IS_OUTPUT_CHNL(chnlObj) (((chnlObj)->attrs).mode == ChannelMode_Output)


/** ============================================================================
 *  @name   LDRVChnlObject
 *
 *  @desc   LDRV Channel object.
 *
 *  @field  signature
 *              Signature of object.
 *  @field  bufSize
 *              Size of buffers on this channel.
 *  @field  maxBufSize
 *              Maximum buffer size supported by underlying DATA streaming
 *              module.
 *  @field  chnlState
 *              State of channel.
 *  @field  freeList
 *              List for free chirps.
 *  @field  requestList
 *              List for requested chirps.
 *  @field  completedList
 *              List for completed chirps.
 *  @field  attrs
 *              Attributes of this channel.
 *  @field  syncEvent
 *              Event to be signaled when some IO is completed for this
 *              channel.
 *  @field  chnlIdleSync
 *              Sync event used by channel idle function call.
 *  @field  doDspCtrl
 *              Indicates whether DSP/BIOS LINK should do the dsp control for
 *              the DSP. This depends on the DSPLink boot mode chosen.
 *  @field  chnlStats
 *              Instrumentation information for the channel.
 *              Defined only if profiling is enabled.
 *  ============================================================================
 */
typedef struct LDRVChnlObject_tag {
    Uint32          signature     ;
    Uint32          bufSize       ;
    Uint32          maxBufSize    ;
    CHNL_State      chnlState     ;
    List *          freeList      ;
    List *          requestList   ;
    List *          completedList ;
    Pvoid           fLock         ;
    Pvoid           rLock         ;
    Pvoid           cLock         ;
    ChannelAttrs    attrs         ;
    SyncEvObject *  syncEvent     ;
    SyncEvObject *  chnlIdleSync  ;
    DSP_BootMode    doDspCtrl   ;
#if defined (DDSP_PROFILE)
    CHNL_Stats      chnlStats     ;
#endif /* if defined (DDSP_PROFILE) */
} LDRVChnlObject ;


/** ----------------------------------------------------------------------------
 *  @name   LDRV_CHNL_Object
 *
 *  @desc   Contains information related to channels.
 *  ----------------------------------------------------------------------------
 */
STATIC LDRVChnlObject * LDRV_CHNL_Object [MAX_DSPS][MAX_CHANNELS] ;


/** ============================================================================
 *  @func   LDRV_CHNL_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_init (IN ProcessorId  procId)
{
    DSP_STATUS   status = DSP_SOK ;
    Uint32       i                ;

    TRC_1ENTER ("LDRV_CHNL_init", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    for (i = 0 ; i < MAX_CHANNELS ; i++) {
        LDRV_CHNL_Object [procId][i] = NULL ;
    }

    TRC_1LEAVE ("LDRV_CHNL_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_exit
 *
 *  @desc   This function finalizes the LDRV CHNL module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_exit (IN ProcessorId procId)
{
    DSP_STATUS   status    = DSP_SOK ;
    DSP_STATUS   tmpStatus = DSP_SOK ;
    Uint32       i                   ;

    TRC_1ENTER ("LDRV_CHNL_exit", procId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;

    /* Close the channels if not already closed */
    for (i = 0 ; i < MAX_CHANNELS ; i++) {
        if (LDRV_CHNL_Object [procId][i] != NULL) {
            tmpStatus = LDRV_CHNL_close (procId, i) ;
            if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
                status = tmpStatus ;
            }
        }
    }

    TRC_1LEAVE ("LDRV_CHNL_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_open
 *
 *  @desc   Opens and prepares a channel for use. The queues to manage data
 *          transfer requests are created in this function.
 *
 *  @modif  LDRV_CHNL_Object [procId][chnlId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_open (IN ProcessorId      procId,
                IN ChannelId        chnlId,
                IN ChannelAttrs *   attrs)
{
    DSP_STATUS     status       = DSP_SOK ;
    SyncAttrs      syncAttrs    = {0} ;
    Bool           closeEvent1  = FALSE ;
    Bool           closeEvent2  = FALSE ;
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Gpp *  gppObj ;
    LINKCFG_Dsp *  dspObj ;
    LINKCFG_LinkDrv *  linkDrv ;
    LINKCFG_DataDrv *  dataTable ;
    Uint32             dataDrvId ;

    TRC_3ENTER ("LDRV_CHNL_open", procId, chnlId, attrs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (attrs != NULL) ;

    DBC_Assert (LDRV_CHNL_Object [procId][chnlId] == NULL) ;

    gppObj = LDRV_LinkCfgPtr->gppObject ;
    dspCfg    = LDRV_LinkCfgPtr->dspConfigs [procId] ;
    dspObj    = dspCfg->dspObject ;
    linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
    dataTable = &(dspCfg->dataTables [linkDrv->dataTableId][0]) ;

    status = LDRV_DATA_getDataDrvId (procId, chnlId, &dataDrvId) ;
    if (DSP_SUCCEEDED (status)) {
    status = MEM_Calloc ((Void **) &LDRV_CHNL_Object [procId][chnlId],
                         sizeof(LDRVChnlObject),
                         MEM_DEFAULT) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        LDRV_CHNL_Object [procId][chnlId]->signature   = SIGN_LDRV_CHNL  ;
        LDRV_CHNL_Object [procId][chnlId]->chnlState   = ChannelState_Idled  ;
        LDRV_CHNL_Object [procId][chnlId]->attrs       = *attrs ;
        LDRV_CHNL_Object [procId][chnlId]->doDspCtrl = dspObj->doDspCtrl ;
        LDRV_CHNL_Object [procId][chnlId]->freeList =
                                 LDRV_CHIRPS_CreateList (gppObj->maxChnlQueue) ;
        LDRV_CHNL_Object [procId][chnlId]->maxBufSize  =
                                              dataTable [dataDrvId].maxBufSize ;

        /* Create the lock for freelist, request, completed */
        LDRV_CHNL_Object [procId][chnlId]->fLock = SYNC_SpinLockCreate () ;
        LDRV_CHNL_Object [procId][chnlId]->rLock = SYNC_SpinLockCreate () ;
        LDRV_CHNL_Object [procId][chnlId]->cLock = SYNC_SpinLockCreate () ;

        if (LDRV_CHNL_Object [procId][chnlId]->freeList == NULL) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        else {
            LDRV_CHNL_Object [procId][chnlId]->requestList =
                                              LDRV_CHIRPS_CreateList (0) ;

            if (LDRV_CHNL_Object [procId][chnlId]->requestList == NULL) {
                status = DSP_EMEMORY ;
                SET_FAILURE_REASON ;
            }
            else {
                LDRV_CHNL_Object [procId][chnlId]->completedList =
                                              LDRV_CHIRPS_CreateList (0) ;
                if (LDRV_CHNL_Object [procId][chnlId]->completedList
                                                                == NULL) {
                    status = DSP_EMEMORY ;
                    SET_FAILURE_REASON ;
                }
            }
        }

        if (DSP_FAILED (status)) {
            SYNC_SpinLockDelete (LDRV_CHNL_Object [procId][chnlId]->fLock) ;
            SYNC_SpinLockDelete (LDRV_CHNL_Object [procId][chnlId]->rLock) ;
            SYNC_SpinLockDelete (LDRV_CHNL_Object [procId][chnlId]->cLock) ;
            LDRV_CHIRPS_FreeList (
                         LDRV_CHNL_Object [procId][chnlId]->freeList) ;
            LDRV_CHIRPS_FreeList (
                         LDRV_CHNL_Object [procId][chnlId]->requestList) ;
            LDRV_CHIRPS_FreeList (
                         LDRV_CHNL_Object [procId][chnlId]->completedList) ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = SYNC_OpenEvent (
                            &(LDRV_CHNL_Object [procId][chnlId]->syncEvent),
                            &syncAttrs ) ;
            closeEvent1 = TRUE ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = SYNC_OpenEvent (
                            &(LDRV_CHNL_Object [procId][chnlId]->chnlIdleSync),
                            &syncAttrs ) ;
            closeEvent2 = TRUE ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        /* Set the state of a new channel to Idle */
        if (DSP_SUCCEEDED (status)) {
            status = SYNC_SetEvent (
                            LDRV_CHNL_Object [procId][chnlId]->chnlIdleSync) ;
        }
        else {
            SET_FAILURE_REASON ;
        }

        SYNC_ProtectionStart () ;
        if (DSP_SUCCEEDED (status)) {
            status = LDRV_DATA_openChannel (procId, chnlId) ;
        }
        SYNC_ProtectionEnd () ;

        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
            LDRV_CHNL_Object [procId][chnlId]->signature = SIGN_NULL ;
            if (closeEvent1) {
                SYNC_CloseEvent (
                            LDRV_CHNL_Object [procId][chnlId]->syncEvent) ;
            }
            if (closeEvent2) {
                SYNC_CloseEvent (
                            LDRV_CHNL_Object [procId][chnlId]->chnlIdleSync) ;
            }
            FREE_PTR (LDRV_CHNL_Object [procId][chnlId]) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   DSP_FAILED (status)
                || (   DSP_SUCCEEDED (status)
                    && (   LDRV_CHNL_Object [procId][chnlId]->chnlState
                        == ChannelState_Idled)
                   )) ;

    TRC_1LEAVE ("LDRV_CHNL_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_close
 *
 *  @desc   Closes the channel.
 *
 *  @modif  LDRV_CHNL_Object [procId][chnlId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_close (IN ProcessorId procId,
                 IN ChannelId   chnlId)
{
    DSP_STATUS       status    = DSP_SOK ;
    DSP_STATUS       tmpStatus = DSP_SOK ;
    LDRVChnlObject * chnlObj   = NULL ;

    TRC_2ENTER ("LDRV_CHNL_close", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;
    if (chnlObj == NULL) {
        status = DSP_EFAIL ;             /* channel not open */
        SET_FAILURE_REASON ;
    }
    else {

        /* Cancel all pending IO */
        tmpStatus = LDRV_CHNL_idle (procId, chnlId, FALSE) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        chnlObj->chnlState = ChannelState_Closed ;

        SYNC_ProtectionStart () ;
        tmpStatus = LDRV_DATA_closeChannel (procId, chnlId) ;
        SYNC_ProtectionEnd () ;

        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
        LDRV_CHNL_Object [procId][chnlId] = NULL ;

        tmpStatus = SYNC_CloseEvent (chnlObj->syncEvent) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = SYNC_CloseEvent (chnlObj->chnlIdleSync) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = LDRV_CHIRPS_FreeList (chnlObj->freeList) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = LDRV_CHIRPS_FreeList (chnlObj->requestList) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = LDRV_CHIRPS_FreeList (chnlObj->completedList) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        chnlObj->signature = SIGN_NULL ;

        /* Del the list locks */
        if (chnlObj->fLock != NULL) {
            SYNC_SpinLockDelete (chnlObj->fLock) ;
        }
        if (chnlObj->rLock != NULL) {
            SYNC_SpinLockDelete (chnlObj->rLock) ;
        }
        if (chnlObj->cLock != NULL) {
            SYNC_SpinLockDelete (chnlObj->cLock) ;
        }

        tmpStatus = FREE_PTR (chnlObj) ;
        if (DSP_FAILED (tmpStatus) && DSP_SUCCEEDED (status)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LDRV_CHNL_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_allocateBuffer
 *
 *  @desc   Allocates an array of buffers of specified size and returns them
 *          to the client.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_allocateBuffer (IN  ProcessorId procId,
                          IN  ChannelId   chnlId,
                          OUT Char8 **    bufArray,
                          IN  Uint32      size,
                          IN  Uint32      numBufs)
{
    DSP_STATUS status  = DSP_SOK ;

    TRC_5ENTER ("LDRV_CHNL_allocateBuffer", procId, chnlId,
                                            bufArray, size, numBufs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs <= MAX_ALLOC_BUFFERS) ;

    if (size > LDRV_CHNL_Object [procId][chnlId]->maxBufSize) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
    LDRV_CHNL_Object [procId][chnlId]->bufSize = size ;

    status = LDRV_DATA_allocateBuffer (procId,
                                       chnlId,
                                       bufArray,
                                       size,
                                       numBufs) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    }

    TRC_1LEAVE ("LDRV_CHNL_allocateBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_freeBuffer
 *
 *  @desc   Frees buffer(s) allocated by LDRV_CHNL_allocateBuffer.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_freeBuffer (IN ProcessorId procId,
                      IN ChannelId   chnlId,
                      IN Char8 **    bufArray,
                      IN Uint32      numBufs)
{
    DSP_STATUS status  = DSP_SOK ;
    Uint32     bufSize ;

    TRC_4ENTER ("LDRV_CHNL_freeBuffer", procId, chnlId, bufArray, numBufs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (bufArray != NULL) ;
    DBC_Require (numBufs <= MAX_ALLOC_BUFFERS) ;

    if (LDRV_CHNL_Object [procId][chnlId] == NULL) {
        status = DSP_EFAIL ;             /* channel not open */
        SET_FAILURE_REASON ;
    }
    else {
        bufSize = LDRV_CHNL_Object [procId][chnlId]->bufSize ;
        if (   (bufSize != 0)
            && (bufSize <= LDRV_CHNL_Object [procId][chnlId]->maxBufSize)) {
        status = LDRV_DATA_freeBuffer (procId,
                                       chnlId,
                                       bufArray,
                                           bufSize,
                                       numBufs) ;
    }
        else {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHNL_freeBuffer", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_addIORequest
 *
 *  @desc   Adds an IO request to a channel. An IO request may be a request
 *          for transferring a buffer from GPP side to DSP side or vice-versa.
 *          The direction of data transfer is decided by the mode of channel.
 *
 *  @modif  LDRV_CHNL_Object [procId][chnlId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_addIORequest (IN ProcessorId        procId,
                        IN ChannelId          chnlId,
                        IN LDRV_CHNL_IOInfo * ioInfo)
{
    DSP_STATUS        status      = DSP_SOK  ;
    LDRV_CHNL_Irp *   chirp       = NULL     ;
    LDRVChnlObject *  chnlObj     = NULL     ;
#if defined (DDSP_PROFILE_DETAILED)
    CHNL_Instrument * chnlInst ;
    Uint32            i        ;
#endif

    TRC_3ENTER ("LDRV_CHNL_addIORequest", procId, chnlId, ioInfo) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioInfo != NULL) ;

    /*  ------------------------------------------------------------------------
     *  Start the protection to ensure that DPC does not preempt the execution
     *  below.
     *  ------------------------------------------------------------------------
     */
    SYNC_ProtectionStart () ;

    DBC_Assert (LDRV_CHNL_Object [procId][chnlId] != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;
    if (chnlObj == NULL) {
        status = DSP_EFAIL ;      /* channel not opened */
        SET_FAILURE_REASON ;
    }
    else if (IS_CHNL_EOS (chnlObj)) {
        status = CHNL_E_EOS ;
        SET_FAILURE_REASON ;
    }
    else {
        SYNC_SpinLockStartEx (chnlObj->fLock) ;
        if (LIST_IsEmpty (chnlObj->freeList)) {
            status = CHNL_E_NOIORPS ;
            SET_FAILURE_REASON ;
        }
        else  {
            SYNC_SpinLockEndEx (chnlObj->fLock, 0u) ;
            if (    (chnlObj->doDspCtrl != DSP_BootMode_Boot_Pwr)
                  && (chnlObj->doDspCtrl != DSP_BootMode_NoLoad_Pwr)) {
                status = LDRV_PROC_isStarted (procId) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }
            SYNC_SpinLockStartEx (chnlObj->fLock) ;
        }
        SYNC_SpinLockEndEx (chnlObj->fLock, 0u) ;
    }

    if (DSP_SUCCEEDED (status)) {
        if (ioInfo->size > LDRV_CHNL_Object [procId][chnlId]->maxBufSize) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        if (IS_CHNL_IDLE (chnlObj)) {
            /* Reset the underlying communication hardware here
             * and make channel ready again
             */
            chnlObj->chnlState = ChannelState_Ready ;
        }

        SYNC_SpinLockStartEx (chnlObj->fLock) ;
        status = LIST_GetHead (chnlObj->freeList, (ListElement **) &chirp) ;
        SYNC_SpinLockEndEx (chnlObj->fLock, 0u) ;
        if (DSP_SUCCEEDED (status)) {
            if (ioInfo->size == 0) {
                chnlObj->chnlState = ChannelState_EOS ;
            }

            chirp->buffer    = (Uint32) ioInfo->buffer   ;
            chirp->size      = ioInfo->size     ;
            chirp->arg       = ioInfo->arg      ;
            chirp->iocStatus = LDRV_CHNL_IOCSTATE_PENDING ;
            chirp->chnlId    = chnlId ;

            SYNC_SpinLockStartEx (chnlObj->rLock) ;
            status = LIST_PutTail (chnlObj->requestList,
                                   (ListElement *) chirp) ;
            SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;

#if defined (LOG_GD_CHNL_I_QUE)
            /* Log the event */
            DSPLINKLOG_LogEvent (
                            GD_CHNL_I_QUE,
                            0,
                            (((procId << 16) & 0xFFFF0000) | (chnlId & 0xFFFF)),
                            (Uint32) ioInfo->buffer,
                            ioInfo->size,
                            0,
                            0) ;
#endif /* if defined (LOG_GD_CHNL_I_QUE) */

#if defined (DDSP_PROFILE)
            (chnlObj->chnlStats.chnlData.numBufsQueued)++ ;
#endif /* defined (DDSP_PROFILE) */

#if defined (DDSP_PROFILE_DETAILED)
            /* Store the first few bytes of buffer if output channel */
            if (chnlObj->attrs.mode == ChannelMode_Output) {
                chnlInst = &(chnlObj->chnlStats.chnlData) ;
                for (i = 0 ; i < DATA_LENGTH ; i++) {
                    chnlInst->archive [chnlInst->archIndex][i]
                                            = ((Char8 *) (chirp->buffer)) [i] ;
                }
                chnlInst->archIndex++ ;
                if (chnlInst->archIndex == HIST_LENGTH) {
                    chnlInst->archIndex = 0 ;
                }
            }
#endif

            if (DSP_SUCCEEDED (status)) {
                status = SYNC_ResetEvent (chnlObj->chnlIdleSync) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    status = LDRV_DATA_request (procId, chnlId) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
            }
            else {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_ERESOURCE ;
            SET_FAILURE_REASON ;
            chirp = NULL ;
        }
    }

    /*  ------------------------------------------------------------------------
     *  End the protection from DPC.
     *  ------------------------------------------------------------------------
     */
    SYNC_ProtectionEnd () ;

    TRC_1LEAVE ("LDRV_CHNL_addIORequest", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_getIOCompletion
 *
 *  @desc   Waits for a specified amount of time for an I/O completion event
 *          on a channel.
 *          Upon successful completion, a buffer is returned as part of ioInfo
 *          structure. A filled buffer is returned in case of an 'Input' channel
 *          and an empty buffer is returned in case of an 'Output' channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_getIOCompletion (IN  ProcessorId        procId,
                           IN  ChannelId          chnlId,
                           IN  Uint32             timeout,
                           OUT LDRV_CHNL_IOInfo * ioInfo)
{
    DSP_STATUS        status     = DSP_SOK ;
    DSP_STATUS        tmpStatus  = DSP_SOK ;
    LDRV_CHNL_Irp *   chirp      = NULL    ;
    LDRVChnlObject *  chnlObj    = NULL    ;

#if defined (DDSP_PROFILE_DETAILED)
    CHNL_Instrument * chnlInst ;
    Uint32            i        ;
#endif

    TRC_4ENTER ("LDRV_CHNL_getIOCompletion", procId, chnlId, timeout, ioInfo) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (ioInfo != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;
    if (chnlObj == NULL) {
        status = DSP_EFAIL ;      /* channel not opened */
        SET_FAILURE_REASON ;
    }
    else if (    (chnlObj->doDspCtrl != DSP_BootMode_Boot_Pwr)
              && (chnlObj->doDspCtrl != DSP_BootMode_NoLoad_Pwr)) {
        SYNC_SpinLockStartEx (chnlObj->cLock) ;
        if (   DSP_SUCCEEDED (status)
            && !LIST_IsEmpty (chnlObj->completedList)) {
            SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
            status = LDRV_PROC_isStarted (procId) ;
            SYNC_SpinLockStartEx (chnlObj->cLock) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
    }

    if (DSP_SUCCEEDED (status)) {
#if defined (LOG_GD_CHNL_R_PEND)
        /* Log the event */
        DSPLINKLOG_LogEvent (
                      GD_CHNL_R_PEND,
                      0,
                      (((procId << 16) & 0xFFFF0000) | (chnlId & 0xFFFF)),
                      (Uint32) ioInfo->buffer,
                      ioInfo->size,
                      0,
                      0) ;
#endif /* if defined (LOG_GD_CHNL_R_PEND) */

        status = SYNC_WaitOnEvent (chnlObj->syncEvent, timeout) ;

        /*  --------------------------------------------------------------------
         *  Start the protection to ensure that DPC does not preempt the
         *  execution below.
         *  Even if status of SYNC_WaitOnEvent is a failure (viz. on timeout)
         *  the list operations still need protection.
         *  --------------------------------------------------------------------
         */
        SYNC_ProtectionStart () ;

        if (DSP_SUCCEEDED (status)) {
            SYNC_SpinLockStartEx (chnlObj->cLock) ;
            if (!LIST_IsEmpty (chnlObj->completedList)) {
                status = LIST_GetHead (chnlObj->completedList,
                                       (ListElement **) &chirp) ;
                SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
                if (DSP_SUCCEEDED (status) && (chirp != NULL)) {
                    ioInfo->buffer      = (Uint8 *) chirp->buffer ;
                    ioInfo->size        = chirp->size   ;
                    ioInfo->arg         = chirp->arg    ;
    #if defined (DDSP_PROFILE_DETAILED)
                    /* Store the first few bytes of buffer if input channel */
                    if (chnlObj->attrs.mode == ChannelMode_Input) {
                        chnlInst = &(chnlObj->chnlStats.chnlData) ;
                        for (i = 0 ; i < DATA_LENGTH ; i++) {
                            chnlInst->archive [chnlInst->archIndex][i]
                                             = ((Char8 *) (chirp->buffer)) [i] ;
                        }
                        chnlInst->archIndex++ ;
                        if (chnlInst->archIndex == HIST_LENGTH) {
                            chnlInst->archIndex = 0 ;
                        }
                    }
    #endif
                    if (chirp->iocStatus != LDRV_CHNL_IOCSTATE_COMPLETE) {
                        ioInfo->completionStatus = IOState_NotCompleted ;
                    }
                    else {
                        ioInfo->completionStatus = IOState_Completed ;
                    }

                    SYNC_SpinLockStartEx (chnlObj->fLock) ;
                    status = LIST_PutTail (chnlObj->freeList,
                                           (ListElement*) chirp) ;
                    SYNC_SpinLockEndEx (chnlObj->fLock, 0u) ;
                    if (DSP_FAILED (status)) {
                        status = DSP_EFAIL ;
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
                SYNC_SpinLockStartEx (chnlObj->fLock) ;
            }
            else {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
            SYNC_SpinLockEndEx (chnlObj->fLock, 0u) ;
        }
        else {
            if ((timeout == SYNC_NOWAIT) && (status == SYNC_E_FAIL)) {
                status = CHNL_E_NOIOC ;
                SET_FAILURE_REASON ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }

        SYNC_SpinLockStartEx (chnlObj->cLock) ;
        if (LIST_IsEmpty (chnlObj->completedList)) {
            SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
            tmpStatus = SYNC_ResetEvent (chnlObj->syncEvent) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status =  tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            SYNC_SpinLockStartEx (chnlObj->cLock) ;
        }
        SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;

        /*  --------------------------------------------------------------------
         *  End the protection from DPC.
         *  --------------------------------------------------------------------
         */
        SYNC_ProtectionEnd () ;
    }

    if (DSP_FAILED (status)) {
        ioInfo->buffer = NULL ;
        ioInfo->arg    = 0    ;
        ioInfo->size   = 0    ;
    }

    DBC_Ensure (   DSP_FAILED (status)
                || (   DSP_SUCCEEDED (status)
                    && (ioInfo->buffer != NULL))) ;

    DBC_Ensure (   DSP_SUCCEEDED (status)
                || (   DSP_FAILED (status)
                    && (ioInfo->buffer == NULL))) ;

    TRC_1LEAVE ("LDRV_CHNL_getIOCompletion", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_addIOCompletion
 *
 *  @desc   Notification for the completion of IO.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_addIOCompletion (IN ProcessorId     procId,
                           IN ChannelId       chnlId,
                           IN LDRV_CHNL_Irp * chirp)
{
    DSP_STATUS        status    = DSP_SOK ;
    DSP_STATUS        tmpStatus = DSP_SOK ;
    LDRVChnlObject *  chnlObj   = NULL    ;

    TRC_3ENTER ("LDRV_CHNL_addIOCompletion", procId, chnlId, chirp) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (LDRV_CHNL_Object [procId][chnlId] != NULL) ;
    DBC_Require (chirp != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;

    SYNC_SpinLockStartEx (chnlObj->cLock) ;
    status = LIST_PutTail (chnlObj->completedList, (ListElement*) chirp) ;
    SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
    if (DSP_SUCCEEDED (status)) {
#if defined (DDSP_PROFILE)
        (chnlObj->chnlStats.chnlData.numBufsQueued)-- ;
#endif /* defined (DDSP_PROFILE) */

#if defined (LOG_GD_CHNL_R_POST)
        /* Log the event */
        DSPLINKLOG_LogEvent (GD_CHNL_R_POST,
                      0,
                      (((procId << 16) & 0xFFFF0000) | (chnlId & 0xFFFF)),
                      chirp->buffer,
                      chirp->size,
                      0,
                      0) ;
#endif /* if defined (LOG_GD_CHNL_R_POST) */

        status = SYNC_SetEvent (chnlObj->syncEvent) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        SYNC_SpinLockStartEx (chnlObj->rLock) ;
        if (LIST_IsEmpty (chnlObj->requestList)) {
            SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
            tmpStatus = SYNC_SetEvent (chnlObj->chnlIdleSync) ;
            if (DSP_FAILED (tmpStatus) && (DSP_SUCCEEDED (status))) {
                status =  tmpStatus ;
                SET_FAILURE_REASON ;
            }
            SYNC_SpinLockStartEx (chnlObj->rLock) ;
        }
        SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHNL_addIOCompletion", status) ;

    return status ;
}


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
 *  @modif  LDRV_CHNL_Object [procId][chnlId]
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_idle (IN ProcessorId procId,
                IN ChannelId   chnlId,
                IN Bool        flush)
{
    DSP_STATUS        status        = DSP_SOK ;
    LDRV_CHNL_Irp *   chirp         = NULL    ;
    LDRV_CHNL_Irp *   firstChirp    = NULL    ;
    LDRVChnlObject *  chnlObj       = NULL    ;

    TRC_3ENTER ("LDRV_CHNL_idle", procId, chnlId, flush) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require ((flush == TRUE) || (flush == FALSE))

    DBC_Assert (LDRV_CHNL_Object [procId][chnlId] != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;

    if (chnlObj == NULL) {
        status = DSP_EFAIL ;      /* channel not opened */
        SET_FAILURE_REASON ;
    }
    else {
        if (   IS_INPUT_CHNL (chnlObj)
            || (   IS_OUTPUT_CHNL (chnlObj)
                && (flush == FALSE))) {
            /*  ----------------------------------------------------------------
             *  Start the protection from DPC.
             *  ----------------------------------------------------------------
             */
            SYNC_ProtectionStart () ;

            /*  ----------------------------------------------------------------
             *  Discard all pending IO.
             *  ----------------------------------------------------------------
             */
            SYNC_SpinLockStartEx (chnlObj->rLock) ;
            if (!LIST_IsEmpty (chnlObj->requestList)) {
                /* store the first element  address from  request list*/
                status = LIST_First (chnlObj->requestList,
                                     (ListElement **) &firstChirp);
                SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
                if ( DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                else {
                    if (firstChirp != NULL) {
                        /* Get the next chirp element address */
                        SYNC_SpinLockStartEx (chnlObj->rLock) ;
                        LIST_Next (chnlObj->requestList,
                                   (ListElement *)firstChirp,
                                   (ListElement **)&chirp) ;
                        SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
                    }
                }

                do {
                    if (    firstChirp->iocStatus
                        !=  LDRV_CHNL_IOCSTATE_INPROGRESS)  {

                        /* Head chirp is still in pending state
                         * Remove the chirp from request list
                         */
                        SYNC_SpinLockStartEx (chnlObj->rLock) ;
                        status = LIST_RemoveElement (chnlObj->requestList,
                                                    (ListElement *)firstChirp) ;
                        SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
                        if (DSP_SUCCEEDED (status)) {
                            /*
                             * put it in completed list and remove it from
                             * request list
                             */
                            firstChirp->size = 0 ;
                            firstChirp->iocStatus |=
                                                  LDRV_CHNL_IOCSTATE_CANCELED ;
                            /*
                             * Insert it in to completed list*/
                            SYNC_SpinLockStartEx (chnlObj->cLock) ;
                            status = LIST_PutTail (chnlObj->completedList,
                                                  (ListElement *) firstChirp) ;
                            SYNC_SpinLockEndEx (chnlObj->cLock, 0u) ;
                            if (DSP_FAILED (status)) {
                                SET_FAILURE_REASON ;
                            }
                            else {
                                status = SYNC_SetEvent (chnlObj->syncEvent) ;
                                if (DSP_FAILED (status)) {
                                    SET_FAILURE_REASON ;
                                }
                            }
                        }

                    }
                    /* Move the firstChirp pointer forward in the list*/
                    firstChirp = chirp ;

                    if (firstChirp != NULL) {
                        /* Get the next chirp element address */
                        SYNC_SpinLockStartEx (chnlObj->rLock) ;
                        LIST_Next (chnlObj->requestList,
                                   (ListElement *) firstChirp,
                                   (ListElement **)&chirp) ;
                        SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
                    }

                } while (firstChirp != NULL);

                SYNC_SpinLockStartEx (chnlObj->rLock) ;
            }
            SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
            /* These to calls can be merged, but left becuase to solve confusion
             */
            SYNC_SpinLockStartEx (chnlObj->rLock) ;
            if (LIST_IsEmpty (chnlObj->requestList) && DSP_SUCCEEDED (status)) {
                SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
                status = SYNC_SetEvent (chnlObj->chnlIdleSync) ;
                if (DSP_SUCCEEDED (status)) {
                    status = LDRV_DATA_cancel (procId, chnlId) ;
                    if (DSP_FAILED (status)) {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
                SYNC_SpinLockStartEx (chnlObj->rLock) ;
            }
            SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;

            if (DSP_SUCCEEDED (status)) {
                chnlObj->chnlState = ChannelState_Idled ;
            }

            /*  ----------------------------------------------------------------
             *  End the protection from DPC.
             *  ----------------------------------------------------------------
             */
            SYNC_ProtectionEnd () ;
        }
        else {
            status = SYNC_WaitOnEvent (chnlObj->chnlIdleSync,
                                       SYNC_WAITFOREVER) ;
            if (DSP_SUCCEEDED (status)) {
                chnlObj->chnlState = ChannelState_Idled ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    DBC_Ensure (   DSP_FAILED(status)
                || (   DSP_SUCCEEDED (status)
                    && (   LDRV_CHNL_Object [procId][chnlId]->chnlState
                        == ChannelState_Idled))) ;

    TRC_1LEAVE ("LDRV_CHNL_idle", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_control
 *
 *  @desc   Provides a hook to perform device dependent control operations
 *          on channels.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_control (IN  ProcessorId  procId,
                   IN  ChannelId    chnlId,
                   IN  Int32        cmd,
                       Pvoid        arg)
{
    DSP_STATUS   status   = DSP_SOK ;

    TRC_4ENTER ("LDRV_CHNL_control", procId, chnlId, cmd, arg) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    status = DSP_ENOTIMPL ;

    TRC_1LEAVE ("LDRV_CHNL_control", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelMode
 *
 *  @desc   Gets the channel mode.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
ChannelMode
LDRV_CHNL_getChannelMode (IN ProcessorId   procId,
                          IN ChannelId     chnlId)
{
    TRC_2ENTER ("LDRV_CHNL_getChannelMode", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    TRC_1LEAVE ("LDRV_CHNL_getChannelMode",
                LDRV_CHNL_Object [procId][chnlId]->attrs.mode) ;

    return LDRV_CHNL_Object [procId][chnlId]->attrs.mode ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelState
 *
 *  @desc   Sets last output channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
CHNL_State
LDRV_CHNL_getChannelState (IN ProcessorId   procId,
                           IN ChannelId     chnlId)
{
    CHNL_State chnlState = ChannelState_Closed ;

    TRC_2ENTER ("LDRV_CHNL_getChannelState", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    if (LDRV_CHNL_Object [procId][chnlId] != NULL) {
        chnlState = LDRV_CHNL_Object [procId][chnlId]->chnlState ;
    }

    TRC_1LEAVE ("LDRV_CHNL_getChannelState", chnlState) ;

    return chnlState ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_setChannelState
 *
 *  @desc   Sets the channel state.
 *
 *  @modif  LDRV_CHNL_Object [procId][chnlId]
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHNL_setChannelState (IN ProcessorId  procId,
                           IN ChannelId    chnlId,
                           IN CHNL_State   state)
{
    TRC_3ENTER ("LDRV_CHNL_setChannelState", procId, chnlId, state) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    LDRV_CHNL_Object [procId][chnlId]->chnlState = state ;

    TRC_0LEAVE ("LDRV_CHNL_setChannelState") ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_getChannelEndianism
 *
 *  @desc   Gets channel endianism information.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Endianism
LDRV_CHNL_getChannelEndianism (IN ProcessorId   procId,
                               IN ChannelId     chnlId)
{
    LDRVChnlObject *   chnlObj       = NULL    ;
    Endianism          endianismInfo           ;

    TRC_2ENTER ("LDRV_CHNL_getChannelEndianism", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (LDRV_CHNL_Object [procId][chnlId] != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;
    endianismInfo = chnlObj->attrs.endianism ;

    TRC_1LEAVE ("LDRV_CHNL_getChannelEndianism", endianismInfo) ;

    return  endianismInfo ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_hasMoreChirps
 *
 *  @desc   Returns TRUE if the channel has more chirps in the IO request queue.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Bool
LDRV_CHNL_hasMoreChirps  (IN ProcessorId   procId,
                          IN ChannelId     chnlId)

{
    Bool hasMoreChirps ;

    TRC_2ENTER ("LDRV_CHNL_hasMoreChirps", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;

    SYNC_SpinLockStartEx (LDRV_CHNL_Object [procId][chnlId]->rLock) ;
    if (LIST_IsEmpty (LDRV_CHNL_Object [procId][chnlId]->requestList)) {
        hasMoreChirps = FALSE ;
    }
    else {
        hasMoreChirps = TRUE ;
    }
    SYNC_SpinLockEndEx (LDRV_CHNL_Object [procId][chnlId]->rLock, 0u) ;

    TRC_1LEAVE ("LDRV_CHNL_hasMoreChirps", hasMoreChirps) ;

    return hasMoreChirps ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_nextRequestChirp
 *
 *  @desc   Gets the chirp from request queue of the specified channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHNL_nextRequestChirp (IN ProcessorId   procId,
                            IN ChannelId     chnlId)
{
    DSP_STATUS        status  = DSP_SOK ;
    LDRV_CHNL_Irp *   chirp   = NULL    ;
    LDRVChnlObject *  chnlObj = NULL    ;

    TRC_2ENTER ("LDRV_CHNL_nextRequestChirp", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (LDRV_CHNL_Object [procId][chnlId] != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;

    SYNC_SpinLockStartEx (chnlObj->rLock) ;
    status = LIST_First (chnlObj->requestList, (ListElement **) &chirp) ;
    SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
    if (DSP_FAILED (status)) {
        chirp = NULL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHNL_nextRequestChirp", chirp) ;
    return chirp ;
}


/** ============================================================================
 *  @func   LDRV_CHNL_getRequestChirp
 *
 *  @desc   Gets the chirp from request queue of the specified channel.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
LDRV_CHNL_Irp *
LDRV_CHNL_getRequestChirp (IN ProcessorId   procId,
                           IN ChannelId     chnlId)
{
    DSP_STATUS        status  = DSP_SOK ;
    LDRV_CHNL_Irp *   chirp   = NULL    ;
    LDRVChnlObject *  chnlObj = NULL    ;

    TRC_2ENTER ("LDRV_CHNL_getRequestChirp", procId, chnlId) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (LDRV_CHNL_Object [procId][chnlId] != NULL) ;

    chnlObj = LDRV_CHNL_Object [procId][chnlId] ;

    SYNC_SpinLockStartEx (chnlObj->rLock) ;
    status = LIST_GetHead (chnlObj->requestList, (ListElement **) &chirp) ;
    SYNC_SpinLockEndEx (chnlObj->rLock, 0u) ;
    if (DSP_FAILED (status)) {
        chirp = NULL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("LDRV_CHNL_getRequestChirp", chirp) ;
    return chirp ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   LDRV_CHNL_instrument
 *
 *  @desc   Gets the instrumentation information related to the
 *          specified channel
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_CHNL_instrument (IN  ProcessorId       procId,
                      IN  ChannelId         chnlId,
                      OUT CHNL_Instrument * retVal)
{
    DSP_STATUS       status  = DSP_SOK ;
    LDRVChnlObject * chnlObj = NULL ;

    TRC_3ENTER ("LDRV_CHNL_instrument", procId, chnlId, retVal) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (IS_VALID_CHNLID (procId, chnlId)) ;
    DBC_Require (retVal != NULL) ;

    if (retVal == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        chnlObj = LDRV_CHNL_Object [procId][chnlId] ;
        *retVal = chnlObj->chnlStats.chnlData ;
    }

    TRC_1LEAVE ("LDRV_CHNL_instrument", status) ;

    return status ;
}
#endif


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_CHNL_debug
 *
 *  @desc   Prints out debug information of CHNL module. It will print all
 *          the important data structures and variable of this module.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_CHNL_debug (ProcessorId procId, ChannelId chnlId)
{
    TRC_2ENTER ("LDRV_CHNL_debug", procId, chnlId) ;

    TRC_3PRINT (TRC_LEVEL4, "ChannelObject"
                            " [procId=%d][chnlId =%d] = 0x%x\n",
                            procId, chnlId,
                            LDRV_CHNL_Object [procId][chnlId]) ;

    if (LDRV_CHNL_Object [procId][chnlId] != NULL) {
        TRC_1PRINT (TRC_LEVEL4, "Channel State = [0x%x]\n",
                              LDRV_CHNL_Object [procId][chnlId]->chnlState) ;

        TRC_0PRINT (TRC_LEVEL4, "Free List Info:\n") ;

        LDRV_CHIRPS_Debug (LDRV_CHNL_Object [procId][chnlId]->freeList) ;

        TRC_0PRINT (TRC_LEVEL4, "Free List Info Ends\n") ;

        TRC_0PRINT (TRC_LEVEL4, "Request List Info:\n") ;

        LDRV_CHIRPS_Debug (LDRV_CHNL_Object [procId][chnlId]->requestList) ;

        TRC_0PRINT (TRC_LEVEL4, "Request List Info Ends\n") ;

        TRC_0PRINT (TRC_LEVEL4, "Completed List Info:\n") ;

        LDRV_CHIRPS_Debug (LDRV_CHNL_Object [procId][chnlId]->completedList) ;

        TRC_0PRINT (TRC_LEVEL4, "Completed List Info Ends\n") ;

        TRC_1PRINT (TRC_LEVEL4, "Channel SyncEvent = [0x%x]\n",
                    LDRV_CHNL_Object [procId][chnlId]->syncEvent) ;
    }

    TRC_0PRINT (TRC_LEVEL1, "DEBUG INFO END\n") ;

    TRC_0LEAVE ("LDRV_CHNL_debug") ;
}
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
