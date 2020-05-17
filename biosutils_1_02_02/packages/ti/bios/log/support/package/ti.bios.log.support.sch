xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.bios.log.support');

/* ======== IMPORTS ======== */

    xdc.loadPackage('ti.bios');
    xdc.loadPackage('ti.bios.utils');
    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.bios.log.support
    var pkg = $om.$$bind('ti.bios.log.support.Package', $$PObj());
    $om.$$bind('ti.bios.log.support', $$VObj('ti.bios.log.support', pkg));
// module LogAux
    var po = $om.$$bind('ti.bios.log.support.LogAux.Module', $$PObj());
    var vo = $om.$$bind('ti.bios.log.support.LogAux', $$VObj('ti.bios.log.support.LogAux', po));
    pkg.$$bind('LogAux', vo);
// module LogTrack
    var po = $om.$$bind('ti.bios.log.support.LogTrack.Module', $$PObj());
    var vo = $om.$$bind('ti.bios.log.support.LogTrack', $$VObj('ti.bios.log.support.LogTrack', po));
    pkg.$$bind('LogTrack', vo);

/* ======== CONSTS ======== */

// module LogAux
// module LogTrack

/* ======== CREATES ======== */

// module LogAux
// module LogTrack

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module LogAux
    var po = $om['ti.bios.log.support.LogAux.Module'].$$init('ti.bios.log.support.LogAux.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
// module LogTrack
    var po = $om['ti.bios.log.support.LogTrack.Module'].$$init('ti.bios.log.support.LogTrack.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('createLogSize', $$T_CNum('(xdc_Int)'), 1024, 'wh');
        po.$$fld('trackLogSize', $$T_CNum('(xdc_Int)'), 4096, 'wh');
        po.$$fld('trackPeriod', $$T_CNum('(xdc_Int)'), 10, 'wh');

/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.bios.log.support
    var po = $om['ti.bios.log.support.Package'].$$init('ti.bios.log.support.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/bios/log/support/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.bios.log.support'].$$init(po, 'ti.bios.log.support', $$DEFAULT, false);
    pkg.$$bind('$name', 'ti.bios.log.support');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.log.support.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [1, 0, 2, 02]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
        ['ti.bios', []],
        ['ti.bios.utils', []],
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
        'lib/logsupport.a64P',
        'lib/logsupport.a674',
        'lib/logsupport.a64',
    ];
    
    pkg.build.libDesc = [
        [
            'lib/logsupport.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/logsupport.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/logsupport.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
        pkg.build.libDesc['lib/logsupport.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/logsupport.a674'].suffix = '674';
        pkg.build.libDesc['lib/logsupport.a64'].suffix = '64';
    }
    
// module LogAux
    var vo = $om['ti.bios.log.support.LogAux'];
    var po = $om['ti.bios.log.support.LogAux.Module'];
    vo.$$init(po, 'ti.bios.log.support.LogAux', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.bios.log.support.LogAux'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.bios.log.support']);
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
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.log.support.LogAux$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr.$seal('length');
    pkg.$$bind('LogAux', vo);
    pkg.$unitNames.$add('LogAux');
// module LogTrack
    var vo = $om['ti.bios.log.support.LogTrack'];
    var po = $om['ti.bios.log.support.LogTrack.Module'];
    vo.$$init(po, 'ti.bios.log.support.LogTrack', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.bios.log.support.LogTrack'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.bios.log.support']);
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
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.log.support.LogTrack$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr.$seal('length');
    pkg.$$bind('LogTrack', vo);
    pkg.$unitNames.$add('LogTrack');

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    $om['ti.bios.log.support.LogAux'].module$meta$init();
    $om['ti.bios.log.support.LogTrack'].module$meta$init();
    pkg.init();
    $om['ti.bios.log.support.LogAux'].$$bless();
    $om['ti.bios.log.support.LogTrack'].$$bless();
    $om.$packages.$add(pkg);
