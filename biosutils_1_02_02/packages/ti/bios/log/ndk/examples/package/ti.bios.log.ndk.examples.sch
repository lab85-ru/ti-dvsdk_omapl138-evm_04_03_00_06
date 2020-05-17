xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.bios.log.ndk.examples');

/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.bios');
    xdc.loadPackage('ti.bios.utils');
    xdc.loadPackage('ti.bios.log.support');
    xdc.loadPackage('ti.bios.log.ndk');
    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.bios.log.ndk.examples
    var pkg = $om.$$bind('ti.bios.log.ndk.examples.Package', $$PObj());
    $om.$$bind('ti.bios.log.ndk.examples', $$VObj('ti.bios.log.ndk.examples', pkg));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.bios.log.ndk.examples
    var po = $om['ti.bios.log.ndk.examples.Package'].$$init('ti.bios.log.ndk.examples.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/bios/log/ndk/examples/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.bios.log.ndk.examples'].$$init(po, 'ti.bios.log.ndk.examples', $$DEFAULT, false);
    pkg.$$bind('$name', 'ti.bios.log.ndk.examples');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.log.ndk.examples.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [1, 0, 2, 02]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.bios', []],
        ['ti.bios.utils', []],
        ['ti.bios.log.support', []],
        ['ti.bios.log.ndk', []],
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
    ];
    
    pkg.build.libDesc = [
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
    }
    

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    pkg.init();
    $om.$packages.$add(pkg);
