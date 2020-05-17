/** ============================================================================
 *  @file   message_multi.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multi/
 *
 *  @desc   This application sends messages to the DSP processor and receives
 *          them back using DSP/BIOS LINK.
 *          The message contents received are verified against the data
 *          sent to DSP.
 *          This is a multi-application message sample. It takes the application
 *          number as a second parameter, and accordingly opens and
 *          locates the corresponding message queues, performing message
 *          transfers with the DSP.
 *          It also demonstrates usage of dynamic configuration.
 *          This application also opens one separate pool for each application
 *          instance.
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
 *  @const  MAX_APPS
 *
 *  @desc   Maximum number of application instances that can be used.
 *  ============================================================================
 */
#define MAX_APPS           16


/** ============================================================================
 *  @func   MESSAGEMULTI_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    strNumIterations
 *              Number of iterations for which a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    strAppNum
 *              The sequence number of the application.
 *
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
 *  @see    MESSAGEMULTI_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_Create (IN Char8 * dspExecutable,
                     IN Char8 * strNumIterations,
                     IN Char8 * strAppNum,
                     IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of times to send the message to the DSP.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              MESSAGE execution failed.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTI_Delete , MESSAGEMULTI_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTI_Execute (IN Uint32 numIterations, IN Uint8 processorId ) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGEMULTI_Create ().
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
 *  @see    MESSAGEMULTI_Create
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Delete (IN Uint8 processorId) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_Main
 *
 *  @desc   The OS independent driver function for the MESSAGE sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    strAppNum
 *              The sequence number of the application.
 *
 *  @arg    strProcessorId
 *              ID of the DSP processor in string format.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTI_Create, MESSAGEMULTI_Execute, MESSAGEMULTI_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Main (IN Char8 * dspExecutable,
                   IN Char8 * strNumIterations,
                   IN Char8 * strAppNum,
                   IN Char8 * strProcessorId) ;


#if defined (DA8XXGEM)
/** ============================================================================
 *  @func   MESSAGEMULTI_Main_DA8XX
 *
 *  @desc   The OS independent driver function for the MESSAGE sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a message is transferred between
 *              GPP and DSP in string format.
 *  @arg    strAppNum
 *              The sequence number of the application.
 *  @arg    strDspAddr
 *              c_int00 address
 *  @arg    strShmAddr
 *              DSPLINK_shmBaseAddress address
 *  @arg    strArgsAddr
 *              .args section address
 *
 *  @arg    strProcessorId
 *              ID of the DSP processor in string format.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTI_Create, MESSAGEMULTI_Execute, MESSAGEMULTI_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTI_Main_DA8XX (IN Char8 * dspExecutable,
                         IN Char8 * strNumIterations,
                         IN Char8 * strAppNum,
                         IN Char8 * strProcessorId,
                         IN Char8 * strDspAddr,
                         IN Char8 * strShmAddr,
                         IN Char8 * strArgsAddr) ;
#endif


/** ============================================================================
 *  @func   MESSAGEMULTI_0Print
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
MESSAGEMULTI_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_1Print
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
MESSAGEMULTI_1Print (Char8 * str, Uint32 arg) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_2Print
 *
 *  @desc   Print a message with two arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\message_os.c
 *
 *  @arg    str
 *              String message to be printed.
 *  @arg    arg1
 *              First argument to be printed.
 *  @arg    arg2
 *              Second argument to be printed.
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
MESSAGEMULTI_2Print (Char8 * str, Uint32 arg1, Uint32 arg2) ;


/** ============================================================================
 *  @func   MESSAGEMULTI_Sleep
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
MESSAGEMULTI_Sleep (Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MESSAGE_H) */
