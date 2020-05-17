/*
 *  Copyright 2008 by Texas Instruments Incorporated.
 *
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *
 */

/*
 *  ======== Settings.xdc ========
 *
 */

/*!
 *  ======== Settings ========
 *  the module that provides include path (for programs to build with 
 *  RTDX header files) and lib path (for programs to build with RTDX libraries)
 *  based on different targets.
 */

metaonly module Settings {
    string getIncludes( string targetName );
    string getLibPath(  string targetName );
}
/*
 *  @(#) ti.rtdx; 2, 0, 0, 5,110; 12-16-2008 11:08:36; /db/vtree/library/trees/rtdxtc/rtdxtc-e06x/src/
 */

