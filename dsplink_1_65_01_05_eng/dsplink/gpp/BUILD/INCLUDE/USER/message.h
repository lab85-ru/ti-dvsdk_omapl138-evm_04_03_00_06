/** ============================================================================
 *  @file   message.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message/
 *
 *  @desc   Defines the configurable parameters for the message test which
 *          sends a message across the DSP processor and receives it back
 *          using DSP/BIOS LINK.
 *          It also does the data verification on the received message.
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


#if !defined (MESSAGE_H)
#define MESSAGE_H


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   MESSAGE_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    strNumIterations
 *              Number of iterations for which a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    processorId
 *             Id of the DSP Processor. 
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
 *  @see    MESSAGE_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGE_Create (IN Char8 * dspExecutable, 
                IN Char8 * strNumIterations,
                IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MESSAGE_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of times to send the message to the DSP.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              MESSAGE execution failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGE_Delete , MESSAGE_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGE_Execute (IN Uint32 numIterations, IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MESSAGE_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGE_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
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
 *  @see    MESSAGE_Create
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Delete (IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MESSAGE_Main
 *
 *  @desc   The OS independent driver function for the MESSAGE sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    strProcessorId
 *             Id of the DSP Processor in string format. 
 *
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGE_Create, MESSAGE_Execute, MESSAGE_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Main (IN Char8 * dspExecutable, 
              IN Char8 * strNumIterations,
              IN Char8 * strProcessorId) ;


/** ============================================================================
 *  @func   MESSAGE_0Print
 *
 *  @desc   Print a message without any arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\message_os.c
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
MESSAGE_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   MESSAGE_1Print
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
MESSAGE_1Print (Char8 * str, Uint32 arg) ;


/** ============================================================================
 *  @func   MESSAGE_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\message_os.c
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
MESSAGE_Sleep (Uint32 uSec) ;

#if defined (DA8XXGEM)
/** ============================================================================
 *  @func   MESSAGE_Main_DA8XX
 *
 *  @desc   The OS independent driver function for the MESSAGE sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    strProcessorId
 *             Id of the DSP Processor in string format. 
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
 *  @see    MESSAGE_Create, MESSAGE_Execute, MESSAGE_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGE_Main_DA8XX (IN Char8 * dspExecutable,
                    IN Char8 * strNumIterations,
                    IN Char8 * strProcessorId,
                    IN Char8 * strDspAddr,
                    IN Char8 * strShmAddr,
                    IN Char8 * strArgsAddr) ;
#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MESSAGE_H) */
