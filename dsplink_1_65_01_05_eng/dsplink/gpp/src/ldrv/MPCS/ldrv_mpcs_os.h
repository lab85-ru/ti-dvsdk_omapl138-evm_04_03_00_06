/** ============================================================================
 *  @file   ldrv_mpcs_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/MPCS/
 *
 *  @desc   Defines the interfaces and data structures for the MPCS - (Multi
 *          Processor Critical Section) sub-component.
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


#if !defined (LDRV_MPCS_OS_H)
#define LDRV_MPCS_OS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_MPCS_OS_moduleInit
 *
 *  @desc   This function initializes the LDRV_MPCS module.
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
 *  @see    LDRV_MPCS_OS_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_OS_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPCS_OS_moduleExit
 *
 *  @desc   This function finalizes the LDRV_MPCS module.
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
 *  @see    LDRV_MPCS_OS_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_OS_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPCS_OS_create
 *
 *  @desc   This function creates and initializes an instance of the MPCS
 *          object.
 *          The memory for the object is provided by the user.
 *
 *  @arg    dspId
 *              ID of the processor with which the MPCS object is to be shared.
 *  @arg    mpcsObj
 *              Pointer to the shared MPCS object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          mpcsObj must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MPCS_OS_delete ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_create (IN     ProcessorId    dspId,
                     IN     MPCS_ShObj *   mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_OS_delete
 *
 *  @desc   This function deletes the instance of the MPCS object.
 *
 *  @arg    dspId
 *              ID of the processor with which the MPCS is shared.
 *  @arg    mpcsObj
 *              Pointer to the MPCS object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Invalid argument.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *          name must be valid.
 *
 *  @leave  None
 *
 *  @see    LDRV_MPCS_OS_create ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_delete (IN     ProcessorId  dspId,
                     IN     MPCS_ShObj * mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_OS_enter
 *
 *  @desc   This function enters the critical section specified by the MPCS
 *          object.
 *
 *  @arg    mpcsObj
 *              Pointer to the MPCS object.
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
 *  @see    LDRV_MPCS_OS_leave ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_enter (IN     MPCS_ShObj * mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_OS_leave
 *
 *  @desc   This function leaves the critical section specified by the MPCS
 *          object.
 *
 *  @arg    mpcsObj
 *              Pointer to the MPCS object.
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
 *  @see    LDRV_MPCS_OS_enter ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_OS_leave (IN     MPCS_ShObj * mpcsObj) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_MPCS_H) */
