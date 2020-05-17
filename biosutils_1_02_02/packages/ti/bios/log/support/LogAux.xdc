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
 *  ======== LogAux.xdc ========
 *  LogAux Configuration interface
 */

/*!
 *  ======== LogAux ======== 
 *  LogAux module.
 *  
 *  The LogAux module provides an API set that is auxiliary to BIOS log.  This
 *  API gives the user a degree of control over how the BIOS logs are used, and
 *  allows the user to obtain data from BIOS logs in a format that can be read
 *  by the RTA decoder.
 *  
 *  In order to use the LogAux module, it is necessary to call the module's
 *  init() function, which should be called during program startup:
 *  @p(code)
 *  LogAux_init()
 *  @p
 *  The LogAux module's init() function could be called from the user's main()
 *  function, for example.
 *
 *  The LogAux module provides the following features for accessing and
 *  controlling BIOS logs:
 *  
 *  @p(blist)
 *  
 *  - Enabling and Disabling a set of BIOS logs.
 *  - Flushing a set of BIOS logs (log data is discarded, not returned).
 *  - Reading raw log data from Target memory and copying it into a buffer for
 *  - for a set of BIOS logs.  The LogAux module will also "remember" which log
 *    and what position it left off on previously in between reads.
 *  
 *  @p
 *  The set of logs that each API will affect is determined by a 32-bit log mask
 *  that is passed to the API.  Each bit in this log mask corresponds to a BIOS
 *  log ID.  Only logs that are set by the log mask will be affected by the
 *  LogAux APIs.
 *
 *  Order Of Log Data Sent By The LogAux Module
 *  @p(blist)
 *  - Data for the ti_bios_log_create log is always sent first (if it is
 *    enabled).
 *  - After all data of the ti_bios_log_create log is sent, the data of all
 *    remaing, enabled logs in the system will follow.
 *  - The order of the data of these remaing logs is sent based on the order
 *    in which these logs are stored in Target memory.  (See the program's
 *    generated map file to find the ordering of the BIOS LOG objects in
 *    memory.
 *  @p
 *
 *  Using The LogAux Module To Read BIOS Logs
 *
 *  Log data may be read by using the LogAux_read() API.  However, this API has
 *  both a prerequisite - LogAux_open(), as well as a postrequisite -
 *  LogAux_close().  In order to read the BIOS log data, the function call flow
 *  must be as follows:
 *    1. call LogAux_open() once.
 *    2. make one or many calls to LogAux_read().
 *    3. call LogAux_close() once.
 *  
 */  

metaonly module LogAux {
}

/*
 *!
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:22; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

