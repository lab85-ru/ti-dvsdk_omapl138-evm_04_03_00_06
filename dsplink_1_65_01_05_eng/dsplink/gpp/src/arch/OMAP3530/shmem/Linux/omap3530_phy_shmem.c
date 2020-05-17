/** ============================================================================
 *  @file   omap3530_phy_shmem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP3530/shmem/Linux/
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
#include <omap3530_hal.h>
#include <omap3530_hal_boot.h>
#include <omap3530_hal_intgen.h>
#include <omap3530_hal_pwr.h>
#include <omap3530_phy_shmem.h>
#include <omap3530_hal_mmu.h>


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


/** ============================================================================
 *  @name   OMAP3530_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
HAL_Interface OMAP3530_shmemInterface = { &OMAP3530_phyShmemInit,
                                          &OMAP3530_phyShmemExit,
                                          &OMAP3530_halBootCtrl,
                                          &OMAP3530_halIntCtrl,
                                          &OMAP3530_halMmuCtrl,
                                          NULL,
                                          &OMAP3530_halPwrCtrl,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL} ;


/* ============================================================================
 *  @func   OMAP3530_phyShmemInit
 *
 *  @desc   Initializes Shared/OMAP Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_phyShmemInit (IN Pvoid halObj)
{
    DSP_STATUS        status    = DSP_SOK ;
    OMAP3530_HalObj * halObject = NULL    ;
    MemMapInfo  mapInfo ;

    TRC_1ENTER ("OMAP3530_phyShmemInit", halObj) ;

    DBC_Require (NULL != halObj) ;

    halObject = (OMAP3530_HalObj *) halObj ;

    halObject->generalCtrlBase = (Uint32)ioremap(GENERAL_CONTROL_BASE, SZ_4K) ;
    halObject->iva2CmBase      = (Uint32)ioremap(IVA2_CM_BASE, SZ_8K) ;
    halObject->coreCmBase      = (Uint32)ioremap(CORE_CM_BASE, SZ_8K) ;
    halObject->perCmBase       = (Uint32)ioremap(PER_CM_BASE, SZ_8K) ;
    halObject->iva2PrmBase     = (Uint32)ioremap(IVA2_PRM_BASE, SZ_2K) ;
    halObject->mailboxBase     = (Uint32)ioremap(MAILBOX_BASE, SZ_4K) ;

    mapInfo.src  = MMU_BASE ;
    mapInfo.size = MMU_SIZE ;
    mapInfo.memAttrs = MEM_UNCACHED ;
    status = MEM_Map (&mapInfo) ;
    if (DSP_FAILED (status)) {
        SET_FAILURE_REASON ;
        halObject->mmuBase = 0 ;
    }
    else {
        halObject->mmuBase = mapInfo.dst ;
    }

    TRC_1LEAVE ("OMAP3530_phyShmemInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   OMAP3530_phyShmemExit
 *
 *  @desc   Finalizes Shared/OMAP Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_phyShmemExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;
    OMAP3530_HalObj * halObject = (OMAP3530_HalObj *) halObj ;
    MemUnmapInfo      unmapInfo                              ;

    TRC_1ENTER ("OMAP3530_phyShmemExit", halObj) ;

    DBC_Require (NULL != halObject) ;

    unmapInfo.addr = halObject->mmuBase ;
    unmapInfo.size = MMU_SIZE ;
    if (unmapInfo.addr != 0) {
        MEM_Unmap (&unmapInfo) ;
        if (DSP_FAILED (status)) {
            SET_FAILURE_REASON ;
        }
        halObject->mmuBase = 0 ;
    }

    iounmap ((Pvoid) halObject->generalCtrlBase) ;
    iounmap ((Pvoid) halObject->iva2CmBase) ;
    iounmap ((Pvoid) halObject->coreCmBase) ;
    iounmap ((Pvoid) halObject->perCmBase) ;
    iounmap ((Pvoid) halObject->iva2PrmBase) ;
    iounmap ((Pvoid) halObject->mailboxBase) ;

    TRC_1LEAVE ("OMAP3530_phyShmemExit", status) ;

    return status ;
}


#if defined (__cplusplus)

}
#endif
