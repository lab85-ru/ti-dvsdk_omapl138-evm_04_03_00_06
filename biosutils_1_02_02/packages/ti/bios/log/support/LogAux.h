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
 *  ======== LogAux.h ========
 */

#ifndef LogAux_
#define LogAux_

#include <std.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== LogAux_disable ========
 *
 *  Disables the set of logs specified by logMask
 */
extern Void LogAux_disable(Uint32 logMask);

/*
 *  ======== LogAux_enable ========
 *
 *  Enables the set of logs specified by logMask
 */
extern Void LogAux_enable(Uint32 logMask);

/*
 *  ======== LogAux_flush ========
 *
 *  Flushes the set of logs specified by logMask.  This function returns no log
 *  data; all data in each log being flushed is discarded. 
 */
extern Void LogAux_flush(Uint32 logMask);

/*
 *  ======== LogAux_init ========
 *
 *  Initialization function, called from main.
 */
extern Void LogAux_init(Void);

/*
 *  ======== LogAux_open ========
 *
 *  Must be called before calling LogAux_read.  Makes necessary initialization
 *  steps for reading log data.  See comment for LogAux_read() function for
 *  usage details.
 *
 *  logMask - the set of logs to read data for.
 */
extern Void LogAux_open(Uint32 logMask);

/*
 *  ======== LogAux_read ========
 *
 *  Copies log data from a set of logs into a buffer.  The logs of interest are
 *  specified by logMask.
 *
 *  Pre-requisites:
 *  It is necessary to call LogAux_open() prior to calling this function.
 *
 *  Post-requisites:
 *  It is necessary to call LogAux_close() after all calls to LogAux_read() have
 *  been made.
 *
 *  Return value:
 *   1  - there is still data left to scan
 *   0  - full scan of logs has completed
 *  -1  - error
 *
 *  Multiple calls to LogAux_read() can be made in between one initial call to
 *  LogAux_open() and one end call to LogAux_close().  In other words, the
 *  calling flow should be like:
 *    1. call LogAux_open() once.
 *    2. make one or many calls to LogAux_read().
 *    3. call LogAux_close() once.
 *
 * dataBuffer - buffer used to store log data.
 * size       - the size of this data buffer.  Determines amount of log data to
 *              fill.  Upon return, size will contain the payload size.
 *              Note: size must be an Int* because it is sent to the NDK API
 *              send(), which must take an integer type.
 */
extern Int LogAux_read(Char *dataBuffer, Int *size);

/*
 *  ======== LogAux_close ========
 *
 *  clean up and reset flags.  Must be called before call after reading log data
 *  via LogAux_read.  See comment for LogAux_read() function for
 *  usage details.
 *  
 */
extern Void LogAux_close(Void);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* LogAux_ */

/*
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:22; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

