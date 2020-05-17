/** ============================================================================
 *  @file   drx416gem_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DRX416GEM/
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
#include <drx416gem_hal.h>
#include <drx416gem_hal_pwr.h>


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
 *  @const  LPSC_GEM
 *
 *  @desc   GEM module number in PSC.
 *  ============================================================================
 */
#define LPSC_GEM             1u

/*  ============================================================================
 *  @const  EPCPR/PTCMD/PTSTAT/PTSTAT1/PDCTL/PDCTL1/MDSTAT_DSP/MDCTL_DSP
 *
 *  @desc   Offset of the PSC module registers from the base of the CFG
 *          memory.
 *  ============================================================================
 */

#define EPCPR               0x0070u
#define PTCMD               0x0120u
#define PTSTAT              0x0128u
#define PDSTAT              0x0200u
#define PDSTAT1             0x0204u
#define PDCTL               0x0300u
#define PDCTL1              0x0304u
#define MDSTAT_DSP          (0x0800u + (4u * LPSC_GEM))
#define MDCTL_DSP           (0x0A00u + (4u * LPSC_GEM))
#define GBLCTL              0x0010u

/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the ARM Auxilary Control Register 0 register from the base
 *          of the CFG memory.
 *  ============================================================================
 */
#define CFG_ARMAUX_CTL0     0x14Cu

/*  ============================================================================
 *  @const  OFFSET_PSC_MODULE
 *
 *  @desc   Offset of the PSC module register from the base of the CFG memory.
 *  ============================================================================
 */
#define  OFFSET_PSC_MODULE   0xA0000u

/*  ============================================================================
 *  @const  CFG_SUSPSRC
 *
 *  @desc   Offset of the register to transfering suspend source to DSP
 *  ============================================================================
 */
#define CFG_SUSPSRC         0x0Cu

/** ============================================================================
 *  @func   DRX416GEM_halPscEnable
 *
 *  @desc   Enables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRX416GEM_halPscEnable (IN Uint32 addr)
{
    DSP_STATUS status    = DSP_SOK ;
    Uint32     domainOn  = 0       ;

    TRC_1ENTER ("DRX416GEM_halPscEnable", addr) ;

    /* Enable LPSC GEM Module */
    REG (addr + MDCTL_DSP) = REG (addr + MDCTL_DSP) | 0x3u ;

    /* Check whether the domain is up or not. */
    domainOn = REG (addr + PDSTAT) & 0x1u ;

    if (domainOn == 0u) {
        /* If domain is not powered, power up the domain. */
        REG (addr + PDCTL) = REG (addr + PDCTL) | 0x1u ;

        /* Program goctl to start transition sequence for LPSCs. */
        REG (addr + PTCMD) = 1u ;

        /* Wait for GOSTAT = NO TRANSITION from PTSTAT. */
        while ((REG (addr + PTSTAT) & 0x1u) != 0u)
        {
            /* Added for MISRAC compliance */
        }
    }
    else {
        /* Program goctl to start transition sequence for LPSCs. */
        REG (addr + PTCMD) = 1u ;

        /* Wait for GOSTAT = NO TRANSITION from PTSTAT. */
        while (((REG (addr + PTSTAT)) & 0x1u) != 0u)
        {
            /* Added for MISRAC compliance */
        }
    }

    /* Wait for MODSTAT = ENABLE from LPSC. */
    while ((REG (addr + MDSTAT_DSP) & 0x1Fu) != 0x3u)
    {
        /* Added for MISRAC compliance */
    }

    TRC_1LEAVE ("DRX416GEM_halPscEnable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DRX416GEM_halPscDisable
 *
 *  @desc   Disables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRX416GEM_halPscDisable (IN Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DRX416GEM_halPscDisable", addr) ;

    /* Asert Force Bit */
    REG (addr + GBLCTL) = REG (addr + GBLCTL) | 0x1u ;

    /* Set OFF as next power domain state */
    REG (addr + PDCTL1) = REG (addr + PDCTL1) & ~0x1 ;

    /* Disable GEM LPSC */
    REG (addr + MDCTL_DSP) = REG (addr + MDCTL_DSP) & ~0x2 ;
    REG (addr + MDCTL_DSP) = REG (addr + MDCTL_DSP) | 0x1u ;

    /* Start power domain OFF transition */
    REG (addr + PTCMD) = 1u ;

    /* Clear EPCGOOD flag */
    REG (addr + PDCTL1) = REG (addr + PDCTL1) & ~0x100 ;

    /* Wait for GOSTAT = NO TRANSITION from PTSTAT. */
    while (((REG (addr + PTSTAT)) & 0x1u) != 0u)
    {
        /* Added for MISRAC compliance */
    }

    /* De-assert FORCE bit */
    REG (addr + GBLCTL) = REG (addr + GBLCTL) & ~0x1 ;

    TRC_1LEAVE ("DRX416GEM_halPscDisable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DRX416GEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DRX416GEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    DRX416GEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;
    Uint32             lockAddr          ;
    Uint32             tAddr          ;
    Uint32             origArmauxCtl     ;
    Uint32             i = 0 ;

    TRC_3ENTER ("DRX416GEM_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DRX416GEM_HalObj *) halObj ;

    addr      =  halObject->baseCfgBus + OFFSET_PSC_MODULE ;
    lockAddr  =  halObject->baseCfgBus
               + halObject->offsetSysModule
               + CFG_ARMAUX_CTL0 ;

    /* Disable PLL & PSC lock */
    origArmauxCtl = REG(lockAddr);
    REG(lockAddr) &= 0xFFF0FFFFu ;
    REG(lockAddr) |= 0x00090000u ;

    for (i = 0; i < 1000000; i++) {
    }

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            DRX416GEM_halPscEnable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            DRX416GEM_halPscDisable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
            /* Transfering suspend source to DSP */
            tAddr =   halObject->baseCfgBus 
                    + halObject->offsetSysModule 
                    + CFG_SUSPSRC ;
            REG (tAddr) = 0x3CF9C1FF ;

            /* Reset the DSP and also reset GEM */
            REG (addr + MDCTL_DSP) &= 0xFEFF ;

        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
           /* Release the DSP from reset. */
           REG (addr + MDCTL_DSP) |= 0x0100 ;

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

    /* Enable PLL & PSC lock */
    REG(lockAddr) = origArmauxCtl;

    TRC_1LEAVE ("DRX416GEM_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
