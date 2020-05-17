#   ============================================================================
#   @file   Rules.mk
#
#   @path   $(DSPLINK)/gpp/src/
#
#   @desc   This file contains the configurable items for the kbuild based
#           makefile.
#
#   @ver    1.65.01.05_eng
#   ============================================================================
#   Copyright (C) 2002-2009, Texas Instruments Incorporated -
#   http://www.ti.com/
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   ============================================================================

# Update these macros to reflect your environment.
#
# KERNEL_DIR   = The Linux kernel source directory
# TOOL_PATH    = Path to the toolchain
# MAKE_OPTS    = Architecture-specific Make options
#

ifeq ("$(TI_DSPLINK_GPPOS)", "Linux")

ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAP3530")
KERNEL_DIR    := ${HOME}/omap3530/AM35x-OMAP35x-PSP-SDK-03.00.00.05/src/kernel/linux-03.00.00.05
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAP3530")

ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL138")
KERNEL_DIR    := ${HOME}/DaVinci-PSP-SDK-03.20.00.11/src/kernel/linux-03.20.00.11
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL138")

ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")
KERNEL_DIR    := ${HOME}/DaVinci-PSP-SDK-03.20.00.11/src/kernel/linux-03.20.00.11
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")

ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
KERNEL_DIR    := ${HOME}/DaVinci-PSP-SDK-03.20.00.11/src/kernel/linux-03.20.00.11
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")

ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL1XX")
KERNEL_DIR    := ${HOME}/DaVinci-PSP-SDK-03.20.00.11/src/kernel/linux-03.20.00.11
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "OMAPL1XX")

ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCIHD")
KERNEL_DIR    := ${HOME}/linux-davinci/git
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCIHD")

ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")
KERNEL_DIR    := ${HOME}/linux-davinci/git
TOOL_PATH     := ${HOME}/toolchains/git/arm-2009q1-203/bin
endif #ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")

saved_cross := $(shell cat $(KERNEL_DIR)/include/generated/kernel.cross 2> /dev/null)
CROSS_COMPILE := $(saved_cross)
export CROSS_COMPILE

PATH  := $(TOOL_PATH):$(PATH)
export PATH

ifeq ("$(TI_DSPLINK_GPPOSPREFIX)", "uc")
CG_PREFIX = arm-linux-uclibcgnueabi-
ifneq ($(CROSS_COMPILE),)
MAKE_OPTS = ARCH=arm
else
MAKE_OPTS = ARCH=arm CROSS_COMPILE=$(TOOL_PATH)/arm-linux-uclibcgnueabi-
endif
else
CG_PREFIX = arm-none-linux-gnueabi-
ifneq ($(CROSS_COMPILE),)
MAKE_OPTS = ARCH=arm
else
MAKE_OPTS = ARCH=arm CROSS_COMPILE=$(TOOL_PATH)/arm-none-linux-gnueabi-
endif
endif # ifeq ("$(TI_DSPLINK_GPPOSPREFIX)", "uc")

endif # ifeq ("$(TI_DSPLINK_GPPOS)", "Linux")
