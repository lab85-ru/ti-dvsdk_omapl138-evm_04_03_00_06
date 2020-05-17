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
#include <pthread.h>
#include <semaphore.h>

/*  ----------------------------------- DSP/BIOS Link                 */
#include <dsplink.h>

/*  ----------------------------------- DSP/BIOS LINK API             */
#include <proc.h>
#include <procdefs.h>
#include <msgq.h>
#include <pool.h>
#include <loaderdefs.h>

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
  MSGQ_MsgHeader header;
  intptr_t                pSyncStruct;
  volatile unsigned char buffer[RPC_BUFSZ];
};


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static sem_t RPC_IPC_msgSemObj;

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/



pthread_t RPC_IPC_recvThread;

// Global CONTROL MSGQ Objects for Tx and Rx
MSGQ_Queue MSGQ_rpcFromGPP = (uint32_t) MSGQ_INVALIDMSGQ;
MSGQ_Queue MSGQ_rpcToGPP = (uint32_t) MSGQ_INVALIDMSGQ;


/************************************************************
* Global Function Definitions                               *
************************************************************/

void *RPC_IPC_receiver( void *ptr )
{
  DSP_STATUS status = DSP_SOK;
  RPC_Msg *msg;
  RPC_SyncStruct *pSync;
  uint16_t msgId;
  
  while(1)
  {
    
    status = MSGQ_get(MSGQ_rpcToGPP, WAIT_FOREVER, (MsgqMsg *) &msg);

    if (DSP_FAILED(status))
    {
      REPORT_STATUS("MSGQ_get"); 
      break;
    }
  
    msgId = MSGQ_getMsgId((MsgqMsg) msg);

    if (RPC_TERMINATE == msgId)
    {
      // If we got the terminate message, then the DSP has been told to shut down
      // So free the message and then quit receiving stuff (kill this thread)
      RPC_IPC_freeMsg(msg);
      break;
    }
    else
    {
      pSync = (RPC_SyncStruct *) msg->pSyncStruct;
      
      // Fill in the message pointer
      pSync->msg = msg;
      
      // Notify the calling thread that the function call has returned
      sem_post(&(pSync->sem));
    }
  }
  
  pthread_exit(NULL);
}

int32_t RPC_IPC_init( void )
{
  DSP_STATUS status = DSP_SOK;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered RPC_IPC_init()\n");

  // Open the RPC TO GPP message queue
  if (DSP_SUCCEEDED(status))
  {
    status = MSGQ_open(RPC_TO_GPP_MSGQNAME, &MSGQ_rpcToGPP, NULL);
    if (DSP_FAILED(status))
    {
      REPORT_STATUS("MSGQ_open");
      return C6RUN_FAIL;
    }
  }
  
  // Connect to the remote sending message queue
  if (DSP_SUCCEEDED(status))
  {
    MSGQ_LocateAttrs syncLocateAttrs;
  
    syncLocateAttrs.timeout = WAIT_FOREVER;
    status = DSP_ENOTFOUND;
    while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
    {
      status = MSGQ_locate(RPC_FROM_GPP_MSGQNAME, &MSGQ_rpcFromGPP, &syncLocateAttrs);
      if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
      {
        VERBOSE_PRINT0("RPC MSGQ_locate() failed! Will retry.\n");
        usleep(10000);
      }
      else if (DSP_FAILED(status))
      {
        REPORT_STATUS("MSGQ_locate");
        return C6RUN_FAIL;
      }
    }
  }

  // Setup a counting semaphore to limit the in-flight messages
  if( sem_init(&RPC_IPC_msgSemObj,0,RPC_MSG_CNT) < 0 )
  {
    printf("sem_init() failed");
    exit(0);
  }
  
  // Start the RPC return receive thread
  if (pthread_create(&RPC_IPC_recvThread, NULL, RPC_IPC_receiver, NULL) != 0)
  {
    printf("Aborting...pthread_create() for RPC receiver failed!\n");
    exit(1);
  }
  VERBOSE_PRINT0("pthread_create() passed!\n");
  
  LOCAL_isModuleInitialized = true;
  
  VERBOSE_PRINT0("Leaving RPC_IPC_init()\n");
  
  return C6RUN_SUCCESS;
}

void RPC_IPC_exit( void )
{
  DSP_STATUS status = DSP_SOK;
  DSP_STATUS tmpStatus = DSP_SOK;
  
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  VERBOSE_PRINT0("Entered RPC_IPC_exit()\n");
  
  // Wait for RPC_IPC_receiver to shutdown
  pthread_join(RPC_IPC_recvThread, NULL);

  VERBOSE_PRINT0("Completed shutdown of RPC_IPC_receiver thread.\n");
  
  if( sem_destroy(&RPC_IPC_msgSemObj) < 0 )
  {
    printf("sem_destroy() failed");
    exit(0);
  }
  
  // Release the remote message queue
  status = MSGQ_release(MSGQ_rpcFromGPP);
  if (DSP_FAILED(status))
  {
    REPORT_STATUS("MSGQ_release");
  }
  else
  {
    VERBOSE_PRINT0("MSGQ_release() passed.\n");
  }

  // Close the GPP's RPC message queue
  tmpStatus = MSGQ_close(MSGQ_rpcToGPP);
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    REPORT_STATUS("MSGQ_close");
  }
  else
  {
    VERBOSE_PRINT0("MSGQ_close() passed.\n");
  }

  LOCAL_isModuleInitialized = false;

  VERBOSE_PRINT0("Leaving RPC_IPC_exit()\n");
}

void RPC_IPC_sendMsg( RPC_Msg *msg, uint16_t msgId, RPC_SyncStruct **pSyncPtr )
{
  DSP_STATUS status;
  RPC_SyncStruct *pSync;
  
  VERBOSE_PRINT0("Entering RPC_IPC_sendMsg()\n");

  MSGQ_setMsgId((MsgqMsg)msg, msgId);

  // Create a sync structure, local to this thread.
  pSync = (RPC_SyncStruct *) malloc(sizeof(RPC_SyncStruct));
  *pSyncPtr = pSync;
  
  // Init the semaphore (part of the sync struct) that the receive
  // thread can post to, and that the sending thread (the one calling
  // this function) can block on. Initial count should be zero.
  sem_init(&(pSync->sem),0,0);
  
  // Put the pointer to the sync structure in the message to be sent.
  // This pointer will come back with the return message so that the 
  // receiving thread can use it to fill in the return message and notify
  // the calling thread (the one calling this function).
  msg->pSyncStruct = (intptr_t) pSync;
  
  // Send the message
  status = MSGQ_put(MSGQ_rpcFromGPP, (MsgqMsg) msg);
  
  if (DSP_FAILED(status))
  { 
    MSGQ_free((MsgqMsg) msg); 
    REPORT_STATUS("MSGQ_put"); 
  }
  
  VERBOSE_PRINT0("Exiting RPC_IPC_sendMsg()\n");
}

bool RPC_IPC_hasReceivedMsg( RPC_SyncStruct *pSync )
{
  int result = sem_trywait(&(pSync->sem));
 
  if ( result < 0)
  {
    return false;
  }
  else
  {
    // Post the semaphore again so that the recvMsg call will get it
    sem_post(&(pSync->sem));
    return true;
  }
}

uint16_t RPC_IPC_recvMsg( RPC_Msg **msg, RPC_SyncStruct *pSync)
{
  uint16_t msgId;
  
  VERBOSE_PRINT0("Entering RPC_IPC_recvMsg()\n");

  // Wait for the receive thread to post the semaphore, then destroy it
  sem_wait(&(pSync->sem));
  sem_destroy(&(pSync->sem));
  
  if (NULL == pSync->msg)
  {
    return RPC_NULL;
  }
  
  *msg = pSync->msg;

  msgId = MSGQ_getMsgId((MsgqMsg) (*msg));
  
  // Free the sync structure now we are done with it
  free(pSync);

  VERBOSE_PRINT0("Exiting RPC_IPC_recvMsg()\n");
  
  return msgId;
}

void RPC_IPC_allocMsg( RPC_Msg **msg )
{
  VERBOSE_PRINT0("Entering RPC_IPC_allocMsg()\n");

  // Semaphore wait on an available message
  sem_wait(&RPC_IPC_msgSemObj);
  
  {
    DSP_STATUS status = DSP_SOK;
    
    // Do MSGQ alloc for a remote procedure call
    status = MSGQ_alloc(COMMON_POOL_ID, sizeof(RPC_Msg), (MSGQ_Msg *) msg );
    if (DSP_FAILED(status))
    { 
      REPORT_STATUS("MSGQ_alloc");
      *msg =  NULL;
    }
  }
  
  VERBOSE_PRINT0("Exiting RPC_IPC_allocMsg()\n");
}

void RPC_IPC_freeMsg( RPC_Msg *msg )
{
  VERBOSE_PRINT0("Entering RPC_IPC_freeMsg()\n");

  MSGQ_free((MSGQ_Msg) msg);
  
  sem_post(&RPC_IPC_msgSemObj);
  
  VERBOSE_PRINT0("Exiting RPC_IPC_freeMsg()\n");
}

void *RPC_IPC_getMsgBuffer( RPC_Msg *msg )
{ 
  return (void *) (msg->buffer);
}

uint32_t RPC_IPC_getMsgBufferSize ( void )
{
  return RPC_BUFSZ;
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

