/*
 * gpp_stubs_io.c
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


#include <sys/ioctl.h>
#include "gpp_stubs_base.h"

/* ----- Stubs for low-level I/O functions ------ */

void rpc_ioctl(void **param_buffer, void *result_buffer)
{
    int result = ioctl(RPC_CAST_PARAM(param_buffer[0], int),
                       RPC_CAST_PARAM(param_buffer[1], int),
                       RPC_CAST_PARAM(param_buffer[2], char *)
                      );
    RPC_LOAD_RESULT(result_buffer, int, result);
}

int rpc_open(void **param_buffer, void *result_buffer)
{
    int result = open (RPC_CAST_PARAM(param_buffer[0], char *),
                       RPC_CAST_PARAM(param_buffer[1], int),
                       RPC_CAST_PARAM(param_buffer[2], int)
                      );
    RPC_LOAD_RESULT(result_buffer, int, result);                      
}

int rpc_close(void **param_buffer, void *result_buffer)
{
    int result = close (RPC_CAST_PARAM(param_buffer[0], int));
    RPC_LOAD_RESULT(result_buffer, int, result);  
}
