/** ============================================================================
 *  @file   ldrv_mplist.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MPLIST/
 *
 *  @desc   Declarations for the MPLIST for GPP-DSP communication.
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


#if !defined (LDRV_MPLIST_H)
#define LDRV_MPLIST_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <list.h>
#include <mplistdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_MPLIST_moduleInit
 *
 *  @desc   This function initializes the LDRV_MPLIST module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MPLIST module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the MPLIST component.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          shDspAddr must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    LDRV_MPLIST_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_exit
 *
 *  @desc   This function finalizes the LDRV MPLIST module for a particular DSP.
 *          Finalizing means no further services will be provided by this
 *          module for this particular DSP.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MPLIST_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_clear
 *
 *  @desc   Clears a statically created list and translates its contents to
 *          work with DSP address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    list
 *              Pointer to the list to be initialized.
 *
 *  @ret    None.
 *
 *  @enter  The list parameter must be a valid pointer.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_getHead (), LDRV_MPLIST_putTail ()
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_clear (IN     ProcessorId dspId,
                   IN     List *      list) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_getHead
 *
 *  @desc   Gets the head pointer from the list while working on the list object
 *          and element fields in DSP address space.
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    list
 *              Pointer to the list whose head element is to be removed.
 *  @arg    headElement
 *              Location to receive a pointer to the head element.
 *  @ret    None.
 *
 *  @enter  The list and headElement parameters must be valid pointers.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_clear (), LDRV_MPLIST_putTail ()
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_getHead (IN     ProcessorId    dspId,
                     IN     List *      list,
                     OUT    ListElement ** headElement ) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_putTail
 *
 *  @desc   Adds the specified element to the tail of the list while working on
 *          the list object and element fields in DSP address space.(extended)
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    list
 *              Pointer to the list to which the element is to be added.
 *  @arg    element
 *              Pointer to list element to be added.
 *
 *  @ret    None.
 *
 *  @enter  The list and element parameters must be valid pointers.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_getHead (), LDRV_MPLIST_clear ()
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_putTail (IN     ProcessorId   dspId,
                     IN     List *        list,
                     IN     ListElement * element ) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_removeElement
 *
 *  @desc   Removes (unlinks) the given element from the list, if the list is
 *          not empty.  Does not free the list element. This function works on
 *          the list object and element fields in DSP address space.(extended)
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    list
 *              Pointer to the list from which element is to be removed.
 *  @arg    element
 *              Pointer to the element to be removed from the list.
 *
 *  @ret    None.
 *
 *  @enter  The list and element parameters must be valid pointers.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_getHead (), LDRV_MPLIST_putTail ()
 *  ============================================================================
 */
EXPORT_API
Void
LDRV_MPLIST_removeElement (IN     ProcessorId    dspId,
                           IN     List *         list,
                           IN     ListElement *  element ) ;


/** ============================================================================
 *  @func   LDRV_MPLIST_isEmpty
 *
 *  @desc   check for an empty list (extended).
 *
 *  @arg    dspId
 *              Processor Id.
 *  @arg    list
 *              Pointer to the list to which the element is to be added.
 *
 *  @ret    None.
 *
 *  @enter  The list parameter must be valid pointer.
 *
 *  @leave  None.
 *
 *  @see    LDRV_MPLIST_getHead (), LDRV_MPLIST_clear ()
 *  ============================================================================
 */
EXPORT_API
Bool
LDRV_MPLIST_isEmpty (IN     ProcessorId dspId,
                     IN     List *      list ) ;

/** ============================================================================
 *  @func   LDRV_MPLIST_getMemInfo
 *
 *  @desc   This function returns the memory information for the MPLIST region.
 *
 *  @arg    mplistMemInfo
 *              Placeholder for returning the memory information related to
 *              MPLIST module
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPLIST_getMemInfo (IN OUT MPLIST_MemInfo * mplistMemInfo) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* if !defined (LDRV_MPLIST_H) */
