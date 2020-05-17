/*
 * trgmsg.c
 */

/*
 * Copyright (C) 1995-2010 Texas Instruments Incorporated - http://www.ti.com/ 
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
*  Re-defines DSP RTS library low level functions which implement C I/O *
*  Derived from C6000 Codegen RTS library file trgmsg.c                 *
************************************************************************/


/*  ----------------------------------- Standard headers            */
#include <stdint.h>
#include <stdlib.h>

/*  ----------------------------------- Application Headers         */
#include "cio_ipc.h"
#include "C6Run_log.h"  


/************************************************************
* Explicit External Declarations                            *
************************************************************/


/************************************************************
* Local Macro Declarations                                  *
************************************************************/


/************************************************************
* Local Typedef Declarations                                *
************************************************************/


/************************************************************
* Local Function Declarations                               *
************************************************************/


/************************************************************
* Local Variable Definitions                                *
************************************************************/


/************************************************************
* Global Variable Definitions                               *
************************************************************/


/************************************************************
* Global Function Definitions                               *
************************************************************/

_CODE_ACCESS void writemsg(      unsigned char  command,
                           const unsigned char *parm,
                           const          char *data,
                                 unsigned int   length)
{
  uint8_t *p;
  uint32_t i;
  CIO_Msg *msg = NULL;

  // Allocate a new message
  CIO_IPC_allocMsg(&msg);

  if (!msg)
  {
    return;
  }
  
  p = (uint8_t *) CIO_IPC_getMsgBuffer(msg);

  /***********************************************************************/
  /* THE LENGTH IS WRITTEN AS A TARGET INT                               */
  /***********************************************************************/ 
  *(uint32_t *)p = length; 
  p += sizeof(uint32_t);

  /***********************************************************************/ 
  /* THE COMMAND IS WRITTEN AS A TARGET BYTE                             */
  /***********************************************************************/ 
  *p++ = command;

  /***********************************************************************/ 
  /* PACK THE PARAMETERS AND DATA SO THE HOST READS IT AS BYTE STREAM    */
  /***********************************************************************/ 
  for (i = 0; i < 8; i++)      PACKCHAR(*parm++, p, i);
  for (i = 0; i < length; i++) PACKCHAR(*data++, p, i+8);
  
  CIO_IPC_sendMsg(msg,CIO_TRANSFER);
}

_CODE_ACCESS void readmsg(register unsigned char *parm,
                          register char          *data)
{
  uint8_t *p;
  uint32_t i;
  uint32_t length;
  CIO_Msg *msg;
  
  if (CIO_IPC_recvMsg(&msg) != CIO_TRANSFER_RET)
  {
    C6RUN_LOG_error0("Transport error during CIO_TRANSFER");
    return;
  }

  p = (uint8_t *) CIO_IPC_getMsgBuffer(msg);

  // THE LENGTH IS READ AS A TARGET INT
  length = *(uint32_t *)p;
  p += sizeof(uint32_t);
  
  // UNPACK THE PARAMETERS AND DATA
  for (i = 0; i < 8; i++) *parm++ = UNPACKCHAR(p, i);
  if (data != NULL) 
     for (i = 0; i < length; i++) *data++ = UNPACKCHAR(p, i+8);

  // Free up this message now that we are done with it
  CIO_IPC_freeMsg(msg);
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/


/***********************************************************
* End file                                                 *
***********************************************************/

