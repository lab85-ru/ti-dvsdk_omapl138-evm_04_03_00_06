/*
 * dsp_stubs_io.c
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


#include "dsp_stubs_base.h"

/* Low level I/O function stubs -------------------------------------*/


int rpc_ioctl(int filedes, int command, char *argp)
{
    RPC_INIT("rpc_ioctl", "iii@");
    RPC_PARAM(int, &filedes);
    RPC_PARAM(int, &command);
    RPC_BUFFER(char*, &argp);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_open(const char *filename, int flags, int mode)
{
    RPC_INIT("rpc_open", "i@ii");
    RPC_BUFFER(char*, &filename);
    RPC_PARAM(int, &flags);
    RPC_PARAM(int, &mode);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}

int rpc_close(int filedes)
{
    RPC_INIT("rpc_close", "ii");
    RPC_PARAM(int, &filedes);
    RPC_PERFORM();
    return RPC_GETRESULT(int);
}
