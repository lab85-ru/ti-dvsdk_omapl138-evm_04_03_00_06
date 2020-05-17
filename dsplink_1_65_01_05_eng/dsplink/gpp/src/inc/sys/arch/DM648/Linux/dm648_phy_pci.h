/** ============================================================================
 *  @file   dm648_phy_pci.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM648/Linux/
 *
 *  @desc   Hardware Abstraction Layer for DM648.
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


#if !defined (DM648_PHY_PCI_H)
#define DM648_PHY_PCI_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>

/*  ----------------------------------- Hardware Abstraction Layer  */
#include <hal.h>
#include <dm648_hal.h>
#include <dm648_phy_pci.h>


#if defined (__cplusplus)
extern "C" {
#endif


/** ============================================================================
 *  @name   DM648_pciInterface
 *
 *  @desc   Interface functions exported by the PCI subcomponent.
 *  ============================================================================
 */
extern HAL_Interface DM648_pciInterface ;


/* ============================================================================
 *  @func   DM648_phyPciInit
 *
 *  @desc   Initializes PCI device.
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
DM648_phyPciInit (IN Pvoid halObject) ;


/* ============================================================================
 *  @func   DM648_phyPciExit
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
DM648_phyPciExit (IN Pvoid halObject) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM648_PHY_PCI_H) */
