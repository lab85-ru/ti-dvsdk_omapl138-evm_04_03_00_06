/** ============================================================================
 *  @file   tskRingIo.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/ring_io/
 *
 *  @desc   Header file for tskRingIo application.
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

#if !defined (TSKRING_IO_)
#define TSKRING_IO_

/*  --------------------------- DSP/BIOS Headers ----------------------------- */
#include <sem.h>

/*  --------------------------- DSP/BIOS LINK Headers ----------------------- */
#include <mpcs.h>

/*  --------------------------- RingIO Headers ----------------------------- */
#include <ringiodefs.h>
#include <ringio.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */

/** ============================================================================
 *  @name   TSKRING_IO_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  writerHandle
 *              Handle to the RingIO used by the application in writer mode.
 *  @field  readerHandle
 *              Handle to the RingIO used by the application in reader mode.
 *  @field  writerSemObj
 *              Semaphore used for RingIO notification for the writer.
 *  @field  readerSemObj
 *              Semaphore used for RingIO notification for the reader.
 *  @field  readerRecvSize
 *              Actual length of the reader buffer that was received.
 *  @field  writerRecvSize
 *              Actual length of the writer buffer that was received.

 *  @field  readerBuf
 *              pointer the  acquired  reader buffer
 *
 *  @field  writerBuf
 *              Pointer to the acquired writer buffer.
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  @field  scaleOpCode
 *              contains OP_MULTIPLY and OP_DIVIDE based on the received
 *              variable attribute.
 *  @field  scaleSize
 *              contains the size of the buffer  on which  processing needs
 *              to be done.
 *  @field  freadStart
 *              boolean flag. if TRUE,  DSP application can start
 *              reading data  from RINGIO1.
 *  @field  freadEnd
 *              boolean flag. if TRUE,  DSP application stops
 *              reading data  from RINGIO1.
 *              .
 *  ============================================================================
 */
typedef struct TSKRING_IO_TransferInfo_tag {
    RingIO_Handle  writerHandle ;
    RingIO_Handle  readerHandle ;
    SEM_Obj        writerSemObj ;
    SEM_Obj        readerSemObj ;
    Uint32         readerRecvSize ;
    Uint32         writerRecvSize ;
    Char *         readerBuf  ;
    Char *         writerBuf  ;
    Uint32         scalingFactor ;
    Uint32         scaleOpCode;
    Uint32         scaleSize;
    Int8           freadStart;
    Int8           freadEnd;
} TSKRING_IO_TransferInfo ;

/** ============================================================================
 *  @func   TSKRING_IO_create
 *
 *  @desc   Create phase function of RING_IO application.
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
Int TSKRING_IO_create (TSKRING_IO_TransferInfo ** transferInfo) ;

/** ============================================================================
 *  @func   TSKRING_IO_execute
 *
 *  @desc   Excecute phase function of RING_IO application.
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
Int TSKRING_IO_execute (TSKRING_IO_TransferInfo * transferInfo) ;

/** ============================================================================
 *  @func   TSKRING_IO_delete
 *
 *  @desc   Delete phase function of RING_IO application.
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
Int TSKRING_IO_delete (TSKRING_IO_TransferInfo * transferInfo) ;

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* !defined (TSKRING_IO_) */
