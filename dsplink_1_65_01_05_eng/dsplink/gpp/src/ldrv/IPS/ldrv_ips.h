/** ============================================================================
 *  @file   ldrv_ips.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/IPS/
 *
 *  @desc   Defines interfaces exposed by LDRV_IPS subcomponent.
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


#if !defined (LDRV_IPS_H)
#define LDRV_IPS_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  @func   LDRV_IPS_moduleInit
 *
 *  @desc   This function initializes the LDRV_IPS module.
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
 *  @see    LDRV_IPS_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_IPS_moduleExit
 *
 *  @desc   This function finalizes the LDRV_IPS module.
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
 *  @see    LDRV_IPS_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_IPS_init
 *
 *  @desc   This function allocates and initializes resources used by this
 *          component.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *  @arg    shDspAddr
 *              Location to receive the DSP address of the shared memory control
 *              region used by the LDRV IPS component.
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
 *  @leave  None.
 *
 *  @see    LDRV_IPS_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_init (IN ProcessorId dspId, OUT Uint32 * shDspAddr) ;


/** ============================================================================
 *  @func   LDRV_IPS_exit
 *
 *  @desc   This function finalizes the LDRV IPS module for a particular DSP.
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
 *  @leave  None.
 *
 *  @see    LDRV_IPS_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_IPS_exit (IN ProcessorId dspId) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_IPS_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    None.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_IPS_debug (IN  ProcessorId  dspId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_IPS_H) */
