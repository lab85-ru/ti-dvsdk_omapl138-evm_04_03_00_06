#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/gstreamer-ti_svnr919/src;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dsplink_1_65_01_05_eng;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linuxutils_2_26_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dmai_2_20_00_15/packages;/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/edma3lld_01_11_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/c6accelw;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/packages
override XDCROOT = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/gstreamer-ti_svnr919/src;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dsplink_1_65_01_05_eng;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/framework-components_2_26_00_01/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codec-engine_2_26_02_11/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdais_6_26_01_03/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/codecs-omapl138_4_02_00_00/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linuxutils_2_26_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/dmai_2_20_00_15/packages;/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/edma3lld_01_11_02_05/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/c6accelw;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/c6accel_1_01_00_07/soc/packages;/opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages;..
HOSTOS = Linux
endif
