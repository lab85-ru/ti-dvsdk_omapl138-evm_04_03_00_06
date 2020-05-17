/** ============================================================================
 *  @file   dpc.c
 *
 *  @path   $(DSPLINK)/gpp/src/osal/Linux/2.6.18/
 *
 *  @desc   Implementation of sub-component DPC.
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
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/completion.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Generic Functions           */
#include <gen_utils.h>

#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and sub-component Identifier.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_OSAL_DPC

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON  TRC_SetReason (status, FID_C_OSAL_DPC, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */

/** ============================================================================
 *  @const  MAX_DPC
 *
 *  @desc   Maximum number of DPCs supported in DSP/BIOS Link port to mvlinux.
 *  ============================================================================
 */
#define MAX_DPC             32

/** ============================================================================
 *  @const  INVALID_INDEX
 *
 *  @desc   Invalid index of a DPC object.
 *  ============================================================================
 */
#define INVALID_INDEX       0xFFFFFFFF


/** ============================================================================
 *  @name   DpcObject_tag
 *
 *  @desc   This structure defines the association between DpcObjects and their
 *          corresponding tasklets. It also contains a bitmap for tracking used
 *          Dpc objects.
 *
 *  @field  isInit
 *              Initialize flag.
 *  @field  proc
 *              Registered function (DPC entry point).
 *  @field  param
 *              Argument to be passed to DPC entry function.
 *  @field  count
 *              Activation counter.
 *  @field  exit
 *              Indicating it is safe to exit.
 *  @field  enable
 *              Mutex for controlling state of DPC.
 *  @field  list
 *              List where task waits for events.
 *  @field  task
 *              Kernel thread.
 *
 *  @see    None
 *  ============================================================================
 */
struct DpcObject_tag {
    Bool                 isInit ;
    FnDpcProc            proc   ;
    Pvoid                param  ;
    atomic_t             count  ;
    atomic_t             enableFlag;
    struct task_struct * task   ;
} ;


/** ============================================================================
 *  @name   DpcArray
 *
 *  @desc   Subcomponent variable to keep of all the DPCs and associated
 *          hisrs created for 'Link.
 *  ============================================================================
 */
DpcObject DpcArray [MAX_DPC] ;

/** ============================================================================
 *  @name   DPC_IsInitialized
 *
 *  @desc   Flag to keep track of initialization of this subcomponent.
 *  ============================================================================
 */
STATIC Bool DPC_IsInitialized = FALSE ;


/** ============================================================================
 *  @name   DpcLockObj
 *
 *  @desc   Lock for using the DPC resources.
 *  ============================================================================
 */
STATIC Pvoid DpcLockObj = NULL ;


/** ----------------------------------------------------------------------------
 *  @func   DPC_Callback
 *
 *  @desc   This function gets invoked by the OS when it schedules a tasklet
 *          to execute.
 *
 *  @arg    index
 *              Indicates the index in DPC object table.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    DPC_Schedule
 *  ----------------------------------------------------------------------------
 */
STATIC
int
DPC_Callback (void * arg) ;


/** ============================================================================
 *  @func   DPC_Initialize
 *
 *  @desc   This function initializes the DPC module. It initializes the
 *          global area (DPC_DpcWorkQMap structure) for holding all the
 *          DPC objects and marks the UsedDPCs bitmap to indicate that no
 *          DPCs are currently in use.
 *
 *  @modif  DPC_DpcWorkQMap
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Initialize ()
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      i                ;

    TRC_0ENTER ("DPC_Initialize") ;

    if (DPC_IsInitialized == FALSE) {

        /* Create the DPC global lock */
        DpcLockObj = SYNC_SpinLockCreate () ;

        for (i = 0u ; i < MAX_DPC ; i++) {
            memset (&DpcArray [i], 0, sizeof (DpcObject)) ;
            DpcArray [i].isInit = FALSE ;
        }

        DPC_IsInitialized = TRUE ;
    }

    DBC_Ensure (DPC_IsInitialized == TRUE) ;

    TRC_1LEAVE ("DPC_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DPC_Finalize
 *
 *  @desc   This function provides an interface to exit from the DPC module.
 *          It frees up all the used DPCs and releases all the resources used by
 *          this module.
 *
 *  @modif  DPC_DpcWorkQMap
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Finalize ()
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      i      = 0       ;

    TRC_0ENTER ("DPC_Finalize") ;

    DBC_Require (DPC_IsInitialized == TRUE) ;

    if (DPC_IsInitialized == TRUE) {
        SYNC_SpinLockStartEx (DpcLockObj) ;
        for (i = 0u ; i < MAX_DPC ; i++) {
            if (DpcArray [i].isInit == TRUE) {
                /* Finalize if any active DPC found */
                DPC_Delete (&DpcArray [i]) ;
            }
        }
        DPC_IsInitialized = FALSE ;
        SYNC_SpinLockEndEx (DpcLockObj, 0u) ;
        /* Now it is safe to delete the lock */
        SYNC_SpinLockDelete (DpcLockObj) ;
        DpcLockObj = NULL ;
    }

    DBC_Ensure (DPC_IsInitialized == FALSE) ;

    TRC_1LEAVE ("DPC_Finalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DPC_Create
 *
 *  @desc   Creates a DPC object and returns it after populating
 *          relevant fields. It sets a bit in UsedDPCs bitmask.
 *
 *  @modif  DPC_DpcWorkQMap
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Create (IN  FnDpcProc    userDPCFn,
            IN  Pvoid        dpcArgs,
            OUT DpcObject ** dpcObj)
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      index  = 0       ;

    TRC_3ENTER ("DPC_Create", userDPCFn, dpcArgs, dpcObj) ;

    DBC_Require (DPC_IsInitialized == TRUE) ;
    DBC_Require (userDPCFn != NULL) ;
    DBC_Require (dpcObj != NULL) ;

    if ((userDPCFn == NULL) || (dpcObj == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Find a free index */
        SYNC_SpinLockStartEx (DpcLockObj) ;
        for (index = 0 ; index < MAX_DPC ; index++) {
            if (DpcArray [index].isInit == FALSE) {
                break ;
            }
        }

        if (index == MAX_DPC) {
            status = DSP_ERESOURCE ;
            SET_FAILURE_REASON ;
        }
        else {
            memset ((Uint8 *) &DpcArray [index], 0, sizeof (DpcObject)) ;
            /* Create the kernel thread */
            DpcArray [index].task = kthread_create (DPC_Callback,
                                                    (Pvoid) index,
                                                    "DSPLINK_DPC_%d",
                                                    (Int) index) ;
            if (DpcArray [index].task != NULL) {
                DpcArray [index].proc  = userDPCFn ;
                DpcArray [index].param = dpcArgs ;

                /* Initialize the state, count and exit flags */
                atomic_set (&DpcArray [index].count, 0) ;
                atomic_set (&DpcArray [index].enableFlag, 1) ;

                /* Initialize the waiting list */
                DpcArray [index].isInit = TRUE ;
                *dpcObj = &DpcArray [index] ;

                /* Release the global lock */
                SYNC_SpinLockEndEx (DpcLockObj, 0u) ;

                /* Finally start the kernel thread */
                wake_up_process (DpcArray [index].task) ;

                /* Again take the global lock */
                SYNC_SpinLockStartEx (DpcLockObj) ;
            }
            else {
                *dpcObj = NULL ;
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
        SYNC_SpinLockEndEx (DpcLockObj, 0u) ;
    }

    TRC_1LEAVE ("DPC_Create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DPC_Delete
 *
 *  @desc   Destroys the DPC object by resetting the UsedDPCs bitmask.
 *
 *  @modif  DPC_DpcWorkQMap
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Delete (IN DpcObject * dpcObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DPC_Delete", dpcObj) ;

    DBC_Require (DPC_IsInitialized == TRUE) ;
    DBC_Require (dpcObj != NULL) ;

    if (dpcObj->isInit == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Clear the count */
        atomic_set (&dpcObj->count, 0) ;
        atomic_set (&dpcObj->enableFlag, 0) ;
        dpcObj->isInit = FALSE ;

        /* Stop the thread */
        kthread_stop (dpcObj->task) ;
    }

    TRC_1LEAVE ("DPC_Delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DPC_Cancel
 *
 *  @desc   Cancels the DPC.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Cancel (IN DpcObject * dpcObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DPC_Cancel", dpcObj) ;

    DBC_Require (DPC_IsInitialized == TRUE) ;
    DBC_Require (dpcObj != NULL) ;

    if (dpcObj->isInit == FALSE) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Clear the count */
        atomic_set (&dpcObj->count, 0) ;
    }

    TRC_1LEAVE ("DPC_Cancel", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DPC_Schedule
 *
 *  @desc   This function schedules the DPC object, causing the user defined
 *          function associated with it to be invoked at a later point of time.
 *          This function is called from ISR context.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
DPC_Schedule (IN  DpcObject * dpcObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DPC_Schedule", dpcObj) ;

    DBC_Require (DPC_IsInitialized == TRUE) ;
    DBC_Require (dpcObj != NULL) ;

    if (dpcObj->isInit == FALSE) {
       status = DSP_EPOINTER ;
       SET_FAILURE_REASON ;
    }
    else {
        /* If DPC is not exited only then wake up */
        if (atomic_read (&dpcObj->enableFlag) == 1) {
            if (atomic_inc_return (&dpcObj->count) == 1) {
                /* Seen an issue without preempt_disable/preempt_enable */
                preempt_disable ();
                wake_up_process (dpcObj->task) ;
                preempt_enable ();
            }
        }
        else {
            atomic_inc (&dpcObj->count);
        }
    }

    TRC_0LEAVE ("DPC_Schedule") ;

    return DSP_SOK ;
}


/** ============================================================================
 *  @func   DPC_Disable
 *
 *  @desc   Disables execution of DPCs.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
DPC_Disable ()
{
    Int16 i ;

    TRC_0ENTER ("DPC_Disable") ;

    for (i = 0 ; i < MAX_DPC ; i++) {
        if (DpcArray [i].isInit == TRUE) {
            /* Take the enable mutex, so that DPC waits for it */
            atomic_set (&DpcArray [i].enableFlag, 0);
        }
    }

    TRC_0LEAVE ("DPC_Disable") ;
}


/** ============================================================================
 *  @func   DPC_Enable
 *
 *  @desc   Enables execution of DPCs.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
DPC_Enable ()
{
    Int16 i ;

    TRC_0ENTER ("DPC_Enable") ;

    for (i = 0 ; i < MAX_DPC ; i++) {
        if (DpcArray [i].isInit == TRUE) {
            /* release the lock, so that DPC takes it and run */
            atomic_set (&DpcArray [i].enableFlag, 1);
            if (atomic_read (&DpcArray [i].count) > 0) {
                wake_up_process (DpcArray [i].task);
            }
        }
    }

    TRC_0LEAVE ("DPC_Enable") ;
}


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   DPC_Debug
 *
 *  @desc   This function prints the current status of DPC objects in
 *          the 'Link.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Void
DPC_Debug ()
{
    Uint32 i = 0 ;

    TRC_0ENTER ("DPC_Debug") ;

    for (i = 0 ; i < MAX_DPC ; i++) {
        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].isInit : [0x%x]\n",
                    i,
                    DpcArray [i].isInit) ;
        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].proc  : [0x%x]\n",
                    i,
                    DpcArray [i].proc) ;
        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].param : [0x%x]\n",
                    i,
                    DpcArray [i].param) ;

        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].count : [0x%x]\n",
                    i,
                    atomic_read( &(DpcArray [i].count))) ;
        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].enableFlag : [0x%x]\n",
                    i,
                    atomic_read(&(DpcArray [i].enableFlag))) ;

        TRC_2PRINT (TRC_LEVEL1,
                    "DpcArray [%d].task : [0x%x]\n",
                    i,
                    DpcArray [i].task) ;
    }

    TRC_0LEAVE ("DPC_Debug") ;
}
#endif /* defined (DDSP_DEBUG) */

/*  ----------------------------------------------------------------------------
 *  @func   DPC_Callback
 *
 *  @desc   This function gets invoked when a tasklet is scheduled to run
 *          by kernel. It services all the pending DPC requests.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
STATIC
int
DPC_Callback (void * arg)
{
    Uint32 index = (Uint32)arg ;

    TRC_1ENTER ("DPC_Callback", arg) ;

    /* Set a high priority to simulate tasklet */
    set_user_nice (current, -10) ;
    set_current_state (TASK_INTERRUPTIBLE) ;

    for (;!kthread_should_stop ();) {
        set_current_state (TASK_INTERRUPTIBLE) ;
        while (atomic_read (&DpcArray [index].count)) {
            if (atomic_read (&DpcArray [index].enableFlag) == 0) {
                break;
            }

            /* Call the attached function */
            DpcArray [index].proc (DpcArray [index].param) ;
            atomic_dec (&DpcArray [index].count) ;
        }
        schedule() ;

    }

    set_current_state (TASK_RUNNING) ;

    TRC_0LEAVE ("DPC_Callback") ;

    return 0 ;
}




#if defined (__cplusplus)
}
#endif
