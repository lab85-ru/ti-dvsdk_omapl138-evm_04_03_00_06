/** ============================================================================
 *  @file   omap2530_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP2530/
 *
 *  @desc   Power management module.
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
#include <omap2530_hal.h>
#include <omap2530_hal_pwr.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_PWR


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_PWR, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  =========================================================================
===
 *  @const  RM_RSTCTRL_DSP_OFFSET
 *
 *  @desc   Offsets from the IVA2 PRM base address
 *  =========================================================================
===
 */
#define RM_RSTCTRL_DSP_OFFSET       0x850

/*  =========================================================================
===
 *  @const  CM_FCLKEN_DSP_OFFSET,
 *          CM_IDLEST_DSP_OFFSET,
 *          CM_CLKSTCTRL_DSP_OFFSET
 *
 *  @desc   Offsets from the IVA2 PRM base address
 *  =========================================================================
===
 */
#define CM_FCLKEN_DSP_OFFSET       0x800
#define CM_ICLKEN_DSP_OFFSET       0x810
#define CM_IDLEST_DSP_OFFSET       0x820
#define CM_CLKSTCTRL_DSP_OFFSET    0x848


/*  =========================================================================
===
 *  @const  CM_FCLKEN_PER_OFFSET,
 *          CM_ICLKEN_PER_OFFSET,
 *          CM_CLKSEL_PER_OFFSET
 *
 *  @desc   Offsets from the PER_CM base address
 *  =========================================================================
===
 */
#define CM_FCLKEN_PER_OFFSET        0x00
#define CM_ICLKEN_PER_OFFSET        0x10
#define CM_CLKSEL_PER_OFFSET        0x40

/*  =========================================================================
===
 *  @const  CM_FCLKEN1_CORE_OFFSET,
 *          CM_FCLKEN2_CORE_OFFSET,
 *          CM_ICLKEN1_CORE_OFFSET,
 *          CM_ICLKEN2_CORE_OFFSET,
 *          CM_CLKSEL2_CORE_OFFSET
 *
 *
 *  @desc   Offsets from the PRCM_BASE address
 *  =========================================================================
===
 */
#define CM_FCLKEN1_CORE_OFFSET      0x200
#define CM_FCLKEN2_CORE_OFFSET      0x204
#define CM_ICLKEN1_CORE_OFFSET      0x210
#define CM_ICLKEN2_CORE_OFFSET      0x214
#define CM_CLKSEL2_CORE_OFFSET      0x244


/** ============================================================================
 *  @func   OMAP2530_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    OMAP2530_HalObj * halObject = NULL  ;

    TRC_3ENTER ("OMAP2530_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (OMAP2530_HalObj *) halObj ;

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            /* Do nothing here for Omap2530 Gem */
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            /* Do nothing here for Omap2530 Gem */
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
             /* Disable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->prcmBase + CM_FCLKEN1_CORE_OFFSET), 7) ;

            /* Disable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->prcmBase + CM_ICLKEN1_CORE_OFFSET), 7) ;

            /* Disable automatic transitions. */
            REG (halObject->prcmBase + CM_CLKSTCTRL_DSP_OFFSET) = 0 ;

            /* Turn off DSP/IVA2 interface clock */
            REG (halObject->prcmBase + CM_ICLKEN_DSP_OFFSET) = 0 ;

            /* Turn off DSP/IVA2 functional clock */
            REG (halObject->prcmBase + CM_FCLKEN_DSP_OFFSET) = 0 ;

            /* Reset RST1_DSP (Megacell + EDMA) */
            /* Release RST2_DSP from reset      */
            REG (halObject->prcmBase + RM_RSTCTRL_DSP_OFFSET) = 1;

            /* Turn on DSP/IVA2 interface clock */
            REG (halObject->prcmBase + CM_ICLKEN_DSP_OFFSET) = 1 ;

            /* Turn on DSP/IVA2 functional clock */
            REG (halObject->prcmBase + CM_FCLKEN_DSP_OFFSET) = 1 ;

            /* Enable automatic transitions. */
            REG (halObject->prcmBase + CM_CLKSTCTRL_DSP_OFFSET) = 1 ;
        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            /* Enable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT ( REG (halObject->prcmBase + CM_ICLKEN1_CORE_OFFSET), 7) ;

            /* Enable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT ( REG (halObject->prcmBase + CM_FCLKEN1_CORE_OFFSET), 7) ;

            /* Setting source to Sys_clk */
            CLEAR_BITS (REG (halObject->prcmBase + CM_CLKSEL2_CORE_OFFSET), 0x300);
            SET_BIT (REG (halObject->prcmBase + CM_CLKSEL2_CORE_OFFSET), 8);

            /* Disable automatic transitions. */
            REG (halObject->prcmBase + CM_CLKSTCTRL_DSP_OFFSET) = 0 ;

            /* Release DSP via RST1_DSP */
            CLEAR_BIT (REG (halObject->prcmBase + RM_RSTCTRL_DSP_OFFSET), 0) ;

            /* Enable automatic transitions. */
            REG (halObject->prcmBase + CM_CLKSTCTRL_DSP_OFFSET) = 1 ;

            /* Check CM_IDLEST_DSP to see if DSP subsystem is in standby or active */
            if ( REG(halObject->prcmBase + CM_IDLEST_DSP_OFFSET) == 1 )
            {
                TRC_0PRINT(TRC_LEVEL1, "DSP subsystem is active.\n");
            }
            else
            {
                TRC_0PRINT(TRC_LEVEL1, "DSP subsystem is in standby.\n");
            }

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

    TRC_1LEAVE ("OMAP2530_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
