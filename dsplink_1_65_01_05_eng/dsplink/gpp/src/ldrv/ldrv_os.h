/** ============================================================================
 *  @file   ldrv_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/
 *
 *  @desc   Defines constants and interfaces for the OS-specific functionality
 *          for the LDRV component.
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


#if !defined (LDRV_OS_H)
#define LDRV_OS_H


/*  ----------------------------------- DSP/BIOS LINK Headers       */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_getLinkCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies only GPP object.
 *
 *  @arg    linkCfg
 *              Pointer to the user-side configuration object.
 *  @arg    knlLinkCfg
 *              Location to receive the pointer to the kernel-side configuration
 *              object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  linkCfg must be a valid pointer.
 *          knlLinkCfg must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkGppCfg (IN  LINKCFG_Object *  linkCfg,
                 OUT LINKCFG_Object ** knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_getLinkDspCfg
 *
 *  @desc   Gets the pointer to kernel configuration structure after creating
 *          it (if required). Copies a DSP object.
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    dspCfg
 *              Pointer to the user-side configuration object.
 *  @arg    knlLinkCfg
 *              Location to receive the pointer to the kernel-side configuration
 *              object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  linkCfg must be a valid pointer.
 *          knlLinkCfg must be a valid pointer.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_getLinkDspCfg (IN  ProcessorId         procId,
                    IN  LINKCFG_DspConfig * dspCfg,
                    OUT LINKCFG_Object *    knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_freeLinkGppCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure. Frees only GPP object
 *
 *  @arg    knlLinkCfg
 *              Pointer to the kernel-side configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
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
LDRV_freeLinkGppCfg (IN  LINKCFG_Object *  knlLinkCfg) ;


/** ============================================================================
 *  @func   LDRV_freeLinkGppCfg
 *
 *  @desc   Frees any memory allocated for the kernel-side DSPLINK configuration
 *          structure. Frees only GPP object
 *
 *  @arg    procId
 *              Processor Identifier.
 *  @arg    knlLinkCfg
 *              Pointer to the kernel-side configuration object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_ECONFIG
 *              Incorrect configuration.
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
LDRV_freeLinkDspCfg (IN  ProcessorId       procId,
                     IN  LINKCFG_Object *  knlLinkCfg) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_OS_H) */
