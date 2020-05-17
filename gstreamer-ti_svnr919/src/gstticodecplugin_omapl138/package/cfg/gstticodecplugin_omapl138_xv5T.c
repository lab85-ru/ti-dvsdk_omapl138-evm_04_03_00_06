/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-u17
 */

#define __nested__
#define __config__

#include <xdc/std.h>

/*
 * ======== GENERATED SECTIONS ========
 *     
 *     MODULE INCLUDES
 *     
 *     <module-name> INTERNALS
 *     <module-name> INHERITS
 *     <module-name> VTABLE
 *     <module-name> PATCH TABLE
 *     <module-name> DECLARATIONS
 *     <module-name> OBJECT OFFSETS
 *     <module-name> TEMPLATES
 *     <module-name> INITIALIZERS
 *     <module-name> FUNCTION STUBS
 *     <module-name> PROXY BODY
 *     <module-name> OBJECT DESCRIPTOR
 *     <module-name> SYSTEM FUNCTIONS
 *     <module-name> PRAGMAS
 *     
 *     INITIALIZATION ENTRY POINT
 *     PROGRAM GLOBALS
 *     CLINK DIRECTIVES
 */


/*
 * ======== MODULE INCLUDES ========
 */



/*
 * ======== xdc.cfg.Program TEMPLATE ========
 */

/*
 *  ======== __ASM__ ========
 *  Define absolute path to program's assembly directory
 */
xdc__META(__ASM__, "@(#)__ASM__ = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/gstreamer-ti_svnr919/src/gstticodecplugin_omapl138/package/cfg/gstticodecplugin_omapl138_xv5T");

/*
 *  ======== __ISA__ ========
 *  Define the ISA of this executable.  This symbol is used by platform
 *  specific "exec" commands that support more than one ISA; e.g., gdb
 */
xdc__META(__ISA__, "@(#)__ISA__ = v5T");

/*
 *  ======== __PLAT__ ========
 *  Define the name of the platform that can run this executable.  This
 *  symbol is used by platform independent "exec" commands
 */
xdc__META(__PLAT__, "@(#)__PLAT__ = ti.platforms.evmOMAPL138");

/*
 *  ======== __TARG__ ========
 *  Define the name of the target used to build this executable.
 */
xdc__META(__TARG__, "@(#)__TARG__ = gnu.targets.arm.GCArmv5T");


/*
 * ======== ti.sdo.ce.ipc.Settings TEMPLATE ========
 */


/*
 * ======== ti.sdo.ce.osal.Global TEMPLATE ========
 */



/*
 *  ======== Global Configuration ========
 *  Do not modify the following; it is automatically generated from the template
 *  Server.xdt in the ti.sdo.ce package and will be overwritten next time the
 *  executable is configured
 */
#include <ti/sdo/ce/osal/Log.h>

Log_Handle ti_sdo_ce_dvtLog = NULL; 

Bool Memory_skipVirtualAddressTranslation = FALSE;


/* List of all packages and their versions and paths, for diagnostics */
String Global_buildInfo[] = {
    "    package gnu.targets.arm.rtsv5T (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/gnu/targets/arm/rtsv5T/) [1, 0, 0, 0]",
    "    package ti.c6accel (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/packages/ti/c6accel/) [1, 0, 0]",
    "    package ti.sdo.codecs.aachedec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/aachedec/) [1, 0, 0]",
    "    package ti.sdo.codecs.g711dec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/g711dec/) [1, 0, 0]",
    "    package ti.sdo.codecs.g711enc (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/g711enc/) [1, 0, 0]",
    "    package ti.sdo.codecs.h264dec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/h264dec/) [1, 0, 0]",
    "    package ti.sdo.codecs.h264enc (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/h264enc/) [1, 0, 0]",
    "    package ti.sdo.codecs.jpegdec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/jpegdec/) [1, 0, 0]",
    "    package ti.sdo.codecs.jpegenc (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/jpegenc/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg2dec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg2dec/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg4dec (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg4dec/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg4enc (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg4enc/) [1, 0, 0]",
    "    package ti.sdo.ce.global (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/global/) [1, 0, 0]",
    "    package ti.xdais.dm (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages/ti/xdais/dm/) [1, 0, 5]",
    "    package ti.sdo.utils.trace (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/utils/trace/) [1, 0, 0]",
    "    package ti.sdo.ce.utils.xdm (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/utils/xdm/) [1, 0, 2]",
    "    package dsplink.gpp (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dsplink_1_65_01_05_eng/dsplink/gpp/) [5, 0, 0]",
    "    package ti.sdo.linuxutils.cmem (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linuxutils_2_26_02_05/packages/ti/sdo/linuxutils/cmem/) [2, 2, 0]",
    "    package gnu.targets (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/gnu/targets/) [1, 0, 1]",
    "    package gnu.targets.arm (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/gnu/targets/arm/) [1, 0, 0, 0]",
    "    package ti.xdais (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages/ti/xdais/) [1, 2.0, 1]",
    "    package ti.sdo.ce.node (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/node/) [1, 0, 0]",
    "    package ti.sdo.fc.global (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/fc/global/) [1, 0, 0]",
    "    package ti.sdo.fc.memutils (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/fc/memutils/) [1, 0, 0]",
    "    package ti.sdo.fc.utils (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/fc/utils/) [1, 0, 2]",
    "    package ti.sdo.fc.dman3 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/fc/dman3/) [1, 0, 4]",
    "    package ti.sdo.fc.acpy3 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages/ti/sdo/fc/acpy3/) [1, 0, 4]",
    "    package ti.catalog.arm (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/ti/catalog/arm/) [1, 0, 1, 0]",
    "    package ti.catalog (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/ti/catalog/) [1, 0, 0]",
    "    package ti.catalog.c6000 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/ti/catalog/c6000/) [1, 0, 0, 0]",
    "    package ti.platforms.evmOMAPL138 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages/ti/platforms/evmOMAPL138/) [1, 0, 0]",
    "    package ti.sdo.ce.osal (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/osal/) [2, 0, 2]",
    "    package ti.sdo.ce.osal.linux (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/osal/linux/) [2, 0, 1]",
    "    package ti.sdo.ce.ipc (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/ipc/) [2, 0, 1]",
    "    package ti.sdo.ce.ipc.dsplink (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/ipc/dsplink/) [2, 0, 1]",
    "    package ti.sdo.ce.alg (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/alg/) [1, 0, 1]",
    "    package ti.sdo.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/) [1, 0, 6]",
    "    package ti.sdo.ce.universal (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/universal/) [1, 0, 0]",
    "    package ti.c6accel.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/packages/ti/c6accel/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.audio1 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/audio1/) [1, 0, 1]",
    "    package ti.sdo.codecs.aachedec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/aachedec/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.speech1 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/speech1/) [1, 0, 1]",
    "    package ti.sdo.codecs.g711dec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/g711dec/ce/) [1, 0, 0]",
    "    package ti.sdo.codecs.g711enc.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/g711enc/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.video2 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/video2/) [1, 0, 2]",
    "    package ti.sdo.codecs.h264dec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/h264dec/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.video1 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/video1/) [1, 0, 2]",
    "    package ti.sdo.codecs.h264enc.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/h264enc/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.image1 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/image1/) [1, 0, 1]",
    "    package ti.sdo.codecs.jpegdec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/jpegdec/ce/) [1, 0, 0]",
    "    package ti.sdo.codecs.jpegenc.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/jpegenc/ce/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg2dec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg2dec/ce/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg4dec.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg4dec/ce/) [1, 0, 0]",
    "    package ti.sdo.codecs.mpeg4enc.ce (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages/ti/sdo/codecs/mpeg4enc/ce/) [1, 0, 0]",
    "    package ti.sdo.ce.speech (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/speech/) [1, 0, 2]",
    "    package ti.sdo.ce.audio (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/audio/) [1, 0, 2]",
    "    package ti.sdo.ce.video (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/video/) [1, 0, 3]",
    "    package ti.sdo.ce.image (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages/ti/sdo/ce/image/) [1, 0, 3]",
    "    package ti.sdo.dmai (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dmai_2_20_00_15/packages/ti/sdo/dmai/) [1, 0, 0]",
    "    package gstticodecplugin_omapl138 (/opt/ti-dvsdk_omapl138-evm_04_03_00_06/gstreamer-ti_svnr919/src/gstticodecplugin_omapl138/) []",
    NULL
};

/*
 * ======== ti.sdo.ce.Engine TEMPLATE ========
 */

/*
 *  ======== Engine Configuration ========
 *  Do not modify following; it is automatically generated from the template
 *  Engine.xdt in the ti.sdo.ce package and will be overwritten next time the
 *  executable is configured
 */
#include <ti/sdo/ce/Engine.h>
#include <ti/xdais/ialg.h>

#include <ti/xdais/idma3.h>

#include <ti/xdais/ires.h>

/* type tables for algorithms */
extern IALG_Fxns G711DEC_TII_IG711DEC;
static String typeTab_G711DEC_TII_IG711DEC[] = {
    "ti.sdo.ce.speech1.ISPHDEC1",
    NULL
};

extern IALG_Fxns H264VENC_TI_IH264VENC;
static String typeTab_H264VENC_TI_IH264VENC[] = {
    "ti.sdo.ce.video1.IVIDENC1",
    NULL
};

extern IALG_Fxns DMJPGE_TIGEM_IDMJPGE;
static String typeTab_DMJPGE_TIGEM_IDMJPGE[] = {
    "ti.sdo.ce.image1.IIMGENC1",
    NULL
};

extern IALG_Fxns JPEGDEC_TI_IJPEGDEC;
static String typeTab_JPEGDEC_TI_IJPEGDEC[] = {
    "ti.sdo.ce.image1.IIMGDEC1",
    NULL
};

extern IALG_Fxns C6ACCEL_TI_IC6ACCEL;
static String typeTab_C6ACCEL_TI_IC6ACCEL[] = {
    "ti.sdo.ce.universal.IUNIVERSAL",
    NULL
};

extern IALG_Fxns AACDEC_TII_IAACDEC;
static String typeTab_AACDEC_TII_IAACDEC[] = {
    "ti.sdo.ce.audio1.IAUDDEC1",
    NULL
};

extern IALG_Fxns H264VDEC_TI_IH264VDEC;
static String typeTab_H264VDEC_TI_IH264VDEC[] = {
    "ti.sdo.ce.video2.IVIDDEC2",
    NULL
};

extern IALG_Fxns MPEG2VDEC_TI_IALG;
static String typeTab_MPEG2VDEC_TI_IALG[] = {
    "ti.sdo.ce.video2.IVIDDEC2",
    NULL
};

extern IALG_Fxns MP4VENC_TI_IMP4VENC;
static String typeTab_MP4VENC_TI_IMP4VENC[] = {
    "ti.sdo.ce.video1.IVIDENC1",
    NULL
};

extern IALG_Fxns M4H3DEC_TI_IM4H3DEC;
static String typeTab_M4H3DEC_TI_IM4H3DEC[] = {
    "ti.sdo.ce.video2.IVIDDEC2",
    NULL
};

extern IALG_Fxns G711ENC_TII_IG711ENC;
static String typeTab_G711ENC_TII_IG711ENC[] = {
    "ti.sdo.ce.speech1.ISPHENC1",
    NULL
};


/* tables of engine algorithms */
extern IALG_Fxns UNIVERSAL_STUBS;
extern IALG_Fxns AUDDEC1_STUBS;
extern IALG_Fxns SPHDEC1_STUBS;
extern IALG_Fxns SPHENC1_STUBS;
extern IALG_Fxns VIDDEC2_STUBS;
extern IDMA3_Fxns H264VDEC_TI_IDMA3;
extern IALG_Fxns VIDENC1_STUBS;
extern IDMA3_Fxns H264VENC_TI_IDMA3;
extern IALG_Fxns IMGDEC1_STUBS;
extern IALG_Fxns IMGENC1_STUBS;
extern IALG_Fxns VIDDEC2_STUBS;
extern IDMA3_Fxns MPEG2VDEC_TI_IDMA3;
extern IALG_Fxns VIDDEC2_STUBS;
extern IDMA3_Fxns M4H3DEC_TI_IDMA3;
extern IALG_Fxns VIDENC1_STUBS;
extern IDMA3_Fxns MP4VENC_TI_IDMA3;

/* algorithm-specific stub/skeleton config data objects */
extern struct ti_sdo_codecs_h264dec_ce_H264DEC_CodecClassConfig ti_sdo_codecs_h264dec_ce_H264DEC_codecClassConfig;
extern struct ti_sdo_codecs_h264enc_ce_H264ENC_CodecClassConfig ti_sdo_codecs_h264enc_ce_H264ENC_codecClassConfig;
extern struct ti_sdo_codecs_jpegdec_ce_JPEGDEC_CodecClassConfig ti_sdo_codecs_jpegdec_ce_JPEGDEC_codecClassConfig;
extern struct ti_sdo_codecs_jpegenc_ce_JPEGENC_CodecClassConfig ti_sdo_codecs_jpegenc_ce_JPEGENC_codecClassConfig;
extern struct ti_sdo_codecs_mpeg2dec_ce_MPEG2DEC_CodecClassConfig ti_sdo_codecs_mpeg2dec_ce_MPEG2DEC_codecClassConfig;
extern struct ti_sdo_codecs_mpeg4dec_ce_MPEG4DEC_CodecClassConfig ti_sdo_codecs_mpeg4dec_ce_MPEG4DEC_codecClassConfig;
extern struct ti_sdo_codecs_mpeg4enc_ce_MPEG4ENC_CodecClassConfig ti_sdo_codecs_mpeg4enc_ce_MPEG4ENC_codecClassConfig;

static Engine_AlgDesc engineAlgs0[] = {
    {
        "c6accel",       /* name */
        {1490041256U},   /* uuid */
        &UNIVERSAL_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_C6ACCEL_TI_IC6ACCEL,              /* typeTab */
        FALSE,          /* isLocal */
        2,        /* groupId */
        1,      /* protocol */
        NULL,         /* iresFxns */
        NULL,               /* stub/skel config params  */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.universal.IUNIVERSAL",
    },
    {
        "aachedec",       /* name */
        {1869133733U},   /* uuid */
        &AUDDEC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_AACDEC_TII_IAACDEC,              /* typeTab */
        FALSE,          /* isLocal */
        2,        /* groupId */
        1,      /* protocol */
        NULL,         /* iresFxns */
        NULL,               /* stub/skel config params  */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.audio1.IAUDDEC1",
    },
    {
        "g711dec",       /* name */
        {1323247091U},   /* uuid */
        &SPHDEC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_G711DEC_TII_IG711DEC,              /* typeTab */
        FALSE,          /* isLocal */
        2,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        NULL,               /* stub/skel config params  */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.speech1.ISPHDEC1",
    },
    {
        "g711enc",       /* name */
        {3593455834U},   /* uuid */
        &SPHENC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_G711ENC_TII_IG711ENC,              /* typeTab */
        FALSE,          /* isLocal */
        2,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        NULL,               /* stub/skel config params  */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.speech1.ISPHENC1",
    },
    {
        "h264dec",       /* name */
        {2701911697U},   /* uuid */
        &VIDDEC2_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_H264VDEC_TI_IH264VDEC,              /* typeTab */
        FALSE,          /* isLocal */
        0,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_h264dec_ce_H264DEC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.video2.IVIDDEC2",
    },
    {
        "h264enc",       /* name */
        {972753848U},   /* uuid */
        &VIDENC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_H264VENC_TI_IH264VENC,              /* typeTab */
        FALSE,          /* isLocal */
        0,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_h264enc_ce_H264ENC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.video1.IVIDENC1",
    },
    {
        "jpegdec",       /* name */
        {1183555817U},   /* uuid */
        &IMGDEC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_JPEGDEC_TI_IJPEGDEC,              /* typeTab */
        FALSE,          /* isLocal */
        1,        /* groupId */
        1,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_jpegdec_ce_JPEGDEC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.image1.IIMGDEC1",
    },
    {
        "jpegenc",       /* name */
        {3732622784U},   /* uuid */
        &IMGENC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_DMJPGE_TIGEM_IDMJPGE,              /* typeTab */
        FALSE,          /* isLocal */
        1,        /* groupId */
        1,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_jpegenc_ce_JPEGENC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.image1.IIMGENC1",
    },
    {
        "mpeg2dec",       /* name */
        {2974277761U},   /* uuid */
        &VIDDEC2_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_MPEG2VDEC_TI_IALG,              /* typeTab */
        FALSE,          /* isLocal */
        0,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_mpeg2dec_ce_MPEG2DEC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.video2.IVIDDEC2",
    },
    {
        "mpeg4dec",       /* name */
        {3351107257U},   /* uuid */
        &VIDDEC2_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_M4H3DEC_TI_IM4H3DEC,              /* typeTab */
        FALSE,          /* isLocal */
        0,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_mpeg4dec_ce_MPEG4DEC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.video2.IVIDDEC2",
    },
    {
        "mpeg4enc",       /* name */
        {3544745280U},   /* uuid */
        &VIDENC1_STUBS,             /* fxns */
        NULL,        /* idma3Fxns */
        typeTab_MP4VENC_TI_IMP4VENC,              /* typeTab */
        FALSE,          /* isLocal */
        0,        /* groupId */
        0,      /* protocol */
        NULL,         /* iresFxns */
        &ti_sdo_codecs_mpeg4enc_ce_MPEG4ENC_codecClassConfig, /* stub/skel config params */
        Engine_USECACHEDMEM_DEFAULT,    /* memType */
        "ti.sdo.ce.video1.IVIDENC1",
    },
    {NULL},
};
static Engine_AlgDesc engineAlgs1[] = {
    {NULL},
};

/* table of all engines available in this application */
static Engine_Desc engineTab[] = {
    {"codecServer",	/* engine name */
      engineAlgs0,	/* alg table */
      "/usr/share/ti/ti-codecs-server/cs.x674",		/* (optional) server name */
      NULL,        /* (optional) Link config ID */
      11 /* number of algs in alg table */
    },
    {"local",	/* engine name */
      engineAlgs1,	/* alg table */
      NULL,		/* (optional) server name */
      NULL,        /* (optional) Link config ID */
      0 /* number of algs in alg table */
    },
    {NULL, NULL, NULL, 0}	/* NULL-terminate the engine table */
};

Engine_Config Engine_config = {
    engineTab,		    /* table of all engines */
    "local"   /* local RMS engine name */
};

Bool ti_sdo_ce_Engine_noCommName = FALSE;

/*
 * ======== ti.sdo.ce.CERuntime TEMPLATE ========
 */


#include <ti/sdo/ce/osal/Global.h>
#include <ti/sdo/ce/ipc/Comm.h>
#include <ti/sdo/ce/osal/Memory.h>
#include <ti/sdo/ce/osal/Queue.h>
#include <ti/sdo/ce/osal/Sem.h>
#include <ti/sdo/ce/osal/SemMP.h>
#include <ti/sdo/ce/osal/Thread.h>
#include <ti/sdo/ce/ipc/Processor.h>
#include <ti/sdo/ce/alg/Algorithm.h>
#include <ti/sdo/ce/osal/Lock.h>
#include <ti/sdo/ce/osal/LockMP.h>
#include <ti/sdo/ce/Server.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/rms.h>
#include <ti/sdo/ce/utils/xdm/XdmUtils.h>

#include <ti/sdo/utils/trace/gt.h>


/*
 *  ======== CERuntime_init ========
 */
Void CERuntime_init(Void)
{
    extern Void IPC_generatedInit();

    GT_init();


    /* if CE_DEBUG is set, turn on tracing and DSP auto trace collection */
    if (Global_getenv("CE_DEBUG") != NULL) {
        extern Bool   Engine_alwaysCollectDspTrace;
        extern String Engine_ceDebugDspTraceMask;

        Engine_alwaysCollectDspTrace = TRUE;



        if (Global_getenv("CE_DEBUG")[0] == '1') {
            GT_set("*+67,CE-3,GT_time=0,GT_prefix=1235");
            Engine_ceDebugDspTraceMask = "*+67,GT_prefix=1235,GT_time=3";
        }
        else if (Global_getenv("CE_DEBUG")[0] == '2') {
            GT_set(
                "*+01234567,CE-3,ti.sdo.ce.osal.SemMP=67,OG=467,OM=4567,OC=67,GT_time=0,GT_prefix=1235");
            Engine_ceDebugDspTraceMask =
                "*+01234567,CR=67,ti.sdo.fc.dman3-2,ti.sdo.fc.dskt2-2,GT_prefix=1235,GT_time=3";
        } else {
            GT_set("*+01234567,CE-3,GT_time=0,GT_prefix=12345");
            Engine_ceDebugDspTraceMask = "*+01234567,GT_prefix=12345,GT_time=3";
        }
    }

    if (Global_getenv("CE_CHECK") != NULL) {
        extern Bool VISA_checked;

        /*
         * Currently just change _this_ processor's value... perhaps we should
         * enable remote processors as well?
         */
        if (Global_getenv("CE_CHECK")[0] == '1') {
            VISA_checked = TRUE;
            /* turn on all GT_7CLASS trace (errors) */
            GT_set("*+7");
        } else if (Global_getenv("CE_CHECK")[0] == '0') {
            VISA_checked = FALSE;
        } else {
            /* leave it whatever it was... maybe we should drop a warning? */
        }
    }

    /* allow user to over-ride via CE_TRACE. */
    GT_set(Global_getenv("CE_TRACE"));
    IPC_generatedInit();
    Global_init();

    Sem_init();
    SemMP_init();


    Memory_init();
    Queue_init();
    Comm_init();
    Thread_init();
    Processor_init();
    LockMP_init();  /* Must be called before DMAN3_init() */
    Algorithm_init();
    XdmUtils_init();
    Lock_init();

    Engine_init();
    Server_init();

}

/*
 *  ======== CERuntime_exit ========
 */
Void CERuntime_exit(Void)
{
    Global_exit();
}

/* for backward compatibility with xdc-m based tools */
Void ti_sdo_ce_CERuntime_init__F(Void) {
    CERuntime_init();
}

/*
 * ======== ti.sdo.ce.Settings TEMPLATE ========
 */

/*
 *  ======== ti.sdo.ce Settings Configuration ========
 *  Do not modify following; it is automatically generated from the template
 *  Settings.xdt in the ti.sdo.ce package and will be overwritten next time the
 *  executable is configured
 */
Bool VISA_checked = FALSE;

/*
 * ======== ti.sdo.ce.video2.VIDDEC2Config TEMPLATE ========
 */


/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IVIDDEC2_CodecClassConfig from viddec2.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_mpeg2dec_ce_MPEG2DEC_CodecClassConfig {
    Bool manageInBufsCache     [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache    [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_mpeg2dec_ce_MPEG2DEC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};
/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IVIDDEC2_CodecClassConfig from viddec2.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_h264dec_ce_H264DEC_CodecClassConfig {
    Bool manageInBufsCache     [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache    [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_h264dec_ce_H264DEC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};
/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IVIDDEC2_CodecClassConfig from viddec2.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_mpeg4dec_ce_MPEG4DEC_CodecClassConfig {
    Bool manageInBufsCache     [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache    [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_mpeg4dec_ce_MPEG4DEC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};

/*
 * ======== ti.sdo.ce.video1.VIDENC1Config TEMPLATE ========
 */


/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IVIDENC1_CodecClassConfig from videnc1.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_mpeg4enc_ce_MPEG4ENC_CodecClassConfig {
    Bool manageInBufsCache   [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache  [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_mpeg4enc_ce_MPEG4ENC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};
/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IVIDENC1_CodecClassConfig from videnc1.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_h264enc_ce_H264ENC_CodecClassConfig {
    Bool manageInBufsCache   [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache  [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_h264enc_ce_H264ENC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};

/*
 * ======== ti.sdo.ce.video1.VIDDEC1Config TEMPLATE ========
 */



/*
 * ======== ti.sdo.ce.image1.IMGDEC1Config TEMPLATE ========
 */


/*
 * This data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IIMGDEC1_CodecClassConfig from imgdec1.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_jpegdec_ce_JPEGDEC_CodecClassConfig {
    Bool manageInBufsCache   [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache  [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_jpegdec_ce_JPEGDEC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};

/*
 * ======== ti.sdo.ce.image1.IMGENC1Config TEMPLATE ========
 */


/* this data structure has codec-specific names and values, but class-specific
 * fields -- it's indentical to struct IIMGENC1_CodecClassConfig from imgecn1.h.
 * Its name doesn't matter, only that the address of data is captured in the
 * "codecClassConfig" field of the algorithm description
 */    
struct ti_sdo_codecs_jpegenc_ce_JPEGENC_CodecClassConfig {
    Bool manageInBufsCache   [ XDM_MAX_IO_BUFFERS ];  
    Bool manageOutBufsCache  [ XDM_MAX_IO_BUFFERS ];  
} ti_sdo_codecs_jpegenc_ce_JPEGENC_codecClassConfig = {
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE},
    {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE}
};

/*
 * ======== ti.sdo.ce.ipc.dsplink.Ipc TEMPLATE ========
 */


/* Max timeout for MSGQ_get() allowed. */
UInt32 ti_sdo_ce_ipc_dsplink_Ipc_maxTimeout = 4294967295UL;

Void IPC_generatedInit()
{
}

/* configuration for interprocessor communication */

/* size of the communication message */
UInt32 Comm_MSGSIZE = 4096;

/* number of Comm_MSGSIZE-sized messages in the system */
UInt32 Comm_NUMMSGS = 64;

/* configure number of retries Comm_locate should do before giving up */
UInt32 Comm_LOCATERETRIES = 20;


/* Arm-side DSPLINK configuration
 * ==============================
 */


#include <ti/sdo/ce/osal/Global.h>

/* Arm-side DspLink configuration tables { */


/* Arm-side DspLink memory map for server "/usr/share/ti/ti-codecs-server/cs.x674":
 *
 * table entry format (osal/Global.h), used by DSP/BIOS Link
 *     name of the memory section
 *     DSP virtual address (start of memory section)
 *     physical address (start of memory section)
 *     size in bytes of the memory region
 *     shared access memory
 *     synchronized
 */
static Global_ArmDspLinkConfigMemTableEntry armDspLinkConfigMemTable__usr_share_ti_ti_codecs_server_cs_x674[] = {
    { "DDR2", 0xC3200000, 0xC3200000, 0x00300000, 1, 0 },
    { "DSPLINKMEM", 0xC3501000, 0xC3501000, 0x000FF000, 1, 0 },
    { "RESET_VECTOR", 0xC3500000, 0xC3500000, 0x00001000, 1, 0 },
    { "DDRALGHEAP", 0xC3600000, 0xC3600000, 0x00A00000, 0, 0 },
    { NULL, 0, 0, 0, 0, 0 }
};


/* Arm-side DspLink configuration table for server "/usr/share/ti/ti-codecs-server/cs.x674": */
static Global_ArmDspLinkConfig ti_sdo_ce_ipc_armDspLinkConfig__usr_share_ti_ti_codecs_server_cs_x674 = {
    armDspLinkConfigMemTable__usr_share_ti_ti_codecs_server_cs_x674,
    BootPwr
};

/* List of server names for all DspLink configuration tables */
String ti_sdo_ce_ipc_armDspLinkConfigServerNames[] = {
    "/usr/share/ti/ti-codecs-server/cs.x674",
    NULL
};

/* List of matching-positions-by-name DspLink configuration tables for all servers */
Global_ArmDspLinkConfig *ti_sdo_ce_ipc_armDspLinkConfigs[] = {
    &ti_sdo_ce_ipc_armDspLinkConfig__usr_share_ti_ti_codecs_server_cs_x674,
    NULL
};
/* } end of arm-side DspLink configuration tables. */

Int Global_useLinkArbiter = FALSE;

Int LAD_connect(String clientName, Int * id) { return 0; }
Int LAD_disconnect(Int id) { return 0; }
Int LAD_getDspStatus(Int cpuId, Int * statusInfo) { return 0; }
Int LAD_releaseDsp(Int id) { return 0; }
Int LAD_startupDsp(Int id, Int cpuId, Int linkId, String image) { return 0; }


Void Power_init() {}
Int Power_on(Ptr handle) { return 0; }
Int Power_off(Ptr handle) { return 0; }
Int Power_connect(Ptr handle) { return 0; }
Int Power_disconnect(Ptr handle) { return 0; }


/*
 * ======== ti.sdo.ce.video.VIDENCConfig TEMPLATE ========
 */



/*
 * ======== ti.sdo.ce.video.VIDDECConfig TEMPLATE ========
 */



/*
 * ======== ti.sdo.ce.image.IMGDECConfig TEMPLATE ========
 */



/*
 * ======== ti.sdo.ce.image.IMGENCConfig TEMPLATE ========
 */



/*
 * ======== ti.sdo.utils.trace.GT TEMPLATE ========
 */




/*
 * C code contribution from ti/sdo/utils/GT.xdt 
 */
#include <xdc/std.h>
#include <ti/sdo/utils/trace/gt.h>

extern Void GTConfig_tracePrintf(String fmt, ...); 
extern Ptr GTConfig_libcMalloc(Int size);
 
extern Int GTConfig_threadId(void);
extern void GTConfig_error(String fmt, ...);
extern void GTConfig_libcFree(Ptr, Int);
extern UInt32 GTConfig_gettimeofday(void);
extern Int GTConfig_pthreadLock(void);
extern Int GTConfig_pthreadUnlock(void);
extern Int GTConfig_posixInit(void);

GT_Config _ti_sdo_utils_trace_GT_params = {
    GTConfig_tracePrintf,
    NULL,
    GTConfig_threadId,
    GTConfig_error,
    GTConfig_libcMalloc,
    GTConfig_libcFree,
    GTConfig_gettimeofday,
    GTConfig_pthreadLock,
    GTConfig_pthreadUnlock,
    GTConfig_posixInit    
};

GT_Config *GT = &_ti_sdo_utils_trace_GT_params;

/*
 * ======== ti.sdo.ce.osal.linux.Settings TEMPLATE ========
 */

UInt32 ti_sdo_ce_osal_linux_SemMP_ipcKey = 0x4c41534f;

UInt32 Memory_maxCbListSize = 100;


/*
 * ======== ti.sdo.ce.alg.Settings TEMPLATE ========
 */

unsigned int ti_sdo_ce_alg_ALG_useHeap = FALSE;
unsigned int ti_sdo_ce_alg_ALG_useCache = FALSE;

unsigned int ti_sdo_ce_alg_ALG_maxGroups = 20; 

UInt32 ti_sdo_ce_alg_ipcKey = 0x4f474c41;

int _ALG_groupUsed[20] = {
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
};

#include <ti/sdo/ce/osal/SemMP.h>
#include <ti/xdais/ialg.h>

SemMP_Handle _ALG_sems[20];
int _ALG_groupRefCount[20] = {
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
     0,
};

IALG_Handle _Algorithm_lockOwner[20] = {
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
};

// No IDMA3 using algs in the system, and DMAN3 is not configured into the
// system.  Stub out these fxn calls.
Void DMAN3_CE_init()
{
}
Void DMAN3_CE_exit()
{
}

int DMAN3_grantDmaChannels(int groupId, void *algHandles, void *dmaFxns,
        int numAlgs)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (0);
}

int DMAN3_releaseDmaChannels(void *algHandles, void * dmaFxns, int numAlgs)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (0);
}

// No IRES using algs in the system, and RMAN is not configured into the
// system.  Stub out these fxn calls.
#include <ti/xdais/ires.h>   /* for IRES types */

IRES_Status RMAN_activateAllResources(IALG_Handle algHandle, IRES_Fxns *resFxns, Int scratchId)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (IRES_OK);
}

IRES_Status RMAN_assignResources(IALG_Handle algHandle, IRES_Fxns *resFxns, 
        Int scratchGroupId)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (IRES_OK);
}

IRES_Status RMAN_deactivateAllResources(IALG_Handle algHandle, 
        IRES_Fxns * resFxns, Int scratchId)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (IRES_OK);
}

IRES_Status RMAN_freeResources(IALG_Handle algHandle, IRES_Fxns * resFxns, 
        Int scratchGroupId)
{
    /* TODO:M  Should assert(false) here.  Should never be called */
    return (IRES_OK);
}

IRES_Status RMAN_init(void)
{
    return (IRES_OK);
}

IRES_Status RMAN_exit(void)
{
    return (IRES_OK);
}




/*
 * ======== ti.sdo.fc.global.Settings TEMPLATE ========
 */

Bool ti_sdo_fc_multiProcess = TRUE;

/*
 * ======== PROGRAM GLOBALS ========
 */


/*
 * ======== CLINK DIRECTIVES ========
 */

#ifdef __ti__
#define PRAGMA(x) _Pragma(#x)
#define xdc__clink_constant(var) \
PRAGMA(DATA_SECTION(var, ".const:" #var))  \
asm("   .sect \".const:" #var "\"\n"  \
  "   .clink\n"  \
  "   .sect \"[0].const:" #var "\"\n"  \
  "   .clink\n"  \
  "   .sect \"[1].const:" #var "\"\n"  \
  "   .clink")

#endif

