/** ============================================================================
 *  @file   da8xxgem_hal_intgen.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DA8XXGEM/
 *
 *  @desc   Hardware Abstraction Layer for Primus.
 *          Defines necessary functions for Interrupt Handling.
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
#include <da8xxgem_hal_intgen.h>


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

/*  ============================================================================
 *  @const  OFFSET_CHIPSIG
 *
 *  @desc   Offset of the register to set interrupt from system module base.
 *  ============================================================================
 */
#define  OFFSET_CHIPSIG          0x174u

/*  ============================================================================
 *  @const  OFFSET_CHIPSIG_CLR
 *
 *  @desc   Offset of the register to clear interrupt from system module base.
 *  ============================================================================
 */
#define  OFFSET_CHIPSIG_CLR      0x178u

/** ============================================================================
 *  @const  NUM_ARM_TO_DSP_INT
 *
 *  @desc   Defines the number of ARM-DSP interrupts supported by the
 *          hal_intgen module.
 *  ============================================================================
 */
#define  NUM_ARM_TO_DSP_INT     2u

/** ============================================================================
 *  @const  NUM_DSP_TO_ARM_INT
 *
 *  @desc   Defines the number of DSP-ARM interrupts supported by the
 *          hal_intgen module.
 *  ============================================================================
 */
#define  NUM_DSP_TO_ARM_INT     2u

/*  ============================================================================
 *  @const  BITPOS_ARM2DSPINTSET
 *
 *  @desc   Start position of the ARM2DSP interrupt set bits in the CHIPSIG
 *          register.
 *  ============================================================================
 */
#define  BITPOS_ARM2DSPINTSET   2u

/*  ============================================================================
 *  @const  BITPOS_DSP2ARMINTCLR
 *
 *  @desc   Start position of the DSP2ARM interrupt clear bits in the CHIPSIG_CLR
 *          register.
 *  ============================================================================
 */
#define  BITPOS_DSP2ARMINTCLR   0

/*  ============================================================================
 *  @const  BASE_ARM2DSP_INTID
 *
 *  @desc   Interrupt number of the first ARM2DSP interrupt.
 *  ============================================================================
 */
#define  BASE_ARM2DSP_INTID     5u

/** ============================================================================
 *  @const  MAX_POLL_COUNT
 *
 *  @desc   Indicates the maximum count to wait for interrupt to be cleared
 *          before timing out.
 *  ============================================================================
 */
#define MAX_POLL_COUNT          0x0FFFFFFFu

/*  ============================================================================
 *  @macro  ARM2DSP_INT_INDEX
 *
 *  @desc   Index of the ARM2DSP interrupt (0/1/2/3) based on the interrupt
 *          number.
 *          Different logic as CHIPINT2 is 5 and CHIPINT3 is 67
 *  ============================================================================
 */
#define  ARM2DSP_INT_INDEX(intId)  ((intId  == 5u) ? 0u : 1u)

/*  ============================================================================
 *  @const  BASE_DSP2ARM_INTID
 *
 *  @desc   Interrupt number of the first DSP2ARM interrupt.
 *  ============================================================================
 */
#define  BASE_DSP2ARM_INTID     28u

/*  ============================================================================
 *  @macro  DSP2ARM_INT_INDEX
 *
 *  @desc   Index of the DSP2ARM interrupt (0/1) based on the interrupt number.
 *  ============================================================================
 */
#define  DSP2ARM_INT_INDEX(intId)  ((intId) - (BASE_DSP2ARM_INTID))

/** ============================================================================
 *  @func   DA8XXGEM_halIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_halIntCtrl (IN         Pvoid          halObj,
                     IN         DSP_IntCtrlCmd cmd,
                     IN         Uint32         intId,
                     IN OUT     Pvoid          arg)
{
    DSP_STATUS         status    = DSP_SOK ;
    DA8XXGEM_HalObj  * halObject = NULL    ;
    Uint32             addr                ;
    Uint32             index               ;

    TRC_3ENTER ("DA8XXGEM_halIntCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DA8XXGEM_HalObj *) halObj ;

    switch (cmd) {
        case DSP_IntCtrlCmd_Enable:
        {
            /* Do nothing here for Primus Gem */
        }
        break ;

        case DSP_IntCtrlCmd_Disable:
        {
            /* Do nothing here for Primus Gem */
        }
        break ;

        case DSP_IntCtrlCmd_Send:
        {
            index = ARM2DSP_INT_INDEX (intId) ;
            addr  =  halObject->baseCfgBus
                   + halObject->offsetSysModule
                   + OFFSET_CHIPSIG ;
            index = ARM2DSP_INT_INDEX (intId) ;
            SET_BIT (*((Uint32 *) addr), (index + BITPOS_ARM2DSPINTSET)) ;

        }
        break ;

        case DSP_IntCtrlCmd_Clear:
        {
            addr  =  halObject->baseCfgBus
                   + halObject->offsetSysModule
                   + OFFSET_CHIPSIG_CLR ;
            index = DSP2ARM_INT_INDEX (intId) ;
            SET_BIT (*((Uint32 *) addr), (index + BITPOS_DSP2ARMINTCLR)) ;

        }
        break ;

        case DSP_IntCtrlCmd_Check:
        {
            /* Do nothing here for Primus Gem */
            *((Bool *) arg) = TRUE ;
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

    TRC_1LEAVE ("DA8XXGEM_halIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
