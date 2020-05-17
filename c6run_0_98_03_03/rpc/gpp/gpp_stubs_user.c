/*
 * gpp_stubs_user.c
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
#include <stdarg.h>
#include "gpp_stubs_base.h"

/* ----- Stubs & functions for RPC testing ------ */

void mixedprint(int a, char b, float c, double d, short e, int f, char *g)
{
    printf("GPP function: mixedprint(%d, %c, %f, %f, %d, %d, %x);\n", a, b, c, d, e, f, g);
    printf("GPP function: string content was: %s \n", g);
    strcpy(g,"012345679");
    printf("GPP function: string content is now modified to: %s \n", g);
}

void rpc_mixedprint(void **param_buffer, void *result_buffer)
{
    mixedprint(RPC_CAST_PARAM(param_buffer[0], int),
               RPC_CAST_PARAM(param_buffer[1], char),
               RPC_CAST_PARAM(param_buffer[2], float),
               RPC_CAST_PARAM(param_buffer[3], double),
               RPC_CAST_PARAM(param_buffer[4], short),
               RPC_CAST_PARAM(param_buffer[5], int),
               RPC_CAST_PARAM(param_buffer[6], char*)
              );
}

char* statalloc_test()
{
    static char s[] = "hello from rpc_statalloc_test";
    return s;
}

void rpc_statalloc_test(void **param_buffer, void *result_buffer)
{
    char *res = statalloc_test();                              
    RPC_LOAD_RESULT(result_buffer, char*, res);
    RPC_LOAD_RESULT_BUFLEN(result_buffer, strlen(res)+1);
}


