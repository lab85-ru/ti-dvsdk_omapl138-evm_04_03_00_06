/** ============================================================================
 *  @file   ring_io.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/ring_io/
 *
 *  @desc   Defines the configurable parameters for the ring_io application,
 *          which exchanges data using a ring buffer shared between the GPP and
 *          the DSP.
 *          The contents of received buffer are verified against the data
 *          sent to DSP.
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


#if !defined (RING_IO_H)
#define RING_IO_H


/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   RING_IO_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    strBufferSize
 *              String representation of buffer size to be used
 *              for data transfer.
 *  @arg    numBytesToTransfer
 *              Number of bytes to be transferred between
 *              GPP and DSP in string format.
 *  @arg    processorId
 *              ID of the DSP processor
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource allocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    RING_IO_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_Create (IN Char8 * dspExecutable,
                IN Char8 * strBufferSize,
                IN Char8 * numBytesToTransfer,
                IN Uint8   processorId) ;


/** ============================================================================
 *  @func   RING_IO_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          RING_IO_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @arg    processorId
 *              ID of the DSP processor
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource deallocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    RING_IO_Create
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_Delete (IN Uint8 processorId) ;


/** ============================================================================
 *  @func   RING_IO_Main
 *
 *  @desc   The OS independent driver function for the RING_IO sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strBufferSize
 *              Buffer size to be used for data-transfer in string format.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *  @arg    strProcessorId
 *              ID of the DSP processor  in string format.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    RING_IO_Create, RING_IO_Execute, RING_IO_Delete
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_Main (IN Char8 * dspExecutable,
              IN Char8 * strBufferSize,
              IN Char8 * strNumIterations,
              IN Char8 * strProcessorId) ;


#if defined (DA8XXGEM)
/** ============================================================================
 *  @func   RING_IO_Main_DA8XX
 *
 *  @desc   The OS independent driver function for the RING_IO sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strBufferSize
 *              Buffer size to be used for data-transfer in string format.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP in string format.
 *  @arg    strProcessorId
 *              ID of the DSP processor  in string format.
 *  @arg    strDspAddr
 *              c_int00 address
 *  @arg    strShmAddr
 *              DSPLINK_shmBaseAddress address
 *  @arg    strArgsAddr
 *              .args section address
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    RING_IO_Create, RING_IO_Execute, RING_IO_Delete
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_Main_DA8XX (IN Char8 * dspExecutable,
                    IN Char8 * strBufferSize,
                    IN Char8 * strNumIterations,
                    IN Char8 * strProcessorId,
                    IN Char8 * strDspAddr,
                    IN Char8 * strShmAddr,
                    IN Char8 * strArgsAddr) ;
#endif


/** ============================================================================
 *  @func   RING_IO_0Print
 *
 *  @desc   Print a message without any arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\ring_io_os.c
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
RING_IO_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   RING_IO_1Print
 *
 *  @desc   Print a message with one arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\message_os.c
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
RING_IO_1Print (Char8 * str, Uint32 arg) ;

/** ============================================================================
 *  @func   RING_IO_YieldTsk
 *
 *  @desc   Yields tasks execution.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\ring_io_os.c
 *
 *  @modif  None
 *  ============================================================================
 */
NORMAL_API
Void
RING_IO_YieldClient (Void) ;
/** ============================================================================
 *  @func   RING_IO_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\ring_io_os.c
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
RING_IO_Sleep (Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (RING_IO_H) */
