/** ============================================================================
 *  @file   sync.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/2.6.18/
 *
 *  @desc   Implementation of sub-component SYNC.
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
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/completion.h>
#include <linux/wait.h>
#include <linux/hardirq.h>
#include <linux/rtmutex.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>
#include <list.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Identifier for sub-component.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_OSAL_SYNC

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_OSAL_SYNC, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @name   SYNC_SpinLockObject
 *
 *  @desc   This object is used by spinlock APIs.
 *
 *  @field  lock
 *              handle to lock object used by OS.
 *  @field  irqFlags
 *              Irq flags value returned from spin_lock_irqsave ().
 *
 *  @see    None
 *  ============================================================================
 */
typedef struct SYNC_SpinLockObject_tag {
    struct mutex lock     ;
    Uint32       irqFlags ;
} SYNC_SpinLockObject ;


/** ============================================================================
 *  @name   SyncEvObject
 *
 *  @desc   This object is used for various event related API.
 *
 *  @field  signature
 *              For identification of this object.
 *  @field  list
 *              List for waiting processes.
 *  @field  value
 *              Current value of the event.
 *  @field  lock
 *              To protect write/read access to event.
 *
 *  @see    None
 *  ============================================================================
 */
struct SyncEvObject_tag {
    Uint32            signature       ;
    wait_queue_head_t list            ;
    Uint32            value           ;
} ;


/** ============================================================================
 *  @name   SyncCsObject
 *
 *  @desc   This object is used by various CS API's.
 *
 *  @field  signature
 *              For identification of this object.
 *  @field  csLock
 *              OS specific lock that is used to implement CS API.
 *
 *  @see    None
 *  ============================================================================
 */
struct SyncCsObject_tag {
    Uint32       signature ;
    struct mutex csLock    ;
} ;


/** ============================================================================
 *  @name   SyncSemObject
 *
 *  @desc   This object is used by various SEM API's.
 *
 *  @field  signature
 *              For identification of this object.
 *  @field  semType
 *              Indicates the type of the semaphore (binary or counting).
 *  @field  list
 *              List of waiting processes.
 *  @field  value
 *              Current status of semaphore (0,1) - binary and (0-n) counting.
 *  @field  lock
 *              Indicates that timeout had occurred.
 *
 *  @see    None
 *  ============================================================================
 */
struct SyncSemObject_tag {
    Uint32            signature ;
    SyncSemType       semType   ;
    wait_queue_head_t list      ;
    Uint32            value     ;
} ;


/** ----------------------------------------------------------------------------
 *  @name   SYNC_IsInitialized
 *
 *  @desc   Indicates if the module has been initialized or not.
 *  ----------------------------------------------------------------------------
 */
STATIC Bool SYNC_IsInitialized = FALSE ;

/** ----------------------------------------------------------------------------
 *  @name   SYNC_PreemptLock
 *
 *  @desc   Spin lock structure to provide protection from IRQ, DPC, TASKs,
 *          tasks and tasklets.
 *  ----------------------------------------------------------------------------
 */
STATIC struct mutex SYNC_PreemptLock ;

/** ----------------------------------------------------------------------------
 *  @name   SYNC_ProtectCsObj
 *
 *  @desc   Handle to the critical section object used for protecting against
 *          other tasks during SYNC_ProtectionStart ()/SYNC_ProtectionEnd ()
 *          calls.
 *  ----------------------------------------------------------------------------
 */
STATIC SyncCsObject * SYNC_ProtectCsObj = NULL ;

/** ============================================================================
 *  @func   SYNC_Initialize
 *
 *  @desc   Initialize SYNC sub-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_Initialize ()
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_0ENTER ("SYNC_Initialize") ;

    DBC_Require (SYNC_IsInitialized == FALSE) ;

    /*  Create the SYNC protection CS object  */
    status = SYNC_CreateCS (&SYNC_ProtectCsObj) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    /* Initialize the global general lock */
    mutex_init (&SYNC_PreemptLock) ;

    SYNC_IsInitialized = TRUE ;

    TRC_1LEAVE ("SYNC_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_Finalize
 *
 *  @desc   Releases resources used by the sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_Finalize ()
{
    DSP_STATUS  status = DSP_SOK ;

    TRC_0ENTER ("SYNC_Finalize") ;

    DBC_Require (SYNC_IsInitialized == TRUE) ;

    /*  Delete the SYNC protection CS object  */
    if (SYNC_ProtectCsObj != NULL) {
        status = SYNC_DeleteCS (SYNC_ProtectCsObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        SYNC_ProtectCsObj = NULL ;
    }

    SYNC_IsInitialized = FALSE ;

    TRC_1LEAVE ("SYNC_Finalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_OpenEvent
 *
 *  @desc   Creates and initializes an event object for thread
 *          synchronization. The event is initialized to a
 *          non-signaled state.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_OpenEvent (OUT SyncEvObject ** event,
                IN  SyncAttrs *     attr)
{
    DSP_STATUS     status = DSP_SOK ;
    SyncEvObject * _event           ;

    TRC_2ENTER ("SYNC_OpenEvent", event, attr) ;

    DBC_Require (event != NULL) ;
    DBC_Require (attr != NULL) ;

    if (attr == NULL) {
        if (event != NULL) {
            *event = NULL ;
        }
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (event == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        status = MEM_Calloc ((Void **) event, sizeof (SyncEvObject),
                            MEM_DEFAULT) ;
        if (DSP_SUCCEEDED (status)) {
            _event = *event ;
            /* Initial value of the event is zero, not set */
            _event->value = 0u ;
            /* Initialize the waiting list */
            init_waitqueue_head (&_event->list) ;
            _event->signature = SIGN_SYNC ;
        }
        else {
            SET_FAILURE_REASON ;
            *event = NULL ;
        }
    }

    DBC_Ensure (   ((event == NULL) && DSP_FAILED (status))
                 || ((*event == NULL) && DSP_FAILED (status))
                 || (   ((*event)->signature == SIGN_SYNC)
                     && DSP_SUCCEEDED (status))) ;

    TRC_1LEAVE ("SYNC_OpenEvent", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_CloseEvent
 *
 *  @desc   Closes the handle corresponding to an event. It also frees the
 *          resources allocated, if any, during call to SYNC_OpenEvent ().
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_CloseEvent (IN SyncEvObject * event)
{
    DSP_STATUS   status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_CloseEvent", event) ;

    DBC_Require (IS_OBJECT_VALID (event, SIGN_SYNC)) ;
    DBC_Require (event != NULL) ;

    status = (IS_OBJECT_VALID (event, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        event->signature = SIGN_NULL ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        status = FREE_PTR (event) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("SYNC_CloseEvent", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_ResetEvent
 *
 *  @desc   Reset the synchronization object to non-signaled state.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_ResetEvent (IN SyncEvObject * event)
{
    DSP_STATUS   status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_ResetEvent", event) ;

    DBC_Require (event != NULL) ;
    DBC_Require (IS_OBJECT_VALID (event, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (event, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        preempt_disable ();
        event->value = 0u ;
        preempt_enable ();
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_ResetEvent", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_SetEvent
 *
 *  @desc   Set the state of synchronization object to signaled and unblock all
 *          threads waiting for it.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_SetEvent (IN SyncEvObject * event)
{
    DSP_STATUS status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_SetEvent", event) ;

    DBC_Require (IS_OBJECT_VALID (event, SIGN_SYNC)) ;
    DBC_Require (event != NULL) ;

    status = (IS_OBJECT_VALID (event, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        preempt_disable ();
        event->value = 1u ;
        preempt_enable () ;
        wake_up_interruptible_all (&event->list) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_SetEvent", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_WaitOnEvent
 *
 *  @desc   Wait for an event to be signaled for a specified amount of time.
 *          It is possible to wait infinitely. This function must 'block'
 *          and not 'spin'.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_WaitOnEvent (IN SyncEvObject * event, IN Uint32 timeout)
{
    DSP_STATUS status   = DSP_SOK ;
    Int32      osStatus           ;

    TRC_2ENTER ("SYNC_WaitOnEvent", event, timeout) ;

    DBC_Require (event != NULL) ;
    DBC_Require (IS_OBJECT_VALID (event, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (event, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        if (timeout == SYNC_NOWAIT) { /* Zero timeout */
            preempt_disable ();
            if (event->value == 0u) {
                status = SYNC_E_FAIL ;
                SET_FAILURE_REASON ;
            }
            preempt_enable ();
        }
        else if (timeout != SYNC_WAITFOREVER) { /* Finite timeout */
            DECLARE_WAITQUEUE (wait, current) ;
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&event->list, &wait) ;
            do {
                set_current_state (TASK_INTERRUPTIBLE) ;
                preempt_disable ();
                if (event->value != 0) {
                    preempt_enable ();
                    break;
                }
                preempt_enable ();
                osStatus = schedule_timeout (msecs_to_jiffies (timeout)) ;
                /* Check for status */
                if (signal_pending(current)) {
                    status = -ERESTARTSYS ;
                    SET_FAILURE_REASON ;
                    break ;
                }
                if (osStatus <= 0) {
                    status = DSP_ETIMEOUT ;
                    SET_FAILURE_REASON ;
                    break ;
                }
            } while (1);
            /* Remove from wait list */
            remove_wait_queue (&event->list, &wait) ;
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING) ;
        }
        else { /* Forever */
            DECLARE_WAITQUEUE (wait, current) ;
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&event->list, &wait) ;
            do {
                set_current_state (TASK_INTERRUPTIBLE) ;
                preempt_disable ();
                if (event->value != 0) {
                    preempt_enable ();
                    break;
                }
                preempt_enable ();
                osStatus = schedule_timeout (MAX_SCHEDULE_TIMEOUT) ;
                /* Check for status */
                if (signal_pending(current)) {
                    status = -ERESTARTSYS ;
                    SET_FAILURE_REASON ;
                    break ;
                }
                if (osStatus <= 0) {
                    status = DSP_ETIMEOUT ;
                    SET_FAILURE_REASON ;
                    break ;
                }
            } while (1);
            /* Remove from wait list */
            remove_wait_queue (&event->list, &wait) ;
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING) ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_WaitOnEvent", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_WaitOnMultipleEvents
 *
 *  @desc   Waits on muliple events. Returns when any of the event is set.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_WaitOnMultipleEvents (IN  SyncEvObject **   syncEvents,
                           IN  Uint32            count,
                           IN  Uint32            timeout,
                           OUT Uint32 *          index)
{
    DSP_STATUS      status = DSP_SOK ;

    TRC_4ENTER ("SYNC_WaitOnMultipleEvents", syncEvents, count,
                                             timeout, index) ;
    DBC_Require (syncEvents != NULL) ;
    DBC_Require (index != NULL) ;

    /* Not Implemented */
    status = DSP_ENOTIMPL ;
    SET_FAILURE_REASON ;

    TRC_1LEAVE ("SYNC_WaitOnMultipleEvents", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_CreateCS
 *
 *  @desc   Creates the Critical section structure.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_CreateCS (OUT SyncCsObject ** cSObj)
{
    DSP_STATUS   status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_CreateCS", cSObj) ;

    DBC_Require (cSObj != NULL) ;

    if (cSObj == NULL) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }

    if (DSP_SUCCEEDED (status)) {
        status =  MEM_Calloc ((Void **) cSObj, sizeof (SyncCsObject),
                              MEM_DEFAULT) ;
    }

    if (DSP_SUCCEEDED (status)) {
        mutex_init (&((*cSObj)->csLock)) ;
        (*cSObj)->signature = SIGN_SYNC ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    DBC_Ensure (   ((cSObj == NULL) && DSP_FAILED (status))
                 || ((*cSObj == NULL) && DSP_FAILED (status))
                 || (   ((*cSObj)->signature == SIGN_SYNC)
                     && DSP_SUCCEEDED (status))) ;

    TRC_1LEAVE ("SYNC_CreateCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_DeleteCS
 *
 *  @desc   Deletes the critical section object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_DeleteCS (IN SyncCsObject * cSObj)
{
    DSP_STATUS   status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_DeleteCS", cSObj) ;

    DBC_Require (cSObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (cSObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (cSObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        cSObj->signature = SIGN_NULL ;
        status = FREE_PTR (cSObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_DeleteCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_EnterCS
 *
 *  @desc   This function enters the critical section that is passed as
 *          argument to it. After successful return of this function no other
 *          thread can enter until this thread exit CS by calling SYNC_LeaveCS
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_EnterCS (IN SyncCsObject * cSObj)
{
    DSP_STATUS   status   = DSP_SOK    ;
    DSP_STATUS   tmpStatus = DSP_SOK ;

    TRC_1ENTER ("SYNC_EnterCS", cSObj) ;

    DBC_Require (cSObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (cSObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (cSObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        tmpStatus = mutex_lock_interruptible (&cSObj->csLock) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_EnterCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_LeaveCS
 *
 *  @desc   This function makes critical section available for other threads
 *          to enter.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_LeaveCS (IN SyncCsObject * cSObj)
{
    DSP_STATUS   status   = DSP_SOK    ;

    TRC_1ENTER ("SYNC_LeaveCS", cSObj) ;

    DBC_Require (cSObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (cSObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (cSObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        mutex_unlock (&cSObj->csLock) ;
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_LeaveCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_CreateSEM
 *
 *  @desc   Creates the semaphore object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_CreateSEM (OUT SyncSemObject ** semObj, IN SyncAttrs * attr)
{
    DSP_STATUS   status   = DSP_SOK ;

    TRC_2ENTER ("SYNC_CreateSEM", semObj, attr) ;

    DBC_Require (semObj != NULL) ;
    DBC_Require (attr != NULL) ;

    if ((semObj == NULL) || (attr == NULL)) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else if (   ((SyncSemType)(attr->flag) != SyncSemType_Binary)
             && ((SyncSemType)(attr->flag) != SyncSemType_Counting))  {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        status =  MEM_Calloc ((Void **) semObj, sizeof (SyncSemObject),
                              MEM_DEFAULT) ;

        if (DSP_SUCCEEDED (status)) {
            (*semObj)->semType = (SyncSemType)(attr->flag) ;
            (*semObj)->value = 0u ;
            init_waitqueue_head (&(*semObj)->list) ;
            (*semObj)->signature = SIGN_SYNC ;
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    if ((DSP_FAILED (status)) && (semObj != NULL)) {
        *semObj = NULL ;
    }

    DBC_Ensure (   ((semObj == NULL) && DSP_FAILED (status))
                 || ((*semObj == NULL) && DSP_FAILED (status))
                 || (   ((*semObj)->signature == SIGN_SYNC)
                     && DSP_SUCCEEDED (status))) ;

    TRC_1LEAVE ("SYNC_CreateSEM", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_DeleteSEM
 *
 *  @desc   Deletes the semaphore object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_DeleteSEM (IN SyncSemObject * semObj)
{
    DSP_STATUS   status   = DSP_SOK ;

    TRC_1ENTER ("SYNC_DeleteSEM", semObj) ;

    DBC_Require (semObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (semObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (semObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        semObj->signature = SIGN_NULL ;
        status = FREE_PTR (semObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_DeleteSEM", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_WaitSEM
 *
 *  @desc   This function waits on the semaphore.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_WaitSEM (IN SyncSemObject * semObj, IN Uint32  timeout)
{
    DSP_STATUS        status   = DSP_SOK ;
    Int32             osStatus           ;

    TRC_2ENTER ("SYNC_WaitSEM", semObj, timeout) ;

    DBC_Require (semObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (semObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (semObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        if (timeout == SYNC_NOWAIT) {
            preempt_disable ();
            if (semObj->value == 0u) {
                status = SYNC_E_FAIL ;
                SET_FAILURE_REASON ;
            }
            else {
                if (semObj->semType == SyncSemType_Binary) {
                    semObj->value = 0u ;
                }
                else {
                    semObj->value-- ;
                }
            }
            preempt_enable ();
        }
        else if (timeout != SYNC_WAITFOREVER) { /* Finite timeout */
            DECLARE_WAITQUEUE (wait, current) ;
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&semObj->list, &wait) ;
            /* Set the current task status as interrutible */
            do {
                set_current_state (TASK_INTERRUPTIBLE) ;
                preempt_disable ();
                if (semObj->value != 0) {
                    if (semObj->semType == SyncSemType_Binary) {
                        semObj->value = 0u ;
                        preempt_enable ();
                    }
                    else {
                        semObj->value-- ;
                        preempt_enable ();
                    }
                    break;
                }
                preempt_enable ();
                osStatus = schedule_timeout (msecs_to_jiffies (timeout)) ;
                if (signal_pending(current)) {
                    status = -ERESTARTSYS ;
                    SET_FAILURE_REASON ;
                    break ;
                }
                if (osStatus <= 0) {
                    status = DSP_ETIMEOUT ;
                    SET_FAILURE_REASON ;
                    break ;
                }
            } while (1);
            /* Remove from wait list */
            remove_wait_queue (&semObj->list, &wait) ;
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING) ;
        }
        else {
            DECLARE_WAITQUEUE (wait, current) ;
            /* Add the current process to wait queue */
            add_wait_queue_exclusive (&semObj->list, &wait) ;
            do {
                set_current_state (TASK_INTERRUPTIBLE) ;
                preempt_disable ();
                if (semObj->value != 0) {
                    if (semObj->semType == SyncSemType_Binary) {
                        semObj->value = 0u ;
                        preempt_enable ();
                    }
                    else {
                        semObj->value-- ;
                        preempt_enable ();
                    }
                    break;
                }
                preempt_enable ();
                osStatus = schedule_timeout (MAX_SCHEDULE_TIMEOUT) ;
                if (signal_pending(current)) {
                    status = -ERESTARTSYS ;
                    SET_FAILURE_REASON ;
                    break ;
                }
                if (osStatus <= 0) {
                    status = DSP_ETIMEOUT ;
                    SET_FAILURE_REASON ;
                    break ;
                }
            } while (1);
            /* Remove from wait list */
            remove_wait_queue (&semObj->list, &wait) ;
            /* Set the current task status as running */
            set_current_state (TASK_RUNNING) ;
        } /* Forever */
    }
    else {
        SET_FAILURE_REASON ;
    }

    TRC_1LEAVE ("SYNC_WaitSEM", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_SignalSEM
 *
 *  @desc   This function signals the semaphore and makes it available for other
 *          threads.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_SignalSEM (IN SyncSemObject * semObj)
{
    DSP_STATUS   status    = DSP_SOK   ;

    TRC_1ENTER ("SYNC_SignalSEM", semObj) ;

    DBC_Require (semObj != NULL) ;
    DBC_Require (IS_OBJECT_VALID (semObj, SIGN_SYNC)) ;

    status = (IS_OBJECT_VALID (semObj, SIGN_SYNC) ? DSP_SOK : DSP_EPOINTER) ;

    if (DSP_SUCCEEDED (status)) {
        if (semObj->semType == SyncSemType_Binary) {
            preempt_disable ();
            semObj->value = 1u ;
            preempt_enable () ;
            wake_up_interruptible (&semObj->list) ;
        }
        else {
            preempt_disable ();
            semObj->value++ ;
            preempt_enable () ;
            wake_up_interruptible (&semObj->list) ;
        }
    }

    TRC_1LEAVE ("SYNC_SignalSEM", status) ;

    return status ;
}


/** ============================================================================
 *  @func   SYNC_ProtectionStart
 *
 *  @desc   Marks the start of protected code execution.
 *          DSP/BIOS Link implements DPC using tasklets.
 *          This function acheives protection by disabling DPCs.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
SYNC_ProtectionStart ()
{
    DSP_STATUS status = DSP_SOK ;
    TRC_0ENTER ("SYNC_ProtectionStart") ;

    status = SYNC_EnterCS (SYNC_ProtectCsObj) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    /* Disable all DPC */
    DPC_Disable () ;

    TRC_0LEAVE ("SYNC_ProtectionStart") ;
}


/** ============================================================================
 *  @func   SYNC_ProtectionEnd
 *
 *  @desc   Marks the end of protected code execution.
 *          DSP/BIOS Link implements DPC using tasklets.
 *          This function enables DPCs.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
SYNC_ProtectionEnd ()
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("SYNC_ProtectionEnd") ;

    /* Enable all DPC */
    DPC_Enable () ;

    status = SYNC_LeaveCS (SYNC_ProtectCsObj) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    TRC_0LEAVE ("SYNC_ProtectionEnd") ;
}


/** ============================================================================
 *  @func   SYNC_SpinLockCreate
 *
 *  @desc   Creates the Spinlock Object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Pvoid
SYNC_SpinLockCreate (Void)
{
    DSP_STATUS            status = DSP_SOK ;
    Pvoid                 spinLockHandle = NULL ;
    SYNC_SpinLockObject * obj = NULL ;

    TRC_0ENTER ("SYNC_SpinLockCreate") ;

    status = MEM_Alloc ((Pvoid *) &spinLockHandle,
                        sizeof (SYNC_SpinLockObject),
                        MEM_DEFAULT) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }
    else {
        obj = (SYNC_SpinLockObject *) spinLockHandle ;
        mutex_init (&obj->lock) ;
        obj->irqFlags = 0 ;
    }

    TRC_1LEAVE ("SYNC_SpinLockCreate", spinLockHandle) ;

    return spinLockHandle ;
}


/** ============================================================================
 *  @func   SYNC_SpinLockDelete
 *
 *  @desc   Deletes the Spinlock Object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
SYNC_SpinLockDelete (IN Pvoid spinLockHandle)
{
    TRC_1ENTER ("SYNC_SpinLockDelete", spinLockHandle) ;

    if (spinLockHandle != NULL) {
        FREE_PTR (spinLockHandle) ;
    }

    TRC_0LEAVE ("SYNC_SpinLockDelete") ;
}


/** ============================================================================
 *  @func   SYNC_SpinLockStartEx
 *
 *  @desc   Begin protection of code through spin lock with all ISRs disabled.
 *          Calling this API protects critical regions of code from preemption
 *          by tasks, DPCs and all interrupts.
 *          This API can be called from DPC context.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Uint32
SYNC_SpinLockStartEx (IN     Pvoid spinLockHandle)
{
    SYNC_SpinLockObject * obj      = NULL ;
    Uint32                irqFlags = 0 ;
    DSP_STATUS            status = DSP_SOK ;

    TRC_1ENTER ("SYNC_SpinLockStartEx", spinLockHandle) ;

    if (spinLockHandle != NULL) {
        obj = (SYNC_SpinLockObject *) spinLockHandle ;
        status =  mutex_lock_interruptible (&obj->lock) ;
    }
    else {
        status = mutex_lock_interruptible (&SYNC_PreemptLock) ;
    }

    TRC_1LEAVE ("SYNC_SpinLockStartEx", irqFlags) ;

    return irqFlags ;
}


/** ============================================================================
 *  @func   SYNC_SpinLockEndEx
 *
 *  @desc   End protection of code through spin lock with all ISRs disabled.
 *          This API can be called from DPC context.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
SYNC_SpinLockEndEx (IN     Pvoid spinLockHandle, IN     Uint32 irqFlags)
{
    SYNC_SpinLockObject * obj = NULL ;

    TRC_2ENTER ("SYNC_SpinLockEndEx", spinLockHandle, irqFlags) ;

    if (spinLockHandle != NULL) {
        obj = (SYNC_SpinLockObject *) spinLockHandle ;
        mutex_unlock (&obj->lock) ;
    }
    else {
        mutex_unlock (&SYNC_PreemptLock) ;
    }

    TRC_0LEAVE ("SYNC_SpinLockEndEx") ;
}


/** ============================================================================
 *  @func   SYNC_SpinLockEnd
 *
 *  @desc   Pause for number microseconds.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
SYNC_UDelay (IN Uint32 numUSec)
{
    TRC_1ENTER ("SYNC_USec", numUSec) ;

    udelay (numUSec) ;

    TRC_0LEAVE ("SYNC_UDelay") ;
}



/** ============================================================================
 *  @func   SYNC_SetPriority
 *
 *  @desc   Change the priority of the current  task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS SYNC_SetPriority (IN Uint32 priority)
{
    DSP_STATUS   status   = DSP_ENOTIMPL ;

    TRC_1ENTER ("SYNC_SetPriority", priority) ;

    TRC_1LEAVE ("SYNC_SetPriority", status) ;

    return status;
}


/** ============================================================================
 *  @func   SYNC_GetPriority
 *
 *  @desc   Get the priority of the current task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
SYNC_GetPriority (OUT Uint32 * priority)
{
    DSP_STATUS   status   = DSP_ENOTIMPL ;

    TRC_0ENTER ("SYNC_GetPriority") ;

    TRC_1LEAVE ("SYNC_GetPriority", status) ;
    return status;
}
#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
