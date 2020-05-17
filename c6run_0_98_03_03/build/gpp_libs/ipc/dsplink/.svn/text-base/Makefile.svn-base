#############################################################################
# Makefile                                                                  #
#                                                                           #
# Targets to check, configure and build GPP-side DSPLink components         #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010-2011 Texas Instruments Incorporated                  #
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
include ../../../../config.mak
include ../../../../Rules.mak

#   ----------------------------------------------------------------------------
#   DSPLink directory definition
#   ----------------------------------------------------------------------------
DSPLINK := $(LINK_INSTALL_DIR)/dsplink
DSPLINKPATH := $(DSPLINK)/gpp/export/BIN/Linux/$(DSPLINK_PLATFORM)

#   ----------------------------------------------------------------------------
#   Common paths
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_GPPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/gpp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)


#   ----------------------------------------------------------------------------
#   Define if link is configured and/or needs to be configured
#     DSPLink should be configured if the platform has changed
#     or is the particular selected modules have changed
#   ----------------------------------------------------------------------------

# Conditional include of the current DSP link config
-include $(DSPLINK)/config/BUILD/CURRENTCFG.MK

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
#   Global variables for building
#   ----------------------------------------------------------------------------
CFLAGS ?= $(ARM_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -std=gnu99 -Wall -Wno-trigraphs \
          -fPIC -fno-strict-aliasing -fno-common -fno-omit-frame-pointer

CINCLUDES := -I$(C6RUN_INSTALL_DIR)/include \
      -I$(DSPLINK)/gpp/inc \
      -I$(DSPLINK)/gpp/inc/usr \
      -I$(DSPLINK)/gpp/inc/sys/Linux \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(C6RUN_GPPLIBS_DIR) \
      -I$(C6RUN_GPPLIBS_DIR)/ipc \
      -I$(C6RUN_GPPLIBS_DIR)/os \
      -I$(C6RUN_GPPLIBS_DIR)/sharedmem \
      -I$(PLATFORMDIR) \
      -I.

DEFS :=  -DOS_LINUX         \
         -DID_GPP=1         \
         -DPROC_COMPONENT   \
         -DPOOL_COMPONENT   \
         -DMSGQ_COMPONENT   \
         -DMSGQ_ZCPY_LINK   \
         -DMAX_DSPS=1       \
         -DMAX_PROCESSORS=2 \
         -DKFILE_DEFAULT

CSRCS = C6Run_ipc.c cio_ipc.c rpc_ipc.c control_ipc.c $(PLATFORMDIR)/DSPLINK_CFG_update.c


#   ----------------------------------------------------------------------------
#   Define input library files
#   ----------------------------------------------------------------------------
LIBS_R := $(DSPLINKPATH)/RELEASE/dsplink.a
LIBS_D := $(DSPLINKPATH)/DEBUG/dsplink.a

ifdef LPM_DSP_POWER_SOC
LIBS_R += $(LPM_INSTALL_DIR)/packages/ti/bios/power/lib/release/lpm.av5T
LIBS_D += $(LPM_INSTALL_DIR)/packages/ti/bios/power/lib/debug/lpm.av5T

CINCLUDES += -I$(LPM_INSTALL_DIR)/packages
endif


#   ----------------------------------------------------------------------------
#   Debug and Release target setup
#   ----------------------------------------------------------------------------
OBJDIR_D      := $(C6RUN_GPPLIBS_DIR)/ipc/debug
OBJDIR_R      := $(C6RUN_GPPLIBS_DIR)/ipc/release

CFLAGS_D      := $(CFLAGS) -g -DDDSP_DEBUG -D__DEBUG -DDEBUG -DVERBOSE 

COBJS_D       := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_D        := $(COBJS_D) $(CPPOBJS_D) $(ASMOBJS_D)

CFLAGS_R      := $(CFLAGS) -O3

COBJS_R       := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_R        := $(COBJS_R) $(CPPOBJS_R) $(ASMOBJS_R)


#   ----------------------------------------------------------------------------
#   Name of the ARM compiler/linker/archiver
#   ----------------------------------------------------------------------------
CC := $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc
AR := $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar


#   ----------------------------------------------------------------------------
#   Compiler/Linker/Archiver procedures
#   ----------------------------------------------------------------------------
.PHONY:	all debug release

# The default build target.
all: debug release

%/.created:
	@mkdir -p $(dir $@)
	@chmod 777 $(dir $@)
	@touch $@

$(OBJDIR_D)/%.o : %.c $(OBJDIR_D)/.created
	@$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -o$@ $<

$(OBJDIR_D)/%.opp : %.cpp $(OBJDIR_D)/.created
	@$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -o$@ $<

$(OBJDIR_R)/%.o : %.c $(OBJDIR_R)/.created
	@$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -o$@ $<

$(OBJDIR_R)/%.opp : %.cpp $(OBJDIR_R)/.created
	@$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -o$@ $<


debug: $(OBJDIR_D)/.created $(LIBS_D) $(OBJS_D)
	@cd $(OBJDIR_D); \
	for i in $(LIBS_D); do \
		$(AR) x $$i; \
	done

release: $(OBJDIR_R)/.created $(LIBS_R) $(OBJS_R)
	@cd $(OBJDIR_R); \
	for i in $(LIBS_R); do \
		$(AR) x $$i; \
	done
  
  
#   ----------------------------------------------------------------------------
#   Run the DSPLink config step libraries (only if needed)
#   ----------------------------------------------------------------------------
.PHONY:	config
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
#   Build the DSPLink gpp libraries
#   ----------------------------------------------------------------------------
$(DSPLINKPATH)/DEBUG/dsplink.a: config $(DSPLINKPATH)/DEBUG/dsplink.lib
	@cp $(DSPLINKPATH)/DEBUG/dsplink.lib $(DSPLINKPATH)/DEBUG/dsplink.o
	@$(AR) rcs $@ $(DSPLINKPATH)/DEBUG/dsplink.o
	@echo "complete!"

$(DSPLINKPATH)/RELEASE/dsplink.a: config $(DSPLINKPATH)/RELEASE/dsplink.lib
	@cp $(DSPLINKPATH)/RELEASE/dsplink.lib $(DSPLINKPATH)/RELEASE/dsplink.o
	@$(AR) rcs $@ $(DSPLINKPATH)/RELEASE/dsplink.o
	@echo "complete!"
  
$(DSPLINKPATH)/DEBUG/dsplink.lib:
	@echo -n "Building DSPLink GPP-side userspace debug library (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/gpp/src/api \
    DSPLINK=$(DSPLINK) \
    BASE_TOOLCHAIN=$(ARM_TOOLCHAIN_PATH) \
    BASE_BUILDOS=$(LINUXKERNEL_INSTALL_DIR) \
    KERNEL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
    CC=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
    AR=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
    LD=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ld \
    COMPILER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
    ARCHIVER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
    CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
    debug >> /dev/null
  
$(DSPLINKPATH)/RELEASE/dsplink.lib:
	@echo -n "Building DSPLink GPP-side userspace release library (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/gpp/src/api \
    DSPLINK=$(DSPLINK) \
    BASE_TOOLCHAIN=$(ARM_TOOLCHAIN_PATH) \
    BASE_BUILDOS=$(LINUXKERNEL_INSTALL_DIR) \
    KERNEL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
    CC=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
    AR=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
    LD=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ld \
    COMPILER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc \
    ARCHIVER=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar \
    CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
    release >> /dev/null


#   ----------------------------------------------------------------------------
#   Build the LPM gpp libraries
#   ----------------------------------------------------------------------------
ifdef LPM_DSP_POWER_SOC
.PHONY: lpm_build

$(LPM_INSTALL_DIR)/packages/ti/bios/power/lib/debug/lpm.av5T: lpm_build
$(LPM_INSTALL_DIR)/packages/ti/bios/power/lib/release/lpm.av5T: lpm_build

lpm_build:
  # Do LPM build for GPP-side libraries
	@echo "/* Generated by ti-local-power-manager.inc OE recipe */" > $(LPM_INSTALL_DIR)/config.bld
	@echo "var Build = xdc.useModule('xdc.bld.BuildEnvironment');" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "var GCArmv5T = xdc.useModule('gnu.targets.arm.GCArmv5T');" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.LONGNAME = \"bin/$(ARM_TOOLCHAIN_PREFIX)gcc\";" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.rootDir = \"$(ARM_TOOLCHAIN_PATH)\";" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.ccOpts.prefix += \" -Wall -fno-strict-aliasing \";" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.lnkOpts.suffix = GCArmv5T.lnkOpts.suffix.replace(\"-lstdc++\", \"\");" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.lnkOpts.suffix += \" -lpthread \";" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.platform = null;" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "GCArmv5T.platforms = [" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "  \"ti.platforms.evm3530\"" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "];" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "/* remove profiles we don't use */" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "delete GCArmv5T.profiles[\"coverage\"];" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "delete GCArmv5T.profiles[\"profile\"];" >> $(LPM_INSTALL_DIR)/config.bld
	@echo "Build.targets.\$$add(GCArmv5T);" >> $(LPM_INSTALL_DIR)/config.bld
	@echo -n "Building LPM GPP-side userspace libraries..."
	cd $(LPM_INSTALL_DIR); \
    XDCOPTIONS=-v XDCPATH=$(LPM_INSTALL_DIR) ${XDC_INSTALL_DIR}/xdc -PR .
	@echo "complete!"
endif


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean dsplink_libs_clean lpm_libs_clean
clean: libs_clean

dsplink_libs_clean:
	@echo -n "Peforming clean on DSPLink GPP-side user-space libraries..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/gpp/src/api \
    DSPLINK=$(DSPLINK) \
    BASE_TOOLCHAIN=$(ARM_TOOLCHAIN_PATH) \
    BASE_BUILDOS=$(LINUXKERNEL_INSTALL_DIR) \
    clean >> /dev/null
	@rm -Rf $(DSPLINKPATH)/RELEASE/*.txt $(DSPLINKPATH)/RELEASE/*.lib
	@rm -Rf $(DSPLINKPATH)/DEBUG/*.txt $(DSPLINKPATH)/DEBUG/*.lib
	@echo "complete!"

ifdef LPM_DSP_POWER_SOC
lpm_libs_clean:
	@echo -n "Peforming clean on LPM GPP-side user-space libraries..."
	cd $(LPM_INSTALL_DIR); \
    XDCOPTIONS=-v XDCPATH=$(LPM_INSTALL_DIR) ${XDC_INSTALL_DIR}/xdc clean -PR .
	@echo "complete!"
endif

libs_clean::
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
  
# Force a reconfigure next time we build
distclean: clean
	@rm -Rf $(DSPLINK)/config/BUILD/CURRENTCFG.MK
  
vpath %.c $(PLATFORMDIR)
