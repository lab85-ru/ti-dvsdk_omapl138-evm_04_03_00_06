/** ============================================================================
 *  @file   tskMessage.h
 *
 *  @path   $(DSPLINK)/dsp/src/samples/message_multi/
 *
 *  @desc   This is a multi-application message sample. It interacts with
 *          multiple application instances on the GPP-side.
 *          It creates MAX_APPS tasks. The tasks wait on notification from the
 *          GPP about the application instance that is created on the GPP-side.
 *          When this notification is received, it opens and locates the
 *          corresponding message queues, performing message transfers with the
 *          GPP.
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


#ifndef TSKMESSAGE_
#define TSKMESSAGE_


/*  ----------------------------------- DSP/BIOS Headers            */
#include <msgq.h>
#include <sem.h>


#ifdef __cplusplus
extern "C" {
#endif


/** ============================================================================
 *  @name   TSKMESSAGE_TransferInfo
 *
 *  @desc   Structure used to keep various information needed by various phases
 *          of the application.
 *
 *  @field  appNum
 *              Sequence number of the application instance using this structure
 *  @field  sequenceNumber
 *              Expected id from the GPP.
 *  @field  numTransfers
 *              Number of message to be transferred between GPP and DSP.
 *  @field  localMsgq
 *              Handle of opened message queue.
 *  @field  locatedMsgqHandle
 *              Handle to located message queue.
 *  @field  msgqSemObj
 *              Semaphore used for message notification.
 *  @field  notifySemObj
 *              Semaphore used for NOTIFY notification.
 *  @field  dspMsgqName
 *              Stores the DSP MSGQ name for this application instance.
 *  @field  gppMsgqName
 *              Stores the GPP MSGQ name for this application instance.
 *  ============================================================================
 */
typedef struct TSKMESSAGE_TransferInfo_tag {
    Uint32     appNum ;
    Uint16     sequenceNumber ;
    Uint16     numTransfers ;
    MSGQ_Queue localMsgq ;
    MSGQ_Queue locatedMsgq ;
    SEM_Obj    msgqSemObj ;
    SEM_Obj    notifySemObj ;
    Char       dspMsgqName [32] ;
    Char       gppMsgqName [32] ;
} TSKMESSAGE_TransferInfo;


/** ============================================================================
 *  @func   TSKMESSAGE_create
 *
 *  @desc   Create phase function of TSKMESSAGE application.
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
Int TSKMESSAGE_create (TSKMESSAGE_TransferInfo * transferInfo);

/** ============================================================================
 *  @func   TSKMESSAGE_execute
 *
 *  @desc   Excecute phase function of TSKMESSAGE application.
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
Int TSKMESSAGE_execute (TSKMESSAGE_TransferInfo * transferInfo);

/** ============================================================================
 *  @func   TSKMESSAGE_delete
 *
 *  @desc   Delete phase function of TSKMESSAGE application.
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
Int TSKMESSAGE_delete (TSKMESSAGE_TransferInfo * transferInfo);


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif /* TSKMESSAGE_ */
