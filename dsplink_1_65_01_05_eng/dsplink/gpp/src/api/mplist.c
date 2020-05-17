/** ============================================================================
 *  @file   mplist.c
 *
 *  @path   $(DSPLINK)/gpp/src/api/
 *
 *  @desc   Implementation of API sub-component MPLIST.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <procdefs.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <drvdefs.h>
#include <drv_api.h>

/*  ----------------------------------- User API                    */
#include <mpcs.h>
#include <pool.h>
#include <_mem_usr.h>
#include <mplist.h>
#include <_mplist.h>

/*  ----------------------------------- C Libraries                 */
#include <string.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent ID.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_API_MPLIST

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason
 *  ============================================================================
 */
#define SET_FAILURE_REASON  \
            TRC_3PRINT (TRC_LEVEL7, \
                        "\nFailure: Status:[0x%x] File:[0x%x] Line:[%d]\n", \
                        status, FID_C_API_MPLIST, __LINE__)

/** ============================================================================
 *  @name   MPLIST_State
 *
 *  @desc   State object for the MPLIST omponent containing all information
 *          required by it.
 *  ============================================================================
 */
extern MPLIST_Object MPLIST_State [MAX_DSPS] ;


/** ============================================================================
 *  @func   MPLIST_create
 *
 *  @desc   Allocates and initializes a doubly linked circular list.
 *
 *  @modif  mplistState->ctrlPtr
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_create (IN     ProcessorId    procId,
               IN     Char8 *        name,
               IN     MPLIST_List *  mplistObj,
               IN     MPLIST_Attrs * attrs)
{
    DSP_STATUS      status       = DSP_SOK ;
    DSP_STATUS      tmpStatus    = DSP_SOK ;
    MPLIST_Header * phyListHead  = NULL    ;
    Int32           slot         = -1      ;
    MPLIST_Object * mplistState  = NULL    ;
    Uint32          i ;
    MPCS_Attrs      mpcsAttrs ;

    TRC_4ENTER ("MPLIST_create", procId, name, mplistObj, attrs) ;

    DBC_Require (name  != NULL) ;
    DBC_Require (attrs != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (   (name == NULL)
        || (attrs == NULL)
        || (IS_VALID_PROCID (procId) == FALSE)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistState = &MPLIST_State [procId] ;
        if (mplistState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Enter CS.*/
        status = MPCS_enter (mplistState->mpcsObj) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }

        /* Check if the name already exists in the array and get a free slot
         * in the array if the existing matching name is not found.
         */
        for (i = 0 ;
            (   (i < mplistState->ctrlPtr->maxEntries)
             && DSP_SUCCEEDED (status)) ;
            i++) {
            if (strncmp (name,
                         mplistState->entryPtr [i].name,
                         DSP_MAX_STRLEN) == 0) {
                status = DSP_EALREADYEXISTS ;
                SET_FAILURE_REASON ;
            }

            if (    (slot == -1)
                &&  (    mplistState->entryPtr [i].phyListHandle
                     ==  NULL)) {
                /* Reserve the found slot. */
                mplistState->entryPtr [i].phyListHandle = (Pvoid) 1 ;
                slot = i ;
            }
        }

        /* Existing matching name was not found, but all slots are full. */
        if (DSP_SUCCEEDED (status) && (slot == -1)) {
            status = DSP_ERESOURCE ;
            SET_FAILURE_REASON ;
        }

        /* Leave CS.*/
        tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            /* Allocate memory for the MPLIST object if it was not provided
             * by the user.
             */
            if (mplistObj == NULL) {
                status = POOL_alloc (attrs->poolId,
                                     (Pvoid *) &mplistObj,
                                     sizeof (MPLIST_List)) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Initialize the list */
                status = POOL_translateAddr (attrs->poolId,
                                             (Pvoid *) &(phyListHead),
                                             AddrType_Dsp,
                                             &(mplistObj->head),
                                             AddrType_Usr) ;

                if (DSP_SUCCEEDED (status)) {
                    mplistObj->head.next = phyListHead ;
                    mplistObj->head.prev = phyListHead ;
                    POOL_writeback (attrs->poolId,
                                    mplistObj,
                                    sizeof (MPLIST_Header)) ;
                }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Create the MPCS object. */
                mpcsAttrs.poolId = attrs->poolId ;
                /* Use the MPLIST name as the MPCS name. */
                status = MPCS_create (procId,
                                      name,
                                      &(mplistObj->mpcsObj),
                                      &mpcsAttrs) ;
            }

            if (DSP_SUCCEEDED (status)) {
                /* Update the control structure with the MPLIST details. */
                tmpStatus = MPCS_enter (mplistState->mpcsObj) ;
                DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

                mplistState->entryPtr [slot].phyListHandle = phyListHead ;
                mplistState->entryPtr [slot].ownerProcId   = ID_GPP ;
                mplistState->entryPtr [slot].poolId = attrs->poolId ;

                strncpy (mplistState->entryPtr [slot].name,
                         name,
                         DSP_MAX_STRLEN) ;

                tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
                DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
            }
            else {
                /* Reset the slot to empty so that it can be used later. */
                mplistState->entryPtr [slot].phyListHandle = NULL ;

                /* Free any memory that was successfully allocated. */
                if (mplistObj != NULL) {
                    POOL_free (attrs->poolId,
                               (Pvoid) mplistObj,
                               sizeof (MPLIST_Header));
                }
            }
        }
    }

    TRC_1LEAVE ("MPLIST_create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_delete
 *
 *  @desc   Removes the list entry from the control structure in shared memory
 *          region. Must ONLY be used for empty lists, because it does not
 *          delete each list element.
 *
 *  @modif  mplistState->ctrlPtr
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_delete (IN ProcessorId procId,
               IN Char8 *     name)
{
    DSP_STATUS      status        = DSP_SOK ;
    DSP_STATUS      tmpStatus     = DSP_SOK ;
    MPLIST_List  *  mplist     = NULL    ;
    MPLIST_Entry *  mplistEntry   = NULL    ;
    Pvoid           phyListHandle = NULL    ;
    MPLIST_Object * mplistState  = NULL    ;
    Uint32          i ;

    TRC_2ENTER ("MPLIST_delete", procId, name) ;

    DBC_Require (name != NULL) ;
    DBC_Require (IS_VALID_PROCID (procId)) ;

    if (name == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistState = &MPLIST_State [procId] ;
        if (mplistState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        tmpStatus = MPCS_enter (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Find the MPLIST entry corresponding to the specified name. */
        for (i = 0 ; i < mplistState->ctrlPtr->maxEntries ; i++) {
            /* Check if the entry is valid. */
            if (    (mplistState->entryPtr [i].phyListHandle != NULL)
                &&  (mplistState->entryPtr [i].phyListHandle != (Pvoid) 1)) {
                if (    strncmp (name, mplistState->entryPtr [i].name, DSP_MAX_STRLEN)
                    ==  0) {
                    phyListHandle = mplistState->entryPtr [i].phyListHandle ;
                    mplistState->entryPtr [i].phyListHandle = (Pvoid) 1 ;
                    break ;
                }
            }
        }

        tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        if (i == mplistState->ctrlPtr->maxEntries) {
            status = DSP_ENOTFOUND ;
            SET_FAILURE_REASON ;
        }
        else {
            mplistEntry = &(mplistState->entryPtr [i]) ;
            /* Only the processor which has created the
             * list, can delete it.
             */
            if (mplistEntry->ownerProcId == ID_GPP) {
                /* Delete the created MPCS object. Name of the MPCS object is
                 * the same as the MPLIST name.
                 */
                status = MPCS_delete (mplistState->ctrlPtr->dspId, name) ;
                if (DSP_FAILED (status)) {
                    SET_FAILURE_REASON ;
                }
                /* Get user space address in order to free the list. */
                tmpStatus = POOL_translateAddr (mplistEntry->poolId,
                                                (Pvoid *) &mplist,
                                                AddrType_Usr,
                                                phyListHandle,
                                                AddrType_Dsp) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }

                if (mplist != NULL) {
                    /* Free the MPLIST. */
                    tmpStatus = POOL_free (mplistEntry->poolId,
                                           (Pvoid) mplist,
                                           sizeof (MPLIST_List)) ;
                    if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                        status = tmpStatus ;
                        SET_FAILURE_REASON ;
                    }
                }

                tmpStatus = MPCS_enter (mplistState->mpcsObj) ;
                DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
                /* Clear the memory region. */
                memset (&mplistState->entryPtr [i], 0, sizeof (MPLIST_Entry)) ;
                tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
                DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
                if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                status = DSP_EINVALIDARG ;
                SET_FAILURE_REASON ;
            }
        }
    }

    TRC_1LEAVE ("MPLIST_delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_open
 *
 *  @desc   This function opens an MPLIST object specified by its name and gets
 *          a handle to the object.
 *          Every process that needs to use the MPLIST object must get a handle
 *          to the object by calling this API.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_open (IN  ProcessorId     procId,
             IN  Char8 *         name,
             OUT MPLIST_Handle * mplistHandle)
{
    DSP_STATUS      status          = DSP_SOK ;
    DSP_STATUS      tmpStatus       = DSP_SOK ;
    MPLIST_List *   virtListHandle  = NULL    ;
    MPCS_Handle     virtLockHandle  = NULL    ;
    MPLIST_Entry *  virtEntryHandle = NULL    ;
    MPLIST_Object * mplistState     = NULL    ;
    MPLIST_Obj *    mplistObj       = NULL    ;
    Uint32         i ;

    TRC_3ENTER ("MPLIST_open", procId, name, mplistHandle) ;

    DBC_Require (IS_VALID_PROCID (procId)) ;
    DBC_Require (name         != NULL) ;
    DBC_Require (mplistHandle != NULL) ;

    /* get the mplist state object */
    mplistState = &MPLIST_State [procId] ;

    if (   (IS_VALID_PROCID (procId) == FALSE)
        || (name            == NULL)
        || (mplistHandle    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistState = &MPLIST_State [procId] ;
        if (mplistState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        /* Prime the variable. */
        *mplistHandle = NULL ;

        tmpStatus = MPCS_enter (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Find the MPLIST entry corresponding to the specified name. */
        for (i = 0 ; i < mplistState->ctrlPtr->maxEntries ; i++) {
            /* Check if the entry is valid. */
            if (    (mplistState->entryPtr [i].phyListHandle != NULL)
                &&  (mplistState->entryPtr [i].phyListHandle != (Pvoid) 1)) {
                if (    strncmp (name,
                                 mplistState->entryPtr [i].name,
                                 DSP_MAX_STRLEN)
                    ==  0) {
                    break ;
                }
            }
        }

        if (i == mplistState->ctrlPtr->maxEntries) {
            status = DSP_ENOTFOUND ;
            tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
            SET_FAILURE_REASON ;
        }
        else {
            /* Get the user space address of the MPLIST object. */
            status = POOL_translateAddr(mplistState->entryPtr [i].poolId,
                                        (Pvoid *) &virtListHandle,
                                        AddrType_Usr,
                                        mplistState->entryPtr [i].phyListHandle,
                                        AddrType_Dsp) ;
            if (DSP_FAILED (status)) {
                SET_FAILURE_REASON ;
            }
            else {
                 POOL_invalidate (mplistState->entryPtr [i].poolId,
                                  virtListHandle,
                                  sizeof (MPLIST_Header)) ;
            }

            /* Get the user address of the MPLIST shared region entry. */
            virtEntryHandle = &(mplistState->entryPtr [i]) ;

            tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
            DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }

            if (DSP_SUCCEEDED (status)) {
                 /* Open the MPCS object to get the user space address of
                  * lock.
                  */
                 status = MPCS_open (mplistState->ctrlPtr->dspId,
                                     name,
                                     &virtLockHandle) ;
                 if (DSP_FAILED (status)) {
                     SET_FAILURE_REASON ;
                 }
            }

            if (DSP_SUCCEEDED (status)) {
                /* Allocate memory for the handle */
                status = _MEM_USR_calloc ((Void **) mplistHandle,
                                          sizeof (MPLIST_Obj)) ;
                if (DSP_SUCCEEDED (status)) {
                    mplistObj = (MPLIST_Obj *) (*mplistHandle) ;
                    mplistObj->procId         = procId ;
                    mplistObj->virtListHandle = virtListHandle ;
                    mplistObj->virtLockHandle = virtLockHandle ;
                    mplistObj->virtEntryHandle = virtEntryHandle ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
        }
    }

    TRC_1LEAVE ("MPLIST_open", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_close
 *
 *  @desc   This function deletes the memory allcoated for the MPLIST handle and
 *          closes an MPLIST instance.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_close (IN MPLIST_Handle mplistHandle)
{
    DSP_STATUS      status         = DSP_SOK ;
    DSP_STATUS      tmpStatus      = DSP_SOK ;
    Pvoid           virtLockHandle = NULL    ;
    MPLIST_Object * mplistState    = NULL    ;
    MPLIST_Obj *    mplistObj      = NULL    ;

    TRC_1ENTER ("MPLIST_close", mplistHandle) ;

    DBC_Require (mplistHandle != NULL) ;

    if (mplistHandle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        mplistState = &MPLIST_State [mplistObj->procId] ;
        if (mplistState->ctrlPtr == NULL) {
            status = DSP_EACCESSDENIED ;
            SET_FAILURE_REASON ;
        }
    }

    if (DSP_SUCCEEDED (status)) {
        tmpStatus = MPCS_enter (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;

        /* Clear the handle entries */
        mplistObj->virtListHandle = NULL ;
        mplistObj->virtEntryHandle = NULL ;

        /* Store value of virtLockHandle in a local variable
         * to call MPCS_close outside lock.
         */
        virtLockHandle = mplistObj->virtLockHandle ;
        mplistObj->virtLockHandle = NULL ;

        tmpStatus = MPCS_leave (mplistState->mpcsObj) ;
        DBC_Assert (DSP_SUCCEEDED (tmpStatus)) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        tmpStatus = MPCS_close (mplistState->ctrlPtr->dspId,
                                (MPCS_Handle) virtLockHandle) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }

        /* Free the user MPLIST handle. */
        tmpStatus = _MEM_USR_free (&mplistHandle) ;
        if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
            status = tmpStatus ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_close", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_isEmpty
 *
 *  @desc   Check for an empty list. This is not protected by a critical section
 *          because its usage in all internal functions is within a critical
 *          section call.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
Bool
MPLIST_isEmpty (IN  MPLIST_Handle mplistHandle)
{
    MPLIST_Entry * entry       = NULL ;
    MPLIST_List *  mplist   = NULL ;
    MPLIST_Elem    phyListHead = NULL ;
    Bool           retVal      = FALSE ;
    DSP_STATUS     status      = DSP_SOK ;

    TRC_1ENTER ("MPLIST_isEmpty", mplistHandle) ;

    DBC_Require (mplistHandle != NULL) ;

    if (mplistHandle == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        entry = (MPLIST_Entry *)
                               (((MPLIST_Obj *)mplistHandle)->virtEntryHandle) ;
        mplist = (MPLIST_List *)
                                (((MPLIST_Obj *)mplistHandle)->virtListHandle) ;

        status = POOL_translateAddr (entry->poolId,
                                     (Pvoid *) &(phyListHead),
                                     AddrType_Dsp,
                                     mplist,
                                     AddrType_Usr) ;

        /* If the element' next points to head itself, the list is empty. */
        if (DSP_SUCCEEDED (status)) {
            POOL_invalidate (entry->poolId, mplist, sizeof (MPLIST_Header)) ;
            if (((Uint32) (mplist->head.next)) == (Uint32) phyListHead) {
                retVal = TRUE ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_isEmpty", retVal) ;

    return retVal ;
}


/** ============================================================================
 *  @func   MPLIST_insertBefore
 *
 *  @desc   Inserts the element before the existing element.
 *
 *  @modif  mplistState->ctrlPtr
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_insertBefore (IN  MPLIST_Handle mplistHandle,
                     IN  MPLIST_Elem   insertElement,
                     IN  MPLIST_Elem   existingElement)
{
    DSP_STATUS     status             = DSP_SOK ;
    DSP_STATUS     tmpStatus          = DSP_SOK ;
    MPLIST_Elem    virtPrevElement    = NULL ;
    MPLIST_Elem    phyExistingElement = NULL ;
    MPLIST_Elem    phyInsertElement   = NULL ;
    MPLIST_Obj *   mplistObj          = NULL ;
    MPCS_Handle    mpcsObj ;
    PoolId         insertElementPoolId ;
    PoolId         existingElementPoolId ;
    PoolId         prevElementPoolId ;

    TRC_3ENTER ("MPLIST_insertBefore",
                mplistHandle,
                insertElement,
                existingElement) ;

    DBC_Require (mplistHandle    != NULL) ;
    DBC_Require (insertElement   != NULL) ;
    DBC_Require (existingElement != NULL) ;

    if (   (mplistHandle    == NULL)
        || (insertElement   == NULL)
        || (existingElement == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        mpcsObj = (MPCS_Handle) mplistObj->virtLockHandle ;
        /* Enter list specific CS. */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            /* Set pool id of element to be added. */
            status = POOL_getPoolId (mplistObj->procId,
                                     insertElement,
                                     AddrType_Usr,
                                     &insertElementPoolId) ;
            if (DSP_SUCCEEDED (status)) {
                /* Get existing element pool id. */
                status = POOL_getPoolId (mplistObj->procId,
                                         existingElement,
                                         AddrType_Usr,
                                         &existingElementPoolId) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                POOL_invalidate (existingElementPoolId,
                                 existingElement,
                                 sizeof (MPLIST_Header)) ;

                /* Get prev element pointer. */
                status = POOL_getPoolId (mplistObj->procId,
                                         existingElement->prev,
                                         AddrType_Dsp,
                                         &prevElementPoolId) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                status = POOL_translateAddr (prevElementPoolId,
                                             (Pvoid *) &(virtPrevElement),
                                             AddrType_Usr,
                                             existingElement->prev,
                                             AddrType_Dsp) ;

                if (DSP_SUCCEEDED (status)) {
                    status = POOL_translateAddr (insertElementPoolId,
                                                 (Pvoid *) &(phyInsertElement),
                                                 AddrType_Dsp,
                                                 insertElement,
                                                 AddrType_Usr) ;
                    if (DSP_SUCCEEDED (status)) {
                        status = POOL_translateAddr
                                      (existingElementPoolId,
                                      (Pvoid *) &(phyExistingElement),
                                      AddrType_Dsp,
                                      existingElement,
                                      AddrType_Usr) ;
                        if (DSP_SUCCEEDED (status)) {
                            /* Add the new element after existing element. */
                            POOL_invalidate (prevElementPoolId,
                                             virtPrevElement,
                                             sizeof (MPLIST_Header)) ;
                            virtPrevElement->next =
                                          (MPLIST_Elem) phyInsertElement ;
                            POOL_writeback (prevElementPoolId,
                                            virtPrevElement,
                                            sizeof (MPLIST_Header)) ;

                            /* Insert element before existing element. */
                            insertElement->prev   =
                                         (MPLIST_Elem) existingElement->prev ;
                            insertElement->next   =
                                         (MPLIST_Elem) phyExistingElement ;
                            POOL_writeback (insertElementPoolId,
                                            insertElement,
                                            sizeof (MPLIST_Header)) ;

                            /* Reset existing element' pointers. */
                            existingElement->prev =
                                         (MPLIST_Elem) phyInsertElement ;
                            POOL_writeback (existingElementPoolId,
                                            existingElement,
                                            sizeof (MPLIST_Header)) ;
                        }
                        else {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
            /* Leave list specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_insertBefore", status) ;
    return status ;
}


/** ============================================================================
 *  @func   MPLIST_putTail
 *
 *  @desc   Adds the specified element to the tail of the list.
 *
 *  @modif  list
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_putTail (IN MPLIST_Handle mplistHandle, IN MPLIST_Elem element)
{
    DSP_STATUS     status          = DSP_SOK ;
    DSP_STATUS     tmpStatus       = DSP_SOK ;
    MPLIST_Elem    virtPrevElement = NULL ;
    MPLIST_Elem    phyElement      = NULL ;
    MPLIST_Elem    phyListHead     = NULL ;
    MPLIST_Obj *   mplistObj       = NULL ;
    MPLIST_Entry * entry ;
    MPLIST_List *  mplist ;
    MPCS_Handle    mpcsObj ;
    PoolId         elementPoolId ;
    PoolId         prevElementPoolId ;

    TRC_2ENTER ("MPLIST_putTail", mplistHandle, element) ;

    DBC_Require (mplistHandle != NULL) ;
    DBC_Require (element      != NULL) ;

    if (   (mplistHandle == NULL)
        || (element      == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        entry     = (MPLIST_Entry *) mplistObj->virtEntryHandle ;
        mplist    = (MPLIST_List *) mplistObj->virtListHandle ;
        mpcsObj   = (MPCS_Handle) mplistObj->virtLockHandle ;

        /* Enter list specific CS. */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            /* Get the pool id of the inserted element. */
            status = POOL_getPoolId (mplistObj->procId,
                                     element,
                                     AddrType_Usr,
                                     &elementPoolId) ;
            if (DSP_SUCCEEDED (status)) {
                status = POOL_translateAddr (elementPoolId,
                                             (Pvoid *) &(phyElement),
                                             AddrType_Dsp,
                                             element,
                                             AddrType_Usr) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Get list head. */
                    status = POOL_translateAddr (entry->poolId,
                                                 (Pvoid *) &(phyListHead),
                                                 AddrType_Dsp,
                                                 mplist,
                                                 AddrType_Usr) ;
                    if (DSP_SUCCEEDED (status)) {
                        POOL_invalidate (entry->poolId,
                                         mplist,
                                         sizeof (MPLIST_Header)) ;

                        /* Add element to tail of the list */
                        element->prev = (MPLIST_Elem) mplist->head.prev ;
                        element->next = (MPLIST_Elem) phyListHead ;
                        status = POOL_writeback (elementPoolId,
                                                 element,
                                                 sizeof (MPLIST_Header)) ;

                        mplist->head.prev = (MPLIST_Elem) phyElement ;
                        POOL_writeback (entry->poolId,
                                        mplist,
                                        sizeof (MPLIST_Header)) ;

                        /* Reset prev pointer */
                        status = POOL_getPoolId(mplistObj->procId,
                                                element->prev,
                                                AddrType_Dsp,
                                                &prevElementPoolId) ;
                        DBC_Assert (DSP_SUCCEEDED (status)) ;
                        status = POOL_translateAddr
                                          (prevElementPoolId,
                                           (Pvoid *) &(virtPrevElement),
                                           AddrType_Usr,
                                           element->prev,
                                           AddrType_Dsp) ;

                        if (DSP_SUCCEEDED (status)) {
                            POOL_invalidate (prevElementPoolId,
                                             virtPrevElement,
                                             sizeof (MPLIST_Header)) ;
                            virtPrevElement->next = phyElement ;
                            POOL_writeback (prevElementPoolId,
                                            virtPrevElement,
                                            sizeof (MPLIST_Header)) ;
                        }
                        else {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
            /* Leave list specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_putTail", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_removeElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_removeElement (IN MPLIST_Handle mplistHandle,
                      IN MPLIST_Elem   element)
{
    DSP_STATUS     status          = DSP_SOK ;
    DSP_STATUS     tmpStatus       = DSP_SOK ;
    MPLIST_Elem    virtPrevElement = NULL ;
    MPLIST_Elem    virtNextElement = NULL ;
    MPLIST_Obj *   mplistObj       = NULL ;
    MPLIST_Entry * entry ;
    MPCS_Handle    mpcsObj ;
    PoolId         elementPoolId ;
    PoolId         prevElementPoolId ;
    PoolId         nextElementPoolId ;

    TRC_2ENTER ("MPLIST_removeElement", mplistHandle, element) ;

    DBC_Require (mplistHandle != NULL) ;
    DBC_Require (element      != NULL) ;

    if (   (mplistHandle == NULL)
        || (element    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        entry     = (MPLIST_Entry *) mplistObj->virtEntryHandle ;
        mpcsObj   = (MPCS_Handle) mplistObj->virtLockHandle ;

        /* Enter list specific CS */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            POOL_invalidate (entry->poolId,
                             element,
                             sizeof (MPLIST_Header)) ;

            /* Get user space address of element'prev */
            status = POOL_getPoolId (mplistObj->procId,
                                     element->prev,
                                     AddrType_Dsp,
                                     &prevElementPoolId) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            status = POOL_translateAddr (prevElementPoolId,
                                         (Pvoid *) &(virtPrevElement),
                                         AddrType_Usr,
                                         element->prev,
                                         AddrType_Dsp) ;

            if (DSP_SUCCEEDED (status)) {
                status = POOL_getPoolId (mplistObj->procId,
                                         element->next,
                                         AddrType_Dsp,
                                         &nextElementPoolId) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                status = POOL_translateAddr (nextElementPoolId,
                                             (Pvoid *) &(virtNextElement),
                                             AddrType_Usr,
                                             element->next,
                                             AddrType_Dsp) ;

                if (DSP_SUCCEEDED (status)) {
                    /* Re-assign element' prev pointer */
                    POOL_invalidate (prevElementPoolId,
                                     virtPrevElement,
                                     sizeof (MPLIST_Header)) ;
                    virtPrevElement->next = (MPLIST_Elem) element->next ;
                    POOL_writeback (prevElementPoolId,
                                    virtPrevElement,
                                    sizeof (MPLIST_Header)) ;

                    /* Re-assign element' next pointer */
                    POOL_invalidate (nextElementPoolId,
                                     virtNextElement,
                                     sizeof (MPLIST_Header)) ;
                    virtNextElement->prev = (MPLIST_Elem) element->prev ;
                    POOL_writeback (nextElementPoolId,
                                    virtNextElement,
                                    sizeof (MPLIST_Header)) ;

                    /* Finalize the element */
                    element->next = NULL ;
                    element->prev = NULL ;

                    /* Get the pool id of the element. */
                    status = POOL_getPoolId (mplistObj->procId,
                                             element,
                                             AddrType_Usr,
                                             &elementPoolId) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    POOL_writeback (elementPoolId,
                                    element,
                                    sizeof (MPLIST_Header)) ;
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Leave list specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_removeElement", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_first
 *
 *  @desc   Returns a pointer to the first element of the list, or NULL if
 *          the list is empty.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_first (IN  MPLIST_Handle mplistHandle,
              OUT MPLIST_Elem * element)
{
    DSP_STATUS     status       = DSP_SOK ;
    DSP_STATUS     tmpStatus    = DSP_SOK ;
    MPLIST_Elem    virtListHead = NULL ;
    MPLIST_Obj *   mplistObj    = NULL ;
    MPLIST_Entry * entry ;
    MPLIST_List *  mplist ;
    MPCS_Handle    mpcsObj ;
    PoolId         poolId ;

    TRC_2ENTER ("MPLIST_first", mplistHandle, element) ;

    DBC_Require (mplistHandle  != NULL) ;
    DBC_Require (element       != NULL) ;

    if (   (mplistHandle  == NULL)
        || (element       == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj  = (MPLIST_Obj *) mplistHandle ;
        entry      = (MPLIST_Entry *) mplistObj->virtEntryHandle ;
        mplist     = (MPLIST_List *) mplistObj->virtListHandle ;
        mpcsObj    = (MPCS_Handle) mplistObj->virtLockHandle ;

        /* Enter list specific CS */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            POOL_invalidate (entry->poolId,
                             mplist,
                             sizeof (MPLIST_Header)) ;

            /* Get user space address for head->next */
            status = POOL_getPoolId (mplistObj->procId,
                                     mplist->head.next,
                                     AddrType_Dsp,
                                     &poolId) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            status = POOL_translateAddr (poolId,
                                         (Pvoid *) &(virtListHead),
                                         AddrType_Usr,
                                         mplist->head.next,
                                         AddrType_Dsp) ;
            if (DSP_SUCCEEDED (status)) {
                /* Prime the variable */
                *element = NULL ;

                if (!MPLIST_isEmpty (mplistHandle)) {
                    POOL_invalidate (poolId,
                                     virtListHead,
                                     sizeof (MPLIST_Header)) ;
                    /* Assign the element to list head' next */
                    *element = (MPLIST_Elem) virtListHead ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Leave list specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }
        else
        {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_first", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_next
 *
 *  @desc   Returns a pointer to the next element of the list, or NULL if
 *          the next element is the head of the list or the list is empty.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_next (IN  MPLIST_Handle mplistHandle,
             IN  MPLIST_Elem   currentElement,
             OUT MPLIST_Elem * nextElement)
{
    DSP_STATUS     status          = DSP_SOK ;
    DSP_STATUS     tmpStatus       = DSP_SOK ;
    MPLIST_Elem    virtNextElement = NULL ;
    MPLIST_Elem    phyListHead     = NULL ;
    MPLIST_Obj *   mplistObj       = NULL ;
    MPLIST_Entry * entry ;
    MPLIST_List *  mplist ;
    MPCS_Handle    mpcsObj ;
    PoolId         currentElementPoolId ;
    PoolId         nextElementPoolId ;

    TRC_3ENTER ("MPLIST_next", mplistHandle, currentElement, nextElement) ;

    DBC_Require (mplistHandle   != NULL) ;
    DBC_Require (currentElement != NULL) ;
    DBC_Require (nextElement    != NULL) ;

    if (   (mplistHandle   == NULL)
        || (currentElement == NULL)
        || (nextElement    == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        entry     = (MPLIST_Entry *) mplistObj->virtEntryHandle ;
        mplist    = (MPLIST_List *) mplistObj->virtListHandle ;
        mpcsObj   = (MPCS_Handle) mplistObj->virtLockHandle ;

        /* Enter list specific CS */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            /* Get physical address of list head */
            status = POOL_translateAddr (entry->poolId,
                                         (Pvoid *) &(phyListHead),
                                         AddrType_Dsp,
                                         mplist,
                                         AddrType_Usr) ;
            if (DSP_SUCCEEDED (status)) {
                /* Get pool id of current element */
                status = POOL_getPoolId (mplistObj->procId,
                                         currentElement,
                                         AddrType_Usr,
                                         &currentElementPoolId) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                POOL_invalidate (currentElementPoolId,
                                 currentElement,
                                 sizeof (MPLIST_Header)) ;

                /* Get user address of current element 'next */
                status = POOL_getPoolId (mplistObj->procId,
                                         currentElement->next,
                                         AddrType_Dsp,
                                         &nextElementPoolId) ;
                DBC_Assert (DSP_SUCCEEDED (status)) ;
                status = POOL_translateAddr (nextElementPoolId,
                                             (Pvoid *) &(virtNextElement),
                                             AddrType_Usr,
                                             currentElement->next,
                                             AddrType_Dsp) ;
                if (DSP_SUCCEEDED (status)) {
                    /* Prime the variable */
                    *nextElement = NULL ;

                    if (!MPLIST_isEmpty (mplistHandle)) {
                        /* Check if it is last element.*/
                        if (currentElement->next != phyListHead) {
                            /* Assign the next element */
                            POOL_invalidate (nextElementPoolId,
                                             virtNextElement,
                                             sizeof (MPLIST_Header)) ;
                            *nextElement = (MPLIST_Elem) virtNextElement ;
                        }
                    }
                }
                else {
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Leave list-specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_FAILED (tmpStatus)) {
                if (DSP_SUCCEEDED (status)) {
                    status = tmpStatus ;
                }
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_next", status) ;

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_getHead
 *
 *  @desc   Gets the head pointer from the list.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_getHead (IN  MPLIST_Handle  mplistHandle,
                OUT MPLIST_Elem *  element)
{
    DSP_STATUS     status           = DSP_SOK ;
    DSP_STATUS     tmpStatus        = DSP_SOK ;
    MPLIST_Elem    phyListHead      = NULL ;
    MPLIST_Elem    virtListHeadNext = NULL ;
    MPLIST_Elem    virtNextElement  = NULL ;
    MPLIST_Obj *   mplistObj        = NULL ;
    MPLIST_Entry * entry ;
    MPLIST_List *  mplist ;
    MPCS_Handle    mpcsObj ;
    PoolId         poolId ;

    TRC_2ENTER ("MPLIST_getHead", mplistHandle, element) ;

    DBC_Require (mplistHandle  != NULL) ;
    DBC_Require (element       != NULL) ;

    if (   (mplistHandle  == NULL)
        || (element       == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        mplistObj = (MPLIST_Obj *) mplistHandle ;
        entry     = (MPLIST_Entry *) mplistObj->virtEntryHandle ;
        mplist    = (MPLIST_List *) mplistObj->virtListHandle ;
        mpcsObj   = (MPCS_Handle) mplistObj->virtLockHandle ;

        /* Enter list specific CS */
        status = MPCS_enter (mpcsObj) ;
        if (DSP_SUCCEEDED (status)) {
            POOL_invalidate (entry->poolId,
                             mplist,
                             sizeof (MPLIST_Header)) ;

            /* Get user space address for head->next */
            status = POOL_getPoolId (mplistObj->procId,
                                     mplist->head.next,
                                     AddrType_Dsp,
                                     &poolId) ;
            DBC_Assert (DSP_SUCCEEDED (status)) ;
            status = POOL_translateAddr (poolId,
                                         (Pvoid *) &(virtListHeadNext),
                                         AddrType_Usr,
                                         mplist->head.next,
                                         AddrType_Dsp) ;
            if (DSP_SUCCEEDED (status)) {
                /* Prime the element */
                *element = NULL ;

                /* Check if list is empty */
                if (!MPLIST_isEmpty (mplistHandle)) {
                    POOL_invalidate (poolId,
                                     virtListHeadNext,
                                     sizeof (MPLIST_Header)) ;

                    /* Assign the element to list head's next variable */
                    *element = (MPLIST_Elem) virtListHeadNext ;

                    /* Reset next and prev pointers */
                    mplist->head.next = (MPLIST_Elem) (*element)->next ;
                    POOL_writeback (entry->poolId,
                                    mplist,
                                    sizeof (MPLIST_Header)) ;

                    /* Get user space address for element->next */
                    status = POOL_getPoolId (mplistObj->procId,
                                             (*element)->next,
                                             AddrType_Dsp,
                                             &poolId) ;
                    DBC_Assert (DSP_SUCCEEDED (status)) ;
                    status = POOL_translateAddr (poolId,
                                                 (Pvoid *) &(virtNextElement),
                                                 AddrType_Usr,
                                                 (*element)->next,
                                                 AddrType_Dsp) ;
                    if (DSP_SUCCEEDED (status)) {
                        /* Get dsp address for list head */
                        status = POOL_translateAddr (entry->poolId,
                                                     (Pvoid *) &(phyListHead),
                                                     AddrType_Dsp,
                                                     mplist,
                                                     AddrType_Usr) ;
                        if (DSP_SUCCEEDED (status)) {
                            POOL_invalidate (poolId,
                                             virtNextElement,
                                             sizeof (MPLIST_Header)) ;
                            virtNextElement->prev = (MPLIST_Elem) phyListHead ;
                            POOL_writeback (poolId,
                                            virtNextElement,
                                            sizeof (MPLIST_Header)) ;
                        }
                        else {
                            SET_FAILURE_REASON ;
                        }
                    }
                    else {
                        SET_FAILURE_REASON ;
                    }
                }
            }
            else {
                SET_FAILURE_REASON ;
            }

            /* Leave list-specific CS */
            tmpStatus = MPCS_leave (mpcsObj) ;
            if (DSP_SUCCEEDED (status) && DSP_FAILED (tmpStatus)) {
                status = tmpStatus ;
                SET_FAILURE_REASON ;
            }
        }
        else {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("MPLIST_getHead", status) ;

    return status ;
}


#if defined (__cplusplus)
}
# endif /* defined (__cplusplus) */
