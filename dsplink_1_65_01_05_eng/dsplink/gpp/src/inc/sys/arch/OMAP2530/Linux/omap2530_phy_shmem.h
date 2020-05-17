/** ============================================================================
 *  @file   omap2530_phy_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/OMAP2530/Linux/
 *
 *  @desc   Physical Interface Abstraction Layer for OMAP2530.
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


#if !defined (OMAP2530_PHY_SHMEM_H)
#define OMAP2530_PHY_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


#if defined (__cplusplus)
extern "C" {
#endif

/*  ============================================================================
 *  @const  GENERAL_CONTROL_BASE/MAILBOX_BASE/MMU_BASE/PRCM_BASE
 *
 *  @desc   Base address of different Peripherals.
 *  ============================================================================
 */
#define GENERAL_CONTROL_BASE   0x49002270
#define MAILBOX_BASE           0x48094000
#define MMU_BASE               0x5D000000
#define MMU_SIZE                   0x1000
#define PRCM_BASE              0x49006000

/** ============================================================================
 *  @name   OMAP2530_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
extern HAL_Interface OMAP2530_shmemInterface ;


/* ============================================================================
 *  @func   OMAP2530_phyShmemInit
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
OMAP2530_phyShmemInit (IN Pvoid halObj) ;


/* ============================================================================
 *  @func   OMAP2530_phyShmemExit
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
OMAP2530_phyShmemExit (IN Pvoid halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (OMAP2530_PHY_SHMEM_H) */
