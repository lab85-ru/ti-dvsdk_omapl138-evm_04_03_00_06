/** ============================================================================
 *  @file   drx416gem_hal_boot.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DRX416GEM/
 *
 *  @desc   Hardware Abstraction Layer for Davinci Boot controller.
 *          Defines the Platform specific HAL (Hardware Abstraction Layer)
 *          object.
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


#if !defined (DRX416GEM_HAL_BOOT_H)
#define DRX416GEM_HAL_BOOT_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   DRX416GEM_halBootCtrl
 *
 *  @desc   Boot controller.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *  @arg    cmd.
 *              Command.
 *  @arg    arg.
 *              Command specific arguments.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRX416GEM_halBootCtrl (IN         Pvoid           halObj,
                       IN         DSP_BootCtrlCmd cmd,
                       IN OUT     Pvoid           arg) ;

#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DRX416GEM_HAL_BOOT_H) */
