#
#  ======== Rules.make ========
#  This file specified variables used by the makefiles. After editing this file
#  you should not have to modify any of Makefiles to build this product.
#
#  The DVSDK_INSTALL_DIR variable is a local support variable only used in
#  this file and does not need to be set if individual components are used.
#
DVSDK_INSTALL_DIR=$(HOME)/ti-dvsdk_omapl138-evm_4_00_00_17
-include $(DVSDK_INSTALL_DIR)/Rules.make
DEMO_INSTALL_DIR =  $(DVSDK_INSTALL_DIR)/dsp_benchmark_demo_1_00_00_01
EXEC_DIR_DEMO = $(EXEC_DIR)/dsp_benchmark_demo

#==============================================================================
# Set the below paths as needed if not previously defined above according 
# to the location where componenets have been installed/placed
#==============================================================================
ifndef PLATFORM
# Define target platform.
PLATFORM=omapl138


# Where the DVSDK is installed.
DVSDK_INSTALL_DIR=$(HOME)/ti-dvsdk_omapl138-evm_4_00_00_17

# Where the DSP Link package is installed.
LINK_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/dsplink_1_65_00_02

# Where the CMEM (contiguous memory allocator) package is installed.
CMEM_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/linuxutils_2_25_02_08

# Where the TI C6x codegen tool is installed.
CODEGEN_INSTALL_DIR=$(DVSDK_INSTALL_DIR)/cgt6x_6_1_14

# ARM Instruction set
ARM_ISA = armv5t

# DSP Instruction set
DSP_ISA = 674

C6ACCEL_INSTALL_DIR=  $(DVSDK_INSTALL_DIR)/c6accel_1_01_00_01
DEMO_INSTALL_DIR =  $(DVSDK_INSTALL_DIR)/dsp_benchmark_demo_1_00_00_01
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
EXEC_DIR_DEMO        = /$(HOME)/targetfs/opt/dsp_benchmark_demo

endif
