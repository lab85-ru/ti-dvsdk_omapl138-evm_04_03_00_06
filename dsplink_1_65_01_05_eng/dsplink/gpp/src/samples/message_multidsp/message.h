/** ============================================================================
 *  @file   message.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/message_multidsp/
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



/*  ============================================================================
 *  @const   ID_DSP_PROCESSOR_0
 *
 *  @desc   ID of the first dsp processor.
 *  ============================================================================
 */
#define ID_DSP_PROCESSOR_0           0


/*  ============================================================================
 *  @const   ID_DSP_PROCESSOR_1
 *
 *  @desc   ID of the second dsp processor.
 *  ============================================================================
 */
#define ID_DSP_PROCESSOR_1           1


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable_0
 *              DSP executable name to be loaded on to DSP processor 0.
 *  @arg    dspExecutable_1
 *              DSP executable name to be loaded on to DSP processor 1.
 *  @arg    strNumIterations
 *              Number of iterations for which a message is transferred between
 *              GPP and DSP in string format.
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource allocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTIDSP_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTIDSP_Create (IN Char8 * dspExecutable_0,
                        IN Char8 * dspExecutable_1,
                        IN Char8 * strNumIterations) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Execute
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
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTIDSP_Delete , MESSAGEMULTIDSP_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MESSAGEMULTIDSP_Execute (IN Uint32 numIterations) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MESSAGEMULTIDSP_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              Resource deallocation failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTIDSP_Create
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTIDSP_Delete (Void) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Main
 *
 *  @desc   The OS independent driver function for the MESSAGEMULTIDSP sample
 *          application.
 *
 *  @arg    dspExecutable_0
 *              DSP executable name to be loaded on to DSP processor 0.
 *  @arg    dspExecutable_1
 *              DSP executable name to be loaded on to DSP processor 1.
 *  @arg    strNumIterations
 *              Number of iterations a MESSAGEMULTIDSP is transferred between
 *              GPP and DSP in string format.
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MESSAGEMULTIDSP_Create, MESSAGEMULTIDSP_Execute,
 *          MESSAGEMULTIDSP_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MESSAGEMULTIDSP_Main (IN Char8 * dspExecutable_0,
                      IN Char8 * dspExecutable_1,
                      IN Char8 * strNumIterations) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_0Print
 *
 *  @desc   Print a MESSAGEMULTIDSP without any arguments.
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
MESSAGEMULTIDSP_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_1Print
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
MESSAGEMULTIDSP_1Print (Char8 * str, Uint32 arg) ;


/** ============================================================================
 *  @func   MESSAGEMULTIDSP_Sleep
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
MESSAGEMULTIDSP_Sleep (Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MESSAGE_H) */
