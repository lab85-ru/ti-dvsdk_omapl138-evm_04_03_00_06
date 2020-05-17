/** ============================================================================
 *  @file   dm648_hal.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM648/
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


#if !defined (DM648_HAL_H)
#define DM648_HAL_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <hal.h>
#if (DM648_PHYINTERFACE == PCI_INTERFACE)
#include <dm648_hal_pci.h>
#endif /* if (DM648_PHYINTERFACE == PCI_INTERFACE) */

#if defined (__cplusplus)
extern "C" {
#endif


#if (DM648_PHYINTERFACE == PCI_INTERFACE)
/** ============================================================================
 *  @name   DM648_HalPciPhyObj
 *
 *  @desc   Forward declaration of PCI Physical Interface object.
 *  ============================================================================
 */
typedef struct DM648_HalPciPhyObj_tag DM648_HalPciPhyObj ;
#endif /* if (DM648_PHYINTERFACE == PCI_INTERFACE) */


/** ============================================================================
 *  @name   DM648_HalObj
 *
 *  @desc   Hardware Abstraction object.
 *
 *  @field  dspId
 *              Processor Identifier.
 *  @field  phyType
 *              Physical Interface type.
 *  @field  interface
 *              Function table for hal functions.
 *  @field  isMapActive
 *              tells whether user has mapped some area.
 *  @field  mapSize
 *              Size of the mapped area.
 *  @field  lock
 *              Lock for using DMA resource.
 *  @field  pci
 *              Structure containging members related to PCI Phy.
 *              Only on PCI platforms
 *  ============================================================================
 */
typedef struct DM648_HalObj_tag {
    ProcessorId           dspId ;
    Uint32                phyType            ;
    HAL_Interface *       interface          ;
    Bool                  isMapActive        ;
    Uint32                mapSize            ;
    Pvoid                 lock               ;
#if (DM648_PHYINTERFACE == PCI_INTERFACE)
    DM648_HalPciPhyObj   pci                ;
#endif /* (DM648_PHYINTERFACE == PCI_INTERFACE) */
} DM648_HalObj ;


/** ============================================================================
 *  @name   DM648_InitParams
 *
 *  @desc   Initialize parameters for HAL init call.
 *
 *  @field  dspId
 *              Processor Identifier.
 *  @field  phyType
 *              Physical Interface type.
 *  @field  shMemEntry
 *              Shared memory entry ID, this is mapped after initialzing the
 *              physical interface.
 *  @field  arg1
 *              Argument 1.
 *  @field  arg2
 *              Argument 2.
 *  @field  arg3
 *              Argument 3.
 *  @field  arg4
 *              Argument 4.
 *  @field  arg5
 *              Argument 5.
 *  @field  arg6
 *              Argument 6.
 *  ============================================================================
 */
typedef struct DM648_InitParams_tag {
    ProcessorId dspId   ;
    Uint32      phyType ;
    Uint32      shMemEntry ;
    Uint32      arg1    ;
    Uint32      arg2    ;
    Uint32      arg3    ;
    Uint32      arg4    ;
    Uint32      arg5    ;
    Uint32      arg6    ;
} DM648_InitParams ;


/** ============================================================================
 *  @func   DM648_halInit
 *
 *  @desc   Initializes the HAL object and physical interface.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    initParams.
 *                  Parameters for initialization.
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
DM648_halInit (IN Pvoid * halObject, IN Pvoid initParams) ;


/** ============================================================================
 *  @func   DM648_halExit
 *
 *  @desc   Finializes the HAL object and physical interface.
 *
 *  @arg    halObject.
 *                  HAL Object
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
DM648_halExit (IN Pvoid * halObject) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM648_HAL_H) */
