/** ============================================================================
 *  @file   tskScale.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/scale/
 *
 *  @desc   Header file for tskScale application.
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


#if !defined (TSKSCALE_)
#define TSKSCALE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>
#include <sem.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @const  MAX_BUFFERS
 *
 *  @desc   Size of buffers array.
 *  ============================================================================
 */
#define MAX_BUFFERS            100


/** ============================================================================
 *  @name   TSKSCALE_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  inputStream
 *              Handle to input stream.
 *  @field  outputStream
 *              Handle to output stream.
 *  @field  msgqQueue
 *              Handle of created message queue.
 *  @field  notifySemObj
 *              Semaphore used for message notification.
 *  @field  numTransfers
 *              Number of transfers to be done.
 *  @field  bufferSize
 *              Buffer size information.
 *  @field  receivedSize
 *              Actual length of the buffer that was received.
 *  @field  numBuffers
 *              Number of valid buffers in "buffer" field.
 *  @field  buffer
 *              Buffer pointer.
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  ============================================================================
 */
typedef struct TSKSCALE_TransferInfo_tag {
    SIO_Handle  inputStream ;
    SIO_Handle  outputStream ;
    MSGQ_Queue  msgqQueue ;
    SEM_Obj     notifySemObj ;
    Uint16      numTransfers ;
    Uint32      bufferSize ;
    Uint16      receivedSize ;
    Uint16      numBuffers ;
    Char *      buffer ;
    Uint16      scalingFactor ;
} TSKSCALE_TransferInfo ;


/** ============================================================================
 *  @func   TSKSCALE_create
 *
 *  @desc   Create phase function of TSKSCALE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKSCALE_create (TSKSCALE_TransferInfo ** transferInfo);

/** ============================================================================
 *  @func   TSKSCALE_execute
 *
 *  @desc   Excecute phase function of TSKSCALE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKSCALE_execute (TSKSCALE_TransferInfo * transferInfo);

/** ============================================================================
 *  @func   TSKSCALE_delete
 *
 *  @desc   Delete phase function of TSKSCALE application.
 *
 *  @arg    transferInfo
 *              Information for transfer.
 *
 *  @ret    SYS_OK
 *              Successful operation.
 *          SYS_EBADIO
 *              Failure occured while doing IO.
 *
 *  @enter  None
 *
 *  @leave  None
 *
 *  @see    None
 *  ============================================================================
 */
Int TSKSCALE_delete (TSKSCALE_TransferInfo * transferInfo);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (TSKSCALE_) */
