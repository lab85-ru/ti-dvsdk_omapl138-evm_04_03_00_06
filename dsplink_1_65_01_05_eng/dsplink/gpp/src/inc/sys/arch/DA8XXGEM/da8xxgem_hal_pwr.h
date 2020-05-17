/** ============================================================================
 *  @file   da8xxgem_hal_pwr.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DA8XXGEM/
 *
 *  @desc   Hardware Abstraction Layer for Power Controller (PWR)
 *          module in Primus. Declares necessary functions for
 *          PSC request handling.
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


#if !defined (DA8XXGEM_HAL_PWR_H)
#define DA8XXGEM_HAL_PWR_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <da8xxgem_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   DA8XXGEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
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
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command .
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
DA8XXGEM_halPwrCtrl (IN         Pvoid          halObj,
                     IN         DSP_PwrCtrlCmd cmd,
                     IN OUT     Pvoid          arg) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DA8XXGEM_HAL_PWR_H) */
