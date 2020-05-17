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
#include <zcpy_mqt.h>
#include <sma_pool.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- Other IPC specific headers    */
#include "cio_ipc.h"
#include "rpc_ipc.h"
#include "dsplink/ipc_common.h"

/*  ----------------------------------- This module's header          */
#include "control_ipc.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/

// Size of the buffer passed in C6Run messages.
#define CONTROL_BUFSZ             (CONTROL_MSG_TOTALSIZE - sizeof(MSGQ_MsgHeader))


/************************************************************
* Local Typedef Declarations                                *
************************************************************/

struct CONTROL_Msg
{
  MSGQ_MsgHeader header;
  volatile uint8_t buffer[CONTROL_BUFSZ];
};


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static SEM_Obj CONTROL_NotifySemObj;

/* Array of local message queues */
static MSGQ_Obj msgQueues[NUM_MSG_QUEUES];

static SMAPOOL_Params PoolParams =
{
  0, /* Pool ID */
  TRUE
};

/* Array of pools. */
static POOL_Obj pools[NUM_POOLS] =
{
  {
    &SMAPOOL_init,               /* Init Function                      */
    (POOL_Fxns *) &SMAPOOL_FXNS, /* Pool interface functions           */
    &PoolParams,              /* Pool params                        */
    NULL                            /* Pool object: Set within pool impl. */
  }
};

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

MSGQ_Queue MSGQ_controlFromGPP, MSGQ_controlToGPP;

#if !defined (DSP_BOOTMODE_NOBOOT)
/* Extern declaration for msgq parameters for the array of pools */
ZCPYMQT_Params mqtParams = {COMMON_POOL_ID};
#endif

/* Array of transports. */
MSGQ_TransportObj transports[MAX_PROCESSORS] =
{
#if (MAX_PROCESSORS == 2 ) /* In case of one DSP and one GPP processor */
     MSGQ_NOTRANSPORT,    /* Represents the local processor */
#endif
     {
        &ZCPYMQT_init,    /* Init Function                 */
        &ZCPYMQT_FXNS,    /* Transport interface functions */
        &mqtParams,       /* Transport params              */
        NULL,             /* Filled in by transport        */
        ID_GPP            /* Processor Id                  */
     }
};

/* MSGQ configuration information.
 * MSGQ_config is a required global variable. */
MSGQ_Config MSGQ_config =
{
    msgQueues,
    transports,
    NUM_MSG_QUEUES,
    MAX_PROCESSORS,
    0,
    MSGQ_INVALIDMSGQ,
    POOL_INVALIDID
};

/* POOL configuration information.
 * POOL_config is a required global variable. */
POOL_Config POOL_config = {pools, NUM_POOLS};


/************************************************************
* Global Function Definitions                               *
************************************************************/

int CONTROL_IPC_init( void )
{
  int32_t status = C6RUN_SUCCESS;
  MSGQ_Attrs MSGQ_attrs = MSGQ_ATTRS;    
  MSGQ_LocateAttrs MSGQ_locateAttrs;  
  
  // If already initialized, just exit
  if (LOCAL_isModuleInitialized)
  {
    return status;
  }    
  
  // Init message queues
  MSGQ_controlFromGPP   = MSGQ_INVALIDMSGQ;
  MSGQ_controlToGPP     = MSGQ_INVALIDMSGQ;
  
  // Init semaphore objects that underly the message queues
  SEM_new(&CONTROL_NotifySemObj, 0);

  // Fill in the attributes for this message queue.
  MSGQ_attrs.notifyHandle = &CONTROL_NotifySemObj;
  MSGQ_attrs.pend = (MSGQ_Pend) SEM_pendBinary;
  MSGQ_attrs.post = (MSGQ_Post) SEM_postBinary;
  
  // Open Message Queues.
  status = MSGQ_open((String) CONTROL_FROM_GPP_MSGQNAME, &MSGQ_controlFromGPP, &MSGQ_attrs);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_open failed", 0);
    return C6RUN_FAIL;
  }  

  // Set the message queue that will receive any async. errors.
  MSGQ_setErrorHandler(MSGQ_controlFromGPP, COMMON_POOL_ID);

  // Locate the GPP-side receive message queues
  MSGQ_locateAttrs.timeout = SYS_FOREVER;
  while(1)
  {
    status = MSGQ_locate((String) CONTROL_TO_GPP_MSGQNAME, &MSGQ_controlToGPP, &MSGQ_locateAttrs);  
    
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

  // Reset the error handler before deleting the CONTROL MSGQ
  MSGQ_setErrorHandler(MSGQ_INVALIDMSGQ, POOL_INVALIDID);

  // Release the CONTROL to-GPP message queue
  if (MSGQ_controlToGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_release(MSGQ_controlToGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_release failed", 0);
    }
  }
  
  // Delete the local CONTROL from-GPP message queue
  if (MSGQ_controlFromGPP != MSGQ_INVALIDMSGQ)
  {
    status = MSGQ_close(MSGQ_controlFromGPP);
    if (status != SYS_OK)
    {
      LOG_error("MSGQ_close failed", 0);
    }
  }
  
  LOCAL_isModuleInitialized = false;
}

// CONTROL_IPC_sendMsg - Sends the passed data and parameters on to the GPP
void CONTROL_IPC_sendMsg(CONTROL_Msg *msg, uint16_t command)
{
  int32_t status;
  
  // Setup message and send
  MSGQ_setMsgId((MSGQ_Msg) msg, command);
  MSGQ_setSrcQueue((MSGQ_Msg) msg, MSGQ_controlFromGPP);
  
  status = MSGQ_put(MSGQ_controlToGPP, (MSGQ_Msg) msg);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_put failed", 0);
    MSGQ_free((MSGQ_Msg) msg);
  }
}

// CONTROL_IPC_recv - Reads the data and parameters passed from the GPP
uint16_t CONTROL_IPC_recvMsg( CONTROL_Msg **msg )
{
  int32_t status;
  uint16_t msgId;

  status = MSGQ_get(MSGQ_controlFromGPP, (MSGQ_Msg *) msg, SYS_FOREVER);
  if (status != SYS_OK)
  {
    LOG_error("MSGQ_get failed", 0);
    return CONTROL_NULL;
  }
  
  msgId = MSGQ_getMsgId((MSGQ_Msg) (*msg));
  
  if (msgId == MSGQ_ASYNCERRORMSGID)
  { 
    LOG_error("Transport error", 0);
    MSGQ_free((MSGQ_Msg) (*msg));
    return CONTROL_NULL;
  }  
  
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
