/** ============================================================================
 *  @file   ldrv_mpcs.h
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


#if !defined (LDRV_MPCS_H)
#define LDRV_MPCS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>
#include <mpcsdefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_MPCS_moduleInit
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
 *  @see    LDRV_MPCS_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPCS_moduleExit
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
 *  @see    LDRV_MPCS_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_MPCS_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the MPCS component.
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
 *  @see    LDRV_MPCS_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_MPCS_exit
 *
 *  @desc   This function finalizes the LDRV MPCS module for a particular DSP.
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
 *  @see    LDRV_MPCS_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_MPCS_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_MPCS_create
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
 *  @see    LDRV_MPCS_delete ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_create (IN     ProcessorId    dspId,
                  IN     MPCS_ShObj *   mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_delete
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
 *  @see    LDRV_MPCS_create ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_delete (IN     ProcessorId  dspId,
                  IN     MPCS_ShObj * mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_enter
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
 *  @see    LDRV_MPCS_leave ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_enter (IN     MPCS_ShObj * mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_leave
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
 *  @see    LDRV_MPCS_enter ()
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
LDRV_MPCS_leave (IN     MPCS_ShObj * mpcsObj) ;


/** ============================================================================
 *  @func   LDRV_MPCS_getMemInfo
 *
 *  @desc   This function returns the memory information for the MPCS region.
 *
 *  @arg    mpcsMemInfo
 *              Placeholder for returning the memory information related to
 *              MPCS module
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
LDRV_MPCS_getMemInfo (IN OUT MPCS_MemInfo * mpcsMemInfo) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_MPCS_H) */
