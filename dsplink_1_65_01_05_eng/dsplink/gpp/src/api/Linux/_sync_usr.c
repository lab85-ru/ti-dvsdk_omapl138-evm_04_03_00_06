/** ============================================================================
 *  @file   _sync_usr.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/Linux/
 *
 *  @desc   Implementation of sub-component SYNC for user-side.
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


/*  ----------------------------------- OS Specific Headers         */
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- User API                    */
#include <_idm_usr.h>
#include <_sync_usr.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */



/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Identifier for sub-component.
 *  ============================================================================
 */
#define COMPONENT_ID        ID_API_SYNC

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_SYNC, __LINE__)


/** ============================================================================
 *  @const  SYNC_KEY_FILE_NAME
 *
 *  @desc   File name to be used for generating the key for SYNC USR component.
 *  ============================================================================
 */
#define SYNC_KEY_FILE_NAME        "/dev/dsplink"

/** ============================================================================
 *  @const  SYNC_KEY_CS
 *
 *  @desc   Value to be used for generating the key for CS component.
 *  ============================================================================
 */
#define SYNC_KEY_CS               'c'

/** ============================================================================
 *  @const  MAX_SYNC_CS
 *
 *  @desc   Maximum number of CS objects that can be created.
 *  ============================================================================
 */
#define MAX_SYNC_CS               200

/** ============================================================================
 *  @const  OS_EFAILURE
 *
 *  @desc   Status code indicating error from the OS calls.
 *  ============================================================================
 */
#define OS_EFAILURE               -1

/** ============================================================================
 *  @const  SEMOP_ALLOC_ONE_RESOURCE
 *
 *  @desc   Indicates that one resource is to be allocated.
 *  ============================================================================
 */
#define SEMOP_ALLOC_ONE_RESOURCE  -1

/** ============================================================================
 *  @const  SEMOP_FREE_ONE_RESOURCE
 *
 *  @desc   Indicates that one resource is to be freed.
 *  ============================================================================
 */
#define SEMOP_FREE_ONE_RESOURCE   1

/** ============================================================================
 *  @const  INIT_RESOURCE_COUNT
 *
 *  @desc   Indicates the initial resource count for the semaphore.
 *  ============================================================================
 */
#define INIT_RESOURCE_COUNT       1

/** ============================================================================
 *  @name   DSPLINK_isSignal
 *
 *  @desc   Global flag to indicate whether execution is in signal context.
 *          Driver on the user-side.
 *  ============================================================================
 */
extern Bool DSPLINK_isSignal ;

/** ============================================================================
 *  @name   SYNC_USR_CsObject_tag
 *
 *  @desc   This object is used by various CS API's.
 *
 *  @field  osSemId
 *              OS-specific ID of the semaphore set to be used.
 *  @field  semId
 *              ID of the semaphore within the semaphore set.
 *  @field  refCount
 *              Reference count for the semaphore to support opening it
 *              multiple times from the same process space from different
 *              threads.
 *  @field entered
 *              Indicated if semaphore state needs to handled specially
 *              in a scenario where DSPLink receives a signal.
 *
 *  @see    None
 *  ============================================================================
 */
struct SYNC_USR_CsObject_tag {
    int    osSemId ;
    Uint32 semId ;
    Uint32 refCount ;
    Bool   entered ;
} ;


/** ============================================================================
 *  @name   SYNC_USR_State
 *
 *  @desc   State object for the SYNC_USR component
 *
 *  @field  isInitialized
 *              Indicates whether the SYNC_USR component is initialized.
 *  @field  csObjs
 *              Array of pointers to SYNC_USR_CsObject objects.
 *  ============================================================================
 */
typedef struct SYNC_USR_State_tag {
    Bool                isInitialized ;
    SYNC_USR_CsObject * csObjs [MAX_SYNC_CS] ;
} SYNC_USR_State ;


/** ============================================================================
 *  @name   SYNC_USR_stateObj
 *
 *  @desc   State object containing all global information for the SYNC_USR
 *          component.
 *  ============================================================================
 */
STATIC SYNC_USR_State SYNC_USR_stateObj =
{
    FALSE
} ;


/** ============================================================================
 *  @func   _SYNC_USR_init
 *
 *  @desc   Initialize SYNC USR sub-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_init (Void)
{
    DSP_STATUS  status = DSP_SOK ;
    key_t       key ;
    int         semId ;
    IDM_Attrs   idmAttrs ;
    Uint32      i ;

    TRC_0ENTER ("_SYNC_USR_init") ;

    for (i = 0 ; i < MAX_SYNC_CS ; i++) {
        SYNC_USR_stateObj.csObjs [i] = NULL ;
    }

    /* Generate the unique key. */
    key = ftok (SYNC_KEY_FILE_NAME, SYNC_KEY_CS);
    if (key == OS_EFAILURE) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Create a semaphore set based on the unique key. */
        semId = semget (key, MAX_SYNC_CS, 0666 | IPC_CREAT) ;
        if (semId == OS_EFAILURE) {
            status = DSP_EFAIL ;
            SET_FAILURE_REASON ;
        }
        else {
            idmAttrs.baseId = 0 ;
            idmAttrs.maxIds = MAX_SYNC_CS ;
            /* Use the component ID as the unique key for the IDM component. */
            status = _IDM_USR_create (COMPONENT_ID, &idmAttrs) ;
            if (DSP_SUCCEEDED (status)) {
                for (i = 0 ; i < MAX_SYNC_CS ; i++) {
                    /* Initialize the semaphores as initially available. */
                    if (    semctl (semId, i, SETVAL, INIT_RESOURCE_COUNT)
                        ==  OS_EFAILURE){
                        status = DSP_EFAIL ;
                        SET_FAILURE_REASON ;
                    }
                }
                SYNC_USR_stateObj.isInitialized = TRUE ;
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    if (DSP_FAILED (status)) {
        _SYNC_USR_exit () ;
    }

    TRC_1LEAVE ("_SYNC_USR_init", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_exit
 *
 *  @desc   Releases resources used by the sub-component.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_exit ()
{
    DSP_STATUS  status = DSP_SOK ;
    key_t       key ;
    int         semId ;
    Uint32      i ;

    TRC_0ENTER ("_SYNC_USR_exit") ;

    status = _IDM_USR_delete (COMPONENT_ID) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
    }

    /* Generate the unique key. */
    key = ftok (SYNC_KEY_FILE_NAME, SYNC_KEY_CS);
    if (key == OS_EFAILURE) {
        if (DSP_SUCCEEDED (status)) {
            status = DSP_EFAIL ;
        }
        SET_FAILURE_REASON ;
    }
    else {
        /* Get the semaphore ID based on the unique key. */
        semId = semget (key, 0, 0) ;
        if (semId == OS_EFAILURE) {
            if (DSP_SUCCEEDED (status)) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            /* Delete the semaphore set. */
            if (    (semctl (semId, 0, IPC_RMID, 0) == OS_EFAILURE)
                &&  DSP_SUCCEEDED (status)) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
        }
    }

    for (i = 0 ; i < MAX_SYNC_CS ; i++) {
        SYNC_USR_stateObj.csObjs [i] = NULL ;
    }

    SYNC_USR_stateObj.isInitialized = FALSE ;

    TRC_1LEAVE ("_SYNC_USR_exit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_createCS
 *
 *  @desc   Creates the Critical section structure.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_createCS (IN Pstr idKey, IN OUT SYNC_USR_CsObject ** csObj)
{
    DSP_STATUS  status      = DSP_SOK ;
    Uint32      semId       = -1 ;
    key_t       key ;
    Uint32      id ;

    TRC_2ENTER ("_SYNC_USR_createCS", idKey, csObj) ;

    DBC_Require (idKey != NULL) ;
    DBC_Require (csObj != NULL) ;

    if (SYNC_USR_stateObj.isInitialized == FALSE) {
        status = DSP_EINIT ;
        SET_FAILURE_REASON ;
    }
    else if ((idKey == NULL) || (csObj == NULL)) {
        status = DSP_EPOINTER ;
        SET_FAILURE_REASON ;
    }
    else {
        /* Get the CS ID corresponding to the specified idKey. */
        status = _IDM_USR_acquireId (COMPONENT_ID, idKey, &id) ;
        if (DSP_SUCCEEDED (status)) {
            /* Generate the unique key. */
            key = ftok (SYNC_KEY_FILE_NAME, SYNC_KEY_CS);
            if (key == OS_EFAILURE) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
            else {
                /* Get the semaphore ID */
                semId = semget (key, 0, 0) ;
                if (semId == OS_EFAILURE) {
                    if (errno == ENOENT) {
                        status = DSP_EINIT ;
                    }
                    else {
                        status = DSP_EFAIL ;
                    }
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Check if the semaphore is already created in this process.*/
                if (SYNC_USR_stateObj.csObjs [id] != NULL) {
                    /* Return handle to existing CS object at specified ID. */
                    *csObj = SYNC_USR_stateObj.csObjs [id] ;
                    (*csObj)->refCount++ ;
                    status = DSP_SEXISTS ;
                }
                else {
                    /* Allocate a new object for this process space. */
                    *csObj =  malloc (sizeof (SYNC_USR_CsObject)) ;
                    if (*csObj == NULL) {
                        status = DSP_EMEMORY ;
                        SET_FAILURE_REASON ;
                    }
                    else {
                        /* Store the allocated semaphore object. */
                        SYNC_USR_stateObj.csObjs [id] = *csObj ;

                        /* Set the fields in the CS object. */
                        (*csObj)->osSemId  = semId ;
                        (*csObj)->semId    = id ;
                        (*csObj)->refCount = 1 ;
                        (*csObj)->entered  = FALSE ;
                    }
                }
            }

            if (DSP_FAILED (status)) {
                _IDM_USR_releaseId (COMPONENT_ID, id) ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    DBC_Ensure (    ((csObj  == NULL) && DSP_FAILED (status))
                 || ((*csObj == NULL) && DSP_FAILED (status))
                 || ((*csObj != NULL) && DSP_SUCCEEDED (status))) ;

    TRC_1LEAVE ("_SYNC_USR_createCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_deleteCS
 *
 *  @desc   Deletes the critical section object.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_deleteCS (IN OUT SYNC_USR_CsObject ** csObj)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("_SYNC_USR_deleteCS", csObj) ;

    DBC_Require (csObj != NULL) ;

    if (csObj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else if (*csObj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        status = _IDM_USR_releaseId (COMPONENT_ID, (*csObj)->semId) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        (*csObj)->refCount-- ;
        if ((*csObj)->refCount == 0) {
            /* Free the semaphore object if the last handle to the semaphore
             * is being closed.
             */
            SYNC_USR_stateObj.csObjs [(*csObj)->semId] = NULL ;
            free (*csObj) ;
            *csObj = NULL ;
        }
    }

    TRC_1LEAVE ("_SYNC_USR_deleteCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_enterCS
 *
 *  @desc   This function enters the critical section that is passed as
 *          argument to it. After successful return of this function no other
 *          process can enter until this process exits the CS by calling
 *          _SYNC_USR_leaveCS.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_enterCS (IN SYNC_USR_CsObject * csObj)
{
    DSP_STATUS    status = DSP_SOK ;
    struct sembuf sb     = {0, SEMOP_ALLOC_ONE_RESOURCE, 0} ;

    TRC_1ENTER ("_SYNC_USR_enterCS", csObj) ;

    DBC_Require (csObj != NULL) ;

    if (DSPLINK_isSignal == FALSE) {
        if (csObj == NULL) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Wait on the semaphore */
            sb.sem_num = csObj->semId ;
            do {
                status = DSP_SOK ;
                if (semop (csObj->osSemId, &sb, 1) == OS_EFAILURE) {
                    if (errno == EINTR) {
                        /* If interrupted by a signal, wait again. */
                        status = DSP_EINTR ;
                    }
                    else {

                        status = DSP_EFAIL ;
                        SET_FAILURE_REASON ;
                    }
                }
            }
            while (status == DSP_EINTR) ;

            csObj->entered = TRUE ;

        }
    }
    else {
        /* Signal is received. Not need to acquire the lock. simply
         * return success
         */
        status = DSP_SOK;
    }

    TRC_1LEAVE ("_SYNC_USR_enterCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_leaveCS
 *
 *  @desc   This function makes critical section available for other processes
 *          to enter.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_leaveCS (IN SYNC_USR_CsObject * csObj)
{
    DSP_STATUS    status = DSP_SOK ;
    struct sembuf sb     = {0, SEMOP_FREE_ONE_RESOURCE, 0}  ;
    Bool       enteredCs = TRUE ;

    TRC_1ENTER ("_SYNC_USR_leaveCS", csObj) ;

    DBC_Require (csObj != NULL) ;

    if (csObj == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (DSPLINK_isSignal == TRUE) {
            if (csObj->entered == FALSE) {
                enteredCs = FALSE ;
            }
        }

        if (enteredCs == TRUE) {
            /* Release the semaphore */
            sb.sem_num = csObj->semId ;
            if (semop (csObj->osSemId, &sb, 1) == OS_EFAILURE) {
                status = DSP_EFAIL ;
                SET_FAILURE_REASON ;
            }
            csObj->entered = FALSE ;
        }
    }

    TRC_1LEAVE ("_SYNC_USR_leaveCS", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_sleep
 *
 *  @desc   This function yields control to other tasks for the defined time
 *          interval. Delay in milliseconds is provided.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_sleep (IN Uint32 delay)
{
    struct timespec tm ;

    if (delay == 0) {
        /* It is undefined whether nanosleep (0) will yield or just return. */
        sched_yield () ;
    }
    else {
        tm.tv_sec  = (delay / 1000) ;
        tm.tv_nsec = ((delay % 1000) * 1000000) ; ;
        nanosleep (&tm, NULL) ;
    }

    return DSP_SOK ;
}


/** ============================================================================
 *  @func   _SYNC_USR_stateObjInit
 *
 *  @desc   Initialize SYNC USR sub-component by allocating all resources.
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
_SYNC_USR_stateObjInit (Void)
{
    DSP_STATUS  status = DSP_SOK ;
    Uint32      i ;

    TRC_0ENTER ("_SYNC_USR_init") ;

    for (i = 0 ; i < MAX_SYNC_CS ; i++) {
        SYNC_USR_stateObj.csObjs [i] = NULL ;
    }
    SYNC_USR_stateObj.isInitialized = TRUE ;

    TRC_1LEAVE ("_SYNC_USR_stateObjInit", status) ;

    return status ;
}


/** ============================================================================
 *  @func   _SYNC_USR_setPriority
 *
 *  @desc   Change the priority of the current  task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS _SYNC_USR_setPriority (IN Uint32 priority)
{
    DSP_STATUS   status   = DSP_ENOTIMPL;

    TRC_1ENTER ("_SYNC_USR_setPriority", priority) ;

    TRC_1LEAVE ("_SYNC_USR_setPriority", status) ;

    return status;
}


/** ============================================================================
 *  @func   _SYNC_USR_getPriority
 *
 *  @desc   Get the priority of the current  task.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS _SYNC_USR_getPriority (OUT Uint32 * curPri)
{
    DSP_STATUS   status   = DSP_ENOTIMPL;
 
	TRC_1ENTER ("_SYNC_USR_getPriority", curPri) ;

	TRC_1LEAVE ("_SYNC_USR_getPriority", status) ;
 
	return status;	
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
