/*
 *  Copyright 2010 by Texas Instruments Incorporated.
 *  @(#) DSP/BIOS_Kernel 5,2,5,28 02-10-2010 (cuda-u28)
 */
metaonly module Build {
    string getIncludes( string targetName );
    string getLibPath(  string targetName );

    xdc.bld.Executable.Instance addExecutable( 
        string                   exeName, 
        xdc.bld.ITarget.Module   target, 
        string                   platformName, 
        xdc.bld.Executable.Attrs exeAttrs, 
        string                   tconfScript, 
        string                   genBaseName,
        string                   tconfImportPath, 
        string                   tconfOptions
        );

    void expandLinkTemplate( any outStream, any args ); 

    /* TODO/FIXME: add getTcis() for Bridge */
}
