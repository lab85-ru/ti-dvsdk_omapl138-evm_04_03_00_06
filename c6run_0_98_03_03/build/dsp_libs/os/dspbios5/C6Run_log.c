/*
 * C6Run_log.c
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
#include <time.h>

/*  ----------------------------------- DSP/BIOS Headers              */
#include "C6Runcfg.h"
#include <std.h>
#include <log.h>

/*  ----------------------------------- Application Headers           */

/*  ----------------------------------- This module's header          */
#include "C6Run_log.h"


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

void C6RUN_LOG_error0 ( char *str )
{
  LOG_error ( str, 0);
}

void C6RUN_LOG_error1 ( char *str, int32_t arg0 )
{
  LOG_error ( str, (Arg) arg0);
}

void C6RUN_LOG_print0 ( char *str )
{
  LOG_printf4(&trace, str);
}

void C6RUN_LOG_print1 ( char *str, int32_t arg0 )
{
  LOG_printf4(&trace, str, (Arg) arg0);
}

void C6RUN_LOG_print2 ( char *str, int32_t arg0, int32_t arg1 )
{
  LOG_printf4(&trace, str, (Arg) arg0, (Arg) arg1);
}

void C6RUN_LOG_print3 ( char *str, int32_t arg0, int32_t arg1, int32_t arg2 )
{
  LOG_printf4(&trace, str, (Arg) arg0, (Arg) arg1, (Arg) arg2);
}

void C6RUN_LOG_print4 ( char *str, int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3 )
{
  LOG_printf4(&trace, str, (Arg) arg0, (Arg) arg1, (Arg) arg2, (Arg) arg3);
}


/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/
