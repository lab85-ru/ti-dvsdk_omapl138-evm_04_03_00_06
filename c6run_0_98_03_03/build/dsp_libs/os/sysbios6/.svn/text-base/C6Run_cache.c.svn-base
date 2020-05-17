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

/************************************************************************
*  Memory allocation routines for C6Run buffers for remote procedures   *
************************************************************************/

/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/*  ----------------------------------- XDC Runtime Headers           */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/*  ----------------------------------- SysBios Headers               */
#include <ti/sysbios/family/c64p/Cache.h>

/*  ----------------------------------- Application Headers           */
#include "c6run.h"

/*  ----------------------------------- This module's header          */



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

void C6RUN_CACHE_globalInv()
{
  Cache_wbInvAll();
}

void C6RUN_CACHE_globalWb()
{
  Cache_wbAll();
}

void C6RUN_CACHE_globalWbInv()
{
  Cache_wbInvAll();
}

void C6RUN_CACHE_inv(void *ptr,size_t size)
{
  Cache_inv(ptr, size, Cache_Type_ALLD, true);
}

void C6RUN_CACHE_wb(void *ptr,size_t size)
{
  Cache_wb(ptr, size, Cache_Type_ALLD, true);
}

void C6RUN_CACHE_wbInv(void *ptr,size_t size)
{
  Cache_wbInv(ptr, size, Cache_Type_ALLD, true);
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
