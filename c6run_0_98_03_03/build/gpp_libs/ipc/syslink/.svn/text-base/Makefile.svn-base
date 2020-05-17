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
#   SysLink directory definition
#   ----------------------------------------------------------------------------
SYSLINK := $(SYSLINK_INSTALL_DIR)/packages/ti/syslink
#SYSLINK_PLATFORM := $(SYSLINK_PLATFORM)
ifndef SYSLINK_VARIANT
SYSLINK_VARIANT := $(SYSLINK_PLATFORM)
endif
ifndef SYSLINK_LOADER
SYSLINK_LOADER  := COFF
endif

#   ----------------------------------------------------------------------------
#   Common paths
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_GPPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/gpp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)


#   ----------------------------------------------------------------------------
#   Global variables for building
#   ----------------------------------------------------------------------------
CFLAGS ?= $(ARM_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -std=gnu99 -Wall -Wno-trigraphs -fPIC -fno-strict-aliasing -fno-common -fno-omit-frame-pointer

CFLAGS += -D_REENTRANT -DSYSLINK_PLATFORM_$(SYSLINK_PLATFORM) -DSYSLINK_BUILDOS_LINUX -DSYSLINK_BUILD_DEBUG -DSYSLINK_TRACE_ENABLE

CINCLUDES := -I$(C6RUN_INSTALL_DIR)/include \
      -I$(SYSLINK_INSTALL_DIR)/packages \
      -I$(SYSLINK)/inc \
      -I$(SYSLINK)/inc/usr \
      -I$(SYSLINK)/inc/usr/Linux \
      -I$(SYSLINK)/utils \
      -I$(IPC_INSTALL_DIR)/packages \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(C6RUN_GPPLIBS_DIR) \
      -I$(C6RUN_GPPLIBS_DIR)/ipc \
      -I$(C6RUN_GPPLIBS_DIR)/os \
      -I$(C6RUN_GPPLIBS_DIR)/sharedmem \
      -I$(PLATFORMDIR) \
      -I.

DEFS := -D_REENTRANT \
        -DSYSLINK_PLATFORM_$(SYSLINK_PLATFORM) \
        -DSYSLINK_BUILDOS_LINUX \
        -DSYSLINK_BUILD_DEBUG \
        -DSYSLINK_TRACE_ENABLE

CSRCS = C6Run_ipc.c cio_ipc.c rpc_ipc.c control_ipc.c

LIBS_R := $(SYSLINK)/lib/syslink.a_release
LIBS_D := $(SYSLINK)/lib/syslink.a_debug


OBJDIR_D      := $(C6RUN_GPPLIBS_DIR)/ipc/debug
OBJDIR_R      := $(C6RUN_GPPLIBS_DIR)/ipc/release

CFLAGS_D      := $(CFLAGS) -g -DDDSP_DEBUG -D__DEBUG -DDEBUG -DVERBOSE 

COBJS_D       := $(CSRCS:%.c=$(OBJDIR_D)/%.o)
CPPOBJS_D     := $(CPPSRCS:%.cpp=$(OBJDIR_D)/%.opp)
ASMOBJS_D     := $(ASRCS:%.s=$(OBJDIR_D)/%.o)
OBJS_D        := $(COBJS_D) $(CPPOBJS_D) $(ASMOBJS_D)

CFLAGS_R      := $(CFLAGS) -O3

COBJS_R       := $(CSRCS:%.c=$(OBJDIR_R)/%.o)
CPPOBJS_R     := $(CPPSRCS:%.cpp=$(OBJDIR_R)/%.opp)
ASMOBJS_R     := $(ASRCS:%.s=$(OBJDIR_R)/%.o)
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
	@cd  $(OBJDIR_R); \
	for i in $(LIBS_R); do \
		$(AR) x $$i; \
	done


#   ----------------------------------------------------------------------------
#   Build the SysLink gpp libraries
#   ----------------------------------------------------------------------------
$(SYSLINK)/lib/syslink.a_release:
	@echo -n "Building SysLink GPP-side userspace release library (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(SYSLINK)/utils/hlos/usr/Linux \
          SYSLINK_ROOT=$(SYSLINK_INSTALL_DIR)/packages \
          SYSLINK_PLATFORM=$(SYSLINK_PLATFORM) \
          SYSLINK_VARIANT=$(SYSLINK_VARIANT) \
          TOOLCHAIN_PREFIX=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
          SYSLINK_PKGPATH="$(IPC_INSTALL_DIR)/packages;$(SYSLINK_INSTALL_DIR)/packages" \
          release >> /dev/null
	@echo "complete!"

$(SYSLINK)/lib/syslink.a_debug:
	@echo -n "Building SysLink GPP-side userspace debug library (be patient)..."
	@$(XDC_INSTALL_DIR)/gmake -C $(SYSLINK)/utils/hlos/usr/Linux \
          SYSLINK_ROOT=$(SYSLINK_INSTALL_DIR)/packages \
          SYSLINK_PLATFORM=$(SYSLINK_PLATFORM) \
          SYSLINK_VARIANT=$(SYSLINK_VARIANT) \
          TOOLCHAIN_PREFIX=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
          SYSLINK_PKGPATH="$(IPC_INSTALL_DIR)/packages;$(SYSLINK_INSTALL_DIR)/packages" \
          debug >> /dev/null
	@echo "complete!"

#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean
clean: libs_clean

libs_clean::
	@echo -n "Peforming clean on SysLink DSP-side user-space libraries..."
	@$(XDC_INSTALL_DIR)/gmake -C $(SYSLINK)/utils/hlos/usr/Linux \
          SYSLINK_ROOT=$(SYSLINK_INSTALL_DIR)/packages \
          SYSLINK_PLATFORM=$(SYSLINK_PLATFORM) \
          clean
	@echo "complete!"

libs_clean::
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
  