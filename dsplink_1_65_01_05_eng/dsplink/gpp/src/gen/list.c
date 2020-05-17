/** ============================================================================
 *  @file   list.c
 *
 *  @path   $(DSPLINK)/gpp/src/gen/
 *
 *  @desc   Implementation of function for LIST component.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_safe.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <mem.h>

/*  ----------------------------------- Generic Functions           */
#include <list.h>



#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_GEN_LIST


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_GEN_LIST, __LINE__)
#else
#define SET_FAILURE_REASON { }
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   LIST_Initialize
 *
 *  @desc   Initializes private data structures for the LIST subcomponent.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_Initialize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("LIST_Initialize") ;

    TRC_1LEAVE ("LIST_Initialize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_Finalize
 *
 *  @desc   Discontinues usage of module.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_Finalize (Void)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_0ENTER ("LIST_Finalize") ;

    TRC_1LEAVE ("LIST_Finalize", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_Create
 *
 *  @desc   Allocates and initializes a circular list.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_Create (OUT List ** list)
{
    DSP_STATUS  status  = DSP_SOK ;
    List *      myList = NULL    ;

    TRC_1ENTER ("LIST_Create", list) ;

    DBC_Require (list != NULL) ;

    if (list == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        status = MEM_Calloc ((Void **) &myList,
                             sizeof(List),
                             MEM_DEFAULT) ;

        if (DSP_FAILED (status)) {
            status = DSP_EMEMORY ;
            SET_FAILURE_REASON ;
        }
        else {
            DBC_Assert (myList != NULL) ;

            myList->head.next = &(myList->head) ;
            myList->head.prev = &(myList->head) ;

            *list = myList ;
        }
    }

    DBC_Ensure (   (DSP_SUCCEEDED (status) && (*list != NULL))
                || (DSP_FAILED (status))) ;

    TRC_1LEAVE ("LIST_Create", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_Delete
 *
 *  @desc   Removes a list by freeing its control structure's memory space.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_Delete (IN List * list)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LIST_Delete", list) ;

    DBC_Require (list != NULL) ;

    if (list == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        status = FREE_PTR (list) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LIST_Delete", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_InitializeElement
 *
 *  @desc   Initializes a list element to default (cleared) values.
 *          This function must not be called to "reset" an element in the middle
 *          of a list chain -- that would break the chain.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_InitializeElement (IN ListElement * element)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("LIST_InitializeElement", element) ;

    DBC_Require (element != NULL) ;

    if (element == NULL) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        element->next = NULL ;
        element->prev = NULL ;
    }

    TRC_1LEAVE ("LIST_InitializeElement", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_InsertBefore
 *
 *  @desc   Insert the element before the existing element.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_InsertBefore (IN  List *          list,
                   IN  ListElement *   insertElement,
                   IN  ListElement *   existingElement)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("LIST_InsertBefore", list, insertElement, existingElement) ;

    DBC_Require(list != NULL) ;
    DBC_Require(insertElement != NULL) ;
    DBC_Require(existingElement != NULL) ;

    if (   (list ==  NULL)
        || (insertElement == NULL)
        || (existingElement == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        existingElement->prev->next = insertElement ;
        insertElement->prev         = existingElement->prev ;
        insertElement->next         = existingElement ;
        existingElement->prev       = insertElement ;
    }

    DBC_Assert (   (DSP_SUCCEEDED (status) && (!LIST_IsEmpty (list)))
                || (DSP_FAILED (status)));

    TRC_1LEAVE ("LIST_InsertBefore", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_PutTail
 *
 *  @desc   Adds the specified element to the tail of the list.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_PutTail (IN List * list, IN ListElement * element)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("LIST_PutTail", list, element) ;

    DBC_Require (list != NULL) ;
    DBC_Require (element != NULL) ;

    if ((list == NULL) || (element == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        element->prev       = list->head.prev ;
        element->next       = &list->head ;
        list->head.prev     = element ;
        element->prev->next = element ;
    }

    DBC_Assert (   (DSP_SUCCEEDED (status) && (!LIST_IsEmpty (list)))
                || (DSP_FAILED (status)));

    TRC_1LEAVE ("LIST_PutTail", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_RemoveElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_RemoveElement (IN List * list, IN ListElement * element)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("LIST_RemoveElement", list, element) ;

    DBC_Require (list != NULL) ;
    DBC_Require (element != NULL) ;

    if ((list == NULL) || (element == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (!LIST_IsEmpty (list)) {
            element->prev->next = element->next ;
            element->next->prev = element->prev ;

            /* set elem fields to NULL to prevent illegal references */
            element->next = NULL ;
            element->prev = NULL ;
        }
        else {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
    }

    TRC_1LEAVE ("LIST_RemoveElement", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_First
 *
 *  @desc   Returns a pointer to the first element of the list, or NULL if
 *          the list is empty.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_First (IN List * list, OUT ListElement ** element)
{
    DSP_STATUS      status = DSP_SOK ;

    TRC_2ENTER ("LIST_First", list, element) ;

    DBC_Require (list != NULL) ;

    if ((list == NULL) || (element == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (!LIST_IsEmpty (list)) {
            *element  = list->head.next ;
        }
        else {
            *element = NULL ;
        }
    }

    TRC_1LEAVE ("LIST_First", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_GetHead
 *
 *  @desc   Pops the head off the list and returns a pointer to it.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_GetHead (IN List * list, OUT ListElement ** headElement)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_2ENTER ("LIST_GetHead", list, headElement) ;

    DBC_Require (list != NULL) ;
    DBC_Require (headElement != NULL) ;

    if ((list == NULL) || (headElement == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (LIST_IsEmpty (list)) {
            *headElement = NULL ;
        }
        else {
            *headElement = list->head.next ;
            list->head.next = (*headElement)->next ;
            (*headElement)->next->prev = &list->head ;
        }
    }

    TRC_1LEAVE ("LIST_GetHead", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_Next
 *
 *  @desc   Returns a pointer to the next element of the list, or NULL if
 *          the next element is the head of the list or the list is empty.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_Next (IN  List *          list,
           IN  ListElement *   currentElement,
           OUT ListElement **  nextElement)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_3ENTER ("LIST_Next", list, currentElement, nextElement) ;

    DBC_Require (list != NULL) ;
    DBC_Require (currentElement != NULL) ;
    DBC_Require (nextElement != NULL) ;

    if (   (list == NULL)
        || (currentElement == NULL)
        || (nextElement == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        *nextElement = NULL ;           /* prime the variable */
        if (!LIST_IsEmpty (list)) {
            if (currentElement->next != &list->head) {
                *nextElement = currentElement->next ;
            }
        }
    }

    TRC_1LEAVE ("LIST_Next", status) ;

    return status ;
}


/** ============================================================================
 *  @func   LIST_SearchElement
 *
 *  @desc   This function searchs for a element in the List.
 *
 *  @modif  None.
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LIST_SearchElement (IN  List *         list,
                    IN  Pvoid          data,
                    OUT ListElement ** elem,
                    IN  ListMatchFunc  matchFunc)
{
    DSP_STATUS          status = DSP_SOK ;
    ListElement *       temp   = NULL    ;
    ListElement *       temp1  = NULL    ;
    Bool                found  = FALSE   ;

    DBC_Require (list != NULL) ;
    DBC_Require (elem != NULL) ;
    DBC_Require (matchFunc != NULL) ;

    if ((list == NULL) || (elem == NULL)) {
        status = DSP_EINVALIDARG ;
        SET_FAILURE_REASON ;
    }
    else {
        if (LIST_IsEmpty (list)) {
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }

        if (DSP_SUCCEEDED (status)) {
            status = LIST_First (list, &temp) ;
            if (DSP_SUCCEEDED (status)) {
                while ((found == FALSE) && (temp != NULL)) {
                    if ((*matchFunc) (temp, data) == TRUE) {
                        found = TRUE ;
                    }
                    else {
                        temp1 = temp ;
                        LIST_Next (list, temp1, &temp) ;
                    }
                }

                if (found == TRUE) {
                    *elem = temp ;
                }
                else {
                    *elem = NULL ;
                    status = DSP_ENOTFOUND ;
                    SET_FAILURE_REASON ;
                }
            }
            else {
                SET_FAILURE_REASON ;
            }
        }
    }

    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
