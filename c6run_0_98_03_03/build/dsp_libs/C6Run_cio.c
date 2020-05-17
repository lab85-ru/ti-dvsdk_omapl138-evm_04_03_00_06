/*
 * C6Run_cio.c
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
*  Defines functions needed for setup and teardown of CIO services      *
************************************************************************/


/*  ----------------------------------- Headers for User Code       */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/*  ----------------------------------- C6Run common Header         */
#include "C6Run_common.h"

/*  ----------------------------------- Application Headers         */
#include "cio_ipc.h"
#include "C6Run_log.h"

/*  ----------------------------------- This module's header        */
#include "C6Run_cio.h"


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

// Init routine to setup the CIO IPC layer
int32_t C6RUN_CIO_init( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    int32_t status = C6RUN_SUCCESS;
    status |= CIO_IPC_init();

    if (status != C6RUN_SUCCESS)
    {
      C6RUN_LOG_error0("C6RUN_CIO_init() failed.");
      return status;
    }
    
    LOCAL_isModuleInitialized = true;
  }
  return C6RUN_SUCCESS;
}

// Exit routine to tell the GPP to shut down the CIO thread, then
// clean up the CIO IPC layer
void C6RUN_CIO_exit( void )
{
  if (!LOCAL_isModuleInitialized)
  {
    return;
  }
  else
  {
    CIO_Msg *CIO_msg = NULL;
      
    // Allocate a new message
    CIO_IPC_allocMsg(&CIO_msg);

    // Send CIO terminate message
    CIO_IPC_sendMsg(CIO_msg,CIO_TERMINATE);

    // Check for acknowledgement of terminate message
    if (CIO_IPC_recvMsg(&CIO_msg) != CIO_TERMINATE_RET)
    {
      C6RUN_LOG_error0("Transport error during CIO_TRANSFER");
      return;
    }
      
    // Free up this message now that we are done with it
    CIO_IPC_freeMsg(CIO_msg);
    
    // Clean-up the CIO IPC layer
    CIO_IPC_exit();
    
    LOCAL_isModuleInitialized = false;
  }
}


/************************************************************
* Local Function Definitions                                *
************************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
