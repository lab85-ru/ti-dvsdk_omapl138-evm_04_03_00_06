/*****************************************************************************/
/* setjmp.h   v6.1.14                                                         */
/* Copyright (c) 1993-2010 Texas Instruments Incorporated                    */
/*****************************************************************************/

#ifndef _SETJMP
#define _SETJMP

#include <linkage.h>


#ifdef __cplusplus
extern "C" namespace std {
#endif

#define setjmp(x) _setjmp(x)

    typedef int jmp_buf[13];
    #if defined(__EDG_JMP_BUF_NUM_ELEMENTS)
       #if __EDG_JMP_BUF_NUM_ELEMENTS !=13
          #error "Front end and runtime disagree on size of jmp_buf"
       #endif
    #endif

_CODE_ACCESS int  _setjmp(jmp_buf env); 
_CODE_ACCESS void longjmp(jmp_buf env, int val);


#ifdef __cplusplus
} /* extern "C" namespace std */
#endif /* __cplusplus */

#endif /* _SETJMP */

#if defined(__cplusplus) && !defined(_CPP_STYLE_HEADER)
using std::jmp_buf;
using std::_setjmp;
using std::longjmp;
#endif /* _CPP_STYLE_HEADER */


