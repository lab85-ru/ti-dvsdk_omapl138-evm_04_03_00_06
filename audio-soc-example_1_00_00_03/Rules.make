# --COPYRIGHT--,BSD
#  Copyright (c) 2010, Texas Instruments Incorporated
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
# 
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# --/COPYRIGHT--

#------------------------------------------------------------------------------
# Please do not modify anything from here (optionally including SDK Rules.make)
-include $(ROOT_DIR)/../Rules.make
#------------------------------------------------------------------------------

#==============================================================================
# Platform for which the application will be built for 
# if not previous defined in above Rules.make
#==============================================================================
ifndef PLATFORM
PLATFORM = omapl138
endif

#==============================================================================
# Set the below paths as needed if not previously defined above according 
# to the location where componenets have been installed/placed
#==============================================================================

# Path to the SDK root directory 
SDK_INSTALL_DIR	?= /home/user/OMAP_L138_arm_1_00_00_08

# Path to ARM Linux Toolchain
CSTOOL_DIR ?= /home/user/CodeSourcery/Sourcery_G++_Lite

# Path to the Linux kernel root dir
LINUXKERNEL_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/DaVinci-PSP-SDK-03.20.00.08/src/kernel/linux-03.20.00.08

# Path to DSP/BIOS root dir
BIOS_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/bios_5_41_02_14

# Path to XDC Tools root dir
XDC_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/xdctools_3_16_01_27

# Path to DSP C6000 Code Generation Tools root dir
CODEGEN_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/cg6x_6_1_9

# Path to BIOS Link root directory
LINK_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/dsplink_linux_1_65_00_01

# Path to the DSP-side Platform Support Package driver root dir
BIOSPSP_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/BIOSPSP_01_30_00

# Path the DSP-side EDMA3 Low Level Driver root dir 
EDMA3LLD_INSTALL_DIR ?= $(SDK_INSTALL_DIR)/edma3_lld_01_10_00_01


