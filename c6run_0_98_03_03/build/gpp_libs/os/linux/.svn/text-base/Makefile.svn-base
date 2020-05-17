#############################################################################
# Makefile                                                                  #
#                                                                           #
# Targets to check, configure and build GPP-side OSAL Linux components      #
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
#   Common path for files shared among GPP and DSP sides
#   ----------------------------------------------------------------------------
C6RUN_COMMON_DIR := $(C6RUN_INSTALL_DIR)/build/common
C6RUN_GPPLIBS_DIR := $(C6RUN_INSTALL_DIR)/build/gpp_libs
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)


#   ----------------------------------------------------------------------------
#   Global variables for building
#   ----------------------------------------------------------------------------
CFLAGS ?= $(ARM_CFLAGS)
CFLAGS += $(PLATFORM_CFLAGS)
CFLAGS += -std=gnu99 \
          -Wall -Wno-trigraphs \
          -fPIC -fno-strict-aliasing -fno-common -fno-omit-frame-pointer

CINCLUDES := -I$(C6RUN_INSTALL_DIR)/include \
      -I$(C6RUN_COMMON_DIR) \
      -I$(PLATFORMDIR) \
      -I$(C6RUN_GPPLIBS_DIR) \
      -I$(C6RUN_GPPLIBS_DIR)/os \
      -I. \


DEFS :=  

CSRCS = C6Run_osal.c


OBJDIR_D := $(C6RUN_GPPLIBS_DIR)/os/debug
OBJDIR_R := $(C6RUN_GPPLIBS_DIR)/os/release

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


debug: $(OBJDIR_D)/.created $(OBJS_D)

release: $(OBJDIR_R)/.created $(OBJS_R)


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean
clean: libs_clean

libs_clean:
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"

