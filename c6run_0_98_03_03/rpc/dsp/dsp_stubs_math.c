/*
 * dsp_stubs_math.c
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
#include "dsp_stubs_base.h"

/* math.h function stubs ------------------------------------- */

double rpc_exp(double x)
{
    RPC_INIT("rpc_exp", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_log(double x)
{
    RPC_INIT("rpc_log", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_log10(double x)
{
    RPC_INIT("rpc_log10", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_pow(double x, double y)
{
    RPC_INIT("rpc_pow", "ddd");
    RPC_PARAM(double, &x);
    RPC_PARAM(double, &y);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_sqrt(double x)
{
    RPC_INIT("rpc_sqrt", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_ceil(double x)
{
    RPC_INIT("rpc_ceil", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_floor(double x)
{
    RPC_INIT("rpc_floor", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_fabs(double x)
{
    RPC_INIT("rpc_fabs", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_ldexp(double x, int n)
{
    RPC_INIT("rpc_ldexp", "ddi");
    RPC_PARAM(double, &x);
    RPC_PARAM(int, &n);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_frexp(double x, int* exp)
{
    RPC_INIT("rpc_frexp", "dd@");
    RPC_PARAM(double, &x);
    RPC_BUFFER(int*, &exp);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_modf(double x, double* ip)
{
    RPC_INIT("rpc_modf", "dd@");
    RPC_PARAM(double, &x);
    RPC_BUFFER(double*, &ip);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_fmod(double x, double y)
{
    RPC_INIT("rpc_fmod", "ddd");
    RPC_PARAM(double, &x);
    RPC_PARAM(double, &y);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_sin(double x)
{
    RPC_INIT("rpc_sin", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_cos(double x)
{
    RPC_INIT("rpc_cos", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_tan(double x)
{
    RPC_INIT("rpc_tan", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_asin(double x)
{
    RPC_INIT("rpc_asin", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_acos(double x)
{
    RPC_INIT("rpc_acos", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_atan(double x)
{
    RPC_INIT("rpc_atan", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_atan2(double y, double x)
{
    RPC_INIT("rpc_atan2", "ddd");
    RPC_PARAM(double, &y);
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_sinh(double x)
{
    RPC_INIT("rpc_sinh", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_cosh(double x)
{
    RPC_INIT("rpc_cosh", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

double rpc_tanh(double x)
{
    RPC_INIT("rpc_tanh", "dd");
    RPC_PARAM(double, &x);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

