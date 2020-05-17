/*****************************************************************************/
/* wctype.h    v6.1.14                                                        */
/* Copyright (c) 2007-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/

/************************************************************************/
/*                                                                      */
/*  WIDE CHARACTER TYPING FUNCTIONS AND MACROS                          */
/*                                                                      */
/************************************************************************/
#ifndef _WCTYPE
#define _WCTYPE

#include <linkage.h>


#ifdef __cplusplus
//----------------------------------------------------------------------------
// <cwctype> IS RECOMMENDED OVER <wctype.h>.  <wctype.h> IS PROVIDED FOR
// COMPATIBILITY WITH C AND THIS USAGE IS DEPRECATED IN C++
//----------------------------------------------------------------------------
extern "C" namespace std
{
#endif /* __cplusplus */

#ifndef WEOF
#define WEOF ((wint_t)(-1))
#endif

/************************************************************************/
/*   TYPES                                                              */
/************************************************************************/
#ifndef _WINTT
#define _WINTT
typedef int   wint_t; 
#endif

typedef void *wctrans_t;
typedef void *wctype_t;

/************************************************************************/
/*   FUNCTION DECLARATIONS                                              */
/************************************************************************/
_CODE_ACCESS int       iswalnum(wint_t _c);
_CODE_ACCESS int       iswalpha(wint_t _c);
_CODE_ACCESS int       iswcntrl(wint_t _c);
_CODE_ACCESS int       iswdigit(wint_t _c);
_CODE_ACCESS int       iswgraph(wint_t _c);
_CODE_ACCESS int       iswlower(wint_t _c);
_CODE_ACCESS int       iswprint(wint_t _c);
_CODE_ACCESS int       iswpunct(wint_t _c);
_CODE_ACCESS int       iswspace(wint_t _c);
_CODE_ACCESS int       iswupper(wint_t _c);
_CODE_ACCESS int       iswxdigit(wint_t _c);
_CODE_ACCESS int       iswascii(wint_t _c);

_CODE_ACCESS wctype_t  wctype(const char *property); 
_CODE_ACCESS int       iswctype(wint_t c, wctype_t category); 

_CODE_ACCESS wint_t    towupper(wint_t _c);
_CODE_ACCESS wint_t    towlower(wint_t _c);
_CODE_ACCESS wint_t    towascii(wint_t _c);

_CODE_ACCESS wctrans_t wctrans(const char *property); 
_CODE_ACCESS wint_t    towctrans(wint_t c, wctrans_t category);

#ifdef __cplusplus
} /* extern "C" namespace std */
#endif /* __cplusplus */

#endif /* ! _WCTYPE */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::wint_t;
using std::wctrans_t;
using std::wctype_t;
using std::iswalnum;
using std::iswalpha;
using std::iswcntrl;
using std::iswdigit;
using std::iswgraph;
using std::iswlower;
using std::iswprint;
using std::iswpunct;
using std::iswspace;
using std::iswupper;
using std::iswxdigit;
using std::iswascii;
using std::wctype;
using std::iswctype;
using std::towupper;
using std::towlower;
using std::towascii;
using std::wctrans;
using std::towctrans;

#endif /* ! _CPP_STYLE_HEADER */
