/** ============================================================================
 *  @file   da8xxgem_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DA8XXGEM/
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
#include <da8xxgem_hal.h>
#include <da8xxgem_hal_pwr.h>


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
 *  @const  PSC_ENABLE\PSC_DISABLE\PSC_SYNCRESET\PSC_SWRSTDISABLE
 *
 *  @desc   States of PSC.
 *  ============================================================================
 */
#define PSC_ENABLE          (0x3)
#define PSC_DISABLE         (0x2)
#define PSC_SYNCRESET       (0x1)
#define PSC_SWRSTDISABLE    (0x0)

/*  ============================================================================
 *  @const  LPSC_GEM
 *
 *  @desc   GEM module number in PSC.
 *  ============================================================================
 */
#define LPSC_GEM            15u

/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the HOST1CFG register from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define BOOT_BASE_ADDR      0x44

/*  ============================================================================
 *  @const  DSP_BT/EPCPR/PTCMD/PTSTAT/PTSTAT1/PDCTL/PDCTL1/MDSTAT_DSP/MDCTL_DSP
 *
 *  @desc   Offset of the PSC module registers from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define EPCPR               0x0070
#define EPCCR               0x0078
#define PTCMD               0x0120
#define PTSTAT              0x0128
#define PDSTAT              0x0200
#define PDSTAT1             0x0204
#define PDCTL               0x0300
#define PDCTL1              0x0304
#define MDSTAT_DSP          0x0800 + (4 * LPSC_GEM)
#define MDCTL_DSP           0x0A00 + (4 * LPSC_GEM)


/** ============================================================================
 *  @func   DA8XXGEM_halPscEnable
 *
 *  @desc   Enables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halPscEnable (IN  Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     domainBit = 2 ;/* DSP power domain is 1*/

    TRC_1ENTER ("DA8XXGEM_halPscEnable", addr) ;

    // Wait for any outstanding transition to complete
    while( REG (addr + PTSTAT) & domainBit) {
        /* Added for MISRAC compliance */
    }

    /*If we are already in that state, just return */
    if((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_ENABLE) {
        /* Perform transition  */
        REG (addr +MDCTL_DSP) = (   (REG (addr +MDCTL_DSP)
                                 &  (0xFFFFFFE0)) | PSC_ENABLE ) ;
        REG (addr + PTCMD) |= domainBit ;

        /* Wait for transition to complete */
        while(( REG (addr + PTSTAT) & domainBit )) {
            /* Added for MISRAC compliance */
        }

        /* Wait and verify the state */
        while ((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_ENABLE) {
            /* Added for MISRAC compliance */
        }
    }

    TRC_0LEAVE ("DA8XXGEM_halPscEnable") ;

    return status ;
}


/** ============================================================================
 *  @func   DA8XXGEM_halPscDisable
 *
 *  @desc   Disables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halPscDisable (IN  Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     domainBit = 2 ; /* DSP power domain is 1*/

    TRC_1ENTER ("DA8XXGEM_halPscDisable", addr) ;

    /* Wait for any outstanding transition to complete */
    while( REG (addr + PTSTAT) & domainBit) {
        /* Added for MISRAC compliance */
    }

    /*If we are already in that state, just return */
    if((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_DISABLE) {
        /* Perform transition  */
        REG (addr +MDCTL_DSP) = (   (REG (addr +MDCTL_DSP)
                                 &  (0xFFFFFFE0)) | PSC_DISABLE ) ;
        REG (addr + PTCMD) |= domainBit ;

        /* Wait for transition to complete */
        while( REG (addr + PTSTAT) & domainBit ) {
            /* Added for MISRAC compliance */
        }

        /* Wait and verify the state */
        while ((REG (addr +MDSTAT_DSP) & 0x1F) != PSC_DISABLE) {
            /* Added for MISRAC compliance */
        }
    }

    TRC_0LEAVE ("DA8XXGEM_halPscDisable") ;

    return (status) ;
}


/** ============================================================================
 *  @func   DA8XXGEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status    = DSP_SOK ;
    DA8XXGEM_HalObj *  halObject = NULL    ;
    Uint32             addr                ;

    TRC_3ENTER ("DA8XXGEM_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DA8XXGEM_HalObj *) halObj ;

    if (halObject != NULL) {

        addr  =  halObject->baseCfgBus + halObject->offsetPsc0  ;

        switch (cmd) {
            case DSP_PwrCtrlCmd_PowerUp:
            {
                status = DA8XXGEM_halPscEnable (addr) ;
            }
            break ;

            case DSP_PwrCtrlCmd_PowerDown:
            {
                status = DA8XXGEM_halPscDisable (addr) ;
            }
            break ;

            case DSP_PwrCtrlCmd_Reset:
            {
                /* Assert DSP local reset  for primus*/
                CLEAR_BIT (REG (addr + MDCTL_DSP), 8) ;
            }
            break ;

            case DSP_PwrCtrlCmd_Release:
            {
                 /* Release the DSP from locae reset. */
                SET_BIT (REG (addr + MDCTL_DSP), 8) ;
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

    }

    TRC_1LEAVE ("DA8XXGEM_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
