#############################################################################
# dsplink.mak                                                               #
#                                                                           #
# Targets to check, configure and build DSPLink components                  #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
#     http://www.ti.com/                                                    #
#                                                                           #
#############################################################################
#
#
#############################################################################
#                                                                           #
#  Redistribution and use in source and binary forms, with or without       #
#  modification, are permitted provided that the following conditions       #
#  are met:                                                                 #
#                                                                           #
#    Redistributions of source code must retain the above copyright         #
#    notice, this list of conditions and the following disclaimer.          #
#                                                                           #
#    Redistributions in binary form must reproduce the above copyright      #
#    notice, this list of conditions and the following disclaimer in the    #
#    documentation and/or other materials provided with the                 #
#    distribution.                                                          #
#                                                                           #
#    Neither the name of Texas Instruments Incorporated nor the names of    #
#    its contributors may be used to endorse or promote products derived    #
#    from this software without specific prior written permission.          #
#                                                                           #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS      #
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT        #
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    #
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT     #
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    #
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT         #
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    #
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    #
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      #
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    #
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     #
#                                                                           #
#############################################################################


#   ----------------------------------------------------------------------------
#   Include the top-level Rules.mak file
#   ----------------------------------------------------------------------------
include ../config.mak
include ../Rules.mak


#   ----------------------------------------------------------------------------
#   DSPLink directory definition
#   ----------------------------------------------------------------------------
DSPLINK := $(LINK_INSTALL_DIR)/dsplink

DSPLINKPATH_GPP := $(DSPLINK)/gpp/export/BIN/Linux/$(DSPLINK_PLATFORM)
DSPLINKPATH_GPP_R := $(DSPLINKPATH_GPP)/RELEASE
DSPLINKPATH_GPP_D := $(DSPLINKPATH_GPP)/DEBUG

DSPLINK_MOD_PATH := $(DSPLINKPATH_GPP_R)
MOD_NAME := dsplinkk.ko

# Conditional include of the current DSP link config
-include $(DSPLINK)/config/BUILD/CURRENTCFG.MK


#   ----------------------------------------------------------------------------
#   LPM directory definition
#   ----------------------------------------------------------------------------
LPMLIB_PATH := $(LPM_INSTALL_DIR)/packages/ti/bios/power/src/lpm

LPM_LIBS := lpm.lib
LPMPATH := $(LPM_INSTALL_DIR)/packages/ti/bios/power/lib
LPMPATH_D := $(LPMPATH)/debug
LPMPATH_R := $(LPMPATH)/release
LPM_LIBS_R := $(LPM_LIBS:%.lib=$(LPMPATH_R)/%.av5T)
LPM_LIBS_D := $(LPM_LIBS:%.lib=$(LPMPATH_D)/%.av5T)

LPM_MOD_PATH := $(LPM_INSTALL_DIR)/packages/ti/bios/power/modules/$(LPM_DSP_POWER_SOC)/lpm



#   ----------------------------------------------------------------------------
#   Define if link is configured and/or needs to be configured
#     DSPLink should be configured if the platform has changed
#     or is the particular selected modules have changed
#   ----------------------------------------------------------------------------
ifneq ($(TI_DSPLINK_CFGDATE),)
LINK_IS_CONFIGURED:=1
ifneq ($(TI_DSPLINK_PLATFORM),$(DSPLINK_PLATFORM))
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current configured platform, $(TI_DSPLINK_PLATFORM), is not same as selected platform, $(DSPLINK_PLATFORM)")
else
ifneq ($(TI_DSPLINK_USE_PROC),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use PROC module.")
else
ifneq ($(TI_DSPLINK_USE_POOL),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use POOL module.")
else
ifneq ($(TI_DSPLINK_USE_NOTIFY),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use NOTIFY module.")
else
ifneq ($(TI_DSPLINK_USE_MPCS),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use MPCS module.")
else
ifneq ($(TI_DSPLINK_USE_RINGIO),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use RINGIO module.")
else
ifneq ($(TI_DSPLINK_USE_MPLIST),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use MPLIST module.")
else
ifneq ($(TI_DSPLINK_USE_MSGQ),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use MSGQ module.")
else
ifneq ($(TI_DSPLINK_USE_CHNL),1)
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "Current DSPLink configuration does not use CHNL module.")
else
  LINK_NEEDS_CONFIGURED:=0
endif
endif
endif
endif
endif
endif
endif
endif
endif
else
  LINK_IS_CONFIGURED:=0
  LINK_NEEDS_CONFIGURED:=1
  LINK_CONFIG_CHK_STEP:=$(shell echo "DSPLink configuration does not exist.")
endif


#   ----------------------------------------------------------------------------
#   Compiler and Linker procedure
#   ----------------------------------------------------------------------------
.PHONY:	all config kernel_mod install_kernel_mod

# The default build target.
all: kernel_mod

#   ----------------------------------------------------------------------------
#   Run the DSPLink config step libraries (only if needed)
#   ----------------------------------------------------------------------------
config:
ifeq ($(LINK_NEEDS_CONFIGURED),1)
# Run DSPLink Config Step
ifeq ($(LINK_IS_CONFIGURED),1)
# Remove current config file if it exists
	@rm -Rf $(DSPLINK)/config/BUILD/CURRENTCFG.MK
endif
	@echo "Performing DSPLink platform configuration:"
	@echo "  Reason: $(LINK_CONFIG_CHK_STEP)"  
	@DSPLINK=$(DSPLINK) perl $(DSPLINK)/config/bin/dsplinkcfg.pl \
    --platform=$(DSPLINK_PLATFORM) \
    --dspcfg_0=$(DSPLINK_DSP_PROCESSOR)SHMEM \
    --dspos_0=$(DSPLINK_DSP_OS) \
    --gppos=$(DSPLINK_GPP_OS) \
    --comps=ponslrmc \
    $(DSPLINK_CFG_OPTS) >> /dev/null
# Check for new current config file to verify completion
	@if [ -e $(DSPLINK)/config/BUILD/CURRENTCFG.MK ]; then \
    echo "DSPLink configuration completed."; \
  else \
    echo "DSPLink configuration failed."; \
  fi
endif
   

#   ----------------------------------------------------------------------------
#   Build the DSPLink kernel module for the configured platform, and make sure the
#   kernel_binaries directory is kept in sync. 
#   ----------------------------------------------------------------------------
kernel_mod: config
	@echo -n "Building DSPLink kernel module..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/gpp/src \
          DSPLINK=$(DSPLINK) \
          OBJDUMP=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)objdump \
          CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
          CC=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
          AR=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
          LD=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ld \
          COMPILER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
          ARCHIVER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
          KERNEL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
          TOOL_PATH=$(ARM_TOOLCHAIN_PATH)/bin \
          all >> /dev/null
	@echo "complete!"
ifdef LPM_DSP_POWER_SOC
	@echo -n "Building LPM kernel module..."
	@$(MAKE) -C $(LPM_MOD_PATH) \
    MVTOOL_PREFIX=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
    LINUXKERNEL_INSTALL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
    DSPLINK_REPO="${LINK_INSTALL_DIR}" \
    clean default >> /dev/null
	@echo "complete!"
endif

install_kernel_mod:
ifdef INSTALL_DIR
	@if [ -e $(DSPLINK_MOD_PATH)/$(MOD_NAME) ]; then \
    echo "DSPLink kernel module found. Copying to install directory..."; \
    install -m 755 $(DSPLINK_MOD_PATH)/$(MOD_NAME) $(INSTALL_DIR)/; \
  fi
ifdef LPM_DSP_POWER_SOC
	@if [ -e $(LPM_MOD_PATH)/lpm_*.ko ]; then \
    echo "LPM kernel module found. Copying to install directory..."; \
    install -m 755 $(LPM_MOD_PATH)/lpm_*.ko $(INSTALL_DIR)/; \
  fi 
endif  
else
	@echo "You did not specify a valid INSTALL_DIR variable when running 'make install_kernel_mod'."
endif


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean kernel_mod_clean
clean: kernel_mod_clean

kernel_mod_clean:
	@echo -n "Cleaning up DSPLink kernel module..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/gpp/src \
    DSPLINK=$(DSPLINK) \
    BASE_TOOLCHAIN=$(ARM_TOOLCHAIN_PATH) \
    BASE_BUILDOS=$(LINUXKERNEL_INSTALL_DIR) \
    clean >> /dev/null
	@rm -f $(DSPLINKPATH_GPP_R)/*.ko
	@rm -f $(DSPLINKPATH_GPP_D)/*.ko
	@echo "complete!"
ifdef LPM_DSP_POWER_SOC
	@echo -n "Cleaning up LPM kernel module..."
	@$(MAKE) -C $(LPM_MOD_PATH) \
    MVTOOL_PREFIX=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
    LINUXKERNEL_INSTALL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
    DSPLINK_REPO="${LINK_INSTALL_DIR}" \
    clean >> /dev/null
	@if [ -e $(LPM_MOD_PATH)/lpm_*.ko ]; then \
    rm -Rf $(LPM_MOD_PATH)/lpm_*.ko; \
  fi
	@echo "complete!"
endif
  
# Force a reconfigure next time we build
distclean: clean
	@rm -Rf $(DSPLINK)/config/BUILD/CURRENTCFG.MK
