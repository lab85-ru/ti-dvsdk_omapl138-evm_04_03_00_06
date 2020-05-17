/** ============================================================================
 *  @file   tnetv107xgem_phy_shmem.c
 *
 *  @path   $(DSPLINK)/gpp/src/arch/TNETV107XGEM/shmem/Linux/
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
#include <tnetv107xgem_hal.h>
#include <tnetv107xgem_hal_boot.h>
#include <tnetv107xgem_hal_intgen.h>
#include <tnetv107xgem_hal_pwr.h>
#include <tnetv107xgem_phy_shmem.h>


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
 *  @name   TNETV107XGEM_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
HAL_Interface TNETV107XGEM_shmemInterface = {&TNETV107XGEM_phyShmemInit,
                                          &TNETV107XGEM_phyShmemExit,
                                          &TNETV107XGEM_halBootCtrl,
                                          &TNETV107XGEM_halIntCtrl,
                                          NULL,
                                          NULL,
                                          &TNETV107XGEM_halPwrCtrl,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL} ;

/* ============================================================================
 *  @func   TNETV107XGEM_phyShmemInit
 *
 *  @desc   Initializes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_phyShmemInit (IN Pvoid halObj)
{
    DSP_STATUS        status    = DSP_SOK ;
    TNETV107XGEM_HalObj * halObject = NULL    ;

    TRC_1ENTER ("TNETV107XGEM_phySharedInit", halObj) ;

    DBC_Require (NULL != halObj) ;

    halObject = (TNETV107XGEM_HalObj *) halObj ;

    halObject->intdBase     = IO_ADDRESS(INTD_BASE)  ;
    halObject->cfgBase      = IO_ADDRESS(CHIP_CFG_BASE) ;
    halObject->clkCtrlBase  = IO_ADDRESS(CLK_CTRL_BASE) ;

    TRC_1LEAVE ("TNETV107XGEM_phySharedInit", status) ;

    return status ;
}


/* ============================================================================
 *  @func   TNETV107XGEM_phyShmemExit
 *
 *  @desc   Finalizes Shared/Davinci Gem device.
 *
 *  @modif  None.
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
TNETV107XGEM_phyShmemExit (IN Pvoid halObj)
{
    DSP_STATUS     status = DSP_SOK ;

    TRC_1ENTER ("TNETV107XGEM_phySharedExit", halObj) ;

    DBC_Require (NULL != halObj) ;

    (void) halObj ;

    TRC_1LEAVE ("TNETV107XGEM_phySharedExit", status) ;

    return status ;
}


#if defined (__cplusplus)

}
#endif
