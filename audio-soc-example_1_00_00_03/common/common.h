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

/* DSP/BIOS Headers */
#include <msgq.h>

/* DSP/BIOS LINK Headers*/
#include <dsplink.h>


/* ID of the POOL used for application */
#define APP_POOL_ID    0

/* Number of samples in the buffer frame */
#define BUFLEN  4096             

/* 
 *  Application data buffer size used by the application 
 *  Need to ensure the buffer is alinged according to cache alignment
 *  expected (128-bytes) by POOL
 */
#define BUFSIZE (DSPLINK_ALIGN (BUFLEN , DSPLINK_BUF_ALIGN))

/* Application message size */
#define APPMSGSIZE (DSPLINK_ALIGN (sizeof (MyMsg), DSPLINK_BUF_ALIGN)) 

/*
 * Application-specific message IDs for message queue
 * All processor must used the same value for the message IDs
 */
enum {
    GPP_OUTPUTMSGID = 0,
    DSP_INPUTMSGID,
    DSP_PROCESSMSGID,
    DSP_OUTPUTMSGID,
    TERMINATEMSGID
};

/* The format of the messages being sent. */
typedef struct MyMsg {
    MSGQ_MsgHeader header;
    Void	*dataBuffer;
} MyMsg;

