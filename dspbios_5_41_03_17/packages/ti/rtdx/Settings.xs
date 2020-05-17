/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

function getTargetDir( targetName ) {
    if (targetName.match( "C2" ) != null) {
        return "c2000";
    }
    if (targetName.match( "C55" ) != null) {
        return "c5500";
    }
    if (targetName.match( "C6" ) != null) {
        return "c6000";
    }
    if (targetName.match( "470" ) != null) {
        return "tms470";
    }
    if (targetName.match( "Arm7" ) != null) {
        return "tms470";
    }
    if (targetName.match( "Arm9" ) != null) {
        return "tms470";
    }
    if (targetName.match( "M3" ) != null) {
        return "tms470";
    }
    throw new Error( "rtdx: unfamiliar target " + targetName );
}

function getLibPath( targetName ) {
    var path = xdc.getPackageBase( this.$package.$name ) + "lib/";
    return path + getTargetDir( targetName );
}

function getIncludes( targetName ) {
    var path = xdc.getPackageBase( this.$package.$name ) + "include/";
    return path + getTargetDir( targetName );
}
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

