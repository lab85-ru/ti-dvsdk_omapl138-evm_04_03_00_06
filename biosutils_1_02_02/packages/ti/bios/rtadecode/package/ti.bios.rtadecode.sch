xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.bios.rtadecode
    var po = $om.$$bind('ti.bios.rtadecode.Package', $$PObj());
    $om.$$bind('ti.bios.rtadecode', $$VObj('ti.bios.rtadecode', po));
// module Settings
    var po = $om.$$bind('ti.bios.rtadecode.Settings.Module', $$PObj());
    $om.$$bind('ti.bios.rtadecode.Settings', $$VObj('ti.bios.rtadecode.Settings', po));

/* ======== CONSTS ======== */

// module Settings

/* ======== CREATES ======== */

// module Settings

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module Settings
    var po = $om['ti.bios.rtadecode.Settings.Module'].$$init('ti.bios.rtadecode.Settings.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');

/* ======== SINGLETONS ======== */

// package ti.bios.rtadecode
    var po = $om['ti.bios.rtadecode.Package'].$$init('ti.bios.rtadecode.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.bios.rtadecode'].$$init(po, 'ti.bios.rtadecode', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.bios.rtadecode');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.rtadecode.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];
// module Settings
    var vo = $om['ti.bios.rtadecode.Settings'];
    var po = $om['ti.bios.rtadecode.Settings.Module'];
    vo.$$init(po, 'ti.bios.rtadecode.Settings', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$package', $om['ti.bios.rtadecode']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('$private', {});
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    if ('Module__Object' in vo) vo.$$bind('$object', new vo.Module__Object);
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.rtadecode.Settings$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Settings', vo);
    pkg.$unitNames.$add('Settings');

/* ======== INITIALIZATION ======== */

    $om['ti.bios.rtadecode.Settings'].module$meta$init();
    pkg.init();
    $om['ti.bios.rtadecode.Settings'].$$bless();
