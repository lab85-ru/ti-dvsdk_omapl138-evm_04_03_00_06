/** ============================================================================
 *  @file   dm6467gem_phy_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6467GEM/Linux/
 *
 *  @desc   Physical Interface Abstraction Layer for DM6467Gem.
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


#if !defined (DM6467GEM_PHY_SHMEM_H)
#define DM6467GEM_PHY_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DM6467GEM_shmemInterface
 *
 *  @desc   Interface functions exported by the Shmem Driver subcomponent.
 *  ============================================================================
 */
extern HAL_Interface DM6467GEM_shmemInterface ;


/* ============================================================================
 *  @func   DM6467GEM_phyShmemInit
 *
 *  @desc   Initializes Shmem Driver/device.
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
DM6467GEM_phyShmemInit (IN Pvoid halObj) ;


/* ============================================================================
 *  @func   DM6467GEM_phyShmemExit
 *
 *  @desc   Finalizes Shmem Driver/device.
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
DM6467GEM_phyShmemExit (IN Pvoid halObj) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM6467GEM_PHY_SHMEM_H) */
