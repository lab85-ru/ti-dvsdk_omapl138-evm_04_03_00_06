#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/examples/ti/sdo/ce/examples/apps/scale/../../../../../..;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linuxutils_2_26_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/biosutils_1_02_02/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/edma3lld_01_11_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dspbios_5_41_03_17/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dsplink_1_65_01_05_eng
override XDCROOT = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36
override XDCBUILDCFG = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/examples/ti/sdo/ce/examples/apps/scale/../../../../../../config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = DEVICES="OMAPL138" PROGRAMS="APP_CLIENT DSP_SERVER" GPPOS="LINUX_GCC" CGTOOLS_V5T=/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linux-devkit/ CC_V5T=bin/arm-arago-linux-gnueabi-gcc CGTOOLS_C64P=/opt/ti-dvsdk_omapl138-evm_04_03_00_06/cgt6x_6_1_14 CGTOOLS_C674=/opt/ti-dvsdk_omapl138-evm_04_03_00_06/cgt6x_6_1_14 CGTOOLS_UCARM9=/db/toolsrc/library/tools/vendors/opensource/buildroot/10122007/build_arm/staging_dir/usr WINCE_ROOTDIR=C:/WINCE600 WINCE_PROJECTROOT=C:/WINCE600/_your_ProjectRoot_/Wince600/TI_EVM_3530_ARMV4I CGTOOLS_LINUX86=_your_Linux86_installation_directory
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/examples/ti/sdo/ce/examples/apps/scale/../../../../../..;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linuxutils_2_26_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/biosutils_1_02_02/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/edma3lld_01_11_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dspbios_5_41_03_17/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dsplink_1_65_01_05_eng;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages;../../../../../..
HOSTOS = Linux
endif
