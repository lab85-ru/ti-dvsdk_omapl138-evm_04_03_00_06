/*
 * C6Run_cache.c
 */

/*
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*  ----------------------------------- OS Specific Headers           */

/*  ----------------------------------- CMEM API                      */
#include <ti/sdo/linuxutils/cmem/include/cmem.h>

/*  ----------------------------------- Application headers           */
#include "c6run.h"
#include "debug_common.h"
#include "C6Run_common.h"
#include "C6Run_sharedmem.h"


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


/************************************************************
* Global Function Definitions                               *
************************************************************/

// Cache management routines
void C6RUN_CACHE_globalInv()
{
  CMEM_cacheInv(C6RUN_SHAREDMEM_localBaseAddr, DSP_REGION_CMEM_SIZE+DSP_REGION_CODE_SIZE);
}

void C6RUN_CACHE_globalWb()
{
  CMEM_cacheWb(C6RUN_SHAREDMEM_localBaseAddr, DSP_REGION_CMEM_SIZE+DSP_REGION_CODE_SIZE);
}

void C6RUN_CACHE_globalWbInv()
{
  CMEM_cacheWbInv(C6RUN_SHAREDMEM_localBaseAddr, DSP_REGION_CMEM_SIZE+DSP_REGION_CODE_SIZE);
}

void C6RUN_CACHE_inv(void *ptr,size_t size)
{
  CMEM_cacheInv(ptr, size);
}

void C6RUN_CACHE_wb(void *ptr,size_t size)
{
  CMEM_cacheWb(ptr, size);
}

void C6RUN_CACHE_wbInv(void *ptr,size_t size)
{
  CMEM_cacheWbInv(ptr, size);
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
