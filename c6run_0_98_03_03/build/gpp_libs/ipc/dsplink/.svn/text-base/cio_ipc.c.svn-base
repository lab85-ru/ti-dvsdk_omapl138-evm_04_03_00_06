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
#include "cio_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

extern int32_t fnomap[CIO_NFILE+1];


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

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

// Global CIO MSGQ Objects for Tx and Rx
MSGQ_Queue MSGQ_cioToGPP = (uint32_t) MSGQ_INVALIDMSGQ;
MSGQ_Queue MSGQ_cioFromGPP = (uint32_t) MSGQ_INVALIDMSGQ;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int CIO_IPC_init( void )
{
  DSP_STATUS status = DSP_SOK;

  if (LOCAL_isModuleInitialized)
  {
    return C6RUN_SUCCESS;
  }
  
  VERBOSE_PRINT0("Entered CIO_IPC_init()\n");

  // Open the CIO TO GPP message queue
  if (DSP_SUCCEEDED(status))
  {
    status = MSGQ_open(CIO_TO_GPP_MSGQNAME, &MSGQ_cioToGPP, NULL);
    if (DSP_FAILED(status))
    {
      REPORT_STATUS("MSGQ_open");
      return C6RUN_FAIL;
    }
  }
  
  // Locate the CIO FROM GPP message queue
  if (DSP_SUCCEEDED(status))
  {
    MSGQ_LocateAttrs syncLocateAttrs;
  
    syncLocateAttrs.timeout = WAIT_FOREVER;
    status = DSP_ENOTFOUND;
    while ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
    {
      status = MSGQ_locate(CIO_FROM_GPP_MSGQNAME, &MSGQ_cioFromGPP, &syncLocateAttrs);
      if ((status == DSP_ENOTFOUND) || (status == DSP_ENOTREADY))
      {
        VERBOSE_PRINT0("CIO MSGQ_locate() failed! Will retry.\n");
        usleep(10000);
      }
      else if (DSP_FAILED(status))
      {
        REPORT_STATUS("MSGQ_locate");
        return C6RUN_FAIL;
      }
    }
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
  DSP_STATUS status = DSP_SOK;
  DSP_STATUS tmpStatus = DSP_SOK;
  
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }

  VERBOSE_PRINT0("Entered CIO_IPC_exit()\n");
  
  // Release the remote message queue
  status = MSGQ_release(MSGQ_cioFromGPP);
  if (DSP_FAILED(status))
  {
    printf("MSGQ_release() failed. Status = [0x%x]\n", (uint32_t)status);
  }    
  VERBOSE_PRINT0("MSGQ_release() passed.\n");

  // Close the GPP's RPC message queue
  tmpStatus = MSGQ_close(MSGQ_cioToGPP);
  if (DSP_SUCCEEDED(status) && DSP_FAILED(tmpStatus))
  {
    status = tmpStatus;
    printf("MSGQ_close() failed. Status = [0x%x]\n", (uint32_t)status);
  }    
  VERBOSE_PRINT0("MSGQ_close() passed.\n");

  LOCAL_isModuleInitialized = false;

  VERBOSE_PRINT0("Leaving CIO_IPC_exit()\n");
}

void CIO_IPC_sendMsg( CIO_Msg *msg, uint16_t msgId )
{
  DSP_STATUS status;
  
  VERBOSE_PRINT0("Entering CIO_IPC_sendMsg()\n");

  MSGQ_setMsgId((MsgqMsg)msg, msgId);

  status = MSGQ_put(MSGQ_cioFromGPP, (MsgqMsg) msg);
  
  if (DSP_FAILED(status))
  { 
    MSGQ_free((MsgqMsg) msg); 
    REPORT_STATUS("MSGQ_put"); 
  }
  
  VERBOSE_PRINT0("Exiting CIO_IPC_sendMsg()\n");
}

uint16_t CIO_IPC_recvMsg( CIO_Msg **msg )
{
  DSP_STATUS status;
  uint16_t msgId;

  VERBOSE_PRINT0("Entered CIO_IPC_recvMsg()\n");

  status = MSGQ_get(MSGQ_cioToGPP, WAIT_FOREVER, (MsgqMsg *) msg);

  if (DSP_FAILED(status))
  {
    REPORT_STATUS("MSGQ_get"); 
    return CIO_NULL;
  }
  
  msgId = MSGQ_getMsgId((MsgqMsg) (*msg));

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

  {
    DSP_STATUS status = DSP_SOK;
    
    // Do MSGQ alloc for a remote procedure call
    status = MSGQ_alloc(COMMON_POOL_ID, sizeof(CIO_Msg), (MSGQ_Msg *) msg );
    if (DSP_FAILED(status))
    { 
      REPORT_STATUS("MSGQ_alloc");
      *msg =  NULL;
    }
  }
  
  VERBOSE_PRINT0("Exiting CIO_IPC_allocMsg()\n");
}

void CIO_IPC_freeMsg( CIO_Msg *msg )
{
  VERBOSE_PRINT0("Entering CIO_IPC_freeMsg()\n");

  MSGQ_free((MSGQ_Msg) msg);
  
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

