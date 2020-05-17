/*
 * gpp_stubs_time.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * Copyright (C) 2010 Yaman Umuroglu 
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


#include <time.h>
#include "gpp_stubs_base.h"

/* ----- Stubs for functions contained in time.h ------ */

void rpc_clock(void **param_buffer, void *result_buffer)
{
    clock_t result = clock();
    RPC_LOAD_RESULT(result_buffer, clock_t, result);
}

void rpc_time(void **param_buffer, void *result_buffer)
{
    time_t result = time(RPC_CAST_PARAM(param_buffer[0], time_t*));
    RPC_LOAD_RESULT(result_buffer, time_t, result);
}

void rpc_difftime(void **param_buffer, void *result_buffer)
{
    double result = difftime(RPC_CAST_PARAM(param_buffer[0], time_t), RPC_CAST_PARAM(param_buffer[1], time_t));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_mktime(void **param_buffer, void *result_buffer)
{
    time_t result = mktime(RPC_CAST_PARAM(param_buffer[0], struct tm*));
    RPC_LOAD_RESULT(result_buffer, time_t, result);
}

void rpc_asctime(void **param_buffer, void *result_buffer)
{
    char* result = asctime(RPC_CAST_PARAM(param_buffer[0], struct tm*));
    RPC_LOAD_RESULT(result_buffer, char*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, strlen(result)+1);
}

void rpc_ctime(void **param_buffer, void *result_buffer)
{
    char* result = ctime(RPC_CAST_PARAM(param_buffer[0], time_t*));
    RPC_LOAD_RESULT(result_buffer, char*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, strlen(result)+1);
}

void rpc_gmtime(void **param_buffer, void *result_buffer)
{
    struct tm* result = gmtime(RPC_CAST_PARAM(param_buffer[0], const time_t*));
    RPC_LOAD_RESULT(result_buffer, struct tm*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, sizeof(struct tm*));
}

void rpc_localtime(void **param_buffer, void *result_buffer)
{
    struct tm* result = localtime(RPC_CAST_PARAM(param_buffer[0], const time_t*));
    RPC_LOAD_RESULT(result_buffer, struct tm*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, sizeof(struct tm*));
}

void rpc_strftime(void **param_buffer, void *result_buffer)
{
    size_t result = strftime(RPC_CAST_PARAM(param_buffer[0], char*),
                             RPC_CAST_PARAM(param_buffer[1], size_t),
                             RPC_CAST_PARAM(param_buffer[2], const char*),
                             RPC_CAST_PARAM(param_buffer[3], const struct tm*)
                            );
    RPC_LOAD_RESULT(result_buffer, size_t, result);
}
