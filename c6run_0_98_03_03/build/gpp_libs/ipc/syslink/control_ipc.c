/*
 * control_ipc.c
 */

/*
 * Copyright (C) 2010-2011 Texas Instruments Incorporated - http://www.ti.com/ 
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
#include <time.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <semaphore.h>

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
#include "control_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in control messages.
#define CONTROL_BUFSZ        (CONTROL_MSG_TOTALSIZE - sizeof(MessageQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

// Message structure for CONTROL messages
struct CONTROL_Msg
{
  MessageQ_MsgHeader header;
  volatile unsigned char buffer[CONTROL_BUFSZ];
};


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static sem_t CONTROL_IPC_msgSemObj;

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

// Global CONTROL MessageQ Objects for Tx and Rx
MessageQ_Handle   MessageQ_controlToGPP;
MessageQ_QueueId  MessageQ_controlFromGPPId;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t CONTROL_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered CONTROL_IPC_init()\n");

  // Create the local receive message queue (CONTROL TO GPP queue)
  MessageQ_controlToGPP = MessageQ_create (CONTROL_TO_GPP_MSGQNAME, NULL);
  if (MessageQ_controlToGPP == NULL)
  {
    status = MessageQ_E_FAIL;
    printf ("MessageQ_create() failed: status = [0x%8X] \n", status);
    return C6RUN_FAIL;
  }

  // Connect to the remote sending message queue
  do
  {
    status = MessageQ_open (CONTROL_FROM_GPP_MSGQNAME, &MessageQ_controlFromGPPId);
  }
  while (status == MessageQ_E_NOTFOUND);
  
  if (status < 0)
  {
    printf ("MessageQ_open() failed: status = [0x%8X]\n", status);
    return C6RUN_FAIL;
  }
  
  if( sem_init(&CONTROL_IPC_msgSemObj,0,CONTROL_MSG_CNT) < 0 )
  {
    printf("sem_init() failed");
    exit(0);
  }
  
  LOCAL_isModuleInitialized = true;
  
  VERBOSE_PRINT0("Leaving CONTROL_IPC_init()\n");
  
  return C6RUN_SUCCESS;
}

void CONTROL_IPC_exit()
{
  int32_t status    = 0;
  int32_t tmpStatus = 0;
  
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  VERBOSE_PRINT0("Entered CONTROL_IPC_exit()\n");
  
  if( sem_destroy(&CONTROL_IPC_msgSemObj) < 0 )
  {
    printf("sem_destroy() failed");
    exit(0);
  }

  // Close the GPP's CONTROL message queue
  status = MessageQ_close(&MessageQ_controlFromGPPId);
  if (status < 0)
  {
    printf ("MessageQ_close() failed: status = [0x%8X]\n", status);
  }
  else
  {
    VERBOSE_PRINT0("MessageQ_close() passed.\n");
  }
  
  tmpStatus = MessageQ_delete(&MessageQ_controlToGPP);
  if ((status >= 0) && (tmpStatus < 0))
  {
    status = tmpStatus;
    printf ("MessageQ_delete() failed: status = [0x%8X]\n", status);
  }
  else
  {
    VERBOSE_PRINT0("MessageQ_delete() passed.\n");
  }

  LOCAL_isModuleInitialized = false;

  VERBOSE_PRINT0("Leaving CONTROL_IPC_exit()\n");
}

void CONTROL_IPC_sendMsg( CONTROL_Msg *msg, uint16_t msgId )
{
  int32_t status;
  
  VERBOSE_PRINT0("Entering CONTROL_IPC_sendMsg()\n");

  MessageQ_setMsgId(msg, msgId);

  status = MessageQ_put(MessageQ_controlFromGPPId, (MessageQ_Msg) msg);
  
  if (status < 0)
  { 
    printf ("MessageQ_put() failed: status = [0x%8X]\n", status); 
  }
  
  VERBOSE_PRINT0("Exiting CONTROL_IPC_sendMsg()\n");
}

uint16_t CONTROL_IPC_recvMsg( CONTROL_Msg **msg )
{
  int32_t status = 0;
  uint16_t msgId = 0;
  
  VERBOSE_PRINT0("Entering CONTROL_IPC_recvMsg()\n");

  status = MessageQ_get(MessageQ_controlToGPP, (MessageQ_Msg *) msg, MessageQ_FOREVER);

  if (status < 0)
  {
    printf ("MessageQ_get() failed: status = [0x%8X]\n", status); 
    return CONTROL_NULL;
  }

  msgId = MessageQ_getMsgId((*msg));

  VERBOSE_PRINT0("Exiting CONTROL_IPC_recvMsg()\n");
  
  return msgId;
}

void CONTROL_IPC_allocMsg( CONTROL_Msg **msg )
{
  VERBOSE_PRINT0("Entering CONTROL_IPC_allocMsg()\n");

  // Semaphore wait on an available message
  sem_wait(&CONTROL_IPC_msgSemObj);

  // Do MessageQ alloc for a remote procedure call
  *msg = (CONTROL_Msg *) MessageQ_alloc(COMMON_HEAP_ID, sizeof(CONTROL_Msg));
  if ((*msg) == NULL)
  {
    printf ("MessageQ_alloc() failed.");
  }
  
  VERBOSE_PRINT0("Exiting CONTROL_IPC_allocMsg()\n");
}

void CONTROL_IPC_freeMsg( CONTROL_Msg *msg  )
{
  VERBOSE_PRINT0("Entering CONTROL_IPC_freeMsg()\n");

  MessageQ_free((MessageQ_Msg) msg);

  sem_post(&CONTROL_IPC_msgSemObj);
  
  VERBOSE_PRINT0("Exiting CONTROL_IPC_freeMsg()\n");
}

void *CONTROL_IPC_getMsgBuffer( CONTROL_Msg *msg )
{ 
  return (void *) (msg->buffer);
}

uint32_t CONTROL_IPC_getMsgBufferSize ( void )
{
  return CONTROL_BUFSZ;
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

