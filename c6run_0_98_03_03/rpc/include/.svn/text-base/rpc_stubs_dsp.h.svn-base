/*
 * rpc_stubs_dsp.h
 * Header file containing declarations for DSP-side stubs
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

#ifndef _RPC_STUBS_DSP_H_
#define _RPC_STUBS_DSP_H_

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/* RPC memory related stubs -------------------------------*/

#define RPC_PHYS_TO_VIRT   (1)
#define RPC_VIRT_TO_PHYS   (2)

void* rpc_malloc(size_t size);
void  rpc_free(void *ptr);
unsigned int rpc_translate_address(unsigned int, unsigned int);

/* RPC testing stubs ------------------------------------------*/

void rpc_mixedprint(int a, char b, float c, double d, short e, int f, char *g);
char* rpc_statalloc_test();
void rpc_printftest(char *format_string, ...);

/* low-level I/O function stubs -------------------------------*/

int rpc_ioctl(int filedes, int command, char *argp);
int rpc_open(const char *filename, int flags, int mode);
int rpc_close(int filedes);

/* string.h function stubs ------------------------------------*/

void * rpc_memchr( void * _cs, int _c, unsigned int _n );
int rpc_memcmp( void * _cs, void * _ct, unsigned int _n );
void * rpc_memcpy( void * _s1, void * _s2, unsigned int _n );
void * rpc_memmove( void * _s1, void * _s2, unsigned int _n );
void * rpc_memset( void * _mem, int _ch, unsigned int _n );
char * rpc_strcat( char * _string1, char * _string2 );
char * rpc_strchr( char * _string, int _c );
int rpc_strcmp( char * _string1, char * _string2 );
char * rpc_strcpy( char * _dest, char * _src );
unsigned int rpc_strcspn( char * _string, char * _chs );
unsigned int rpc_strlen( char * _string );
char * rpc_strncat( char * _to, char * _from, unsigned int _n );
int rpc_strncmp( char * _string1, char * _string2, unsigned int _n );
char * rpc_strncpy( char * _to, char * _from, unsigned int _n );
char * rpc_strpbrk( char * _string, char * _chs );
char * rpc_strrchr( char * _string, int _c );
unsigned int rpc_strspn( char * _string, char * _chs );
char * rpc_strstr( char * _string1, char * _string2 );
int rpc_strcoll( char * _string1, char * _string2 );
unsigned int rpc_strxfrm( char * _to, char * _from, unsigned int _n );
char * rpc_strtok( char * _str1, char * _str2 );
char * rpc_strerror( int _errno );

/* stdio.h function stubs -------------------------------------*/

FILE* rpc_fopen(const char* filename, const char* mode);
FILE* rpc_freopen(const char* filename, const char* mode, FILE* stream);
int rpc_fflush(FILE* stream);
int rpc_fclose(FILE* stream);
int rpc_remove(const char* filename);
int rpc_rename(const char* oldname, const char* newname);
FILE* rpc_tmpfile();
char* rpc_tmpnam(char s[L_tmpnam]);
void rpc_setbuf(FILE* stream, char* buf);
int rpc_fprintf(FILE* stream, const char* format, ...);
int rpc_printf(const char* format, ...);
int rpc_sprintf(char* s, const char* format, ...);
//int rpc_vfprintf(FILE* stream, const char* format, va_list arg);
//int rpc_vprintf(const char* format, va_list arg);
//int rpc_vsprintf(char* s, const char* format, va_list arg);
int rpc_fscanf(FILE* stream, const char* format, ...);
int rpc_scanf(const char* format, ...);
int rpc_sscanf(char* s, const char* format, ...);
int rpc_fgetc(FILE* stream);
char* rpc_fgets(char* s, int n, FILE* stream);
int rpc_fputc(int c, FILE* stream);
int rpc_fputs(const char* s, FILE* stream);
int rpc_getc(FILE* stream);
int rpc_getchar(void);
char* rpc_gets(char* s);
int rpc_putc(int c, FILE* stream);
int rpc_putchar(int c);
int rpc_puts(const char* s);
int rpc_ungetc(int c, FILE* stream);
size_t rpc_fread(void* ptr, size_t size, size_t nobj, FILE* stream);
size_t rpc_fwrite(const void* ptr, size_t size, size_t nobj, FILE* stream);
int rpc_fseek(FILE* stream, long offset, int origin);
long rpc_ftell(FILE* stream);
void rpc_rewind(FILE* stream);
int rpc_fgetpos(FILE* stream, fpos_t* ptr);
int rpc_fsetpos(FILE* stream, const fpos_t* ptr);
void rpc_clearerr(FILE* stream);
int rpc_feof(FILE* stream);
int rpc_ferror(FILE* stream);
void rpc_perror(const char* s);

/* ctype.h function stubs -------------------------------------*/

int rpc_isalnum(int c);
int rpc_isalpha(int c);
int rpc_iscntrl(int c);
int rpc_isdigit(int c);
int rpc_isgraph(int c);
int rpc_islower(int c);
int rpc_isprint(int c);
int rpc_ispunct(int c);
int rpc_isspace(int c);
int rpc_isupper(int c);
int rpc_isxdigit(int c);
int rpc_tolower(int c);
int rpc_toupper(int c);

/* math.h function stubs ------------------------------------- */

double rpc_exp(double x);
double rpc_log(double x);
double rpc_log10(double x);
double rpc_pow(double x, double y);
double rpc_sqrt(double x);
double rpc_ceil(double x);
double rpc_floor(double x);
double rpc_fabs(double x);
double rpc_ldexp(double x, int n);
double rpc_frexp(double x, int* exp);
double rpc_modf(double x, double* ip);
double rpc_fmod(double x, double y);
double rpc_sin(double x);
double rpc_cos(double x);
double rpc_tan(double x);
double rpc_asin(double x);
double rpc_acos(double x);
double rpc_atan(double x);
double rpc_atan2(double y, double x);
double rpc_sinh(double x);
double rpc_cosh(double x);
double rpc_tanh(double x);

/* stdlib.h function stubs ----------------------------------- */

int rpc_abs(int n);
long rpc_labs(long n);
div_t rpc_div(int num, int denom);
ldiv_t rpc_ldiv(long num, long denom);
double rpc_atof(const char* s);
int rpc_atoi(const char* s);
long rpc_atol(const char* s);
double rpc_strtod(const char* s, char** endp);
long rpc_strtol(const char* s, char** endp, int base);
unsigned long rpc_strtoul(const char* s, char** endp, int base);
int rpc_system(const char* s);
char* rpc_getenv(const char* name);
int rpc_rand(void);
void rpc_srand(unsigned int seed);

/* time.h function stubs ------------------------------------- */

clock_t rpc_clock(void);
time_t rpc_time(time_t* tp);
double rpc_difftime(time_t time2, time_t time1);
time_t rpc_mktime(struct tm* tp);
char* rpc_asctime(const struct tm* tp);
char* rpc_ctime(const time_t* tp);
struct tm* rpc_gmtime(const time_t* tp);
struct tm* rpc_localtime(const time_t* tp);
size_t rpc_strftime(char* s, size_t smax, const char* fmt, const struct tm* tp);

#ifdef __cplusplus
}
#endif

#endif //_RPC_STANDALONE_H_

