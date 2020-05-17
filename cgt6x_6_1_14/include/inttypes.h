/*****************************************************************************/
/* INTTYPES.H v6.1.14                                                         */
/* Copyright (c) 2002-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/
#ifndef _INTTYPES_H_
#define _INTTYPES_H_

#include <stdint.h>

/* 7.8 Format conversion of integer types */

typedef struct { intmax_t quot, rem; } imaxdiv_t;

/* 
   According to footnotes in the 1999 C standard, "C++ implementations
   should define these macros only when __STDC_FORMAT_MACROS is defined
   before <inttypes.h> is included." 
*/
#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)
#include "_fmt_specifier.h"
#endif /* !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS) */

/* 7.8.2 Functions for greatest-width integer types */
#include <linkage.h>

_CODE_ACCESS intmax_t  imaxabs(intmax_t j);
_CODE_ACCESS imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);
_CODE_ACCESS intmax_t  strtoimax(const char * restrict nptr, 
		    char ** restrict endptr, int base);
_CODE_ACCESS uintmax_t strtoumax(const char * restrict nptr, 
		    char ** restrict endptr, int base);

#endif /* _INTTYPES_H_ */
