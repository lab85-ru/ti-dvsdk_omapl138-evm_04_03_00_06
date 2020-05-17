/** ============================================================================
 *  @file   mpcs.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Implementation of API sub-component MPCS for Multi-Processor
 *          Critical Section.
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


/*  ----------------------------------- C Libraries                 */
#include <string.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Profiling                   */
#if defined (DDSP_PROFILE)
#include <profile.h>
#endif /* #if defined (DDSP_PROFILE) */

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <proc.h>
#include <pool.h>
#include <_mpcs.h>
#include <mpcs.h>
#include <_mpcs_os.h>



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_MPCS

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MPCS, __LINE__)

/** ============================================================================
 *  @name   FREE_OBJECT
 *
 *  @desc   Indicates that the MPCS object needs to be freed.
 *  ============================================================================
 */
#define FREE_OBJECT         1u

/** ============================================================================
 *  @name   NO_FREE_OBJECT
 *
 *  @desc   Indicates that the MPCS object does not need to be freed.
 *  ============================================================================
 */
#define NO_FREE_OBJECT      0u

/** ============================================================================
 *  @name   MPCS_FREE
 *
 *  @desc   Value to indicate that the critical section is free.
 *  ============================================================================
 */
#define MPCS_FREE           0xBADEu

/** ============================================================================
 *  @name   MPCS_BUSY
 *
 *  @desc   Value to indicate that the critical section is busy.
 *  ============================================================================
 */
#define MPCS_BUSY           0xDABAu

/** ============================================================================
 *  @name   SELF
 *
 *  @desc   Value to indicate index number of the current processor. For owner
 *          of MPCS Object, it will be set to 0, otherwise set to 1 for peer.
 *  ============================================================================
 */
#define SELF                0u

/** ============================================================================
 *  @name   OTHER
 *
 *  @desc   Value to indicate index number of the other processor.For owner of
 *          MPCS Object, it will be set to 1, otherwise set to 0 for peer.
 *  ============================================================================
 */
#define OTHER               1u

/** ============================================================================
 *  @name   MPCS_state
 *
 *  @desc   State object for the MPCS component containing all information
 *          required by it.
 *  ============================================================================
 */
extern MPCS_Object MPCS_State [MAX_DSPS] ;


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
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_create (IN     ProcessorId    procId,
             IN     Pstr           name,
             IN     MPCS_ShObj *   mpcsShObj,
             IN     MPCS_Attrs *   attrs)
{
    DSP_STATUS    status    = DSP_SOK ;
    DSP_STATUS    tmpStatus = DSP_SOK ;
    Uint32        slot      = MPCS_INVALID_ID ;
    MPCS_Object * mpcsState = NULL ;
    Uint32        physAddr ;
    Uint32        i ;

    TRC_4ENTER ("MPCS_create", procId, name, mpcsShObj, attrs) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (name  != NULL) ;
    DBC_Require (attrs != NULL) ;

    if (    (IS_VALID_PROCID (procId) == FALSE)
        ||  (name  == NULL)
        ||  (attrs == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;
        if (mpcsState->ctrlPtr == NULL) {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        tmpStatus = MPCS_enter (mpcsState->csObj) ;
        /* mpcsState->csObj is not NULL at this point. */
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Check if the MPCS object to be created is reserved and not to be
         * registered inside the MPCS entries table.
         */
        if (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) == 0) {
            /* User must provide memory for the shared MPCS object */
            DBC_Assert (mpcsShObj != NULL) ;
            if (mpcsShObj == NULL) {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
            else {
                mpcsShObj->gppMpcsObj.freeObject = NO_FREE_OBJECT ;
            }
        }
        else {
            /* Check if the name already exists in the array and get a free
             * slot in the array if the existing matching name is not found.
             */
            for (i = 0 ;
                 (   (i < mpcsState->ctrlPtr->maxEntries)
                  && (DSP_SUCCEEDED (status))) ;
                 i++) {
                if (    strncmp (name,
                                 mpcsState->entryPtr [i].name,
                                 DSP_MAX_STRLEN)
                    ==  0) {
                    status = DSP_EALREADYEXISTS ;
                    SET_FAILURE_REASON ;
                }

                if (    (slot == MPCS_INVALID_ID)
                    &&  (mpcsState->entryPtr [i].physAddress ==  NULL)) {
                    slot = i ;
                }
            }

            /* Existing matching name was not found, but all slots are full. */
            if (DSP_SUCCEEDED (status) && (slot == MPCS_INVALID_ID)) {
                status = DSP_ERESOURCE ;
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                /* Allocate memory for the MPCS object if it was not provided
                 * by the user.
                 */
                if (mpcsShObj == NULL) {
                    status = POOL_alloc (attrs->poolId,
                                         (Pvoid *) &mpcsShObj,
                                         sizeof (MPCS_ShObj)) ;

                    if (DSP_SUCCEEDED (status)) {
                        mpcsShObj->gppMpcsObj.freeObject = FREE_OBJECT ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    /* Client has allocated memory for mpcsShObj and given the
                     * pointer to this function.
                     */
                    mpcsShObj->gppMpcsObj.freeObject = NO_FREE_OBJECT ;
                }

                if (DSP_SUCCEEDED (status)) {
                    /* Translate the user address to physical address. */
                    status = POOL_translateAddr (attrs->poolId,
                                                 (Pvoid *) &physAddr,
                                                 AddrType_Dsp,
                                                 mpcsShObj,
                                                 AddrType_Usr) ;
                    if (DSP_SUCCEEDED (status)) {
                        mpcsState->entryPtr [slot].ownerProcId = ID_GPP ;
                        mpcsState->entryPtr [slot].poolId      = attrs->poolId ;
                        mpcsState->entryPtr [slot].physAddress =
                                                              (Pvoid) physAddr ;
                        strncpy (mpcsState->entryPtr [slot].name,
                                 name,
                                 DSP_MAX_STRLEN) ;
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
            }
        }

        /* Initialize shared MPCS object */
        if (DSP_SUCCEEDED (status)) {
            mpcsShObj->turn                     = SELF  ;
#if defined (DDSP_PROFILE)
            mpcsShObj->gppMpcsObj.conflicts     = 0 ;
            mpcsShObj->gppMpcsObj.numCalls      = 0 ;
            mpcsShObj->dspMpcsObj.conflicts     = 0 ;
            mpcsShObj->dspMpcsObj.numCalls      = 0 ;
#endif
            mpcsShObj->gppMpcsObj.localLock     = 0 ;
            mpcsShObj->dspMpcsObj.localLock     = 0 ;
            mpcsShObj->gppMpcsObj.flag          = (Uint16) MPCS_FREE  ;
            mpcsShObj->dspMpcsObj.flag          = (Uint16) MPCS_FREE  ;
            mpcsShObj->dspMpcsObj.freeObject    = NO_FREE_OBJECT ;

            if (slot == MPCS_INVALID_ID) {
                mpcsShObj->gppMpcsObj.mpcsEntryAddr = MPCS_INVALID_ID ;
            }
            else {
                mpcsShObj->gppMpcsObj.mpcsEntryAddr =  (Uint32) (&mpcsState->entryPtr [slot]) ;
            }

#if defined (GPP_SWI_MODE)
            mpcsShObj->gppMpcsObj.priority      = 0 ;
            mpcsShObj->dspMpcsObj.priority      = 0 ;
#endif

            if (attrs->poolId != POOL_INVALIDID) {
                POOL_writeback (attrs->poolId, mpcsShObj, sizeof (MPCS_ShObj)) ;
            }
        }

        tmpStatus = MPCS_leave (mpcsState->csObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPCS_create", status) ;

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
EXPORT_API
DSP_STATUS
MPCS_delete (IN     ProcessorId procId,
             IN     Pstr        name)
{
    DSP_STATUS    status    = DSP_SOK ;
    DSP_STATUS    tmpStatus = DSP_SOK ;
    MPCS_Object * mpcsState = NULL ;
    MPCS_ShObj  * mpcsShObj ;
    Uint32        i ;
    PoolId        poolId ;

    TRC_2ENTER ("MPCS_delete", procId, name) ;

    DBC_Require (name != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (name == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;
        if (mpcsState->ctrlPtr == NULL)  {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (   DSP_SUCCEEDED (status)
        && (strncmp (name, MPCS_RESV_LOCKNAME, MPCS_RESV_LOCKNAMELEN) != 0)) {
        tmpStatus = MPCS_enter (mpcsState->csObj) ;
        /* mpcsState->csObj is not NULL at this point. */
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Find the MPCS entry corresponding to the specified name. */
        for (i = 0 ; i < mpcsState->ctrlPtr->maxEntries ; i++) {
            /* Check if the entry is valid. */
            if (mpcsState->entryPtr [i].physAddress != NULL) {
                if (    strncmp (name,
                                 mpcsState->entryPtr [i].name,
                                 DSP_MAX_STRLEN)
                    ==  0) {
                    break ;
                }
            }
        }

        if (i == mpcsState->ctrlPtr->maxEntries) {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Get the user address of the MPCS object. */
            if (mpcsState->entryPtr [i].ownerProcId == ID_GPP) {
                poolId =
                    POOL_makePoolId (procId, mpcsState->entryPtr [i].poolId) ;
                status = POOL_translateAddr (poolId,
                                             (Pvoid *) &mpcsShObj,
                                             AddrType_Usr,
                                             mpcsState->entryPtr [i].physAddress,
                                             AddrType_Dsp) ;
                if (DSP_SUCCEEDED (status)) {
                    mpcsShObj->dspMpcsObj.flag = (Uint16) MPCS_FREE ;
                    mpcsShObj->gppMpcsObj.flag = (Uint16) MPCS_FREE ;
                    mpcsShObj->gppMpcsObj.mpcsEntryAddr   = 0;

#if defined (GPP_SWI_MODE)
                    mpcsShObj->gppMpcsObj.priority      = 0 ;
                    mpcsShObj->dspMpcsObj.priority      = 0 ;
#endif


                    /* Free the MPCS object if memory was allocated internally*/
                    if (mpcsShObj->gppMpcsObj.freeObject == FREE_OBJECT) {
                        tmpStatus = POOL_free (poolId,
                                               (Pvoid) mpcsShObj,
                                               sizeof (MPCS_ShObj)) ;
                        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                            status = tmpStatus ;
                            SET_FAILURE_REASON ;
                        }
                    }

                    /* Release the entry in the MPCS region. */
                    memset (&mpcsState->entryPtr [i], 0, sizeof (MPCS_Entry)) ;
                }

            }
            else {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
        }

        tmpStatus = MPCS_leave (mpcsState->csObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPCS_delete", status) ;

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
EXPORT_API
DSP_STATUS
MPCS_open (IN     ProcessorId    procId,
           IN     Pstr           name,
           OUT    MPCS_Handle *  mpcsHandle)
{
    DSP_STATUS    status = DSP_SOK ;
    MPCS_Obj *    mpcsObj   = NULL ;
    MPCS_Object * mpcsState = NULL ;

    TRC_3ENTER ("MPCS_open", procId, name, mpcsHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (name       != NULL) ;
    DBC_Require (mpcsHandle != NULL) ;

    if (    (IS_VALID_PROCID (procId) == FALSE)
        ||  (name       == NULL)
        ||  (mpcsHandle == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;
        if (mpcsState->ctrlPtr == NULL)  {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _MPCS_open (procId, name, mpcsHandle, NULL) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        else {
            /* Assign the procId */
            mpcsObj = (MPCS_Obj *) *mpcsHandle ;
            mpcsObj->procId = procId ;
        }
    }

    TRC_1LEAVE ("MPCS_open", status) ;

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
EXPORT_API
DSP_STATUS
MPCS_close (IN     ProcessorId    procId,
            IN     MPCS_Handle    mpcsHandle)
{
    DSP_STATUS    status    = DSP_SOK ;
    DSP_STATUS    tmpStatus = DSP_SOK ;
    Bool          resvMpcs  = FALSE ;
    MPCS_Obj *    mpcsObj   = (MPCS_Obj *) mpcsHandle ;
    MPCS_Object * mpcsState = NULL ;

    TRC_2ENTER ("MPCS_close", procId, mpcsHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (mpcsHandle != NULL) ;

    if ((IS_VALID_PROCID (procId) == FALSE) || (mpcsHandle == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [procId] ;
        if (mpcsState->ctrlPtr == NULL)  {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Check if the MPCS object to be opened is the reserved one used for
         * protection of MPCS region.
         */
        if (mpcsState->csObj == mpcsObj) {
            resvMpcs = TRUE ;
        }
        else {
            tmpStatus = MPCS_enter (mpcsState->csObj) ;
            /* mpcsState->csObj is not NULL at this point. */
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        }

        status = _MPCS_OS_close (procId, mpcsHandle) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        if (resvMpcs == FALSE) {
            tmpStatus = MPCS_leave (mpcsState->csObj) ;
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("MPCS_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPCS_enter
 *
 *  @desc   Enters the critical section specified by the MPCS object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_enter (IN MPCS_Handle mpcsHandle)
{
    DSP_STATUS    status    = DSP_SOK ;
    MPCS_Obj *    mpcsObj   = (MPCS_Obj *) mpcsHandle ;
    MPCS_Object * mpcsState = NULL ;
#if defined (DDSP_PROFILE)
    Bool          conflictFlag = FALSE ;
#endif
    PoolId        poolId   = 0;

    TRC_1ENTER ("MPCS_enter", mpcsHandle) ;

    DBC_Require (mpcsHandle != NULL) ;

    if (mpcsHandle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [mpcsObj->procId] ;
        if (mpcsState->ctrlPtr == NULL)  {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        status = _MPCS_OS_enter (mpcsObj->procId, mpcsHandle) ;

        if (DSP_SUCCEEDED (status)) {
            /* Indicate, needs to use the resource. */
            mpcsObj->mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_BUSY ;

            if (mpcsObj->mpcsId != MPCS_INVALID_ID) {
                poolId = POOL_makePoolId(mpcsObj->procId ,
                                mpcsState->entryPtr [mpcsObj->mpcsId].poolId) ;
                POOL_writeback (poolId,
                                (Pvoid) &mpcsObj->mpcsObj->gppMpcsObj,
                                sizeof (MPCS_ProcObj)) ;
            }

            /* Give away the turn. */
            mpcsObj->mpcsObj->turn = OTHER ;

            if (mpcsObj->mpcsId != MPCS_INVALID_ID) {
                POOL_writeback (poolId,
                                (Pvoid) &mpcsObj->mpcsObj->turn,
                                sizeof (mpcsObj->mpcsObj->turn)) ;
            }

            /* Wait while other process is using the resource and owns the
             * turn.
             */
            if (mpcsObj->mpcsId != MPCS_INVALID_ID) {
                POOL_invalidate (poolId,
                                 (Pvoid) mpcsObj->mpcsObj,
                                 sizeof (MPCS_ShObj)) ;
            }
            while (     (mpcsObj->mpcsObj->dspMpcsObj.flag == MPCS_BUSY)
                   &&   (mpcsObj->mpcsObj->turn != SELF)) {

                if (mpcsObj->mpcsId != MPCS_INVALID_ID) {
                    POOL_invalidate (poolId,
                                     (Pvoid) mpcsObj->mpcsObj,
                                     sizeof (MPCS_ShObj)) ;
                }
#if defined (DDSP_PROFILE)
                conflictFlag = TRUE ;
#endif
            }

#if defined (DDSP_PROFILE)
            mpcsObj->mpcsObj->gppMpcsObj.numCalls++ ;
            if (conflictFlag == TRUE) {
                mpcsObj->mpcsObj->gppMpcsObj.conflicts++ ;
            }
#endif
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPCS_enter", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPCS_leave
 *
 *  @desc   Leaves the critical section specified by the MPCS object.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_leave (IN MPCS_Handle mpcsHandle)
{
    DSP_STATUS    status  = DSP_SOK ;
    MPCS_Obj *    mpcsObj = (MPCS_Obj *) mpcsHandle ;
    MPCS_Object * mpcsState = NULL ;
    PoolId        poolId ;

    TRC_1ENTER ("MPCS_leave", mpcsHandle) ;

    DBC_Require (mpcsHandle != NULL) ;

    if (mpcsHandle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mpcsState = &MPCS_State [mpcsObj->procId] ;
        if (mpcsState->ctrlPtr == NULL)  {
            /* Check if the MPCS component has been initialized. */
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Check if GPP side is using the resource i.e there has been a
         * corresponding MPCS_enter.
         */
        if (mpcsObj->mpcsObj->gppMpcsObj.flag == (Uint16) MPCS_BUSY) {
            /* Release the resource. */
            mpcsObj->mpcsObj->gppMpcsObj.flag = (Uint16) MPCS_FREE ;

            if (mpcsObj->mpcsId != MPCS_INVALID_ID) {
            poolId = POOL_makePoolId(mpcsObj->procId ,
                                mpcsState->entryPtr [mpcsObj->mpcsId].poolId) ;
                POOL_writeback (poolId,
                                (Pvoid) &mpcsObj->mpcsObj->gppMpcsObj,
                                sizeof (MPCS_ProcObj)) ;
            }

            status = _MPCS_OS_leave  (mpcsObj->procId, mpcsHandle) ;

            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPCS_leave", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
