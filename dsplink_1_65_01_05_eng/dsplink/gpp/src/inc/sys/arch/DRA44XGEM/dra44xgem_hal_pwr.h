/** ============================================================================
 *  @file   dra44xgem_hal_pwr.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DRA44XGEM/
 *
 *  @desc   Hardware Abstraction Layer for Power Controller (PWR)
 *          module in Davinci. Declares necessary functions for
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


#if !defined (DRA44XGEM_HAL_PWR_H)
#define DRA44XGEM_HAL_PWR_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dra44xgem_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   DRA44XGEM_halPwrCtrl
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
DRA44XGEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DRA44XGEM_HAL_PWR_H) */
