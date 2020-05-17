/** ============================================================================
 *  @file   tnetv107xgem_phy_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/TNETV107XGEM/Linux/
 *
 *  @desc   Physical Interface Abstraction Layer for TNETV107XGem.
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


#if !defined (TNETV107XGEM_PHY_SHMEM_H)
#define TNETV107XGEM_PHY_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*  ============================================================================
 *  @const  INTD_BASE/CHIP_CFG_BASE/CLK_CTRL_BASE
 *
 *  @desc   Base address of different Peripherals.
 *  ============================================================================
 */
#define INTD_BASE               0x08038000
#define CHIP_CFG_BASE           0x08087000
#define CLK_CTRL_BASE           0x0808A000


/** ============================================================================
 *  @name   TNETV107XGEM_shmemInterface
 *
 *  @desc   Interface functions exported by the Shared Driver subcomponent.
 *  ============================================================================
 */
extern HAL_Interface TNETV107XGEM_shmemInterface ;


/* ============================================================================
 *  @func   TNETV107XGEM_phyShmemInit
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
TNETV107XGEM_phyShmemInit (IN Pvoid halObj) ;


/* ============================================================================
 *  @func   TNETV107XGEM_phyShmemExit
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
TNETV107XGEM_phyShmemExit (IN Pvoid halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (TNETV107XGEM_PHY_SHMEM_H) */
