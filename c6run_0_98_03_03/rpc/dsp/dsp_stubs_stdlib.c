/*
 * dsp_stubs_stdlib.c
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
#include "dsp_stubs_base.h"

/* stdlib.h function stubs ----------------------------------- */

int rpc_abs(int n)
{
    RPC_INIT("rpc_abs", "ii");
    RPC_PARAM(int, &n);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

long rpc_labs(long n)
{
    RPC_INIT("rpc_labs", "ii");
    RPC_PARAM(long, &n);
    RPC_PERFORM();
    return RPC_GETRESULT(long);
}

div_t rpc_div(int num, int denom)
{
    RPC_INIT("rpc_div", "#ii");
    RPC_PARAM(int, &num);
    RPC_PARAM(int, &denom);
    RPC_PERFORM();
    return RPC_GETRESULT(div_t);
}

ldiv_t rpc_ldiv(long num, long denom)
{
    RPC_INIT("rpc_ldiv", "#ii");
    RPC_PARAM(long, &num);
    RPC_PARAM(long, &denom);
    RPC_PERFORM();
    return RPC_GETRESULT(ldiv_t);
}

double rpc_atof(const char* s)
{
    RPC_INIT("rpc_atof", "d@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

int rpc_atoi(const char* s)
{
    RPC_INIT("rpc_atoi", "i@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

long rpc_atol(const char* s)
{
    RPC_INIT("rpc_atol", "i@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(long);
}

double rpc_strtod(const char* s, char** endp)
{
    RPC_INIT("rpc_strtod", "d@@");
    RPC_BUFFER(char*, &s);
    RPC_BUFFER(char*, &endp);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

long rpc_strtol(const char* s, char** endp, int base)
{
    RPC_INIT("rpc_strtol", "i@@i");
    RPC_BUFFER(char*, &s);
    RPC_BUFFER(char*, &endp);
    RPC_PARAM(int, &base);
    RPC_PERFORM();
    return RPC_GETRESULT(long);
}

unsigned long rpc_strtoul(const char* s, char** endp, int base)
{
    RPC_INIT("rpc_strtoul", "i@@i");
    RPC_BUFFER(char*, &s);
    RPC_BUFFER(char*, &endp);
    RPC_PARAM(int, &base);
    RPC_PERFORM();
    return RPC_GETRESULT(unsigned long);
}

int rpc_system(const char* s)
{
    RPC_INIT("rpc_system", "i@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

char* rpc_getenv(const char* name)
{
    RPC_INIT("rpc_getenv", "$@");
    RPC_BUFFER(char*, &name);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

int rpc_rand(void)
{
    RPC_INIT("rpc_rand", "iv");
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

void rpc_srand(unsigned int seed)
{
    RPC_INIT("rpc_srand", "vi");
    RPC_PARAM(unsigned int, &seed);
    RPC_PERFORM();
}
