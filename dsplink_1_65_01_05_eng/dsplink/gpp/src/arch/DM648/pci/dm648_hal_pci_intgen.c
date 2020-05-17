/** ============================================================================
 *  @file   dm648_hal_pci_intgen.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM648/pci/
 *
 *  @desc   Interrupt Generation module.
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
#include <dm648_hal_pci_intgen.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_INTGEN


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_INTGEN, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/** ============================================================================
 *  @func   DM648_halPciIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halPciIntCtrl (IN         Pvoid          halObj,
                     IN         DSP_IntCtrlCmd cmd,
                     IN         Uint32         intId,
                     IN OUT     Pvoid          arg)
{
    DSP_STATUS        status  = DSP_SOK ;
    DM648_HalObj  *   halObject = (DM648_HalObj *) halObj ;
    DM648_pciRegs  *  pciRegs = NULL    ;

    TRC_3ENTER ("DM648_halPciIntCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObject) ;

    pciRegs  = (DM648_pciRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                   + DM648_PCIREG_BASE
                                   - DM648_BAR2_BASE) ;
    switch (cmd) {
        case DSP_IntCtrlCmd_Enable:
        {
            pciRegs->PCIHINTSET |= DM648_SOFTINT0_MASK ;
        }
        break ;

        case DSP_IntCtrlCmd_Disable:
        {
            pciRegs->PCIHINTCLR |= DM648_SOFTINT0_MASK ;
        }
        break ;

        case DSP_IntCtrlCmd_Send:
        {
            pciRegs->PCIBINTSET |= DM648_SOFTINT1_MASK ;
            pciRegs->PCISTATSET |= DM648_SOFTINT1_MASK ;
        }
        break ;

        case DSP_IntCtrlCmd_Clear:
        {
            pciRegs->PCIHINTCLR |= DM648_SOFTINT0_MASK ;
            pciRegs->PCISTATCLR |= DM648_SOFTINT0_MASK ;
            pciRegs->PCIHINTSET |= DM648_SOFTINT0_MASK ;
        }
        break ;

        case DSP_IntCtrlCmd_Check:
        {
            if (   (pciRegs->PCICSRMIR & DM648_INTSTATUS_MASK)
                == (DM648_INTSTATUS_MASK)) {
                *((Bool *) arg) = TRUE ;
            }
        }
        break ;

        case DSP_IntCtrlCmd_GetIntId:
        {
            *((Uint32 *) arg) = halObject->pci.irqId ;
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

    TRC_1LEAVE ("DM648_halPciIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
