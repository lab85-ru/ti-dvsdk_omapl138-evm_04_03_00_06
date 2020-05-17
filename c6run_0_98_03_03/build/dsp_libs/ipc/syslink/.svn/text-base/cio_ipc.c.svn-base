/*
 * cio_ipc.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <stdbool.h>

/*  ----------------------------------- XDC Runtime Headers           */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>

/*  ----------------------------------- SysBios Headers               */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

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

static Semaphore_Handle CIO_IPC_msgSemaphore;
static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

MessageQ_Handle   MessageQ_cioFromGPP;
MessageQ_QueueId  MessageQ_cioToGPPId;

CIO_Msg *CIO_msg;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int CIO_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;
  
  CIO_IPC_msgSemaphore = Semaphore_create(CIO_MSG_CNT, NULL, NULL);

  // Create a message queue instance
  MessageQ_cioFromGPP = MessageQ_create(CIO_FROM_GPP_MSGQNAME, NULL);
  if (MessageQ_cioFromGPP == NULL)
  {
    Log_error0("MessageQ_create failed\n" );
    return C6RUN_FAIL;
  }

  // Open the remote message queue. Spin until it is ready.
  do
  {
    status = MessageQ_open(CIO_TO_GPP_MSGQNAME, &MessageQ_cioToGPPId);
    if (status == MessageQ_E_NOTFOUND)
    {
      // Sleep for a while before trying again.
      Task_sleep (10);
    }
  }
  while (status != MessageQ_S_SUCCESS);

  LOCAL_isModuleInitialized = TRUE;
  
  return status;
}

void CIO_IPC_exit( void )
{
  int32_t status;
  
  // If not already initialized, just exit
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  // Disconnect from the remote queue on the GPP
  status = MessageQ_close (&MessageQ_cioToGPPId);
  if (status < 0)
  {
    Log_error0("MessageQ_close failed\n");
  }
  
  // Delete the local receive queue
  status = MessageQ_delete (&MessageQ_cioFromGPP);
  if (status < 0)
  {
    Log_error0("MessageQ_delete failed\n");
  }

  Semaphore_delete(&CIO_IPC_msgSemaphore);

  LOCAL_isModuleInitialized = FALSE;
}

void CIO_IPC_sendMsg( CIO_Msg *msg, uint16_t msgId )
{
  int32_t status;

  MessageQ_setMsgId((MessageQ_Msg) msg, msgId);
  MessageQ_setReplyQueue(MessageQ_cioFromGPP,(MessageQ_Msg) msg);

  status = MessageQ_put(MessageQ_cioToGPPId, (MessageQ_Msg) msg);
  
  if (status < 0)
  { 
    Log_error1("MessageQ_put() failed: status = [0x%x]\n", status);
  }
}

uint16_t CIO_IPC_recvMsg( CIO_Msg **msg )
{
  int32_t status;
  uint16_t msgId;
  
  status = MessageQ_get(MessageQ_cioFromGPP, (MessageQ_Msg *) msg, MessageQ_FOREVER);

  if (status < 0)
  {
    Log_error1("MessageQ_get() failed: status = [0x%x]\n", status); 
    return CIO_NULL;
  }

  msgId = MessageQ_getMsgId((MessageQ_Msg) (*msg));

  return msgId;
}

void CIO_IPC_allocMsg( CIO_Msg **msg )
{
  Semaphore_pend(CIO_IPC_msgSemaphore, BIOS_WAIT_FOREVER);
  
  *msg = (CIO_Msg *) MessageQ_alloc(COMMON_HEAP_ID, sizeof(CIO_Msg));
  if (*msg == NULL)
  {
    Log_error0("MessageQ_alloc failed");
  }
}

void CIO_IPC_freeMsg( CIO_Msg *msg )
{
  MessageQ_free((MessageQ_Msg)msg);
  Semaphore_post(CIO_IPC_msgSemaphore);
}

void *CIO_IPC_getMsgBuffer( CIO_Msg *msg )
{
  return (void *) msg->buffer;
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
