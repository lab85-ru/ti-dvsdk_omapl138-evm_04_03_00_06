#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = 
override XDCROOT = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /opt/ti-dvsdk_omapl138-evm_04_03_00_06/xdctools_3_16_03_36/packages;../..
HOSTOS = Linux
endif
