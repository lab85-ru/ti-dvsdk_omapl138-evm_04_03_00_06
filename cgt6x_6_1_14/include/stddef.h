/*****************************************************************************/
/* stddef.h   v6.1.14                                                         */
/* Copyright (c) 1993-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/

#ifndef _STDDEF
#define _STDDEF

#ifdef __cplusplus
extern "C" namespace std {
#endif

#ifndef NULL
#define NULL 0
#endif

typedef __PTRDIFF_T_TYPE__ ptrdiff_t;

#ifndef _SIZE_T
#define _SIZE_T
typedef __SIZE_T_TYPE__ size_t;
#endif

#ifndef __cplusplus
#ifndef _WCHAR_T
#define _WCHAR_T

typedef __WCHAR_T_TYPE__ wchar_t;

#endif /* _WCHAR_T */
#endif /* ! __cplusplus */

#define offsetof(_type, _ident) ((size_t)__intaddr__(&(((_type *)0)->_ident)))

#ifdef __cplusplus
} /* extern "C" namespace std */
#endif  /* __cplusplus */

#endif  /* _STDDEF */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::ptrdiff_t;
using std::size_t;
/*using std::wchar_t;*/
#endif /* _CPP_STYLE_HEADER */

