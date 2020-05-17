/*
 * dsp_stubs_user.c
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


#include <stdio.h>
#include "dsp_stubs_base.h"

extern void* rpc_malloc(size_t size);
extern void  rpc_free(void *ptr);


/* RPC testing stubs ------------------------------------------*/

void rpc_mixedprint(int a, char b, float c, double d, short e, int f, char *g)
{
    printf("DSP stub: rpc_mixedprint(%d, %c, %f, %f, %d, %d, %x);\n", a, b, c, d, e, f, g);
    printf("DSP stub: string content in g is %s \n",g);
    RPC_INIT("rpc_mixedprint", "vicfdsi@");
    RPC_PARAM(int, &a);
    RPC_PARAM(char, &b);
    RPC_PARAM(float, &c);
    RPC_PARAM(double, &d);
    RPC_PARAM(short, &e);
    RPC_PARAM(int, &f);
    RPC_BUFFER(char*, &g);
    RPC_PERFORM();
}

char* rpc_statalloc_test()
{
    RPC_INIT("rpc_statalloc_test", "$");
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}












