/** ============================================================================
 *  @file   dm6467gem_hal_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6467GEM/
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
#include <dm6467gem_hal.h>
#include <dm6467gem_hal_pwr.h>


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
 *  @const  DSP_BT/PTCMD/PTSTAT/MDSTAT_DSP/MDCTL_DSP
 *
 *  @desc   Offset of the PSC module registers from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define DSP_BT              0x0014
#define EPCPR               0x1070
#define PTCMD               0x1120
#define PTSTAT              0x1128
#define PDSTAT              0x1200
#define PDCTL               0x1300
#define MDSTAT_DSP          0x1800 + (4 * LPSC_GEM)
#define MDCTL_DSP           0x1A00 + (4 * LPSC_GEM)

/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the DSP BOOT ADDR register from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define BOOT_BASE_ADDR      0x08

/*  ============================================================================
 *  @const  MDCTL_NEXT_MASK
 *
 *  @desc   Mask to start power transition.
 *  ============================================================================
 */
#define MDCTL_NEXT_MASK     (0x0000001Fu)

/*  ============================================================================
 *  @const  MDCTL_NEXT_SHIFT
 *
 *  @desc   Mask to start power transition.
 *  ============================================================================
 */
#define MDCTL_NEXT_SHIFT    (0x00000000u)

/*  ============================================================================
 *  @const  PDCTL_NEXT_MASK
 *
 *  @desc   Mask to start power transition.
 *  ============================================================================
 */
#define PDCTL_NEXT_MASK     (0x00000001u)

/*  ============================================================================
 *  @const  PDCTL_NEXT_SHIFT
 *
 *  @desc   Mask to start power transition.
 *  ============================================================================
 */
#define PDCTL_NEXT_SHIFT         (0x00000000u)


/** ============================================================================
 *  @func   DM6467GEM_halPscEnable
 *
 *  @desc   Enables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_halPscEnable (IN  Uint32 addr)
{
    DSP_STATUS status    = DSP_SOK ;
	Uint32     i ;

    TRC_1ENTER ("DM6467GEM_halPscEnable", addr) ;

    /* Check if the DSP is in host-boot mode */
    if (TEST_BIT (REG (addr + DSP_BT), 17) == 0u) {

        /* Check if the DSP is not already powered up */
        if ((REG (addr  + MDSTAT_DSP) & 0x001f) != 0x03u) {
            /*
             *  Step 1 - Wait for PTSTAT.GOSTAT to clear
             */
            while (REG (addr + PTSTAT) & 0x00000001) ;

            /*
             *  Step 2 - Set MDCTLx.NEXT to new state
             */
            REG (addr + MDCTL_DSP) &= (~MDCTL_NEXT_MASK) ;
            REG (addr + MDCTL_DSP) |= (0x3u) ;

            /*
             *  Step 3 - Start power transition ( set PTCMD.GO to 1 )
             */
            REG (addr + PTCMD) = 1 ;

            /*
             *  Step 4 - Wait for PTSTAT.GOSTAT to clear
             */
            while( REG (addr + PTSTAT) & 0x00000001 ) ;

            /*
             *  Step 5 - Verify state changed
             */
            while( ( REG (addr + MDSTAT_DSP) & 0x001f ) != 0x3u );
        }
    }

    for (i = 0; i < 10000; i++) {
    }

    TRC_1LEAVE ("DM6467GEM_halPscEnable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6467GEM_halPscDisable
 *
 *  @desc   Disables the PSC for GEM Module.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_halPscDisable (IN  Uint32 addr)
{
    DSP_STATUS status = DSP_SOK ;

    TRC_1ENTER ("DM6467GEM_halPscDisable", addr) ;

    (Void) addr ;

    TRC_1LEAVE ("DM6467GEM_halPscDisable", status) ;

    return status ;
}


/** ============================================================================
 *  @func   DM6467GEM_halPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_halPwrCtrl (IN         Pvoid          halObj,
                      IN         DSP_PwrCtrlCmd cmd,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    DM6467GEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;

    TRC_3ENTER ("DM6467GEM_halPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DM6467GEM_HalObj *) halObj ;
    addr  =  halObject->baseCfgBus + halObject->offsetSysModule ;

    switch (cmd) {
        case DSP_PwrCtrlCmd_PowerUp:
        {
            DM6467GEM_halPscEnable (addr) ;
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
            DM6467GEM_halPscDisable (addr) ;
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

    TRC_1LEAVE ("DM6467GEM_halPwrCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
