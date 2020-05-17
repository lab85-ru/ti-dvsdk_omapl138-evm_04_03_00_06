/*
 * cio_ipc.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 */

/* 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

/*  ----------------------------------- SysLink                       */
#include <ti/syslink/Std.h>
#include <ti/syslink/SysLink.h>
#include <ti/syslink/IpcHost.h>

/*  ----------------------------------- SysLink IPC                   */
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/Notify.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "syslink/ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "cio_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern int32_t fnomap[CIO_NFILE+1];


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in C6Run messages.
#define CIO_BUFSZ             (CIO_MSG_TOTALSIZE - sizeof(MessageQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

struct CIO_Msg
{
  MessageQ_MsgHeader header;
  volatile uint8_t buffer[CIO_BUFSZ];
};


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

// Global CIO MessageQ Objects for Tx and Rx
MessageQ_Handle     MessageQ_cioToGPP;
MessageQ_QueueId    MessageQ_cioFromGPPId;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t CIO_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered CIO_IPC_init()\n");

  // Open the CIO TO GPP message queue

  // Create the local receive message queue (CIO TO GPP queue)
  MessageQ_cioToGPP = MessageQ_create (CIO_TO_GPP_MSGQNAME, NULL);
  if (MessageQ_cioToGPP == NULL)
  {
    status = MessageQ_E_FAIL;
    printf ("MessageQ_create() failed: status = [0x%8X] \n", status);
    return C6RUN_FAIL;
  }

  // Connect to the remote sending message queue
  do
  {
    status = MessageQ_open (CIO_FROM_GPP_MSGQNAME, &MessageQ_cioFromGPPId);
  }
  while (status == MessageQ_E_NOTFOUND);
  
  if (status < 0)
  {
    printf ("MessageQ_open() failed: status = [0x%8X]\n", status);
    return C6RUN_FAIL;
  }
  
  // Initialize file ID mapping array for stdin (0), stdout (1), and stderr (2).                                             
  fnomap[0] = 0;
  fnomap[1] = 1;
  fnomap[2] = 2;
  
  LOCAL_isModuleInitialized = true;
  
  VERBOSE_PRINT0("Leaving CIO_IPC_init()\n");
  
  return C6RUN_SUCCESS;
}

void CIO_IPC_exit( void )
{
  int32_t status      = 0;
  int32_t tmpStatus   = 0;
  
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  VERBOSE_PRINT0("Entered CIO_IPC_exit()\n");
  
  // Release the remote message queue
  status = MessageQ_close(&MessageQ_cioFromGPPId);
  if (status < 0)
  {
    printf ("MessageQ_close() failed: status = [0x%8X]\n", status);
  }
  else
  {
    VERBOSE_PRINT0("MessageQ_close() passed.\n");
  }
  
  tmpStatus = MessageQ_delete(&MessageQ_cioToGPP);
  if ((status >= 0) && (tmpStatus < 0))
  {
    printf ("MessageQ_delete() failed: status = [0x%8X]\n", status);
  }
  else
  {
    VERBOSE_PRINT0("MessageQ_delete() passed.\n");
  }  

  LOCAL_isModuleInitialized = false;

  VERBOSE_PRINT0("Leaving CIO_IPC_exit()\n");
}

void CIO_IPC_sendMsg( CIO_Msg *msg, uint16_t msgId )
{
  int32_t status = 0;
  
  VERBOSE_PRINT0("Entering CIO_IPC_sendMsg()\n");

  MessageQ_setMsgId(msg, msgId);

  status = MessageQ_put(MessageQ_cioFromGPPId, (MessageQ_Msg) msg);
  
  if (status < 0)
  { 
    printf ("MessageQ_put() failed: status = [0x%8X]\n", status); 
  }  
  
  VERBOSE_PRINT0("Exiting CIO_IPC_sendMsg()\n");
}

uint16_t CIO_IPC_recvMsg( CIO_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  VERBOSE_PRINT0("Entered CIO_IPC_recvMsg()\n");

  status = MessageQ_get(MessageQ_cioToGPP, (MessageQ_Msg *) msg, MessageQ_FOREVER);

  if (status < 0)
  {
    printf ("MessageQ_get() failed: status = [0x%8X]\n", status); 
    return CIO_NULL;
  }

  msgId = MessageQ_getMsgId(*msg);

  VERBOSE_PRINT0("Leaving CIO_IPC_recvMsg()\n");
  
  return msgId;
}

// Command to read message contents received from DSP
void CIO_IPC_readMsg ( CIO_Msg *msg, uint8_t *command, uint8_t *parmbuf, uint8_t *buffer)
{
  uint32_t length = 0xffff;
  volatile uint8_t *p;
  int32_t i;

  p = msg->buffer;

  length = UNLOADLONG(p, 0);
  p += INT_ADDR_INC;

  *command = *p++;

  for (i = 0; i < 8; i++)
  {
    parmbuf[i] = *p++;
  }

  for (i = 0; i < length; i++)
  {
    buffer[i] = *p++;
  }
}

// Command to fill-in message contents to send to DSP
void CIO_IPC_writeMsg( CIO_Msg *msg, uint8_t *parmbuf, uint8_t *buffer, uint32_t length)
{
  volatile uint8_t *p;
  int32_t i;

  p = msg->buffer;

  LOADLONG(p, length, 0);
  p += INT_ADDR_INC;

  for (i = 0; i < 8; i++)
  {
    *p++ = parmbuf[i];
  }

  if (!buffer) return;

  for (i = 0; i < length; i++)
  {
    *p++ = buffer[i];
  }
}

void CIO_IPC_allocMsg( CIO_Msg **msg )
{
  VERBOSE_PRINT0("Entering CIO_IPC_allocMsg()\n");

  *msg = (CIO_Msg *) MessageQ_alloc(COMMON_HEAP_ID, sizeof(CIO_Msg));
  if ((*msg) == NULL)
  {
    printf ("MessageQ_alloc() failed.");
  }
  
  VERBOSE_PRINT0("Exiting CIO_IPC_allocMsg()\n");
}

void CIO_IPC_freeMsg( CIO_Msg *msg )
{
  VERBOSE_PRINT0("Entering CIO_IPC_freeMsg()\n");

  MessageQ_free((MessageQ_Msg) msg);
  
  VERBOSE_PRINT0("Exiting CIO_IPC_freeMsg()\n");
}

void *CIO_IPC_getMsgBuffer( CIO_Msg *msg )
{ 
  return (void *) (msg->buffer);
}

uint32_t CIO_IPC_getMsgBufferSize ( void )
{
  return CIO_BUFSZ;
}



/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

