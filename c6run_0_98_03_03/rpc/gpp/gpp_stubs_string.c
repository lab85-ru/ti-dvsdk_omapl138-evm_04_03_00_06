/*
 * gpp_stubs_string.c
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


#include <string.h>
#include "gpp_stubs_base.h"

/* ----- Stubs for functions contained in string.h ------ */

void rpc_memchr(void **param_buffer, void *result_buffer)
{
    void * _rpc_result_ = memchr(
      RPC_CAST_PARAM(param_buffer[0], void *),
      RPC_CAST_PARAM(param_buffer[1], int),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, void *, _rpc_result_);
}

void rpc_memcmp(void **param_buffer, void *result_buffer)
{
    int _rpc_result_ = memcmp(
      RPC_CAST_PARAM(param_buffer[0], void *),
      RPC_CAST_PARAM(param_buffer[1], void *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, int, _rpc_result_);
}

void rpc_memcpy(void **param_buffer, void *result_buffer)
{
    void * _rpc_result_ = memcpy(
      RPC_CAST_PARAM(param_buffer[0], void *),
      RPC_CAST_PARAM(param_buffer[1], void *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, void *, _rpc_result_);
}

void rpc_memmove(void **param_buffer, void *result_buffer)
{
    void * _rpc_result_ = memmove(
      RPC_CAST_PARAM(param_buffer[0], void *),
      RPC_CAST_PARAM(param_buffer[1], void *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, void *, _rpc_result_);
}

void rpc_memset(void **param_buffer, void *result_buffer)
{
    void * _rpc_result_ = memset(
      RPC_CAST_PARAM(param_buffer[0], void *),
      RPC_CAST_PARAM(param_buffer[1], int),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, void *, _rpc_result_);
}

void rpc_strcat(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strcat(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strchr(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strchr(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], int)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strcmp(void **param_buffer, void *result_buffer)
{
    int _rpc_result_ = strcmp(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, int, _rpc_result_);
}

void rpc_strcpy(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strcpy(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strcspn(void **param_buffer, void *result_buffer)
{
    unsigned int _rpc_result_ = strcspn(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, unsigned int, _rpc_result_);
}

void rpc_strlen(void **param_buffer, void *result_buffer)
{
    unsigned int _rpc_result_ = strlen(
      RPC_CAST_PARAM(param_buffer[0], char *)
    );
    RPC_LOAD_RESULT(result_buffer, unsigned int, _rpc_result_);
}

void rpc_strncat(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strncat(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strncmp(void **param_buffer, void *result_buffer)
{
    int _rpc_result_ = strncmp(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, int, _rpc_result_);
}

void rpc_strncpy(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strncpy(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strpbrk(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strpbrk(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strrchr(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strrchr(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], int)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strspn(void **param_buffer, void *result_buffer)
{
    unsigned int _rpc_result_ = strspn(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, unsigned int, _rpc_result_);
}

void rpc_strstr(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strstr(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strcoll(void **param_buffer, void *result_buffer)
{
    int _rpc_result_ = strcoll(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, int, _rpc_result_);
}

void rpc_strxfrm(void **param_buffer, void *result_buffer)
{
    unsigned int _rpc_result_ = strxfrm(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *),
      RPC_CAST_PARAM(param_buffer[2], unsigned int)
    );
    RPC_LOAD_RESULT(result_buffer, unsigned int, _rpc_result_);
}

void rpc_strtok(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strtok(
      RPC_CAST_PARAM(param_buffer[0], char *),
      RPC_CAST_PARAM(param_buffer[1], char *)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

void rpc_strerror(void **param_buffer, void *result_buffer)
{
    char * _rpc_result_ = strerror(
      RPC_CAST_PARAM(param_buffer[0], int)
    );
    RPC_LOAD_RESULT(result_buffer, char *, _rpc_result_);
}

