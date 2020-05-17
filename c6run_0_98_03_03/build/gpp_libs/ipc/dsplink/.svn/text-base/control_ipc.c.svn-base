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
#include "control_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern uint8_t C6RUN_IPC_processorId;


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in control messages.
#define CONTROL_BUFSZ        (CONTROL_MSG_TOTALSIZE-sizeof(MSGQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

// Message structure for CONTROL messages
struct CONTROL_Msg
{
  MSGQ_MsgHeader header;
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

// Global CONTROL MSGQ Objects for Tx and Rx
MSGQ_Queue MSGQ_controlFromGPP = (uint32_t) MSGQ_INVALIDMSGQ;
MSGQ_Queue MSGQ_controlToGPP = (uint32_t) MSGQ_INVALIDMSGQ;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t CONTROL_IPC_init( void )
{
  DSP_STATUS status = DSP_SOK;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered CONTROL_IPC_init()\n");
    
  // Open the CONTROL TO GPP message queue
  if (DSP_SUCCEEDED(status))
  {
    status = MSGQ_open(CONTROL_TO_GPP_MSGQNAME, &MSGQ_controlToGPP, NULL);
    if (DSP_FAILED(status))
    {
      REPORT_STATUS("MSGQ_open");
      return C6RUN_FAIL;
    }
  }
  
  // Locate the CONTROL FROM GPP message queue
  if (DSP_SUCCEEDED(status))
  {
    MSGQ_LocateAttrs syncLocateAttrs;
  
    syncLocateAttrs.timeout = WAIT_FOREVER;
    status = DSP_ENOTFOUND;
    while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
    {
      status = MSGQ_locate(CONTROL_FROM_GPP_MSGQNAME, &MSGQ_controlFromGPP, &syncLocateAttrs);
      if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
      { 
        VERBOSE_PRINT0("CONTROL MSGQ_locate() failed! Will retry.\n");
        usleep(10000);
      }
      else if (DSP_FAILED(status))
      {
        REPORT_STATUS("MSGQ_locate");
        return C6RUN_FAIL;
      }
    }
  }
  
  // Set the message queue that will receive any async. errors
  if (DSP_SUCCEEDED(status))
  {
    status = MSGQ_setErrorHandler(MSGQ_controlToGPP, POOL_makePoolId(C6RUN_IPC_processorId, COMMON_POOL_ID));
    if (DSP_FAILED(status))
    {
      printf("MSGQ_setErrorHandler () failed. Status = [0x%8X]\n", (uint32_t)status);
      return C6RUN_FAIL;
    }
  }
  VERBOSE_PRINT0("MSGQ_setErrorHandler() passed!\n");
  
  if( sem_init(&CONTROL_IPC_msgSemObj,0,CONTROL_MSG_CNT) < 0 )
  {
    printf("sem_init() failed");
    exit(0);
  }
  
  LOCAL_isModuleInitialized = true;
  
  VERBOSE_PRINT0("Leaving CONTROL_IPC_init()\n");
  
  return C6RUN_SUCCESS;
}

void CONTROL_IPC_exit( void )
{
  DSP_STATUS status = DSP_SOK;
  DSP_STATUS tmpStatus = DSP_SOK;
  
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

  // Release the remote message queue
  status = MSGQ_release(MSGQ_controlFromGPP);
  if (DSP_FAILED(status))
  {
    printf("MSGQ_release() failed. Status = [0x%8X]\n", (uint32_t)status);
  }
  else
  {
    VERBOSE_PRINT0("MSGQ_release() passed.\n");
  }

  // Close the GPP's CONTROL message queue
  tmpStatus = MSGQ_close(MSGQ_controlToGPP);
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

  VERBOSE_PRINT0("Leaving CONTROL_IPC_exit()\n");
}

void CONTROL_IPC_sendMsg( CONTROL_Msg *msg, uint16_t msgId )
{
  DSP_STATUS status;
  
  VERBOSE_PRINT0("Entering CONTROL_IPC_sendMsg()\n");

  MSGQ_setMsgId((MsgqMsg)msg, msgId);

  status = MSGQ_put(MSGQ_controlFromGPP, (MsgqMsg) msg);
  
  if (DSP_FAILED(status))
  { 
    MSGQ_free((MsgqMsg) msg); 
    REPORT_STATUS("MSGQ_put"); 
  }
  
  VERBOSE_PRINT0("Exiting CONTROL_IPC_sendMsg()\n");
}

uint16_t CONTROL_IPC_recvMsg( CONTROL_Msg **msg )
{
  DSP_STATUS status;
  uint16_t msgId;
  
  VERBOSE_PRINT0("Entering CONTROL_IPC_recvMsg()\n");

  status = MSGQ_get(MSGQ_controlToGPP, WAIT_FOREVER, (MsgqMsg *) msg);

  if (DSP_FAILED(status))
  {
    REPORT_STATUS("MSGQ_get"); 
    return CONTROL_NULL;
  }

  msgId = MSGQ_getMsgId((MsgqMsg) (*msg));

  VERBOSE_PRINT0("Exiting CONTROL_IPC_recvMsg()\n");
  
  return msgId;
}

void CONTROL_IPC_allocMsg( CONTROL_Msg **msg )
{
  VERBOSE_PRINT0("Entering CONTROL_IPC_allocMsg()\n");

  // Semaphore wait on an available message
  sem_wait(&CONTROL_IPC_msgSemObj);
  
  {
    DSP_STATUS status = DSP_SOK;
    
    // Do MSGQ alloc for a remote procedure call
    status = MSGQ_alloc(COMMON_POOL_ID, sizeof(CONTROL_Msg), (MSGQ_Msg *) msg );
    if (DSP_FAILED(status))
    { 
      REPORT_STATUS("MSGQ_alloc");
      *msg =  NULL;
    }
  }
  
  VERBOSE_PRINT0("Exiting CONTROL_IPC_allocMsg()\n");
}

void CONTROL_IPC_freeMsg( CONTROL_Msg *msg  )
{
  VERBOSE_PRINT0("Entering CONTROL_IPC_freeMsg()\n");

  MSGQ_free((MSGQ_Msg) msg);

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

