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
 *  ======== LogTrack.xdc ========
 *  LogTrack Configuration interface
 */

/*!
 *  ======== LogTrack ========
 *  LogTrack module.
 *  
 *  The LogTrack module is used for tracking values which include CPU load, user
 *  defined values, and dynamic TSK creation.  The LogTrack module "tracks," or
 *  prints out information about these values to the two special BIOS logs
 *  "ti_bios_log_track" and "ti_bios_log_create".
 *  
 *  To use the LogTrack module, import the configuration code from one of the
 *  following two files, depending on the build type (Tconf or RTSC) being used
 *  for the user's program:
 * 
 *     a. ti/bios/log/support/LogTrack.tci
 * 
 *         If the user app is built using Tconf, then include this file into
 *         the build by adding the following statement into the *.tcf file:
 *     
 *  @p(code)
 *         utils.importFile('LogTrack.tci');
 *  @p
 *
 *     b. ti/bios/log/support/package.xs
 *
 *         If the user app is built using RTSC, then the user must use the Load
 *         module. This is done by adding the following line to the *.cfg file:
 *     
 *  @p(code)
 *         var LogTrack = xdc.useModule('ti.bios.log.support.LogTrack');
 *  @p
 *     
 *     NOTE: The two above mentioned methods are mutually exclusive.  One can't
 *           use both method 'a' and method 'b' in the same program build; the
 *           user must choose the appropriate one for their build.
 * 
 *  
 *  Additionally, it's necessary to call the module's init function, which
 *  should be called during program startup:
 *  @p(code)
 *  LogTrack_init()
 *  @p
 *  The LogTrack module's init function could be called from the user's main()
 *  function, for example.
 *
 *  By default, a BIOS PRD object (which runs every 10ms) prints out the CPU
 *  load of the DSP, followed by four zero values.  These values are printed to
 *  the BIOS log "ti_bios_log_track".
 *  Also, the user may change these four values that the LogTrack module prints.
 *  This may be done by setting the address locations of those values using the 
 *  following API: 
 *  @p(code)
 *  extern Void LogTrack_setTrackAddrs(Uint32 *addr1, Uint32 *addr2, Uint32
 *          *addr3, Uint32 *addr4);
 *  @p
 *  
 *  In order for an address to be valid, it must be a memory mapped address.  It
 *  is required that all four arguments to this API be filled; however if the
 *  user does not wish to print four different address values, then the address
 *  0x0 may be supplied as a "don't care" value.  This API will print the value
 *  stored in each address, unless it is address 0x0, in which case it will
 *  print "0".
 *
 *  The LogTrack module also contains a hook function to print information about
 *  dynamically created BIOS tasks (TSK).  Each time a BIOS TSK instance is
 *  created at run time, its information is printed to the "ti_bios_log_create"
 *  BIOS log.
 *
 */

metaonly module LogTrack {
    config Int createLogSize = 1024; /*! size of the "ti_bios_log_create" log */
    config Int trackLogSize = 4096;  /*! size of the "ti_bios_log_track" log */
    config Int trackPeriod = 10; /*! period of the PRD which prints out CPU load and the user defined addresses in the LogTrack_Config structure. */

}

/*
 *!
 */
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:23; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

