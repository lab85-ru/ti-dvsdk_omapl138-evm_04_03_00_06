#############################################################################
# Makefile                                                                  #
#                                                                           #
# Builds the DSP-side Posix component lib for the specified platform        #
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
include ../../../../../config.mak
include ../../../../../Rules.mak


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
CINCLUDES := -I$(CODEGEN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_INSTALL_DIR)/include/c6run/dsp \
      -I$(BIOS_INSTALL_DIR)/packages/ti/bios/include \
      -I$(BIOS_INSTALL_DIR)/packages/ti/rtdx/include/c6000 \
      -I$(BIOS_INSTALL_DIR)/packages/ti/psl/include \
      -I$(C6RUN_COMMON_DIR) \
      -I$(C6RUN_COMMON_DIR)/ipc \
      -I$(C6RUN_DSPLIBS_DIR) \
      -I$(C6RUN_DSPLIBS_DIR)/ipc \
      -I$(C6RUN_DSPLIBS_DIR)/os \
      -I$(C6RUN_DSPLIBS_DIR)/os/dspbios5 \
      -I$(C6RUN_DSPLIBS_DIR)/sharedmem \
      -I$(PLATFORMDIR) \
      -I.

# Archiver flags
ARFLAGS := -r
     
CSRCS = unistd/sleep.c unistd/getopt_long.c


#   ----------------------------------------------------------------------------
#   Debug and Release target setup
#   ----------------------------------------------------------------------------
OBJDIR_D      := $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)/posix/debug
OBJDIR_R      := $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)/posix/release

CFLAGS_D      := $(CFLAGS) -g -d"_DEBUG" $(DEBUG)

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
#   Output binary names
#   ----------------------------------------------------------------------------
BINDIR := $(C6RUN_DSPLIBS_DIR)/os/$(DSPOS)/posix/lib
LIBD := $(BINDIR)/posixd.lib
LIBR := $(BINDIR)/posix.lib


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

debug: $(LIBD)

$(LIBD) : $(OBJS_D) $(BINDIR)/.created
# Create a library archive using local objects
	@echo Compiling $@ Library...
	@$(AR) $(ARFLAGS) $@ $(OBJS_D)

release: $(LIBR)
  
$(LIBR) : $(OBJS_R) $(BINDIR)/.created
# Create a library archive using local objects and temp extracted objects
	@echo Compiling $@ Library...
	$(AR) $(ARFLAGS) $@ $(OBJS_R)


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean
clean:
	@echo -n "Removing binary library files..."
	@rm -Rf $(LIBR) $(LIBD)
	@echo "complete!"
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
  
vpath %.c unistd/