/** ============================================================================
 *  @file   dm6437_hal_vlynq_intgen.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/vlynq/
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

#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>
#include <dm6437_hal_vlynq_intgen.h>


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
 *  @func   DM6437_halVlynqIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halVlynqIntCtrl (IN         Pvoid          halObj,
                        IN         DSP_IntCtrlCmd cmd,
                        IN         Uint32         intId,
                        IN OUT     Pvoid          arg)
{
    DSP_STATUS         status    = DSP_SOK ;
    DM6437_HalObj *    halObject = (DM6437_HalObj *) halObj ;
    DM6437_pciRegs *   pciRegs   = NULL    ;
    DM6437_vlynqRegs * vlynqRegs = NULL    ;

    TRC_3ENTER ("DM6437_halVlynqIntCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObject) ;

    pciRegs    = (DM6437_pciRegs *) (  halObject->vlynq.region3Addr
                                     + DM6437_PCIREG_BASE) ;
    vlynqRegs  = (DM6437_vlynqRegs *) halObject->vlynq.ctrlBaseAddr ;

    switch (cmd) {
        case DSP_IntCtrlCmd_Enable:
        {
        }
        break ;

        case DSP_IntCtrlCmd_Disable:
        {
        }
        break ;

        case DSP_IntCtrlCmd_Send:
        {
            /* here we use PCI interrupt on DM6437 */
            pciRegs->PCIBINTSET |= DM6437_SOFTINT1_MASK ;
            pciRegs->PCISTATSET |= DM6437_SOFTINT1_MASK ;
        }
        break ;

        case DSP_IntCtrlCmd_Clear:
        {
            /* Clear vector 31u */
            vlynqRegs->LOCAL_INTSTATCLR |= 0x80000000u ;
        }
        break ;

        case DSP_IntCtrlCmd_Check:
        {
            if (vlynqRegs->LOCAL_INTSTATCLR & 0x80000000u) {
                *((Bool *) arg) = TRUE ;
            }
        }
        break ;

        case DSP_IntCtrlCmd_GetIntId:
        {
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

    TRC_1LEAVE ("DM6437_halVlynqIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
