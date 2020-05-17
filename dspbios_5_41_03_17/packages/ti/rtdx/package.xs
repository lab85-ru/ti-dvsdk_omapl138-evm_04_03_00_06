/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== package.xs ========
 *
 */

/*
 *  ======== getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 *
 *  The 'this' object is the current package object being included by
 *  the program being configured.  Thus, it is possible to
 *  conditionally return library names based on package attributes
 *  including the program object itself (this.prog).
 *
 *  Parameters
 *      this            - the current package object
 *      this.prog       - the program being configured (after all of the
 *                        program's configuration scripts complete)
 *      this.prog.build - program attributes specified in the build script
 *
 *  Returns
 *      null        - indicates that no library is to be linked
 *      non-null    - name of the library to link with (relative to the
 *                    package directory).
 */
function getLibs()
{
    var basename, lib, cfgScript;
    var profile;

    cfgScript = Program.build.cfgScript;

    if (this.RtdxModule.overrideLegacyLib == false) {
        /*
         * if the cfgScript is a tconf script, then BIOS/tconf will provide the
         * rtdx library in the generated <prog>cfg.cmd file.
         * 
         * we should probably verify that BIOS/tcf specifies same RTDX library
         * as RTDX configuration specifies, but this would require using 
         * undocumented/supported $$legacy internal function.
         */
        if (cfgScript.indexOf(".tcf", cfgScript.length - 4) > 0) {
            return (null);
        }
    }

    /* library is in current package */
    if (this.RtdxModule.protocol == this.RtdxModule.PROTOCOL_SIMULATION) {
        basename = "rtdxsim";
    }
    else if (this.RtdxModule.protocol == this.RtdxModule.PROTOCOL_JTAG) {
        basename = "rtdx";
    }
    else if (this.RtdxModule.protocol == this.RtdxModule.PROTOCOL_HIGH_SPEED) {
        basename = "rtdxhs";
    }
    else  {
        basename = "rtdx";
    }

    if (this.RtdxModule.noAutoInit == true) {
        basename += "_noInit";
    }

    if (Program.build.profile == "debug") {
        profile = "debug"; 
    }
    else {
        profile = "release"; 
    }

    var dir = "lib/" + profile + "/";

    if (this.RtdxModule.protocol == this.RtdxModule.PROTOCOL_HIGH_SPEED) {
        if (Program.build.target.suffix == "64") {
            lib = dir + basename + ".a62";
        }
        else {
            lib = dir + basename + ".a" + Program.build.target.suffix;
        }
    }
    else {
        if (Program.build.target.suffix == "470") {
            if (Program.build.target.model.endian == "little") {
                lib = dir + basename + "470_arm7_9_32_le.a470";
            }
            else {
                lib = dir + basename + "470_arm7_9_32_be.a470e";
            }
        }
        else {
            lib = dir + basename + ".a" + Program.build.target.suffix;
        }
    }

    return (lib);
}

/*
 *  ======== getSects ========
 *  This method is called during generation (after a program's
 *  configuration script completes) and is responsible for returning a
 *  template file name that is used to generate package-specific
 *  linker command file statements.
 *
 *  Context:
 *          this    the current package's xdc.IPackage.Module object
 *                  after the program's configuration script has completed.
 *
 *          prog    Configuration model program object (xdc.Program) after
 *                  program's configuration script has completed.
 *
 *  Params:
 *          void
 *
 *  Returns
 *          string  path name of template file.  The path name is relative
 *                  to the package path.
 *
 *                  If null, no data is to be generated.
 *
 *                  the template is evaluated in a context where the this
 *                  pointer is the package object.
 *
 *  Throws
 *          exceptions thrown are for fatal errors
 */
function getSects()
{
    var cfgScript = Program.build.cfgScript;

    /*
     * if the cfgScript is a tconf script, then BIOS/tconf will provide the
     * rtdx library in the generated <prog>cfg.cmd file.  We need to
     * include the rtdx.xdt linker template which provides path to the
     * rtdx installation so that the linker can find the library.
     */
    if (cfgScript.indexOf(".tcf", cfgScript.length - 4) > 0) {
        return ("ti/rtdx/rtdx.xdt");
    }

    return (null);
}

function close()
{
  var RtdxModule = xdc.module('ti.rtdx.RtdxModule');

  if(RtdxModule.$used) {    
    if (Program.sectMap[".rtdx_data"] == undefined) {
        Program.sectMap[".rtdx_data"] = 
            {loadSegment: Program.platform.dataMemory}; 
    } 

    if (Program.sectMap[".rtdx_text"] == undefined) {
        Program.sectMap[".rtdx_text"] = 
            {loadSegment: Program.platform.codeMemory}; 
    }
  }
}

/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:37; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

