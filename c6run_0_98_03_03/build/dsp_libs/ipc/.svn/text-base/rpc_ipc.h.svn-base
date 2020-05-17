/*
 * rpc_ipc.h
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
 
/************************************************************************
*  Header file for rpc_ipc.c module.                                    *
************************************************************************/

#ifndef _RPC_IPC_H_
#define _RPC_IPC_H_

/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*  ----------------------------------- IPC Specific Headers          */
#include "rpc_ipc_common.h"   // definitions in common with GPP side

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/


/***********************************************************
* Global Typedef Declarations                              *
***********************************************************/

// Forward declaration of RPC_Msg structure
typedef struct RPC_Msg RPC_Msg;


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/

extern RPC_Msg *RPC_msg;


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern int32_t    RPC_IPC_init( void );
extern void       RPC_IPC_exit( void );

extern void       RPC_IPC_sendMsg( RPC_Msg *msg, uint16_t msgId );
extern uint16_t   RPC_IPC_recvMsg( RPC_Msg **msg );

extern void       RPC_IPC_allocMsg( RPC_Msg **msg );
extern void       RPC_IPC_freeMsg( RPC_Msg *msg );

extern void      *RPC_IPC_getMsgBuffer ( RPC_Msg *msg );
extern uint32_t   RPC_IPC_getMsgBufferSize( void );


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_RPC_IPC_H_

