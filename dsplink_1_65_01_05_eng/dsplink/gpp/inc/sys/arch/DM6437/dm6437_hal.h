/** ============================================================================
 *  @file   dm6437_hal.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/DM6437/
 *
 *  @desc   Hardware Abstraction Layer for 64LC_PCI.
 *          Define the Platform specific HAL (Hardware Abstraction Layer)
 *          object.
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (DM6437_HAL_H)
#define DM6437_HAL_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <hal.h>
#if (DM6437_PHYINTERFACE == PCI_INTERFACE)
#include <dm6437_hal_pci.h>
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE) */
#if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE)
#include <dm6437_hal_vlynq.h>
#endif /* if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE) */

#if defined (__cplusplus)
extern "C" {
#endif


#if (DM6437_PHYINTERFACE == PCI_INTERFACE)
/** ============================================================================
 *  @name   DM6437_HalPciPhyObj
 *
 *  @desc   Forward declaration of PCI Physical Interface object.
 *  ============================================================================
 */
typedef struct DM6437_HalPciPhyObj_tag DM6437_HalPciPhyObj ;
#endif /* if (DM6437_PHYINTERFACE == PCI_INTERFACE) */


#if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE)
/** ============================================================================
 *  @name   DM6437_HalVlynqPhyObj
 *
 *  @desc   Forward declaration of VLYNQ Physical Interface object.
 *  ============================================================================
 */
typedef struct DM6437_HalVlynqPhyObj_tag DM6437_HalVlynqPhyObj ;
#endif /* if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE) */

/** ============================================================================
 *  @name   DM6437_HalObj
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
 *  @field  pci
 *              Structure containging members related to PCI Phy.
 *              Only on PCI platforms
 *  @field  lock
 *              Lock for using DMA resource.
 *  @field  vlynq
 *              Structure containging members related to VLYNQ Phy.
 *              Only on VLYNQ Platforms
 *  @field  shMemDspAddr
 *              Shared memory base address in DSP address space.
 *  @field  shMemVirtAddr
 *              Shared memory virtual base address .
 *  @field  shMemPhysAddr
 *              Shared memory physical base address .
 *  ============================================================================
 */
typedef struct DM6437_HalObj_tag {
    ProcessorId           dspId ;
    Uint32                phyType            ;
    HAL_Interface *       interface          ;
    Bool                  isMapActive        ;
    Uint32                mapSize            ;
    Uint32                shMemDspAddr  ;
    Uint32                shMemVirtAddr ;
    Uint32                shMemPhysAddr ;
    Pvoid                 lock          ;
#if (DM6437_PHYINTERFACE == PCI_INTERFACE)
    DM6437_HalPciPhyObj   pci                ;
#endif /* (DM6437_PHYINTERFACE == PCI_INTERFACE) */
#if (DM6437_PHYINTERFACE == VLYNQ_INTERFACE)
    DM6437_HalVlynqPhyObj vlynq              ;
#endif /* (DM6437_PHYINTERFACE == VLYNQ_INTERFACE) */
} DM6437_HalObj ;


/** ============================================================================
 *  @name   DM6437_InitParams
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
 *  @field  arg5
 *              Argument 6.
 *  ============================================================================
 */
typedef struct DM6437_InitParams_tag {
    ProcessorId dspId   ;
    Uint32      phyType ;
    Uint32      shMemEntry ;
    Uint32      arg1    ;
    Uint32      arg2    ;
    Uint32      arg3    ;
    Uint32      arg4    ;
    Uint32      arg5    ;
    Uint32      arg6    ;
} DM6437_InitParams ;


/** ============================================================================
 *  @func   DM6437_halInit
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
 *          DSP_EMEMORY
 *              Out of memory error.
 *          DSP_EINVALIDARG
 *              Invalid argument.
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
DM6437_halInit (IN Pvoid * halObject, IN Pvoid initParams) ;


/** ============================================================================
 *  @func   DM6437_halExit
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
DM6437_halExit (IN Pvoid * halObject) ;


#if defined (__cplusplus)
}
#endif


#endif  /* !defined (DM6437_HAL_H) */
