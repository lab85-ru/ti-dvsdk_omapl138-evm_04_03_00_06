xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __RTA__ = $om.$name == 'rta';
var $$path = Packages.Iliad.SYS.Path.curpath().split(';');
var $$ses = new Packages.xdc.spec2.BrowserSession($$path);
var $$spec = $$ses.findPkg('ti.bios.help');

/* ======== OBJECTS ======== */

// package ti.bios.help
    var pkg = $om.$$bind('ti.bios.help.Package', $$PObj());
    $om.$$bind('ti.bios.help', $$VObj('ti.bios.help', pkg));
// module Settings
    var po = $om.$$bind('ti.bios.help.Settings.Module', $$PObj());
    var vo = $om.$$bind('ti.bios.help.Settings', $$VObj('ti.bios.help.Settings', po));
    pkg.$$bind('Settings', vo);

/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== CONSTS ======== */

// module Settings

/* ======== CREATES ======== */

// module Settings

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */

// module Settings

/* ======== TYPES ======== */

// module Settings
    var po = $om['ti.bios.help.Settings.Module'].$$init('ti.bios.help.Settings.Module', $$Module);
        po.$$fld('$label', $$T_Str, '//ti.bios.help.Settings', 'rh');
        po.$$fld('$hostonly', $$T_Num, 1, 'r');

/* ======== RTA ======== */

if (__RTA__) {


} // __RTA__

/* ======== SINGLETONS ======== */

// package ti.bios.help
    var po = $om['ti.bios.help.Package'].$$init('ti.bios.help.Package', $om['xdc.IPackage.Module']);
    po.$$bind('$capsule', undefined);
    var pkg = $om['ti.bios.help'].$$init(po, 'ti.bios.help', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.bios.help');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.help.');
    pkg.$$bind('$spec', $$spec);
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];
// module Settings
    var vo = $om['ti.bios.help.Settings'];
    var po = $om['ti.bios.help.Settings.Module'];
    vo.$$init(po, 'ti.bios.help.Settings', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', $$ses.findUnit('ti.bios.help.Settings'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.bios.help']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$mcfgs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
if (__CFG__) {
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.help.Settings$$instance$static$init' in $om));
} // __CFG__
    vo.$$bind('$$fxntab', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Settings', vo);
    pkg.$unitNames.$add('Settings');

/* ======== INITIALIZATION ======== */

    $om['ti.bios.help.Settings'].module$meta$init();
    pkg.init();
    $om['ti.bios.help.Settings'].$$bless();
