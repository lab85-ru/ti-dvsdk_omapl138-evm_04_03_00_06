/** ============================================================================
 *  @file   scale.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/scale/
 *
 *  @desc   Defines the configurable parameters for the scale test which
 *          sends a buffer across the DSP processor and receives a buffer back
 *          using DSP/BIOS LINK. The DSP scales the returned buffer based on
 *          the control message sent by the GPP.
 *          It also does the data verification on the received buffer.
 *          In this application the transfer is done using only one buffer and
 *          no queueing of buffers is done.
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


#if !defined (SCALE_H)
#define SCALE_H

#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  CHNL_ID_OUTPUT
 *
 *  @desc   ID of channel used to send data to DSP.
 *  ============================================================================
 */
#define CHNL_ID_OUTPUT     0


/** ============================================================================
 *  @const  CHNL_ID_INPUT
 *
 *  @desc   ID of channel used to receive data from DSP.
 *  ============================================================================
 */
#define CHNL_ID_INPUT      1


/** ============================================================================
 *  @func   SCALE_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    bufferSize
 *              String representation of buffer size to be used
 *              for data transfer.
 *  @arg    strNumIterations
 *              Number of iterations a data buffer is transferred between
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
 *  @see    SCALE_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SCALE_Create (IN Char8 * dspExecutable,
              IN Char8 * strBufferSize,
              IN Char8 * strNumIterations,
              IN Uint8   processorId) ;


/** ============================================================================
 *  @func   SCALE_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of iterations a data buffer is transferred between
 *              GPP and DSP.
 *  @arg    processorId
 *              ID of the DSP processor
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              SCALE execution failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    SCALE_Delete , SCALE_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
SCALE_Execute (IN Uint32 numIterations, Uint8 processorId ) ;


/** ============================================================================
 *  @func   SCALE_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          SCALE_Create ().
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
 *  @see    SCALE_Create
 *  ============================================================================
 */
NORMAL_API
Void
SCALE_Delete (Uint8 processorId) ;


/** ============================================================================
 *  @func   SCALE_Main
 *
 *  @desc   The OS independent driver function for the scale sample application.
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
 *  @see    SCALE_Create, SCALE_Execute, SCALE_Delete
 *  ============================================================================
 */
NORMAL_API
Void
SCALE_Main (IN Char8 * dspExecutable,
            IN Char8 * strBuffersize,
            IN Char8 * strNumIterations,
            IN Char8 * strProcessorId) ;


#if defined (DA8XXGEM)
/** ============================================================================
 *  @func   SCALE_Main_DA8XX
 *
 *  @desc   The OS independent driver function for the scale sample application.
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
 *  @see    SCALE_Create, SCALE_Execute, SCALE_Delete
 *  ============================================================================
 */
NORMAL_API
Void
SCALE_Main_DA8XX (IN Char8 * dspExecutable,
                  IN Char8 * strBuffersize,
                  IN Char8 * strNumIterations,
                  IN Char8 * strProcessorId,
                  IN Char8 * strDspAddr,
                  IN Char8 * strShmAddr,
                  IN Char8 * strArgsAddr) ;
#endif


/** ============================================================================
 *  @func   SCALE_0Print
 *
 *  @desc   Print a message without any arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\scale_os.c
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
SCALE_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   SCALE_1Print
 *
 *  @desc   Print a message with one arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\scale_os.c
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
SCALE_1Print (Char8 * str, Uint32 arg) ;


/** ============================================================================
 *  @func   SCALE_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\scale_os.c
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
SCALE_Sleep (Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SCALE_H) */
