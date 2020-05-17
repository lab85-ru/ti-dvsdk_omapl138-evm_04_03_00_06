#############################################################################
# Makefile                                                                  #
#                                                                           #
# Targets to check, configure and build GPP-side SysLink components         #
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
#   SysLink directory definition
#   ----------------------------------------------------------------------------
SYSLINK := $(SYSLINK_INSTALL_DIR)/packages/ti/syslink


#   ----------------------------------------------------------------------------
#   Common paths
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_DSPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/dsp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)


#   ----------------------------------------------------------------------------
#   Global variables for building
#   ----------------------------------------------------------------------------
CFLAGS = $(DSP_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -q -pdv -pden -mo -eo=o

CFLAGS += -D_REENTRANT -DSYSLINK_PLATFORM_$(SYSLINK_PLATFORM) -DSYSLINK_BUILDOS_LINUX -DSYSLINK_BUILD_DEBUG -DSYSLINK_TRACE_ENABLE

CINCLUDES := -I$(CODEGEN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include/c6run/dsp \
      -I$(SYSLINK_INSTALL_DIR)/packages \
      -I$(SYSLINK)/inc \
      -I$(SYSLINK)/inc/usr \
      -I$(SYSLINK)/inc/usr/Linux \
      -I$(SYSLINK)/utils \
      -I$(IPC_INSTALL_DIR)/packages \
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

DEFS := 

# Define source files
CSRCS = cio_ipc.c rpc_ipc.c control_ipc.c


#   ----------------------------------------------------------------------------
#   Define input library files
#   ----------------------------------------------------------------------------
INPUT_LIBS_R := 
INPUT_LIBS_D := 


#   ----------------------------------------------------------------------------
#   Include IPC/SharedMem/etc. specific make options (if they exist)
#   ----------------------------------------------------------------------------
include $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)/syslink.mak


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
#   Name of the TI compiler
#   ----------------------------------------------------------------------------
CC := $(CODEGEN_INSTALL_DIR)/bin/cl6x
AR := $(CODEGEN_INSTALL_DIR)/bin/ar6x


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
debug: $(OUTPUT_LIB_D)
  
$(OUTPUT_LIB_D): $(OBJDIR_D)/.created syslink $(INPUT_LIBS_D) $(OBJS_D)
	@echo -n "Creating $@ Library..."
	@$(AR) -aq $@ $(OBJS_D)
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
release: $(OUTPUT_LIB_R)
  
$(OUTPUT_LIB_R): $(OBJDIR_R)/.created syslink $(INPUT_LIBS_R) $(OBJS_R)
	@echo -n "Creating $@ Library..."
	@$(AR) -aq $@ $(OBJS_R)
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
#   Build the SysLink DSP libraries
#   ----------------------------------------------------------------------------
.PHONY: syslink
syslink:
	@echo -n "Building SysLink DSP-side libraries (be patient)..."
	@cd $(SYSLINK_INSTALL_DIR)/packages; \
    CODEGEN_INSTALL_DIR=$(CODEGEN_INSTALL_DIR) \
    SYSLINK_PLATFORM=$(SYSLINK_PLATFORM) \
    SYSLINK_VARIANT=$(SYSLINK_VARIANT) \
    $(XDC_INSTALL_DIR)/xdc XDCBUILDCFG="$(PLATFORMDIR)/platform.bld" \
      XDCPATH="$(IPC_INSTALL_DIR)/packages;$(SYSBIOS_INSTALL_DIR)/packages" \
      .libraries -Pr $$PWD;
	@echo "complete!"


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean syslink_libs_clean
clean: libs_clean

syslink_libs_clean:
	@echo -n "Peforming clean on SysLink DSP-side libraries..."
	@cd $(SYSLINK_INSTALL_DIR)/packages; \
    SYSLINK_PLATFORM=$(SYSLINK_PLATFORM) \
    $(XDC_INSTALL_DIR)/xdc XDCBUILDCFG="$(PLATFORMDIR)/platform.bld" \
      XDCPATH="$(IPC_INSTALL_DIR)/packages;$(SYSBIOS_INSTALL_DIR)/packages" \
      clean -PR $$PWD
	@echo "complete!"

libs_clean:
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
	@echo -n "Removing output libraries..."
	@rm -Rf $(OUTPUT_LIB_R) $(OUTPUT_LIB_D)
	@echo "complete!"  


  