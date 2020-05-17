/*
 *  ======== locateRecap ========
 */
function locateRecap(symTab)
{
    /* Look along the package path for the ti.bios.rov.recaps package. */    

    /* Get the XDC path from the environment. */
    var xdcPath = environment['xdc.path'];
    var paths = xdcPath.split(';');    
    
    /* For each path on the package path... */
    var recaps = "";
    for each (var path in paths) {
        var file = new java.io.File(path + "/ti/bios/rov/recaps/");
        if (file.exists()) {
            recaps = file.getCanonicalPath();
            break;
        }
    }

    /* Throw an error if the recaps package wasn't found. */
    if (recaps == "") {
        throw (new Error("Could not find package ti.bios.rov.recaps along the " +
                         "package path."));
    }
    
    /* Determine the recap file to use based on the ISA. */
    var isa = symTab.getISA();
    
    /* 6x targets. */
    if ((isa == 0x62) || (isa == 0x64) || (isa == 0x67) || (isa == 0x6F)) {
        var be = symTab.getSymbolValue('GBL_BIGENDIAN');

        if (be == -1) {
            throw (new Error("Could not find symbol GBL_BIGENDIAN needed to " +
                             "detect endianness."));
        }
        else if (be == 0) {
            return (recaps + "/C6x_little_endian.rov.xs");
        }
        else if (be == 1) {
            return (recaps + "/C6x_big_endian.rov.xs");
        }
        else {
            throw (new Error("Unrecognized value for GBL_BIGENDIAN: " + be));
        }
    }
    /* 28x targets. */
    else if (isa == 0x28) {
        return (recaps + "/C28_large.rov.xs");
    }
    /* 55x targets. */
    else if (isa == 0x55) {
        /* Large and huge have same types, so we don't care which it is. */
        return (recaps + "/C55_large.rov.xs");
    }
    else {
        throw (new Error("Unsupported ISA: " + isa));
    }
}
