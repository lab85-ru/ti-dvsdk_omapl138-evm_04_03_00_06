/** ============================================================================
 *  @file   omap3530_dspclk.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP3530/
 *
 *  @desc   Defines the interfaces and data structures for the DSPCLK access.
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


#if !defined (OMAP3530DSPCLK_H)
#define OMAP3530DSPCLK_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   OMAP3530_getDspClkRate
 *
 *  @desc   Get DSP clock frequency
 *
 *  @arg    dspIdentifier
 *              String which describes DSP
 *  @arg    cpuFreq
 *              DSP clock frequency
 *              number of bytes to copy.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Failed to copy.
 *
 *  @enter  dspIdentifier, cpuFreq must be valid.
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OMAP3530_getDspClkRate (IN Void * dspIdentifier, OUT Uint32 * cpuFreq);




#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !define (OMAP3530DSPCLK_H) */
