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

/************************************************************************
*  Defines Remote Procedure Call task.                                  *
************************************************************************/


/*  ----------------------------------- Headers for User Code       */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/*  ----------------------------------- C6Run common Header         */
#include "C6Run_common.h"

/*  ----------------------------------- Application Headers         */
#include "rpc_ipc.h"
#include "C6Run_log.h"

/*  ----------------------------------- This Module's Header        */
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

typedef void (*pC6RUN_RPC_stubFxn)(void *msgBuffer);


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

#if (0)
// Task function to execute the remote procedure call
void C6RUN_RPC_task( RPC_Msg *msg )
{
  /*-------------------------------------------------------------
  * Get a pointer to the RPC packet. The packet has 2 components
  *   1) Stub function address
  *   2) Stub data packet (collection of packed parameters, cache 
  *      commands, etc.)
  *------------------------------------------------------------*/
  uint32_t *p = (uint32_t *) RPC_IPC_getMsgBuffer(msg);
  pC6RUN_RPC_stubFxn stubFxn;
  
  // Get Stub Function Address from message buffer
  stubFxn = (pC6RUN_RPC_stubFxn) *p;
  
  // Call Stub function, passing in message buffer
  (*stubFxn)((void *)p);
  
  // Send back the message to the sender (we assume any result data
  // has been placed in the message by the stub function)
  RPC_IPC_sendMsg(msg, RPC_GPP2DSP_FXNCALL_RET);
}
#endif

// Init routine to setup the RPC IPC layer
int32_t C6RUN_RPC_init( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    int32_t status = C6RUN_SUCCESS;
    status |= RPC_IPC_init();

    if (status != C6RUN_SUCCESS)
    {
      C6RUN_LOG_error0("C6RUN_CIO_init() failed.");
      return status;
    }
    
    LOCAL_isModuleInitialized = true;
  }
  return C6RUN_SUCCESS;
}

// Exit routine to clean up the RPC IPC layer
void C6RUN_RPC_exit( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  else
  {
    // Clean-up the RPC IPC layer
    RPC_IPC_exit();
    LOCAL_isModuleInitialized = false;
  }
}


/************************************************************
* Local Function Definitions                                *
************************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
