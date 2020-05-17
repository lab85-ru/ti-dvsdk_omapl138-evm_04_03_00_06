xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.bios.utils');

/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.bios');
    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.bios.utils
    var pkg = $om.$$bind('ti.bios.utils.Package', $$PObj());
    $om.$$bind('ti.bios.utils', $$VObj('ti.bios.utils', pkg));
// module Load
    var po = $om.$$bind('ti.bios.utils.Load.Module', $$PObj());
    var vo = $om.$$bind('ti.bios.utils.Load', $$VObj('ti.bios.utils.Load', po));
    pkg.$$bind('Load', vo);

/* ======== CONSTS ======== */

// module Load

/* ======== CREATES ======== */

// module Load

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module Load
    var po = $om['ti.bios.utils.Load.Module'].$$init('ti.bios.utils.Load.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');

/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.bios.utils
    var po = $om['ti.bios.utils.Package'].$$init('ti.bios.utils.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/bios/utils/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.bios.utils'].$$init(po, 'ti.bios.utils', $$DEFAULT, false);
    pkg.$$bind('$name', 'ti.bios.utils');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.utils.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [2, 0, 2, 02]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.bios', []],
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
        'lib/utils.a64P',
        'lib/utils.a674',
        'lib/utils.a64',
    ];
    
    pkg.build.libDesc = [
        [
            'lib/utils.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/utils.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/utils.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
        pkg.build.libDesc['lib/utils.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/utils.a674'].suffix = '674';
        pkg.build.libDesc['lib/utils.a64'].suffix = '64';
    }
    
// module Load
    var vo = $om['ti.bios.utils.Load'];
    var po = $om['ti.bios.utils.Load.Module'];
    vo.$$init(po, 'ti.bios.utils.Load', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.bios.utils.Load'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.bios.utils']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('$$mcfgs', []);
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    vo.$$bind('$$romcfgs', '|');
    if ('Module_State' in vo) vo.$$bind('$object', new vo.Module_State);
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.utils.Load$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Load', vo);
    pkg.$unitNames.$add('Load');

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    $om['ti.bios.utils.Load'].module$meta$init();
    pkg.init();
    $om['ti.bios.utils.Load'].$$bless();
    $om.$packages.$add(pkg);
