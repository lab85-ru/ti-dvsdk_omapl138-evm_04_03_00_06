/*
 * C6Run_util.c
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
*  System utility routines for C6Run                                    *
************************************************************************/

/*  ----------------------------------- Headers for User Code       */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*  ----------------------------------- XDC Runtime Headers         */
#include <xdc/std.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>

/*  ----------------------------------- SysBios Headers             */
#include <ti/sysbios/utils/Load.h>

/*  ----------------------------------- Application Headers         */
#include "debug_common.h"

/*  ----------------------------------- This module's header        */
#include "C6Run_util.h"


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

volatile bool C6RUN_UTIL_initialized = false;

uint64_t C6RUN_UTIL_timestampFreqInMHz = 0;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t C6RUN_UTIL_init( void )
{
  int32_t status = 0;
  Types_FreqHz freq;

  if (C6RUN_UTIL_initialized)
  {
    return status;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_UTIL_init()\n");

  Load_reset();
  
  Timestamp_getFreq(&freq);
  
  C6RUN_UTIL_timestampFreqInMHz = ((((uint64_t)freq.hi) << 32) + freq.lo) / 1000000;
  
  C6RUN_UTIL_initialized = true;
  
  VERBOSE_PRINT0("Leaving C6RUN_UTIL_init()\n");
  
  return status;
}

void C6RUN_UTIL_exit ( void )
{
  if (!C6RUN_UTIL_initialized)
  {
    return;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_UTIL_exit()\n");
  
  VERBOSE_PRINT0("Leaving C6RUN_UTIL_exit()\n");
}

int32_t C6RUN_UTIL_getLoad( void )
{
  return (int32_t) Load_getCPULoad();
}

// Replacement clock function to use in place of RTS lib version
_CODE_ACCESS clock_t clock(void)   
{
  clock_t result;
  Types_Timestamp64 tsResult;
  uint64_t timestamp;
  
  Timestamp_get64(&tsResult);
  
  timestamp = (((uint64_t)tsResult.hi) << 32) + tsResult.lo;

  // Calculate absolute time in microseconds
  result = (clock_t) (timestamp / C6RUN_UTIL_timestampFreqInMHz);
  
  return result;
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
