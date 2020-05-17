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
 *  ======== LogServerCgi.xdc ========
 */

/*!
 *  ======== LogServerCgi ========
 *  LogServerCgi module.
 *  
 *  The LogServerCgi module provides a command interface to the Target DSP using
 *  the Common Gateway Interface (CGI).  The commands in this interface may be
 *  called by a remote machine when connected to the Target via ethernet cable.
 *  These commands must be sent over the internet using the HTTP protocol.
 *
 *  Each command in this interface corresponds to an operation that can be
 *  performed by the DSP.  The following CGI commands are supported by the
 *  LogServerCgi module:
 *  
 *  @p(code)
 *  capturelogs.cgi     // capture log data for a set of BIOS logs.
 *  disablelogging.cgi  // disable a set of BIOS logs.
 *  enablelogging.cgi   // enable a set of BIOS logs.
 *  getcpuspeed.cgi     // get the speed of the DSP.
 *  logflush.cgi        // flush and discard data for a set of BIOS logs.
 *  readword.cgi        // read a word of data from Target memory.
 *  settrackaddrs.cgi   // set addresses for which the LogTrack module will
 *                      // print values for.
 *  stopstream.cgi      // stop streaming of BIOS log data.
 *  streamlogs.cgi      // stream log data for a set of BIOS logs.
 *  writeword.cgi       // write a word of data to Target memory.
 *  @p
 *  
 *  Each of the above CGIs corresponds to a C function, which is called in its
 *  own thread context.  This thread will be started with the default priority
 *  of OS_TASKPRINORM, as defined in the NDK.  Each CGI function will run to
 *  completion, at which point the thread will terminate.
 *  
 *  If the user desires, some properties of these CGI threads can be configured
 *  using the C structure LogServerCgi_Config.  This structure allows
 *  the user to change the default CGI thread priority, the size of the log data
 *  buffer (the buffer used to copy log data from the target in order to send it
 *  to the Host), as well as the memory segment from which to allocate this
 *  buffer.  The structure is defined as follows:
 *  
 *  @p(code)
 *  typedef struct LogServerCgi_Config {
 *      Uint32 priority; 
 *      Int segId;
 *      Uint32 bufSize;
 *  } LogServerCgi_Config;
 *  @p
 *  
 *  @p(blist)
 *  priority - used to change the priority which CGI threads will run at.  The
 *            setting of CGI thread priority using this variable will result in
 *            all CGI threads in the program running at that priority level.
 *            However, the user is limited to the values in the range
 *            [0 - OS_TASKPRINORM].
 *            The following rules apply when setting the CGI thread priority:
 *
 *   @p(html)
 *    <ol>
 *           <li>priority set to a number in the range [0 - OS_TASKPRINORM] -
 *               All CGI threads in the program will run at the value specified.
 *               </li>
 *           <li>priority set to a negative number - All CGI threads will run at
 *               the default priority of OS_TASKPRINORM.</li>
 *           <li>priority  set to a value greater than OS_TASKPRINORM - All CGI
 *               threads will run at default priority of OS_TASKPRINORM.</li>
 *    </ol>
 *   @p
 *  @p
 *  @p(blist)
 *  segId     - Used to change the memory segment in which the data buffer used
 *              for copying log data is allocated from.
 *  @p
 *  @p(blist)
 *  bufSize   - Used to change the size of the data buffer used for copying log
 *              data. 
 *  @p
 *
 *  If the user wishes to use this structure to configure these settings, then
 *  it is necessary to define an instance of this type named LogServerCgi_config
 *  in the user's code.
 *  
 *  For example, in order to change the CGI thread priority to 1 (for all CGIs),
 *  change the memory segment to allocate the transfer buffer from to segment 0,
 *  and to change the size of the transfer buffer to 4096, the following code
 *  should be added to the user's program:
 *  
 *  @p(code)
 *  LogServerCgi_Config  LogServerCgi_config =
 *          {1, 
 *          0,
 *          4096,
 *          };
 *  @p
 *  Important note: Re-defining the above mentioned config structure works by
 *  using a linker ordering trick.  Since the user program containing the above
 *  redefinition is linked after the logservercgi library, the latest definition
 *  will override the one in the logservercgi library.  That being said, it is
 *  important that the user's redefined config definition be linked after the
 *  logservercgi library, or any other file that may redefine the
 *  LogServerCgi_config struct!
 */  
metaonly module LogServerCgi {
}

/*
 *  LogServerCgi Configuration interface
 */
/*
 *  @(#) ti.bios.log.ndk; 1, 0, 2, 02,31; 5-23-2009 17:29:07; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

