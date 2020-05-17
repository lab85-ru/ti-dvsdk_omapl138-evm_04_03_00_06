/** ============================================================================
 *  @file   da8xxgem_phy_shmem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DA8XXGEM/shmem/Linux/
 *
 *  @desc   Linux shmem driver interface file.
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


/*  ----------------------------------- OS Headers                  */
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>

/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- Trace & Debug               */
#include <_trace.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <da8xxgem_hal.h>
#include <da8xxgem_hal_boot.h>
#include <da8xxgem_hal_intgen.h>
#include <da8xxgem_hal_pwr.h>
#include <da8xxgem_phy_shmem.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @macro  COMPONENT_ID
 *
 *  @desc   Component and Subcomponent Identifier.
 *  ============================================================================
 */
#define  COMPONENT_ID       ID_ARCH_SHMEM_PHY

/** ============================================================================
 *  @macro  SET_FAILURE_REASON
 *
 *  @desc   Sets failure reason.
 *  ============================================================================
 */
#if defined (DDSP_DEBUG)
#define SET_FAILURE_REASON TRC_SetReason (status,FID_C_ARCH_PHY_SHMEM,__LINE__)
#else
#define SET_FAILURE_REASON
#endif /* if defined (DDSP_DEBUG) */


/*  ============================================================================
 *  @const  OFFSET_SYSTEM_MODULE
 *
 *  @desc   Offset of the system module registers from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define  OFFSET_SYSTEM_MODULE   0x14000u

 /*  ============================================================================
 *  @const  OFFSET_PSC0
 *
 *  @desc   Offset of the PSC0 from the base of the CFG
 *          memory.
 *  ============================================================================
 */
#define  OFFSET_PSC0   0x10000u


/** ============================================================================
 *  @name   DA8XXGEM_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
HAL_Interface DA8XXGEM_shmemInterface = {&DA8XXGEM_phyShmemInit,
                                          &DA8XXGEM_phyShmemExit,
                                          &DA8XXGEM_halBootCtrl,
                                          &DA8XXGEM_halIntCtrl,
                                          NULL,
                                          NULL,
                                          &DA8XXGEM_halPwrCtrl,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL} ;


/* ============================================================================
 *  @func   DA8XXGEM_phyShmemInit
 *
 *  @desc   Initializes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_phyShmemInit (IN Pvoid halObj)
{
    DSP_STATUS        status    = DSP_SOK ;
    DA8XXGEM_HalObj * halObject = NULL    ;

    TRC_1ENTER ("DA8XXGEM_phyShmemInit", halObj) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DA8XXGEM_HalObj *) halObj ;

    halObject->baseCfgBus      = (Uint32)IO_ADDRESS(IO_PHYS)  ;
    halObject->offsetSysModule = OFFSET_SYSTEM_MODULE ;
    halObject->offsetPsc0      = OFFSET_PSC0 ;

    TRC_1LEAVE ("DA8XXGEM_phyShmemInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DA8XXGEM_phyShmemExit
 *
 *  @desc   Finalizes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DA8XXGEM_phyShmemExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;

    TRC_1ENTER ("DA8XXGEM_phyShmemExit", halObj) ;

    DBC_Require (NULL != halObj) ;

    (void) halObj ;

    TRC_1LEAVE ("DA8XXGEM_phyShmemExit", status) ;

    return status ;
}


#if defined (__cplusplus)

}
#endif
