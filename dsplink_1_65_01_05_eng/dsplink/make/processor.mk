#   ============================================================================
#   @file   processor.mk
#
#   @path   $(DSPLINK)/make/
#
#   @desc   This makefile defines device-specific macros used by MAKE system.
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


ifndef PROCESSOR_MK

define PROCESSOR_MK
endef


#   ============================================================================
#   Define the macro for target.
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
TARGET              := GPP
else
TARGET              := DSP
endif

#   ============================================================================
#   Define the directory hierarchy for exported headers & targets
#   ============================================================================
ifneq ("$(TI_DSPLINK_GPPTEMPPATH)", "")
GPP_EXPORT          := $(TI_DSPLINK_GPPTEMPPATH)$(DIRSEP)export
else
GPP_EXPORT          := $(TI_DSPLINK_GPPROOT)$(DIRSEP)export
endif
ifneq ("$(TI_DSPLINK_DSPTEMPPATH)", "")
DSP_EXPORT          := $(TI_DSPLINK_DSPTEMPPATH)$(DIRSEP)export
else
DSP_EXPORT          := $(TI_DSPLINK_DSPROOT)$(DIRSEP)export
endif

ifeq ("$(DEVICETYPE)", "GPP")
EXPORT_DIR          := $(GPP_EXPORT)
else
EXPORT_DIR          := $(DSP_EXPORT)
endif


#   ============================================================================
#   Define the directory hierarchy for generated objects and binaries
#   ============================================================================
ifneq ("$(TI_DSPLINK_GPPTEMPPATH)", "")
GPP_BUILD          := $(TI_DSPLINK_GPPTEMPPATH)$(DIRSEP)BUILD
else
GPP_BUILD          := $(TI_DSPLINK_GPPROOT)$(DIRSEP)BUILD
endif
ifneq ("$(TI_DSPLINK_DSPTEMPPATH)", "")
DSP_BUILD          := $(TI_DSPLINK_DSPTEMPPATH)$(DIRSEP)BUILD
else
DSP_BUILD          := $(TI_DSPLINK_DSPROOT)$(DIRSEP)BUILD
endif

ifeq ("$(DEVICETYPE)", "GPP")
BUILD_DIR           := $(GPP_BUILD)
else
BUILD_DIR           := $(DSP_BUILD)
endif


#   ============================================================================
#   Specify the configuration file to be used for this build
#   ============================================================================
CONFIG_DEVICES  := $(DSPLINK)$(DIRSEP)config$(DIRSEP)all
CONFIG_OS_C     := $(DSPLINK)$(DIRSEP)config$(DIRSEP)all$(DIRSEP)CFG_$(TI_DSPLINK_GPPOS).c
CONFIG_GPP_C    := $(DSPLINK)$(DIRSEP)config$(DIRSEP)all$(DIRSEP)CFG_$(TI_DSPLINK_GPPDEVICE).c
CONFIG_SYS_C    := $(DSPLINK)$(DIRSEP)config$(DIRSEP)all$(DIRSEP)CFG_system.c
CONFIG_DSP_C    := $(foreach file, $(TI_DSPLINK_DSPCFGFILES), $(DSPLINK)$(DIRSEP)config$(DIRSEP)all$(DIRSEP)$(file))

#   ============================================================================
#   Specify the root directory for the sources.
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
DEVICEROOT          := $(TI_DSPLINK_GPPROOT)
else
DEVICEROOT          := $(TI_DSPLINK_DSPROOT)
endif

#   ============================================================================
#   Set the OS to build for, based on the device type.
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
export BUILDOS         := $(TI_DSPLINK_GPPOS)
export BUILDOSVERSION  := $(TI_DSPLINK_GPPOSVERSION)
else
export BUILDOS         := $(TI_DSPLINK_DSPOS)
export BUILDOSVERSION  := $(TI_DSPLINK_DSPOSVERSION)
endif
endif   # ifndef PROCESSOR_MK
