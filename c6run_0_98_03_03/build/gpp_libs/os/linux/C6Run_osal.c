/*
 * C6Run_osal.c
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
#include <time.h>

/*  ----------------------------------- OS Specific Headers           */
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>

/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"

/*  ----------------------------------- This module's header          */
#include "C6Run_osal.h"


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


/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile bool C6RUN_OSAL_initialized = false;


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Loading DSP image from shared memory (uses CMEM heap allocated buffer)
// and initializing the DSPLink message queue interface
int32_t C6RUN_OSAL_create(void)
{
  if (C6RUN_OSAL_initialized)
  {
    return 0;
  }

  VERBOSE_PRINT0("Entered C6RUN_OSAL_create()\n");
  
  C6RUN_OSAL_initialized = true;
    
  VERBOSE_PRINT0("Leaving C6RUN_OSAL_create()\n");
  
  return 0;
}

// Teardown of DSPLink interface and shutting off of DSP.  This should
// usually only be called after the DSP code has terminated (exits main()).
void C6RUN_OSAL_delete(void)
{
  if (!C6RUN_OSAL_initialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_OSAL_delete()\n");
  
  C6RUN_OSAL_initialized = false;

  VERBOSE_PRINT0("Leaving C6RUN_OSAL_delete()\n");
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

