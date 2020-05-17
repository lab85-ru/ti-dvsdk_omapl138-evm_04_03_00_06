/** ============================================================================
 *  @file   dm6437_hal_pci_intgen.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6437/
 *
 *  @desc   Interrupt Generation module header file.
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


#if !defined (DM6437_HAL_PCI_INTGEN_H)
#define DM6437_HAL_PCI_INTGEN_H


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
 *  @func   DM6437_halPciIntCtrl
 *
 *  @desc   Interrupt Controller.
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    cmd.
 *                  Boot Command.
 *  @arg    intId
 *                  Interrupt Id.
 *  @arg    arg.
 *                  Command specific argument (Optional).
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EINVALIDARG
 *              Unsupported interrupt control command.
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
DM6437_halPciIntCtrl (IN         Pvoid          halObject,
                      IN         DSP_IntCtrlCmd cmd,
                      IN         Uint32         intId,
                      IN OUT     Pvoid          arg) ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM6437_HAL_PCI_INTGEN_H) */
