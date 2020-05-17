/** ============================================================================
 *  @file   dm6467gem_phy_shmem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/DM6467GEM/shmem/Linux/
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
#include <trc.h>
#include <print.h>

/*  ----------------------------------- OSAL Headers                */
#include <osal.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <dm6467gem_hal.h>
#include <dm6467gem_hal_boot.h>
#include <dm6467gem_hal_intgen.h>
#include <dm6467gem_hal_pwr.h>
#include <dm6467gem_phy_shmem.h>


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
#define  OFFSET_SYSTEM_MODULE   0x40000


/** ============================================================================
 *  @name   DM6467GEM_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
HAL_Interface DM6467GEM_shmemInterface = {&DM6467GEM_phyShmemInit,
                                          &DM6467GEM_phyShmemExit,
                                          &DM6467GEM_halBootCtrl,
                                          &DM6467GEM_halIntCtrl,
                                          NULL,
                                          NULL,
                                          &DM6467GEM_halPwrCtrl,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL} ;


/* ============================================================================
 *  @func   DM6467GEM_phyShmemInit
 *
 *  @desc   Initializes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_phyShmemInit (IN Pvoid halObj)
{
    DSP_STATUS        status    = DSP_SOK ;
    DM6467GEM_HalObj * halObject = NULL    ;

    TRC_1ENTER ("DM6467GEM_phySharedInit", halObj) ;

    DBC_Require (NULL != halObj) ;

    halObject = (DM6467GEM_HalObj *) halObj ;

    halObject->baseCfgBus      =(Uint32) IO_ADDRESS(IO_PHYS)  ;
    halObject->offsetSysModule = OFFSET_SYSTEM_MODULE ;

    TRC_1LEAVE ("DM6467GEM_phySharedInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   DM6467GEM_phyShmemExit
 *
 *  @desc   Finalizes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
DM6467GEM_phyShmemExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;

    TRC_1ENTER ("DM6467GEM_phySharedExit", halObj) ;

    DBC_Require (NULL != halObj) ;

    (void) halObj ;

    TRC_1LEAVE ("DM6467GEM_phySharedExit", status) ;

    return status ;
}


#if defined (__cplusplus)

}
#endif
