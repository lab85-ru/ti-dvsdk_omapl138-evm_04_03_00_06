/** ============================================================================
 *  @file   dm6437_hal_vlynq_pwr.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6437/vlynq/
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
#include <_trace.h>
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <sync.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>
#include <dm6437_hal_vlynq_pwr.h>


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
 *  @func   DM6437_halVlynqPwrChangeState
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
DM6437_halVlynqPwrChangeState (IN Pvoid           halObj,
                               IN Uint32          lpscNum,
                               IN Uint16          state);


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_halVlynqPwrCfgPLL1
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
DM6437_halVlynqPwrCfgPLL1 (IN Pvoid           halObj,
                           IN Uint32          pllm,
                           IN Uint32          div1,
                           IN Uint32          div2) ;


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_halVlynqPwrCfgDDR2
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
DM6437_halVlynqPwrCfgDDR2 (IN Pvoid halObj, IN Uint32  freq) ;


/** ============================================================================
 *  @func   DM6437_halVlynqPwrCtrl
 *
 *  @desc   Power conrtoller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6437_halVlynqPwrCtrl (IN         Pvoid          halObj,
                        IN         DSP_PwrCtrlCmd cmd,
                        IN OUT     Pvoid          arg)
{
    DSP_STATUS        status    = DSP_SOK ;
    DM6437_pscRegs *  pscRegs   = NULL ;
    DM6437_HalObj *   halObject = (DM6437_HalObj *) halObj ;

    TRC_3ENTER ("DM6437_halVlynqPwrCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObject) ;

    (Void) arg ;

    pscRegs  = (DM6437_pscRegs *) (  halObject->vlynq.region3Addr
                                   + DM6437_PSCREG_BASE) ;
    switch (cmd) {
        case DSP_PwrCtrlCmd_PeripheralUp:
        {
            /* Enable the DDR2 PSC Module */
            DM6437_halVlynqPwrChangeState (halObject, LPSC_DDR, 0x3) ;

            /* Programme the PLL1 (DDR2) to run at higher frequency. */
            DM6437_halVlynqPwrCfgPLL1 (halObject, 23, 11, 1) ;

            /* Programme the DDR PHY register corresponding to PLL1
             * frequency.
             */
            DM6437_halVlynqPwrCfgDDR2 (halObject, 162) ;
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
            pscRegs->MDCTL [LPSC_GEM] &= ~DM6437_LRESET_MASK ;
        }
        break ;

        case DSP_PwrCtrlCmd_Release:
        {
            pscRegs->MDCTL [LPSC_GEM] |= DM6437_LRESET_MASK ;
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


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_halVlynqPwrChangeState
 *
 *  @desc   Change power module state to ( ENABLE, DISABLE, SYNCRESET, RESET ).
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM6437_halVlynqPwrChangeState (IN Pvoid           halObj,
                             IN Uint32          lpscNum,
                             IN Uint16          state)
{
    DM6437_pscRegs *  pscRegs = NULL ;
    DM6437_HalObj *    halObject = (DM6437_HalObj *) halObj ;

    TRC_3ENTER ("DM6437_halVlynqPwrChangeState", halObj, lpscNum, state) ;

    pscRegs  = (DM6437_pscRegs *) (  halObject->vlynq.region3Addr
                                   + DM6437_PSCREG_BASE) ;
    /*  ------------------------------------------------------------------------
     *  Step 1 - Wait for PTSTAT.GOSTAT to clear
     *  ------------------------------------------------------------------------
     */
    while (pscRegs->PTSTAT & 0x1) ;

    /*  ------------------------------------------------------------------------
     *  Step 2 - Set MDCTLx.NEXT to new state
     *  ------------------------------------------------------------------------
     */
    pscRegs->MDCTL [lpscNum] &= 0xFFE0 ;
    pscRegs->MDCTL [lpscNum] |= state  ;

    /*  ------------------------------------------------------------------------
     *  Step 3 - Start power transition ( set PTCMD.GO to 1 )
     *  ------------------------------------------------------------------------
     */
    pscRegs->PTCMD = 0x0001 ;

    /*  ------------------------------------------------------------------------
     *  Step 4 - Wait for PTSTAT.GOSTAT to clear
     *  ------------------------------------------------------------------------
     */
    while (pscRegs->PTSTAT & 0x1) ;

    /*  ------------------------------------------------------------------------
     *  Step 5 - Verify state changed
     *  ------------------------------------------------------------------------
     */
    while ((pscRegs->MDSTAT [lpscNum] & 0x001F) != state) ;

    TRC_0LEAVE ("DM6437_halVlynqIntCtrl") ;
}


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_halVlynqPwrCfgPLL1
 *
 *  @desc   Configures the PLL1.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM6437_halVlynqPwrCfgPLL1 (IN Pvoid           halObj,
                         IN Uint32          pllm,
                         IN Uint32          div1,
                         IN Uint32          div2)
{
    DM6437_pllRegs *  pllRegs = NULL ;
    DM6437_HalObj *    halObject = (DM6437_HalObj *) halObj ;

    TRC_4ENTER ("DM6437_halVlynqPwrCfgPLL1", halObj, pllm, div1, div2) ;

    pllRegs  = (DM6437_pllRegs *) (  halObject->vlynq.region3Addr
                                   + DM6437_PLL1REG_BASE) ;
    /*  ------------------------------------------------------------------------
     *  Step 1 - Set PLL to BYPASS mode and select clock source.
     *  ------------------------------------------------------------------------
     */
    pllRegs->PLLCTL &= 0xFFFFFEFF ;
    pllRegs->PLLCTL |= 0x0 ;
    pllRegs->PLLCTL &= 0xFFFFFFDF ;
    pllRegs->PLLCTL &= 0xFFFFFFFE ;

    SYNC_UDelay (10000) ;

    pllRegs->PLLCTL &= 0xFFFFFFF7 ;
    pllRegs->PLLCTL |= 0x00000010 ;
    pllRegs->PLLCTL &= 0xFFFFFFFD ;
    pllRegs->PLLCTL &= 0xFFFFFFEF ;
    pllRegs->PLLCTL &= 0xFFFFFEFF ;
    pllRegs->PLLCTL |= 0x0 ;

    /*  ------------------------------------------------------------------------
     *  Step 2 - Load PLL multiplier.
     *  ------------------------------------------------------------------------
     */
    pllRegs->PLLM = pllm ;

    /*  ------------------------------------------------------------------------
     *  Step 3 - Load PLL2 divider - DDR2 and VPBE are programmable.
     *  ------------------------------------------------------------------------
     */
    pllRegs->PLLDIV1  =  div2 ;
    pllRegs->PLLDIV2  =  div1 ;
    pllRegs->PLLDIV1 &= 0xFFFF7FFF ;
    pllRegs->PLLDIV2 &= 0xFFFF7FFF ;
    pllRegs->PLLDIV1 |= 0x00008000 ;
    pllRegs->PLLDIV2 |= 0x00008000 ;

    /*  ------------------------------------------------------------------------
     *  Step 4 - Set phase alignment and Wait for operation to finish.
     *  ------------------------------------------------------------------------
     */
    pllRegs->PLLCMD |= 0x00000001 ;
    while ((pllRegs->PLLSTAT & 0x1) == 0x1) ;

    /*  ------------------------------------------------------------------------
     *  Step 5 - Wait for PLL to re-lock ( 2000 cycles ).
     *  ------------------------------------------------------------------------
     */
    SYNC_UDelay (10000) ;
    pllRegs->PLLCTL |= 0x00000008 ;

    /*  ------------------------------------------------------------------------
     *  Step 6 - Switch out of BYPASS mode.
     *  ------------------------------------------------------------------------
     */
    SYNC_UDelay (10000) ;
    pllRegs->PLLCTL |= 0x00000001 ;

    TRC_0LEAVE ("DM6437_halVlynqPwrCfgPLL1") ;
}


/*  ----------------------------------------------------------------------------
 *  @func   DM6437_halVlynqPwrCfgDDR2
 *
 *  @desc   Configures the DDR2 module.
 *
 *  @modif  None
 *  ----------------------------------------------------------------------------
 */
NORMAL_API
Void
DM6437_halVlynqPwrCfgDDR2 (IN Pvoid halObj, IN Uint32  freq)
{
    DM6437_ddrRegs *   ddrRegs = NULL ;
    DM6437_vlynqRegs * pvlynqRegs  = NULL  ;
    DM6437_HalObj *    halObject = (DM6437_HalObj *) halObj ;
    Uint32 *           vtpiocr = NULL ;
    Uint32 *           vtpr    = NULL ;
    Uint32 *           dftEnb  = NULL ;
    Uint32             tempAddr ;

    TRC_2ENTER ("DM6437_halVlynqPwrCfgPLL1", halObj, freq) ;

    ddrRegs    = (DM6437_ddrRegs *) (halObject->vlynq.region2Addr) ;
    pvlynqRegs  = (DM6437_vlynqRegs *) (  halObject->vlynq.region3Addr
                                        + DM6437_PEERVLYNQREG_BASE) ;

    vtpiocr  = (Uint32 *) (halObject->vlynq.region2Addr + 0xF0) ;
    vtpr     = (Uint32 *) (halObject->vlynq.region3Addr + 0x42038) ;
    dftEnb   = (Uint32 *) (halObject->vlynq.region3Addr + 0x4004C) ;

    /* Enable the DDR2 PSC Module */
    DM6437_halVlynqPwrChangeState (halObject, LPSC_DDR, 0x3) ;

    /* Save the original base address and size */
    tempAddr = pvlynqRegs->LOCAL_RAMO2 ;

    /* Set the new address and size */
    pvlynqRegs->LOCAL_RAMO2 = DM6437_DDRREG_BASE ;

    /*  ------------------------------------------------------------------------
     *  Step 1. Enable DDR2 PHY.
     *  ------------------------------------------------------------------------
     */
    ddrRegs->DDRPHYCR =  0x50006405 ;
    ddrRegs->SDBCR    =  0x00138822 ;
    ddrRegs->SDTIMR   =  0x16492148 ;
    ddrRegs->SDTIMR2  =  0x000CC702 ;
    ddrRegs->SDBCR    =  0x00130822 ;

    /*  ------------------------------------------------------------------------
     *  Step 3. Refresh Control [ 7.8 usec * freq in MHz ].
     *  ------------------------------------------------------------------------
     */
    ddrRegs->SDRCR = 0x4EF ;

    /* Reset the DDR2 PSC Module */
    DM6437_halVlynqPwrChangeState (halObject, LPSC_DDR, 0x1) ;
    /* Enable the DDR2 PSC Module */
    DM6437_halVlynqPwrChangeState (halObject, LPSC_DDR, 0x3) ;

    /*  ------------------------------------------------------------------------
     *  Step 4. Enable VTP calibration
     *  ------------------------------------------------------------------------
     */
    *vtpiocr  = 0x201F ;
    *vtpiocr  = 0xA01F ;

    /* wait for 33 VTP clock cycles */
    SYNC_UDelay (10000);
    *dftEnb   = 1 ;
    *vtpiocr  = 0xA000 | (*vtpr | 0x3FF) ;

    /* Wait for few more clock cycles */
    SYNC_UDelay (10000);
    *vtpiocr &= 0xFFFFDFFF ;
    *dftEnb   = 0 ;

    /* Save the original base address and size */
    pvlynqRegs->LOCAL_RAMO2 = tempAddr ;

    TRC_0LEAVE ("DM6437_halVlynqPwrCfgDDR2") ;
}


#if defined (__cplusplus)
}
#endif
