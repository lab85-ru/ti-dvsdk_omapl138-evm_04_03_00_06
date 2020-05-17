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

/*  ----------------------------------- This module's header        */
#include "cio_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in C6Run messages.
#define CIO_BUFSZ             (CIO_MSG_TOTALSIZE - sizeof(MSGQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

struct CIO_Msg
{
  MSGQ_MsgHeader header;
  volatile uint8_t buffer[CIO_BUFSZ];
};


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static SEM_Obj CIO_IPC_NotifySemObj;
static SEM_Obj CIO_IPC_msgSemObj;

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

MSGQ_Queue MSGQ_cioFromGPP, MSGQ_cioToGPP;
CIO_Msg *CIO_msg;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int CIO_IPC_init( void )
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
  MSGQ_cioFromGPP       = MSGQ_INVALIDMSGQ; 
  MSGQ_cioToGPP         = MSGQ_INVALIDMSGQ;
  
  // Init semaphore objects that underly the message queues
  SEM_new(&CIO_IPC_NotifySemObj, 0);
  SEM_new(&CIO_IPC_msgSemObj,CIO_MSG_CNT);

  // Fill in the attributes for this message queue
  MSGQ_attrs.notifyHandle = &CIO_IPC_NotifySemObj;
  MSGQ_attrs.pend = (MSGQ_Pend) SEM_pendBinary;
  MSGQ_attrs.post = (MSGQ_Post) SEM_postBinary;
  
  // Open Message Queues
  status = MSGQ_open((String) CIO_FROM_GPP_MSGQNAME, &MSGQ_cioFromGPP, &MSGQ_attrs);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_open failed", 0);
    return C6RUN_FAIL;
  }
  
  // Locate the GPP-side receive message queues
  MSGQ_locateAttrs.timeout = SYS_FOREVER;
  while(1)
  {
    status = MSGQ_locate((String) CIO_TO_GPP_MSGQNAME, &MSGQ_cioToGPP, &MSGQ_locateAttrs);
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
  if (MSGQ_cioToGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_release(MSGQ_cioToGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_release failed", 0);
    }
  }

  // Delete the local receive queue
  if (MSGQ_cioFromGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_close(MSGQ_cioFromGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_close failed", 0);
    }
  }
  
  LOCAL_isModuleInitialized = FALSE;
}

void CIO_IPC_sendMsg( CIO_Msg *msg, uint16_t msgId )
{
  int32_t status;

  MSGQ_setMsgId((MSGQ_Msg) msg, msgId);
  MSGQ_setSrcQueue((MSGQ_Msg) msg, MSGQ_cioFromGPP);
  
  status = MSGQ_put(MSGQ_cioToGPP, (MSGQ_Msg) msg);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_put failed", 0);
    MSGQ_free((MSGQ_Msg) msg);
  }
}

uint16_t CIO_IPC_recvMsg( CIO_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  status = MSGQ_get(MSGQ_cioFromGPP, (MSGQ_Msg *) msg, SYS_FOREVER);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_get failed", 0);
    return CIO_NULL;
  }
  
  msgId = MSGQ_getMsgId((MSGQ_Msg) (*msg));

  return msgId;
}

void CIO_IPC_allocMsg( CIO_Msg **msg )
{
  SEM_pend(&CIO_IPC_msgSemObj,SYS_FOREVER);
  
  {
    Int status = MSGQ_alloc(COMMON_POOL_ID, (MSGQ_Msg *) msg, sizeof(CIO_Msg));
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_alloc for CIO_TRANSFER failed", 0);
      *msg =  NULL;
    }
  }
}

void CIO_IPC_freeMsg( CIO_Msg *msg )
{
  MSGQ_free((MSGQ_Msg) msg);
  SEM_post(&CIO_IPC_msgSemObj);
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
