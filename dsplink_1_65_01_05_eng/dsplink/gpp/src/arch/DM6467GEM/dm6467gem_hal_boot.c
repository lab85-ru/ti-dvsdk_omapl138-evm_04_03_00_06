/** ============================================================================
 *  @file   dm6467gem_hal_boot.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6467GEM/
 *
 *  @desc   Boot control module.
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


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6467gem_hal.h>
#include <dm6467gem_hal_boot.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_BOOT


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_BOOT, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the DSP BOOT ADDR register from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define BOOT_BASE_ADDR      0x08


/** ============================================================================
 *  @func   DM6467GEM_halBootCtrl
 *
 *  @desc   Boot controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_halBootCtrl (IN         Pvoid           halObj,
                        IN         DSP_BootCtrlCmd cmd,
                        IN OUT     Pvoid           arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    DM6467GEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;

    TRC_3ENTER ("DM6467GEM_halBootCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DM6467GEM_HalObj *) halObj ;
    addr  =  halObject->baseCfgBus
           + halObject->offsetSysModule
           + BOOT_BASE_ADDR ;

    switch (cmd) {
        case DSP_BootCtrlCmd_SetEntryPoint:
        {
            REG (addr) = (Uint32) arg ;
        }
        break ;

        case DSP_BootCtrlCmd_SetBootComplete:
        {
            /* Do nothing here for DavinciHD gem */
        }
        break ;

        case DSP_BootCtrlCmd_ResetBootComplete:
        {
            /* Do nothing here for DavinciHD gem */
        }
        break ;

        default:
        {
            /* Unsupported interrupt control command */
            status = DSP_EINVALIDARG ;
            SET_FAILURE_REASON ;
        }
        break ;
    }

    TRC_1LEAVE ("DM6467GEM_halBootCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
