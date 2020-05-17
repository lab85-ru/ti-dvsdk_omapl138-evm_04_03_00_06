/** ============================================================================
 *  @file   omap3530_phy_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP3530/Linux/
 *
 *  @desc   Physical Interface Abstraction Layer for OMAP3530.
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


#if !defined (OMAP3530_PHY_SHMEM_H)
#define OMAP3530_PHY_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


#if defined (__cplusplus)
extern "C" {
#endif

/*  ============================================================================
 *  @const  GENERAL_CONTROL_BASE/IVA2_CM_BASE/CORE_CM_BASE/PER_CM_BASE
 *          MAILBOX_BASE/MMU_BASE
 *
 *  @desc   Base address of different Peripherals.
 *  ============================================================================
 */
#define GENERAL_CONTROL_BASE   0x48002270
#define IVA2_CM_BASE           0x48004000
#define CORE_CM_BASE           0x48004A00
#define PER_CM_BASE            0x48005000
#define IVA2_PRM_BASE          0x48306000
#define MAILBOX_BASE           0x48094000
#define MMU_BASE               0x5D000000
#define MMU_SIZE                   0x1000


/** ============================================================================
 *  @name   OMAP3530_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
extern HAL_Interface OMAP3530_shmemInterface ;


/* ============================================================================
 *  @func   OMAP3530_phyShmemInit
 *
 *  @desc   Initializes Shared Driver/device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_phyShmemInit (IN Pvoid halObj) ;


/* ============================================================================
 *  @func   OMAP3530_phyShmemExit
 *
 *  @desc   Finalizes Shared Driver/device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              All other error conditions.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
OMAP3530_phyShmemExit (IN Pvoid halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP3530_PHY_SHMEM_H) */
