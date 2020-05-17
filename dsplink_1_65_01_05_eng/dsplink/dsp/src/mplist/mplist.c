/** ============================================================================
 *  @file   mplist.c
 *
 *  @path   $(DSPLINK)/dsp/src/mplist/
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


/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <gbl.h>
#include <mem.h>
#include <sys.h>
#include <pool.h>
#include <string.h>

/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>
#include <dbc.h>
#include <hal_cache.h>
#include <mpcs.h>
#include <mplist.h>
#include <_mplist.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  FILEID
 *
 *  @desc   File Id of this file.
 *  ============================================================================
 */
#define     FILEID            FID_MPLIST_C

/** ============================================================================
 *  @const  MPLIST_MEM_ALIGN
 *
 *  @desc   Default alignment to be used for local memory allocations.
 *  ============================================================================
 */
#define     MPLIST_MEM_ALIGN  0

/** ============================================================================
 *  @name   MPLIST_ctrlPtr
 *
 *  @desc   MPLIST Entry Table. This table maps an MPLIST to the MPLIST
 *          Control structure
 *  ============================================================================
 */
extern MPLIST_Ctrl * MPLIST_ctrlPtr ;

/** ============================================================================
 *  @name   MPLIST_mpcsObj
 *
 *  @desc   Declaration of pointer to MPLIST Control region.
 *  ============================================================================
 */
extern MPCS_Handle   MPLIST_mpcsObj ;


/** ============================================================================
 *  @func   MPLIST_create
 *
 *  @desc   Allocates and initializes a doubly linked circular list. Adds an
 *          entry in the control structure in shared memory.
 *
 *  @modif  MPLIST_ctrlPtr
 *  ============================================================================
 */
Int
MPLIST_create (IN     Uint16         procId,
               IN     Char *         name,
               IN     MPLIST_List  * mplistObj,
               IN     MPLIST_Attrs * attrs)
{
    Int           status     = SYS_OK ;
    Int           tmpStatus  = SYS_OK ;
    Int32         slot       = -1 ;
    Uint32        i ;
    MPCS_Attrs    mpcsAttrs ;

    DBC_require (name  != NULL) ;
    DBC_require (attrs != NULL) ;

    if ((name == NULL) || (attrs == NULL)){
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPLIST_ctrlPtr == NULL) {
        /* Check if the MPLIST component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        tmpStatus = MPCS_enter (MPLIST_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

        /* Check if the name already exists in the array and get a free slot
         * in the array if the existing matching name is not found.
         */
        for (i = 0 ;
            (i < MPLIST_ctrlPtr->maxEntries) && (status == SYS_OK) ;
            i++) {
            HAL_cacheInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                          sizeof (MPLIST_Entry)) ;
            if (strncmp (name, MPLIST_ctrlPtr->dspAddrEntry [i].name,
                        DSP_MAX_STRLEN) == 0) {
                status = SYS_EBUSY ;
                SET_FAILURE_REASON (status) ;
            }

            if ((slot == -1)
                &&  (MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle ==  NULL)) {
                /* Reserve the found slot. */
                MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle = (Void *) 1 ;
                HAL_cacheWbInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                                sizeof (MPLIST_Entry)) ;
                slot = i ;
            }
        }

        /* Existing matching name was not found, but all slots are full. */
        if ((status == SYS_OK) && (slot == -1)) {
            status = SYS_ENOTFOUND ;
            SET_FAILURE_REASON (status) ;
        }
        tmpStatus = MPCS_leave (MPLIST_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

        if ((status == SYS_OK)) {
            /* Allocate memory for the MPLIST object if it was not provided
             * by the user.
             */
            if (mplistObj == NULL) {
                status = POOL_alloc (attrs->poolId,
                                    (Void **) &mplistObj,
                                    sizeof (MPLIST_List)) ;
                if (status != SYS_OK) {
                    SET_FAILURE_REASON (status) ;
                }
            }

            if (status == SYS_OK) {
                /* Initialize the list */
                mplistObj->head.next = &(mplistObj->head) ;
                mplistObj->head.prev = &(mplistObj->head) ;
                HAL_cacheWbInv ((Ptr) mplistObj, sizeof (MPLIST_Header)) ;
            }

            /* Create the MPCS object */
            if ((status == SYS_OK)) {
                mpcsAttrs.poolId = attrs->poolId ;
                /* Use the MPLIST name as the MPCS name. */
                status = MPCS_create (procId,
                                      name,
                                      &(mplistObj->mpcsObj),
                                      &mpcsAttrs) ;
            }

            /* Add the entry in the list */
            if ((status == SYS_OK)) {
                MPCS_enter (MPLIST_mpcsObj) ;
                MPLIST_ctrlPtr->dspAddrEntry [slot].phyListHandle = mplistObj ;
                MPLIST_ctrlPtr->dspAddrEntry [slot].ownerProcId   =
                                                              GBL_getProcId () ;
                MPLIST_ctrlPtr->dspAddrEntry [slot].poolId = attrs->poolId ;
                strncpy (MPLIST_ctrlPtr->dspAddrEntry [slot].name,
                         name,
                         DSP_MAX_STRLEN) ;

                HAL_cacheWbInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [slot]),
                                    sizeof (MPLIST_Entry)) ;

                MPCS_leave (MPLIST_mpcsObj) ;
            }
            else {
                /* Reset the slot to empty so that it can be used later. */
                MPLIST_ctrlPtr->dspAddrEntry [slot].phyListHandle = NULL ;
                HAL_cacheWbInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [slot]),
                                sizeof (MPLIST_Entry)) ;

                /* Free any memory that was successfully allocated. */
                if (mplistObj != NULL) {
                    POOL_free (attrs->poolId,
                              (Void *) mplistObj,
                              sizeof (MPLIST_List)) ;
                }
            }
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_delete
 *
 *  @desc   Removes a list by freeing its control structure's memory space.
 *          Must ONLY be used for empty lists, because it does not walk the
 *          chain of list elements.
 *
 *  @modif  MPLIST_ctrlPtr
 *  ============================================================================
 */
Int
MPLIST_delete (IN  Uint16    procId,
               IN  Char *    name)
{
    Int            status    = SYS_OK ;
    Int            tmpStatus = SYS_OK ;
    MPLIST_Entry * mplistEntry ;
    Uint32       i ;

    DBC_require (name != NULL) ;

    if (name == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPLIST_ctrlPtr == NULL) {
        /* Check if the MPLIST component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        tmpStatus = MPCS_enter (MPLIST_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

        /* Find the MPLIST entry corresponding to the specified name. */
        for (i = 0 ; i < MPLIST_ctrlPtr->maxEntries ; i++) {
            HAL_cacheInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                          sizeof (MPLIST_Entry)) ;
            /* Check if the entry is valid. */
            if (   (MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle != NULL)
                && (    MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle
                    !=  (Void *) 1)) {
                if (strncmp (name, MPLIST_ctrlPtr->dspAddrEntry [i].name,
                            DSP_MAX_STRLEN) == 0) {
                    break ;
                }
            }
        }

        MPCS_leave (MPLIST_mpcsObj) ;

        if (i != MPLIST_ctrlPtr->maxEntries) {
            mplistEntry = &(MPLIST_ctrlPtr->dspAddrEntry [i]) ;
            HAL_cacheInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                           sizeof (MPLIST_Entry)) ;

            if ( mplistEntry->ownerProcId == GBL_getProcId()) {
                /* Delete the created MPCS object. */
                status = MPCS_delete (procId, name) ;
                DBC_assert (status == SYS_OK) ;

                /* Free the list. */
                POOL_free (mplistEntry->poolId,
                          (Void *) mplistEntry->phyListHandle,
                          sizeof (MPLIST_List)) ;

                tmpStatus = MPCS_enter (MPLIST_mpcsObj) ;
                DBC_assert (tmpStatus == SYS_OK) ;
                if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
                    status = tmpStatus ;
                    SET_FAILURE_REASON (status) ;
                }
                memset (&MPLIST_ctrlPtr->dspAddrEntry [i],
                        0,
                        sizeof (MPLIST_Entry)) ;
                HAL_cacheWbInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                                sizeof (MPLIST_Entry)) ;
                MPCS_leave (MPLIST_mpcsObj) ;
            }
            else {
                status = SYS_EINVAL ;
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            status = SYS_ENOTFOUND ;
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_open
 *
 *  @desc   This function opens an MPLIST object specified by its name and gets
 *          a handle to the object.
 *
 *  @modif  None
 *  ============================================================================
 */
Int
MPLIST_open (IN     Uint16          procId,
             IN     Char *          name,
             OUT    MPLIST_Handle * mplistHandle)
{
    Int            status          = SYS_OK ;
    Int            tmpStatus       = SYS_OK ;
    MPLIST_List *  phyListHandle   = NULL ;
    MPCS_Handle    phyLockHandle   = NULL ;
    MPLIST_Entry * phyEntryHandle  = NULL ;
    Uint32         i ;

    /* To avoid compiler warning */
    (Void) procId ;

    DBC_require (name         != NULL) ;
    DBC_require (mplistHandle != NULL) ;

    if (   (name            == NULL)
        || (mplistHandle    == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON(status) ;
    }
    else if (MPLIST_ctrlPtr == NULL) {
        *mplistHandle = NULL ;
        /* Check if the MPLIST component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON(status) ;
    }
    else {
        *mplistHandle = NULL ;
        tmpStatus = MPCS_enter (MPLIST_mpcsObj) ;
        DBC_assert(tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

        /* Find the MPLIST entry corresponding to the specified name. */
        for (i = 0 ; i < MPLIST_ctrlPtr->maxEntries ; i++) {
            HAL_cacheInv ((Ptr) &(MPLIST_ctrlPtr->dspAddrEntry [i]),
                          sizeof (MPLIST_Entry)) ;
            /* Check if the entry is valid. */
            if (    (MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle != NULL)
                &&  (   MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle
                     != (Void *) 1)) {
                if (strncmp (name, MPLIST_ctrlPtr->dspAddrEntry [i].name,
                            DSP_MAX_STRLEN) == 0) {
                    break ;
                }
            }
        }
        MPCS_leave (MPLIST_mpcsObj) ;

        if (i == MPLIST_ctrlPtr->maxEntries) {
            status = SYS_ENOTFOUND ;
            SET_FAILURE_REASON (status) ;
        }
        else {
            /* Get the user address of the MPLIST object. */
            phyListHandle = MPLIST_ctrlPtr->dspAddrEntry [i].phyListHandle ;

            /* Open the MPCS object to get the physical address of lock. */
            status = MPCS_open (MPLIST_ctrlPtr->dspId,
                                name,
                                &phyLockHandle) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }

            /* Get the user address of the MPLIST shared region entry. */
            phyEntryHandle = &(MPLIST_ctrlPtr->dspAddrEntry [i]) ;

            if (status == SYS_OK){
                *mplistHandle = MEM_calloc (DSPLINK_SEGID, sizeof (MPLIST_Obj),
                                            MPLIST_MEM_ALIGN) ;
                if (*mplistHandle != MEM_ILLEGAL) {
                    (*mplistHandle)->phyListHandle  = phyListHandle ;
                    (*mplistHandle)->phyLockHandle  = phyLockHandle ;
                    (*mplistHandle)->phyEntryHandle = phyEntryHandle ;
                }
                else {
                    status = SYS_EALLOC ;
                    SET_FAILURE_REASON (status) ;
                }
            }
        }
    }

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_close
 *
 *  @desc   This function closes an MPLIST instance.
 *  ============================================================================
 */
Int
MPLIST_close (IN  MPLIST_Handle mplistHandle)
{
    Int status    = SYS_OK ;
    Int tmpStatus = SYS_OK ;

    DBC_require (mplistHandle != NULL) ;

    if (mplistHandle == NULL) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else if (MPLIST_ctrlPtr == NULL) {
        /* Check if the MPLIST component has been initialized. */
        status = SYS_EDEAD ;
        SET_FAILURE_REASON (status);
    }
    else {
        tmpStatus = MPCS_enter (MPLIST_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        mplistHandle->phyListHandle = NULL ;
        mplistHandle->phyEntryHandle = NULL ;

        tmpStatus = MPCS_leave (MPLIST_mpcsObj) ;
        DBC_assert (tmpStatus == SYS_OK) ;
        if ((status == SYS_OK) && (tmpStatus != SYS_OK)) {
            status = tmpStatus ;
            SET_FAILURE_REASON (status) ;
        }

        status = MPCS_close (MPLIST_ctrlPtr->dspId,
                            (MPCS_Handle) mplistHandle->phyLockHandle) ;
        mplistHandle->phyLockHandle = NULL ;

        /* Free the user MPLIST handle. */
        MEM_free (DSPLINK_SEGID,
                  mplistHandle,
                  sizeof(MPLIST_Obj)) ;
    }

    return status ;
}


/** ============================================================================
 *  @func   MPLIST_isEmpty
 *
 *  @desc   Check for an empty list.
 *
 *  @modif  None.
 *  ============================================================================
 */
Bool
MPLIST_isEmpty (IN  MPLIST_Handle mplistHandle)
{
    Bool          retVal    = FALSE ;
    MPLIST_List * mpListObj ;

    DBC_require (mplistHandle != NULL) ;

    mpListObj = (MPLIST_List *) mplistHandle->phyListHandle ;

    HAL_cacheInv ((Ptr) (mpListObj), sizeof (MPLIST_Header)) ;

    /* If the next points to head itself, the list is empty */
    if (((Uint32)(mpListObj)->head.next) == (Uint32)(&(mpListObj->head))) {
        retVal = TRUE ;
    }

    return retVal ;
}


/** ============================================================================
 *  @func   MPLIST_insertBefore
 *
 *  @desc   Inserts the element before the existing element.
 *
 *  @modif  MPLIST_ctrlPtr
 *  ============================================================================
 */
Int
MPLIST_insertBefore (IN  MPLIST_Handle mplistHandle,
                     IN  MPLIST_Elem   insertElement,
                     IN  MPLIST_Elem   existingElement)
{
    Int         status = SYS_OK ;
    MPCS_Handle mpcsObj ;

    DBC_require (mplistHandle    != NULL) ;
    DBC_require (insertElement   != NULL) ;
    DBC_require (existingElement != NULL) ;

    if (   (mplistHandle    == NULL)
        || (insertElement   == NULL)
        || (existingElement == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpcsObj = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {
            HAL_cacheInv ((Ptr) (existingElement),
                           sizeof (MPLIST_Header)) ;
            HAL_cacheInv ((Ptr) (existingElement->prev),
                          sizeof (MPLIST_Header)) ;

            existingElement->prev->next = insertElement ;
            HAL_cacheWbInv ((Ptr) (existingElement->prev),
                            sizeof (MPLIST_Header)) ;
            insertElement->prev         = existingElement->prev ;
            insertElement->next         = existingElement ;
            HAL_cacheWbInv ((Ptr) (insertElement),
                             sizeof (MPLIST_Header)) ;
            existingElement->prev       = insertElement ;
            HAL_cacheWbInv ((Ptr) (existingElement),
                             sizeof (MPLIST_Header)) ;

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else
        {
            SET_FAILURE_REASON (status) ;
        }
    }

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
Int
MPLIST_putTail (IN  MPLIST_Handle mplistHandle, IN  MPLIST_Elem element)
{
    Int           status  = SYS_OK ;
    MPLIST_List * mpListObj ;
    MPCS_Handle   mpcsObj ;

    DBC_require (mplistHandle != NULL) ;
    DBC_require (element      != NULL) ;

    if (   (mplistHandle == NULL)
        || (element      == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpListObj = (MPLIST_List *) mplistHandle->phyListHandle ;
        mpcsObj   = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {
            HAL_cacheInv ((Ptr) (mpListObj), sizeof (MPLIST_Header)) ;

            element->prev        = (MPLIST_Elem) mpListObj->head.prev ;
            element->next        = (MPLIST_Elem) &(mpListObj->head) ;
            HAL_cacheWbInv ((Ptr) (element),sizeof (MPLIST_Header)) ;

            mpListObj->head.prev = (MPLIST_Elem) element ;
            HAL_cacheWbInv ((Ptr) (mpListObj),sizeof (MPLIST_Header)) ;

            HAL_cacheInv ((Ptr) (element->prev), sizeof (MPLIST_Header)) ;
            element->prev->next  = (MPLIST_Elem) element ;

            HAL_cacheWbInv ((Ptr) (element->prev),sizeof (MPLIST_Header)) ;

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

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
Int
MPLIST_removeElement (IN  MPLIST_Handle   mplistHandle,
                      IN  MPLIST_Elem     element)
{
    Int         status = SYS_OK ;
    MPCS_Handle mpcsObj ;

    DBC_require (mplistHandle != NULL) ;
    DBC_require (element      != NULL) ;

    if (   (mplistHandle == NULL)
        || (element      == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpcsObj = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {
            HAL_cacheInv ((Ptr) (element), sizeof (MPLIST_Header)) ;
            HAL_cacheInv ((Ptr) (element->prev), sizeof (MPLIST_Header)) ;
            HAL_cacheInv ((Ptr) (element->next), sizeof (MPLIST_Header)) ;

            element->prev->next = element->next ;
            HAL_cacheWbInv ((Ptr) (element->prev),
                             sizeof (MPLIST_Header)) ;
            element->next->prev = element->prev ;
            HAL_cacheWbInv ((Ptr) (element->next),
                             sizeof (MPLIST_Header)) ;

            /* set elem fields to NULL to prevent illegal references */
            element->next = NULL ;
            element->prev = NULL ;

            HAL_cacheWbInv ((Ptr) (element),sizeof (MPLIST_Header)) ;

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else
        {
            SET_FAILURE_REASON (status) ;
        }
    }

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
Int
MPLIST_first (IN  MPLIST_Handle mplistHandle,
              OUT MPLIST_Elem * element)
{
    Int           status = SYS_OK ;
    MPLIST_List * mpListObj ;
    MPCS_Handle   mpcsObj ;

    DBC_require (mplistHandle != NULL) ;
    DBC_require (element      != NULL) ;

    if (   (mplistHandle == NULL)
        || (element      == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpListObj = (MPLIST_List *) mplistHandle->phyListHandle ;
        mpcsObj   = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {

           *element = NULL ;
           HAL_cacheInv ((Ptr) (mpListObj), sizeof (MPLIST_Header)) ;

           if (!MPLIST_isEmpty (mplistHandle)) {
               *element  = mpListObj->head.next ;
               HAL_cacheInv ((Ptr) (*element), sizeof (MPLIST_Header)) ;
           }

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else
        {
            SET_FAILURE_REASON (status) ;
        }
    }

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
Int
MPLIST_getHead (IN  MPLIST_Handle  mplistHandle,
                OUT MPLIST_Elem *  element)
{
    Int           status = SYS_OK ;
    MPLIST_List * mpListObj ;
    MPCS_Handle   mpcsObj ;

    DBC_require (mplistHandle != NULL) ;
    DBC_require (element      != NULL) ;

    if (    (mplistHandle == NULL)
         || (element      == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpListObj = (MPLIST_List *) mplistHandle->phyListHandle ;
        mpcsObj   = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {
            *element = NULL ;
            HAL_cacheInv ((Ptr) (mpListObj), sizeof (MPLIST_Header)) ;

            if (!MPLIST_isEmpty (mplistHandle)) {
                *element = (MPLIST_Elem) (mpListObj->head.next) ;

                HAL_cacheInv ((Ptr) (*element), sizeof (MPLIST_Header)) ;
                mpListObj->head.next = (MPLIST_Elem) ((*element)->next) ;
                HAL_cacheWbInv ((Ptr) (mpListObj), sizeof (MPLIST_Header)) ;

                HAL_cacheInv ((Ptr) (*element)->next, sizeof (MPLIST_Header)) ;
                (*element)->next->prev = (MPLIST_Elem)(&(mpListObj->head)) ;
                HAL_cacheWbInv ((Ptr) ((*element)->next),
                                 sizeof(MPLIST_Header) ) ;

            }

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

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
Int
MPLIST_next (IN  MPLIST_Handle   mplistHandle,
             IN  MPLIST_Elem     currentElement,
             OUT MPLIST_Elem *   nextElement)
{
    Int           status = SYS_OK ;
    MPLIST_List * mpListObj ;
    MPCS_Handle   mpcsObj ;

    DBC_require (mplistHandle   != NULL) ;
    DBC_require (currentElement != NULL) ;
    DBC_require (nextElement    != NULL) ;

    if (   (mplistHandle   == NULL)
        || (currentElement == NULL)
        || (nextElement    == NULL)) {
        status = SYS_EINVAL ;
        SET_FAILURE_REASON (status) ;
    }
    else {
        mpListObj = (MPLIST_List *) mplistHandle->phyListHandle ;
        mpcsObj   = (MPCS_Handle) mplistHandle->phyLockHandle ;

        status = MPCS_enter (mpcsObj) ;
        if (status == SYS_OK) {
            /* prime the variable */
            *nextElement = NULL ;

            if (!MPLIST_isEmpty (mplistHandle)) {
                HAL_cacheInv ((Ptr) (currentElement),
                              sizeof (MPLIST_Header)) ;

                /* Checking if current element is last in the list */
                if (currentElement->next != &(mpListObj->head)) {
                    *nextElement = currentElement->next ;
                    HAL_cacheInv ((Ptr) (*nextElement),
                                  sizeof (MPLIST_Header)) ;
                }
            }

            status = MPCS_leave (mpcsObj) ;
            if (status != SYS_OK) {
                SET_FAILURE_REASON (status) ;
            }
        }
        else {
            SET_FAILURE_REASON (status) ;
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
