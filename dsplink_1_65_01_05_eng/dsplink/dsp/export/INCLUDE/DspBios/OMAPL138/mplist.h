/** ============================================================================
 *  @file   mplist.h
 *
 *  @path   $(DSPLINK)/dsp/inc/
 *
 *  @desc   Declarations of MPLIST management control structures and definitions
 *          of inline MPLIST management functions for lists present in shared
 *          memory.
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
 *          memory.  The memory for the object may or may not be provided by the
 *          user.
 *          Uses POOl api to allocate a list object. The list object contains a
 *          MPCS object and a single element and initializes that element to
 *          indicate that it is the "end of the list" (i.e., the list is empty).
 *          An entry is added in the shared memory control structure for the
 *          list.
 *
 *  @arg    procId
 *              Processor id of the processor making this create call.
 *  @arg    name
 *              Name of the list to be created.
 *  @arg    mpcsObj
 *              Pointer to the shared MPLIST object.
 *              If memory for the MPLIST object is provided by the user, the
 *              MPLIST object handle is not NULL.
 *              Otherwise, if the memory is to be allocated by the MPLIST
 *              component, the MPLIST object handle can be specified as
 *              NULL.
 *  @arg    attrs
 *              Attributes of the list to be created.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *          SYS_EALLOC
 *              Operation failed due to a memory error.
 *          SYS_EBUSY
 *              The specified MPLIST name already exists.
 *          SYS_ENOTFOUND
 *              All MPLIST entries are currently in use.
 *          SYS_EDEAD
 *              The MPLIST component has not been initialized.
 *
 *  @enter  name must be a valid pointer.
 *          attr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int
MPLIST_create (IN     Uint16         procId,
               IN     Char *         name,
               IN     MPLIST_List  * mplistObj,
               IN     MPLIST_Attrs * attrs) ;


/** ============================================================================
 *  @func   MPLIST_delete
 *
 *  @desc   Removes a list by freeing its control structure's memory space.
 *          Must ONLY be used for empty lists, because it does not walk the
 *          chain of list elements.  Calling this function on a non-empty list
 *          will cause a memory leak.
 *
 *  @arg    procId
 *              Processor id of the processor making this delete call.
 *  @arg    name
 *              Name of the list to be deleted.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *          SYS_EFREE
 *              Operation failed due to a memory error.
 *          SYS_ENOTFOUND
 *              Specified MPLIST object name does not exist.
 *          SYS_EDEAD
 *              The MPLIST component has not been initialized.
 *
 *  @enter  name must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create
 *  ============================================================================
 */
Int
MPLIST_delete (IN  Uint16    procId,
               IN  Char *    name) ;


/** ============================================================================
 *  @func   MPLIST_open
 *
 *  @desc   This function opens an MPLIST object specified by its name and gets
 *          a handle to the object.
 *          Every process that needs to use the MPLIST object must get a handle
 *          to the object by calling this API.
 *
 *  @arg    procId
 *              Processor id of the processor making this open call.
 *  @arg    name
 *              Name of the list to be opened.
 *  @arg    mplistHandle
 *              Handle of list which has to be opened.
 *
 *  @ret    SYS_SOK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *          SYS_EALLOC
 *              Operation failed due to a memory error.
 *          SYS_ENOTFOUND
 *              Specified MPLIST object name does not exist.
 *          SYS_EDEAD
 *              The MPLIST component has not been initialized.
 *
 *  @enter  mplistHandle must be a valid pointer .
 *          name must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_close
 *  ============================================================================
 */
Int
MPLIST_open (IN     Uint16          procId,
             IN     Char *          name,
             OUT    MPLIST_Handle * mplistHandle) ;


/** ============================================================================
 *  @func   MPLIST_close
 *
 *  @desc   This function closes an MPLIST instance.
 *
 *  @arg    mplistHandle
 *              Handle for list operations.
 *
 *  @ret    SYS_EOK
 *              Operation successfully completed.
 *          SYS_EFAIL
 *              General Failure.
 *          SYS_EFREE
 *              Operation failed due to a memory error.
 *          SYS_ENOTFOUND
 *              Specified MPLIST object name does not exist.
 *          SYS_EDEAD
 *              The MPLIST component has not been initialized.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_open_
 *  ============================================================================
 */
Int
MPLIST_close (IN  MPLIST_Handle mplistHandle) ;

/** ============================================================================
 *  @func   MPLIST_isEmpty
 *
 *  @desc   Check for an empty list.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *
 *  @ret    TRUE
 *              List is empty.
 *          FALSE
 *              List is not empty.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Bool
MPLIST_isEmpty (IN  MPLIST_Handle mplistHandle) ;


/** ============================================================================
 *  @func   MPLIST_insertBefore
 *
 *  @desc   Inserts the element before the existing element.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    insertElement
 *              Pointer to element in list to insert.
 *  @arg    existingElement
 *              Pointer to existing list element.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          insertElement must be a valid pointer.
 *          existingElement must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create, MPLIST_putTail
 *  ============================================================================
 */
Int
MPLIST_insertBefore (IN  MPLIST_Handle mplistHandle,
                     IN  MPLIST_Elem   insertElement,
                     IN  MPLIST_Elem   existingElement) ;


/** ============================================================================
 *  @func   MPLIST_putTail
 *
 *  @desc   Adds the specified element to the tail of the list.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    element
 *              Pointer to list element to be added.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          element must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore
 *  ============================================================================
 */
Int
MPLIST_putTail (IN  MPLIST_Handle mplistHandle, IN  MPLIST_Elem element) ;


/** ============================================================================
 *  @func   MPLIST_removeElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    element
 *              Pointer to element in list to remove.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          Element must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore, MPLIST_create, MPLIST_putTail
 *  ============================================================================
 */
Int
MPLIST_removeElement (IN  MPLIST_Handle   mplistHandle,
                      IN  MPLIST_Elem     element) ;


/** ============================================================================
 *  @func   MPLIST_first
 *
 *  @desc   Returns a pointer to the first element of the list, or NULL if
 *          the list is empty.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    element
 *              parameter for holding the first element.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  mplistHandle must be a valid pointer.
 *          Element must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_create, MPLIST_putTail, MPLIST_insertBefore
 *  ============================================================================
 */
Int
MPLIST_first (IN  MPLIST_Handle mplistHandle,
              OUT MPLIST_Elem * element) ;


/** ============================================================================
 *  @func   MPLIST_getHead
 *
 *  @desc   Pops the head off the list and returns a pointer to it.
 *          If the list is empty, returns NULL.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    headElement
 *              Parameter to hold the head element.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  List handle must be a valid pointer.
 *          Head element must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_putTail, MPLIST_insertBefore
 *  ============================================================================
 */
Int
MPLIST_getHead (IN  MPLIST_Handle  mplistHandle,
                OUT MPLIST_Elem *  element) ;


/** ============================================================================
 *  @func   MPLIST_next
 *
 *  @desc   Returns a pointer to the next element of the list, or NULL if
 *          the next element is the head of the list or the list is empty.
 *
 *  @arg    mplistHandle
 *              List handle for list operations.
 *  @arg    currentElement
 *              Pointer to element in list to remove.
 *  @arg    nextElement
 *              Pointer to next element.
 *
 *  @ret    SYS_OK
 *              Operation successfully completed.
 *          SYS_EINVAL
 *              Invalid Parameters.
 *
 *  @enter  List handle must be a valid pointer.
 *          currentElement must be a valid pointer.
 *          nextElement must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPLIST_insertBefore, MPLIST_putTail
 *  ============================================================================
 */
Int
MPLIST_next (IN  MPLIST_Handle   mplistHandle,
             IN  MPLIST_Elem     currentElement,
             OUT MPLIST_Elem *   nextElement) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MPLIST_H) */

