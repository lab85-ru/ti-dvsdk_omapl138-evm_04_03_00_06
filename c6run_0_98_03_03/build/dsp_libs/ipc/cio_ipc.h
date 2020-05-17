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
*  Header file for cio.c module.                                        *
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
extern far "c" {
#endif

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/

/*---------------------------------------------------------------------------*/
/* THESE MACROS PACK AND UNPACK SHORTS AND LONGS INTO CHARACTER ARRAYS       */
/*                                                                           */
/* THIS IS DONE BYTE REVERSED TO MAKE THE PC'S JOB EASIER AND SINCE THE      */
/*     DEVICE COULD BE BIG OR LITTLE ENDIAN                                  */
/*---------------------------------------------------------------------------*/
#define LOADSHORT(x,y,z)  { x[(z)]   = (unsigned short) (y); \
                            x[(z)+1] = (unsigned short) (y) >> 8;  }

#define UNLOADSHORT(x,z) ((short) ( (short) x[(z)] +             \
				   ((short) x[(z)+1] << 8)))

#define LOAD32(x,y,z) { x[(z)]   = (unsigned int) (y); \
                          x[(z)+1] = (unsigned int) (y) >> 8; \
                          x[(z)+2] = (unsigned int) (y) >> 16;  \
                          x[(z)+3] = (unsigned int) (y) >> 24;   }

#define UNLOAD32(x,z) ( (int) x[(z)]           +  \
		       ((int) x[(z)+1] << 8)   +  \
		       ((int) x[(z)+2] << 16)  +  \
		       ((int) x[(z)+3] << 24))

/*---------------------------------------------------------------------------*/
/* THESE MACROS CAN BE USED TO PACK AND UNPACK CHARACTER STREAMS INTO TARGET */
/* WORDS FOR MORE EFFICIENT TRANSFER.                                        */
/*---------------------------------------------------------------------------*/
#define PACKCHAR(val, base, byte) ( (base)[(byte)] = (val) )

#define UNPACKCHAR(base, byte)    ( (base)[byte] )


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
extern void       CIO_IPC_freeMsg( CIO_Msg *msg );

extern void      *CIO_IPC_getMsgBuffer( CIO_Msg *msg );


/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif

#endif //_CIO_IPC_H_

