/** ============================================================================
 *  @file   omap2530_phy_shmem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/OMAP2530/shmem/Linux/
 *
 *  @desc   Linux shmem driver interface file.
 *  @rev    00.04
 *  ============================================================================
 *  Copyright (c) Texas Instruments Incorporated 2002-2007
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
#include <omap2530_hal.h>
#include <omap2530_hal_boot.h>
#include <omap2530_hal_intgen.h>
#include <omap2530_hal_pwr.h>
#include <omap2530_hal_mmu.h>
#include <omap2530_phy_shmem.h>


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
 *  @name   OMAP2530_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
HAL_Interface OMAP2530_shmemInterface = {&OMAP2530_phyShmemInit,
                                          &OMAP2530_phyShmemExit,
                                          &OMAP2530_halBootCtrl,
                                          &OMAP2530_halIntCtrl,
                                          &OMAP2530_halMmuCtrl,
                                          NULL,
                                          &OMAP2530_halPwrCtrl,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL} ;


/* ============================================================================
 *  @func   OMAP2530_phyShmemInit
 *
 *  @desc   Initializes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_phyShmemInit (IN Pvoid halObj)
{
    DSP_STATUS        status    = DSP_SOK ;
    OMAP2530_HalObj * halObject = NULL    ;
    MemMapInfo  mapInfo ;

    TRC_1ENTER ("OMAP2530_phyShmemInit", halObj) ;

    DBC_Require (NULL != halObj) ;

    halObject = (OMAP2530_HalObj *) halObj ;

    halObject->generalCtrlBase = IO_ADDRESS(GENERAL_CONTROL_BASE) ;
    halObject->mailboxBase     = IO_ADDRESS(MAILBOX_BASE) ;
    halObject->prcmBase        = IO_ADDRESS(PRCM_BASE) ;

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

    TRC_1LEAVE ("OMAP2530_phyShmemInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   OMAP2530_phyShmemExit
 *
 *  @desc   Finalizes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP2530_phyShmemExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;
    OMAP2530_HalObj * halObject = (OMAP2530_HalObj *) halObj ;
    MemUnmapInfo      unmapInfo                              ;

    TRC_1ENTER ("OMAP2530_phyShmemExit", halObj) ;

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

    TRC_1LEAVE ("OMAP2530_phyShmemExit", status) ;

    return status ;
}


#if defined (__cplusplus)

}
#endif
