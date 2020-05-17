/** ============================================================================
 *  @file   dm6437_hal_pci_dma.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6437/
 *
 *  @desc   DMA module header file.
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


#if !defined (DM6437_HAL_PCI_DMA_H)
#define DM6437_HAL_PCI_DMA_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <dm6437_hal.h>
#include <dm6437_hal_pci.h>


#if defined (__cplusplus)
extern "C" {
#endif /* if defined (__cplusplus) */


/* ============================================================================
 *  @func   DM6437_halPciReadDMA
 *
 *  @desc   DMAs contents from DSP memory to GPP Memory. Here read means DSP
 *          write.
 *
 *  @arg    halObject.
 *              HAL Object
 *  @arg    srcAddr.
 *              Source Address.
 *  @arg    dstAddr.
 *              Destination Address.
 *  @arg    size.
 *              Number of bytes to transfer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Not able to do read DMA for specified size.
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
DM6437_halPciReadDMA (IN Pvoid  halObject,
                      IN Uint32 srcAddr,
                      IN Uint32 dstAddr,
                      IN Uint32 size) ;


/* ============================================================================
 *  @func   DM6437_halPciWriteDMA
 *
 *  @desc   DMAs contents from GPP memory to DSP Memory. Here write means DSP
 *          read.
 *
 *  @arg    halObject.
 *              HAL Object
 *  @arg    srcAddr.
 *              Source Address.
 *  @arg    dstAddr.
 *              Destination Address.
 *  @arg    size.
 *              Number of bytes to transfer.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Not able to do write DMA for specified size.
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
DM6437_halPciWriteDMA (IN Pvoid  halObject,
                       IN Uint32 srcAddr,
                       IN Uint32 dstAddr,
                       IN Uint32 size) ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM6437_HAL_PCI_DMA_H) */
