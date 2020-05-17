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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*  ----------------------------------- XDC Runtime Headers           */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/IHeap.h>

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
#include "cio_ipc.h"
#include "rpc_ipc.h"
#include "syslink/ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "control_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in C6Run messages.
#define CONTROL_BUFSZ             (CONTROL_MSG_TOTALSIZE - sizeof(MessageQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

struct CONTROL_Msg
{
  MessageQ_MsgHeader header;
  volatile uint8_t buffer[CONTROL_BUFSZ];
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

HeapMemMP_Handle  heapHandle;

uint16_t          hostProcId;

MessageQ_Handle   MessageQ_controlFromGPP;
MessageQ_QueueId  MessageQ_controlToGPPId;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int CONTROL_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;
  
  // If already initialized, just exit
  if (LOCAL_isModuleInitialized)
  {
    return status;
  }
  
  // Save the host Processor ID
  hostProcId = MultiProc_getId("HOST");
  
  // Connect to the host processor. Loop until connected
  do
  {
    status = Ipc_attach (hostProcId);
  }
  while (status < 0);

  // Open the heap created by the other processor. Loop until opened.
  do
  {
    status = HeapMemMP_open(COMMON_HEAP_NAME, &heapHandle);
    if (status == HeapMemMP_E_NOTFOUND)
    {
      /* Sleep for a while before trying again. */
      Task_sleep (10);
    }
  }
  while (status != HeapMemMP_S_SUCCESS);

  // Register this heap with the MessageQ module
  MessageQ_registerHeap((IHeap_Handle)heapHandle, COMMON_HEAP_ID);

  // Create a message queue instance
  MessageQ_controlFromGPP = MessageQ_create(CONTROL_FROM_GPP_MSGQNAME, NULL);
  if (MessageQ_controlFromGPP == NULL)
  {
    Log_error0("MessageQ_create failed\n" );
    return C6RUN_FAIL;
  }

  // Open the remote message queue. Spin until it is ready.
  do
  {
    status = MessageQ_open(CONTROL_TO_GPP_MSGQNAME, &MessageQ_controlToGPPId);
    if (status == MessageQ_E_NOTFOUND)
    {
      // Sleep for a while before trying again.
      Task_sleep (10);
    }
  }
  while (status != MessageQ_S_SUCCESS);

  LOCAL_isModuleInitialized = true;
  
  return status;
}

void CONTROL_IPC_exit( void )
{
  int32_t status;

  // If already initialized, just exit
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  // Delete the local receive queue
  status = MessageQ_delete (&MessageQ_controlFromGPP);
  if (status < 0)
  {
    Log_error0("MessageQ_delete failed\n");
  }

  // Disconnect from the remote queue on the GPP
  status = MessageQ_close (&MessageQ_controlToGPPId);
  if (status < 0)
  {
    Log_error0("MessageQ_close failed\n");
  }

  MessageQ_unregisterHeap (COMMON_HEAP_ID);
  status = HeapMemMP_close (&heapHandle);
  if (status < 0)
  {
    Log_error0("HeapMemMP_close failed\n" );
  }
  
  // Spin until Ipc_detach success then stop
  do
  {
    status = Ipc_detach(hostProcId);
  }
  while (status < 0);
  
  Ipc_stop();
  
  LOCAL_isModuleInitialized = false;
}

// CONTROL_IPC_sendMsg - Sends the passed data and parameters on to the GPP
void CONTROL_IPC_sendMsg(CONTROL_Msg *msg, uint16_t command)
{
  int32_t status;

  // Setup message and send
  MessageQ_setMsgId((MessageQ_Msg) msg, command);
  MessageQ_setReplyQueue(MessageQ_controlFromGPP,(MessageQ_Msg) msg);

  status = MessageQ_put(MessageQ_controlToGPPId, (MessageQ_Msg) msg);
  
  if (status < 0)
  { 
    Log_error1("MessageQ_put() failed: status = [0x%x]\n", status);
  }
}

// CONTROL_IPC_recv - Reads the data and parameters passed from the GPP
uint16_t CONTROL_IPC_recvMsg( CONTROL_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  status = MessageQ_get(MessageQ_controlFromGPP, (MessageQ_Msg *) msg, MessageQ_FOREVER);

  if (status < 0)
  {
    Log_error1("MessageQ_get() failed: status = [0x%x]\n", status); 
    return CONTROL_NULL;
  }

  msgId = MessageQ_getMsgId((MessageQ_Msg) (*msg));
  
  return msgId;
}

void *CONTROL_IPC_getMsgBuffer( CONTROL_Msg *msg )
{
  return (void *) msg->buffer;
}

uint32_t CONTROL_IPC_getMsgBufferSize( void )
{
  return CONTROL_BUFSZ;
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
