/** ============================================================================
 *  @file   dm648_hal_pci_rdwr.h
 *
 *  @path   $(DSPLINK)/gpp/inc/sys/arch/DM648/
 *
 *  @desc   Read Write module header file.
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


#if !defined (DM648_HAL_PCI_RDWR_H)
#define DM648_HAL_PCI_RDWR_H


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
 *  @func   DM648_halPciRead
 *
 *  @desc   Reads a buffer from dsp memory .
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    dspAddr.
 *                  DSP Address to be used as read address.
 *  @arg    cBytes.
 *                  Number of bytes to transfer.
 *  @arg    readBuffer.
 *                  Destination buffer.
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
DM648_halPciRead (IN  Pvoid          halObject,
                  IN  Uint32         dspAddr,
                  IN  Uint32         cBytes,
                  OUT Char8 *        readBuffer) ;

/* ============================================================================
 *  @func   DM648_halPciWrite
 *
 *  @desc   Write a buffer to dsp memory .
 *
 *  @arg    halObject.
 *                  HAL Object
 *  @arg    dspAddr.
 *                  DSP Address to be used as read address.
 *  @arg    cBytes.
 *                  Number of bytes to transfer.
 *  @arg    writeBuffer.
 *                  Source buffer.
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
DM648_halPciWrite (IN  Pvoid          halObject,
                   IN  Uint32         dspAddr,
                   IN  Uint32         cBytes,
                   IN  Char8 *        writeBuffer) ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM648_HAL_PCI_RDWR_H) */
