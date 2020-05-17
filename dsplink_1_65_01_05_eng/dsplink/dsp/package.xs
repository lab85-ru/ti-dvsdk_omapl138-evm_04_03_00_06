/** ============================================================================
 *  @file   package.xs
 *
 *  @path   $(DSPLINK)/dsp/
 *
 *  @desc   RTSC specific file
 *
 *  @ver    1.65.01.05_eng
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */



function calcLibPath( program, profile )
{
    var platDir;
    var device = program.cpu.deviceName;

    // XDCtools 3.10 has new findSuffix() function. Must pass in suffix array
    // because dsplink libraries are built with makefile instead of XDCtools.
    if ("findSuffix" in prog.build.target) {
        var suffix = prog.build.target.findSuffix(["64P", "674"]);
        if (suffix == null) {
            return "";  // nothing to contribute
        }
    }

    /* assume we are using a 64P target, suffix not actually used */
    if (device.match(/TMS320CDM6446/)) {
        platDir = "export/BIN/DspBios/DAVINCI/DM6446GEM_0";
    }
    else if (device.match(/TMS320CDM6467/)) {
        platDir = "export/BIN/DspBios/DAVINCIHD/DM6467GEM_0";
    }
    else if (device.match(/TMS320C3430|OMAP3530/)) {
        platDir = "export/BIN/DspBios/OMAP3530/OMAP3530_0";
    }
    else if (device.match(/TMS320C2430|OMAP2530/)) {
        platDir = "export/BIN/DspBios/OMAP2530/OMAP2530_0";
    }
    else if (device.match(/TMS320DA8xx|TMS320DA805|TMS320DA807|TMS320DA808|TMS320DA810|TMS320DA828|TMS320DA830/)) {
        platDir = "export/BIN/DspBios/DA8XX/DA8XXGEM_0";
    }
    else if (device.match(/OMAPL137/)) {
        platDir = "export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0";
    }
    else if (device.match(/TMS320DM357/)) {
        platDir = "export/BIN/DspBios/DM357/DM357GEM_0";
    }
    else if (device.match(/TNETV107X|TMS320CTNETV107X/)) {
        platDir = "export/BIN/DspBios/LEO/TNETV107XGEM_0";
    }
    else if (device.match(/TMS320DA850/)) {
        platDir = "export/BIN/DspBios/DA850/DA850GEM_0";
    }
    else if (device.match(/OMAPL138/)) {
        platDir = "export/BIN/DspBios/OMAPL138/OMAPL138GEM_0";
    }
    else {
        return null; /* unsupported target */
    }

    return platDir + "/" +
           (profile == "debug" ? "DEBUG" : "RELEASE" );
}

function getLibs() {

    var libPath  = calcLibPath (Program, this.profile);
    var libs     = "" ;
    var numLibs  = 0;

    if (this.Global.useProc == 1) {
        numLibs++;
        libs += libPath + "/" + "dsplink.lib";
    }
    if (this.Global.useMsgq == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkmsg.lib";
    }
    if (this.Global.useChnl == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkdata.lib";
    }
    if (this.Global.useNotify == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinknotify.lib";
    }
    if (this.Global.useMpcs == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkmpcs.lib";
    }
    if (this.Global.useMplist == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkmplist.lib";
    }
    if (this.Global.usePool == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkpool.lib";
    }
    if (this.Global.useRingio == 1) {
        if (numLibs++ > 0) {
            libs += ";";
        }
        libs += libPath + "/" + "dsplinkringio.lib";
    }

    return libs;
}


