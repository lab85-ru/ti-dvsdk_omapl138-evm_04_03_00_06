/** ============================================================================
 *  @file   ring_io_os.h
 *
 *  @path   $(DSPLINK)/gpp/src/samples/ring_io/Linux/
 *
 *  @desc   OS specific definitions for the ring_io sample.
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


#if !defined (RING_IO_OS_H)
#define RING_IO_OS_H

/*  ----------------------------------- OS Specific Headers           */
#include <pthread.h>
#include <sys/types.h>
/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

#ifdef RING_IO_MULTIPROCESS
#define     API_RESTART     DSP_ERESTART /* To restart an  function call if it is
                                         * interrupted by a signal
                                         */
#endif


/** ============================================================================
 *  @name   RING_IO_ClientInfo
 *
 *  @desc   Structure to hold thread/process information.
 *
 *  @field  tid
 *              Thread Identification number.
 *  @field  processorId
 *              ID of the dsp processor.
 *
 *  ============================================================================
 */
typedef struct RINIG_IO_ClientInfo_tag {

#ifdef RING_IO_MULTIPROCESS
    pid_t      pid ;
    char       processName[32];
#else
    pthread_t  tid ;
#endif
    Uint8      processorId ;

} RING_IO_ClientInfo ;

/** ============================================================================
 *  @func   atoi
 *
 *  @desc   Extern declaration for function that converts a string into an
 *          integer value.
 *
 *  @arg    str
 *              String representation of the number.
 *
 *  @ret    <valid integer>
 *              If the 'initial' part of the string represents a valid integer
 *          0
 *              If the string passed does not represent an integer or is NULL.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
extern int atoi (const char * str) ;

/** ============================================================================
 *  @func   RING_IO_Atoi
 *
 *  @desc   This function converts a string into an integer value.
 *
 *  @arg    str
 *              String representation of the number.
 *
 *  @ret    <valid integer>
 *              If the 'initial' part of the string represents a valid integer
 *          0
 *              If the string passed does not represent an integer or is NULL.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
#define RING_IO_Atoi atoi

/** ============================================================================
 *  @func   RING_IO_IntToString
 *
 *  @desc   This function converts an integer value into a string.
 *
 *  @arg    num
 *              The number that neesd to be converted to string.
 *  @arg    str
 *              String representation of the number.
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
RING_IO_IntToString (IN Int num, OUT Char8 * str) ;

/** ============================================================================
 *  @func   RING_IO_CreateSem
 *
 *  @desc   This function creates a semaphore.
 *
 *  @arg    semPtr
 *              Location to receive the semaphore object.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_CreateSem (OUT Pvoid * semPtr) ;

/** ============================================================================
 *  @func   RING_IO_DeleteSem
 *
 *  @desc   This function deletes a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to be deleted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_DeleteSem (IN Pvoid semHandle) ;

/** ============================================================================
 *  @func   RING_IO_WaitSem
 *
 *  @desc   This function waits on a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to wait on.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_WaitSem (IN Pvoid semHandle) ;

/** ============================================================================
 *  @func   RING_IO_PostSem
 *
 *  @desc   This function posts a semaphore.
 *
 *  @arg    semHandle
 *              Pointer to the semaphore object to be posted.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_PostSem (IN Pvoid semHandle) ;

/** ============================================================================
 *  @func   RING_IO_Create_client
 *
 *  @desc   Function to create a new thread or a Process..
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
Uint32
RING_IO_Create_client (RING_IO_ClientInfo * pInfo, Pvoid funcPtr, Pvoid args);

/** ============================================================================
 *  @func   RING_IO_Join_client
 *
 *  @desc   Wait for thread/process to terminate.
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RING_IO_Join_client (RING_IO_ClientInfo *pInfo);

/** ============================================================================
 *  @func   RING_IO_Exit_client
 *
 *  @desc   Function to exit from a caller task(thread/process).
 *
 *  @modif  None
 *  ============================================================================
 */
EXPORT_API
DSP_STATUS
RING_IO_Exit_client (RING_IO_ClientInfo *pInfo);

/** ============================================================================
 *  @func   RING_IO_OS_init
 *
 *  @desc   This function creates a memory pool.
 *
 *  @arg    none
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_OS_init (Void) ;

/** ============================================================================
 *  @func   RING_IO_OS_enit
 *
 *  @desc   This function deletes a memory pool.
 *
 *  @arg    none
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
RING_IO_OS_exit (Void) ;


/** ============================================================================
 *  @func   RING_IO_Atoll
 *
 *  @desc   Converts ascii to long int
 *
 *  @ret    <valid integer>
 *              If the 'initial' part of the string represents a valid integer
 *          0
 *              If the string passed does not represent an integer or is NULL.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
NORMAL_API
Uint32
RING_IO_Atoll (Char8 * str) ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (RING_IO_OS_H) */
