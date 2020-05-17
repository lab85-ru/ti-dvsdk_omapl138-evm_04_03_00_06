xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.psl
    var po = $om.$$bind('ti.psl.Package', $$PObj());
    $om.$$bind('ti.psl', $$VObj('ti.psl', po));

/* ======== CONSTS ======== */


/* ======== CREATES ======== */


/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */


/* ======== SINGLETONS ======== */

// package ti.psl
    var po = $om['ti.psl.Package'].$$init('ti.psl.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('../package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    var pkg = $om['ti.psl'].$$init(po, 'ti.psl', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.psl');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.psl.');
    pkg.$$bind('$vers', [5, 0, 0, 0]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
        'lib/PSL_debug_c5510.a55',
        'lib/PSL_cfg_c5507.a55',
        'lib/PSL_c5510.a55L',
        'lib/PSL_cfg_c5507.a55L',
        'lib/PSL_cfg_c5509a.a55',
        'lib/PSL_c5507.a55L',
        'lib/PSL_c5509a.a55L',
        'lib/PSL_c5509a.a55',
        'lib/PSL_debug_c5510.a55L',
        'lib/PSL_cfg_c5510.a55',
        'lib/PSL_cfg_c5509a.a55L',
        'lib/PSL_debug_c5507.a55L',
        'lib/PSL_debug_c5503.a55',
        'lib/PSL_debug_c5509a.a55',
        'lib/PSL_cfg_dsk5510.a55',
        'lib/PSL_cfg_dsk5510.a55L',
        'lib/PSL_c5507.a55',
        'lib/PSL_cfg_c5503.a55',
        'lib/PSL_cfg_evm5509a.a55',
        'lib/PSL_cfg_c5510.a55L',
        'lib/PSL_cfg_c5503.a55L',
        'lib/PSL_debug_c5509a.a55L',
        'lib/PSL_c5503.a55L',
        'lib/PSL_c5510.a55',
        'lib/PSL_debug_c5503.a55L',
        'lib/PSL_debug_c5507.a55',
        'lib/PSL_cfg_evm5509a.a55L',
        'lib/PSL_c5503.a55',
    ];

/* ======== INITIALIZATION ======== */

    pkg.init();
