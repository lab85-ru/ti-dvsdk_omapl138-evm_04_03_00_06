/** ============================================================================
 *  @file   dm648_hal_pci_map.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM648/
 *
 *  @desc   Map module header file.
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


#if !defined (DM648_HAL_PCI_MAP_H)
#define DM648_HAL_PCI_MAP_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <dm648_hal.h>
#include <dm648_hal_pci.h>


#if defined (__cplusplus)
extern "C" {
#endif /* if defined (__cplusplus) */


/* ============================================================================
 *  @func   DM648_halPciMapCtrl
 *
 *  @desc   Mapping controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  MAP Command.
 *  @arg    dspBufAddr.
 *                  DSP address to be mapped.
 *  @arg    size.
 *                  Size of area to be mapped.
 *  @arg    bufVirtPtr.
 *                  virtual address of mapped area.
 *  @arg    bufPhysAddr.
 *                  Physical address of mapped area in HOST address space.
 *  @arg    mappedSize.
 *              Size of memory region mapped.
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
DM648_halPciMapCtrl (IN      Pvoid           halObject,
                     IN      DSP_MapCtrlCmd  cmd,
                     IN      Uint32          dspBufAddr,
                     IN      Uint32          size,
                     OUT     Uint32 *        bufVirtPtr,
                     OUT     Uint32 *        bufPhysAddr,
                     OUT     Uint32 *        mappedSize) ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM648_HAL_PCI_MAP_H) */
