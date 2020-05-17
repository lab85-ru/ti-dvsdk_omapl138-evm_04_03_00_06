/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Use of this software is controlled by the terms and conditions found in
 * the New BSD Software License Agreement, which is reproduced in its
 * entirity below.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== LogServerCgi.h ========
 */

#ifndef LogServerCgi_
#define LogServerCgi_

#include <ti/bios/include/std.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This structure can be used to define the priority of CGI command threads.
 *  When a CGI function is run, the first thing it will do is change the
 *  priority of the task (whose context it's running under) to the value
 *  defined by this structure.
 *
 * priority  - used to change the priority which CGI threads will run at.  The
 *           setting of CGI thread priority using this variable will result in
 *           all CGI threads in the program running at that priority level.
 *           However, the user is limited to the values in the range [0 - 5].
 *           The following rules apply when setting the CGI thread priority:
 *
 *           1. priority set to a number in the range [0 - 5] - All CGI threads
 *               in the program will run at the value specified.
 *           2. priority set to a negative number - All CGI threads will run at
 *               the default priority of OS_TASKPRINORM.
 *           3. priority  set to a value greater than OS_TASKPRINORM - All CGI
 *              threads will run at default priority of OS_TASKPRINORM.
 *
 * segId     - Used to change the memory segment in which the data buffer used
 *             for copying log data is allocated from.
 * bufSize   - Used to change the size of the data buffer used for copying log
 *             data.
 */
typedef struct LogServerCgi_Config {
    Uint32 priority;   /* priority to set for CGI threads */
    Int segId;         /* segment ID to use for MEM_alloc()*/
    Uint32 bufSize;    /* size of the log data buffer */
} LogServerCgi_Config;

extern LogServerCgi_Config LogServerCgi_config;

/*
 *  ======== LogServerCgi_add ========
 *
 *  Add CGI scripts.
 */
extern void LogServerCgi_add(void);

/*
 *  ======== LogServerCgi_init ========
 *
 *  Initialization function.
 */
extern Void LogServerCgi_init(Void);

/*
 *  ======== LogServerCgi_remove ========
 *
 *  Remove CGI scripts.
 */
extern void LogServerCgi_remove(void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* LogServerCgi_ */

/*
 */
/*
 *  @(#) ti.bios.log.ndk; 1, 0, 2, 02,31; 5-23-2009 17:29:07; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

