/** ============================================================================
 *  @file   tnetv107xgem_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/TNETV107XGEM/
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
#include <tnetv107xgem_hal.h>
#include <tnetv107xgem_hal_pwr.h>


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
#define LPSC_GEM            1

/*  ============================================================================
 *  @const  DSP_BT/EPCPR/PTCMD/PTSTAT/PTSTAT1/PDCTL/PDCTL1/MDSTAT_DSP/MDCTL_DSP
 *
 *  @desc   Offset of the PSC module registers from the base of the Clock Ctrl
 *          memory. (i.e 0x0808A000)
 *  ============================================================================
 */
#define PTCMD               0x1120
#define PTSTAT              0x1128
#define MDSTAT_DSP          0x1800 + (4 * LPSC_GEM)
#define MDCTL_DSP           0x1A00 + (4 * LPSC_GEM)
#define GEM_LRST            0x0008


/** ============================================================================
 *  @func   TNETV107XGEM_halPscEnable
 *
 *  @desc   Enables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_halPscEnable (IN  Uint32 addr)
{
    DSP_STATUS status    = DSP_SOK ;
    Uint32     domainbit = 1       ;

    TRC_1ENTER ("TNETV107XGEM_halPscEnable", addr) ;

    /* Check if the DSP module is disabled */
    if ((REG (addr + MDSTAT_DSP) & 0x001F) != 0x0003) {
        /* Wait for any pending state transtions to finish */
        while( (REG (addr + PTSTAT) & domainbit ) != 0 )
        {
            /* Added for MISRAC compliance */
        }
        /* Set DSP power domain next state to on. */
        /* REG (addr + PDCTL) |= 0x1; */ /* no effect */

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

    TRC_1LEAVE ("TNETV107XGEM_halPscEnable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   TNETV107XGEM_halPscDisable
 *
 *  @desc   Disables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_halPscDisable (IN  Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;
    Uint32     domainbit = 1 ;

    TRC_1ENTER ("TNETV107XGEM_halPscDisable", addr) ;


    /* Wait for any pending state transtions to finish */
    while( (REG (addr + PTSTAT) & domainbit ) != 0 )
    {
        /* Added for MISRAC compliance */
    }

    /* Set DSP module next state to SyncReset (clk on, in reset) */
    REG (addr + MDCTL_DSP) = (REG (addr + MDCTL_DSP) & ~(0x1F)) | 0x01;
    
    /* Start state transition */
    REG (addr + PTCMD) = domainbit;

    /* Wait for state transtion to finish */
    while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
    {
        /* Added for MISRAC compliance */
    } 

    /* Set DSP next state to software reset disable (clk off, in reset). */
    REG(addr + MDCTL_DSP) &= ~(0x1F);

    /* Start state transition */
    REG (addr + PTCMD) = domainbit;

    /* Wait for state transtion to finish */
    while( ( REG (addr + PTSTAT) & domainbit ) != 0 )
    {
        /* Added for MISRAC compliance */
    }

    TRC_1LEAVE ("TNETV107XGEM_halPscDisable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   TNETV107XGEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    TNETV107XGEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;

    TRC_3ENTER ("TNETV107XGEM_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (TNETV107XGEM_HalObj *) halObj ;
    addr  =  halObject->clkCtrlBase;

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            TNETV107XGEM_halPscEnable (addr) ;            
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            /* This causes system to hang; NEED TO FIX THIS - Madhvi Jain */
            //TNETV107XGEM_halPscDisable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
            /* Reset the DSP and also reset GEM */
            CLEAR_BIT (REG (addr + MDCTL_DSP), 8) ;            
            /* Put DSP module into local reset. */
            REG( addr + GEM_LRST ) = 1;
            /* Release the DSP from reset. */
            SET_BIT (REG (addr + MDCTL_DSP), 8) ;                        
        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            /* Release DSP module out of local reset. */
            REG( addr + GEM_LRST ) = 0;                        
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

    TRC_1LEAVE ("TNETV107XGEM_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
