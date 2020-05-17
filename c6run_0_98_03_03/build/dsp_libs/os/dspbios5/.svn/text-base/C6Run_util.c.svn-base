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

/*  ----------------------------------- DSP/BIOS Headers            */
#include <std.h>
#include <gbl.h>
#include <clk.h>

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

#define LOAD_WINDOW_IN_MS (500)


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/

static uint32_t minIdleFxnTime = 0xFFFFFFFF;
static uint32_t loadWindowTime = 0;
static int32_t  currLoad = 0;


/************************************************************
* Global Variable Definitions                               *
************************************************************/

volatile bool C6RUN_UTIL_initialized = false;


/************************************************************
* Global Function Definitions                               *
************************************************************/

int32_t C6RUN_UTIL_init( void )
{
  int32_t status = 0;

  if (C6RUN_UTIL_initialized)
  {
    return status;
  }
  
  VERBOSE_PRINT0("Entered C6RUN_UTIL_init()\n");

  loadWindowTime = CLK_countspms() * LOAD_WINDOW_IN_MS;
  
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
  return currLoad;
}

void C6RUN_UTIL_idleFxn( void )
{
  static uint32_t idleCount = 0;
  static uint32_t totalTime = 0;
  static uint32_t t0 = 0;
  uint32_t        t1 = 0;
  uint32_t        delta;

  // Keep track of times through this function
  idleCount++;                   

  // Compute the time difference since the last call this function
  t1 = CLK_gethtime();
  delta = t1 - t0;
  t0 = t1;
  
  // Accumulate totalIdleTime
  totalTime += delta;   

  /* 
   * 'minloop' keeps track of the shortest time thru the IDL loop.
   * Note that we check to make sure delta is not zero (delta
   * will be zero during IDL loop calibration)
   */ 
  if (delta != 0 && delta < minIdleFxnTime)
  {
    minIdleFxnTime = delta;
  }

  // Compute the CPU load as 100 - "percent time idle"
  // "percent time idle" = 100 * (idleCount * minIdleFxnTime)/totalTime
  if (totalTime > loadWindowTime) 
  {
    currLoad = (int32_t) (100 - ( (100 * idleCount * minIdleFxnTime) / totalTime));
    idleCount = 0;
    totalTime = 0;
  }
}

// Replacement clock function to use in place of RTS lib version
_CODE_ACCESS clock_t clock(void)   
{
  clock_t result;

  // Calculate absolute time in microseconds
  result = (clock_t) ((CLK_gethtime() *  CLK_cpuCyclesPerHtime() * 1000) / GBL_getFrequency());
  
  return result;
}



/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
