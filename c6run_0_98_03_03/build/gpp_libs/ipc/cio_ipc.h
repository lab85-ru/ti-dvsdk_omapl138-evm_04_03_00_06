/*
 * cio_ipc.h
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
*  Header file for cio_ipc.c module.                                    *
************************************************************************/


#ifndef _CIO_IPC_H_
#define _CIO_IPC_H_

/*  ----------------------------------- Standard C Headers            */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*  ----------------------------------- IPC Specific Headers          */
#include "cio_ipc_common.h"

// Prevent C++ name mangling
#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/*****************************************************************************/
/* THE TARGET WRITES DATA PASSED IN parmbuf IN LITTLE ENDIAN.  IF ON A BIG   */
/* ENDIAN HOST, WE NEED TO SWAP BYTES WHEN READING AND STORING TO parmbuf.   */
/*****************************************************************************/
#ifdef HOST_LITTLE
  #define UNLOADINT(x,z)   (*(unsigned short *) ((x)+(z)))
  #define LOADINT(x,y,z)   (*(unsigned short *) ((x)+(z)) = (unsigned short) (y));
  #define UNLOADLONG(x,z)  (*(unsigned long  *) ((x)+(z)))
  #define LOADLONG(x,y,z)  (*(unsigned long  *) ((x)+(z)) = (unsigned long) (y));
#else
  #define UNLOADINT(x,z)  ( (uint8_t) x[(z)] + \
                           ((uint8_t) x[(z)+1] << 8))

  #define LOADINT(x,y,z)   { x[(z)]   = (unsigned short) (y);\
                             x[(z)+1] = (unsigned short) (y) >> 8; }

  #define UNLOADLONG(x,z) ( (unsigned long) (uint8_t) x[(z)]          + \
                           ((unsigned long) (uint8_t) x[(z)+1] << 8)  + \
                           ((unsigned long) (uint8_t) x[(z)+2] << 16) + \
                           ((unsigned long) (uint8_t) x[(z)+3] << 24))

  #define LOADLONG(x,y,z) { x[(z)]   = (unsigned long) (y); \
                            x[(z)+1] = (unsigned long) (y) >> 8; \
                            x[(z)+2] = (unsigned long) (y) >> 16; \
                            x[(z)+3] = (unsigned long) (y) >> 24; }
#endif /* HOST_LITTLE */

#define T_INTSZ 4
#define T_CHARSZ 1
#define MEM_INC 1

#define INT_ADDR_INC  (T_INTSZ  / MEM_INC)
#define BYTE_ADDR_INC (T_CHARSZ / MEM_INC)


/***********************************************************
* Global Typedef Declarations                              *
***********************************************************/

// Forward declaration of CIO_Msg structure
typedef struct CIO_Msg CIO_Msg;


/***********************************************************
* Global Variable Declarations                             *
***********************************************************/


/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern int32_t    CIO_IPC_init( void );
extern void       CIO_IPC_exit( void );

extern void       CIO_IPC_sendMsg( CIO_Msg *msg, uint16_t msgId );
extern uint16_t   CIO_IPC_recvMsg( CIO_Msg **msg );

extern void       CIO_IPC_allocMsg( CIO_Msg **msg );
extern void       CIO_IPC_freeMsg ( CIO_Msg *msg );

extern void      *CIO_IPC_getMsgBuffer( CIO_Msg *msg );
extern uint32_t   CIO_IPC_getMsgBufferSize ( void );

extern void       CIO_IPC_readMsg ( CIO_Msg *msg, uint8_t *command, uint8_t *parmbuf, uint8_t *buffer);
extern void       CIO_IPC_writeMsg( CIO_Msg *msg, uint8_t *parmbuf, uint8_t *buffer, uint32_t length);


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_CIO_IPC_H_

