/** ============================================================================
 *  @file   package.xs
 *
 *  @path   $(DSPLINK)/gpp/
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


function calcLibPath(program, Global, profile)
{
    var suffix;
    var device = program.cpu.deviceName;

    // XDCtools 3.10 has new findSuffix() function. Must pass in suffix array
    // because dsplink libraries are built with makefile instead of XDCtools.
    if ("findSuffix" in prog.build.target) {
        suffix = prog.build.target.findSuffix(["470MV", "470uC", "v4TCE"]);
        if (suffix == null) {
            return "";  // nothing to contribute
        }
    }
    else {
        suffix = prog.build.target.suffix;
    }


    if (Global.runtimeOS == Global.LINUX) {
        var osDir;

        /* when building for evmDM6446 ... */
        if (device.match(/TMS320CDM6446/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/DAVINCI";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/DAVINCI";
            }
            else {
                return null; /* unsupported target */
            }
        }

        /* else when building for evmDM6467 ... */
        else if (device.match(/TMS320CDM6467/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/DAVINCIHD";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/DAVINCIHD";
            }
            else {
                return null; /* unsupported target */
            }
        }
        /* else when building for 3430 ... */
        else if (device.match(/TMS320C3430|OMAP3530/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/OMAP3530";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/OMAP3530";
            }
            else {
                return null; /* unsupported target */
            }
        }
        else if (device.match(/TMS320C2430|OMAP2530/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/OMAP2530";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/OMAP2530";
            }
            else {
                return null; /* unsupported target */
            }
        }

     /* else when building for evmDM357 ... */
        else if (device.match(/TMS320DM357/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/DM357";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/DM357";
            }
            else {
                return null; /* unsupported target */
            }
        }

       else if (device.match(/TMS320DA8xx|TMS320DA805|TMS320DA807|TMS320DA808|TMS320DA810|TMS320DA828|TMS320DA830/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/DA8XX";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/DA8XX";
            }
            else {
                return null; /* unsupported target */
            }
        }
        else if (device.match(/OMAPL137/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/OMAPL1XX";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/OMAPL1XX";
            }
            else {
                return null; /* unsupported target */
            }
        }
	/* else when building for evmTNETV107X... */
        else if (device.match(/TMS320CTNETV107X|TNETV107X/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/LEO";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/LEO";
            }
            else {
                return null; /* unsupported target */
            }
        }

       else if (device.match(/TMS320DA850/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/DA850";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/DA850";
            }
            else {
                return null; /* unsupported target */
            }
        }
        else if (device.match(/OMAPL138/)) {
            if (suffix == "470MV") {
                osDir = "export/BIN/Linux/OMAPL138";
            }
            else if (suffix == "470uC") {
                osDir = "export/BIN/ucLinux/OMAPL138";
            }
            else {
                return null; /* unsupported target */
            }
        }

        return osDir + "/" + (profile == "debug" ? "DEBUG" : "RELEASE" );
    }
    else if (Global.runtimeOS == Global.WINCE) {
        var osDir;

        /* when building for evmDM6446 ... */
        if (device.match(/TMS320CDM6446/)) {
            osDir = "export/BIN/WinCE/DAVINCI";
        }
        /* else when building for 3430 ... */
        else if (device.match(/TMS320C3430|OMAP3530/)) {
            if (suffix == "v4TCE") {
                osDir = "export/BIN/WinCE/OMAP3530";
            }
            else {
                return null; /* unsupported target */
            }
        }
		else if (device.match(/TMS320CTNETV107X|TNETV107X/)) {
            if (suffix == "v4TCE") {
                osDir = "export/BIN/WinCE/LEO";
            }
            else {
                return null; /* unsupported target */
            }
        }

        osDir += "/" + (profile == "debug" ? "DEBUG" : "RELEASE" );

        return (osDir);
    }
}

function getLibs(prog)
{
    var libPath  = calcLibPath(prog, this.Global, this.profile);

    if (libPath == null) {
        return null;
    }
    else {
        if (this.Global.runtimeOS == this.Global.WINCE) {
            var libNames = [ "dsplinkapi" ];
        }
        else {
        	var libNames = [ "dsplink" ];
        }

        var libs = "";
        for (var i = 0; i < libNames.length; i++) {
            libs += libPath + "/" + libNames[i] + ".lib";
            if (i < libNames.length - 1) {
                libs += ";";
            }
        }
        print("    will link with " + this.$name + ":" + libs);
        return libs;
    }
}
