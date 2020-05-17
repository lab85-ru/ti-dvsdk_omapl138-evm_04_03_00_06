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
 *  ======== inputThread.c ========
 */

#include <std.h>

/* DSP/BIOS interface files */
#include <log.h>
#include <tsk.h>
#include <sem.h>
#include <mem.h>
#include <sio.h>
#include <sys.h>
#include <msgq.h>
#include <bcache.h>

/* Common header file shared between GPP and DSP */
#include "../common/common.h"

extern Void deleteOutputStream (Void);

/* Statically defined LOG object */
extern LOG_Obj trace;

/* Input TSK message queue handle */
static MSGQ_Queue inputMsgQueue;

/*
 *  ======== inputThreadInit ========
 *  Creates the message queue for the input TSK. 
 */
Void inputThreadInit (Void)
{
    Int        msgqStatus;
    MSGQ_Attrs msgqAttrs;
    SEM_Handle inputSemHandle;

    inputSemHandle = SEM_create(0, NULL);

    if (inputSemHandle == NULL) {
        SYS_abort("Semaphore creation failed");
    }

    /*
     *  Open the input message queue.  
     *  Using semaphores as the notification mechanism.
     */
    msgqAttrs              = MSGQ_ATTRS;
    msgqAttrs.notifyHandle = (Ptr)inputSemHandle;
    msgqAttrs.pend         = (MSGQ_Pend)SEM_pendBinary;
    msgqAttrs.post         = (MSGQ_Post)SEM_postBinary;

    msgqStatus = MSGQ_open("inputDSP", &inputMsgQueue, &msgqAttrs);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to open the input message queue");
    }
}

/*
 *  ======== inputThread ========
 *  Sends a message, waits for the message back from output and then repeat. 
 */
Void inputThread(Void)
{
    MyMsg             *msg, *inputMsg, *outputMsg;
    Int               msgqStatus;
    MSGQ_Queue        dstMsgQueue;
    MSGQ_Queue        replyMsgQueue;
    MSGQ_LocateAttrs  syncLocateAttrs ;
    Int msgId;

    /*
     *  Locate the process's message queue handle.
     *  Passing NULL for the attributes, so it is a
     *  synchronous locate.
     */
    msgqStatus = MSGQ_locate("process", &dstMsgQueue, NULL);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to locate the process message queue");
     }
    
    /* 
     *  Locate GPP message queue handle.
     *  Spin until GPP message is located
     *  while allowing other TSKs to execute. 
     */
    msgqStatus = SYS_ENOTFOUND;
    while ((msgqStatus == SYS_ENOTFOUND) || (msgqStatus == SYS_ENODEV)) {
        syncLocateAttrs.timeout = SYS_FOREVER;
        msgqStatus = MSGQ_locate("outputGPP", &replyMsgQueue, &syncLocateAttrs);
        if ((msgqStatus == SYS_ENOTFOUND) || (msgqStatus == SYS_ENODEV)) {
            TSK_sleep(1000);
        }
        else if (msgqStatus != SYS_OK) {
            SYS_abort("Failed to locate the outputGPP message queue");
        }
    }

    /* Allocate a message */
    msgqStatus = MSGQ_alloc(0, (MSGQ_Msg *)&inputMsg, APPMSGSIZE);
    if (msgqStatus != SYS_OK) {        
        SYS_abort("Failed to allocate a message");
    }

    /* Put a message on the GPP message queue to get system going */
    msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)inputMsg);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to send a message");
    }

    inputMsg = NULL;
    outputMsg = NULL;

    for (;;) {

	    /* Wait forever for the message on the input message queue */
        msgqStatus = MSGQ_get(inputMsgQueue, (MSGQ_Msg *)&msg, SYS_FOREVER);
        if (msgqStatus != SYS_OK) {
            SYS_abort("Failed to get a message from GPP");
        }        
        
        /* Check message ID */
        msgId = MSGQ_getMsgId((MSGQ_Msg)msg);
        
        switch (msgId) {
            case GPP_OUTPUTMSGID:
                /* 
                 *  Invalidate the data buffer received from GPP 
                 *  to ensure cache coherency
                 */
                BCACHE_inv (msg->dataBuffer, BUFSIZE, TRUE); 
                
                /* 
                 *  Place received message on the output message 
                 *  to be sent to the process TSK 
                 */
                outputMsg = msg;
                break;
            case DSP_PROCESSMSGID:
                /* 
                 *  Place received message on the input message
                 *  to be sent to the input TSK 
                 */
                inputMsg = msg;
                break;
            case TERMINATEMSGID:
                /* Delete DSP audio stream */
                deleteOutputStream();

                /* Send message back on the GPP to terminate app */
                inputMsg = msg;
                msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)inputMsg);
                if (msgqStatus != SYS_OK) {
                    SYS_abort("Failed to send a GPP message back");
                }
            default:
                SYS_abort("Unknown message received");  //Should never happen
                break;
        } 
        
        if ((inputMsg != NULL) && (outputMsg != NULL)) {
            
            /* Send message back on the GPP message queue */
            msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)inputMsg);
            if (msgqStatus != SYS_OK) {
                SYS_abort("Failed to send a GPP message back");
            }

            /* Set MSGQ Id for process TSK to handle and send message */
            MSGQ_setMsgId( (MSGQ_Msg)outputMsg, DSP_INPUTMSGID);

            msgqStatus = MSGQ_put(dstMsgQueue, (MSGQ_Msg)outputMsg);
            if (msgqStatus != SYS_OK) {
                SYS_abort("Failed to send a message to process function");
            }            
            LOG_printf(&trace, "Sending input msg to Processing TSK");      

            inputMsg = NULL;
            outputMsg = NULL;
        }        
    }
}
