/*
 * ipc_common.h
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
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

/*****************************************************************************
*  This file contains commmon debug and error reporting macros               *
*****************************************************************************/

#ifndef _DEBUG_COMMON_H_
#define _DEBUG_COMMON_H_

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/* Error Handling Macros */
#define REPORT_STATUS(name) \
        printf(name "() failed. Status = [0x%08x]\n", (unsigned int) status);

#define IF_ERROR_REPORT_AND_RETURN(name) \
        if (DSP_FAILED(status)) \
        { \
            printf(name "() failed. Status = [0x%x]\n", (unsigned int) status);\
            return status;\
        }

#define IF_ERROR_REPORT_1ST_AND_CONTINUE(name) \
    if (!failure_seen && DSP_FAILED(status)) \
    { \
        printf(name "() failed. Status = [0x%x]\n", (unsigned int) status); \
        failure_seen = 1; \
    }

/* Debug Macros */
#ifdef VERBOSE
  #define VERBOSE_PRINT0(STR)                 printf(STR)
  #define VERBOSE_PRINT1(STR,ARG1)            printf(STR,ARG1)
  #define VERBOSE_PRINT2(STR,ARG1,ARG2)       printf(STR, ARG1, ARG2)
  #define VERBOSE_PRINT3(STR,ARG1,ARG2,ARG3)  printf(STR, ARG1, ARG2, ARG3)
#else
  #define VERBOSE_PRINT0(STR)                 do{ }while(0)
  #define VERBOSE_PRINT1(STR,ARG1)            do{ }while(0)
  #define VERBOSE_PRINT2(STR,ARG1,ARG2)       do{ }while(0)
  #define VERBOSE_PRINT3(STR,ARG1,ARG2,ARG3)  do{ }while(0)
#endif


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif // _DEBUG_COMMON_H_
