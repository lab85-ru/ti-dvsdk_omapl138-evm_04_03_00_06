/*
 * control_ipc_common.h
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
*  This file contains common CONTROL IPC definitions used on DSP and         *
*    GPP sides                                                               *
*****************************************************************************/

#ifndef _CONTROL_IPC_COMMON_H_
#define _CONTROL_IPC_COMMON_H_

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

#define CONTROL_MSG_TOTALSIZE (1024)

// Number of in-flight messages
#define CONTROL_MSG_CNT       (1)

// Common message queue names for C6Run CONTROL MSGQs.
#define CONTROL_FROM_GPP_MSGQNAME  "CONTROL_FROM_GPP_MSGQ"
#define CONTROL_TO_GPP_MSGQNAME    "CONTROL_TO_GPP_MSGQ"


/***********************************************************
* Global Typedef declarations                              *
***********************************************************/

// Command values for CONTROL messages
typedef enum _CONTROL_MSG_CMDS_
{
  CONTROL_NULL          = 0,
  CONTROL_DSP_STARTAPP,
  CONTROL_DSP_STARTLIB,    
  CONTROL_DSP_STOP,
  CONTROL_DSP_READY,
  CONTROL_DSP_FINISHED,
  CONTROL_DSP_ABORTED
}
CONTROL_MSG_Commands;


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

#endif // _CONTROL_IPC_COMMON_H_
