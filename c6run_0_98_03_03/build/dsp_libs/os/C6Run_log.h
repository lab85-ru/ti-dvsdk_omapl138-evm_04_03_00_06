/*
 * C6Run_log.h
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

#ifndef _C6RUN_LOG_H_
#define _C6RUN_LOG_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
* Global Macro Declarations                                *
***********************************************************/ 


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

// C6Run logging routines
extern void C6RUN_LOG_error0 ( char *str );
extern void C6RUN_LOG_error1 ( char *str, int32_t arg0 );
extern void C6RUN_LOG_print0 ( char *str );
extern void C6RUN_LOG_print1 ( char *str, int32_t arg0 );
extern void C6RUN_LOG_print2 ( char *str, int32_t arg0, int32_t arg1 );
extern void C6RUN_LOG_print3 ( char *str, int32_t arg0, int32_t arg1, int32_t arg2 );
extern void C6RUN_LOG_print4 ( char *str, int32_t arg0, int32_t arg1, int32_t arg2, int32_t arg3 );



/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_C6RUN_LOG_H_