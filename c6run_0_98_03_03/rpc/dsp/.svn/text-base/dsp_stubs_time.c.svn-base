/*
 * dsp_stubs_time.c
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
#include "dsp_stubs_base.h"

/* time.h function stubs ------------------------------------- */

// assuming time_t and clock_t defined as unsigned int

clock_t rpc_clock(void)
{
    RPC_INIT("rpc_clock", "iv");
    RPC_PERFORM();
    return RPC_GETRESULT(clock_t);
}

time_t rpc_time(time_t* tp)
{
    RPC_INIT("rpc_time", "i@");
    RPC_BUFFER(time_t*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(time_t);
}

double rpc_difftime(time_t time2, time_t time1)
{
    
    RPC_INIT("rpc_difftime", "dii");
    RPC_PARAM(time_t, &time2);
    RPC_PARAM(time_t, &time1);
    RPC_PERFORM();
    return RPC_GETRESULT(double);
}

time_t rpc_mktime(struct tm* tp)
{
    RPC_INIT("rpc_mktime", "i@");
    RPC_BUFFER(struct tm*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(time_t);
}

char* rpc_asctime(const struct tm* tp)
{
    RPC_INIT("rpc_asctime", "$@");
    RPC_BUFFER(struct tm*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

char* rpc_ctime(const time_t* tp)
{
    RPC_INIT("rpc_ctime", "$@");
    RPC_BUFFER(time_t*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

struct tm* rpc_gmtime(const time_t* tp)
{
    RPC_INIT("rpc_asctime", "a@");
    RPC_BUFFER(time_t*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(struct tm*);
}

struct tm* rpc_localtime(const time_t* tp)
{
    RPC_INIT("rpc_localtime", "a@");
    RPC_BUFFER(time_t*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(struct tm*);
}

size_t rpc_strftime(char* s, size_t smax, const char* fmt, const struct tm* tp)
{
    RPC_INIT("rpc_strftime", "i@i@@");
    RPC_BUFFER(char*, &s);
    RPC_PARAM(size_t, &smax);
    RPC_BUFFER(char*, &fmt);
    RPC_BUFFER(char*, &fmt);
    RPC_BUFFER(struct tm*, &tp);
    RPC_PERFORM();
    return RPC_GETRESULT(size_t);
}
