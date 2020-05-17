#############################################################################
# Makefile                                                                  #
#                                                                           #
# Builds the GPP (ARM)-side C6Run lib for the configured platform           #
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
include ../../config.mak
include ../../Rules.mak
 
 
#   ----------------------------------------------------------------------------
#   Project name.
#   ----------------------------------------------------------------------------
PROJNAME := c6run


#   ----------------------------------------------------------------------------
#   Common path for files shared among GPP and DSP sides
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_GPPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/gpp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)

# Select defaults if none provided
GPPOS ?= linux
IPC ?= dsplink
SHAREDMEM ?= cmem

IPCDIR := $(C6RUN_GPPLIBS_DIR)/ipc/$(IPC)
OSDIR  := $(C6RUN_GPPLIBS_DIR)/os/$(GPPOS)
SMDIR  := $(C6RUN_GPPLIBS_DIR)/sharedmem/$(SHAREDMEM)


#   ----------------------------------------------------------------------------
#   Global variables for building
#   ----------------------------------------------------------------------------

# Use environment variable for CFLAGS, if it exists
CFLAGS ?= $(ARM_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -std=gnu99 -Wall -Wno-trigraphs -fPIC -fno-strict-aliasing -fno-common -fno-omit-frame-pointer

CINCLUDES := -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(PLATFORMDIR) \
      -I$(C6RUN_GPPLIBS_DIR) \
      -I$(C6RUN_GPPLIBS_DIR)/ipc \
      -I$(C6RUN_GPPLIBS_DIR)/os \
      -I$(C6RUN_GPPLIBS_DIR)/sharedmem

# Define Archiver flags
ARFLAGS = rcs

# Define source files
CSRCS := C6Run.c C6Run_cio.c C6Run_rpc.c C6Run_control.c
MALLOCSRC := malloc.c
CPPSRCS :=
ASRCS :=

OBJDIR_D      := debug
IPC_OBJDIR_D  := $(C6RUN_GPPLIBS_DIR)/ipc/$(OBJDIR_D)
OS_OBJDIR_D   := $(C6RUN_GPPLIBS_DIR)/os/$(OBJDIR_D)
SM_OBJDIR_D   := $(C6RUN_GPPLIBS_DIR)/sharedmem/$(OBJDIR_D)

OBJDIR_R      := release
IPC_OBJDIR_R  := $(C6RUN_GPPLIBS_DIR)/ipc/$(OBJDIR_R)
OS_OBJDIR_R   := $(C6RUN_GPPLIBS_DIR)/os/$(OBJDIR_R)
SM_OBJDIR_R   := $(C6RUN_GPPLIBS_DIR)/sharedmem/$(OBJDIR_R)


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

# Output library names
BINDIR  := $(C6RUN_INSTALL_DIR)/lib/c6run_gpp
MAINR   := $(BINDIR)/lib$(PROJNAME)_main.a
MAIND   := $(BINDIR)/lib$(PROJNAME)_maind.a
LIBR    := $(BINDIR)/lib$(PROJNAME).a
LIBD    := $(BINDIR)/lib$(PROJNAME)d.a
MALLOCR := $(BINDIR)/lib$(PROJNAME)_malloc.a
MALLOCD := $(BINDIR)/lib$(PROJNAME)_mallocd.a

#   ----------------------------------------------------------------------------
#   Name of the ARM compiler/linker/archiver
#   ----------------------------------------------------------------------------
CC := $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc
AR := $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)ar


#   ----------------------------------------------------------------------------
#   Compiler/Linker/Archiver procedures
#   ----------------------------------------------------------------------------
.PHONY: all debug release
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



#   ----------------------------------------------------------------------------
#   Building Debug... 
#   ----------------------------------------------------------------------------
.PHONY: $(IPC_OBJDIR_D) $(OS_OBJDIR_D) $(SM_OBJDIR_D)
debug: $(LIBD) $(MAIND) $(MALLOCD)

$(LIBD) : $(OBJS_D) $(IPC_OBJDIR_D) $(OS_OBJDIR_D) $(SM_OBJDIR_D) $(BINDIR)/.created Makefile
  # Create a library archive using local objects and temp extracted objects
	@echo -n "Creating $@ Library..."
	@$(AR) $(ARFLAGS) $@ $(OBJS_D) $(IPC_OBJDIR_D)/*.* $(OS_OBJDIR_D)/*.* $(SM_OBJDIR_D)/*.*
	@echo "complete!"
  
$(MALLOCD) : $(OBJDIR_D)/malloc.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<

$(MAIND) : $(OBJDIR_D)/main.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<

$(IPC_OBJDIR_D):
	@$(MAKE) -C $(IPCDIR) debug

$(OS_OBJDIR_D):
	@$(MAKE) -C $(OSDIR) debug

$(SM_OBJDIR_D):
	@$(MAKE) -C $(SMDIR) debug



#   ----------------------------------------------------------------------------
#   Building Release... 
#   ----------------------------------------------------------------------------
.PHONY: $(IPC_OBJDIR_R) $(OS_OBJDIR_R) $(SM_OBJDIR_R)
release: $(LIBR) $(MAINR) $(MALLOCR) 
  
$(LIBR) : $(OBJS_R) $(IPC_OBJDIR_R) $(OS_OBJDIR_R) $(SM_OBJDIR_R) $(BINDIR)/.created Makefile
  # Create a library archive using local objects and temp extracted objects
	@echo -n "Creating $@ Library..."
	@$(AR) $(ARFLAGS) $@ $(OBJS_R) $(IPC_OBJDIR_R)/*.* $(OS_OBJDIR_R)/*.* $(SM_OBJDIR_R)/*.*
	@echo "complete!"  

$(MALLOCR) : $(OBJDIR_R)/malloc.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<

$(MAINR) : $(OBJDIR_R)/main.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<
  
$(IPC_OBJDIR_R):
	@$(MAKE) -C $(IPCDIR) release

$(OS_OBJDIR_R):
	@$(MAKE) -C $(OSDIR) release

$(SM_OBJDIR_R):
	@$(MAKE) -C $(SMDIR) release


#   ----------------------------------------------------------------------------
#   Clean Rule
#   ----------------------------------------------------------------------------
.PHONY: clean
clean:
  # Remove all local temp object files
	@rm -rf $(OBJDIR_D) $(IPC_OBJDIR_D) $(OS_OBJDIR_D) $(SM_OBJDIR_D)
	@rm -rf $(OBJDIR_R) $(IPC_OBJDIR_R) $(OS_OBJDIR_R) $(SM_OBJDIR_R)
  # Clean the subdirectory object files
	@$(MAKE) -C $(IPCDIR) clean
	@$(MAKE) -C $(OSDIR) clean
	@$(MAKE) -C $(SMDIR) clean
  # Remove GPP-side C6RunLib libs
	@rm -Rf $(LIBR) $(LIBD)
  # Remove GPP-side main object files
	@rm -Rf $(MAINR) $(MAIND)
  # Remove GPP-side malloc object files
	@rm -Rf $(MALLOCR) $(MALLOCD)
