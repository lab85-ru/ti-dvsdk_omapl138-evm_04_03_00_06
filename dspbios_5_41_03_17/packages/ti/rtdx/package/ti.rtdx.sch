xdc.loadCapsule('xdc/om2.xs');

var $om = xdc.om;
var __CFG__ = $om.$name == 'cfg';
var __ROV__ = $om.$name == 'rov';
var $$pkgspec = xdc.$$ses.findPkg('ti.rtdx');

/* ======== IMPORTS ======== */

    xdc.loadPackage('xdc');
    xdc.loadPackage('xdc.corevers');

/* ======== OBJECTS ======== */

// package ti.rtdx
    var pkg = $om.$$bind('ti.rtdx.Package', $$PObj());
    $om.$$bind('ti.rtdx', $$VObj('ti.rtdx', pkg));
// module Settings
    var po = $om.$$bind('ti.rtdx.Settings.Module', $$PObj());
    var vo = $om.$$bind('ti.rtdx.Settings', $$VObj('ti.rtdx.Settings', po));
    pkg.$$bind('Settings', vo);
// module RtdxModule
    var po = $om.$$bind('ti.rtdx.RtdxModule.Module', $$PObj());
    var vo = $om.$$bind('ti.rtdx.RtdxModule', $$VObj('ti.rtdx.RtdxModule', po));
    pkg.$$bind('RtdxModule', vo);
    $om.$$bind('ti.rtdx.RtdxModule.TransferProtocol', $$PEnm('ti.rtdx.RtdxModule.TransferProtocol'));

/* ======== CONSTS ======== */

// module Settings
// module RtdxModule
    $om.$$bind('ti.rtdx.RtdxModule.PROTOCOL_SIMULATION', $$Enum($om['ti.rtdx.RtdxModule.TransferProtocol'], 'ti.rtdx.RtdxModule.PROTOCOL_SIMULATION', 0));
    $om.$$bind('ti.rtdx.RtdxModule.PROTOCOL_JTAG', $$Enum($om['ti.rtdx.RtdxModule.TransferProtocol'], 'ti.rtdx.RtdxModule.PROTOCOL_JTAG', 1));
    $om.$$bind('ti.rtdx.RtdxModule.PROTOCOL_HIGH_SPEED', $$Enum($om['ti.rtdx.RtdxModule.TransferProtocol'], 'ti.rtdx.RtdxModule.PROTOCOL_HIGH_SPEED', 2));

/* ======== CREATES ======== */

// module Settings
// module RtdxModule

/* ======== FUNCTIONS ======== */

// fxn Settings.getIncludes
    var fxn = $om.$$bind('ti.rtdx.Settings$$getIncludes', $$PFxn($om['ti.rtdx.Settings.Module'], $$T_Str, 1, 1, false));
        fxn.$$arg(0, 'targetName', $$T_Str, undefined);
// fxn Settings.getLibPath
    var fxn = $om.$$bind('ti.rtdx.Settings$$getLibPath', $$PFxn($om['ti.rtdx.Settings.Module'], $$T_Str, 1, 1, false));
        fxn.$$arg(0, 'targetName', $$T_Str, undefined);

/* ======== SIZES ======== */


/* ======== TYPES ======== */

// module Settings
    var cap = $om.$$bind('ti.rtdx.Settings$$capsule', xdc.loadCapsule('ti/rtdx/Settings.xs'));
    var po = $om['ti.rtdx.Settings.Module'].$$init('ti.rtdx.Settings.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        var fxn = cap['module$meta$init'];
        if (fxn) $om.$$bind('ti.rtdx.Settings$$module$meta$init', true);
        if (fxn) po.$$fxn('module$meta$init', $$T_Met, fxn);
        po.$$fxn('getIncludes', $om['ti.rtdx.Settings$$getIncludes'], cap['getIncludes']);
        po.$$fxn('getLibPath', $om['ti.rtdx.Settings$$getLibPath'], cap['getLibPath']);
// module RtdxModule
    var po = $om['ti.rtdx.RtdxModule.Module'].$$init('ti.rtdx.RtdxModule.Module', $$Module);
        po.$$fld('$hostonly', $$T_Num, 1, 'r');
        po.$$fld('protocol', $om['ti.rtdx.RtdxModule.TransferProtocol'], $om['ti.rtdx.RtdxModule.PROTOCOL_JTAG'], 'wh');
        po.$$fld('bufferSizeInWords', $$T_CNum('(xdc_UInt)'), 256, 'wh');
        po.$$fld('noAutoInit', $$T_Bool, false, 'wh');
        po.$$fld('overrideLegacyLib', $$T_Bool, false, 'wh');

/* ======== ROV ======== */

if (__ROV__) {


} // __ROV__

/* ======== SINGLETONS ======== */

// package ti.rtdx
    var po = $om['ti.rtdx.Package'].$$init('ti.rtdx.Package', $om['xdc.IPackage.Module']);
    var cap = $om.$$bind('xdc.IPackage$$capsule', xdc.loadCapsule('ti/rtdx/package.xs'));
        if (cap['init']) po.$$fxn('init', $om['xdc.IPackage$$init'], cap['init']);
        if (cap['close']) po.$$fxn('close', $om['xdc.IPackage$$close'], cap['close']);
        if (cap['validate']) po.$$fxn('validate', $om['xdc.IPackage$$validate'], cap['validate']);
        if (cap['exit']) po.$$fxn('exit', $om['xdc.IPackage$$exit'], cap['exit']);
        if (cap['getLibs']) po.$$fxn('getLibs', $om['xdc.IPackage$$getLibs'], cap['getLibs']);
        if (cap['getSects']) po.$$fxn('getSects', $om['xdc.IPackage$$getSects'], cap['getSects']);
    po.$$bind('$capsule', cap);
    var pkg = $om['ti.rtdx'].$$init(po, 'ti.rtdx', $$DEFAULT, false);
    $om.$packages.$add(pkg);
    pkg.$$bind('$name', 'ti.rtdx');
    pkg.$$bind('$category', 'Package');
    pkg.$$bind('$$qn', 'ti.rtdx.');
    pkg.$$bind('$spec', $$pkgspec);
    pkg.$$bind('$vers', [2, 0, 0, 5]);
    pkg.$attr.$seal('length');
    pkg.$$bind('$imports', [
    ]);
    if (pkg.$vers.length >= 3) {
        pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));
    }
    
    pkg.build.libraries = [
        'lib/release/rtdx.a7e',
        'lib/debug/rtdxhs_noInit.a67',
        'lib/release/rtdxsim.a67e',
        'lib/release/rtdxsim.a67Pe',
        'lib/debug/rtdxhs.a67Pe',
        'lib/release/rtdx.a64Pe',
        'lib/release/rtdxsim.a62',
        'lib/debug/rtdxhs_noInit.a67Pe',
        'lib/debug/rtdx_noInit.a67e',
        'lib/release/rtdxsim.a64',
        'lib/debug/rtdx.a64P',
        'lib/debug/rtdxhs.a62e',
        'lib/release/rtdxsim.a67',
        'lib/release/rtdxhs.a674',
        'lib/release/rtdxhs_noInit.a67Pe',
        'lib/debug/rtdx470_arm7_9_32_le.a470',
        'lib/debug/rtdxsim.a28FP',
        'lib/release/rtdx.a67Pe',
        'lib/debug/rtdx.a55H',
        'lib/debug/rtdxsim.a674',
        'lib/debug/rtdx.a64e',
        'lib/debug/rtdx.a55L',
        'lib/release/rtdx.a64P',
        'lib/release/rtdxsim.a62e',
        'lib/release/rtdx_noInit.a62',
        'lib/release/rtdxhs.a67P',
        'lib/release/rtdx_noInit.a64',
        'lib/release/rtdx_noInit.a64e',
        'lib/debug/rtdx.a64Pe',
        'lib/debug/rtdx_noInit.a62e',
        'lib/release/rtdx_noInit.a67',
        'lib/debug/rtdx.a9t',
        'lib/release/rtdx.a55H',
        'lib/debug/rtdxhs.a64P',
        'lib/release/rtdx.a64e',
        'lib/release/rtdx.a55L',
        'lib/debug/rtdx470_arm7_9_16_le.a470',
        'lib/debug/rtdxsim.a67P',
        'lib/debug/rtdxsim.a28L',
        'lib/release/rtdx470_arm7_9_32_be.a470e',
        'lib/release/rtdxhs.a67e',
        'lib/debug/rtdx.a67Pe',
        'lib/debug/rtdx.am3',
        'lib/debug/rtdx_noInit.a67Pe',
        'lib/release/rtdx470_arm7_9_32_le.a470',
        'lib/debug/rtdx.a674',
        'lib/debug/rtdxsim.a67e',
        'lib/debug/rtdx.a62',
        'lib/release/rtdxhs_noInit.a67P',
        'lib/release/rtdx.a9t',
        'lib/release/rtdxsim.a64P',
        'lib/release/rtdxhs.a64Pe',
        'lib/debug/rtdx.a64',
        'lib/release/rtdxsim.a28FP',
        'lib/debug/rtdx.a67',
        'lib/release/rtdxhs.a62',
        'lib/debug/rtdx470_arm7_9_16_be.a470e',
        'lib/release/rtdx470_arm7_9_16_le.a470',
        'lib/release/rtdxhs.a67',
        'lib/debug/rtdxsim.a62',
        'lib/release/rtdxhs_noInit.a62',
        'lib/debug/rtdxsim.a64',
        'lib/debug/rtdxsim.a67',
        'lib/release/rtdxhs_noInit.a67',
        'lib/release/rtdxsim.a55H',
        'lib/release/rtdxhs.a62e',
        'lib/release/rtdx.am3',
        'lib/release/rtdx.a674',
        'lib/release/rtdxhs.a67Pe',
        'lib/release/rtdxhs_noInit.a67e',
        'lib/release/rtdx.a28FP',
        'lib/release/rtdxsim.a64e',
        'lib/release/rtdxsim.a55L',
        'lib/debug/rtdx.a67P',
        'lib/debug/rtdx.a28L',
        'lib/debug/rtdx_noInit.a64e',
        'lib/release/rtdx.a62',
        'lib/debug/rtdxhs_noInit.a67P',
        'lib/release/rtdx.a64',
        'lib/debug/rtdxsim.a62e',
        'lib/debug/rtdxsim.a64Pe',
        'lib/release/rtdx_noInit.a67P',
        'lib/release/rtdx.a67',
        'lib/debug/rtdxhs.a674',
        'lib/debug/rtdx_noInit.a62',
        'lib/debug/rtdx_noInit.a64',
        'lib/debug/rtdx.a67e',
        'lib/debug/rtdx_noInit.a67',
        'lib/release/rtdx.a67P',
        'lib/release/rtdx.a28L',
        'lib/debug/rtdxsim.a67Pe',
        'lib/debug/rtdxhs_noInit.a67e',
        'lib/release/rtdx_noInit.a67e',
        'lib/release/rtdxhs_noInit.a62e',
        'lib/release/rtdxhs.a64P',
        'lib/debug/rtdx.a28FP',
        'lib/debug/rtdx.am3e',
        'lib/debug/rtdx.a7',
        'lib/release/rtdx_noInit.a67Pe',
        'lib/debug/rtdx.a9',
        'lib/release/rtdxsim.a674',
        'lib/debug/rtdxhs.a62',
        'lib/debug/rtdxhs.a67P',
        'lib/release/rtdx.a67e',
        'lib/debug/rtdxhs.a67',
        'lib/debug/rtdxsim.a64P',
        'lib/debug/rtdx470_arm7_9_32_be.a470e',
        'lib/debug/rtdx.a62e',
        'lib/debug/rtdx.a7e',
        'lib/release/rtdx.am3e',
        'lib/debug/rtdxhs.a67e',
        'lib/debug/rtdxhs_noInit.a62e',
        'lib/release/rtdx_noInit.a62e',
        'lib/debug/rtdxsim.a55H',
        'lib/release/rtdxsim.a67P',
        'lib/release/rtdxsim.a28L',
        'lib/debug/rtdxsim.a64e',
        'lib/debug/rtdxsim.a55L',
        'lib/release/rtdx.a7',
        'lib/release/rtdxsim.a64Pe',
        'lib/debug/rtdxhs.a64Pe',
        'lib/release/rtdx.a9',
        'lib/debug/rtdx_noInit.a67P',
        'lib/release/rtdx.a62e',
        'lib/release/rtdx470_arm7_9_16_be.a470e',
        'lib/debug/rtdxhs_noInit.a62',
    ];
    
    pkg.build.libDesc = [
        [
            'lib/release/rtdx.a7e',
            {
                target: 'ti.targets.arm.Arm7_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdxsim.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/release/rtdxsim.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/release/rtdx.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/release/rtdxsim.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/release/rtdxsim.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/debug/rtdx.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/debug/rtdxhs.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdxsim.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdxhs.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx470_arm7_9_32_le.a470',
            {
                target: 'ti.targets.TMS470'
            }
        ],
        [
            'lib/debug/rtdxsim.a28FP',
            {
                target: 'ti.targets.C28_float'
            }
        ],
        [
            'lib/release/rtdx.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a55H',
            {
                target: 'ti.targets.C55_huge'
            }
        ],
        [
            'lib/debug/rtdxsim.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/debug/rtdx.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a55L',
            {
                target: 'ti.targets.C55_large'
            }
        ],
        [
            'lib/release/rtdx.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/release/rtdxsim.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdx_noInit.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/release/rtdxhs.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx_noInit.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/release/rtdx_noInit.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdx_noInit.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/debug/rtdx.a9t',
            {
                target: 'ti.targets.arm.Arm9t'
            }
        ],
        [
            'lib/release/rtdx.a55H',
            {
                target: 'ti.targets.C55_huge'
            }
        ],
        [
            'lib/debug/rtdxhs.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/release/rtdx.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/release/rtdx.a55L',
            {
                target: 'ti.targets.C55_large'
            }
        ],
        [
            'lib/debug/rtdx470_arm7_9_16_le.a470',
            {
                target: 'ti.targets.TMS470'
            }
        ],
        [
            'lib/debug/rtdxsim.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/debug/rtdxsim.a28L',
            {
                target: 'ti.targets.C28_large'
            }
        ],
        [
            'lib/release/rtdx470_arm7_9_32_be.a470e',
            {
                target: 'ti.targets.TMS470_big_endian'
            }
        ],
        [
            'lib/release/rtdxhs.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.am3',
            {
                target: 'ti.targets.arm.M3'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/release/rtdx470_arm7_9_32_le.a470',
            {
                target: 'ti.targets.TMS470'
            }
        ],
        [
            'lib/debug/rtdx.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/debug/rtdxsim.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a9t',
            {
                target: 'ti.targets.arm.Arm9t'
            }
        ],
        [
            'lib/release/rtdxsim.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/release/rtdxhs.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/release/rtdxsim.a28FP',
            {
                target: 'ti.targets.C28_float'
            }
        ],
        [
            'lib/debug/rtdx.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdxhs.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdx470_arm7_9_16_be.a470e',
            {
                target: 'ti.targets.TMS470_big_endian'
            }
        ],
        [
            'lib/release/rtdx470_arm7_9_16_le.a470',
            {
                target: 'ti.targets.TMS470'
            }
        ],
        [
            'lib/release/rtdxhs.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/debug/rtdxsim.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdxsim.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/debug/rtdxsim.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdxsim.a55H',
            {
                target: 'ti.targets.C55_huge'
            }
        ],
        [
            'lib/release/rtdxhs.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdx.am3',
            {
                target: 'ti.targets.arm.M3'
            }
        ],
        [
            'lib/release/rtdx.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/release/rtdxhs.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/release/rtdx.a28FP',
            {
                target: 'ti.targets.C28_float'
            }
        ],
        [
            'lib/release/rtdxsim.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/release/rtdxsim.a55L',
            {
                target: 'ti.targets.C55_large'
            }
        ],
        [
            'lib/debug/rtdx.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/debug/rtdx.a28L',
            {
                target: 'ti.targets.C28_large'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/release/rtdx.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/debug/rtdxsim.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/debug/rtdxsim.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/release/rtdx_noInit.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/debug/rtdxhs.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a64',
            {
                target: 'ti.targets.C64'
            }
        ],
        [
            'lib/debug/rtdx.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/release/rtdx.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a28L',
            {
                target: 'ti.targets.C28_large'
            }
        ],
        [
            'lib/debug/rtdxsim.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/release/rtdx_noInit.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/release/rtdxhs_noInit.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdxhs.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/debug/rtdx.a28FP',
            {
                target: 'ti.targets.C28_float'
            }
        ],
        [
            'lib/debug/rtdx.am3e',
            {
                target: 'ti.targets.arm.M3_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a7',
            {
                target: 'ti.targets.arm.Arm7'
            }
        ],
        [
            'lib/release/rtdx_noInit.a67Pe',
            {
                target: 'ti.targets.C67P_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a9',
            {
                target: 'ti.targets.arm.Arm9'
            }
        ],
        [
            'lib/release/rtdxsim.a674',
            {
                target: 'ti.targets.C674'
            }
        ],
        [
            'lib/debug/rtdxhs.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
        [
            'lib/debug/rtdxhs.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs.a67',
            {
                target: 'ti.targets.C67'
            }
        ],
        [
            'lib/debug/rtdxsim.a64P',
            {
                target: 'ti.targets.C64P'
            }
        ],
        [
            'lib/debug/rtdx470_arm7_9_32_be.a470e',
            {
                target: 'ti.targets.TMS470_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/debug/rtdx.a7e',
            {
                target: 'ti.targets.arm.Arm7_big_endian'
            }
        ],
        [
            'lib/release/rtdx.am3e',
            {
                target: 'ti.targets.arm.M3_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs.a67e',
            {
                target: 'ti.targets.C67_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdx_noInit.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/debug/rtdxsim.a55H',
            {
                target: 'ti.targets.C55_huge'
            }
        ],
        [
            'lib/release/rtdxsim.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdxsim.a28L',
            {
                target: 'ti.targets.C28_large'
            }
        ],
        [
            'lib/debug/rtdxsim.a64e',
            {
                target: 'ti.targets.C64_big_endian'
            }
        ],
        [
            'lib/debug/rtdxsim.a55L',
            {
                target: 'ti.targets.C55_large'
            }
        ],
        [
            'lib/release/rtdx.a7',
            {
                target: 'ti.targets.arm.Arm7'
            }
        ],
        [
            'lib/release/rtdxsim.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs.a64Pe',
            {
                target: 'ti.targets.C64P_big_endian'
            }
        ],
        [
            'lib/release/rtdx.a9',
            {
                target: 'ti.targets.arm.Arm9'
            }
        ],
        [
            'lib/debug/rtdx_noInit.a67P',
            {
                target: 'ti.targets.C67P'
            }
        ],
        [
            'lib/release/rtdx.a62e',
            {
                target: 'ti.targets.C62_big_endian'
            }
        ],
        [
            'lib/release/rtdx470_arm7_9_16_be.a470e',
            {
                target: 'ti.targets.TMS470_big_endian'
            }
        ],
        [
            'lib/debug/rtdxhs_noInit.a62',
            {
                target: 'ti.targets.C62'
            }
        ],
    ];
    if ('suffix' in xdc.om['xdc.IPackage$$LibDesc']) {
        pkg.build.libDesc['lib/release/rtdx.a7e'].suffix = '7e';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdxsim.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/release/rtdxsim.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdxhs.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/release/rtdx.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/release/rtdxsim.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/release/rtdxsim.a64'].suffix = '64';
        pkg.build.libDesc['lib/debug/rtdx.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/debug/rtdxhs.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdxsim.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdxhs.a674'].suffix = '674';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdx470_arm7_9_32_le.a470'].suffix = '470';
        pkg.build.libDesc['lib/debug/rtdxsim.a28FP'].suffix = '28FP';
        pkg.build.libDesc['lib/release/rtdx.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdx.a55H'].suffix = '55H';
        pkg.build.libDesc['lib/debug/rtdxsim.a674'].suffix = '674';
        pkg.build.libDesc['lib/debug/rtdx.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/debug/rtdx.a55L'].suffix = '55L';
        pkg.build.libDesc['lib/release/rtdx.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/release/rtdxsim.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdx_noInit.a62'].suffix = '62';
        pkg.build.libDesc['lib/release/rtdxhs.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx_noInit.a64'].suffix = '64';
        pkg.build.libDesc['lib/release/rtdx_noInit.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/debug/rtdx.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdx_noInit.a67'].suffix = '67';
        pkg.build.libDesc['lib/debug/rtdx.a9t'].suffix = '9t';
        pkg.build.libDesc['lib/release/rtdx.a55H'].suffix = '55H';
        pkg.build.libDesc['lib/debug/rtdxhs.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/release/rtdx.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/release/rtdx.a55L'].suffix = '55L';
        pkg.build.libDesc['lib/debug/rtdx470_arm7_9_16_le.a470'].suffix = '470';
        pkg.build.libDesc['lib/debug/rtdxsim.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/debug/rtdxsim.a28L'].suffix = '28L';
        pkg.build.libDesc['lib/release/rtdx470_arm7_9_32_be.a470e'].suffix = '470e';
        pkg.build.libDesc['lib/release/rtdxhs.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/debug/rtdx.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdx.am3'].suffix = 'm3';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/release/rtdx470_arm7_9_32_le.a470'].suffix = '470';
        pkg.build.libDesc['lib/debug/rtdx.a674'].suffix = '674';
        pkg.build.libDesc['lib/debug/rtdxsim.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/debug/rtdx.a62'].suffix = '62';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a9t'].suffix = '9t';
        pkg.build.libDesc['lib/release/rtdxsim.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/release/rtdxhs.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/debug/rtdx.a64'].suffix = '64';
        pkg.build.libDesc['lib/release/rtdxsim.a28FP'].suffix = '28FP';
        pkg.build.libDesc['lib/debug/rtdx.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdxhs.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdx470_arm7_9_16_be.a470e'].suffix = '470e';
        pkg.build.libDesc['lib/release/rtdx470_arm7_9_16_le.a470'].suffix = '470';
        pkg.build.libDesc['lib/release/rtdxhs.a67'].suffix = '67';
        pkg.build.libDesc['lib/debug/rtdxsim.a62'].suffix = '62';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdxsim.a64'].suffix = '64';
        pkg.build.libDesc['lib/debug/rtdxsim.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdxsim.a55H'].suffix = '55H';
        pkg.build.libDesc['lib/release/rtdxhs.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdx.am3'].suffix = 'm3';
        pkg.build.libDesc['lib/release/rtdx.a674'].suffix = '674';
        pkg.build.libDesc['lib/release/rtdxhs.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/release/rtdx.a28FP'].suffix = '28FP';
        pkg.build.libDesc['lib/release/rtdxsim.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/release/rtdxsim.a55L'].suffix = '55L';
        pkg.build.libDesc['lib/debug/rtdx.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/debug/rtdx.a28L'].suffix = '28L';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/release/rtdx.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a64'].suffix = '64';
        pkg.build.libDesc['lib/debug/rtdxsim.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/debug/rtdxsim.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/release/rtdx_noInit.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a67'].suffix = '67';
        pkg.build.libDesc['lib/debug/rtdxhs.a674'].suffix = '674';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a64'].suffix = '64';
        pkg.build.libDesc['lib/debug/rtdx.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a67'].suffix = '67';
        pkg.build.libDesc['lib/release/rtdx.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a28L'].suffix = '28L';
        pkg.build.libDesc['lib/debug/rtdxsim.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/release/rtdx_noInit.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/release/rtdxhs_noInit.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdxhs.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/debug/rtdx.a28FP'].suffix = '28FP';
        pkg.build.libDesc['lib/debug/rtdx.am3e'].suffix = 'm3e';
        pkg.build.libDesc['lib/debug/rtdx.a7'].suffix = '7';
        pkg.build.libDesc['lib/release/rtdx_noInit.a67Pe'].suffix = '67Pe';
        pkg.build.libDesc['lib/debug/rtdx.a9'].suffix = '9';
        pkg.build.libDesc['lib/release/rtdxsim.a674'].suffix = '674';
        pkg.build.libDesc['lib/debug/rtdxhs.a62'].suffix = '62';
        pkg.build.libDesc['lib/debug/rtdxhs.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/debug/rtdxhs.a67'].suffix = '67';
        pkg.build.libDesc['lib/debug/rtdxsim.a64P'].suffix = '64P';
        pkg.build.libDesc['lib/debug/rtdx470_arm7_9_32_be.a470e'].suffix = '470e';
        pkg.build.libDesc['lib/debug/rtdx.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/debug/rtdx.a7e'].suffix = '7e';
        pkg.build.libDesc['lib/release/rtdx.am3e'].suffix = 'm3e';
        pkg.build.libDesc['lib/debug/rtdxhs.a67e'].suffix = '67e';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdx_noInit.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/debug/rtdxsim.a55H'].suffix = '55H';
        pkg.build.libDesc['lib/release/rtdxsim.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdxsim.a28L'].suffix = '28L';
        pkg.build.libDesc['lib/debug/rtdxsim.a64e'].suffix = '64e';
        pkg.build.libDesc['lib/debug/rtdxsim.a55L'].suffix = '55L';
        pkg.build.libDesc['lib/release/rtdx.a7'].suffix = '7';
        pkg.build.libDesc['lib/release/rtdxsim.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/debug/rtdxhs.a64Pe'].suffix = '64Pe';
        pkg.build.libDesc['lib/release/rtdx.a9'].suffix = '9';
        pkg.build.libDesc['lib/debug/rtdx_noInit.a67P'].suffix = '67P';
        pkg.build.libDesc['lib/release/rtdx.a62e'].suffix = '62e';
        pkg.build.libDesc['lib/release/rtdx470_arm7_9_16_be.a470e'].suffix = '470e';
        pkg.build.libDesc['lib/debug/rtdxhs_noInit.a62'].suffix = '62';
    }
    
// module Settings
    var vo = $om['ti.rtdx.Settings'];
    var po = $om['ti.rtdx.Settings.Module'];
    vo.$$init(po, 'ti.rtdx.Settings', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.rtdx.Settings'));
    vo.$$bind('$capsule', $om['ti.rtdx.Settings$$capsule']);
    vo.$$bind('$package', $om['ti.rtdx']);
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
    vo.$$bind('$$meta_iobj', 0 + ('ti.rtdx.Settings$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr.$seal('length');
    pkg.$$bind('Settings', vo);
    pkg.$unitNames.$add('Settings');
// module RtdxModule
    var vo = $om['ti.rtdx.RtdxModule'];
    var po = $om['ti.rtdx.RtdxModule.Module'];
    vo.$$init(po, 'ti.rtdx.RtdxModule', $$DEFAULT, false);
    vo.$$bind('Module', po);
    vo.$$bind('$category', 'Module');
    vo.$$bind('$spec', xdc.$$ses.findUnit('ti.rtdx.RtdxModule'));
    vo.$$bind('$capsule', undefined);
    vo.$$bind('$package', $om['ti.rtdx']);
    vo.$$bind('$$tdefs', []);
    vo.$$bind('$$proxies', []);
    vo.$$bind('$$mcfgs', []);
    vo.$$bind('TransferProtocol', $om['ti.rtdx.RtdxModule.TransferProtocol']);
    vo.$$bind('PROTOCOL_SIMULATION', $om['ti.rtdx.RtdxModule.PROTOCOL_SIMULATION']);
    vo.$$bind('PROTOCOL_JTAG', $om['ti.rtdx.RtdxModule.PROTOCOL_JTAG']);
    vo.$$bind('PROTOCOL_HIGH_SPEED', $om['ti.rtdx.RtdxModule.PROTOCOL_HIGH_SPEED']);
    pkg.$modules.$add(vo);
    $om.$modules.$add(vo);
    vo.$$bind('$$instflag', 0);
    vo.$$bind('$$iobjflag', 1);
    vo.$$bind('$$sizeflag', 1);
    vo.$$bind('$$dlgflag', 0);
    vo.$$bind('$$iflag', 0);
    vo.$$bind('$$romcfgs', '|');
    if ('Module_State' in vo) vo.$$bind('$object', new vo.Module_State);
    vo.$$bind('$$meta_iobj', 0 + ('ti.rtdx.RtdxModule$$instance$static$init' in $om));
    vo.$$bind('$$fxntab', []);
    vo.$$bind('$$logEvtCfgs', []);
    vo.$$bind('$$errorDescCfgs', []);
    vo.$$bind('$$assertDescCfgs', []);
    vo.$attr['@Template:"./RtdxModule.xdt"'] = "./RtdxModule.xdt";
    vo.$attr.$seal('length');
    vo.$$bind('TEMPLATE$', './RtdxModule.xdt');
    pkg.$$bind('RtdxModule', vo);
    pkg.$unitNames.$add('RtdxModule');

/* ======== INITIALIZATION ======== */

if (__CFG__) {
} // __CFG__
    $om['ti.rtdx.Settings'].module$meta$init();
    $om['ti.rtdx.RtdxModule'].module$meta$init();
    pkg.init();
    $om['ti.rtdx.Settings'].$$bless();
    $om['ti.rtdx.RtdxModule'].$$bless();
