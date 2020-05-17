/*
 *  ======== package.cfg ========
 *  This script is inserted into the package's schema when the schema
 *  is generated from the package's CDL files. It is executed whenever
 *  the package's schema is included; e.g., whenever a program is being
 *  configured and the program's configuration script includes this
 *  package.
 *
 *  Typically this script will re-define the "getLibs" function property
 *  of the package object so that this package can control the name of
 *  the library linked into the program being created.
 *
 *  Pre-Conditions:
 *	prog	- "global" Program object of the program including this package
 *	pkg	- "global" Package object of the current package
 *
 *  Post-Condtions:
 *	pkg.getLibs optionally set to a function that returns a library name
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
 *	this		- the current package object
 *	this.prog	- the program being configured (after all of the
 *			  program's configuration scripts complete)
 *	this.prog.build - program attributes specified in the build script
 *
 *  Returns
 *	null	    - indicates that no library is to be linked
 *	non-null    - name of the library to link with (relative to the
 *		      package directory).
 */
function getLibs()
{
    /* libraries found via linker command file */
    return (null);
};

/*
 *  ======== getSects ========
 *  This method is called during generation (after a program's
 *  configuration script completes) and is responsible for returning a
 *  template file name that is used to generate package-specific
 *  linker command file statements.
 *
 *  Context:
 *	    this    the current package's xdc.IPackage.Module object
 *		    after the program's configuration script has completed.
 *
 *	    prog    Configuration model program object (xdc.Program) after
 *		    program's configuration script has completed.
 *
 *  Params:
 *	    void
 *
 *  Returns
 *	    string  path name of template file.  The path name is relative
 *		    to the package path.
 *
 *		    If null, no data is to be generated.
 *
 *		    the template is evaluated in a context where the this
 *		    pointer is the package object.
 *
 *  Throws
 *	    exceptions thrown are for fatal errors
 */
function getSects()
{
    return ("ti/psl/psl.xdt");
}
