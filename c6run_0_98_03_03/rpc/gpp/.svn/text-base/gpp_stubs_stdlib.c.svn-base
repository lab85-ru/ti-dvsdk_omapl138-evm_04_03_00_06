/*
 * gpp_stubs_stdlib.c
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


#include <stdlib.h>
#include "gpp_stubs_base.h"

/* ----- Stubs for functions contained in stdlib.h ------ */

void rpc_abs(void **param_buffer, void *result_buffer)
{
    int result = abs(RPC_CAST_PARAM(param_buffer[0], int));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_labs(void **param_buffer, void *result_buffer)
{
    int result = labs(RPC_CAST_PARAM(param_buffer[0], long));
    RPC_LOAD_RESULT(result_buffer, long, result);
}

void rpc_div(void **param_buffer, void *result_buffer)
{
    div_t *result = (div_t*) malloc(sizeof(div_t));
    *result = div(RPC_CAST_PARAM(param_buffer[0], int),RPC_CAST_PARAM(param_buffer[1], int));
    RPC_LOAD_RESULT(result_buffer, div_t*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, sizeof(div_t));
}

void rpc_ldiv(void **param_buffer, void *result_buffer)
{
    ldiv_t *result = (ldiv_t*) malloc(sizeof(div_t));
    *result = ldiv(RPC_CAST_PARAM(param_buffer[0], long),RPC_CAST_PARAM(param_buffer[1], long));
    RPC_LOAD_RESULT(result_buffer, ldiv_t*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, sizeof(ldiv_t));
}


void rpc_atof(void **param_buffer, void *result_buffer)
{
    double result = atof(RPC_CAST_PARAM(param_buffer[0], const char*));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_atoi(void **param_buffer, void *result_buffer)
{
    int result = atof(RPC_CAST_PARAM(param_buffer[0], const char*));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_atol(void **param_buffer, void *result_buffer)
{
    long result = atol(RPC_CAST_PARAM(param_buffer[0], const char*));
    RPC_LOAD_RESULT(result_buffer, long, result);
}

void rpc_strtod(void **param_buffer, void *result_buffer)
{
    double result = strtod(RPC_CAST_PARAM(param_buffer[0], const char*),RPC_CAST_PARAM(param_buffer[1], char**));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_strtol(void **param_buffer, void *result_buffer)
{
    long result =   strtol(RPC_CAST_PARAM(param_buffer[0], const char*),
                           RPC_CAST_PARAM(param_buffer[1], char**),
                           RPC_CAST_PARAM(param_buffer[2], int));
    RPC_LOAD_RESULT(result_buffer, long, result);
}

void rpc_strtoul(void **param_buffer, void *result_buffer)
{
    unsigned long result = strtoul(RPC_CAST_PARAM(param_buffer[0], const char*),
                                  RPC_CAST_PARAM(param_buffer[1], char**),
                                  RPC_CAST_PARAM(param_buffer[2], int));
    RPC_LOAD_RESULT(result_buffer, unsigned long, result);
}

void rpc_system(void **param_buffer, void *result_buffer)
{
    int result = system(RPC_CAST_PARAM(param_buffer[0], const char*));
    RPC_LOAD_RESULT(result_buffer, int, result);
}


void rpc_getenv(void **param_buffer, void *result_buffer)
{
    char* result = getenv(RPC_CAST_PARAM(param_buffer[0], const char*));
    RPC_LOAD_RESULT(result_buffer, char*, result);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, strlen(result)+1);
}

void rpc_rand(void **param_buffer, void *result_buffer)
{
    int result = rand();
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_srand(void **param_buffer, void *result_buffer)
{
    srand(RPC_CAST_PARAM(param_buffer[0], unsigned int));
}
