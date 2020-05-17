/** ============================================================================
 *  @file   swiScale.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/scale/
 *
 *  @desc   Header file for swiScale application.
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


#if !defined (SWISCALE_)
#define SWISCALE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @name   SWISCALE_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  gioInputChan
 *              Handle input GIO channel.
 *  @field  gioOutputChan
 *              Handle output GIO channel.
 *  @field  appReadCb
 *              Callback structure for read requests.
 *  @field  appWriteCb
 *              Callback structure for write requests.
 *  @field  inputBuffer
 *              Buffer to receive data from GPP.
 *  @field  outputBuffer
 *              Buffer to send data to GPP.
 *  @field  readMAUs
 *              Number of MAUs read from GPP.
 *  @field  msgqQueue
 *              Handle to created message queue.
 *  @field  swiData
 *              SWI structure for SWI that handles data.
 *  @field  swiMsg
 *              SWI structure for SWI that handles messages.
 *  @field  bufferSize
 *              Size of buffer.
 *  @field  scalingFactor
 *              Used to scale the output buffer values.
 *  ============================================================================
 */
typedef struct SWISCALE_TransferInfo_tag {
    GIO_Handle      gioInputChan ;
    GIO_Handle      gioOutputChan ;
    GIO_AppCallback appReadCb ;
    GIO_AppCallback appWriteCb ;
    Char *          inputBuffer ;
    Char *          outputBuffer ;
    Uint16          readMAUs ;
    MSGQ_Queue      msgqQueue ;
    SWI_Handle      swiData ;
    SWI_Handle      swiMsg ;
    Uint16          bufferSize ;
    Uint16          scalingFactor ;
} SWISCALE_TransferInfo ;


/** ============================================================================
 *  @func   SWISCALE_create
 *
 *  @desc   Create phase function of SWISCALE application.
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
Int SWISCALE_create (SWISCALE_TransferInfo ** transferInfo);

/** ============================================================================
 *  @func   SWISCALE_execute
 *
 *  @desc   Excecute phase function of SWISCALE application.
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
Int SWISCALE_execute (SWISCALE_TransferInfo * transferInfo);

/** ============================================================================
 *  @func   SWISCALE_delete
 *
 *  @desc   Delete phase function of SWISCALE application.
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
Int SWISCALE_delete (SWISCALE_TransferInfo * transferInfo);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (SWISCALE_) */
