/*****************************************************************************/
/*  ERRNO.H  v6.1.14                                                          */
/*  Copyright (c) 1995-2010 Texas Instruments Incorporated                   */
/*****************************************************************************/

#ifndef _ERRNO
#define _ERRNO


#ifdef __cplusplus
//----------------------------------------------------------------------------
// <cerrno> IS RECOMMENDED OVER <errno.h>.  <errno.h> IS PROVIDED FOR
// COMPATIBILITY WITH C AND THIS USAGE IS DEPRECATED IN C++
//----------------------------------------------------------------------------
extern "C" namespace std
{
#endif /* __cplusplus */

#include <linkage.h>


extern _DATA_ACCESS int errno;
#ifdef __cplusplus
  #define errno ::std::errno
#else
  #define errno errno
#endif
#define EDOM   1
#define ERANGE 2
#define ENOENT 3
#define EFPOS  5
#define EILSEQ 6

#ifdef __cplusplus
} /* extern "C" namespace std */
#endif /* __cplusplus */

#endif  /* _ERRNO */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)


#endif /* _CPP_STYLE_HEADER */

