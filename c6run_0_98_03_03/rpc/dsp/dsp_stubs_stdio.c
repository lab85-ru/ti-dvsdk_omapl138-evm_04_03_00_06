/*
 * dsp_stubs_stdio.c
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

/* stdio.h function stubs -------------------------------------*/

#define MAX_STDIO_VARIADIC_PARAMS       10
#define MAX_STDIO_VARIADIC_PARAMSIZE    MAX_STDIO_VARIADIC_PARAMS * sizeof(double)

FILE* rpc_fopen(const char* filename, const char* mode)
{
    RPC_INIT("rpc_fopen", "a@@");
    RPC_BUFFER(const char*, &filename);
    RPC_BUFFER(const char*, &mode);
    RPC_PERFORM();
    return RPC_GETRESULT(FILE*);
}

FILE* rpc_freopen(const char* filename, const char* mode, FILE* stream)
{
    RPC_INIT("rpc_freopen", "a@@a");
    RPC_BUFFER(const char*, &filename);
    RPC_BUFFER(const char*, &mode);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(FILE*);
}

int rpc_fflush(FILE* stream)
{
    RPC_INIT("rpc_fflush", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_fclose(FILE* stream)
{
    RPC_INIT("rpc_fclose", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_remove(const char* filename)
{
    RPC_INIT("rpc_remove", "i@");
    RPC_BUFFER(const char*, &filename);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_rename(const char* oldname, const char* newname)
{
    RPC_INIT("rpc_rename", "i@@");
    RPC_BUFFER(const char*, &oldname);
    RPC_BUFFER(const char*, &newname);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

FILE* rpc_tmpfile()
{
    RPC_INIT("rpc_tmpfile", "iv");
    RPC_PERFORM();
    return RPC_GETRESULT(FILE*);
}

char* rpc_tmpnam(char s[L_tmpnam])
{
    RPC_INIT("rpc_tmpnam", "@@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

void rpc_setbuf(FILE* stream, char* buf)
{
    RPC_INIT("rpc_setbuf", "va@");
    RPC_BUFFER(FILE*, &stream);
    RPC_BUFFER(char*, &buf);
    RPC_PERFORM();
}

int rpc_fprintf(FILE* stream, const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_fprintf", "ia@>");
    RPC_BUFFER(FILE*, &stream);
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}

int rpc_printf(const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_printf", "i@>");
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}


int rpc_sprintf(char* s, const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_sprintf", "i@@>");
    RPC_BUFFER(char*, &s);
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}

//int rpc_vfprintf(FILE* stream, const char* format, va_list arg);
//int rpc_vprintf(const char* format, va_list arg);
//int rpc_vsprintf(char* s, const char* format, va_list arg);

int rpc_fscanf(FILE* stream, const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_fscanf", "ia@<");
    RPC_BUFFER(FILE*, &stream);
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}

int rpc_scanf(const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_scanf", "i@<");
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}

int rpc_sscanf(char* s, const char* format, ...)
{
    char *var_buf = (char*) rpc_malloc(MAX_STDIO_VARIADIC_PARAMSIZE);
    memcpy(var_buf, (void*)((unsigned int)&format + sizeof(char*)), MAX_STDIO_VARIADIC_PARAMSIZE);
    RPC_INIT("rpc_sscanf", "i@@<");
    RPC_BUFFER(char*, &s);
    RPC_BUFFER(char*, &format);
    RPC_BUFFER(char*, &var_buf);
    RPC_PERFORM();
    rpc_free(var_buf);
    return RPC_GETRESULT(int);
}

int rpc_fgetc(FILE* stream)
{
    RPC_INIT("rpc_fgets", "i@");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

char* rpc_fgets(char* s, int n, FILE* stream)
{
    RPC_INIT("rpc_fgets", "@@ia");
    RPC_BUFFER(char*, &s);
    RPC_PARAM(int, &n);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

int rpc_fputc(int c, FILE* stream)
{
    RPC_INIT("rpc_fputc", "iia");
    RPC_PARAM(int, &c);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_fputs(const char* s, FILE* stream)
{
    RPC_INIT("rpc_fputs", "i@a");
    RPC_BUFFER(const char*, &s);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_getc(FILE* stream)
{
    RPC_INIT("rpc_getc", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_getchar(void)
{
    RPC_INIT("rpc_getchar", "iv");
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

char* rpc_gets(char* s)
{
    RPC_INIT("rpc_gets", "@@");
    RPC_BUFFER(char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(char*);
}

int rpc_putc(int c, FILE* stream)
{
    RPC_INIT("rpc_putc", "iia");
    RPC_PARAM(int, &c);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_putchar(int c)
{
    RPC_INIT("rpc_putchar", "ii");
    RPC_PARAM(int, &c);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_puts(const char* s)
{
    RPC_INIT("rpc_puts", "i@");
    RPC_BUFFER(const char*, &s);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_ungetc(int c, FILE* stream)
{
    RPC_INIT("rpc_ungetc", "iia");
    RPC_PARAM(int, &c);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

size_t rpc_fread(void* ptr, size_t size, size_t nobj, FILE* stream)
{
    RPC_INIT("rpc_fread", "i@iia");
    RPC_BUFFER(void*, &ptr);
    RPC_PARAM(int, &size);
    RPC_PARAM(int, &nobj);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(size_t);
}

size_t rpc_fwrite(const void* ptr, size_t size, size_t nobj, FILE* stream)
{
    RPC_INIT("rpc_fwrite", "i@iia");
    RPC_BUFFER(void*, &ptr);
    RPC_PARAM(int, &size);
    RPC_PARAM(int, &nobj);
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(size_t);
}

int rpc_fseek(FILE* stream, long offset, int origin)
{
    RPC_INIT("rpc_fseek", "iaii");
    RPC_BUFFER(FILE*, &stream);
    RPC_PARAM(long, &offset);
    RPC_PARAM(int, &origin);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

long rpc_ftell(FILE* stream)
{
    RPC_INIT("rpc_ftell", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(long);
}

void rpc_rewind(FILE* stream)
{
    RPC_INIT("rpc_rewind", "va");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
}

int rpc_fgetpos(FILE* stream, fpos_t* ptr)
{
    RPC_INIT("rpc_fgetpos", "ia@");
    RPC_BUFFER(FILE*, &stream);
    RPC_BUFFER(fpos_t*, &ptr);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_fsetpos(FILE* stream, const fpos_t* ptr)
{
    RPC_INIT("rpc_fsetpos", "ia@");
    RPC_BUFFER(FILE*, &stream);
    RPC_BUFFER(const fpos_t*, &ptr);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

void rpc_clearerr(FILE* stream)
{
    RPC_INIT("rpc_clearerr", "va");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
}

int rpc_feof(FILE* stream)
{
    RPC_INIT("rpc_feof", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_ferror(FILE* stream)
{
    RPC_INIT("rpc_ferror", "ia");
    RPC_BUFFER(FILE*, &stream);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

void rpc_perror(const char* s)
{
    RPC_INIT("rpc_perror", "i@");
    RPC_BUFFER(const char*, &s);
    RPC_PERFORM();
}
