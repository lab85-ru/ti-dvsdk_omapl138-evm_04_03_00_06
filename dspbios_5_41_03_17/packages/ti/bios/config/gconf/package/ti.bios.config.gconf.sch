xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;


/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');

/* ======== OBJECTS ======== */

// package ti.bios.config.gconf
    var po = $om.$$bind('ti.bios.config.gconf.Package', $$PObj());
    $om.$$bind('ti.bios.config.gconf', $$VObj('ti.bios.config.gconf', po));
// module Settings
    var po = $om.$$bind('ti.bios.config.gconf.Settings.Module', $$PObj());
    $om.$$bind('ti.bios.config.gconf.Settings', $$VObj('ti.bios.config.gconf.Settings', po));

/* ======== CONSTS ======== */

// module Settings

/* ======== CREATES ======== */

// module Settings

/* ======== FUNCTIONS ======== */


/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module Settings
    var po = $om['ti.bios.config.gconf.Settings.Module'].$$init('ti.bios.config.gconf.Settings.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');

/* ======== SINGLETONS ======== */

// package ti.bios.config.gconf
    var po = $om['ti.bios.config.gconf.Package'].$$init('ti.bios.config.gconf.Package', $om['xdc.IPackage.Module']);
    var pkg = $om['ti.bios.config.gconf'].$$init(po, 'ti.bios.config.gconf', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.bios.config.gconf');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.bios.config.gconf.');
    pkg.$$bind('$vers', []);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    var $PKG = pkg;
    pkg.build.libraries = [
    ];
// module Settings
    var vo = $om['ti.bios.config.gconf.Settings'];
    var po = $om['ti.bios.config.gconf.Settings.Module'];
    vo.$$init(po, 'ti.bios.config.gconf.Settings', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$package', $om['ti.bios.config.gconf']);
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
    vo.$$bind('$$meta_iobj', 0 + ('ti.bios.config.gconf.Settings$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Settings', vo);
    pkg.$unitNames.$add('Settings');

/* ======== INITIALIZATION ======== */

    $om['ti.bios.config.gconf.Settings'].module$meta$init();
    pkg.init();
    $om['ti.bios.config.gconf.Settings'].$$bless();
