#############################################################################
# Makefile                                                                  #
#                                                                           #
# Builds the DSP-side libraries for the configured platform                 #
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
#   Common paths
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_DSPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/dsp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)

# Select defaults if none provided
IPC ?= dsplink
ifeq ($(IPC),dsplink)
DSPOS  = dspbios5
endif
DSPOS ?= sysbios6
SHAREDMEM ?= cmem

IPCDIR := $(C6RUN_DSPLIBS_DIR)/ipc/$(IPC)
OSDIR  := $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)
SMDIR  := $(C6RUN_DSPLIBS_DIR)/sharedmem/$(SHAREDMEM)


#   ----------------------------------------------------------------------------
#   Global variables for building
#   ----------------------------------------------------------------------------
CFLAGS = $(DSP_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -q -pdv -pden -mo -eo=o

CINCLUDES := -I$(CODEGEN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include/c6run/dsp \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(PLATFORMDIR) \
      -I$(C6RUN_DSPLIBS_DIR) \
      -I$(C6RUN_DSPLIBS_DIR)/ipc \
      -I$(C6RUN_DSPLIBS_DIR)/os \
      -I$(C6RUN_DSPLIBS_DIR)/sharedmem

# Define Archiver flags
ARFLAGS := -r

DEFS :=

# Define source files
CSRCS := C6Run_exit.c C6Run_cio.c C6Run_rpc.c trgmsg.c
CPPSRCS :=
ASRCS :=


#   ----------------------------------------------------------------------------
#   Define input library files
#   ----------------------------------------------------------------------------
OS_OBJDIR_D := $(C6RUN_DSPLIBS_DIR)/os/debug
OS_OBJDIR_R := $(C6RUN_DSPLIBS_DIR)/os/release

IPC_OBJDIR_D := $(C6RUN_DSPLIBS_DIR)/ipc/debug
IPC_OBJDIR_R := $(C6RUN_DSPLIBS_DIR)/ipc/release

SM_OBJDIR_D := $(C6RUN_DSPLIBS_DIR)/sharedmem/debug
SM_OBJDIR_R := $(C6RUN_DSPLIBS_DIR)/sharedmem/release

OS_LIB_D   := $(OS_OBJDIR_D)/os.lib
IPC_LIB_D  := $(IPC_OBJDIR_D)/ipc.lib
SM_LIB_D   := $(SM_OBJDIR_D)/sm.lib

OS_LIB_R   := $(OS_OBJDIR_R)/os.lib
IPC_LIB_R  := $(IPC_OBJDIR_R)/ipc.lib
SM_LIB_R   := $(SM_OBJDIR_R)/sm.lib

#INPUT_LIBS_D := $(IPC_LIB_D) $(OS_LIB_D) $(SM_LIB_D)
#INPUT_LIBS_R := $(IPC_LIB_R) $(OS_LIB_R) $(SM_LIB_R)
INPUT_LIBS_D := $(OS_OBJDIR_D) $(IPC_OBJDIR_D) $(SM_OBJDIR_D)
INPUT_LIBS_R := $(OS_OBJDIR_R) $(IPC_OBJDIR_R) $(SM_OBJDIR_R)



#   ----------------------------------------------------------------------------
#   Debug and Release target setup
#   ----------------------------------------------------------------------------
OBJDIR_D      := $(C6RUN_DSPLIBS_DIR)/debug
OBJDIR_R      := $(C6RUN_DSPLIBS_DIR)/release

CFLAGS_D := $(CFLAGS) -g -d"_DEBUG" $(DEBUG)

COBJS_D       := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_D     := $(addprefix $(OBJDIR_D)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_D        := $(COBJS_D) $(CPPOBJS_D) $(ASMOBJS_D)

CFLAGS_R := $(CFLAGS) -o3 

COBJS_R       := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(CSRCS)))))
CPPOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .opp, $(notdir $(basename $(CPPSRCS)))))
ASMOBJS_R     := $(addprefix $(OBJDIR_R)/, $(addsuffix .o, $(notdir $(basename $(ASRCS))))) 
OBJS_R        := $(COBJS_R) $(CPPOBJS_R) $(ASMOBJS_R)


#   ----------------------------------------------------------------------------
#   Output binary names
#   ----------------------------------------------------------------------------
BINDIR := $(C6RUN_INSTALL_DIR)/lib/c6run_dsp
MAINR := $(BINDIR)/lib$(PROJNAME)_main.a
MAIND := $(BINDIR)/lib$(PROJNAME)_maind.a
OUTPUT_LIB_R := $(BINDIR)/lib$(PROJNAME).a
OUTPUT_LIB_D := $(BINDIR)/lib$(PROJNAME)d.a
CMDR := $(BINDIR)/$(PROJNAME).cmd
CMDD := $(BINDIR)/$(PROJNAME)d.cmd


#   ----------------------------------------------------------------------------
#   Name of the TI compiler
#   ----------------------------------------------------------------------------
CC := $(CODEGEN_INSTALL_DIR)/bin/cl6x
AR := $(CODEGEN_INSTALL_DIR)/bin/ar6x


#   ----------------------------------------------------------------------------
#   Compiler/Linker/Archiver procedures
#   ----------------------------------------------------------------------------
.PHONY: all debug release
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
.PRECIOUS: $(BINDIR)/.created $(OBJDIR_D)/.created
debug: $(OUTPUT_LIB_D) $(MAIND) $(CMDD)

$(OUTPUT_LIB_D) : $(OBJS_D) $(INPUT_LIBS_D) $(BINDIR)/.created
  # Create a library archive using local objects and temp extracted objects
	@echo -n "Creating $@ Library..."
	@$(AR) -aq $@ $(OBJS_D) $(IPC_OBJDIR_D)/*.* $(OS_OBJDIR_D)/*.* $(SM_OBJDIR_D)/*.*
	@echo "complete!"

$(MAIND) : $(OBJDIR_D)/main.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<

$(OS_LIB_D) $(OS_OBJDIR_D) : $(BINDIR)/.created
	@$(MAKE) -C $(OSDIR) debug
  
$(IPC_LIB_D) $(IPC_OBJDIR_D) : $(OS_LIB_D) $(BINDIR)/.created
	@$(MAKE) -C $(IPCDIR) debug

$(SM_LIB_D) $(SM_OBJDIR_D): $(OS_LIB_D) $(BINDIR)/.created
	@$(MAKE) -C $(SMDIR) debug
  

#   ----------------------------------------------------------------------------
#   Building Release... 
#   ----------------------------------------------------------------------------
.PRECIOUS: $(OBJDIR_R)/.created
release: $(OUTPUT_LIB_R) $(MAINR) $(CMDR)
  
$(OUTPUT_LIB_R) : $(OBJS_R) $(INPUT_LIBS_R) $(BINDIR)/.created
  # Create a library archive using local objects and temp extracted objects
	@echo -n "Creating $@ Library..."
	@$(AR) -aq $@ $(OBJS_R) $(IPC_OBJDIR_R)/*.* $(OS_OBJDIR_R)/*.* $(SM_OBJDIR_R)/*.*
	@echo "complete!"

$(MAINR) : $(OBJDIR_R)/main.o $(BINDIR)/.created
	@$(AR) $(ARFLAGS) $@ $<

$(OS_LIB_R) $(OS_OBJDIR_R): $(BINDIR)/.created
	@$(MAKE) -C $(OSDIR) release
  
$(IPC_LIB_R) $(IPC_OBJDIR_R): $(OS_LIB_R) $(BINDIR)/.created
	@$(MAKE) -C $(IPCDIR) release

$(SM_LIB_R) $(SM_OBJDIR_R): $(OS_LIB_R) $(BINDIR)/.created
	@$(MAKE) -C $(SMDIR) release

  
#   ----------------------------------------------------------------------------
#   Creating final linker command file
#   ----------------------------------------------------------------------------
$(CMDR):
	@mv $(C6RUN_DSPLIBS_DIR)/os/linker.cmd $(CMDR)

$(CMDD):  
	@mv $(C6RUN_DSPLIBS_DIR)/os/linker.cmd $(CMDD)


#   ----------------------------------------------------------------------------
#   Clean Rule
#   ----------------------------------------------------------------------------
.PHONY: clean
clean: 
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
  # Remove c6run libs
	@echo -n "Removing output libraries..."  
	@rm -Rf $(OUTPUT_LIB_R) $(OUTPUT_LIB_D)
  # Remove DSP linker command file
	@rm -Rf $(CMDR) $(CMDD)
  # Remove main object files
	@rm -Rf $(MAINR) $(MAIND)  
	@echo "complete!"  
  # Clean component directories
	@$(MAKE) -C $(OSDIR) clean
	@$(MAKE) -C $(IPCDIR) clean
	@$(MAKE) -C $(SMDIR) clean

