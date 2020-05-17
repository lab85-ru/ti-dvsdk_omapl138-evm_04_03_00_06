/** ============================================================================
 *  @file   dm6437_hal_vlynq_rdwr.h
 *
 *  @path   $(DSPLINK)/gpp/src/inc/sys/arch/DM6437/
 *
 *  @desc   Read Write module header file.
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


#if !defined (DM6437_HAL_VLYNQ_RDWR_H)
#define DM6437_HAL_VLYNQ_RDWR_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>
#include <_dsplink.h>

/*  ----------------------------------- OSAL Headers                */
#include <dm6437_hal.h>
#include <dm6437_hal_vlynq.h>


#if defined (__cplusplus)
extern "C" {
#endif /* if defined (__cplusplus) */


/* ============================================================================
 *  @func   DM6437_halVlynqRead
 *
 *  @desc   Reads a buffer from dsp memory (for now this is done as bytes).
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
DM6437_halVlynqRead (IN  Pvoid          halObject,
                     IN  Uint32         dspAddr,
                     IN  Uint32         cBytes,
                     OUT Char8 *        readBuffer) ;


/* ============================================================================
 *  @func   DM6437_halVlynqWrite
 *
 *  @desc   Write a buffer to dsp memory (for now this is done as bytes).
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
DM6437_halVlynqWrite (IN  Pvoid          halObject,
                      IN  Uint32         dspAddr,
                      IN  Uint32         cBytes,
                      IN  Char8 *        writeBuffer) ;


#if defined (__cplusplus)
}
#endif /* if defined (__cplusplus) */

#endif /* !defined (DM6437_HAL_VLYNQ_RDWR_H) */
