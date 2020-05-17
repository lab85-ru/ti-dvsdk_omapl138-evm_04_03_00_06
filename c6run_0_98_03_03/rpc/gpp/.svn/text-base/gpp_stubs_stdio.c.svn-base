/*
 * gpp_stubs_stdio.c
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

// FIXME move these to common defines
#define MAX_STDIO_VARIADIC_PARAMS       10
#define MAX_STDIO_VARIADIC_PARAMSIZE    MAX_STDIO_VARIADIC_PARAMS * sizeof(double)
#define MAX_STDIO_VARIADIC_PLACEHOLDER  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f

/* ----- Stubs for functions contained in stdio.h ------ */

void rpc_fopen(void **param_buffer, void *result_buffer)
{
    FILE* result = fopen(RPC_CAST_PARAM(param_buffer[0], const char *),
                        RPC_CAST_PARAM(param_buffer[1], const char *)
                        );
    RPC_LOAD_RESULT(result_buffer, FILE*, result);
}

void rpc_freopen(void **param_buffer, void *result_buffer)
{
    FILE* result = freopen( RPC_CAST_PARAM(param_buffer[0], const char *),
                            RPC_CAST_PARAM(param_buffer[1], const char *),
                            RPC_CAST_PARAM(param_buffer[2], FILE *)
                            );
    RPC_LOAD_RESULT(result_buffer, FILE*, result);
}

void rpc_fflush(void **param_buffer, void *result_buffer)
{
    int result = fflush(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fclose(void **param_buffer, void *result_buffer)
{
    int result = fclose(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_remove(void **param_buffer, void *result_buffer)
{
    int result = remove(RPC_CAST_PARAM(param_buffer[0], const char *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_rename(void **param_buffer, void *result_buffer)
{
    int result = rename(RPC_CAST_PARAM(param_buffer[0], const char *),
                        RPC_CAST_PARAM(param_buffer[1], const char *)
                        );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_tmpfile(void **param_buffer, void *result_buffer)
{
    FILE* result = tmpfile();
    RPC_LOAD_RESULT(result_buffer, FILE*, result);
}

void rpc_tmpnam(void **param_buffer, void *result_buffer)
{
    char *result = tmpnam(RPC_CAST_PARAM(param_buffer[0], char*));
    RPC_LOAD_RESULT(result_buffer, char*, result);
}

void rpc_setbuf(void **param_buffer, void *result_buffer)
{
    setbuf(RPC_CAST_PARAM(param_buffer[0], FILE *),
           RPC_CAST_PARAM(param_buffer[1], char *)
          );
}

/* Begin variadic input/output functions */

/* fprintf------------------------------------------------------------------------ */

int rpc_fprintf_wrapper(FILE *stream, char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vfprintf(stream, fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_fprintf(void **param_buffer, void *result_buffer)
{
    FILE *stream = RPC_CAST_PARAM(param_buffer[0], FILE*);
    char *format_string = RPC_CAST_PARAM(param_buffer[1], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[2], char*);
    
    int result = rpc_fprintf_wrapper(stream, format_string, var_base,
                                     MAX_STDIO_VARIADIC_PLACEHOLDER
                                    );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

/* printf------------------------------------------------------------------------ */

int rpc_printf_wrapper(char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vprintf(fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_printf(void **param_buffer, void *result_buffer)
{
    char *format_string = RPC_CAST_PARAM(param_buffer[0], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[1], char*);
    
    int result = rpc_printf_wrapper(format_string, var_base,
                                    MAX_STDIO_VARIADIC_PLACEHOLDER
                                   );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

/* sprintf------------------------------------------------------------------------ */

int rpc_sprintf_wrapper(char *string, char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vsprintf(string, fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_sprintf(void **param_buffer, void *result_buffer)
{
    char *string = RPC_CAST_PARAM(param_buffer[0], char*);
    char *format_string = RPC_CAST_PARAM(param_buffer[1], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[2], char*);
    
    int result = rpc_sprintf_wrapper(string, format_string, var_base,
                                     MAX_STDIO_VARIADIC_PLACEHOLDER
                                    );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

/* fscanf ------------------------------------------------------------------------ */

int rpc_fscanf_wrapper(FILE *stream, char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vfscanf(stream, fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_fscanf(void **param_buffer, void *result_buffer)
{
    FILE *stream = RPC_CAST_PARAM(param_buffer[0], FILE*);
    char *format_string = RPC_CAST_PARAM(param_buffer[1], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[2], char*);
    
    int result = rpc_fscanf_wrapper(stream, format_string, var_base,
                                    MAX_STDIO_VARIADIC_PLACEHOLDER
                                   );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

/* scanf------------------------------------------------------------------------ */

int rpc_scanf_wrapper(char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vscanf(fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_scanf(void **param_buffer, void *result_buffer)
{
    char *format_string = RPC_CAST_PARAM(param_buffer[0], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[1], char*);
    
    int result = rpc_scanf_wrapper(format_string, var_base,
                                    MAX_STDIO_VARIADIC_PLACEHOLDER
                                   );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

/* sscanf------------------------------------------------------------------------ */

int rpc_sscanf_wrapper(char *string, char *fmt, char *vars, ...)
{
    int ret = 0;
    memcpy((void*)((unsigned int)(&vars) + sizeof(char*)), vars, MAX_STDIO_VARIADIC_PARAMSIZE);
    va_list ap;
    va_start(ap, vars);
    ret = vsscanf(string, fmt, ap);
    va_end(ap);
    return ret;
}

void rpc_sscanf(void **param_buffer, void *result_buffer)
{
    char *string = RPC_CAST_PARAM(param_buffer[0], char*);
    char *format_string = RPC_CAST_PARAM(param_buffer[1], char*);
    char *var_base = RPC_CAST_PARAM(param_buffer[2], char*);
    
    int result = rpc_sscanf_wrapper(string, format_string, var_base,
                                    MAX_STDIO_VARIADIC_PLACEHOLDER
                                   );

    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fgetc(void **param_buffer, void *result_buffer)
{
    int result = fgetc(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fgets(void **param_buffer, void *result_buffer)
{
    char* result = fgets(RPC_CAST_PARAM(param_buffer[0], char *),
                         RPC_CAST_PARAM(param_buffer[1], int),
                         RPC_CAST_PARAM(param_buffer[2], FILE *)
                        );
    RPC_LOAD_RESULT(result_buffer, char*, result);
}

void rpc_fputc(void **param_buffer, void *result_buffer)
{
    int result = fputc(RPC_CAST_PARAM(param_buffer[0], int),
                       RPC_CAST_PARAM(param_buffer[1], FILE *)
                      );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fputs(void **param_buffer, void *result_buffer)
{
    int result = fputs(RPC_CAST_PARAM(param_buffer[0], char *),
                         RPC_CAST_PARAM(param_buffer[1], FILE *)
                        );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_getc(void **param_buffer, void *result_buffer)
{
    int result = getc(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_getchar(void **param_buffer, void *result_buffer)
{
    int result = getchar();
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_gets(void **param_buffer, void *result_buffer)
{
    char* result = gets(RPC_CAST_PARAM(param_buffer[0], char *));
    RPC_LOAD_RESULT(result_buffer, char*, result);    
}

void rpc_putc(void **param_buffer, void *result_buffer)
{
    int result = putc(RPC_CAST_PARAM(param_buffer[0], int),
                       RPC_CAST_PARAM(param_buffer[1], FILE *)
                      );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_putchar(void **param_buffer, void *result_buffer)
{
    int result = putchar(RPC_CAST_PARAM(param_buffer[0], int));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_puts(void **param_buffer, void *result_buffer)
{
    int result = puts(RPC_CAST_PARAM(param_buffer[0], const char *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_ungetc(void **param_buffer, void *result_buffer)
{
    int result = ungetc(RPC_CAST_PARAM(param_buffer[0], int),
                        RPC_CAST_PARAM(param_buffer[1], FILE *)
                       );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fread(void **param_buffer, void *result_buffer)
{
    size_t result = fread(RPC_CAST_PARAM(param_buffer[0], void*),
                          RPC_CAST_PARAM(param_buffer[1], size_t),
                          RPC_CAST_PARAM(param_buffer[2], size_t),
                          RPC_CAST_PARAM(param_buffer[3], FILE*)
                         );
    RPC_LOAD_RESULT(result_buffer, size_t, result);
}

void rpc_fwrite(void **param_buffer, void *result_buffer)
{
    size_t result = fwrite(RPC_CAST_PARAM(param_buffer[0], void*),
                           RPC_CAST_PARAM(param_buffer[1], size_t),
                           RPC_CAST_PARAM(param_buffer[2], size_t),
                           RPC_CAST_PARAM(param_buffer[3], FILE*)
                          );
    RPC_LOAD_RESULT(result_buffer, size_t, result);
}

void rpc_fseek(void **param_buffer, void *result_buffer)
{
    int result = fseek(RPC_CAST_PARAM(param_buffer[0], FILE*),
                       RPC_CAST_PARAM(param_buffer[1], long),
                       RPC_CAST_PARAM(param_buffer[2], int)
                      );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_ftell(void **param_buffer, void *result_buffer)
{
    long result = ftell(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, long, result);
}

void rpc_rewind(void **param_buffer, void *result_buffer)
{
    rewind(RPC_CAST_PARAM(param_buffer[0], FILE *));
}

void rpc_fgetpos(void **param_buffer, void *result_buffer)
{
    int result = fgetpos(RPC_CAST_PARAM(param_buffer[0], FILE *),
                         RPC_CAST_PARAM(param_buffer[1], fpos_t *)
                        );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_fsetpos(void **param_buffer, void *result_buffer)
{
    int result = fsetpos(RPC_CAST_PARAM(param_buffer[0], FILE *),
                         RPC_CAST_PARAM(param_buffer[1], fpos_t *)
                        );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_clearerr(void **param_buffer, void *result_buffer)
{
    clearerr(RPC_CAST_PARAM(param_buffer[0], FILE *));
}

void rpc_feof(void **param_buffer, void *result_buffer)
{
    int result = feof(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_ferror(void **param_buffer, void *result_buffer)
{
    int result = ferror(RPC_CAST_PARAM(param_buffer[0], FILE *));
    RPC_LOAD_RESULT(result_buffer, int, result);
}

void rpc_perror(void **param_buffer, void *result_buffer)
{
    perror(RPC_CAST_PARAM(param_buffer[0], const char *));
}
