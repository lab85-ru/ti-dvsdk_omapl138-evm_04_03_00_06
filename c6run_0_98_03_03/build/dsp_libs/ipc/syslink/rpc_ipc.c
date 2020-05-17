/*
 * rpc_ipc.c
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
#include "rpc_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern void     C6RUN_RPC_task( RPC_Msg *msg );


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in RPC messages.
#define RPC_BUFSZ        (RPC_MSG_TOTALSIZE - sizeof(MessageQ_MsgHeader) - sizeof(intptr_t))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

// Message structure for RPC messages
struct RPC_Msg
{
  MessageQ_MsgHeader      header;
  intptr_t                pSyncStruct;  
  volatile unsigned char  buffer[RPC_BUFSZ];
};

typedef void (*pRPC_IPC_stubFxn)(void *msgBuffer);

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

Task_Handle       RPC_IPC_recvThread;

MessageQ_Handle   MessageQ_rpcFromGPP;
MessageQ_QueueId  MessageQ_rpcToGPPId;

Task_Handle       RPC_IPC_completedThreads[RPC_MSG_CNT];
uint32_t          completedThreadsCnt;


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Task function to execute the remote procedure call
void RPC_IPC_executer( RPC_Msg *msg )
{
  /*-------------------------------------------------------------
  * Get a pointer to the RPC packet. The packet has 2 components
  *   1) Stub function address
  *   2) Stub data packet (collection of packed parameters, cache 
  *      commands, etc.)
  *------------------------------------------------------------*/
  uint32_t *p = (uint32_t *) RPC_IPC_getMsgBuffer(msg);
  pRPC_IPC_stubFxn stubFxn;
  
  // Get Stub Function Address from message buffer
  stubFxn = (pRPC_IPC_stubFxn) *p;
  
  // Call Stub function, passing in message buffer
  (*stubFxn)((void *)p);
  
  // Send back the message to the sender (we assume any result data
  // has been placed in the message by the stub function)
  RPC_IPC_sendMsg(msg, RPC_GPP2DSP_FXNCALL_RET);
  
  // Add this completed task to list of tasks to be destroyed
  // We can't delete ourself, so let someone else do it
  {
    unsigned int restoreVal = _disable_interrupts();
    RPC_IPC_completedThreads[completedThreadsCnt++] = Task_self();
    _restore_interrupts(restoreVal);
  }
}

void RPC_IPC_receiver( void )
{
  RPC_Msg *msg;
  uint16_t msgId;

  // Continuous monitor for remote procedure calls from GPP
  while (1)
  {
    // Delete tasks (if any) that have run to completion
    {
      unsigned int restoreVal = _disable_interrupts();
      while (completedThreadsCnt)
      {
        Task_delete(&RPC_IPC_completedThreads[--completedThreadsCnt]);
      }
      _restore_interrupts(restoreVal);
    }  
  
    // Blocking call to get next incoming RPC
    msgId = RPC_IPC_recvMsg(&msg);
    
    if (RPC_GPP2DSP_FXNCALL == msgId)
    {
      // Start a new task to execute the function (pass the message buffer as argument)
      Task_Params taskParams; 
 
      Task_Params_init(&taskParams);
      taskParams.arg0 = (UArg) msg;
   
      Task_create((Task_FuncPtr)&RPC_IPC_executer, &taskParams, NULL);
      
    }
    else if (RPC_TERMINATE == msgId)
    {
      break;
    }
    else
    {
      // Just return the message with NULL id
      RPC_IPC_sendMsg(msg, RPC_NULL);
    }
  }
}

int32_t RPC_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;
  
  // If already initialized, just exit
  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }

  // Create the local receive message queue (RPC FROM GPP queue)
  MessageQ_rpcFromGPP = MessageQ_create(RPC_FROM_GPP_MSGQNAME, NULL);
  if (MessageQ_rpcFromGPP == NULL)
  {
    Log_error0("MessageQ_create failed\n" );
    return C6RUN_FAIL;
  }

  // Connect to the remote sending message queue
  do
  {
    status = MessageQ_open(RPC_TO_GPP_MSGQNAME, &MessageQ_rpcToGPPId);
    if (status == MessageQ_E_NOTFOUND)
    {
      // Sleep for a while before trying again.
      Task_sleep (10);
    }
  }
  while (status != MessageQ_S_SUCCESS);
  
  // Start the RPC calling receive thread
  {
    Task_Params taskParams; 
 
    Task_Params_init(&taskParams);
  
    // Start RPC task
    RPC_IPC_recvThread = Task_create((Task_FuncPtr)&RPC_IPC_receiver, &taskParams, NULL); 
    if (RPC_IPC_recvThread == NULL)
    {
      Log_error0("Create C6RUN_RPC_task: Failed");
    }
  }
  
  completedThreadsCnt = 0;
  
  LOCAL_isModuleInitialized = true;
  
  return status;
}

void RPC_IPC_exit( void )
{
  int32_t status;
  
  // If not already initialized, just exit
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  
  // Send the RPC_TERMINATE command
  {
    RPC_Msg *msg;
  
    RPC_IPC_allocMsg(&msg);
    RPC_IPC_sendMsg(msg, RPC_TERMINATE);
  }
  
  // Delete the receive thread
  Task_delete(&RPC_IPC_recvThread);

  // Disconnect from the remote queue on the GPP
  status = MessageQ_close (&MessageQ_rpcToGPPId);
  if (status < 0)
  {
    Log_error0("MessageQ_close failed\n");
  }
  
  // Delete the local receive queue
  status = MessageQ_delete (&MessageQ_rpcFromGPP);
  if (status < 0)
  {
    Log_error0("MessageQ_delete failed\n");
  }
  

  LOCAL_isModuleInitialized = false;
}

// RPC_IPC_sendMsg - Sends the data and parameters on to the GPP
void RPC_IPC_sendMsg( RPC_Msg *msg, uint16_t msgId )
{
  int32_t status;

  // Setup message and send  
  MessageQ_setMsgId((MessageQ_Msg) msg, msgId);
  MessageQ_setReplyQueue(MessageQ_rpcFromGPP,(MessageQ_Msg) msg);

  status = MessageQ_put(MessageQ_rpcToGPPId, (MessageQ_Msg) msg);
  
  if (status < 0)
  { 
    Log_error1("MessageQ_put() failed: status = [0x%x]\n", status);
  }
}

// RPC_IPC_recvMsg - Reads the data and parameters passed from the GPP
uint16_t RPC_IPC_recvMsg( RPC_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  status = MessageQ_get(MessageQ_rpcFromGPP, (MessageQ_Msg *) msg, MessageQ_FOREVER);

  if (status < 0)
  {
    Log_error1("MessageQ_get() failed: status = [0x%x]\n", status); 
    return RPC_NULL;
  }

  msgId = MessageQ_getMsgId((MessageQ_Msg) (*msg));
  
  return msgId;
}

void RPC_IPC_allocMsg( RPC_Msg **msg )
{
  // Do MessageQ alloc for a remote procedure call
  *msg = (RPC_Msg *) MessageQ_alloc(COMMON_HEAP_ID, sizeof(RPC_Msg));
  if ((*msg) == NULL)
  {
    Log_error0 ("MessageQ_alloc() failed.");
  } 
}

void RPC_IPC_freeMsg( RPC_Msg *msg )
{
  VERBOSE_PRINT0("Entering RPC_IPC_freeMsg()\n");

  MessageQ_free((MessageQ_Msg) msg);
  
  VERBOSE_PRINT0("Exiting RPC_IPC_freeMsg()\n");
}

void *RPC_IPC_getMsgBuffer ( RPC_Msg *msg )
{
  return (void *) msg->buffer;
}

uint32_t RPC_IPC_getMsgBufferSize( void )
{
  return RPC_BUFSZ;
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
