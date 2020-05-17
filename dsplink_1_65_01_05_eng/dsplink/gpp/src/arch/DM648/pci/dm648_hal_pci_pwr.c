/** ============================================================================
 *  @file   dm648_hal_pci_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM648/pci/
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

/*  ----------------------------------- OSAL Headers                */
#include <sync.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm648_hal.h>
#include <dm648_hal_pci.h>
#include <dm648_hal_pci_pwr.h>


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
#define SET_FAILURE_REASON   TRC_SetReason (status,FID_C_ARCH_HAL_PWR, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ----------------------------------------------------------------------------
 *  @func   DM648_halPciPwrChangeState
 *
 *  @desc   Change power module state to ( ENABLE, DISABLE, SYNCRESET, RESET ).
 *
 *  @arg    halObject.
 *              HAL Object.
 *  @arg    lpscNum.
 *              LPSC module number.
 *  @arg    state.
 *              Desired state.
 *
 *  @ret    None.
 *
 *  @enter  halObject must be initialized.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM648_halPciPwrChangeState (IN Pvoid           halObj,
                            IN Uint32          lpscNum,
                            IN Uint16          state);


/*  ----------------------------------------------------------------------------
 *  @func   DM648_halPciPwrCfgPLL1
 *
 *  @desc   Configures the PLL1.
 *
 *  @arg    halObject.
 *              HAL Object.
 *  @arg    pllm.
 *              Multiplier constant.
 *  @arg    div1.
 *              Divider1 constant.
 *  @arg    div2.
 *              Divider2 constant.
 *
 *  @ret    None.
 *
 *  @enter  halObject must be initialized.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM648_halPciPwrCfgPLL1 (IN Pvoid           halObj,
                        IN Uint32          pllm,
                        IN Uint32          div1,
                        IN Uint32          div2) ;


/*  ----------------------------------------------------------------------------
 *  @func   DM648_halPciPwrCfgDDR2
 *
 *  @desc   Configures the DDR2 module.
 *
 *  @arg    halObject.
 *              HAL Object.
 *  @arg    freq.
 *              frequency of DDR2 interface.
 *
 *  @ret    None.
 *
 *  @enter  halObject must be initialized.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM648_halPciPwrCfgDDR2 (IN Pvoid halObj, IN Uint32  freq) ;


/** ============================================================================
 *  @func   DM648_halPciPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM648_halPciPwrCtrl (IN         Pvoid          halObj,
                     IN         DSP_PwrCtrlCmd cmd,
                     IN OUT     Pvoid          arg)
{
    DSP_STATUS        status    = DSP_SOK ;


    DM648_HalObj *   halObject = (DM648_HalObj *) halObj ;
    Uint32           regs ;
    TRC_3ENTER ("DM648_halPciPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObject) ;

    (Void) arg ;

   /* pscRegs  = (DM648_pscRegs *) (  halObject->pci.barVirt [CFG_REGS_BAR_NO]
                                   + DM6437_PSCREG_BASE) ; */
    regs  = (Uint32) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                      + DM648_MDCTL_BASE
                      - DM648_BAR2_BASE) ;
    switch (cmd) {
        case DSP_PwrCtrlCmd_PeripheralUp:
        {
            /* Enable the DDR2 PSC Module */
            DM648_halPciPwrChangeState (halObject, LPSC_DDR, 0x3) ;

            /* Programme the DDR PHY register corresponding to PLL1
             * frequency.
             */
            DM648_halPciPwrCfgDDR2 (halObject, 162) ;

            /* Enable EDMA module*/
            DM648_halPciPwrChangeState (halObject, LPSC_EDMA_TPCC, 0x3) ;

          /*  DM6437_halPciPwrChangeState (halObject, LPSC_EDMA_TPTC0, 0x3) ;
              DM6437_halPciPwrChangeState (halObject, LPSC_EDMA_TPTC1, 0x3) ; */
        }
        break ;

        case DSP_PwrCtrlCmd_PowerUp:
        {
        }
        break ;

        case DSP_PwrCtrlCmd_PowerDown:
        {
        }
        break ;

        case DSP_PwrCtrlCmd_Reset:
        {
            *((Uint32 *) (regs + 4 * LPSC_GEM)) &= ~DM648_LRESET_MASK ;
        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            *((Uint32 *) (regs + 4 * LPSC_GEM)) |= DM648_LRESET_MASK ;
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


/*  ----------------------------------------------------------------------------
 *  @func   DM648_halPciPwrChangeState
 *
 *  @desc   Change power module state to ( ENABLE, DISABLE, SYNCRESET, RESET ).
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM648_halPciPwrChangeState (IN Pvoid           halObj,
                            IN Uint32          lpscNum,
                            IN Uint16          state)
{
    DM648_HalObj *    halObject = (DM648_HalObj *) halObj ;
    Uint32            regs ;

    TRC_3ENTER ("DM648_halPciPwrChangeState", halObj, lpscNum, state) ;

    /*  ------------------------------------------------------------------------
     *  Step 1 - Set MDCTLx.NEXT to new state
     *  ------------------------------------------------------------------------
     */
    regs  = (Uint32) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                      + DM648_MDCTL_BASE
                      - DM648_BAR2_BASE) ;
    *((Uint32 *) (regs + 4 * lpscNum)) &= 0xFFFFFFE0 ;
    *((Uint32 *) (regs + 4 * lpscNum)) |= state ;

    /*  ------------------------------------------------------------------------
     *  Step 2 - Start power transition ( set PTCMD.GO to 1 )
     *  ------------------------------------------------------------------------
     */
    regs  = (Uint32) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                      + DM648_PTCMD_BASE
                      - DM648_BAR2_BASE) ;
    *((Uint32 *) regs) = 0x0001 ;

    /*  ------------------------------------------------------------------------
     *  Step 3 - Wait for PTSTAT.GOSTAT to clear
     *  ------------------------------------------------------------------------
     */
    regs  = (Uint32) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                      + DM648_PTSTAT_BASE
                      - DM648_BAR2_BASE) ;
    while (*((Uint32 *) regs) & 0x1) ;

    /*  ------------------------------------------------------------------------
     *  Step 5 - Verify state changed
     *  ------------------------------------------------------------------------
     */
    regs  = (Uint32) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                      + DM648_MDSTAT_BASE
                      - DM648_BAR2_BASE) ;
    while ((*((Uint32 *) (regs + 4 * lpscNum)) & 0x001F) != state) ;

    TRC_0LEAVE ("DM648_halPciIntCtrl") ;
}


/*  ----------------------------------------------------------------------------
 *  @func   DM648_halPciPwrCfgDDR2
 *
 *  @desc   Configures the DDR2 module.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM648_halPciPwrCfgDDR2 (IN Pvoid halObj, IN Uint32  freq)
{
    DM648_ddrRegs   * ddrRegs   = NULL ;
    DM648_pciRegs   * pciRegs   = NULL ;
    DM648_HalObj *    halObject = (DM648_HalObj *) halObj ;
    Uint32            oldBar0TrlVal ;

    TRC_2ENTER ("DM648_halPciPwrCfgPLL1", halObj, freq) ;

    pciRegs = (DM648_pciRegs *) (  halObject->pci.barVirt[CFG_REGS_BAR_NO]
                                 + DM648_PCIREG_BASE
                                 - DM648_BAR2_BASE) ;

    /* Enable the DDR2 PSC Module */
    DM648_halPciPwrChangeState (halObject, LPSC_DDR, 0x3) ;

     /* Save the old translation register value */
    oldBar0TrlVal = pciRegs->PCIBAR0TRL ;

    /* program the new address */
    pciRegs->PCIBAR0TRL = DM648_DDRREG_BASE & pciRegs->PCIBAR0MSK ;

    /* setup the DDR2 registers */
    ddrRegs  = (DM648_ddrRegs *) (  halObject->pci.barVirt [SCRATCH_BAR_NUMBER]
                                   + DM648_DDRREG_BASE
                                   - pciRegs->PCIBAR0TRL) ;

    /* program the DDR2 registers */
    /* DDR2 Register Initialization (for 265.9 MHz) */
    /* Production board with 2 GB DDR */

    ddrRegs->DDRCTL = 0x50006405u ;   /* DLL powered, ReadLatency=5 */
    ddrRegs->SDCFG  = 0x00008832u ;   /* DDR Bank: 32-bit bus, CAS=4, 8 banks, 1024-word pg */
    ddrRegs->SDTIM1 = 0x68DB5BD9u ;   /* SDRAM Timinig 1 register */
    ddrRegs->SDTIM2 = 0x00B7C722u ;   /* SDRAM Timinig 2 register */
    ddrRegs->SDCFG    &= ~(0x8000);   /* Clear TIMUNLOCK bit */
    ddrRegs->SDREF  = 0x0000081Au ;   /*Refresh Control [ 7.8 usec * freq ] */
    SYNC_UDelay (1) ;
    /* restore the old value */
    pciRegs->PCIBAR0TRL = oldBar0TrlVal ;

    TRC_0LEAVE ("DM648_halPciPwrCfgDDR2") ;
}


#if defined (__cplusplus)
}
#endif
