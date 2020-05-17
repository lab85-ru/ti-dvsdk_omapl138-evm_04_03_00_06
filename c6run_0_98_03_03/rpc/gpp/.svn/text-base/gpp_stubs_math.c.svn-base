/*
 * gpp_stubs_math.c
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


#include <math.h>
#include "gpp_stubs_base.h"

/* ----- Stubs for functions contained in math.h ------ */

void rpc_exp(void **param_buffer, void *result_buffer)
{
    double result = exp(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_log(void **param_buffer, void *result_buffer)
{
    double result = log(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_log10(void **param_buffer, void *result_buffer)
{
    double result = log10(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_pow(void **param_buffer, void *result_buffer)
{
    double result = pow(RPC_CAST_PARAM(param_buffer[0], double),
                        RPC_CAST_PARAM(param_buffer[1], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_sqrt(void **param_buffer, void *result_buffer)
{
    double result = sqrt(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_ceil(void **param_buffer, void *result_buffer)
{
    double result = ceil(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_floor(void **param_buffer, void *result_buffer)
{
    double result = floor(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_fabs(void **param_buffer, void *result_buffer)
{
    double result = fabs(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_ldexp(void **param_buffer, void *result_buffer)
{
    double result = ldexp(RPC_CAST_PARAM(param_buffer[0], double),
                          RPC_CAST_PARAM(param_buffer[1], int));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_frexp(void **param_buffer, void *result_buffer)
{
    double result = frexp(RPC_CAST_PARAM(param_buffer[0], double),
                          RPC_CAST_PARAM(param_buffer[1], int*));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_modf(void **param_buffer, void *result_buffer)
{
    double result = modf(RPC_CAST_PARAM(param_buffer[0], double),
                          RPC_CAST_PARAM(param_buffer[1], double*));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_fmod(void **param_buffer, void *result_buffer)
{
    double result = fmod(RPC_CAST_PARAM(param_buffer[0], double),
                         RPC_CAST_PARAM(param_buffer[1], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_sin(void **param_buffer, void *result_buffer)
{
    double result = sin(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_cos(void **param_buffer, void *result_buffer)
{
    double result = cos(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_tan(void **param_buffer, void *result_buffer)
{
    double result = tan(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_asin(void **param_buffer, void *result_buffer)
{
    double result = asin(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_acos(void **param_buffer, void *result_buffer)
{
    double result = acos(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_atan(void **param_buffer, void *result_buffer)
{
    double result = atan(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_atan2(void **param_buffer, void *result_buffer)
{
    double result = atan2(RPC_CAST_PARAM(param_buffer[0], double),
                          RPC_CAST_PARAM(param_buffer[1], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_sinh(void **param_buffer, void *result_buffer)
{
    double result = sinh(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_cosh(void **param_buffer, void *result_buffer)
{
    double result = cosh(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}

void rpc_tanh(void **param_buffer, void *result_buffer)
{
    double result = tanh(RPC_CAST_PARAM(param_buffer[0], double));
    RPC_LOAD_RESULT(result_buffer, double, result);
}
