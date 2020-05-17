/** ============================================================================
 *  @file   swiMessage.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message/
 *
 *  @desc   This is simple SWI based application that uses MSGQ interface to
 *          implement control from GPP. It receives a messages from the GPP,
 *          verifies its content and sends it back to the GPP.
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


#ifndef SWIMESSAGE_
#define SWIMESSAGE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>
#include <swi.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @name   SWIMESSAGE_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  sequenceNumber
 *              Expected id from the GPP.
 *  @field  localMsgq
 *              Handle to local message queue.
 *  @field  locatedMsgq
 *              Handle to located remote message queue.
 *  @field  swi
 *              SWI structure.
 *  ============================================================================
 */
typedef struct SWIMESSAGE_TransferInfo_tag {
    Uint16      sequenceNumber;
    MSGQ_Queue  localMsgq ;
    MSGQ_Queue  locatedMsgq ;
    SWI_Handle  swi ;
} SWIMESSAGE_TransferInfo;


/** ============================================================================
 *  @func   SWIMESSAGE_create
 *
 *  @desc   Create phase function of SWIMESSAGE application.
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
Int SWIMESSAGE_create (SWIMESSAGE_TransferInfo ** transferInfo) ;

/** ============================================================================
 *  @func   SWIMESSAGE_delete
 *
 *  @desc   Delete phase function of SWIMESSAGE application.
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
Int SWIMESSAGE_delete (SWIMESSAGE_TransferInfo * transferInfo) ;


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* SWIMESSAGE_ */
