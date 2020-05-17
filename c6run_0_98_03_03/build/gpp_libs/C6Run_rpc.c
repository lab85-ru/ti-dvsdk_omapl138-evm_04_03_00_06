/*
 * C6Run_rpc.c
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
#include <time.h>

/*  ----------------------------------- OS Specific Headers           */


/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "rpc_ipc.h"
#include "rpc_ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_rpc.h"

/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

struct _C6RUN_ASYNC_STRUCT_ 
{
  RPC_SyncStruct rpc;
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


/************************************************************
* Global Function Definitions                               *
************************************************************/

// C6Run RPC initialzation routine
int32_t C6RUN_RPC_init( void )
{

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
    
  VERBOSE_PRINT0("Entered C6RUN_RPC_init()\n");

  if (C6RUN_SUCCESS != RPC_IPC_init())
  {
    printf("RPC_IPC_init() failed.");
    return C6RUN_FAIL;
  }
  
  // Register exit routine for calling upon program termination
  atexit( C6RUN_RPC_exit );    
  
  LOCAL_isModuleInitialized = true;
  
  VERBOSE_PRINT0("Leaving C6RUN_RPC_init()\n");
  
  return C6RUN_SUCCESS;  
}

// C6Run RPC cleanup routine
void C6RUN_RPC_exit( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_RPC_exit()\n");
  
  RPC_IPC_exit();
  LOCAL_isModuleInitialized = false;
  
  VERBOSE_PRINT0("Leaving C6RUN_RPC_exit()\n");
}

// Send function call message and await reply indicating completion
void C6RUN_RPC_syncDispatch(void *msg)
{
  uint16_t msgId;
  
  VERBOSE_PRINT0("Entered C6RUN_RPC_syncDispatch()\n");

  {
    // Send the message
    RPC_SyncStruct *pSync;
    RPC_IPC_sendMsg(msg, RPC_GPP2DSP_FXNCALL, &pSync);
 
    // Wait to get the return message from the DSP
    msgId = RPC_IPC_recvMsg( (RPC_Msg **) &msg, pSync );
  }
  
  if (msgId != RPC_GPP2DSP_FXNCALL_RET)
  {
    printf("MSGQ_getMsgId is not RPC_GPP2DSP_FXNCALL_RET.");
    exit(1);
  }
  
  VERBOSE_PRINT0("Leaving C6RUN_RPC_syncDispatch()\n");
}

void C6RUN_RPC_asyncDispatchSend(void *msg, C6RUN_RPC_AsyncHandle *hAsync)
{ 
  VERBOSE_PRINT0("Entered C6RUN_RPC_asyncDispatchSend()\n");

  {
    // Send the message
    RPC_SyncStruct *pSync;
    RPC_IPC_sendMsg(msg, RPC_GPP2DSP_FXNCALL, &pSync);
    
    *hAsync = (C6RUN_RPC_AsyncHandle) pSync;
  }
  
  VERBOSE_PRINT0("Leaving C6RUN_RPC_asyncDispatchSend()\n");
}

bool C6RUN_RPC_asyncIsComplete(C6RUN_RPC_AsyncHandle hAsync)
{
  RPC_SyncStruct *pSync;

  pSync = (RPC_SyncStruct *) hAsync;
  
  return RPC_IPC_hasReceivedMsg(pSync);
}

void C6RUN_RPC_asyncDispatchRecv(void *msg, C6RUN_RPC_AsyncHandle hAsync)
{
  uint16_t msgId;
  
  VERBOSE_PRINT0("Entered C6RUN_RPC_asyncDispatchRecv()\n");

  {
    // Send the message
    RPC_SyncStruct *pSync;

    pSync = (RPC_SyncStruct *) hAsync;
 
    // Wait to get the return message from the DSP
    msgId = RPC_IPC_recvMsg( (RPC_Msg **) &msg, pSync );
  }
  
  if (msgId != RPC_GPP2DSP_FXNCALL_RET)
  {
    printf("MSGQ_getMsgId is not RPC_GPP2DSP_FXNCALL_RET.");
    exit(1);
  }  
  
  VERBOSE_PRINT0("Leaving C6RUN_RPC_asyncDispatchRecv()\n");
}

void* C6RUN_RPC_asyncGetMsg( C6RUN_RPC_AsyncHandle hAsync )
{
  return (void *) hAsync->rpc.msg;
}

void* C6RUN_RPC_allocMsgBuff(void **msg)
{
  RPC_Msg **rpcMsg = (RPC_Msg **)msg;
  RPC_IPC_allocMsg( rpcMsg );

  return (void *) RPC_IPC_getMsgBuffer(*rpcMsg);
}

void C6RUN_RPC_freeMsgBuff(void *msg)
{
  RPC_IPC_freeMsg((RPC_Msg *) msg);
}

void* C6RUN_RPC_getMsgBuffer(void *msg)
{ 
  return (void *) RPC_IPC_getMsgBuffer((RPC_Msg *) msg);
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
