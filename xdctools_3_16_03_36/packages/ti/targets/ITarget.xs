/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * --/COPYRIGHT--*/
/*
 *  ======== ITarget.xs ========
 *
 *! Revision History
 *! ================
 *! 07-May-2008 sasha	rearrangement of whole_program profiles (21494, 25333)
 *! 08-Apr-2008 sasha	removed workaround for bugzilla 795 (old 54 tools)
 *! 13-Jun-2007 kw	use -op2 for Arm
 *! 14-Nov-2006 sasha	replaced -mv5e with -mv4 for TMS470 targets
 *! 01-Nov-2006 sasha	added "-o3" back to alchemy options for arm targets
 *! 04-Oct-2005 sasha	added workaround for bugzilla 795
 *! 18-Aug-2005 sasha	If an alchemy profile has linkOpts defined, then
 *!			nothing is added to the link command line in link()
 *! 16-Aug-2005	sasha	Removed the check for the alchemy target from link()
 *!
 */

var debug;
if (debug == null) {
    debug = function (msg) {};
}

var _bldUtils = xdc.useModule('xdc.bld.Utils');

/*
 *  ======== _isaChainMap ========
 *  This map defines an "is a " releationship between ISAs (Instruction
 *  Set Architectures).  This table controls both the names of generated
 *  assembly language files (as part of the configuration model) and the
 *  searching of assembly language sources files.
 *
 *  Each chain is an array of other ISAs that are "source-code compatible"
 *  with the specified ISA.  The 64P ISA chain below is defined to as
 *  ["62", "64", "64P"] which indicates that .s62, .s64, and .s64P assembly
 *  language files may be recompiled to run on a 64P device.
 */
var _isaChainMap = {
    isa_62:  ["62"],
    isa_64:  ["62", "64"],
    isa_64P: ["62", "64", "64P"],
    isa_674: ["62", "64", "64P", "674"],
    isa_67:  ["62", "67"],
    isa_67P: ["62", "67", "67P"],
    isa_64T: ["64T"],
    isa_28:  ["28"],
    isa_28FP:["28", "28FP"],
    isa_430: ["430"],
    isa_470: ["470", "v4T"],
    isa_v4T: ["470", "v4T"],
    isa_v5T: ["470", "v4T", "v5T"],
    isa_v6:  ["470", "v4T", "v5T", "v6"],
    isa_v7M: ["v7M"],
    isa_v7R: ["470", "v7R"],
    isa_54:  ["54"],
    isa_55:  ["55"],
    isa_55P: ["55", "55P"]
};

/*
 *  ======== _targetCmds ========
 *  Hash table of commands (indexed by target.suffix).  Commands are
 *  themselves hashtables (indexed by file type: "asm", "c", ...) that
 *  define the commands for cc, asm , etc.
 */
var _targetCmds = null;

/*
 *  ======== _targetEnvs ========
 *  Hash table of path settings for each target (indexed by target.suffix).
 */
var _targetEnvs = {};

/*
 *  ======== _thisPkg ========
 *  This variable is initialized by this package's init method to the
 *  package containing this file.  We use this in lieu of hard coding
 *  this package's name in this file.
 */
var _thisPkg = null;

/*
 *  ======== _versionCache ========
 *  A cache of version results to avoid calling the same compiler more
 *  than once to get a version string.
 */
var _versionCache = {};

/*
 *  ======== _noCompressTargets ========
 *  List of targets for which we use --no_compress option.
 */
var _noCompressTargets = {C64P: 1, C64P_big_endian: 1, C674: 1, C674_big_endian: 1,
    C64T_big_endian: 1, C64T: 1};

/*
 *  ======== ITarget.archive ========
 */
function archive(goal)
{
    var target = this;

    _initTabs();

    var result = null;
    var tool2cmd = _targetCmds[target.suffix];

    if (tool2cmd != null) {
	result = new xdc.om['xdc.bld.ITarget'].CommandSet;
	result.msg = "archiving " + goal.files + " into $@ ...";
	_setEnv(target, result);

	/*  HACK: because ar500 gives "phase" errors on "ar r" of long names
	 *  we may need to forcefully remove the archive and rebuild.  This
	 *  can be done here by adding the string "$(RM) $@\n" to the
	 *  beginning of result.cmds; we don't do this because we already
	 *  delete goals before making them in xdc/bld/package.xs.
	 */
	result.cmds = _bldUtils.expandString(tool2cmd["ar"], {
	    AROPTS_P:	target.arOpts.prefix,
	    AROPTS_S:	target.arOpts.suffix,
	    aropts:	goal.opts,
	    files:	goal.files
	});
    }

    return (result);
}

/*
 *  ======== ITarget.asmName ========
 */
function asmName(cname)
{
    return ('_' + cname);
}

/*
 *  ======== ITarget.compile ========
 */
function compile(goal) {
    return (_compile(this, goal, false));
}

/*
 *  ======== ITarget.getVersion ========
 */
function getVersion()
{
    var target = this;
    _initTabs();

    var result = target.$name + "{";

    /* call compiler to get its version number and append to result */
    var tool2cmd = _targetCmds[target.suffix];
    if (tool2cmd != null) {
	var cmd = tool2cmd["vers"].replace("\$(rootDir)", target.rootDir);

	/* Check if rootDir is set correctly */
        var file = new java.io.File(target.rootDir);
        if (!file.exists()) {
            throw new Error(target.rootDir + " cannot be found. Ensure that" +
                " rootDir for the " + target.name + " target is set " +
                "correctly in " + java.lang.System.getenv("XDCBUILDCFG"));
        }

	var envs = {};
	_setEnv(target, envs);

	/* if we already know the result, return it now */
	var tmp;
	var verKey = cmd + envs.path;
	if ((tmp = _versionCache[verKey]) != null) {
	    return (result + tmp.split("{")[1]);
	}

	/* otherwise, run compiler to get version string output */
	var path = "PATH=" + envs.path.join(";");
	path = path.replace("$(" + target.$name + ".rootDir)",
	    target.rootDir, "g");
	if (environment["xdc.hostOS"] != "Windows") {
	    path = path.replace(/;/g, ":");
	}
	var attrs = {envs: [path]};

	var status = {};
	if (xdc.exec(cmd, attrs, status) >= 0) {
	    var comp = status.output.match(/^\w+/);
	    comp = comp == null ? "" : comp[0];

	    /* extract compiler version number from compiler output */
    	    var cver;
	    var va = status.output.match(/Version\s+[0-9\.]+/mg);
	    if (va != null) {	    /* old style version string */
		cver = va[0].replace(/[Version\s]/g, "");
	    }
	    else {
		va = status.output.match(/\s+v([0-9\.]+)([IPAB]([0-9]+)){0,1}/mg);
		if (va != null) {   /* new style version string */
		    cver = va[0].replace(/\s+v/, "");
		}
		else {
		    print(target.$name + ": warning: version match failed on '"
			+ status.output + "'");
		    cver = "0.0.0";
		}
	    }

	    /* get user-specified key mapping */
	    var key = target.versionMap[comp + "_" + cver];

	    /* if not specified, generate a default mapping */
	    if (key == null) {
//		print(target.$name +" can't find '" + comp + "_" + cver + "'");
		va = cver.split('.');
		var radius = va[0] + '.' + va[1];   /* Major.Minor */
		var patch = 0;			    /* Update[.W] */
		var suffix;			    /* [,date] */
		if (va.length == 3) {	    /* new style (without W) */
		    va = va[2].match(/([0-9]+)([IPAB]([0-9]+)){0,1}/);
		    patch = va[1];
		    suffix = va[3];
		}
		else if (va.length == 4) {  /* new style (with W) */
		    var w = va[2];
		    va = va[3].match(/([0-9]+)([IPAB]([0-9]+)){0,1}/);
		    patch = va[1] + '.' + w;
		    suffix = va[3];
		}
		suffix = suffix == null ? "" : (',' + suffix);

		key = "1,0," + radius + "," + patch + suffix;
	    }

	    /* create target compatibility string */
	    result += key;
	}
	else {
	    print(target.$name + ": warning: exec of '" + cmd + "' failed:"
		+ status.output + "; check tools installation");
	}

	/* cache the result so we don't have to run this command again */
	_versionCache[verKey] = result;
    }

    return (result);
}

/*
 *  ======== ITarget.link ========
 */
function link(goal)
{
    var target = this;
    _initTabs();

    var result = null;
    var tool2cmd = _targetCmds[target.suffix];

    if (tool2cmd != null) {
	result = new xdc.om['xdc.bld.ITarget'].CommandSet;
	result.msg = "lnk" + target.suffix + " $@ ...";
	_setEnv(target, result);

	var lib = "";
	if (("linkLib" in target) && target.linkLib != null) {
	    lib = " -l $(rootDir)/lib/" + target.linkLib;
	}

	var cmd = tool2cmd["link"];
	var pre = target.lnkOpts == null ? "" :
	    (goal.dllMode ? "" : target.lnkOpts.prefix);

	if (goal.profile.match(/whole_program/)) {
	    cmd += "$(MV) $(notdir $(basename $@)).asm $(XDCCFGDIR)/$@.opt\n";
	    /* If the linkOpts are set for the 'whole_program' profile or the
	     * 'whole_progam_debug' profile, and these options set compile
	     * options, then an additional set of the compile options should
	     * not be added. This is intended for the internal use only.
	     */
	    if (this.profiles[goal.profile].linkOpts == undefined ||
	        !this.profiles[goal.profile].linkOpts.match(/--opt/)) {
                var allOpts = target.cc.opts + " " + target.ccOpts.prefix
                    + " " + target.ccOpts.suffix
                    + " " + this.profiles[goal.profile].compileOpts.copts
                    + " -op2 -O3 -k -os";

                /* The profile "whole_program" compiles with '-O2' because we
                 * want the standard object code to be optimized at O2 level in
                 * case it is linked without invoking the optimizer from the
                 * linker. However, we already added '-O3' for the case when
                 * the linker invokes the optimizer, so we need to remove '-O2'
                 * to avoid confusion.
                 */
                allOpts = allOpts.replace(" -O2", " ");
                if (goal.profile == "whole_program_debug") {
                    allOpts += " --optimize_with_debug";
                }

                if (target.isa.substring(0, 2) == "28"
                    || target.isa.substring(0, 2) == "55") {
                    /* the newest 5.0.0 compiler is crashing with '-op2' */
                    allOpts = allOpts.replace("-op2", "-op3");
                }

                /* some older compiler did not have -mt and inline_limit,
                 * and don't support --no_compress.
                 */
                var compString = this.getVersion().split('{')[1];
                var compVersion = compString.split(',');
		if (target.name in _noCompressTargets && goal.dllMode) {
		    allOpts += " --no_compress";    
		}

                if ((target.isa.substring(0, 2) == "55" && compVersion[2] < '4')
                    || (target.isa.substring(0, 2) == "54")) {
                    pre += " --opt='" + allOpts + "'";
                }
                else {
                    pre += " --opt='" + allOpts
                        + " --inline_recursion_limit=20'";
                }

                /* When linking assemblies for Arm with 4.5 compiler, the link
                 * lasts for minutes, so we have to add another switch to skip
                 * placement optimization, until the bug is fixed.  Same issue
                 * and workaround also applies to 28x 5.1 compiler.
                 */
                if ( ((target.isa[0] == 'v' || target.isa == "470") &&
			goal.dllMode && compVersion[2] == "4.5") || 
                     ((target.isa.substring(0, 2) == "28") &&
                        goal.dllMode && compVersion[2] == "5.1") )  {
                    pre += " --no_placement_optimization";
                }
	    }
	}

	result.cmds = _bldUtils.expandString(cmd, {
	    LOPTS_P: pre,
	    LOPTS_S: target.lnkOpts == null ? "" :
	        (goal.dllMode ? "-r -m $(XDCCFGDIR)/$@.map" :
	                        target.lnkOpts.suffix + lib),
	    lopts:   goal.opts,
	    files:   goal.files
	});

	if (goal.dllMode) {
	    result.cmds += "$(MV) $@ $(XDCCFGDIR)/$@.obj\n";
	    result.cmds += "$(TOUCH) $@\n";

            /* If we are linking an assembly in the 'whole_program' profile, we
             * need to compile the optimized 'asm/opt' file to get a
	     * relocatable 'lib' file. The existing 'lib' file is a partially
	     * linked file, which has trampolines and is not relocatable.
             */
            if (!goal.isRom && goal.profile.match(/whole_program/)) {
		/* first we need to remove [d] from section names in the opt
		 * files
		 */
		var asmSuffix = ".s" + target.suffix;
		result.cmds += "$(MV) $(XDCCFGDIR)/$@.obj $(XDCCFGDIR)/$@.pobj"
		    + "\n";
            	result.cmds +=
            	    "$(SED) -r 's/sect[ 	]\"\[[0-9]+\]/sect	\"/g' "
            	    + "<$(XDCCFGDIR)/$@.opt >$(XDCCFGDIR)/$@" + asmSuffix + "\n";

            	var newGoal = new xdc.om['xdc.bld.ITarget.CompileGoal'];
            	newGoal.base = "$@";
            	newGoal.srcPrefix = "$(XDCCFGDIR)/";
            	newGoal.srcSuffix = asmSuffix;
            	newGoal.dstPrefix = "$(XDCCFGDIR)/";
            	newGoal.dstSuffix = ".obj";
            	newGoal.opts = new xdc.om['xdc.bld.ITarget'].CompileOptions();
                newGoal.profile = goal.profile;

		/* In the profile 'whole_program_debug', we have 
                 * --symdebug:dwarf option supplied in the --opt link option.
                 * Therefore, the generated assembly file has that option
                 * embedded instead of the default --symdebug:skeletal. We
                 * have to pass --symdebug:dwarf on the command line, so the
                 * option inside the assembly file and the option on the
                 * command line will match. There is also possibility that
                 * the user passed --opt through profile's linkOpts or 
                 * through ccOpts or through cc.opts, so we have to examine
                 * the link command line and extract --symdebug option, if
                 * it was passed. 
                 */
                var linkLine = result.cmds.match
                    (/.*--opt=["'](?:.*\s)?(--symdebug:\w+)(?:\s.*)?["'].*\n/);
                if (linkLine != null) {
		    newGoal.opts.aopts = linkLine[1];
		}

            	var ret = _compile(target, newGoal, false);
                ret.cmds = ret.cmds.replace(/\$</, "$(XDCCFGDIR)/$@" +
                    asmSuffix);
                ret.cmds = ret.cmds.replace(/-eo.\w+/, "-eo.obj");

                /* remove MKDEP part */
                ret.cmds = ret.cmds.substring(0, ret.cmds.indexOf('\n'));
                result.cmds += ret.cmds;
            }
	}
    }

    return (result);
}

/*
 *  ======== ITarget.scompile ========
 */
function scompile(goal) {
    return (_compile(this, goal, true));
}

/*
 *  ======== ITarget.getISAChain ========
 */
function getISAChain(isa) {
    var key = "isa_" + (isa == null ? this.isa : isa);
    var result = _isaChainMap[key];
    return (result == null ? [] : result);
}

/*
 *  ======== _langOpts ========
 */
var _langOpts = {asm: "-fa", c: "-fc", cpp: "-fp"};

/*
 *  ======== _compile ========
 */
function _compile(target, goal, asm)
{
    var result = null;

    _initTabs();

    var tool2cmd = _targetCmds[target.suffix];	/* get tool to template map */

    if (tool2cmd != null) {
	var ext = target.extensions[goal.srcSuffix];
	if (ext != null) {
	    var ea = ext.typ.split(':');
	    var cmdType = ea[0];
	    var langOpt = _langOpts[cmdType];
	    if (ea[1] != null) {
		langOpt = ea[1];
	    }

	    if (asm == true && cmdType == "asm") {
		return (null);	/* nothing to do to convert to asm code */
	    }
	    result = new xdc.om['xdc.bld.ITarget'].CommandSet;
	    result.msg = (cmdType == "c" ? "cl" : cmdType)
			    + target.suffix
			    + (asm == true ? " -n $< ..." : " $< ...");

	    /* BUG: normally we should retain the trailing "/" in
	     * the dstDir directory but we are working around a cl6x
	     * bug (SDSsq23670).
	     */
	    var dstDir = goal.dstPrefix + goal.base;
	    dstDir = dstDir.replace(/[\/\\][^\/\\]*$/, "");

            var ccoptsPre = target.ccOpts.prefix;
            var ccoptsSuf = target.ccOpts.suffix;
            /* If the target defines ccConfigOpts, they should be used
             * instead of ccOpts. The string expansion has to be done here,
             * in the case ccConfigOpts contains COPTS. If the expansion was
             * left for the end of the function, there would be an infinite
             * recursion.
             */
            if ("ccConfigOpts" in target && goal.configOpts == true) {
                ccoptsPre = _bldUtils.expandString(target.ccConfigOpts.prefix,
                    {"ccOpts.prefix": target.ccOpts.prefix});
                ccoptsSuf = _bldUtils.expandString(target.ccConfigOpts.suffix,
                    {"ccOpts.suffix": target.ccOpts.suffix});
            }
            
	    _setEnv(target, result);
	    result.cmds = _bldUtils.expandString(tool2cmd[cmdType], {
		COPTS_P:    ccoptsPre,
		COPTS_S:    ccoptsSuf,
		AOPTS_P:    target.asmOpts.prefix,
		AOPTS_S:    target.asmOpts.suffix,
		ASMONLY:    asm ? "-n -s --symdebug:none" : "",
		dstDir:	    dstDir,
		srcExt:	    goal.srcSuffix,
		copts:	    (goal.configOpts && "cfgcopts" in goal.opts)
		            ? goal.opts.cfgcopts : goal.opts.copts,
		aopts:	    goal.opts.aopts,
		defs:	    goal.opts.defs,
		incs:	    goal.opts.incs,
		langOpt:    langOpt
	    });
            
            /* The option '--gen_profile_info' cannot be used with 6.0.x
             * compiler for 6x, and any of Arm compilers older or equal to
             * 4.6.0B1.
             */
            if (goal.profile.match(/coverage/)
                || goal.profile.match(/profile/)) {
                var compString = target.getVersion().split('{')[1];
                var compVersion = compString.split(',');
                if (target.isa == "64P" || target.isa == "674") {
                    if (compVersion[2] <= "6.0") {
                        result.cmds =
                            result.cmds.replace(/--gen_profile_info/g, "");
                    }
                }
                if (target.isa == "470" || target.isa[0] == "v") {
                    if (compVersion[2] <= "4.6") {
                        result.cmds =
                            result.cmds.replace(/--gen_profile_info/g, "");
                    }
                }
            }
	}
    }

    return (result);
}

/*
 *  ======== _init ========
 *  Initialize this capsule at package init time.
 */
function _init(pkg)
{
    _thisPkg = pkg;
}

/*
 *  ======== _initTabs ========
 *  Initialize command tables *after* config.bld to allow config.bld to
 *  adjust the commands.
 */
function _initTabs()
{
    if (_targetCmds == null) {
	_targetCmds = {};

	/*
	 * Loop over all packages in case another package has a module
	 * that wants to extend this package's ITarget interface.
	 */
	var pa = xdc.om.$packages;
	for (var j = 0; j < pa.length; j++) {
	    var ma = pa[j].$modules;
	    for (var i = 0; i < ma.length; i++) {
		if (ma[i] instanceof _thisPkg.ITarget.Module) {
		    var targ = _thisPkg.ITarget.Module(ma[i]);
		    _mkCmds(targ);
		}
	    }
	}
    }
}

/*
 *  ======== _mkCmds ========
 */
function _mkCmds(target)
{
    var cmdOpts, cmd;
    var tool2cmd = {};

    /* cmd prefix:
     *	    $(binDir) for optional codegen debug support
     */
    var cmdPrefix = target.binDir;

    /* default make dependencies command */
    var mkdep = "$(MKDEP) -a $@.dep -p $(dstDir) -s o"
	    + target.suffix + " $< -C ";

    /* define assembly options */
    cmdOpts = " $(AOPTS_P) " + target.asm.opts + " -eo.o" + target.suffix
          + " -ea.s" + target.suffix
          + " $(AOPTS_S) $(defs) $(aopts) $(incs) $(XDCINCS) "
	  + target.includeOpts;

    /* define assemble command template */
    cmd  = cmdPrefix + target.asm.cmd + cmdOpts
	     + " -fr=./$(dstDir) $(langOpt) $<\n";
    cmd = cmd.concat(mkdep + cmdOpts);
    tool2cmd["asm"] = cmd;

    /* define the C options */
    /* -eo and -ea options are added here rather than in target defintions,
     * so that configuro can ask for cc.opts and get only the necessary
     * options.
     *
     * The users who build with XDC still get the extension functionality that
     * allows us to build for multiple targets.
     */
    cmdOpts = " $(ASMONLY) $(COPTS_P) " + target.cc.opts + " -eo.o"
    	  + target.suffix + " -ea.s" + target.suffix
	  + " $(COPTS_S) $(defs) $(copts) $(incs) $(XDCINCS) "
	  + target.includeOpts + " -fs=./$(dstDir) -fr=./$(dstDir)";

    if (0) {
	 /* redefine MKDEP command template to use compiler.  This gives us
	  * precise dependencies but the TI compiler does not (yet) allow one
	  * to specify the output file for dependiences, so we can't run a
	  * parallel build!
	  *
	  * Once the compiler has the ability to support parallel build we
	  * can incorporate the dependency generation with the compilation
	  * and avoid a separate compile step (-ppa -ppd=$@.dep).
	  */
	 mkdep = cmdPrefix + target.cc.cmd + " -ppd " + cmdOpts + " $<\n"
	    + "@$(MV) $(patsubst %$(srcExt),%.pp,$<) $@.dep\n#";
    }

    /* define the C compile command template */
    cmd =  cmdPrefix + target.cc.cmd + cmdOpts + " $(langOpt) $<\n";
    cmd = cmd.concat(mkdep + cmdOpts);
    tool2cmd["c"] = cmd;

    /* define the C++ compile command template */
    tool2cmd["cpp"] = cmd;  /* only diff between C and C++ is $(langOpt) */

    /* define the link command template */
    cmd = "$(RM) $(XDCCFGDIR)/$@.map\n" + cmdPrefix + target.lnk.cmd
	      + " $(LOPTS_P) $(lopts) -q -o $@ $(files) " + target.lnk.opts
	      + " $(LOPTS_S)\n";
    tool2cmd["link"] = cmd;

    /* define the ar command template */
    cmd = cmdPrefix + target.ar.cmd + " $(AROPTS_P) " + target.ar.opts
	    + " $@ $(AROPTS_S) $(aropts) $(files)";
    tool2cmd["ar"] = cmd;

    /* define the vers command template */
    cmd = cmdPrefix + target.vers.cmd + " " + target.vers.opts;
    tool2cmd["vers"] = cmd;

    /* add set of command templates to _targetCmds */
    _targetCmds[target.suffix] = tool2cmd;
}

/*
 *  ======== _setEnv ========
 */
function _setEnv(target, result)
{
    var path = _targetEnvs[target.suffix];

    if (path == null) {
        var pathTemplate = "$(pathPrefix);$(binDir);$(PATH)";

	/* expand path template (because binDir and pathPrefix might
	 * embed additional $(...) elements
	 */
	var tmp = _bldUtils.expandString(pathTemplate, {
	    binDir:     target.binDir,
	    pathPrefix: target.pathPrefix
	});
	path = tmp.split(";");

	/* save the result to avoid recomputing this for each compile line */
	_targetEnvs[target.suffix] = path;
    }

    result.path = path;
    result.envs = ["C_DIR="];
}
