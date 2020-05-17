/** ============================================================================
 *  @file   mplist.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Declarations of Multi-processor list management control structures
 *          and definitions of list management functions for lists present in
 *          shared memory.
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


#if !defined (MPLIST_H)
#define MPLIST_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mplistdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   MPLIST_create
 *
 *  @desc   Allocates and initializes a doubly linked circular list in shared
 *          memory. The memory for the object may or may not be provided by the
 *          user.
 *          Uses POOl api to allocate a list object. The list object contains a
 *          MPCS object and a single element and initializes that element to
 *          indicate that it is the "end of the list" (i.e., the list is empty).
 *          An entry is added in the shared memory control structure for the
 *          list.
 *
 *  @arg    procId
 *              Processor id of the processor making this create call.
 *  @arg    mplistObj
 *              Pointer to the shared MPLIST object.
 *              If memory for the MPLIST object is provided by the user, the
 *              MPLIST object handle is not NULL.
 *              Otherwise, if the memory is to be allocated by the MPLIST
 *              component, the MPLIST object handle can be specified as
 *              NULL.
 *  @arg    name
 *              Name of the list to be created.
 *  @arg    attrs
 *              Attributes of the list to be created.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EALREADYEXISTS
 *              The specified MPLIST name is already in use.
 *          DSP_ERESOURCE
 *              All MPLIST entries are currently in use.
 *          DSP_EACCESSDENIED
 *              The MPLIST component has not been initialized.
 *          DSP_ENOTFOUND
 *              Information about specified POOL buffer was not available.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  Processor id must be valid.
 *          MPLIST name must be valid.
 *          MPLIST creation attributes must be a valid.
 *          PROC_attach has been successful.
 *          POOL has been configured according to application specification.
 *
 *  @leave  None
 *
 *  @see    MPLIST_delete
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_create (IN     ProcessorId    procId,
               IN     Char8 *        name,
               IN     MPLIST_List *  mplistObj,
               IN     MPLIST_Attrs * attrs) ;


/** ============================================================================
 *  @func   MPLIST_delete
 *
 *  @desc   Removes a list by freeing its control structure's memory space.
 *          Must ONLY be used for empty lists, because it does not delete the
 *          list elements.  Calling this function on a non-empty list will cause
 *          a memory leak.
 *
 *  @arg    procId
 *              Processor id of the processor making this delete call.
 *  @arg    name
 *              Name of the list to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EACCESSDENIED
 *              The MPLIST component has not been initialized.
 *          DSP_ENOTFOUND
 *              The MPLIST to be deleted is not present.
 *              Information about specified POOL buffer was not available.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  Processor Id must be valid.
 *          MPLIST name must be valid.
 *          All MPLIST operations on that MP_LIST are complete.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_delete (IN ProcessorId procId,
               IN Char8 *     name) ;


/** ============================================================================
 *  @func   MPLIST_open
 *
 *  @desc   This function opens an MPLIST object specified by its name and gets
 *          a handle to the object.
 *          Every process that needs to use the MPLIST object must get a handle
 *          to the object by calling this API.
 *
 *  @arg    procId
 *              Processor id of the processor making the open call.
 *  @arg    name
 *              Name of the list to be opened.
 *  @arg    mplistHandle
 *              Handle of list which has been opened.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFAIL
 *              General Failure.
 *          DSP_EACCESSDENIED
 *              The MPLIST component has not been initialized.
 *          DSP_ENOTFOUND
 *              The MPLIST to be deleted is not present.
 *              Information about specified POOL buffer was not available.
 *          DSP_EACCESSDENIED
 *              The MPLIST component has not been initialized.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          name must be a valid pointer.
 *          MPLIST_create has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_close
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_open (IN     ProcessorId     procId,
             IN     Char8 *         name,
             OUT    MPLIST_Handle * mplistHandle) ;


/** ============================================================================
 *  @func   MPLIST_close
 *
 *  @desc   This function closes an MPLIST instance.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SFREE
 *              Resource has been freed successfully.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EACCESSDENIED
 *              The MPLIST component has not been initialized.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          All MPLIST operations are successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_open
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_close (IN MPLIST_Handle mplistHandle) ;


/** ============================================================================
 *  @func   MPLIST_isEmpty
 *
 *  @desc   Check for an empty list.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *
 *  @ret    TRUE
 *              List is empty.
 *          FALSE
 *              List is not empty.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          MPLIST_create has been successful.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
Bool
MPLIST_isEmpty (IN  MPLIST_Handle mplistHandle) ;


/** ============================================================================
 *  @func   MPLIST_insertBefore
 *
 *  @desc   Inserts the element before the existing element.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    insertElement
 *              Pointer to element in list to insert.
 *  @arg    existingElement
 *              Pointer to existing list element.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_ENOTFOUND
 *              Information about specified POOL buffer was not available.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          insertElement must be a valid pointer.
 *          existingElement must be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create, MPLIST_putTail
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_insertBefore (IN  MPLIST_Handle mplistHandle,
                     IN  MPLIST_Elem   insertElement,
                     IN  MPLIST_Elem   existingElement) ;


/** ============================================================================
 *  @func   MPLIST_putTail
 *
 *  @desc   Adds the specified element to the tail of the list.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    element
 *              Pointer to list element to be added.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_ENOTFOUND
 *              Information about specified POOL buffer was not available.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          element must be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_putTail (IN MPLIST_Handle mplistHandle,
                IN MPLIST_Elem element) ;


/** ============================================================================
 *  @func   MPLIST_removeElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    element
 *              Pointer to element in list to remove.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          element be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore, MPLIST_create, MPLIST_putTail
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_removeElement (IN MPLIST_Handle mplistHandle,
                      IN MPLIST_Elem   element) ;


/** ============================================================================
 *  @func   MPLIST_first
 *
 *  @desc   Returns a pointer to the first element of the list, or NULL if
 *          the list is empty.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    element
 *              OUT parameter for holding the first element.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          element be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create, MPLIST_puttail, MPLIST_insertbefore
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_first (IN MPLIST_Handle   mplistHandle,
              OUT MPLIST_Elem *  element) ;


/** ============================================================================
 *  @func   MPLIST_next
 *
 *  @desc   Returns a pointer to the next element of the list, or NULL if
 *          the next element is the head of the list or the list is empty.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    currentElement
 *              Pointer to element in list to remove.
 *  @arg    nextElement
 *              OUT parameter to hold the next element.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          currentElement be a valid pointer.
 *          nextElement be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore, MPLIST_putTail
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_next (IN  MPLIST_Handle   mplistHandle,
             IN  MPLIST_Elem     currentElement,
             OUT MPLIST_Elem *   nextElement) ;


/** ============================================================================
 *  @func   MPLIST_getHead
 *
 *  @desc   Pops the head off the list and returns a pointer to it.
 *          If the list is empty, returns NULL.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *  @arg    element
 *              OUT Parameter to hold the head element.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid arguments.
 *          DSP_ENOTFOUND
 *              Information about specified POOL buffer was not available.
 *          DSP_EFAIL
 *              General Failure.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          element be a valid pointer.
 *          MPLIST_open has been successful.
 *
 *  @leave  None
 *
 *  @see    MPLIST_puttail, MPLIST_insertbefore
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPLIST_getHead (IN MPLIST_Handle mplistHandle, OUT MPLIST_Elem * element) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MPLIST_H) */

