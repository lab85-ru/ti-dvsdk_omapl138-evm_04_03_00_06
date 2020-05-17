/** ============================================================================
 *  @file   notify_knl.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/2.6.18/
 *
 *  @desc   Implementation of Kernel-side NOTIFY.
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


/*  ----------------------------------- OS Specific Headers         */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/spinlock.h>
#include <linux/signal.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

#if defined (RINGIO_COMPONENT)
#include <ringio.h>
#endif /* if defined (RINGIO_COMPONENT) */
#if defined (MSGQ_COMPONENT)
#include <msgqdefs.h>
#endif /* if defined (MSGQ_COMPONENT) */

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Profiling                     */
#include <profile.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>

/*  ----------------------------------- LINK driver                 */
#include <ips.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_OSAL_NOTIFY_KNL

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason \
                                       (status, FID_C_OSAL_NOTIFY_KNL, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @macro  MAX_PROCESSES
 *
 *  @desc   Maximum number of user supported.
 *  ============================================================================
 */
#define  MAX_PROCESSES 32u


/** ============================================================================
 *  @name   NOTIFY_KnlRegInfo
 *
 *  @desc   Structure to store the event listener registration information.
 *
 *  @field  element
 *              List element header
 *  @field  pid
 *              Process Identifier for user process.
 *  @field  dspId
 *              Processor identifier
 *  @field  ipsId
 *              IPS identifier
 *  @field  eventNo
 *              Event number used for the registration
 *  @field  cbFxn
 *              Callback function for the event.
 *  @field  cbArg
 *              Callback argument specified during the callback registration
 *              process.
 *  @field  ringIoId
 *              RingIO identifier (Defined on when RINGIO component is selected)
 *  ============================================================================
 */
typedef struct NOTIFY_KnlRegInfo_tag {
    ListElement    element ;
    Uint32         pid ;
    Uint32         dspId ;
    Uint32         ipsId ;
    Uint32         eventNo ;
    FnNotifyCbck   cbFxn ;
    Pvoid          cbArg ;
#if defined (RINGIO_COMPONENT)
    Uint32         ringIoId ;
#endif /* if defined (RINGIO_COMPONENT) */
} NOTIFY_KnlRegInfo ;


/** ============================================================================
 *  @name   UEVENT_Buf
 *
 *  @desc   Structure of User Event.
 *
 *  @field  element
 *              List element.
 *  @field  eventNo
 *              Event No.
 *  @field  data
 *              Data associated with event.
 *  @field  func
 *              User callback function.
 *  @field  param
 *              User callback argument.
 *  @field  isRingio
 *              tells whether ringio packet. (Defined on when RINGIO component
 *              is selected)
 *  ============================================================================
 */
typedef struct UEVENT_Buf_tag {
    ListElement  element ;
    Uint32       eventNo ;
    Uint32       data    ;
    FnNotifyCbck func    ;
    Pvoid        param    ;
#if defined(RINGIO_COMPONENT)
    Uint32       isRingio ;
#endif /* if defined(RINGIO_COMPONENT) */
} UEVENT_Buf ;


/** ============================================================================
 *  @name   UEVENT_State
 *
 *  @desc   Keeps the information related to UEVENT.
 *
 *  @field  bufList
 *              head of received event list.
 *  @field  pid
 *              process group ID.
 *  @field  refCount
 *              reference count, used when multiple notify_register is called
 *              from same process space (multi threads/processes).
 *  @field  semObj
 *              Semphore for waiting on event.
 *  @field  terObj
 *              Semphore for waiting on termination event.
 *  @field  lock
 *              lock for protecting uevent data.
 *  ============================================================================
 */
typedef struct UEVENT_State_tag {
    List *          bufList  ;
    Uint32          pid      ;
    Uint32          refCount ;
    SyncSemObject * semObj   ;
    SyncSemObject * terObj   ;
    Pvoid           lock     ;
} UEVENT_State ;


/** ----------------------------------------------------------------------------
 *  @name   NOTIFY_knlEventList
 *
 *  @desc   Array containing event Listeners.
 *  ----------------------------------------------------------------------------
 */
STATIC List * NOTIFY_knlEventList [MAX_DSPS] ;


#if defined (RINGIO_COMPONENT)
/** ----------------------------------------------------------------------------
 *  @name   NOTIFY_KnlRingIORegInfo
 *
 *  @desc   IPS Registration information for the RingIO event.
 *  ----------------------------------------------------------------------------
 */
STATIC NOTIFY_KnlRegInfo * NOTIFY_KnlRingIORegInfo [MAX_DSPS] ;
#endif /* if defined (RINGIO_COMPONENT) */

/** ============================================================================
 *  @name   uEventProcessBufList
 *
 *  @desc   List headers for all user processes registered.
 *  ============================================================================
 */
STATIC UEVENT_State UEVENT_state [MAX_PROCESSES] ;

/** ============================================================================
 *  @name   UEVENT_IsInitialized
 *
 *  @desc   Flag for indicating initialization status.
 *  ============================================================================
 */
STATIC Bool UEVENT_IsInitialized = FALSE ;

/*  ----------------------------------------------------------------------------
 *  @name   NOTIFY_KnlLockHandle
 *
 *  @desc   Handle to lock used by spinlock.
 *  ----------------------------------------------------------------------------
 */
STATIC Pvoid NOTIFY_KnlLockHandle  [MAX_DSPS]  ;

/*  ----------------------------------------------------------------------------
 *  @name   UEVENT_LockHandle
 *
 *  @desc   Handle to lock used by spinlock.
 *  ----------------------------------------------------------------------------
 */
STATIC Pvoid UEVENT_LockHandle = NULL ;

/*  ----------------------------------------------------------------------------
 *  @name   NOTIFY_KnlInitializeCount
 *
 *  @desc   Variable for tracking initialization count of component.
 *  ----------------------------------------------------------------------------
 */
STATIC Uint32 NOTIFY_KnlInitializeCount [MAX_DSPS] ;

/** ============================================================================
 *  @name   LDRV_LinkCfgPtr
 *
 *  @desc   Global pointer containing DSPLINK configuration information for the
 *          driver.
 *  ============================================================================
 */
extern LINKCFG_Object * LDRV_LinkCfgPtr ;


/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlMatchEvent
 *
 *  @desc   This function matches an event registration info in a list with
 *          the event number used for registration.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
NOTIFY_KnlMatchEvent (IN ListElement * elem, Pvoid  data) ;


#if defined (RINGIO_COMPONENT)
/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlMatchRingIoId
 *
 *  @desc   This function matches an event registration info in a list with
 *          the RingIO Id number used for registration.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
NOTIFY_KnlMatchRingIoId (IN ListElement * elem, Pvoid  data) ;
#endif /* if defined (RINGIO_COMPONENT) */


/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlCallback
 *
 *  @desc   This function implements the callback registered with IPS.
 *
 *  @arg    eventNo
 *              Event number associated with the callback being invoked.
 *  @arg    arg
 *              Fixed argument registered with the IPS component along with
 *              the callback function.
 *  @arg    info
 *              Run-time information provided to the upper layer by the IPS
 *              component. This information is specific to the IPS being
 *              implemented.
 *
 *  @ret    None
 *
 *  @enter  None.
 *
 *  @leave  None
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
NOTIFY_KnlCallback (IN Uint32 eventNo, IN     Pvoid arg, IN     Pvoid info) ;


/** ============================================================================
 *  @func   NOTIFY_Initialize
 *
 *  @desc   This function initializes the Kernel-side NOTIFY component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlInitialize (IN ProcessorId dspId)
{
    DSP_STATUS          status = DSP_SOK ;
#if defined (RINGIO_COMPONENT)
    LINKCFG_DspConfig * dspCfg ;
    LINKCFG_Dsp *       dspObj ;
    LINKCFG_LinkDrv *   linkDrv ;
#endif /* if defined (RINGIO_COMPONENT) */

#if defined (RINGIO_COMPONENT)
    LINKCFG_RingIo *    ringIoObj ;
#endif /* if defined (RINGIO_COMPONENT) */

    TRC_1ENTER ("NOTIFY_KnlInitialize", dspId) ;

    if (NOTIFY_KnlInitializeCount [dspId] == 0) {
        NOTIFY_KnlLockHandle [dspId] = SYNC_SpinLockCreate () ;
        if (NOTIFY_KnlLockHandle [dspId] == NULL) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
    }

    if (   (NOTIFY_KnlInitializeCount [dspId] == 0)
        && (DSP_SUCCEEDED (status))) {
        NOTIFY_KnlInitializeCount [dspId] = 1 ;
        status = LIST_Create (&(NOTIFY_knlEventList [dspId])) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
#if defined (RINGIO_COMPONENT)
        /*
         *  Pre register the kernel call back function for the RingIO
         *  component.
         */
        status = MEM_Alloc ((Void **) &NOTIFY_KnlRingIORegInfo [dspId],
                            sizeof (NOTIFY_KnlRegInfo),
                            MEM_DEFAULT) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            dspCfg    = LDRV_LinkCfgPtr->dspConfigs [dspId] ;
            dspObj    = dspCfg->dspObject ;
            linkDrv   = &(dspCfg->linkDrvObjects [dspObj->linkDrvId]) ;
            ringIoObj =
                    &(dspCfg->ringIoObjects [linkDrv->ringIoTableId]) ;
            status = LIST_InitializeElement (
                                       &(NOTIFY_KnlRingIORegInfo [dspId]->element)) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;

            if (DSP_SUCCEEDED (status)) {
                SYNC_SpinLockStartEx (NOTIFY_KnlLockHandle [dspId]) ;
                NOTIFY_KnlRingIORegInfo [dspId]->pid      = 0 ;
                NOTIFY_KnlRingIORegInfo [dspId]->dspId    = dspId ;
                NOTIFY_KnlRingIORegInfo [dspId]->ipsId    = ringIoObj->ipsId ;
                NOTIFY_KnlRingIORegInfo [dspId]->cbFxn    = NULL ;
                NOTIFY_KnlRingIORegInfo [dspId]->cbArg    = NULL ;
                NOTIFY_KnlRingIORegInfo [dspId]->eventNo  = ringIoObj->ipsEventNo ;
                NOTIFY_KnlRingIORegInfo [dspId]->ringIoId = (Uint32) -1 ;
                status = LIST_PutTail (NOTIFY_knlEventList [dspId],
                                       &(NOTIFY_KnlRingIORegInfo [dspId]->element)) ;
                SYNC_SpinLockEndEx (NOTIFY_KnlLockHandle [dspId], 0u) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                if (DSP_SUCCEEDED (status)) {
                    status = IPS_register (dspId,
                                           NOTIFY_KnlRingIORegInfo [dspId]->ipsId,
                                           NOTIFY_KnlRingIORegInfo [dspId]->eventNo,
                                           NOTIFY_KnlCallback,
                                           NOTIFY_KnlRingIORegInfo [dspId]) ;
                    if (DSP_SUCCEEDED (status)) {
                        status = IPS_setReserved (dspId,
                                                  NOTIFY_KnlRingIORegInfo [dspId]->ipsId,
                                                  NOTIFY_KnlRingIORegInfo [dspId]->eventNo) ;
                        if (DSP_FAILED (status)) {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }
#endif /* if defined (RINGIO_COMPONENT) */
    }
    else {
        NOTIFY_KnlInitializeCount [dspId]++ ;
    }

    DBC_Ensure (   (DSP_SUCCEEDED (status) && NOTIFY_knlEventList [dspId] != NULL)
                || (DSP_FAILED    (status))) ;

    TRC_1LEAVE ("NOTIFY_KnlInitialize", status) ;

    return status ;
}

/** ============================================================================
 *  @func   NOTIFY_KnlFinalize
 *
 *  @desc   This function Finalizes the Kernel-side NOTIFY component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlFinalize (IN ProcessorId dspId)
{
    DSP_STATUS          status    = DSP_SOK ;
    DSP_STATUS          tmpStatus = DSP_SOK ;
    NOTIFY_KnlRegInfo * regInfo  ;

    TRC_1ENTER ("NOTIFY_KnlFinalize", dspId) ;

    if (NOTIFY_KnlInitializeCount [dspId] == 1) {
        NOTIFY_KnlInitializeCount [dspId] = 0 ;
#if defined (RINGIO_COMPONENT)
        /* Unregister the kernel call back function for the RingIO component */
        status = IPS_unregister (dspId,
                                 NOTIFY_KnlRingIORegInfo [dspId]->ipsId,
                                 NOTIFY_KnlRingIORegInfo [dspId]->eventNo,
                                 NOTIFY_KnlCallback,
                                 NOTIFY_KnlRingIORegInfo [dspId]) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        status = IPS_clearReserved (dspId,
                                    NOTIFY_KnlRingIORegInfo [dspId]->ipsId,
                                    NOTIFY_KnlRingIORegInfo [dspId]->eventNo) ;

        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
#endif /* if defined (RINGIO_COMPONENT) */

        while (   (LIST_IsEmpty (NOTIFY_knlEventList [dspId]) != TRUE)
               && (DSP_SUCCEEDED (tmpStatus))) {
            tmpStatus = LIST_GetHead (NOTIFY_knlEventList [dspId],
                                      (ListElement **) &regInfo) ;
            if ((DSP_SUCCEEDED (tmpStatus)) && (regInfo != NULL)) {
                tmpStatus = FREE_PTR (regInfo) ;
                if ((DSP_SUCCEEDED (status)) && (DSP_FAILED (tmpStatus))) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
        }

        LIST_Delete (NOTIFY_knlEventList [dspId]) ;
        if (NOTIFY_KnlLockHandle [dspId] != NULL) {
            SYNC_SpinLockDelete (NOTIFY_KnlLockHandle [dspId]) ;
        }
    }
    else if (NOTIFY_KnlInitializeCount [dspId] > 0) {
        NOTIFY_KnlInitializeCount [dspId]-- ;
    }

    /* Send the termination packet to notify thread */
    UEVENT_AddBufByPid ((Uint32) current->mm,
                        (Uint32) -1,
                        (Uint32) 0,
                        NULL,
                        NULL,
                        0) ;

    TRC_1LEAVE ("NOTIFY_KnlFinalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_KnlRegister
 *
 *  @desc   This function registers a callback from an event.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOTIFY_KnlRegister (IN     ProcessorId   dspId,
                    IN     Uint32        ipsId,
                    IN     Uint32        eventNo,
                    IN     FnNotifyCbck  cbFxn,
                    IN     Pvoid         cbArg)
{
    DSP_STATUS          status = DSP_SOK ;
    NOTIFY_KnlRegInfo * regInfo  ;

    DBC_Require (cbFxn != NULL) ;

    TRC_5ENTER ("NOTIFY_KnlRegister", dspId, ipsId, eventNo, cbFxn, cbArg) ;

    if (NOTIFY_KnlInitializeCount [dspId] >= 1) {
        status = MEM_Alloc ((Void **) &regInfo,
                            sizeof (NOTIFY_KnlRegInfo),
                            MEM_DEFAULT) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            status = LIST_InitializeElement (&(regInfo->element)) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;

            SYNC_SpinLockStartEx (NOTIFY_KnlLockHandle [dspId]) ;
            regInfo->pid     = (Uint32) current->mm ;
            regInfo->dspId   = dspId ;
            regInfo->ipsId   = ipsId ;
            regInfo->cbFxn   = cbFxn ;
            regInfo->cbArg   = cbArg ;
            regInfo->eventNo = eventNo ;
#if defined (RINGIO_COMPONENT)
            if (   (eventNo == NOTIFY_KnlRingIORegInfo [dspId]->eventNo)
                && (ipsId   == NOTIFY_KnlRingIORegInfo [dspId]->ipsId)) {
                regInfo->ringIoId =
                      ((RingIO_Client *) cbArg)->virtControlHandle->entryId ;
            }
#endif /* if defined (RINGIO_COMPONENT) */

            status = LIST_PutTail (NOTIFY_knlEventList [dspId], &(regInfo->element)) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            SYNC_SpinLockEndEx (NOTIFY_KnlLockHandle [dspId], 0u) ;

            /*
             *  If this is not a registration attempt for RingIO event, then
             *  go ahead and register the callback information with IPS
             */
#if defined (RINGIO_COMPONENT)
            if (   (ipsId != NOTIFY_KnlRingIORegInfo [dspId]->ipsId)
                || (   (ipsId == NOTIFY_KnlRingIORegInfo [dspId]->ipsId)
                    && (eventNo != NOTIFY_KnlRingIORegInfo [dspId]->eventNo))) {
#endif /* if defined (RINGIO_COMPONENT) */
                status = IPS_register (dspId,
                                       ipsId,
                                       eventNo,
                                       NOTIFY_KnlCallback,
                                       regInfo) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
#if defined (RINGIO_COMPONENT)
            }
#endif /* if defined (RINGIO_COMPONENT) */
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("NOTIFY_KnlRegister", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_KnlUnregister
 *
 *  @desc   This function unregisters a callback from an event.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOTIFY_KnlUnregister (IN     ProcessorId    dspId,
                      IN     Uint32         ipsId,
                      IN     Uint32         eventNo,
                      IN     FnNotifyCbck   cbFxn,
                      IN     Pvoid          cbArg)
{
    DSP_STATUS          status    = DSP_SOK ;
    DSP_STATUS          tmpStatus = DSP_SOK ;
    NOTIFY_KnlRegInfo * regInfo  ;
    NOTIFY_KnlRegInfo   unregInfo ;

    TRC_5ENTER ("NOTIFY_KnlUnregister", dspId, ipsId, eventNo, cbFxn, cbArg) ;

    if (NOTIFY_KnlInitializeCount [dspId] >= 1) {
        unregInfo.dspId   = dspId ;
        unregInfo.ipsId   = ipsId ;
        unregInfo.cbFxn   = cbFxn ;
        unregInfo.cbArg   = cbArg ;
        unregInfo.eventNo = eventNo ;
        SYNC_SpinLockStartEx (NOTIFY_KnlLockHandle [dspId]) ;
        status = LIST_SearchElement (
                       (List *) NOTIFY_knlEventList [dspId],
                       (Pvoid) &unregInfo,
                       (ListElement **) &regInfo,
                       (ListMatchFunc) (&NOTIFY_KnlMatchEvent)) ;

        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            tmpStatus = LIST_RemoveElement (NOTIFY_knlEventList [dspId],
                            &(regInfo->element)) ;
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        }

        SYNC_SpinLockEndEx (NOTIFY_KnlLockHandle [dspId], 0u) ;

        if (regInfo != NULL) {
#if defined (RINGIO_COMPONENT)
            if (   (ipsId != NOTIFY_KnlRingIORegInfo [dspId]->ipsId)
                || (   (ipsId == NOTIFY_KnlRingIORegInfo [dspId]->ipsId)
                    && (eventNo != NOTIFY_KnlRingIORegInfo [dspId]->eventNo))) {
#endif /* if defined (RINGIO_COMPONENT) */
                tmpStatus = IPS_unregister (dspId,
                                            ipsId,
                                            eventNo,
                                            NOTIFY_KnlCallback,
                                            regInfo) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

#if defined (RINGIO_COMPONENT)
            }
#endif /* if defined (RINGIO_COMPONENT) */

            tmpStatus = FREE_PTR (regInfo) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
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

    TRC_1LEAVE ("NOTIFY_KnlUnregister", status) ;

    return status ;
}


/** ============================================================================
 *  @func   NOTIFY_KnlNotify
 *
 *  @desc   This function notifys the other side about an event.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
NOTIFY_KnlNotify (IN     ProcessorId dspId,
                  IN     Uint32      ipsId,
                  IN     Uint32      eventNo,
                  IN     Uint32      payload)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_4ENTER ("NOTIFY_KnlNotify", dspId, ipsId, eventNo, payload) ;

    if (NOTIFY_KnlInitializeCount [dspId] >= 1) {
        status = IPS_notify (dspId, ipsId, eventNo, payload, TRUE) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1ENTER ("NOTIFY_KnlNotify", status) ;

    return status ;
}

/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlMatchEvent
 *
 *  @desc   This function matches an event listener with a list element.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
NOTIFY_KnlMatchEvent (IN ListElement * elem, Pvoid data)
{
    Bool                retVal    = FALSE ;
    NOTIFY_KnlRegInfo * regInfo   = NULL  ;
    NOTIFY_KnlRegInfo * unRegInfo = NULL  ;

    TRC_2ENTER ("NOTIFY_KnlMatchEvent", elem, data) ;

    DBC_Require (elem != NULL) ;

    regInfo   = (NOTIFY_KnlRegInfo *) elem ;
    unRegInfo = (NOTIFY_KnlRegInfo *) data ;
    if (    (regInfo->dspId   == unRegInfo->dspId)
        &&  (regInfo->ipsId   == unRegInfo->ipsId)
        &&  (regInfo->cbFxn   == unRegInfo->cbFxn)
        &&  (regInfo->cbArg   == unRegInfo->cbArg)
        &&  (regInfo->eventNo == unRegInfo->eventNo)) {
        retVal = TRUE ;
    }

    TRC_1ENTER ("NOTIFY_KnlMatchEvent", retVal) ;

    return retVal ;
}


#if defined (RINGIO_COMPONENT)
/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlMatchRingIoId
 *
 *  @desc   This function matches an event registration info in a list with
 *          the RingIO Id number used for registration.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Bool
NOTIFY_KnlMatchRingIoId (IN ListElement * elem, Pvoid  data)
{
    Bool                retVal  = FALSE ;
    NOTIFY_KnlRegInfo * regInfo = NULL  ;
    Uint32              entryId = (Uint32) data & 0xFFFF ;

    TRC_2ENTER ("NOTIFY_KnlMatchRingIoId", elem, data) ;

    DBC_Require (elem != NULL) ;

    regInfo = (NOTIFY_KnlRegInfo *) elem ;
    if (    (    regInfo->eventNo
             ==  NOTIFY_KnlRingIORegInfo [regInfo->dspId]->eventNo)
        &&  (   regInfo->ipsId
             == NOTIFY_KnlRingIORegInfo [regInfo->dspId]->ipsId)
        &&  (regInfo->ringIoId == (Uint32) entryId)) {
        retVal = TRUE ;
    }

    TRC_1ENTER ("NOTIFY_KnlMatchRingIoId", retVal) ;

    return retVal ;
}
#endif /* if defined (RINGIO_COMPONENT) */


/** ----------------------------------------------------------------------------
 *  @func   NOTIFY_KnlCallback
 *
 *  @desc   This function implements the callback registered with IPS. Here
 *          to pass event no. back to user function (so that it can do another
 *          level of demultiplexing of callbacks)
 *
 *          This function is called in ISR context on Linux.
 *
 *  @modif  None.
 *  ----------------------------------------------------------------------------
 */
STATIC
NORMAL_API
Void
NOTIFY_KnlCallback (IN Uint32 eventNo, IN     Pvoid arg, IN     Pvoid info)
{
#if defined (RINGIO_COMPONENT)
    DSP_STATUS           status     = DSP_EFAIL ;
    NOTIFY_KnlRegInfo *  regInfo    = NULL ;
#endif /* if defined (RINGIO_COMPONENT) */
    NOTIFY_KnlRegInfo *  argRegInfo = NULL ;
    ProcessorId          dspId  ;

    DBC_Require (arg != NULL) ;

    argRegInfo = (NOTIFY_KnlRegInfo *) arg ;
    if (arg != NULL) {
        dspId  = argRegInfo->dspId ;
#if defined (RINGIO_COMPONENT)
        if (    (eventNo           == NOTIFY_KnlRingIORegInfo [dspId]->eventNo)
            &&  (argRegInfo->ipsId == NOTIFY_KnlRingIORegInfo [dspId]->ipsId)) {
            /* Find the pid which has to be signaled for this ringio instance */
            status = LIST_SearchElement (
                                   (List *) NOTIFY_knlEventList [dspId],
                                   info,            /* This is the ringio Id */
                                   (ListElement **) &regInfo,
                                   (ListMatchFunc) (&NOTIFY_KnlMatchRingIoId)) ;
            if (DSP_SUCCEEDED (status)) {
                UEVENT_AddBufByPid (regInfo->pid,
                                    regInfo->eventNo,
                                    (Uint32) info,
                                    NULL,
                                    regInfo->cbArg,
                                    1 /* RINGIO EVENT */) ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
        else {
#endif /* if defined (RINGIO_COMPONENT) */
            UEVENT_AddBufByPid (argRegInfo->pid,
                                argRegInfo->eventNo,
                                (Uint32) info,
                                argRegInfo->cbFxn,
                                argRegInfo->cbArg,
                                0 /* NORMAL EVENT */) ;
#if defined (RINGIO_COMPONENT)
       }
#endif /* if defined (RINGIO_COMPONENT) */
    }
}


/** ============================================================================
 *  @func   UEVENT_Initialize
 *
 *  @desc   This function initializes the UEVENT notification component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
UEVENT_Initialize (Void)
{
    Uint32 i ;

    TRC_0ENTER ("UEVENT_Initialize") ;

    if (UEVENT_IsInitialized == FALSE) {
        for (i = 0 ; i < MAX_PROCESSES ; i++) {
            UEVENT_state [i].bufList = NULL ;
            UEVENT_state [i].pid     = -1   ;
            UEVENT_state [i].refCount = 0   ;
        }

        /* Create the lock handle */
        UEVENT_IsInitialized = TRUE ;
        UEVENT_LockHandle = SYNC_SpinLockCreate ()  ;
        DBC_Assert (UEVENT_LockHandle != NULL) ;
    }

    TRC_0LEAVE ("UEVENT_Initialize") ;
}


/** ============================================================================
 *  @func   UEVENT_Finalize
 *
 *  @desc   This function finalize the UEVENT notification component.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Void
UEVENT_Finalize (Void)
{
    Uint32 i ;

    TRC_0ENTER ("UEVENT_Finalize") ;

    if (UEVENT_IsInitialized == TRUE) {

        SYNC_SpinLockStartEx (UEVENT_LockHandle) ;
        for (i = 0 ; i < MAX_PROCESSES ; i++) {
            UEVENT_state [i].bufList = NULL ;
            UEVENT_state [i].pid     = -1   ;
            UEVENT_state [i].refCount = 0   ;
        }
        SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;

        /* Now safely delete the lock */
        SYNC_SpinLockDelete (UEVENT_LockHandle) ;

        UEVENT_IsInitialized = FALSE ;
    }

    TRC_0LEAVE ("UEVENT_Finalize") ;
}


/** ============================================================================
 *  @func   UEVENT_AddNewProcess
 *
 *  @desc   This function adds a new process for getting event nofiicatin.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_AddNewProcess (Void)
{
    DSP_STATUS      status  = DSP_SOK ;
    Bool            flag    = FALSE ;
    Uint32          pid     = (Uint32) current->mm ;
    Uint32          doInit  = 0 ;
    List *          bufList = NULL ;
    SyncSemObject * semObj  = NULL ;
    SyncSemObject * terObj  = NULL ;
    SyncAttrs       semAttrs ;
    Uint32          i ;

    TRC_0ENTER ("UEVENT_AddNewProcess") ;

    if (UEVENT_IsInitialized == TRUE) {
        SYNC_SpinLockStartEx (UEVENT_LockHandle) ;
        for (i = 0 ; (i < MAX_PROCESSES) ; i++) {
            if (UEVENT_state [i].pid == pid) {
                UEVENT_state [i].refCount++ ;
                doInit = 1 ;
                break ;
            }
        }
        SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;

        if (doInit == 0) {
            status = LIST_Create (&bufList) ;
            if (DSP_SUCCEEDED (status)) {
                /* Create the semaphore */
                semAttrs.flag = SyncSemType_Binary ;
                /* Create the termination semaphore */
                status = SYNC_CreateSEM (&terObj, &semAttrs) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                status = SYNC_CreateSEM (&semObj, &semAttrs) ;
               if (DSP_SUCCEEDED (status)) {
                    SYNC_SpinLockStartEx (UEVENT_LockHandle) ;
                    for (i = 0 ; (i < MAX_PROCESSES) && (flag != TRUE) ; i++) {
                        if (UEVENT_state [i].pid == -1) {
                            UEVENT_state [i].semObj = semObj ;
                            UEVENT_state [i].terObj = terObj ;
                            UEVENT_state [i].pid = pid ;
                            UEVENT_state [i].refCount++ ;
                            UEVENT_state [i].bufList = bufList ;
                            flag = TRUE ;
                            break ;
                        }
                    }
                    SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;
                }
                else {
                    SET_FAILURE_REASON ;
                }

               /* Create the lock */
               if (DSP_SUCCEEDED (status)) {
                   UEVENT_state [i].lock = SYNC_SpinLockCreate () ;
               }

                if (flag != TRUE) {
                    LIST_Delete (bufList) ;
                    if (semObj != NULL) {
                        SYNC_DeleteSEM (semObj) ;
                    }
                    status = DSP_EFAIL ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("UEVENT_AddNewProcess", status) ;

    return status ;
}


/** ============================================================================
 *  @func   UEVENT_RemoveUserProcess
 *
 *  @desc   This function adds a new process for getting event nofiicatin.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_RemoveUserProcess (Void)
{
    DSP_STATUS      status    = DSP_SOK ;
    DSP_STATUS      tmpStatus = DSP_SOK ;
    Bool            flag      = FALSE   ;
    Uint32          pid       = (Uint32) current->mm ;
    List *          bufList   = NULL ;
    SyncSemObject * semObj    = NULL ;
    SyncSemObject * terObj    = NULL ;
    Pvoid           lock      = NULL ;
    Uint32          i ;

    TRC_0ENTER ("UEVENT_RemoveUserProcess") ;

    if (UEVENT_IsInitialized == TRUE) {
        SYNC_SpinLockStartEx (UEVENT_LockHandle) ;

        for (i = 0 ; (i < MAX_PROCESSES) && (flag != TRUE) ; i++) {
            if (UEVENT_state [i].pid == pid) {
                if (UEVENT_state [i].refCount == 1) {
                    UEVENT_state [i].pid = -1 ;
                    UEVENT_state [i].refCount-- ;

                    /* Store in local variable to delete outside
                     * lock.
                     */
                    bufList = UEVENT_state [i].bufList ;
                    semObj  = UEVENT_state [i].semObj ;
                    terObj  = UEVENT_state [i].terObj ;
                    lock    = UEVENT_state [i].lock  ;

                    /* Release any waiting client on completion. */
                    UEVENT_state [i].bufList = NULL ;
                    UEVENT_state [i].semObj  = NULL ;
                    UEVENT_state [i].terObj  = NULL ;
                    UEVENT_state [i].lock  = NULL ;

                    flag = TRUE ;
                    break ;
                }
                else {
                    UEVENT_state [i].refCount-- ;
                }
            }
        }
        SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;

        if (flag != TRUE) {
            if (i == MAX_PROCESSES) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = LIST_Delete (bufList) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }

            /* Remove/delete the termination semphore */
            SYNC_DeleteSEM (terObj) ;

            /* Remove/delete the semphore */
            tmpStatus = SYNC_DeleteSEM (semObj) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            /* Delete the lock */
            SYNC_SpinLockDelete (lock) ;
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("UEVENT_RemoveUserProcess", status) ;

    return status ;
}


/** ============================================================================
 *  @func   UEVENT_AddBufByPid
 *
 *  @desc   This function adds a data to a registered process.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
UEVENT_AddBufByPid (IN     Uint32       pid,
                    IN     Uint32       eventNo,
                    IN     Uint32       data,
                    IN     FnNotifyCbck cbFxn,
                    IN     Pvoid        param,
                    IN     Uint32       isRingio)
{
    DSP_STATUS    status = DSP_SOK ;
    Bool          flag   = FALSE   ;
    UEVENT_Buf  * uBuf    = NULL   ;
    Uint32        i                ;

    TRC_5ENTER ("UEVENT_AddBufByPid", pid, eventNo, data, cbFxn, param) ;

    if (UEVENT_IsInitialized == TRUE) {
        SYNC_SpinLockStartEx (UEVENT_LockHandle) ;
        for (i = 0 ; (i < MAX_PROCESSES) && (flag != TRUE) ; i++) {
            if (UEVENT_state [i].pid == pid) {
                flag = TRUE ;
                break ;
            }
        }
        SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;

        if (flag != TRUE) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
        else {
            uBuf = (UEVENT_Buf *) kmalloc (sizeof (UEVENT_Buf), GFP_ATOMIC) ;
            if (uBuf != NULL) {
                status = LIST_InitializeElement (&uBuf->element) ;
                if (DSP_FAILED (status)) {
                    kfree (uBuf) ;
                    SET_FAILURE_REASON ;
                }
                else {
                    uBuf->data    = data ;
                    uBuf->eventNo = eventNo ;
                    uBuf->func    = cbFxn ;
                    uBuf->param   = param ;
#if defined(RINGIO_COMPONENT)
                    uBuf->isRingio  = isRingio ;
#endif /* if defined(RINGIO_COMPONENT) */
                    SYNC_SpinLockStartEx (UEVENT_state [i].lock) ;
                    status = LIST_PutTail (UEVENT_state [i].bufList,
                                           &(uBuf->element)) ;
                    SYNC_SpinLockEndEx (UEVENT_state [i].lock, 0u) ;

                    if (DSP_FAILED (status)) {
                        kfree (uBuf) ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* Post the semphore */
                        SYNC_SignalSEM (UEVENT_state [i].semObj) ;
                        /* Termination packet */
                        if (uBuf->eventNo == (Uint32) -1) {
                            SYNC_WaitSEM (UEVENT_state [i].terObj,
                                          SYNC_WAITFOREVER) ;
                        }
                    }
                }
            }
        }
    }
    else {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("UEVENT_AddBufByPid", status) ;

    return status ;
}


/** ============================================================================
 *  @func   UEVENT_GetBuf
 *
 *  @desc   This function gets a buf if there any.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Uint32
UEVENT_GetBuf (OUT Pvoid packet)
{
    DSP_STATUS    status = DSP_SOK ;
    Bool          flag   = FALSE   ;
    UEVENT_Buf  * uBuf   = NULL    ;
    Uint32        pid    = (Uint32) current->mm ;
    Uint32        retVal = 0       ;
    Uint32        i                ;

    DBC_Require (packet != NULL) ;

    TRC_1ENTER ("UEVENT_GetBuf", packet) ;

    if (UEVENT_IsInitialized == TRUE) {
        SYNC_SpinLockStartEx (UEVENT_LockHandle) ;
        for (i = 0 ; (i < MAX_PROCESSES) && (flag != TRUE) ; i++) {
            if (UEVENT_state [i].pid == pid) {
                flag = TRUE ;
                break ;
            }
        }
        SYNC_SpinLockEndEx (UEVENT_LockHandle, 0u) ;

        if (flag == TRUE) {
            if (UEVENT_state [i].bufList != NULL) {

                /* Wait for the event, only if packet is not present */
                SYNC_SpinLockStartEx (UEVENT_state [i].lock) ;
                if (LIST_IsEmpty (UEVENT_state [i].bufList)) {
                    SYNC_SpinLockEndEx (UEVENT_state [i].lock, 0u) ;

                    status = SYNC_WaitSEM (UEVENT_state [i].semObj,
                                           SYNC_WAITFOREVER) ;
                    SYNC_SpinLockStartEx (UEVENT_state [i].lock) ;
                }
                SYNC_SpinLockEndEx (UEVENT_state [i].lock, 0u) ;

                if (DSP_SUCCEEDED (status)) {
                    SYNC_SpinLockStartEx (UEVENT_state [i].lock) ;

                    status = LIST_GetHead (UEVENT_state [i].bufList,
                                           (ListElement **) &uBuf) ;

                    SYNC_SpinLockEndEx (UEVENT_state [i].lock, 0u) ;

                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    if (uBuf != NULL) {
                        retVal = copy_to_user ((Pvoid) packet,
                                               uBuf,
                                               sizeof (UEVENT_Buf)) ;

                        if (retVal != 0) {
                            TRC_1PRINT (TRC_LEVEL4,
                                        "copy_to_user call failed %d\n",
                                        retVal) ;
                        }
                        else {
                            retVal = sizeof (UEVENT_Buf) ;
                            if (uBuf->eventNo == (Uint32) -1) {
                                /* Post the semphore */
                                SYNC_SignalSEM (UEVENT_state [i].terObj) ;
                            }
                        }
                        kfree (uBuf) ;
                    }
                }
                else {
                    retVal = -ERESTARTSYS ;
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("UEVENT_GetBuf", retVal) ;

    return retVal ;
}


#if defined (DDSP_PROFILE)
/** ============================================================================
 *  @func   NOTIFY_KnlInstrument
 *
 *  @desc   This function gives the profile instrumentation
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
NOTIFY_KnlInstrument (IN     ProcessorId      dspId,
                      IN     Uint32           ipsId,
                      OUT    IPS_Instrument * retVal)
{
    DSP_STATUS          status = DSP_SOK ;

    DBC_Require (IS_VALID_PROCID (dspId)) ;
    DBC_Require (retVal != NULL) ;

    TRC_3ENTER ("NOTIFY_KnlInstrument", dspId, ipsId, retVal) ;

    status = IPS_instrument (dspId, ipsId, retVal) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("NOTIFY_KnlInstrument", status) ;

    return status ;
}
#endif /* if defined (DDSP_PROFILE) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
