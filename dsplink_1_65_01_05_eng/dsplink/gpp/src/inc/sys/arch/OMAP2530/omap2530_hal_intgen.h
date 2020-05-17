/** ============================================================================
 *  @file   omap2530_hal_intgen.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP2530/
 *
 *  @desc   Declares necessary functions for Interrupt Handling.
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


#if !defined (OMAP2530_HAL_INTGEN_H)
#define OMAP2530_HAL_INTGEN_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <omap2530_hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @func   OMAP2530_halIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @arg    halObj.
 *              Pointer to HAL object.
 *  @arg    cmd.
 *              Command.
 *  @arg    intId.
 *              Interrupt Identifier.
 *  @arg    arg.
 *              Command specific arguments.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *            DSP_EINVALIDARG
 *              Unsupported interrupt control command.
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
OMAP2530_halIntCtrl (IN         Pvoid          halObj,
                      IN         DSP_IntCtrlCmd cmd,
                      IN         Uint32         intId,
                      IN OUT     Pvoid          arg) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP2530_HAL_INTGEN_H) */
