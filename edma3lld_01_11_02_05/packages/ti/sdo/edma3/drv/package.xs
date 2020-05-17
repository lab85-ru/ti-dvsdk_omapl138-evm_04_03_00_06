/*
 * ======== package.xs ========
 *
 */

/*
 * ======== getLibs ========
 */
function getLibs(prog)
{
    print ("Inside EDMA3 Drv getLibs");

    /* Prepare variables to form the library path within this package */
    var name;
    var name_c64x = "edma3_drv_bios.lib";
    var name_c67x = "edma3_drv_bios_c674.lib";

    var name_i;
    var name_c64x_i = "edma3_drv_bios_i.lib";
    var name_c67x_i = "edma3_drv_bios_c674_i.lib";
    var lib = "lib/";

    print ("Target Name: " + Program.build.target.name);


    switch (Program.build.target.name) {
	    case 'C64P':
	    	/* C64P */
	    	name = name_c64x;
	    	name_i = name_c64x_i;
	    	break;

	    case 'C674':
	    	/* C674 */
	    	name = name_c67x;
	    	name_i = name_c67x_i;
	    	break;

		default:
			/* Default is C64P */
	    	name = name_c64x;
	    	name_i = name_c64x_i;
	    	break;
		}


    switch (this.profile) {
	    case 'debug':
	        /* enable debug build for debug profile only */
	        lib = lib + "Debug/" + name;
	        break;

	    case 'idebug':
	        /* enable debug build for debug profile only */
	        lib = lib + "Debug/" + name_i;
	        break;

	    case 'irelease':
	        /* enable irelease build for irelease profile only */
	        lib = lib + "Release/" + name_i;
	        break;

	    default:
	        /* release profile for everything else */
	        lib = lib + "Release/" + name;
	    }

    print("    will link with " + this.$name + ":" + lib);

    /* return the library name */
    return (lib);
}

