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

    /*
     *  verify that user didn't use both
     * 
     *     utils.importFile(Load.tci) AND xdc.useModule(Load)
     * 
     *  (fix for CQ18864)
     */

    // create a pattern for matching Load.tci in all loaded files
    var pattern = /Load\.tci/g;

    //check if Load.tci has been loaded
    for (var x in utils.loadedFiles) {

        if (pattern.test(x)) {

            throw new Error("Illegal attempt to use both \"utils.importFile(\'"
                    + x + "\')\" and \"xdc.useModule(\'" + this
                    + ".Load\')\" in the same program build.");

            break;
        }
    }

    if (xdc.om['ti.bios.utils'].Load.$used > 0) {
        /* create an IDL object needed for Load_getcpuload() to work properly */
        var Load_cpuloadIdlObj =
                $$legacy.prog.module("IDL").create("Load_cpuloadIdlObj");
        Load_cpuloadIdlObj.fxn = $$legacy.prog.extern("Load_idlefxn");
        Load_cpuloadIdlObj.calibration = true;

        /* Must call prog.gen() to recreate the configuration model. */
        $$legacy.prog.gen();
    }
}

function getLibs(prog)
{
    /* "mangle" program build attrs into an appropriate directory name */
    var name = "lib/utils";

    /* return the library name: name.a<arch> */
    var lib = name + ".a" + prog.build.target.suffix;
    print("    will link with " + this.$name + ":" + lib);

    return (lib);
}
/*
 *  @(#) ti.bios.utils; 2, 0, 2, 02,31; 5-23-2009 17:29:34; /db/vtree/library/trees/biosutils/biosutils-c02x/src/
 */

