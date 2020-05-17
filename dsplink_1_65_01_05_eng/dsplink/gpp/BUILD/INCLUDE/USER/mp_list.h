/** ============================================================================
 *  @file   mp_list.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/mp_list/
 *
 *  @desc   Defines the interface used for driving the MPLIST
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


#if !defined (MP_LIST_H)
#define MP_LIST_H

/*  ----------------------------------- DSP/BIOS Link                   */
#include <dsplink.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   MP_LIST_Create
 *
 *  @desc   This function allocates and initializes resources used by
 *          this application.
 *
 *  @arg    dspExecutable
 *              DSP executable name.
 *  @arg    strNumIterations
 *              Number of iterations for which a mp_list is transferred between
 *              GPP and DSP in string format.
 *  @arg    strNumElem
 *              Number of elements which should be added to tail of list for
 *              each iteration.
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
 *  @see    MP_LIST_Delete
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_Create (IN Char8 * dspExecutable,
                IN Char8 * strNumIterations,
                IN Char8 * strNumElem,
                IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MP_LIST_Execute
 *
 *  @desc   This function implements the execute phase for this application.
 *
 *  @arg    numIterations
 *              Number of iterations.
 *  @arg    numElem
 *              Number of elements which should be added to tail of list for
 *              each iteration.
 *
 *  @arg    processorId
 *             Id of the DSP Processor. 
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              MP_LIST execution failed.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MP_LIST_Delete , MP_LIST_Create
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
MP_LIST_Execute (IN Uint32 numIterations,
                 IN Uint32 numElem,
                 IN Uint8   processorId) ;


/** ============================================================================
 *  @func   MP_LIST_Delete
 *
 *  @desc   This function releases resources allocated earlier by call to
 *          MP_LIST_Create ().
 *          During cleanup, the allocated resources are being freed
 *          unconditionally. Actual applications may require stricter check
 *          against return values for robustness.
 *
 *  @arg    numElem
 *              Number of elements which should be deleted.
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
 *  @see    MP_LIST_Create
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Delete (IN Uint32 numElem, IN Uint8 processorId) ;


/** ============================================================================
 *  @func   MP_LIST_Main
 *
 *  @desc   The OS independent driver function for the MP_LIST sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a mp_list is transferred between
 *              GPP and DSP in string format.
 *  @arg    strNumElement
 *              Number of iterations to put at tail of list in string format.
 *  @arg    strProcessorId
 *              ID of the DSP processor in string format.
 *
 *  @ret    None
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    MP_LIST_Create, MP_LIST_Execute, MP_LIST_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Main (IN Char8 * dspExecutable,
              IN Char8 * strNumIterations,
              IN Char8 * strNumElem,
              IN Char8 * strProcessorId) ;


#if defined (DA8XXGEM)
/** ============================================================================
 *  @func   MP_LIST_Main
 *
 *  @desc   The OS independent driver function for the MP_LIST sample
 *          application.
 *
 *  @arg    dspExecutable
 *              Name of the DSP executable file.
 *  @arg    strNumIterations
 *              Number of iterations a mp_list is transferred between
 *              GPP and DSP in string format.
 *  @arg    strNumElement
 *              Number of iterations to put at tail of list in string format.
 *  @arg    strProcessorId
 *              ID of the DSP processor in string format.
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
 *  @see    MP_LIST_Create, MP_LIST_Execute, MP_LIST_Delete
 *  ============================================================================
 */
NORMAL_API
Void
MP_LIST_Main_DA8XX (IN Char8 * dspExecutable,
                    IN Char8 * strNumIterations,
                    IN Char8 * strNumElem,
                    IN Char8 * strProcessorId,
                    IN Char8 * strDspAddr,
                    IN Char8 * strShmAddr,
                    IN Char8 * strArgsAddr) ;
#endif


/** ============================================================================
 *  @func   MP_LIST_0Print
 *
 *  @desc   Print a mp_list without any arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\mp_list_os.c
 *
 *  @arg    str
 *              String mp_list to be printed.
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
MP_LIST_0Print (Char8 * str) ;


/** ============================================================================
 *  @func   MP_LIST_1Print
 *
 *  @desc   Print a mp_list with one arguments.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\mp_list_os.c
 *
 *  @arg    str
 *              String mp_list to be printed.
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
MP_LIST_1Print (Char8 * str, Uint32 arg) ;


/** ============================================================================
 *  @func   MP_LIST_Sleep
 *
 *  @desc   Sleeps for the specified number of microseconds.
 *          This is a OS specific function and is implemented in file:
 *              <GPPOS>\mp_list_os.c
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
MP_LIST_Sleep (Uint32 uSec) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (MP_LIST_H) */
