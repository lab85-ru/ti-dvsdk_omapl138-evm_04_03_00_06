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
 *  ======== outputThread.c ========
 */

#include <std.h>

/* DSP/BIOS interface files */
#include <log.h>
#include <tsk.h>
#include <sem.h>
#include <mem.h>
#include <sys.h>
#include <sio.h>
#include <msgq.h>

/* DSP Platform support package (PSP) interface files */
#include <ti/pspiom/mcasp/Mcasp.h>

/* EDMA3 driver interface files */
#include <ti/sdo/edma3/drv/edma3_drv.h>

/* Common header file shared between GPP and DSP */
#include "../common/common.h"

/* Define Memory alignment for use of L2 cache  */
#define MEMALIGN 128

/* Number of buffers to be issued and reclaimed by the driver */
#define NUM_BUFS 4

/* Output TSK message queue handle */
static MSGQ_Queue outputMsgQueue;

/* Output stream handle */
static SIO_Handle outputStream;

/* Buffers array for the SIO buffers */
static Ptr bufTx[NUM_BUFS];

/* Channel parameters to initialize channel object */
extern Mcasp_ChanParams  mcasp_chanparam;

/* EDMA3 Driver Handle to create device */
extern EDMA3_DRV_Handle hEdma[];

/* Statically defined LOG object */
extern LOG_Obj trace;

/*
 *  ======== outputThreadInit ========
 *  Creates the message queue for the output TSK. 
 */
Void outputThreadInit (Void)
{
    Int        msgqStatus;
    MSGQ_Attrs msgqAttrs;
    SEM_Handle outputSemHandle;

    outputSemHandle = SEM_create(0, NULL);
    
    if (outputSemHandle == NULL) {
        SYS_abort("Semaphore creation failed");
    }

    /*
     *  Open the output message queue.  
     *  Using semaphores as the notification mechanism.
     */
    msgqAttrs              = MSGQ_ATTRS;
    msgqAttrs.notifyHandle = (Ptr)outputSemHandle;
    msgqAttrs.pend         = (MSGQ_Pend)SEM_pendBinary;
    msgqAttrs.post         = (MSGQ_Post)SEM_postBinary;
    msgqStatus = MSGQ_open("output", &outputMsgQueue, &msgqAttrs);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to open the output message queue");
    }
}


/*
 *  ======== outputThread ========
 *  Waits for the message, issues buffer to driver, and
 *  sends message back to process TSK and repeat. 
 */
Void outputThread(Void)
{

    MyMsg     *msg;
    Int        msgqStatus;
    MSGQ_Queue replyMsgQueue;
    Ptr xmt;
    Int32 count = 0;
    Int nmadus = 0;

    /* Allocate buffers for the SIO buffer exchanges */
    for(count = 0; count < NUM_BUFS ; count ++) {
        bufTx[count]= (Ptr)MEM_calloc(0, BUFSIZE, MEMALIGN);

        if(NULL == bufTx[count]) {
            LOG_printf(&trace,"\r\nMEM_calloc failed.\n");
        }
    }

    for(count = 0; count < NUM_BUFS; count ++) {
        /* Issue the empty buffers to the output stream */
        SIO_issue(outputStream, bufTx[count], BUFSIZE, NULL);
    }
    
    /*
     *  Locate the process's message queue handle.
     *  Passing NULL for the attributes, so it is a
     *  synchronous locate.
     */
    msgqStatus = MSGQ_locate("process", &replyMsgQueue, NULL);

    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to locate the reply message queue");
    }

    /* Allocate a message */
    msgqStatus = MSGQ_alloc(0, (MSGQ_Msg *)&msg, APPMSGSIZE);
    if (msgqStatus != SYS_OK) {        
        SYS_abort("Failed to allocate a message");
    }

    /* Reclaim the first buffer to pass buffer pointer in the message queue */
    nmadus = SIO_reclaim(outputStream, (Ptr *)&xmt, NULL);
    if (nmadus < 0) {
        LOG_printf(&trace,"\r\nError reclaiming empty buffer from the output stream\n");
    }
    
    /* Initialize message buffer with drivers buffers */
    msg->dataBuffer = xmt;

    /* Set MSGQ Id for processing task to handle and send message */
    MSGQ_setMsgId( (MSGQ_Msg)msg, DSP_OUTPUTMSGID);

    msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)msg);
    if (msgqStatus != SYS_OK) {
        SYS_abort("Failed to send a message");
    }
    
    for (;;) {

        /* Reclaim empty buffer from the output stream to be reused */
        nmadus = SIO_reclaim(outputStream, (Ptr *)&xmt, NULL);
        if (nmadus < 0) {
            LOG_printf(&trace,"\r\nError reclaiming empty buffer from the output stream\n");
        }

        /* Wait forever to get a message on the output message queue */
        msgqStatus = MSGQ_get(outputMsgQueue, (MSGQ_Msg *)&msg, SYS_FOREVER);
        if (msgqStatus != SYS_OK) {
            SYS_abort("Failed to get a message");
        }

        LOG_printf(&trace, "Received message from process TSK");

        /* Issue full buffer to the output stream with buffer from message */
        if (SIO_issue(outputStream, msg->dataBuffer, BUFSIZE, NULL)
	    != SYS_OK) {
            LOG_printf(&trace,"\r\nFailed to issue empty buffer to stream\n");
        }

        /* Set message buffer to the drivers buffers previously reclaimed */
        msg->dataBuffer = xmt;

        /* Set MSGQ Id for processing task to handle and send message */
        MSGQ_setMsgId( (MSGQ_Msg)msg, DSP_OUTPUTMSGID);
        
        msgqStatus = MSGQ_put(replyMsgQueue, (MSGQ_Msg)msg);
        if (msgqStatus != SYS_OK) {
            SYS_abort("Failed to send a message");
        }
    }
}

/*
 *  ======== createOutputStream ========
 *  Creates the driver output stream to be used by the applications.
 *  This function must be call in the context of a TSK function 
 */
Void createOutputStream (Void)
{
    SIO_Attrs   attrs;

    attrs       = SIO_ATTRS;
    attrs.nbufs = NUM_BUFS;
    attrs.align = MEMALIGN;
    attrs.model = SIO_ISSUERECLAIM;

    mcasp_chanparam.edmaHandle = hEdma[0];
    
    /* create the codec output stream */
    outputStream = SIO_create( "/dioAudioOUT", SIO_OUTPUT, BUFSIZE , &attrs );

    if (outputStream == NULL) {
        SYS_abort("Failed to create output Stream");
    }
}

/*
 *  ======== delteOutputStream ========
 *  Deletes the driver output stream to be used by the applications.
 *  This function must be call in the context of a TSK function 
 */
Void deleteOutputStream (Viod)
{
    Bool status;
    Int sioStatus;
    Int32 count = 0;

    if (outputStream) {
	    /* Delete SIO stream */
        sioStatus = SIO_delete (outputStream);
        if (sioStatus != SYS_OK) {
           SYS_abort("SIO stream delete failed.\n");
        }	
        /* Free buffers used by SIO */
        for(count = 0; count < NUM_BUFS ; count ++) {
            status = MEM_free(0, bufTx[count], BUFSIZE);

            if(status == NULL) {
                SYS_abort("\r\nMEM_free failed.\n");
            }
        }
    }
}

