#############################################################################
# Makefile                                                                  #
#                                                                           #
# Targets to check, configure and build DSP-side DSPLink components         #
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
#   Project name
#   ----------------------------------------------------------------------------
PROJNAME := ipc


#   ----------------------------------------------------------------------------
#   DSPLink directory definition
#   ----------------------------------------------------------------------------
DSPLINK := $(LINK_INSTALL_DIR)/dsplink
DSPLINKPATH := $(DSPLINK)/dsp/export/BIN/DspBios/$(DSPLINK_PLATFORM)/$(DSPLINK_DSP_PROCESSOR)_0


#   ----------------------------------------------------------------------------
#   Common paths
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_DSPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/dsp_libs
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
CFLAGS = $(DSP_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -q -pdv -pden -mo -eo=o


CINCLUDES := -I$(CODEGEN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include/c6run/dsp \
      -I$(DSPLINK)/dsp/inc \
      -I$(DSPLINK)/dsp/inc/DspBios \
      -I$(DSPLINK)/dsp/inc/DspBios/5.XX \
      -I$(DSPLINK)/dsp/inc/DspBios/5.XX/$(DSPLINK_DSP_PROCESSOR) \
      -I$(DSPLINK)/dsp/inc/C64XX \
      -I$(BIOS_INSTALL_DIR)/packages/ti/bios/include \
      -I$(BIOS_INSTALL_DIR)/packages/ti/rtdx/include/c6000 \
      -I$(BIOS_INSTALL_DIR)/packages/ti/psl/include \
      -I$(DSPLINK)/dsp/export/INCLUDE/DspBios/$(DSPLINK_PLATFORM) \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(C6RUN_DSPLIBS_DIR) \
      -I$(C6RUN_DSPLIBS_DIR)/ipc \
      -I$(C6RUN_DSPLIBS_DIR)/os \
      -I$(C6RUN_DSPLIBS_DIR)/sharedmem \
      -I$(PLATFORMDIR) \
      -I.

# Define Archiver flags
ARFLAGS := -r

DEFS := -dMAX_PROCESSORS=2 -dID_GPP=1

# Define source files
CSRCS = cio_ipc.c rpc_ipc.c control_ipc.c


#   ----------------------------------------------------------------------------
#   Define input library files
#   ----------------------------------------------------------------------------
LIBS := dsplink.lib       \
        dsplinkpool.lib   \
        dsplinkmsg.lib    \
        dsplinkmpcs.lib   \
        dsplinkmplist.lib \
        dsplinkringio.lib \
        dsplinknotify.lib \
        dsplinkdata.lib
INPUT_LIBS_R := $(LIBS:%.lib=$(DSPLINKPATH)/RELEASE/%.lib)
INPUT_LIBS_D := $(LIBS:%.lib=$(DSPLINKPATH)/DEBUG/%.lib)


#   ----------------------------------------------------------------------------
#   Include IPC/SharedMem/etc. specific make options (if they exist)
#   ----------------------------------------------------------------------------
include $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)/dsplink.mak


#   ----------------------------------------------------------------------------
#   Debug and Release target setup
#   ----------------------------------------------------------------------------
OBJDIR_D      := $(C6RUN_DSPLIBS_DIR)/ipc/debug
OBJDIR_R      := $(C6RUN_DSPLIBS_DIR)/ipc/release

CFLAGS_D      := $(CFLAGS) -g -d"_DEBUG" $(DEBUG)

COBJS_D       := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_D        := $(COBJS_D) $(CPPOBJS_D) $(ASMOBJS_D)

CFLAGS_R      := $(CFLAGS) -o3

COBJS_R       := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_R        := $(COBJS_R) $(CPPOBJS_R) $(ASMOBJS_R)


#   ----------------------------------------------------------------------------
#   Output binary names
#   ----------------------------------------------------------------------------
BINDIR := $(C6RUN_INSTALL_DIR)/lib/c6run_dsp
OUTPUT_LIB_R := $(BINDIR)/lib$(PROJNAME).a
OUTPUT_LIB_D := $(BINDIR)/lib$(PROJNAME)d.a


#   ----------------------------------------------------------------------------
#   Name of the TI compiler and DSP/BIOS builder (Tconf)
#   ----------------------------------------------------------------------------
CC := $(CODEGEN_INSTALL_DIR)/bin/cl6x
AR := $(CODEGEN_INSTALL_DIR)/bin/ar6x
TCONF := tconf


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

$(OBJDIR_D)/%.o: %.c $(OBJDIR_D)/.created
	$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

$(OBJDIR_D)/%.o: %.cpp $(OBJDIR_D)/.created
	$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

$(OBJDIR_D)/%.o: %.s62 $(OBJDIR_D)/.created
	$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

$(OBJDIR_D)/%.o: %.asm $(OBJDIR_D)/.created
	$(CC) -c $(DEFS) $(CFLAGS_D) $(CINCLUDES) -fr$(OBJDIR_D) $<

$(OBJDIR_R)/%.o: %.c $(OBJDIR_R)/.created
	$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<

$(OBJDIR_R)/%.o: %.cpp $(OBJDIR_R)/.created
	$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<

$(OBJDIR_R)/%.o: %.s62 $(OBJDIR_R)/.created
	$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<
  
$(OBJDIR_R)/%.o: %.asm $(OBJDIR_R)/.created
	$(CC) -c $(DEFS) $(CFLAGS_R) $(CINCLUDES) -fr$(OBJDIR_R) $<
  


#   ----------------------------------------------------------------------------
#   Building Debug... 
#   ----------------------------------------------------------------------------
.PRECIOUS: $(OBJDIR_D)/.created
debug: $(OUTPUT_LIB_D) $(OBJS_D)
  
$(OUTPUT_LIB_D): $(OBJDIR_D)/.created config $(INPUT_LIBS_D)
	@echo -n "Creating $@ Library..."
	@for i in $(INPUT_LIBS_D); do \
    libname=`basename $$i`; \
    mkdir $(OBJDIR_D)/$$libname; \
    cd $(OBJDIR_D)/$$libname; \
    $(AR) -xq $$i; \
    for j in `ls`; do \
      mv $$j $$libname.$$j; \
    done; \
    $(AR) -aq $@ *.*; \
    cd ..; \
    rm -Rf $(OBJDIR_D)/$$libname; \
  done;
	@echo "complete!"

  
#   ----------------------------------------------------------------------------
#   Building Release... 
#   ----------------------------------------------------------------------------
.PRECIOUS: $(OBJDIR_R)/.created
release: $(OUTPUT_LIB_R) $(OBJS_R)
  
$(OUTPUT_LIB_R): $(OBJDIR_R)/.created config $(INPUT_LIBS_R) 
	@echo -n "Creating $@ Library..."
	@for i in $(INPUT_LIBS_R); do \
    libname=`basename $$i`; \
    mkdir $(OBJDIR_R)/$$libname; \
    cd $(OBJDIR_R)/$$libname; \
    $(AR) -xq $$i; \
    for j in `ls`; do \
      mv $$j $$libname.$$j; \
    done; \
    $(AR) -aq $@ *.*; \
    cd ..; \
    rm -Rf $(OBJDIR_R)/$$libname; \
  done;
	@echo "complete!"


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
#   Build the DSPLink DSP libraries
#   ----------------------------------------------------------------------------
$(INPUT_LIBS_D):
	@echo -n "Building DSPLink DSP-side Debug libraries (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/dsp/src \
    DSPLINK=$(DSPLINK) \
    BASE_SABIOS=$(BIOS_INSTALL_DIR) \
    XDCTOOLS_DIR=$(XDC_INSTALL_DIR) \
    BASE_CGTOOLS=$(CODEGEN_INSTALL_DIR) \
    BASE_IPC=$(IPC_INSTALL_DIR) \
    debug >> /dev/null
	@echo "complete!"
	@echo "Library: $@"

$(INPUT_LIBS_R):
	@echo -n "Building DSPLink DSP-side Release library (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/dsp/src \
    DSPLINK=$(DSPLINK) \
    BASE_SABIOS=$(BIOS_INSTALL_DIR) \
    XDCTOOLS_DIR=$(XDC_INSTALL_DIR) \
    BASE_CGTOOLS=$(CODEGEN_INSTALL_DIR) \
    BASE_IPC=$(IPC_INSTALL_DIR) \
    release >> /dev/null
	@echo "complete!"
	@echo "Library: $@"


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean dsplink_libs_clean
clean: libs_clean

dsplink_libs_clean:
	@echo -n "Peforming clean on DSPLink DSP-side user-space libraries..."
	@$(XDC_INSTALL_DIR)/gmake -C $(DSPLINK)/dsp/src \
    DSPLINK=$(DSPLINK) \
    BASE_SABIOS=$(BIOS_INSTALL_DIR) \
    XDCTOOLS_DIR=$(XDC_INSTALL_DIR) \
    BASE_CGTOOLS=$(CODEGEN_INSTALL_DIR) \
    clean >> /dev/null
	@rm -Rf $(DSPLINKPATH)/RELEASE/*.txt $(DSPLINKPATH)/RELEASE/*.lib
	@rm -Rf $(DSPLINKPATH)/DEBUG/*.txt $(DSPLINKPATH)/DEBUG/*.lib
	@echo "complete!"

libs_clean:
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
	@echo -n "Removing output libraries..."
	@rm -Rf $(OUTPUT_LIB_R) $(OUTPUT_LIB_D)
	@echo "complete!"  


# Force a reconfigure next time we build
distclean: clean
	@rm -Rf $(DSPLINK)/config/BUILD/CURRENTCFG.MK
  
vpath %.c $(PLATFORMDIR)
