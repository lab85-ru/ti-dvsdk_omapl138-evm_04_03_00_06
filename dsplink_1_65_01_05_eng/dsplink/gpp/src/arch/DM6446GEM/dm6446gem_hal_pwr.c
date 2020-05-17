/** ============================================================================
 *  @file   dm6446gem_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6446GEM/
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
#include <dm6446gem_hal.h>
#include <dm6446gem_hal_pwr.h>


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
#define LPSC_GEM            39

/*  ============================================================================
 *  @const  PDNUM_GEM
 *
 *  @desc   Power domain number for GEm.
 *  ============================================================================
 */
#define PDNUM_GEM            1

/*  ============================================================================
 *  @const  DSP_BT/EPCPR/PTCMD/PTSTAT/PTSTAT1/PDCTL/PDCTL1/MDSTAT_DSP/MDCTL_DSP
 *
 *  @desc   Offset of the PSC module registers from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define DSP_BT              0x0014
#define EPCPR               0x1070
#define EPCCR               0x1078
#define PTCMD               0x1120
#define PTSTAT              0x1128
#define PDSTAT              0x1200
#define PDSTAT1             0x1204
#define PDCTL               0x1300
#define PDCTL1              0x1304
#define MDSTAT_DSP          0x1800 + (4 * LPSC_GEM)
#define MDCTL_DSP           0x1A00 + (4 * LPSC_GEM)
#define GBLCTL              0x1010

/*  ============================================================================
 *  @const  CHP_SHRTSW
 *
 *  @desc   Offset of the CHIP RAIL short register from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define CHP_SHRTSW          0x38

/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the DSP BOOT ADDR register from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define BOOT_BASE_ADDR      0x08


/** ============================================================================
 *  @func   DM6446GEM_halPscEnable
 *
 *  @desc   Enables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6446GEM_halPscEnable (IN  Uint32 addr)
{
    DSP_STATUS status    = DSP_SOK ;
    Uint32     domainbit = 2       ;

    TRC_1ENTER ("DM6446GEM_halPscEnable", addr) ;

    /* Check if the DSP is in host-boot mode */
    if (TEST_BIT (REG (addr + DSP_BT), 8) == 0u) {
        /* Check if the DSP module is disabled */
        if ((REG (addr + MDSTAT_DSP) & 0x001F) != 0x0003) {
            /* Wait for any pending state transtions to finish */
            while( (REG (addr + PTSTAT) & domainbit ) != 0 )
            {
                /* Added for MISRAC compliance */
            }

            /* Assert the force bit */
            REG (addr + GBLCTL) |= 0x1 ;

            /* Set DSP power domain next state to on. */
            REG (addr + PDCTL1) |= 0x1;

            /* Hold DSP module in softare reset disabled state. Workaround
             * for Bug DaVinci.BTS_Si_BUGS.105 */
            REG (addr + MDCTL_DSP) &= ~(0x1F);

            /* Clear the external power control pending indicator */
            REG (addr + EPCCR) = 0x2;

            /* Start state transition */
            REG (addr + PTCMD) = domainbit;

            /* Wait for external power request */
            while( (REG (addr + EPCPR) & domainbit ) == 0 )
            {
                /* Added for MISRAC compliance */
            }

            /* Call external power control to turn on external power supply. */
            /**** TBA ****/

            /* Close the chip shorting switch */
            REG (addr + CHP_SHRTSW) = 1 ;

            /* Clear external power control pending register indicator. */
            REG (addr + EPCCR) = 0x2;

            /* Set the External Power Good Indicator */
            REG (addr + PDCTL1) |= 0x0100;

            /* Wait for state transtion to finish */
            while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
            {
                /* Added for MISRAC compliance */
            }

            /* De-assert FORCE bit */
            REG (addr + GBLCTL) &= ~(0x1) ;

            /* Now enable DSP module state (re: bug above). */
            REG (addr + MDCTL_DSP) = (REG (addr + MDCTL_DSP) & ~(0x1F)) | 0x3;

            /* Hold DSP in local reset. */
            REG (addr + MDCTL_DSP) &= ~(0x100);

            /* Start state transition */
            REG (addr + PTCMD) = domainbit;

            /* Wait for state transtion to finish */
            while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
            {
                /* Added for MISRAC compliance */
            }
        }
    }

    TRC_1LEAVE ("DM6446GEM_halPscEnable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6446GEM_halPscDisable
 *
 *  @desc   Disables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6446GEM_halPscDisable (IN  Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     domainbit = 2 ;

    TRC_1ENTER ("DM6446GEM_halPscDisable", addr) ;

    /* Check if the DSP is in host-boot mode */
    if (TEST_BIT (REG (addr + DSP_BT), 8) == 0u) {
        /* Wait for any pending state transtions to finish */
        while( (REG (addr + PTSTAT) & domainbit ) != 0 )
        {
            /* Added for MISRAC compliance */
        }

        /* Set DSP module next state to SyncReset (clk on, in reset) */
        REG (addr + MDCTL_DSP) = (REG (addr + MDCTL_DSP) & ~(0x1F)) | 0x01;

        /* Assert Force Bit */
        REG (addr + GBLCTL) |= 0x1 ;

        /* Start state transition */
        REG (addr + PTCMD) = domainbit;

        /* Wait for state transtion to finish */
        while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
        {
            /* Added for MISRAC compliance */
        }

        /* De-assert FORCE bit */
        REG (addr + GBLCTL) &= ~(0x1) ;

        /* Set DSP power domain next state to off. */
        REG (addr + PDCTL1) &= ~(0x1);

        /* Clear the external power control pending indicator. */
        REG (addr + EPCCR) = domainbit;

        /* Set DSP next state to software reset disable (clk off, in reset). */
        REG(addr + MDCTL_DSP) &= ~(0x1F);

        /* Start state transition */
        REG (addr + PTCMD) = domainbit;

        /* Wait for external power control pending register bit to go high. */
        while( (REG (addr + EPCPR) & domainbit ) == 0 )
        {
            /* Added for MISRAC compliance */
        }

        /* Open the chip shorting switch */
        REG ( addr + CHP_SHRTSW) = 0;

        /* Call external power control to turn off external power supply. */
        /**** TBA ****/

        /* Clear external power control pending register bit. */
        REG (addr + EPCCR) = domainbit;

        /* Clear the External Power Good Indicator */
        REG (addr + PDCTL1) &= ~(0x100);

        /* Wait for state transtion to finish */
        while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
        {
            /* Added for MISRAC compliance */
        }

    }

    TRC_1LEAVE ("DM6446GEM_halPscDisable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6446GEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6446GEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    DM6446GEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;

    TRC_3ENTER ("DM6446GEM_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DM6446GEM_HalObj *) halObj ;
    addr  =  halObject->baseCfgBus + halObject->offsetSysModule ;

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            DM6446GEM_halPscEnable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            DM6446GEM_halPscDisable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
            /* Reset the DSP and also reset GEM */
            CLEAR_BIT (REG (addr + MDCTL_DSP), 8) ;
        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            /* Release the DSP from reset. */
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

    TRC_1LEAVE ("DM6446GEM_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
