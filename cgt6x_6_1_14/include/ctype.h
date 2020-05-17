/*****************************************************************************/
/* ctype.h    v6.1.14                                                         */
/* Copyright (c) 1993-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/

/************************************************************************/
/*                                                                      */
/*  CHARACTER TYPING FUNCTIONS AND MACROS                               */
/*                                                                      */
/*  Note that in this implementation, either macros or functions may    */
/*  be used.  Macros are prefixed with an underscore.                   */
/*                                                                      */
/************************************************************************/
#ifndef _CTYPE
#define _CTYPE

#include <linkage.h>


#ifdef __cplusplus
//----------------------------------------------------------------------------
// <cctype> IS RECOMMENDED OVER <ctype.h>.  <ctype.h> IS PROVIDED FOR
// COMPATIBILITY WITH C AND THIS USAGE IS DEPRECATED IN C++
//----------------------------------------------------------------------------
extern "C" namespace std
{
#endif /* __cplusplus */

extern _DATA_ACCESS const unsigned char _ctypes_[];

/************************************************************************/
/*   FUNCTION DECLARATIONS                                              */
/************************************************************************/
#include "_isfuncdcl.h"

_IDECL int toupper(int _c);
_IDECL int tolower(int _c);

#ifdef __cplusplus
} /* extern "C" namespace std */
#endif /* __cplusplus */


/************************************************************************/
/*  On this ELSE path, all the TI ctype table and ctype bit flags are   */
/*  defined.                                                            */
/************************************************************************/
/************************************************************************/
/*  MACRO DEFINITIONS                                                   */
/************************************************************************/
#define _U_   0x01       /* control chars     */
#define _L_   0x02       /* lower case letter */
#define _N_   0x04       /* digit             */
#define _S_   0x08       /* white space       */
#define _P_   0x10       /* punctuation       */
#define _C_   0x20       /* control chars     */
#define _H_   0x40       /* A-F, a-f and 0-9  */
#define _B_   0x80       /* blank             */

#define _isalnum(a)  (_ctypes_[(a)+1] & (_U_ | _L_ | _N_))
#define _isalpha(a)  (_ctypes_[(a)+1] & (_U_ | _L_))
#define _iscntrl(a)  (_ctypes_[(a)+1] & _C_)
#define _isdigit(a)  (_ctypes_[(a)+1] & _N_)
#define _isgraph(a)  (_ctypes_[(a)+1] & (_U_ | _L_ | _N_ | _P_))
#define _islower(a)  (_ctypes_[(a)+1] & _L_)
#define _isprint(a)  (_ctypes_[(a)+1] & (_B_ | _U_ | _L_ | _N_ | _P_))
#define _ispunct(a)  (_ctypes_[(a)+1] & _P_)
#define _isspace(a)  (_ctypes_[(a)+1] & _S_)
#define _isupper(a)  (_ctypes_[(a)+1] & _U_)
#define _isxdigit(a) (_ctypes_[(a)+1] & _H_)


#define _isascii(a)  (((a) & ~0x7F) == 0)
#define _toupper(b)  ((_islower(b)) ? (b) - ('a' - 'A') : (b))
#define _tolower(b)  ((_isupper(b)) ? (b) + ('a' - 'A') : (b))
#define _toascii(a)  ((a) & 0x7F)

#ifdef _INLINE

#ifdef __cplusplus
using std::_ctypes_;
namespace std {
#endif /* __cplusplus */

#include "_isfuncdef.h"

/****************************************************************************/
/*  tolower                                                                 */
/****************************************************************************/
static __inline int tolower(int ch)
{
   /*
    This code depends on two assumptions: (1) all of the letters of the
    alphabet of a given case are contiguous, and (2) the lower and upper
    case forms of each letter are displaced from each other by the same
    constant value.
   */

   if ( (unsigned int)(ch - 'A') <= (unsigned int)('Z' - 'A')) ch += 'a' - 'A';
   return ch;
}

/****************************************************************************/
/*  toupper                                                                 */
/****************************************************************************/
static __inline int toupper(int ch)
{
   /*
    This code depends on two assumptions: (1) all of the letters of the
    alphabet of a given case are contiguous, and (2) the lower and upper
    case forms of each letter are displaced from each other by the same
    constant value.
   */

   if ( (unsigned int)(ch - 'a') <= (unsigned int)('z' - 'a')) ch -= 'a' - 'A';
   return ch;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _INLINE */

#endif /* ! _CTYPE */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::_ctypes_;
using std::isalnum;
using std::isalpha;
using std::iscntrl;
using std::isdigit;
using std::isgraph;
using std::islower;
using std::isprint;
using std::ispunct;
using std::isspace;
using std::isupper;
using std::isxdigit;
using std::isascii;
using std::toupper;
using std::tolower;
using std::toascii;
#endif /* ! _CPP_STYLE_HEADER */

