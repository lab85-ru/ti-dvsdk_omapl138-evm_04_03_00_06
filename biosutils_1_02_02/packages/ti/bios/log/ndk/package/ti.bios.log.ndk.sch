xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.bios.log.ndk');

/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.bios');
    xdc.loadPackage('ti.bios.log.support');
    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.bios.log.ndk
    var pkg = $om.$$bind('ti.bios.log.ndk.Package', $$PObj());
    $om.$$bind('ti.bios.log.ndk', $$VObj('ti.bios.log.ndk', pkg));
// module LogServerCgi
    var po = $om.$$bind('ti.bios.log.ndk.LogServerCgi.Module', $$PObj());
    var vo = $om.$$bind('ti.bios.log.ndk.LogServerCgi', $$VObj('ti.bios.log.ndk.LogServerCgi', po));
    pkg.$$bind('LogServerCgi', vo);

/* ======== CONSTS ======== */

// module LogServerCgi

/* ======== CREATES ======== */

// module LogServerCgi

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module LogServerCgi
    var po = $om['ti.bios.log.ndk.LogServerCgi.Module'].$$init('ti.bios.log.ndk.LogServerCgi.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');

/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.bios.log.ndk
    var po = $om['ti.bios.log.ndk.Package'].$$init('ti.bios.log.ndk.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/bios/log/ndk/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.bios.log.ndk'].$$init(po, 'ti.bios.log.ndk', $$DEFAULT, false);
    pkg.$$bind('$name', 'ti.bios.log.ndk');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.log.ndk.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [1, 0, 2, 02]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.bios', []],
        ['ti.bios.log.support', []],
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
        'lib/logservercgi.a64',
        'lib/logservercgi.a64P',
        'lib/logservercgi.a674',
    ];
    
    pkg.build.libDesc = [
        [
            'lib/logservercgi.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/logservercgi.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/logservercgi.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
        pkg.build.libDesc['lib/logservercgi.a64'].suffix = '64';
        pkg.build.libDesc['lib/logservercgi.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/logservercgi.a674'].suffix = '674';
    }
    
// module LogServerCgi
    var vo = $om['ti.bios.log.ndk.LogServerCgi'];
    var po = $om['ti.bios.log.ndk.LogServerCgi.Module'];
    vo.$$init(po, 'ti.bios.log.ndk.LogServerCgi', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.bios.log.ndk.LogServerCgi'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.bios.log.ndk']);
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
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.log.ndk.LogServerCgi$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr.$seal('length');
    pkg.$$bind('LogServerCgi', vo);
    pkg.$unitNames.$add('LogServerCgi');

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    $om['ti.bios.log.ndk.LogServerCgi'].module$meta$init();
    pkg.init();
    $om['ti.bios.log.ndk.LogServerCgi'].$$bless();
    $om.$packages.$add(pkg);
