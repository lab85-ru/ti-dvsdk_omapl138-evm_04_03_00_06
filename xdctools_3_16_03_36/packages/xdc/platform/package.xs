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
 *  ======== IPackage.init ========
 *  Initialize this package.
 *
 *  This is called after all modules in this package are defined and
 *  initialized.
 */
function init()
{
//    print(this.$name + ".init() ...");
}

/*
 *  ======== IPackage.exit ========
 *  Finalize this package
 */
function exit() {
//    print(this.$name + ".exit() ...");
}

/*
 *  ======== IPackage.getLibs ========
 *  This package's implementation of xdc.IPackage.getLibs.
 */
function getLibs()
{
    return (null);    /* no library required for this package */
}

/*
 *  @(#) xdc.platform; 1, 0, 1, 0,237; 2-22-2010 10:33:24; /db/ztree/library/trees/xdc/xdc-u17x/src/packages/
 */

