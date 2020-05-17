/* 
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
/*
 *  ======== ITarget ========
 */
var ITarget = xdc.loadCapsule("ITarget.xs");

/*
 *  ======== init ========
 *  Initialize this package.
 *
 *  This is called after all modules in this package are defined and
 *  initialized.  It is called before config.bld.
 */
function init()
{
    ITarget._init(this);
    
    /* initialize module configuration params that are computed */
    var ma = this.$modules;
    for (var i = 0; i < ma.length; i++) {
        if (ma[i] instanceof this.ITarget.Module) {
            var targ = this.ITarget.Module(ma[i]);
            var isas = targ.getISAChain(null);

            if (isas.length > 0) {
                /* generate a default platform for each target, if necessary */
                if (targ.platform == null) {
                    targ.platform = "ti.platforms.sim" + isas[0] + "xx";
                    if (isas[0] == "430") {
                        targ.platform = "ti.platforms.exp430F5438";
                    }
                }
    
                /* add target-specific assembly-language extensions */
                for (var idx = 0; idx < isas.length; idx++) {
                    var ext = ".s" + isas[idx];
                    targ.extensions.$putHead(ext,
                        new xdc.om['xdc.bld.ITarget.Extension'](
                            {suf: ext, typ: "asm"}
                        )
                    );
                }
            }
            else {
                print("Warning: ISAChain is empty for target " + targ);
            }

            /* add .s<suffix> to list of recognized extensions */
            var ext = ".s" + targ.suffix;
            targ.extensions.$putHead(ext,new xdc.om['xdc.bld.ITarget.Extension']({
                suf: ext,   typ: "asm"
            }));

        }
    }
}

/*
 *  ======== Package.getLibs ========
 *  This package's implementation of xdc.IPackage.getLibs.
 */
function getLibs()
{
    return (null);    /* no library required for this package */
}

/*
 *  @(#) ti.targets; 1, 0, 3,385; 2-24-2010 16:24:16; /db/ztree/library/trees/xdctargets/xdctargets-b36x/src/
 */

