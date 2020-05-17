/** ============================================================================
 *  @file   mpcs.c
 *
 *  @path   $(DSPLINK)/dsp/src/mpcs/
 *
 *  @desc   Implementation of MPCS-Multi Processor Critical Section.
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <sys.h>
#include <hwi.h>
#include <swi.h>
#include <tsk.h>
#include <gbl.h>
#include <lck.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <_mpcs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define FILEID               FID_MPCS_C

/** ============================================================================
 *  @name   FREE_OBJECT
 *
 *  @desc   Indicates that the MPCS object needs to be freed.
 *  ============================================================================
 */
#define FREE_OBJECT          1

/** ============================================================================
 *  @name   NO_FREE_OBJECT
 *
 *  @desc   Indicates that the MPCS object does not need to be freed.
 *  ============================================================================
 */
#define NO_FREE_OBJECT       0

/** ============================================================================
 *  @name   MPCS_FREE
 *
 *  @desc   Value to indicate that the critical section is free.
 *  ============================================================================
 */
#define MPCS_FREE            0xBADE

/** ============================================================================
 *  @name   MPCS_BUSY
 *
 *  @desc   Value to indicate that the critical section is busy.
 *  ============================================================================
 */
#define MPCS_BUSY            0xDABA

/** ============================================================================
 *  @name   SELF
 *
 *  @desc   Value to indicate index number of the current processor. For owner
 *          of MPCS Object, it will be set to 0, otherwise set to 1 for peer.
 *  ============================================================================
 */
#define SELF                 1

/** ============================================================================
 *  @name   OTHER
 *
 *  @desc   Value to indicate index number of the other processor.For owner of
 *          MPCS Object, it will be set to 1, otherwise set to 0 for peer.
 *  ============================================================================
 */
#define OTHER                0


/** ============================================================================
 *  @name   MPCS_ctrlPtr
 *
 *  @desc   Declaration of the pointer to MPCS Control region.
 *  ============================================================================
 */
extern MPCS_Ctrl *   MPCS_ctrlPtr  ;

/** ============================================================================
 *  @name   MPCS_csObj
 *
 *  @desc   Declaration of the handle to global MPCS protecting the MPCS region.
 *  ============================================================================
 */
extern MPCS_Handle   MPCS_csObj ;

/** ============================================================================
 *  @name   MPCS_HwiKey
 *
 *  @desc   Key for global HWI disable
 *  ============================================================================
 */
static Uint32 MPCS_HwiKey ;

#if defined (DSP_TSK_MODE)
/** ============================================================================
 *  @name   MPCS_GlobalLock
 *
 *  @desc   Global Lock for MPCS critical section protection
 *  ============================================================================
 */
Uint32 MPCS_GlobalLock = NULL;
#endif

/** ============================================================================
 *  @name   MPCS_CONTENTION_POLLCOUNT
 *
 *  @desc   Indicates the poll count for which the while loop
 *          in Peterson will spin before sleep is called.
 *          This sleep allows other tasks to run if deadlock
 *          has happened.
 *  ============================================================================
 */
#define MPCS_CONTENTION_POLLCOUNT       800


/** ============================================================================
 *  @func   MPCS_create
 *
 *  @desc   This function creates and initializes an instance of the MPCS
 *          object.
 *          The memory for the object may or may not be provided by the user.
 *          If provided by the user, the memory for the object must be shared
 *          across the processors using the MPCS. It must also already be mapped
 *          into user space for OSes supporting user/kernel separation.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPCS_create (IN     Uint16         procId,
             IN     Char *         name,
             IN     MPCS_ShObj *   mpcsObj,
             IN     MPCS_Attrs *   attrs)
{
    Int        status    = SYS_OK ;
    Int        tmpStatus = SYS_OK ;
    Uint32     slot      = MPCS_INVALID_ID ;
    Uint32     i ;

    (Void) procId ; /* To remove compiler warning */

    DBC_require (name  != NULL) ;
    DBC_require (attrs != NULL) ;

    if ((name  == NULL) || (attrs == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPCS_ctrlPtr == NULL) {
        /* Check if the MPCS component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        tmpStatus = MPCS_enter (MPCS_csObj) ;
        /* MPCS_csObj is not NULL at this point. */
        DBC_assert (tmpStatus == SYS_OK) ;

        /* Check if the MPCS object to be created is reserved and not to be
         * registered inside the MPCS entries table.
         */
        if (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) == 0) {
            /* User must provide memory for the shared MPCS object */
            DBC_assert (mpcsObj != NULL) ;
            if (mpcsObj == NULL) {
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
            else {
                HAL_cacheInv ((Ptr) mpcsObj, sizeof (MPCS_ShObj)) ;
                mpcsObj->dspMpcsObj.freeObject = NO_FREE_OBJECT ;
            }
        }
        else {
            /* Check if the name already exists in the array and get a free
             * slot in the array if the existing matching name is not found.
             */
            for (i = 0 ;
                 (i < MPCS_ctrlPtr->maxEntries) && (status == SYS_OK) ;
                 i++) {
                HAL_cacheInv ((Ptr) &(MPCS_ctrlPtr->dspAddrEntry [i]),
                              sizeof (MPCS_Entry)) ;
                if (strncmp (name, MPCS_ctrlPtr->dspAddrEntry [i].name,
                            DSP_MAX_STRLEN) == 0) {
                    status = SYS_EBUSY ;
                    SET_FAILURE_REASON (status) ;
                }

                if (    (slot == MPCS_INVALID_ID)
                    &&  (MPCS_ctrlPtr->dspAddrEntry [i].physAddress ==  NULL)) {
                    slot = i ;
                }
            }

            /* Existing matching name was not found, but all slots are full. */
            if ((status == SYS_OK) && (slot == MPCS_INVALID_ID)) {
                status = SYS_ENOTFOUND ;
                SET_FAILURE_REASON (status) ;
            }

            if (status == SYS_OK) {
                /* Allocate memory for the MPCS object if it was not provided
                 * by the user.
                 */
                if (mpcsObj == NULL) {
                    status = POOL_alloc (attrs->poolId,
                                         (Ptr *) &mpcsObj,
                                         sizeof (MPCS_ShObj)) ;

                    if (status == SYS_OK) {
                        mpcsObj->dspMpcsObj.freeObject = FREE_OBJECT ;
                    }
                    else {
                        SET_FAILURE_REASON (status) ;
                    }
                }
                else {
                    /* Client has allocated memory for mpcsObj and given the
                     * pointer to this function.
                     */
                    HAL_cacheInv ((Ptr) mpcsObj, sizeof (MPCS_ShObj)) ;
                    mpcsObj->dspMpcsObj.freeObject = NO_FREE_OBJECT ;
                }

                if (status == SYS_OK) {
                    MPCS_ctrlPtr->dspAddrEntry [slot].ownerProcId =
                                                             GBL_getProcId () ;
                    MPCS_ctrlPtr->dspAddrEntry [slot].poolId = attrs->poolId ;
                    MPCS_ctrlPtr->dspAddrEntry [slot].physAddress = (Ptr)
                                                                       mpcsObj ;
                    strncpy (MPCS_ctrlPtr->dspAddrEntry [slot].name,
                             name,
                             DSP_MAX_STRLEN) ;
                    HAL_cacheWbInv ((Ptr) &(MPCS_ctrlPtr->dspAddrEntry [slot]),
                                    sizeof (MPCS_Entry)) ;
                }
            }
        }

        /* Initialize shared MPCS object */
        if (status == SYS_OK) {
            mpcsObj->turn                     = SELF  ;
#if defined (DDSP_PROFILE)
            mpcsObj->gppMpcsObj.conflicts     = 0 ;
            mpcsObj->gppMpcsObj.numCalls      = 0 ;
            mpcsObj->dspMpcsObj.conflicts     = 0 ;
            mpcsObj->dspMpcsObj.numCalls      = 0 ;
#endif
            mpcsObj->gppMpcsObj.localLock     = 0 ;
            mpcsObj->dspMpcsObj.localLock     = 0 ;
            mpcsObj->gppMpcsObj.flag          = (Uint16) MPCS_FREE  ;
            mpcsObj->dspMpcsObj.flag          = (Uint16) MPCS_FREE  ;
            mpcsObj->gppMpcsObj.freeObject    = NO_FREE_OBJECT ;
#if defined (DSP_SWI_MODE)
            mpcsObj->gppMpcsObj.priority      = 0 ;
            mpcsObj->dspMpcsObj.priority      = 0 ;
#endif /* if defined(DSP_SWI_MODE) */
            if (slot == MPCS_INVALID_ID) {
                mpcsObj->dspMpcsObj.mpcsEntryAddr = MPCS_INVALID_ID ;
            }
            else {
                mpcsObj->dspMpcsObj.mpcsEntryAddr = (Uint32) &(MPCS_ctrlPtr->dspAddrEntry [slot]) ;
            }
#if defined (DSP_TSK_MODE)
            if (MPCS_GlobalLock == NULL) {
                MPCS_GlobalLock = (Uint32) LCK_create (NULL);
            }
            mpcsObj->dspMpcsObj.localLock = MPCS_GlobalLock ;
            if (mpcsObj->dspMpcsObj.localLock == NULL) {
                status = SYS_EALLOC ;
            }
            else {
                mpcsObj->dspMpcsObj.localFlag = MPCS_SEMCREATED_CREATE ;
            }
#endif /* if defined(TSK_MODE) */
        }

        HAL_cacheWbInv ((Ptr) mpcsObj, sizeof (MPCS_ShObj)) ;

        tmpStatus = MPCS_leave (MPCS_csObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_delete
 *
 *  @desc   This function deletes the instance of the MPCS object.
 *
 *  @modif  None
 *  ============================================================================
 */
Int
MPCS_delete (IN     Uint16      procId,
             IN     Char *      name)
{
    Int          status    = SYS_OK ;
    Int          tmpStatus = SYS_OK ;
    MPCS_ShObj * mpcsObj ;
    Uint32       i ;

    DBC_require (name != NULL) ;

    (Void) procId ; /* To remove compiler warning */

    if (name  == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPCS_ctrlPtr == NULL) {
        /* Check if the MPCS component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    /* Check if the MPCS object to be created is reserved and not to be
     * registered inside the MPCS entries table.
     * Nothing is to be done for reserved MPCS object name.
     * In case of RESV the SEM object are not deleted because
     * the mpcs object is provided by user and same object is required in
     * MPCS delete.And no proto type available for same.
     */
    else if (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) != 0) {
        tmpStatus = MPCS_enter (MPCS_csObj) ;
        /* MPCS_csObj is not NULL at this point. */
        DBC_assert (tmpStatus == SYS_OK) ;

        /* Find the MPCS entry corresponding to the specified name. */
        for (i = 0 ; i < MPCS_ctrlPtr->maxEntries ; i++) {
            HAL_cacheInv ((Ptr) &(MPCS_ctrlPtr->dspAddrEntry [i]),
                          sizeof (MPCS_Entry)) ;
            /* Check if the entry is valid. */
            if (MPCS_ctrlPtr->dspAddrEntry [i].physAddress != NULL) {
                if (strncmp (name, MPCS_ctrlPtr->dspAddrEntry [i].name,
                            DSP_MAX_STRLEN) == 0) {
                    break ;
                }
            }
        }

        if (i == MPCS_ctrlPtr->maxEntries) {
            status = SYS_ENOTFOUND ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            mpcsObj = (MPCS_ShObj *)
                               MPCS_ctrlPtr->dspAddrEntry [i].physAddress ;
            HAL_cacheInv ((Ptr) mpcsObj, sizeof (MPCS_ShObj)) ;

            if ( MPCS_ctrlPtr->dspAddrEntry [i].ownerProcId == GBL_getProcId()) {
#if defined (DSP_TSK_MODE)
                if ((mpcsObj->dspMpcsObj.localLock) != NULL) {
                    if (mpcsObj->dspMpcsObj.localFlag == MPCS_SEMCREATED_CREATE) {
                        (mpcsObj->dspMpcsObj.localLock) = NULL ;
                        (mpcsObj->dspMpcsObj.localFlag) = NULL ;
                    }
                }
#endif /* if defined(DSP_TSK_MODE) */
                DBC_assert (mpcsObj->gppMpcsObj.flag != MPCS_BUSY) ;
                mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_FREE ;
                mpcsObj->dspMpcsObj.flag = (Uint16) MPCS_FREE ;
#if defined (DSP_SWI_MODE)
                mpcsObj->gppMpcsObj.priority      = 0 ;
                mpcsObj->dspMpcsObj.priority      = 0 ;
                mpcsObj->dspMpcsObj.mpcsEntryAddr = 0 ;
#endif
                /* Free the MPCS object if memory was allocated internally */
                if (mpcsObj->dspMpcsObj.freeObject == FREE_OBJECT) {
                    POOL_free (MPCS_ctrlPtr->dspAddrEntry [i].poolId,
                               (Ptr) mpcsObj,
                               sizeof (MPCS_ShObj)) ;
                }

                HAL_cacheWbInv ((Ptr) mpcsObj, sizeof (MPCS_ShObj)) ;

                /* Release the entry in the MPCS region. */
                memset (&MPCS_ctrlPtr->dspAddrEntry [i], 0, sizeof (MPCS_Entry)) ;
                HAL_cacheWbInv ((Ptr) &(MPCS_ctrlPtr->dspAddrEntry [i]),
                                sizeof (MPCS_Entry)) ;
            }
            else {
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
        }

        tmpStatus = MPCS_leave (MPCS_csObj) ;
        DBC_assert (status == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_open
 *
 *  @desc   This function opens an MPCS object specified by its name and gets a
 *          handle to the object.
 *          Every process that needs to use the MPCS object must get a handle to
 *          the object by calling this API.
 *
 *  @modif  None
 *  ============================================================================
 */
Int
MPCS_open (IN     Uint16         procId,
           IN     Char *         name,
           OUT    MPCS_Handle *  mpcsHandle)
{
    Int status = SYS_OK ;

    DBC_require (name       != NULL) ;
    DBC_require (mpcsHandle != NULL) ;

    if ((name == NULL) || (mpcsHandle == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPCS_ctrlPtr == NULL) {
        /* Check if the MPCS component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        status = _MPCS_open (procId, name, mpcsHandle, NULL) ;
        if (status != SYS_OK) {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_close
 *
 *  @desc   This function closes an MPCS object specified by its handle.
 *
 *  @modif  None
 *  ============================================================================
 */
Int
MPCS_close (IN     Uint16         procId,
            IN     MPCS_Handle    mpcsHandle)
{
    Int status = SYS_OK ;

    (Void) procId ; /* To avoid compiler warning. */

    DBC_require (mpcsHandle != NULL) ;

    if (mpcsHandle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPCS_ctrlPtr == NULL) {
        /* Check if the MPCS component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        HAL_cacheInv ((Ptr) mpcsHandle, sizeof (MPCS_ShObj)) ;
        mpcsHandle->dspMpcsObj.mpcsEntryAddr = 0;
        HAL_cacheWbInv ((Ptr) &(mpcsHandle->dspMpcsObj),
                        (sizeof (MPCS_ProcObj))) ;

#if defined (DSP_TSK_MODE)
        HAL_cacheInv ((Ptr) mpcsHandle, sizeof (MPCS_ShObj)) ;
        if ((mpcsHandle->dspMpcsObj.localLock) != NULL) {
            if (mpcsHandle->dspMpcsObj.localFlag == MPCS_SEMCREATED_OPEN) {
                (mpcsHandle->dspMpcsObj.localLock) = NULL ;
                (mpcsHandle->dspMpcsObj.localFlag) = NULL ;
                HAL_cacheWbInv ((Ptr) &(mpcsHandle->dspMpcsObj),
                                (sizeof (MPCS_ProcObj) + sizeof(Uint32))) ;
            }
        }
#endif /* if defined(DSP_TSK_MODE) */
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_enter
 *
 *  @desc   This function enters the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPCS_enter (IN     MPCS_Handle mpcsHandle)
{
    Int status = SYS_OK ;
    Uint32     timeout = MPCS_CONTENTION_POLLCOUNT;
    Uint32     sleepcount = 0;

#if defined (DDSP_PROFILE)
    Bool conflictFlag = FALSE ;
#endif

    DBC_require (mpcsHandle != NULL) ;

    if (mpcsHandle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
#if defined (DSP_TSK_MODE)
        if (TSK_isTSK () == TRUE) {
            LCK_pend ((LCK_Handle) (mpcsHandle->dspMpcsObj.localLock),
                                    SYS_FOREVER) ;
        }
#else /* if defined (DSP_TSK_MODE) */
        if (  (TSK_isTSK () == TRUE)
            ||(SWI_isSWI () == TRUE)) {
            SWI_disable () ;
        }
#endif /* if defined (DSP_TSK_MODE) */
        else {
            MPCS_HwiKey = HWI_disable () ;
        }

        /* indicate, needs to use the resource. */
        mpcsHandle->dspMpcsObj.flag = (Uint16) MPCS_BUSY ;
        HAL_cacheWbInv ((Ptr) &(mpcsHandle->dspMpcsObj),
                        sizeof (MPCS_ProcObj)) ;

        /* Give away the turn. */
        mpcsHandle->turn = OTHER ;
        HAL_cacheWbInv ((Ptr) &(mpcsHandle->turn), sizeof (mpcsHandle->turn)) ;

        HAL_cacheInv ((Ptr) mpcsHandle, sizeof (MPCS_ShObj)) ;
        /* Wait while other process is using the resource and has the turn. */
        while (   (mpcsHandle->gppMpcsObj.flag == MPCS_BUSY)
               && (mpcsHandle->turn != SELF)) {
            HAL_cacheInv ((Ptr) mpcsHandle, sizeof (MPCS_ShObj)) ;
#if defined (DDSP_PROFILE)
            conflictFlag = TRUE ;
#endif /* if defined (DDSP_PROFILE) */
            sleepcount++;
            if ((timeout != 0xFFFFFFFFl ) && (sleepcount == timeout)){
#if defined (DSP_TSK_MODE)
                TSK_sleep (1);
#else /* if defined (DSP_TSK_MODE) */
#endif /* if defined (DSP_TSK_MODE) */
                sleepcount = 0;
                timeout = timeout >> 1;
            }
        }

#if defined (DDSP_PROFILE)
        mpcsHandle->dspMpcsObj.numCalls += 1 ;
        if (conflictFlag == TRUE) {
            mpcsHandle->dspMpcsObj.conflicts += 1 ;
        }
        HAL_cacheWbInv ((Ptr) &(mpcsHandle->dspMpcsObj),
                        sizeof (MPCS_ProcObj)) ;
#endif /* if defined (DDSP_PROFILE) */
    }

    return status ;
}


/** ============================================================================
 *  @func   MPCS_leave
 *
 *  @desc   This function leaves the critical section specified by the MPCS
 *          object.
 *
 *  @modif  None.
 *  ============================================================================
 */
Int
MPCS_leave (IN     MPCS_Handle mpcsHandle)
{
    Int status = SYS_OK ;

    DBC_require (mpcsHandle != NULL) ;

    if (mpcsHandle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        /* Check if DSP side is using the resource i.e there has been a
         * corresponding MPCS_enter.
         */
        if (mpcsHandle->dspMpcsObj.flag == (Uint16) MPCS_BUSY) {
            /* Release the resource. */
            mpcsHandle->dspMpcsObj.flag = (Uint16) MPCS_FREE ;

            HAL_cacheWbInv ((Ptr) &(mpcsHandle->dspMpcsObj),
                            sizeof (MPCS_ProcObj)) ;
#if defined (DSP_TSK_MODE)
            if (TSK_isTSK () == TRUE) {
                LCK_post ((LCK_Handle)(mpcsHandle->dspMpcsObj.localLock)) ;
            }
#else /* if defined (DSP_TSK_MODE) */
            if (  (SWI_isSWI () == TRUE)
                ||(TSK_isTSK () == TRUE)) {
                SWI_enable () ;
            }
#endif /* if defined (DSP_TSK_MODE) */
            else {
                HWI_restore (MPCS_HwiKey) ;
            }
        }
        else {
            status = SYS_EINVAL ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
