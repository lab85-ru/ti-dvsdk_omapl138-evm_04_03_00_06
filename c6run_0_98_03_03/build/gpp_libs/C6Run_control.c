/*
 * C6Run_control.c
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
#include "control_ipc.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_control.h"

/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


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

// C6Run CONTROL initialzation routine
int32_t C6RUN_CONTROL_init( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    VERBOSE_PRINT0("Entered C6RUN_CONTROL_init()\n");

    if (C6RUN_SUCCESS != CONTROL_IPC_init())
    {
      printf("CONTROL_IPC_init() failed.");
      return C6RUN_FAIL;
    }
    
    // Register exit routine for calling upon program termination
    atexit( C6RUN_CONTROL_exit );
    
    LOCAL_isModuleInitialized = true;
    
    VERBOSE_PRINT0("Leaving C6RUN_CONTROL_init()\n");
  }
  
  return C6RUN_SUCCESS;  
}

// C6Run CONTROL cleanup routine
void C6RUN_CONTROL_exit( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_CONTROL_exit()\n");
  
  CONTROL_IPC_exit();
  LOCAL_isModuleInitialized = false;
  
  VERBOSE_PRINT0("Leaving C6RUN_CONTROL_exit()\n");
}

void C6RUN_CONTROL_sendMsg(void *msg, uint16_t msgId)
{
  CONTROL_IPC_sendMsg( (CONTROL_Msg *)msg, msgId );
}

uint16_t C6RUN_CONTROL_recvMsg(void **msg)
{
  return CONTROL_IPC_recvMsg( (CONTROL_Msg **) &msg);
}

void* C6RUN_CONTROL_allocMsgBuff(void **msg)
{
  CONTROL_Msg **controlMsg = (CONTROL_Msg **)msg;
  CONTROL_IPC_allocMsg( controlMsg );

  return (void *) CONTROL_IPC_getMsgBuffer(*controlMsg);
}

void C6RUN_CONTROL_freeMsgBuff(void *msg)
{
  CONTROL_IPC_freeMsg((struct CONTROL_Msg *) msg);
}

void* C6RUN_CONTROL_getMsgBuffer(void *msg)
{ 
  return (void *) CONTROL_IPC_getMsgBuffer((CONTROL_Msg *) msg);
}

uint32_t C6RUN_CONTROL_getMsgBufferSize( void )
{
  return CONTROL_IPC_getMsgBufferSize();
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
