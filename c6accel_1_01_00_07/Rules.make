###################################################################################
#  ======== Rules.make ========
#  This file specified variables used by the makefiles. After editing this file
#  you should not have to modify any of Makefiles to build this product.
#
#  The DVSDK_INSTALL_DIR variable is a local support variable only used in
#  this file and does not need to be set if individual components are used.
###################################################################################
#=========================================================
# If C6Accel package is found under DVSDK Root Directory
# Set only the DVSDK INSTALL Directory below
#=========================================================
DVSDK_INSTALL_DIR=$(HOME)/ti-dvsdk_omapl138-evm_4_01_00_09
-include $(DVSDK_INSTALL_DIR)/Rules.make
EXEC_DIR_C6ACCEL = $(EXEC_DIR)/c6accel_app

ifeq ($(PLATFORM),omapl138)
# RTSC platform
PLATFORM_XDC = ti.platforms.evmOMAPL138

# ARM Instruction set
ARM_ISA = armv5t

# DSP Instruction set
DSP_ISA = 674

else

# RTSC platform
PLATFORM_XDC = ti.platforms.evm3530

# ARM Instruction set
ARM_ISA = armv7-a

# DSP Instruction set
DSP_ISA = 64P

endif



#==============================================================================
# Set the below paths as needed if not previously defined above  
# according to the location where components have been installed/placed
#==============================================================================
ifndef PLATFORM
# Define target platform.
PLATFORM=omapl138

ifeq ($(PLATFORM),omapl138)
# Where the DVSDK is installed.
DVSDK_INSTALL_DIR=$(HOME)/ti-dvsdk_da850-omapl138-evm_4_00_00_06

# Where the Codec Engine package is installed.
CE_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/codec-engine_2_25_02_11

# Where the XDAIS package is installed.
XDAIS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/xdais_6_25_02_11

# Where the DSP Link package is installed.
LINK_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/dsplink_1_65_00_02

# Where the CMEM (contiguous memory allocator) package is installed.
CMEM_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/linuxutils_2_25_02_08

# Where the EDMA3 Low Level Driver is installed.
EDMA3_LLD_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/edma3lld_01_11_00_03

# Where the TI C6x codegen tool is installed.
CODEGEN_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/cgt6x_6_1_14

# Where the RTSC tools package is installed.
XDC_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/xdctools_3_16_03_36

# Where Framework Components product is installed.
FC_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/framework-components_2_25_01_05

# Where DSP/BIOS is installed.
BIOS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/dspbios_5_41_03_17

# Where biosutils package is installed.
BIOSUTILS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/biosutils_1_02_02

SERVER_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/codecs-omapl138_1_10_00_01

# RTSC platform
PLATFORM_XDC = ti.platforms.evmOMAPL138

# ARM Instruction set
ARM_ISA = armv5t

# DSP Instruction set
DSP_ISA = 674

else

# Where the DVSDK is installed.
DVSDK_INSTALL_DIR=$(HOME)/ti-dvsdk_dm3730-am3715-evm_4_00_07142010

# Where the Codec Engine package is installed.
CE_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/codec-engine_2_25_02_11

# Where the XDAIS package is installed.
XDAIS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/xdais_6_25_02_11

# Where the DSP Link package is installed.
LINK_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/dsplink_1_65_00_02

# Where the CMEM (contiguous memory allocator) package is installed.
CMEM_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/linuxutils_2_25_02_08

# Where the EDMA3 Low Level Driver is installed.
EDMA3_LLD_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/edma3_lld_01_11_00_03

# Where the TI C6x codegen tool is installed.
CODEGEN_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/cgt6x_6_1_14

# Where the RTSC tools package is installed.
XDC_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/xdctools_3_16_03_36

# Where Framework Components product is installed.
FC_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/framework-components_2_25_01_05

# Where DSP/BIOS is installed.
BIOS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/dspbios_5_41_03_17

# Where biosutils package is installed.
BIOSUTILS_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/biosutils_1_02_02

# Where the local power manager is installed.
LPM_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/local-power-manager_1_24_02_09

# RTSC platform
PLATFORM_XDC = ti.platforms.evm3530

# ARM Instruction set
ARM_ISA = armv7-a

# DSP Instruction set
DSP_ISA = 64P

endif

C6ACCEL_INSTALL_DIR=  $(DVSDK_INSTALL_DIR)/c6accel_1_01_00_01

# The prefix to be added before the GNU compiler tools (optionally including
# path), i.e. "arm_v5t_le-" or "/opt/bin/arm_v5t_le-".
CSTOOL_DIR=/home/user/CodeSourcery/Sourcery_G++_Lite
CSTOOL_PREFIX=$(CSTOOL_DIR)/bin/arm-none-linux-gnueabi-
MVTOOL_PREFIX=$(CSTOOL_PREFIX)

# Where to copy the resulting executables and data to (when executing 'make
# install') in a proper file structure. This EXEC_DIR should either be visible
# from the target, or you will have to copy this (whole) directory onto the
# target filesystem.
EXEC_DIR         = /$(HOME)/targetfs/opt
EXEC_DIR_C6ACCEL = $(EXEC_DIR)/c6accel_ver11

endif
