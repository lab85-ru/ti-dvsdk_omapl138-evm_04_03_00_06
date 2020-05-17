/** ============================================================================
 *  @file   dm6437_phy_pci.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6437/Linux/
 *
 *  @desc   Hardware Abstraction Layer for DM6437.
 *          Define the Platform specific HAL (Hardware Abstraction Layer)
 *          object.
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


#if !defined (DM6437_PHY_PCI_H)
#define DM6437_PHY_PCI_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>
#include <dm6437_hal.h>
#include <dm6437_phy_pci.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DM6437_pciInterface
 *
 *  @desc   Interface functions exported by the PCI subcomponent.
 *  ============================================================================
 */
extern HAL_Interface DM6437_pciInterface ;


/* ============================================================================
 *  @func   DM6437_phyPciInit
 *
 *  @desc   Initializes PCI device.
 *
 *  @arg    halObject.
 *              HAL object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_ENOTFOUND
 *              Specified PCI card was not found at the particular BUS and slot.
 *          DSP_EFAIL
 *              A general failure occurred.
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
DM6437_phyPciInit (IN Pvoid halObject) ;


/* ============================================================================
 *  @func   DM6437_phyPciExit
 *
 *  @desc   Finalizes PCI device.
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
DM6437_phyPciExit (IN Pvoid halObject) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM6437_PHY_PCI_H) */
