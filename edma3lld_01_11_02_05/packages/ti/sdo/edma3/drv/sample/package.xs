/*
 * ======== package.xs ========
 *
 */

/*
 * ======== getLibs ========
 */
function getLibs(prog)
{
    var bool = 0;

    print ("Inside EDMA3 Drv Sample getLibs");

    /* Prepare variables to form the library path within this package */
    var name = "edma3_drv_bios_sample.lib";
    var name_i = "edma3_drv_bios_sample_i.lib";
    var lib = "lib/";

	/* Devices supported */
	var devices = 	[
						'TMS320C6424',
						'TMS320C6452',
						'TMS320C6455',
						'TMS320C6472',
						'TMS320C6474',
						'TMS320C6747',
						'TMS320C6748',
						'TMS320DA830',
						'TMS320CDM648',
						'TMS320CDM6437',
						'TMS320CDM6446',
						'TMS320CDM6467',
						'OMAP2530',
						'OMAP3530',
						'TMS320C2430',
						'TMS320C3430',
						'OMAPL137',
						'OMAPL138',
						'TMS320CTCI6486',
						'TMS320CTCI6488',
					];


    /* Directories for each platform */
    var dir = [ 'c6424/',
                'c6452/',
                'c6455/',
                'c6472/',
                'c6474/',
                'c6747/',
                'c6748/',
                'da830/',
                'dm648/',
                'dm6437/',
                'dm6446/',
                'dm6467/',
                'omap25xx/',
                'omap35xx/',
                'omap2430/',
                'omap3430/',
                'omapl137/',
                'omapl138/',
                'tci6486/',
                'tci6488/',
              ];


    for (var i = 0; i < devices.length; i++)
        {
        if (java.lang.String(Program.cpu.deviceName).equals(devices[i]))
            {
                /* Choose the selected platform and build the complete name. */
                lib = lib + dir[i];
                bool = 1;
                break;
            }
        }

    if (bool == 0)
        throw new Error('Unexpected value in "platform" parameter')


    /* check the build profile to get the library to return */
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

