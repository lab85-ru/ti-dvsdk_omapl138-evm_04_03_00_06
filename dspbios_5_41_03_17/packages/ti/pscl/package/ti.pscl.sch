xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.pscl');

/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.pscl
    var pkg = $om.$$bind('ti.pscl.Package', $$PObj());
    $om.$$bind('ti.pscl', $$VObj('ti.pscl', pkg));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.pscl
    var po = $om['ti.pscl.Package'].$$init('ti.pscl.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/pscl/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.pscl'].$$init(po, 'ti.pscl', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.pscl');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.pscl.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [1, 0, 0, 0]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
        'lib/pscl_cfg_evm6748_456.a674',
        'lib/pscl_cfg_evm6748_372.a674',
        'lib/pscl.a674',
        'lib/pscl_cfg_null.a674',
        'lib/pscl_cfg_evm6748_408.a674',
        'lib/pscl_cfg_evm6748.a674',
    ];
    
    pkg.build.libDesc = [
        [
            'lib/pscl_cfg_evm6748_456.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/pscl_cfg_evm6748_372.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/pscl.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/pscl_cfg_null.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/pscl_cfg_evm6748_408.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/pscl_cfg_evm6748.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
        pkg.build.libDesc['lib/pscl_cfg_evm6748_456.a674'].suffix = '674';
        pkg.build.libDesc['lib/pscl_cfg_evm6748_372.a674'].suffix = '674';
        pkg.build.libDesc['lib/pscl.a674'].suffix = '674';
        pkg.build.libDesc['lib/pscl_cfg_null.a674'].suffix = '674';
        pkg.build.libDesc['lib/pscl_cfg_evm6748_408.a674'].suffix = '674';
        pkg.build.libDesc['lib/pscl_cfg_evm6748.a674'].suffix = '674';
    }
    

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    pkg.init();
