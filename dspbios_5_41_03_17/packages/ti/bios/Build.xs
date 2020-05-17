/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
function getIncludes( targetName ) {
    // fixme! should not require -I!
    return " -I" + xdc.getPackageBase( "ti.bios" ) + "include ";
}

function getLibPath( targetName ) {
    return xdc.getPackageBase( "ti.bios" ) + "lib";
}


function addExecutable( exeName, target, platformName, exeAttrs,
    tconfScript, genBaseName, tconfImportPath, tconfOptions )
{
    var printd = 0 ? print : function(){};
    var xdcRoot = environment[ "xdc.root" ];
    
    var progDir = exeName.match( "/" ) == null ? "." : 
                  exeName.replace( /\/[^\/]+$/, "" );
    printd( "Build.xs-> progDir = " + progDir ); 

    var suffix = target.suffix.substring( 0,2 );
    switch (suffix) {
        case "64": 
            suffix = "62"; 
            break;
        default:
            break;
    }
        
    var generatedBiosSources = [
        progDir + "/" + genBaseName + "cfg_c.c",
        progDir + "/" + genBaseName + "cfg.s" + suffix ];
        
    var actualExeAttrs = {};
    for (var i in exeAttrs) {
        actualExeAttrs[i] = exeAttrs[i];
    }
    
    var userLinkTemplate = actualExeAttrs.linkTemplate;
    printd( "Build.xs-> userLinkTemplate = " + userLinkTemplate ); 
    
    if (userLinkTemplate == null) {
        actualExeAttrs.linkTemplate = 
            (this.$package.$name).replace(/\./g, "/") + 
	    "/xdcbuild/topLinkCmd.xdt";
    }
        
    /* add -I<progDir> to incs */
    if (actualExeAttrs.incs == undefined) {
        actualExeAttrs.incs = "";
    }
    actualExeAttrs.incs += "-I" + progDir;    
        
    /* add two arguments to the cfgArgs field: */
    var cfgArgs = 
        actualExeAttrs.cfgArgs != undefined ? actualExeAttrs.cfgArgs : "{}";
    //fixme: will fail if there is a trailing , before } in the original    
    actualExeAttrs.cfgArgs = 
        cfgArgs.replace( /\}s*$/, 
            ( cfgArgs.match( ":" ) != null ? ", " : "" )       +
            " genBiosCmdFile:  '" + progDir + "/" + genBaseName + "cfg.cmd', " +
            " userLinkTemplate:'" + userLinkTemplate + "', "   +
            " }" ); 
    printd( "Build.xs-> cfgArgs = \n" + actualExeAttrs.cfgArgs + "\n" ); 
    
    var exe = Pkg.addExecutable( exeName, target, platformName, actualExeAttrs);
    exe.addObjects( generatedBiosSources );

    // and now add makefile rules to generate BIOS files:
    var rules = "";
    var generatedHeaderFile = progDir + "/" + genBaseName + "cfg.h";
    
    tconfOptions += " -Dconfig.programName=" + progDir + "/" + genBaseName;    

    // add rule that runs tconf on the generated header file        
    rules +=
        generatedHeaderFile + ": " + tconfScript + "\n" +
        "\t" + "@$(ECHO) running TCONF on " + tconfScript + "\n" +
        "\t" + xdcRoot + "/tconf " + 
        "-Dconfig.tiRoot=" + xdcRoot + " " + 
        "-Dconfig.importPath=\"" + 
             ".;" + 
             tconfImportPath + ";" +
             xdc.curPath() +
             "\" " +
        tconfOptions + " " +
        "-Dconfig.userScriptName=" + tconfScript + " " +
        xdc.getPackageBase( "ti.bios" ) + "xdcbuild/tconfEntryScript.tci " + 
        "\n";
        
    // the rest of generated files depend on the first, so parallel make is ok
    for (var i = 0; i < generatedBiosSources.length; i++) {
        rules +=
            generatedBiosSources[i] + ": " + generatedHeaderFile + "\n";
    }
    
    rules += "ifneq (clean, $(MAKECMDGOALS)) \n" +
             "-include " + progDir + "/" + genBaseName + "cfg.dep \n" +
             "endif \n" 
    
    rules += ".interfaces : " + progDir + "/" + genBaseName + "cfg.h" + "\n";
    
    rules += "clean::\n" +
             "\t rm -f " + progDir + "/" + genBaseName + ".cdb" + "\n" +
             "\t rm -f " + progDir + "/" + genBaseName + "cfg.h" + "\n" +
             "\t rm -f " + progDir + "/" + genBaseName + "cfg_c.c" + "\n" +
             "\t rm -f " + progDir + "/" + genBaseName + "cfg.[hs]" + 
			   target.suffix.substring( 0,2 ) + "\n" +
             "\t rm -f " + progDir + "/" + genBaseName + "cfg.cmd" + "\n" +
             "\t rm -f " + progDir + "/" + genBaseName + "cfg.dep" + "\n";
    Pkg.makeEpilogue += rules;
    
    return exe;
}

function expandLinkTemplate( out, args ) {
    var thisPkgDir = this.$package.$name.replace( /\./g, "/" );
    var biosTemplate = xdc.loadTemplate( 
		       thisPkgDir + "/xdcbuild/biosLinkCmd.xdt" );
    biosTemplate.genStream( out, this.$package, args );
}
		   
