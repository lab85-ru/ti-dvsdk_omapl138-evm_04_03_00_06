/* --COPYRIGHT--,BSD
 * Copyright (c) 2010, Texas Instruments Incorporated
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/*
 *  ======== processThread.c ========
 */

#include <std.h>

/* DSP/BIOS interface files */
#include <log.h>
#include <tsk.h>
#include <sem.h>
#include <sys.h>
#include <msgq.h>

/* Common header file shared between GPP and DSP */
#include "../common/common.h"

/* Statically defined LOG object */
extern LOG_Obj trace;

/* Process TSK message queue handle */
static MSGQ_Queue processMsgQueue;


Void processThreadInit (Void)
{
    Int        msgqStatus;
    MSGQ_Attrs msgqAttrs;
    SEM_Handle processSemHandle;

    processSemHandle = SEM_create(0, NULL);

    if (processSemHandle == NULL) {
        SYS_abort("Semaphore creation failed");
    }

    /*
     *  Open the process message queue.  
     *  Using semaphores as the notification mechanism.
     */
    msgqAttrs              = MSGQ_ATTRS;
    msgqAttrs.notifyHandle = (Ptr)processSemHandle;
    msgqAttrs.pend         = (MSGQ_Pend)SEM_pendBinary;
    msgqAttrs.post         = (MSGQ_Post)SEM_postBinary;
    msgqStatus = MSGQ_open("process", &processMsgQueue, &msgqAttrs);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to open the process message queue");
    }
}


/*
 *  ======== processThread ========
 *  Receives message, processes it and pass it through to output task. 
 */
Void processThread(Void)
{    
    MyMsg     *msg, *inputMsg, *outputMsg;
    Int        msgqStatus;
    MSGQ_Queue passMsgQueue;
    MSGQ_Queue replyMsgQueue;
    Int msgId;

    /*
     *  Locate the output's message queue handle.
     *  Passing NULL for the attributes, so it is a
     *  synchronous locate.
     */
    msgqStatus = MSGQ_locate("output", &passMsgQueue, NULL);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to locate the output message queue");
    }

    /*
     *  Locate the input TSK message queue handle.
     *  Passing NULL for the attributes, so it is a
     *  synchronous locate.
     */
    msgqStatus = MSGQ_locate("inputDSP", &replyMsgQueue, NULL);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to locate the output message queue");
    }

    /* Allocate a message */
    msgqStatus = MSGQ_alloc(0, (MSGQ_Msg *)&inputMsg, APPMSGSIZE);
    if (msgqStatus != SYS_OK) {        
        SYS_abort("Failed to allocate a message");
    }

    /* Set MSGQ Id for input task to handle and send message */
    MSGQ_setMsgId( (MSGQ_Msg)inputMsg, DSP_PROCESSMSGID);
    
    msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)inputMsg);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to send a message");
     }

    inputMsg = NULL;
    outputMsg = NULL;

    /* While there is no problem, keep processing messages. */
    for (;;) {

        /* Wait forever to get a message */
        msgqStatus = MSGQ_get(processMsgQueue, (MSGQ_Msg *)&msg, SYS_FOREVER);
        if (msgqStatus != SYS_OK) {
            SYS_abort("Failed to get a message");
        }

        /* Check message ID */
        msgId = MSGQ_getMsgId((MSGQ_Msg)msg);

        switch (msgId) {
            case DSP_INPUTMSGID:
                /* Set input message to pass back to input TSK */
                inputMsg = msg;
                break;
            case DSP_OUTPUTMSGID:
                /* Set output message to pass back to output TSK */
                outputMsg = msg;
                break;
            default:
                SYS_abort("Unknown message received");  //Should never happen
                break;
        }

        if ((inputMsg != NULL) && (outputMsg != NULL)) {
        
            /* 
             *  Do your algorithm processing here by replacing memcpy.
             *  Must ensure that the input message data buffer is
             *  passed into the output message data buffer here. 
             *  A simple memcpy is performed to simulated algorithm processing
             */            
            memcpy(outputMsg->dataBuffer ,inputMsg->dataBuffer, BUFSIZE );

            /* Set MSGQ Id for input task to handle and send message back */
            MSGQ_setMsgId( (MSGQ_Msg)inputMsg, DSP_PROCESSMSGID);
            
            msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)inputMsg);

            /* Send message to output Task with valid buffer data */
            msgqStatus = MSGQ_put(passMsgQueue, (MSGQ_Msg)outputMsg);
            LOG_printf(&trace, "Process and passed msg with buffer");

            inputMsg = NULL;
            outputMsg = NULL;
        }
    }
}

