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
xdc.loadPackage('xdc.services.intern.cmd');

/* add any .jar file dependencies to the generated makefile */
var args = arguments;
var deps = xdc.$$addJars();
if (deps != null) {
    arguments.push("-D");
    args = arguments.concat(deps);
}

Packages.xdc.services.intern.cmd.Builder.main(args);

