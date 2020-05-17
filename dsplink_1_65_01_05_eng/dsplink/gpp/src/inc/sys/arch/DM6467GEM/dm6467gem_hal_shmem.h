/** ============================================================================
 *  @file   dm6467gem_hal_shmem.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6467GEM/
 *
 *  @desc   Hardware Abstraction Layer for Shared memory interface.
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


#if !defined (DM6467GEM_HAL_SHMEM_H)
#define DM6467GEM_HAL_SHMEM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DM6467GEM_HalShmemPhyObj_tag
 *
 *  @desc   Hardware Abstraction Phy Interface object.
 *
 *  @field  baseCfgBus
 *              Kernel Virtual base address of the configuration bus peripherals
 *              memory range.
 *  @field  offsetSysModule
 *              Offset of the system module from CFG base.
 *  ============================================================================
 */
struct DM6467GEM_HalShmemPhyObj_tag {
    Uint32      baseCfgBus        ;
    Uint32      offsetSysModule   ;
} ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM6467GEM_HAL_SHMEM_H) */
