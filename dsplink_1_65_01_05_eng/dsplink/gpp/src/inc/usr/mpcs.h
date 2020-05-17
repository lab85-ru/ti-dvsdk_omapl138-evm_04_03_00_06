/** ============================================================================
 *  @file   mpcs.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/usr/
 *
 *  @desc   Defines the interfaces and data structures for the API sub-component
 *          MPCS for Multi-Processor Critical Section.
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


#if !defined (MPCS_H)
#define MPCS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


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
 *  @arg    procId
 *              ID of the processor with which the MPCS object is to be shared.
 *  @arg    name
 *              System-wide unique name for the MPCS object.
 *  @arg    mpcsShObj
 *              Pointer to the shared MPCS object.
 *              If memory for the MPCS object is provided by the user, the MPCS
 *              object handle is not NULL.
 *              Otherwise, if the memory is to be allocated by the MPCS
 *              component, the MPCS object handle can be specified as NULL.
 *  @arg    attrs
 *              Attributes for creation of the MPCS object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EALREADYEXISTS
 *              The specified MPCS name already exists.
 *          DSP_ERESOURCE
 *              All MPCS entries are currently in use.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          name must be valid.
 *          attrs must be valid.
 *
 *  @leave  None
 *
 *  @see    MPCS_delete ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_create (IN     ProcessorId    procId,
             IN     Pstr           name,
             IN     MPCS_ShObj *   mpcsShObj,
             IN     MPCS_Attrs *   attrs) ;


/** ============================================================================
 *  @func   MPCS_delete
 *
 *  @desc   This function deletes the instance of the MPCS object.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    name
 *              System-wide unique name for the MPCS object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_ENOTFOUND
 *              Specified MPCS object name does not exist.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          name must be valid.
 *
 *  @leave  None
 *
 *  @see    MPCS_create ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_delete (IN     ProcessorId procId,
             IN     Pstr        name) ;


/** ============================================================================
 *  @func   MPCS_open
 *
 *  @desc   This function opens an MPCS object specified by its name and gets a
 *          handle to the object.
 *          Every process that needs to use the MPCS object must get a handle to
 *          the object by calling this API.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS object is to be shared.
 *  @arg    name
 *              System-wide unique name for the MPCS object.
 *  @arg    mpcsHandle
 *              Location to receive the MPCS object handle, which is valid in
 *              the process space of the calling process.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SEXISTS
 *              The MPCS connection already exists.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_ENOTFOUND
 *              Specified MPCS object name does not exist.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          name must be valid.
 *          mpcsHandle must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    MPCS_close ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_open (IN     ProcessorId    procId,
           IN     Pstr           name,
           OUT    MPCS_Handle *  mpcsHandle) ;


/** ============================================================================
 *  @func   MPCS_close
 *
 *  @desc   This function closes an MPCS object specified by its handle.
 *
 *  @arg    procId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    mpcsHandle
 *              Handle to the MPCS object to be closed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_SFREE
 *              The last close for specified MPCS resulted in it getting closed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_ENOTFOUND
 *              Specified MPCS object name does not exist.
 *          DSP_EACCESSDENIED
 *              The MPCS component has not been initialized.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  procId must be valid.
 *          mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @see    MPCS_open ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_close (IN     ProcessorId    procId,
            IN     MPCS_Handle    mpcsHandle) ;


/** ============================================================================
 *  @func   MPCS_enter
 *
 *  @desc   This function enters the critical section specified by the MPCS
 *          object.
 *
 *  @arg    mpcsHandle
 *              Handle to the MPCS object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @see    MPCS_leave ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_enter (IN     MPCS_Handle mpcsHandle) ;


/** ============================================================================
 *  @func   MPCS_leave
 *
 *  @desc   This function leaves the critical section specified by the MPCS
 *          object.
 *
 *  @arg    mpcsHandle
 *              Handle to the MPCS object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  mpcsHandle must be valid.
 *
 *  @leave  None
 *
 *  @see    MPCS_enter ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
MPCS_leave (IN     MPCS_Handle mpcsHandle) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MPCS_H) */
