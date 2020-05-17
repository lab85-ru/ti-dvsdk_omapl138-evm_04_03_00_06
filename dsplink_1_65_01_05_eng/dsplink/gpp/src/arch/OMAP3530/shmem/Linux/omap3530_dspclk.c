/** ============================================================================
 *  @file   omap3530_dspclk.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP3530/shmem/Linux/
 *
 *  @desc   Implementation of dsp clk functionality.
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


/*  ----------------------------------- OS Specific Headers         */
#include <linux/clk.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_DSPCLK

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status,FID_C_ARCH_DSPCLK,__LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   OMAP3530_getDspClkRate
 *
 *  @desc   Get DSP clock frequency
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
OMAP3530_getDspClkRate (IN Void  * dspIdentifier, OUT Uint32 * cpuFreq)
{
    DSP_STATUS  status    = DSP_SOK ;
    struct      clk * clk = NULL ;
    Char8 *     dspId     = NULL ;

    TRC_2ENTER ("OMAP3530_getDspClkRate", dspIdentifier, cpuFreq) ;

    DBC_Require (dspIdentifier != NULL) ;
    DBC_Require (cpuFreq != NULL) ;

    dspId = (Char8 *) dspIdentifier ;

    clk = clk_get (NULL, dspId);
    if (clk == NULL) {
        status = DSP_EFAIL ;
        SET_FAILURE_REASON ;
    }
    else {
        *cpuFreq = (clk_get_rate (clk)/1000) ;
    }

    TRC_1LEAVE ("OMAP3530_getDspClkRate", status) ;

    return status ;
}




#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
