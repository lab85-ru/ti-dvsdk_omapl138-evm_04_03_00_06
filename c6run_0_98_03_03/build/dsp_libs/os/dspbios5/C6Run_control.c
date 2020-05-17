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

/************************************************************************
*  Defines DSP/Bios main entry point that serves to kick off the        *
*  user supplied main in its own task.                                  *
************************************************************************/


/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/*  ----------------------------------- DSP/BIOS Headers              */
#include "C6Runcfg.h"
#include <sys.h>
#include <sem.h>
#include <msgq.h>
#include <pool.h>
#include <gbl.h>
#include <clk.h>
#include <log.h>
#include <tsk.h>

/*  ----------------------------------- DSP/BIOS LINK Headers         */
#include <dsplink.h>
#include <failure.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"
#include "C6Run_common.h"
#include "C6Run_sharedmem.h"
#include "C6Run_rpc.h"
#include "C6Run_cio.h"
#include "C6Run_util.h"
#include "C6Run_log.h"

/*  ----------------------------------- IPC Specific Headers          */
#include "control_ipc.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_control.h"


/************************************************************
* Explicit External Declarations                            *
************************************************************/

// Declaration for main function from either user-supplied application 
// code (for C6RunApp) or from a dummy implementation (for C6RunLib)
extern int main(int argc, char* argv []);

typedef struct { int argc; char *argv[1]; } ARGS;

#pragma DATA_SECTION(__c6run_args__,".args");
uint8_t __c6run_args__[1024];

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

// We use static global variable for the message pointer
static CONTROL_Msg *msg;

static bool LOCAL_isModuleInitialized = false;


/************************************************************
* Global Variable Definitions                               *
************************************************************/


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Task function for DSP-side of C6Run control
void C6RUN_CONTROL_task( void )
{
  uint16_t msgId;
  int retVal = 0;

  // Setup inter-processor interfaces
  int32_t status = C6RUN_CONTROL_init();
  
  if (status != C6RUN_SUCCESS)
  {
    exit(1);
  }
  
  // Wait for the STARTLIB or STARTAPP message
  msgId = CONTROL_IPC_recvMsg(&msg);
  
  if (CONTROL_DSP_STARTLIB == msgId)
  {
    status = C6RUN_RPC_init();
  
    if (status != C6RUN_SUCCESS)
    {
      exit(1);
    }
  
    // Send the DSP_READY message so the GPP knows we are waiting
    CONTROL_IPC_sendMsg(msg, CONTROL_DSP_READY);
    
    // Wait for the stop message (and ONLY the stop message)
    while (1)
    {
      msgId = CONTROL_IPC_recvMsg(&msg);
      if (CONTROL_DSP_STOP == msgId)
      {
        break;
      }
    }
    
    // Shutdown the RPC activity
    C6RUN_RPC_exit();
  }
  else if (CONTROL_DSP_STARTAPP == msgId)
  {
    // Extract the main function arguments
    int i;
    
    ARGS *pargs = (ARGS*) __c6run_args__;

    uint32_t    *argsPtr  = CONTROL_IPC_getMsgBuffer(msg);
    uintptr_t   *localArgvPtr;
    uintptr_t   *remoteArgvPtr;
    
    pargs->argc   = (int) *argsPtr++;
    localArgvPtr  = (uintptr_t *) pargs->argv;
    remoteArgvPtr = (uintptr_t *) argsPtr;
    
    // Calculate new pointer for argv[0] in DSP .args section
    localArgvPtr[0] = (uintptr_t) (localArgvPtr + pargs->argc);
    
    // Update argv pointers to point to memory in .args section
    for (i = 1; i<pargs->argc; i++)
    {
      localArgvPtr[i] = (remoteArgvPtr[i] - remoteArgvPtr[0]) + localArgvPtr[0];
    }
    
    // Update argv pointer from the GPP to match current remote address space
    remoteArgvPtr[0] = (uintptr_t) (remoteArgvPtr + pargs->argc);
    
    // Copy the updated argv pointers and the actual data pointed to
    memcpy( (void *)localArgvPtr[0], (void *)remoteArgvPtr[0], (CONTROL_IPC_getMsgBufferSize() - sizeof(pargs->argc) - sizeof(uintptr_t)*pargs->argc) );

    // Call the user supplied main function
    retVal = main(pargs->argc, pargs->argv);
  }
  
  // We call the exit routine as this should be the same as what happens
  // when main() terminates.  It ensures that all atexit() registered routines
  // will get called.
  exit(retVal);
}

// Init the CONTROL routines
int32_t C6RUN_CONTROL_init( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    int32_t status = C6RUN_SUCCESS;
    status |= CONTROL_IPC_init();
    
    // Startup dependent modules
    status |= C6RUN_CIO_init();
    status |= C6RUN_UTIL_init();
    status |= C6RUN_SHAREDMEM_init();

    if (status != C6RUN_SUCCESS)
    {
      C6RUN_LOG_error0("C6RUN_CONTROL_init() failed.");
      return status;
    }
    
    LOCAL_isModuleInitialized = true;
  }
  return C6RUN_SUCCESS;
}

// Exit routine to tell the GPP host that we are exiting cleanly and 
// return a result
void C6RUN_CONTROL_exit(int retVal)
{
  // Send FINISHED message and teardown IPC and return retVal back to GPP
  memcpy( CONTROL_IPC_getMsgBuffer(msg), (void *)&retVal, sizeof(int) );
  
  // Shutdown dependent modules
  C6RUN_SHAREDMEM_exit();
  C6RUN_UTIL_exit();
  C6RUN_CIO_exit();
 
  // Control message to tell the GPP to terminate the DSP 
  CONTROL_IPC_sendMsg(msg, CONTROL_DSP_FINISHED);
  
  CONTROL_IPC_exit();
}

// Abort routine that would get called in unusual circumstances
void C6RUN_CONTROL_abort( void )
{
  // CIO exit
  C6RUN_CIO_exit();

  CONTROL_IPC_sendMsg(msg,CONTROL_DSP_ABORTED);
  
  // CONTROL_IPC_exit();

  // Die here waiting for the DSP to be shutdown  
  while(1);  
}


/************************************************************
* Local Function Definitions                                *
************************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
