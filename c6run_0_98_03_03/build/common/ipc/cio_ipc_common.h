/*
 * cio_ipc_common.h
 */

/*
 * Copyright (C) 2010-2011 Texas Instruments Incorporated - http://www.ti.com/ 
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
*  This file contains common CIO definitions used on DSP and GPP sides       *
*****************************************************************************/

#ifndef _CIO_IPC_COMMON_H_
#define _CIO_IPC_COMMON_H_

/*  ----------------------------------- Standard C headers          */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

#define CIO_MSG_TOTALSIZE     (1024)

// Number of file IDs available.
#define CIO_NFILE             (20)

// Number of in-flight messages.
#define CIO_MSG_CNT           (1)

// Common message queue names for C6Run CIO MSGQs.
#define CIO_FROM_GPP_MSGQNAME "CIO_FROM_GPP_MSGQNAME"
#define CIO_TO_GPP_MSGQNAME   "CIO_TO_GPP_MSGQNAME"

// Constants for the data transfer functions for the predefined driver
#ifndef _DTOPEN
  #define _DTOPEN    (0xF0)
  #define _DTCLOSE   (0xF1)
  #define _DTREAD    (0xF2)
  #define _DTWRITE   (0xF3)
  #define _DTLSEEK   (0xF4)
  #define _DTUNLINK  (0xF5)
  #define _DTGETENV  (0xF6)
  #define _DTRENAME  (0xF7)
  #define _DTGETTIME (0xF8)
  #define _DTGETCLK  (0xF9)
  #define _DTSYNC    (0xFF)
#endif

//-----------------------------------------------------------------------------
// TARGET'S FILE OPEN FLAGS   *** MAKE SURE THIS STAYS CONSISTENT WITH
//                            *** FILE.H IN THE TARGET'S CIO ROUTINES
//-----------------------------------------------------------------------------
#ifndef TRG_O_RDONLY
  #define  TRG_O_RDONLY    (0x0000) /* open for reading      */
  #define  TRG_O_WRONLY    (0x0001) /* open for writing      */
  #define  TRG_O_RDWR      (0x0002) /* open for read & write */
  #define  TRG_O_APPEND    (0x0008) /* append on each write  */
  #define  TRG_O_CREAT     (0x0200) /* open with file create */
  #define  TRG_O_TRUNC     (0x0400) /* open with truncation  */
  #define  TRG_O_BINARY    (0x8000) /* open in binary mode   */
#endif


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/

// Command values for CIO messages
typedef enum _CIO_MSG_CMDS_
{
  CIO_NULL          = 0,
  CIO_TRANSFER         ,
  CIO_TRANSFER_RET     ,
  CIO_TERMINATE        ,
  CIO_TERMINATE_RET
}
CIO_MSG_Commands;


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

#endif // _CIO_COMMON_H_
