/** ============================================================================
 *  @file   omap3530_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP3530/
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
#include <omap3530_hal.h>
#include <omap3530_hal_pwr.h>


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


/*  ============================================================================
 *  @const  RM_RSTCTRL_DSP_OFFSET
 *
 *  @desc   Offsets from the IVA2 PRM base address
 *  ============================================================================
  */
#define RM_RSTCTRL_DSP_OFFSET       0x50

/*  ============================================================================
 *  @const  CM_FCLKEN_IVA2_OFFSET,
 *          CM_CLKSTCTRL_IVA2_OFFSET
 *
 *  @desc   Offsets from the IVA2 PRM base address
 *  ============================================================================
  */
#define CM_FCLKEN_IVA2_OFFSET       0x00
#define CM_CLKSTCTRL_IVA2_OFFSET    0x48


/*  ============================================================================
 *  @const  CM_FCLKEN_PER_OFFSET,
 *          CM_ICLKEN_PER_OFFSET,
 *          CM_CLKSEL_PER_OFFSET
 *
 *  @desc   Offsets from the PER_CM base address
 *  ============================================================================
  */
#define CM_FCLKEN_PER_OFFSET        0x00
#define CM_ICLKEN_PER_OFFSET        0x10
#define CM_CLKSEL_PER_OFFSET        0x40


/** ============================================================================
 *  @func   OMAP3530_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    OMAP3530_HalObj * halObject = NULL  ;

    TRC_3ENTER ("OMAP3530_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (OMAP3530_HalObj *) halObj ;

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            /* Do nothing here for Omap3530 Gem */
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            /* Do nothing here for Omap3530 Gem */
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
            /* Disable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->perCmBase + CM_FCLKEN_PER_OFFSET), 6) ;

            /* Disable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->perCmBase + CM_ICLKEN_PER_OFFSET), 6) ;

            /* Disable automatic transitions. */
            REG (halObject->iva2CmBase + CM_CLKSTCTRL_IVA2_OFFSET) = 0 ;

            /* Turn off IVA2 functional clock */
            REG (halObject->iva2CmBase + CM_FCLKEN_IVA2_OFFSET) = 0 ;

            /* Reset RST3_IVA2 and RST1_IVA2 */
            REG (halObject->iva2PrmBase + RM_RSTCTRL_DSP_OFFSET) =  5 ;

            /* Turn on IVA2 functional clock */
            REG (halObject->iva2CmBase + CM_FCLKEN_IVA2_OFFSET) = 1 ;

            /* Enable automatic transitions. */
            REG (halObject->iva2CmBase + CM_CLKSTCTRL_IVA2_OFFSET) = 3 ;

        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            /* Enable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT (REG (halObject->perCmBase + CM_FCLKEN_PER_OFFSET), 6) ;

            /* Enable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT (REG (halObject->perCmBase + CM_ICLKEN_PER_OFFSET), 6) ;

            /* Setting source to Sys_clk */
            SET_BIT (REG (halObject->perCmBase + CM_CLKSEL_PER_OFFSET), 3) ;

            /* Release DSP via RST1_IVA2 */
            CLEAR_BIT (REG (halObject->iva2PrmBase + RM_RSTCTRL_DSP_OFFSET), 0) ;

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

    TRC_1LEAVE ("OMAP3530_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
