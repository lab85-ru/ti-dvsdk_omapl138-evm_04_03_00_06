/** ============================================================================
 *  @file   mapregion.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mapregion/
 *
 *  @desc   Defines the interface used for driving the mapregion
 *          sample application.
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


#if !defined (MAPREGION_H)
#define MAPREGION_H

/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   MAPREGION_Main
 *
 *  @desc   The OS independent driver function for the sample application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strBufferSize
 *              Buffer size to be used for data-transfer in string format.
 *  @arg    bufferSize
 *              Buffer size to be used for data-transfer in numeric format.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *  @arg    numIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in numeric format.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_Main (IN Char8 * dspExecutable,
                IN Char8 * strBufferSize,
                IN Uint32  bufferSize,
                IN Char8 * strNumIterations,
                IN Uint32  numIterations,
                IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MAPREGION_0Print
 *
 *  @desc   Print a message without any arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\MAPREGION_os.c
 *
 *  @arg    str
 *              String message to be printed.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_0Print (IN Char8 * str) ;


/** ============================================================================
 *  @func   MAPREGION_1Print
 *
 *  @desc   Print a message with one arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\mapregion_os.c
 *
 *  @arg    str
 *              String message to be printed.
 *  @arg    arg
 *              Argument to be printed.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_1Print (IN Char8 * str, IN Uint32 arg) ;


/** ============================================================================
 *  @func   MAPREGION_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\MAPREGION_os.c
 *
 *  @arg    uSec
 *              Microseconds to sleep.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Void
MAPREGION_Sleep (IN Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MAPREGION_H) */

