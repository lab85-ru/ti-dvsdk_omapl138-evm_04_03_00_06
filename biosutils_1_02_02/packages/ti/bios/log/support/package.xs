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
 *  ======== close ========
 *  Close this package; optionally (re)set other config parameters in the
 *  model
 */
function close()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }

    var LogTrack = xdc.useModule('ti.bios.log.support.LogTrack');

    /* special log for tracking dynamic TSK creation */
    var createLog = $$legacy.prog.module("LOG").create( "ti_bios_log_create" );
    createLog.comment  = "This log tracks dynamic TSK creation";
    createLog.bufLen   = LogTrack.createLogSize;
    createLog.dataType = "raw data";

    /* special log for tracking CPU load and user values */

    var trackLog = $$legacy.prog.module("LOG").create( "ti_bios_log_track" );
    trackLog.comment  = "This log tracks CPU load and user values";
    trackLog.bufLen   = LogTrack.trackLogSize;
    trackLog.dataType = "raw data"

    /* create a PRD which will print CPU load and user value data to the track log */
    var trackPrd = $$legacy.prog.module("PRD").create("ti_bios_log_trackPrd");
    trackPrd.period = LogTrack.trackPeriod;
    trackPrd.fxn = $$legacy.prog.extern("LogTrack_prdFxn");

    /* Initialize the create hook so we can track dynamic tasks */
    $$legacy.prog.module("TSK").CREATEFXN =
            $$legacy.prog.extern("LogTrack_createHook");

    /*
     *  Use time-stamp LOG for DVT
     */
    $$legacy.prog.module("LOG").TS = true;

    // Must call prog.gen() to recreate the configuration model.
    $$legacy.prog.gen();
}

/*
 *  ======== getLibs ========
 */
function getLibs() {
    /* "mangle" program build attrs into an appropriate directory name */
    var name = "lib/logsupport";

    /* return the library name: name.a<arch> */
    var lib = name + ".a" + prog.build.target.suffix;
    print("    will link with " + this.$name + ":" + lib);

    return (lib);
}
/*
 *  @(#) ti.bios.log.support; 1, 0, 2, 02,31; 5-23-2009 17:29:23; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

