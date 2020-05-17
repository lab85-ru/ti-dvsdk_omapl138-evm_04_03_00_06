/*
 * dsp_stubs_string.c
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
#include "dsp_stubs_base.h"

/* string.h function stubs ----------------------------------- */

void * rpc_memchr( void * _cs, int _c, unsigned int _n )
{
    RPC_INIT("rpc_memchr", "@@ii");
    RPC_BUFFER(void *, &_cs);
    RPC_PARAM(int, &_c);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(void *);
}

int rpc_memcmp( void * _cs, void * _ct, unsigned int _n )
{
    RPC_INIT("rpc_memcmp", "i@@i");
    RPC_BUFFER(void *, &_cs);
    RPC_BUFFER(void *, &_ct);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

void * rpc_memcpy( void * _s1, void * _s2, unsigned int _n )
{
    RPC_INIT("rpc_memcpy", "@@@i");
    RPC_BUFFER(void *, &_s1);
    RPC_BUFFER(void *, &_s2);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(void *);
}

void * rpc_memmove( void * _s1, void * _s2, unsigned int _n )
{
    RPC_INIT("rpc_memmove", "@@@i");
    RPC_BUFFER(void *, &_s1);
    RPC_BUFFER(void *, &_s2);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(void *);
}

void * rpc_memset( void * _mem, int _ch, unsigned int _n )
{
    RPC_INIT("rpc_memset", "@@ii");
    RPC_BUFFER(void *, &_mem);
    RPC_PARAM(int, &_ch);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(void *);
}

char * rpc_strcat( char * _string1, char * _string2 )
{
    RPC_INIT("rpc_strcat", "@@@");
    RPC_BUFFER(char *, &_string1);
    RPC_BUFFER(char *, &_string2);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

char * rpc_strchr( char * _string, int _c )
{
    RPC_INIT("rpc_strchr", "@@i");
    RPC_BUFFER(char *, &_string);
    RPC_PARAM(int, &_c);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

int rpc_strcmp( char * _string1, char * _string2 )
{
    RPC_INIT("rpc_strcmp", "i@@");
    RPC_BUFFER(char *, &_string1);
    RPC_BUFFER(char *, &_string2);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

char * rpc_strcpy( char * _dest, char * _src )
{
    RPC_INIT("rpc_strcpy", "@@@");
    RPC_BUFFER(char *, &_dest);
    RPC_BUFFER(char *, &_src);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

unsigned int rpc_strcspn( char * _string, char * _chs )
{
    RPC_INIT("rpc_strcspn", "i@@");
    RPC_BUFFER(char *, &_string);
    RPC_BUFFER(char *, &_chs);
    RPC_PERFORM();
    return RPC_GETRESULT(unsigned int);
}

unsigned int rpc_strlen( char * _string )
{
    RPC_INIT("rpc_strlen", "i@");
    RPC_BUFFER(char *, &_string);
    RPC_PERFORM();
    return RPC_GETRESULT(unsigned int);
}

char * rpc_strncat( char * _to, char * _from, unsigned int _n )
{
    RPC_INIT("rpc_strncat", "@@@i");
    RPC_BUFFER(char *, &_to);
    RPC_BUFFER(char *, &_from);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

int rpc_strncmp( char * _string1, char * _string2, unsigned int _n )
{
    RPC_INIT("rpc_strncmp", "i@@i");
    RPC_BUFFER(char *, &_string1);
    RPC_BUFFER(char *, &_string2);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

char * rpc_strncpy( char * _to, char * _from, unsigned int _n )
{
    RPC_INIT("rpc_strncpy", "@@@i");
    RPC_BUFFER(char *, &_to);
    RPC_BUFFER(char *, &_from);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

char * rpc_strpbrk( char * _string, char * _chs )
{
    RPC_INIT("rpc_strpbrk", "@@@");
    RPC_BUFFER(char *, &_string);
    RPC_BUFFER(char *, &_chs);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

char * rpc_strrchr( char * _string, int _c )
{
    RPC_INIT("rpc_strrchr", "@@i");
    RPC_BUFFER(char *, &_string);
    RPC_PARAM(int, &_c);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

unsigned int rpc_strspn( char * _string, char * _chs )
{
    RPC_INIT("rpc_strspn", "i@@");
    RPC_BUFFER(char *, &_string);
    RPC_BUFFER(char *, &_chs);
    RPC_PERFORM();
    return RPC_GETRESULT(unsigned int);
}

char * rpc_strstr( char * _string1, char * _string2 )
{
    RPC_INIT("rpc_strstr", "@@@");
    RPC_BUFFER(char *, &_string1);
    RPC_BUFFER(char *, &_string2);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

int rpc_strcoll( char * _string1, char * _string2 )
{
    RPC_INIT("rpc_strcoll", "i@@");
    RPC_BUFFER(char *, &_string1);
    RPC_BUFFER(char *, &_string2);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

unsigned int rpc_strxfrm( char * _to, char * _from, unsigned int _n )
{
    RPC_INIT("rpc_strxfrm", "i@@i");
    RPC_BUFFER(char *, &_to);
    RPC_BUFFER(char *, &_from);
    RPC_PARAM(int, &_n);
    RPC_PERFORM();
    return RPC_GETRESULT(unsigned int);
}

char * rpc_strtok( char * _str1, char * _str2 )
{
    RPC_INIT("rpc_strtok", "@@@");
    RPC_BUFFER(char *, &_str1);
    RPC_BUFFER(char *, &_str2);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}

char * rpc_strerror( int _errno )
{
    RPC_INIT("rpc_strerror", "@i");
    RPC_PARAM(int, &_errno);
    RPC_PERFORM();
    return RPC_GETRESULT(char *);
}


