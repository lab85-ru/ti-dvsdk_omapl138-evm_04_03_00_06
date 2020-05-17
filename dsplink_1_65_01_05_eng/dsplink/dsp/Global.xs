/** ============================================================================
 *  @file   Global.xs
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


var API_INCLUDES = "DRV_includes.txt";
var API_DEFINES  = "DRV_defines.txt";
var API_LINKPATH = "DSPLINK.txt";

var debug = function() {}

/*
 * ======== getIncludes ========
 */
function getIncludes()
{
    var locationOfBuildTxtFiles;
   
    if (this.deviceName == this.OMAP3530) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/OMAP3530/OMAP3530_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAP2530) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/OMAP2530/OMAP2530_0/RELEASE/";
    }

    else if (this.deviceName == this.DM6446GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/DAVINCI/DM6446GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.DM6467GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/DAVINCIHD/DM6467GEM_0/RELEASE/";
    }
    else if (this.deviceName == this.DM357GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/DM357/DM357GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.DA8XXGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/DA8XX/DA8XXGEM_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAPL1XXGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/RELEASE/";
    }
    else if (this.deviceName == this.TNETV107XGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/LEO/TNETV107XGEM_0/RELEASE/";
    }
    else if (this.deviceName == this.DA850GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/DA850/DA850GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAPL138GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                "export/BIN/DspBios/OMAPL138/OMAPL138GEM_0/RELEASE/";
    }


    else {
        throw "\nError: DSP side DeviceName is undefined.\n";
    }


    // get orig link build path (func defined below)
    var originalLinkPath = getTextFileContents(
                           locationOfBuildTxtFiles + API_LINKPATH );

    // get orig includes in form -I<dsplink>/this -I<dsplink>/that
    var originalIncludes = getTextFileContents(
                           locationOfBuildTxtFiles + API_INCLUDES );

    var currentLinkPath =  new java.io.File(
                           xdc.getPackageBase( this.$package.$name ) + ".." ).
                           getCanonicalPath();

    var includes = "";
    var incs = originalIncludes.split( "-I" );
    for (var i = 0; i < incs.length; i++) {
        if (incs[i].indexOf( originalLinkPath ) == 0) {
            includes += " -I" +
                        incs[i].replace( originalLinkPath, currentLinkPath );
        }
    }
    includes += " ";
    includes = includes.replace( /\\/g, "/" );

    debug( "orig link build path = " + originalLinkPath );
    debug( "currentLinkPath = " + currentLinkPath );
    debug( "orig includes = " + originalIncludes + originalIncludes.replace(/-I/g,"\n-I"));
    debug( "includes = " + includes + includes.replace(/-I/g,"\n-I") );

    return includes;
}

/*
 * ======== getDefines ========
 */
function getDefines()
{
    var locationOfBuildTxtFiles;
    
    if (this.deviceName == this.OMAP3530) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/OMAP3530/OMAP3530_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAP2530) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/OMAP2530/OMAP2530_0/RELEASE/";
    }

    else if (this.deviceName == this.DM6446GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/DAVINCI/DM6446GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.DM6467GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/DAVINCIHD/DM6467GEM_0/RELEASE/";
    }


    else if (this.deviceName == this.DM357GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                   "export/BIN/DspBios/DM357/DM357GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.DA8XXGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/DA8XX/DA8XXGEM_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAPL1XXGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/RELEASE/";
    }
	else if (this.deviceName == this.TNETV107XGEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/LEO/TNETV107XGEM_0/RELEASE/";
    }    

    else if (this.deviceName == this.DA850GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/DA850/DA850GEM_0/RELEASE/";
    }

    else if (this.deviceName == this.OMAPL138GEM) {
        locationOfBuildTxtFiles = xdc.getPackageBase( this.$package.$name ) +
                                  "export/BIN/DspBios/OMAPL138/OMAPL138GEM_0/RELEASE/";
    }

    else {
        throw "\nError: DSP side DeviceName is undefined.\n";
    }

    var defines = getTextFileContents(
                  locationOfBuildTxtFiles + API_DEFINES ) + " ";

    return defines;
}

/*
 * ======== getTextFileContents ========
 * local utility function
 */
function getTextFileContents( filename )
{
    var contents = "";
    try {
        var file  = new java.io.FileReader( filename );
        var lfile = new java.io.LineNumberReader( file );
        var line;
        while ((line = lfile.readLine()) != null) {
            contents += "" + line;
        }
        lfile.close();
        file.close();
    } catch(e) {
        throw "\nError: cannot open file " + filename + " for reading.\n";
    }
    return contents;
}
