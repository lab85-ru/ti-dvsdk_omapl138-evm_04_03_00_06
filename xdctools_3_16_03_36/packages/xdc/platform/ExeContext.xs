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
 *  ======== ExeContext.Instance.init ========
 *  Create a ExeContext instance
 */
function instance$meta$init()
{
    var Utils = this.$module.$package.Utils;

    /* create a "data sheet" instance for the device */
    this.attrs = Utils.getCpuDataSheet(this.$args.cpu);

    /* initialize other fields of the execution context instance */
    this.id = this.$args.cpu.id;
    this.deviceName = this.$args.cpu.deviceName;
    this.catalogName = this.$args.cpu.catalogName;
    this.clockRate = this.$args.cpu.clockRate;
    var rev = this.$args.cpu.revision;
    if (rev != null) {
	this.revision = rev;
    }
    
    this.board = this.$args.board;

    return (this);
}
/*
 *  @(#) xdc.platform; 1, 0, 1, 0,237; 2-22-2010 10:33:24; /db/ztree/library/trees/xdc/xdc-u17x/src/packages/
 */

