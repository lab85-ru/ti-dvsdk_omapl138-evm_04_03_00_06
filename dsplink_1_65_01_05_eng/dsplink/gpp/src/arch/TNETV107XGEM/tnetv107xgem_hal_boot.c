/** ============================================================================
 *  @file   tnetv107xgem_hal_boot.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/TNETV107XGEM/
 *
 *  @desc   Boot control module.
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
#include <tnetv107xgem_hal_boot.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_HAL_BOOT


/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON   TRC_SetReason (status, FID_C_ARCH_HAL_BOOT, __LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @const  BOOT_BASE_ADDR
 *
 *  @desc   Offset of the DSP BOOT ADDR register from the base of the CFG
 *          memory. (i.e. 0x08087000)
 *  ============================================================================
 */
#define BOOT_BASE_ADDR           0x044

/** ============================================================================
 *  @const  OFFSET_KICK0/OFFSET_KICK1/KICK0_UNLOCKVAL/KICK1_UNLOCKVAL
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#define OFFSET_KICK0             0x038
#define OFFSET_KICK1             0x03C
#define KICK0_UNLOCKVAL     0x83e70b13
#define KICK1_UNLOCKVAL     0x95a4f1e0



/** ============================================================================
 *  @func   TNETV107XGEM_halBootCtrl
 *
 *  @desc   Boot controller.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_halBootCtrl (IN         Pvoid           halObj,
                        IN         DSP_BootCtrlCmd cmd,
                        IN OUT     Pvoid           arg)
{
    DSP_STATUS         status  = DSP_SOK ;
    TNETV107XGEM_HalObj * halObject = NULL  ;
    Uint32             addr              ;
    Uint32             base_addr              ;

    TRC_3ENTER ("TNETV107XGEM_halBootCtrl", halObj, cmd, arg) ;

    DBC_Require (NULL != halObj) ;

    halObject = (TNETV107XGEM_HalObj *) halObj ;
    base_addr  =  halObject->cfgBase;
    addr = base_addr + BOOT_BASE_ADDR ;
    TRC_1PRINT (TRC_LEVEL4,"TNETV107XGEM_halBootCtrl addr=[0x%x]\n",addr);


    switch (cmd) {
        case DSP_BootCtrlCmd_SetEntryPoint:
        {
            /* Unlock the kick0 area */
            REG(base_addr + OFFSET_KICK0) = KICK0_UNLOCKVAL ;
            /* Unlock the kick1 area */
            REG(base_addr + OFFSET_KICK1) = KICK1_UNLOCKVAL ;
            REG (addr) =  (Uint32)(arg) ;
            /* Lock the kick0 area */
            REG(base_addr + OFFSET_KICK0) = 0x00000000 ;        
    
            TRC_1PRINT (TRC_LEVEL4,"TNETV107XGEM_halBootCtrl addr_val=[0x%x]\n",REG(addr));
        }
        break ;

        case DSP_BootCtrlCmd_SetBootComplete:
        {
            /* Do nothing here for Davinci gem */
        }
        break ;

        case DSP_BootCtrlCmd_ResetBootComplete:
        {
            /* Do nothing here for Davinci gem */
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

    TRC_1LEAVE ("TNETV107XGEM_halBootCtrl", status) ;

    return status ;
}


#if defined (__cplusplus)
}
#endif
