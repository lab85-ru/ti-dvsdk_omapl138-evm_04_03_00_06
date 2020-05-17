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

/*  ----------------------------------- DSP/BIOS Headers              */
#include "C6Runcfg.h"
#include <gbl.h>
#include <sys.h>
#include <sem.h>
#include <msgq.h>
#include <pool.h>
#include <tsk.h>
#include <log.h>

/*  ----------------------------------- DSP/BIOS LINK Headers         */
#include <dsplink.h>
#include <platform.h>
#include <failure.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "dsplink/ipc_common.h"

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
#define RPC_BUFSZ        (RPC_MSG_TOTALSIZE - sizeof(MSGQ_MsgHeader) - sizeof(intptr_t))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

// Message structure for RPC messages
struct RPC_Msg
{
  MSGQ_MsgHeader          header;
  intptr_t                pSyncStruct;  
  volatile unsigned char buffer[RPC_BUFSZ];
};

typedef void (*pRPC_IPC_stubFxn)(void *msgBuffer);

/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static SEM_Obj RPC_IPC_NotifySemObj;
static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

TSK_Handle        RPC_IPC_recvThread;

MSGQ_Queue        MSGQ_rpcFromGPP, MSGQ_rpcToGPP;

TSK_Handle        RPC_IPC_completedThreads[RPC_MSG_CNT];
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
    RPC_IPC_completedThreads[completedThreadsCnt++] = TSK_self();
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
        TSK_delete(RPC_IPC_completedThreads[--completedThreadsCnt]);
      }
      _restore_interrupts(restoreVal);
    }  
  
    // Blocking call to get next incoming RPC
    msgId = RPC_IPC_recvMsg(&msg);
    
    if (RPC_GPP2DSP_FXNCALL == msgId)
    {
      // Start a new task to execute the function (pass the message buffer as argument)
      TSK_Attrs  attrs = TSK_ATTRS;

      TSK_create((Fxn)&RPC_IPC_executer, &attrs, (Arg) msg);
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
  MSGQ_Attrs MSGQ_attrs = MSGQ_ATTRS;
  MSGQ_LocateAttrs MSGQ_locateAttrs;
  
  // If already initialized, just exit
  if (LOCAL_isModuleInitialized)
  {
    return status;
  }
  
  // Init the message queues
  MSGQ_rpcFromGPP       = MSGQ_INVALIDMSGQ; 
  MSGQ_rpcToGPP         = MSGQ_INVALIDMSGQ;
  
  // Init semaphore objects that underly the message queues
  SEM_new(&RPC_IPC_NotifySemObj, 0);

  // Fill in the attributes for this message queue
  MSGQ_attrs.notifyHandle = &RPC_IPC_NotifySemObj;
  MSGQ_attrs.pend = (MSGQ_Pend) SEM_pendBinary;
  MSGQ_attrs.post = (MSGQ_Post) SEM_postBinary;
  
  // Open Message Queues
  status = MSGQ_open((String) RPC_FROM_GPP_MSGQNAME, &MSGQ_rpcFromGPP, &MSGQ_attrs);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_open failed", 0);
    return C6RUN_FAIL;
  }
  
  // Connect to the remote sending message queue
  MSGQ_locateAttrs.timeout = SYS_FOREVER;
  while(1)
  {
    status = MSGQ_locate((String) RPC_TO_GPP_MSGQNAME, &MSGQ_rpcToGPP, &MSGQ_locateAttrs);
    if (status == SYS_OK)
    {
      status = C6RUN_SUCCESS;
      break;
    }

    if (status == SYS_ENOTFOUND || status == SYS_ENODEV)
    {
      TSK_sleep(1000);
      continue;
    }

    LOG_error("MSGQ_locate failed", 0);
    status = C6RUN_FAIL;
    break;
  }

  // Start the RPC calling receive thread
  {
    // Start a new task to execute the function (pass the message buffer as argument)
    TSK_Attrs  attrs = TSK_ATTRS;

    RPC_IPC_recvThread = TSK_create((Fxn)&RPC_IPC_receiver, &attrs);
    
    // Start RPC task
    if (RPC_IPC_recvThread == NULL)
    {
      LOG_error("Create C6RUN_RPC_task: Failed",0);
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
  TSK_delete(RPC_IPC_recvThread);
  
  // Disconnect from the remote queue on the GPP
  if (MSGQ_rpcToGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_release(MSGQ_rpcToGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_release failed", 0);
    }
  }

  // Delete the local receive queue
  if (MSGQ_rpcFromGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_close(MSGQ_rpcFromGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_close failed", 0);
    }
  }
  LOCAL_isModuleInitialized = false;
}

// RPC_IPC_sendMsg - Sends the data and parameters on to the GPP
void RPC_IPC_sendMsg( RPC_Msg *msg, uint16_t msgId )
{
  int32_t status;

  // Setup message and send  
  MSGQ_setMsgId((MSGQ_Msg) msg, msgId);
  MSGQ_setSrcQueue((MSGQ_Msg) msg, MSGQ_rpcFromGPP);
  
  status = MSGQ_put(MSGQ_rpcToGPP, (MSGQ_Msg) msg);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_put failed", 0);
    MSGQ_free((MSGQ_Msg) msg);
  }
}

// RPC_IPC_recvMsg - Reads the data and parameters passed from the GPP
uint16_t RPC_IPC_recvMsg( RPC_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  status = MSGQ_get(MSGQ_rpcFromGPP, (MSGQ_Msg *) msg, SYS_FOREVER);

  if (status != SYS_OK)
  {
    LOG_error("MSGQ_get failed", 0);
    return RPC_NULL;
  }
  
  msgId = MSGQ_getMsgId((MSGQ_Msg) (*msg));
  
  return msgId;
}

void RPC_IPC_allocMsg( RPC_Msg **msg )
{
  Int status = MSGQ_alloc(COMMON_POOL_ID, (MSGQ_Msg *) msg, sizeof(RPC_Msg));
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_alloc for CIO_TRANSFER failed", 0);
    *msg =  NULL;
  }
}

void RPC_IPC_freeMsg( RPC_Msg *msg )
{
  MSGQ_free((MSGQ_Msg) msg);
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
