/** ============================================================================
 *  @file   ldrv_ringio.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/RINGIO/
 *
 *  @desc   Low level functions for RingIO module.
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


#if !defined (LDRV_RINGIO_H)
#define LDRV_RINGIO_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>
#include <_ringiodefs.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_RINGIO_moduleInit
 *
 *  @desc   This function initializes the LDRV_RINGIO module.
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
 *  @see    LDRV_RINGIO_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_RINGIO_moduleExit
 *
 *  @desc   This function finalizes the LDRV_RINGIO module.
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
 *  @see    LDRV_RINGIO_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_RINGIO_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the RINGIO component.
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
 *  @see    LDRV_RINGIO_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_RINGIO_exit
 *
 *  @desc   This function finalizes the LDRV RINGIO module for a particular DSP.
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
 *  @see    LDRV_RINGIO_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_RINGIO_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_RINGIO_getMemInfo
 *
 *  @desc   This function returns the memory information for the RINGIO region.
 *
 *  @arg    ringIoMemInfo
 *              Placeholder for returning the memory information related to
 *              RINGIO module
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
LDRV_RINGIO_getMemInfo (IN OUT RingIO_MemInfo * ringIoMemInfo) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* if !defined (LDRV_RINGIO_H) */
