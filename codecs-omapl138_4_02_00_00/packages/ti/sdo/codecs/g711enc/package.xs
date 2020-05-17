/*
 *  ======== package.xs ========
 *
 */

/*
 *  ======== getLibs ========
 */

function getLibs(prog)
{
    var name = "";
    var suffix = "";

    if ("findSuffix" in prog.build.target) {
        var suffixes = ["64P"];
        suffix = prog.build.target.findSuffix(suffixes);
        if (suffix == null) {
            /* print("No compatible lib found in ti.sdo.codecs.g711enc"); */
            /* no compatible lib found in this package, return "" */
            return ("");
        }
    }
    else {
        throw new Error("This package requires xdctools 3.15 or greater.");
    }

    /* And finally, the location of the this library is in lib */
    name = "lib/g711enc_tii.l64P";

    /* and dump a helpful breadcrumb */
    print("    will link with " + this.$name + ":" + name);

    return (name);
}


/*
 *  ======== getSects ========
 */
function getSects()
{
    var template = null;

    if (Program.build.target.isa == "64P"||Program.build.target.isa == "674") {
        template = "ti/sdo/codecs/g711enc/link.xdt";
    }

    return (template);
}