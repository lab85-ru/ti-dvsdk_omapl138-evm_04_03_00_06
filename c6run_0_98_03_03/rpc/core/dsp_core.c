/*
 * dsp_core.c
 */

/*
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * Copyright (C) 2010 Yaman Umuroglu 
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
*  Core module which provides the DSP-side functionality for            *
*  DSP->GPP RPC                                                         *
************************************************************************/


/*  ----------------------------------- Standard headers            */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*  ----------------------------------- RPC specific headers        */
#include "dsp_stubs_base.h"

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

void rpc_send();
void rpc_recv();

/************************************************************
* Local Variable Definitions                                *
************************************************************/

unsigned char rpc_response_received = 0;

/************************************************************
* Global Variable Definitions                               *
************************************************************/

char rpc_request_buffer[RPC_BUFSZ];
char rpc_response_buffer[RPC_RESPSZ];
unsigned int rpc_request_length;

/************************************************************
* Global Function Definitions                               *
************************************************************/

/* Request CMEM-allocated buffer from the GPP, needed for RPCs with buffer parameters */
void* rpc_malloc(size_t size)
{
    // Load requested allocation size into the RPC buffer
    *(size_t*)rpc_request_buffer = size;
    rpc_request_length = sizeof(size_t);
    unsigned char param_buffer[8];
    writemsg(RPC_MSG_MALLOC, param_buffer, rpc_request_buffer, rpc_request_length);
    // Obtain the reply in the same manner as other RPC calls
    rpc_recv();
    // Cast the reply into void* and return it
    return RPC_GETRESULT(void *);
}


/* Free CMEM-allocated buffer */
void rpc_free(void *ptr)
{
    *(unsigned int *)rpc_request_buffer = (unsigned int) ptr;
    rpc_request_length = sizeof(unsigned int);
    unsigned char param_buffer[8];
    writemsg(RPC_MSG_FREE, param_buffer, rpc_request_buffer, rpc_request_length);
    // Obtain the reply in the same manner as other RPC calls
    rpc_recv();
}

/* Perform physical<->virtual address translations */
unsigned int rpc_translate_address(unsigned int src_addr, unsigned int type)
{
    // The GPP is responsible for mapping addresses back and forth
    // so we pass the source address and translation type in the RPC buffer
    // and send a RPC_MSG_TRANSLATE request to the GPP, then extract the result
    *(unsigned int *)rpc_request_buffer = src_addr;
    *(unsigned int *)(rpc_request_buffer + sizeof(unsigned int)) = type;
    rpc_request_length = 2 * sizeof(unsigned int);
    unsigned char param_buffer[8];
    writemsg(RPC_MSG_TRANSLATE, param_buffer, rpc_request_buffer, rpc_request_length);
    // Obtain the reply and extract an unsigned int from it, which is the result
    rpc_recv();
    return *(unsigned int*)rpc_response_buffer;
}

/* Perform the RPC send-receive calls */
void rpc_perform()
{
    rpc_response_received = 0;
    rpc_send();
    rpc_recv();   
}

/***********************************************************
* Local Function Definitions                               *
***********************************************************/

/* Send marshalled RPC request to the GPP side */
void rpc_send()
{
    /* FIXME move RPC to its own transport and control system */
    /* For now, we're using the CIO transport system to carry data */
    unsigned char param_buffer[8];
    writemsg(RPC_MSG_REQUEST, param_buffer, rpc_request_buffer, rpc_request_length);
}

/* Receive RPC response from GPP side */
void rpc_recv()
{
    /* FIXME move RPC to its own transport and control system*/
    /* For now, we're using the CIO transport system to carry data */
    unsigned char param_buffer[8];
    
    readmsg(param_buffer, rpc_response_buffer);
    
    /* FIXME check message ID and verify it's RPC_MSG_RESPONSE */
    
    rpc_response_received = 1;
}

/***********************************************************
* End file                                                 *
***********************************************************/
