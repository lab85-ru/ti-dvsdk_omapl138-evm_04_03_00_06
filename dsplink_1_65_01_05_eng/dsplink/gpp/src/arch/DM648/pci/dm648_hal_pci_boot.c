/** ============================================================================
 *  @file   dm648_hal_pci_boot.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM648/pci/
 *
 *  @desc   BOOT control module.
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
#include <dm648_hal.h>
#include <dm648_hal_pci.h>
#include <dm648_hal_pci_boot.h>


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
#define SET_FAILURE_REASON   TRC_SetReason (status,FID_C_ARCH_HAL_BOOT,__LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   DM648_halPciBootCtrl
 *
 *  @desc   Boot controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halPciBootCtrl (IN         Pvoid           halObj,
                      IN         DSP_BootCtrlCmd cmd,
                      IN OUT     Pvoid           arg)
{
    DSP_STATUS        status  = DSP_SOK ;
    DM648_devRegs *   devRegs = NULL ;
    DM648_HalObj *    halObject = (DM648_HalObj *) halObj ;

    TRC_3ENTER ("DM648_halPciBootCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObject) ;

    devRegs  = (DM648_devRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                   + DM648_DEVREG_BASE - DM648_BAR2_BASE) ;
    switch (cmd) {
        case DSP_BootCtrlCmd_SetEntryPoint:
        {
            devRegs->DSPBOOTADDR = (Uint32) arg ;
        }
        break ;

        case DSP_BootCtrlCmd_SetBootComplete:
        {
            devRegs->BOOTCMPLT |= DM648_BOOTCMPLTBC_MASK ;
        }
        break ;

        case DSP_BootCtrlCmd_ResetBootComplete:
        {
            devRegs->BOOTCMPLT &= ~DM648_BOOTCMPLTBC_MASK ;
        }
        break ;

        default:
        {
            /* Unsupported interrupt control command */
            status = DSP_EINVALIDARG ;
        }
        break ;
    }

    TRC_1LEAVE ("DM648_halPciBootCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
