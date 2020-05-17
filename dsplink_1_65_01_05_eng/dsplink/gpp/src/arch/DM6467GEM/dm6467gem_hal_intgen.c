/** ============================================================================
 *  @file   dm6467gem_hal_intgen.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6467GEM/
 *
 *  @desc   Hardware Abstraction Layer for DavinciHD.
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
#include <dm6467gem_hal.h>
#include <dm6467gem_hal_intgen.h>


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
 *  @const  OFFSET_DSPINT
 *
 *  @desc   Offset of the DSPINT ARM to DSP interrupt status from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_DSPINT             0x60

/*  ============================================================================
 *  @const  OFFSET_DSPINTSET
 *
 *  @desc   Offset of the DSPINTSET ARM to DSP interrupt set from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_DSPINTSET          0x64

/*  ============================================================================
 *  @const  OFFSET_DSPINTCLR
 *
 *  @desc   Offset of the DSPINTCLR ARM to DSP interrupt clear from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_DSPINTCLR         0x68

/*  ============================================================================
 *  @const  OFFSET_ARMINT
 *
 *  @desc   Offset of the ARMINT DSP to ARM interrupt status from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_ARMINT            0x70

/*  ============================================================================
 *  @const  OFFSET_ARMINTSET
 *
 *  @desc   Offset of the ARMINTSET DSP to ARM interrupt set from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_ARMINTSET         0x74

/*  ============================================================================
 *  @const  OFFSET_ARMINTCLR
 *
 *  @desc   Offset of the ARMINTCLR DSP to ARM interrupt clear from
 *          system module base.
 *  ============================================================================
 */
#define  OFFSET_ARMINTCLR         0x78

/** ============================================================================
 *  @const  NUM_ARM_TO_DSP_INT
 *
 *  @desc   Defines the number of ARM-DSP interrupts supported by the
 *          hal_intgen module.
 *  ============================================================================
 */
#define  NUM_ARM_TO_DSP_INT     4

/** ============================================================================
 *  @const  NUM_DSP_TO_ARM_INT
 *
 *  @desc   Defines the number of DSP-ARM interrupts supported by the
 *          hal_intgen module.
 *  ============================================================================
 */
#define  NUM_DSP_TO_ARM_INT     1

/*  ============================================================================
 *  @const  BITPOS_ARM2DSPINTSET
 *
 *  @desc   Start position of the ARM2DSP interrupt set bits in the DSPINTSET
 *          register.
 *  ============================================================================
 */
#define  BITPOS_ARM2DSPINTSET   0

/*  ============================================================================
 *  @const  BITPOS_ARM2DSPSTATUS
 *
 *  @desc   Start position of the ARM2DSP interrupt status bits in the DSPINT
 *          register.
 *  ============================================================================
 */
#define  BITPOS_ARM2DSPSTATUS   0

/*  ============================================================================
 *  @const  BITPOS_DSP2ARMINTCLR
 *
 *  @desc   Start position of the DSP2ARM interrupt status bits in the ARMINT
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
#define  BASE_ARM2DSP_INTID     16

/*  ============================================================================
 *  @const  BASE_DSP2ARM_INTID
 *
 *  @desc   Interrupt number of the first DSP2ARM interrupt.
 *  ============================================================================
 */
#define  BASE_DSP2ARM_INTID     45

/** ============================================================================
 *  @const  MAX_POLL_COUNT
 *
 *  @desc   Indicates the maximum count to wait for interrupt to be cleared
 *          before timing out.
 *  ============================================================================
 */
#define MAX_POLL_COUNT          0x0FFFFFFF

/*  ============================================================================
 *  @macro  ARM2DSP_INT_INDEX
 *
 *  @desc   Index of the ARM2DSP interrupt (0/1/2/3) based on the interrupt
 *          number.
 *  ============================================================================
 */
#define  ARM2DSP_INT_INDEX(intId)  (intId - BASE_ARM2DSP_INTID)

/*  ============================================================================
 *  @macro  DSP2ARM_INT_INDEX
 *
 *  @desc   Index of the DSP2ARM interrupt (0) based on the interrupt number.
 *  ============================================================================
 */
#define  DSP2ARM_INT_INDEX(intId)  (intId - BASE_DSP2ARM_INTID)


/** ============================================================================
 *  @func   DM6467GEM_halIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_halIntCtrl (IN         Pvoid          halObj,
                      IN         DSP_IntCtrlCmd cmd,
                      IN         Uint32         intId,
                      IN OUT     Pvoid          arg)
{
    DSP_STATUS         status    = DSP_SOK ;
    DM6467GEM_HalObj * halObject = NULL    ;
    Uint32             addr              ;
    Uint32             index             ;

    TRC_3ENTER ("DM6467GEM_halIntCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DM6467GEM_HalObj *) halObj ;
    index = ARM2DSP_INT_INDEX (intId) ;

    switch (cmd) {
        case DSP_IntCtrlCmd_Enable:
        {
            /* Do nothing here for DavinciHD Gem */
        }
        break ;

        case DSP_IntCtrlCmd_Disable:
        {
            /* Do nothing here for DavinciHD Gem */
        }
        break ;

        case DSP_IntCtrlCmd_Send:
        {
            addr  =  halObject->baseCfgBus
                   + halObject->offsetSysModule
                   + OFFSET_DSPINTSET ;
            SET_BIT (*((Uint32 *) addr), (index + BITPOS_ARM2DSPINTSET)) ;
        }
        break ;

        case DSP_IntCtrlCmd_Clear:
        {
            addr  =  halObject->baseCfgBus
                   + halObject->offsetSysModule
                   + OFFSET_ARMINTCLR ;
            CLEAR_BIT (*((Uint32 *) addr), (index + BITPOS_DSP2ARMINTCLR)) ;
        }
        break ;

        case DSP_IntCtrlCmd_Check:
        {
            /* Do nothing here for DavinciHD Gem */
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

    TRC_1LEAVE ("DM6467GEM_halIntCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
