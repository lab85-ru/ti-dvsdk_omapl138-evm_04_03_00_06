#############################################################################
# Makefile                                                                  #
#                                                                           #
# Targets to check, configure and build DSP-side OS abstraction components  #
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
PROJNAME := os


#   ----------------------------------------------------------------------------
#   DSPLink directory definition
#   ----------------------------------------------------------------------------
DSPLINK := $(LINK_INSTALL_DIR)/dsplink

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
      
      
# Define Linker command flags, file names, paths
LDFLAGS := -z -r --priority
LDINCLUDES := -i$(BIOS_INSTALL_DIR)/packages/ti/bios/lib \
              -i$(BIOS_INSTALL_DIR)/packages/ti/rtdx/lib/c6000 \
              -i$(BIOS_INSTALL_DIR)/packages/ti/psl/lib




# Define Archiver flags
ARFLAGS := -r

DEFS :=  

# Define source files
CSRCS = C6Run_boot.c C6Run_cache.c C6Run_control.c C6Run_log.c C6Run_main.c C6Run_util.c


BIOSTCF := C6Run.tcf
BIOSTCI := $(PLATFORMDIR)/platform.tci
BIOSSRC := $(BIOSTCF:%.tcf=%cfg.s62) $(BIOSTCF:%.tcf=%cfg_c.c)
BIOSCMD := $(BIOSTCF:%.tcf=%cfg.cmd)
TCONFFLAGS := -Dconfig.importPath="$(BIOS_INSTALL_DIR)/packages; \
  $(BIOS_INSTALL_DIR)/packages/ti/bios; \
  $(DSPLINK)/dsp/inc/DspBios; \
  $(DSPLINK)/dsp/inc/DspBios/5.XX/$(DSPLINK_DSP_PROCESSOR); \
  $(DSPLINK)/dsp/inc/DspBios/5.XX; \
  ./"

CSRCS := $(CSRCS) $(BIOSTCF:%.tcf=%cfg_c.c)
ASRCS := $(ASRCS) $(BIOSTCF:%.tcf=%cfg.s62)
  
BIOSCMD_FINAL := $(BIOSCMD:%.cmd=%_final.cmd)
BIOSCMD_PARTIAL := $(BIOSCMD:%.cmd=%_partial.cmd)
BIOSCMD_LIBLIST := $(BIOSCMD:%.cmd=%_liblist.txt)

#   ----------------------------------------------------------------------------
#   Define input library files
#   ----------------------------------------------------------------------------
# Define required POSIX support libraries and paths
POSIX_LIBS := posix.lib
POSIXPATH := $(C6RUN_INSTALL_DIR)/build/dsp_libs/os/dspbios5/posix
POSIX_LIBS_R := $(POSIX_LIBS:%.lib=$(POSIXPATH)/lib/%.lib)
POSIX_LIBS_D := $(POSIX_LIBS:%.lib=$(POSIXPATH)/lib/%d.lib)

INPUT_LIBS_R := $(POSIX_LIBS_R)
INPUT_LIBS_D := $(POSIX_LIBS_D)


#   ----------------------------------------------------------------------------
#   Include IPC/SharedMem/etc. specific make options (if they exist)
#   ----------------------------------------------------------------------------
-include $(C6RUN_DSPLIBS_DIR)/ipc/$(IPC)/dspbios5.mak
-include $(C6RUN_DSPLIBS_DIR)/sharedmem/$(SHAREDMEM)/dspbios5.mak


#   ----------------------------------------------------------------------------
#   Debug and Release target setup
#   ----------------------------------------------------------------------------
OBJDIR_D      := $(C6RUN_DSPLIBS_DIR)/os/debug
OBJDIR_R      := $(C6RUN_DSPLIBS_DIR)/os/release

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

CMD           := $(C6RUN_DSPLIBS_DIR)/os/linker.cmd


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
NM := $(CODEGEN_INSTALL_DIR)/bin/nm6x


#   ----------------------------------------------------------------------------
#   Compiler/Linker/Archiver procedures
#   ----------------------------------------------------------------------------
.PHONY:	all debug release

# The default build target.
all: bios debug release

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
  
$(OUTPUT_LIB_D): $(OBJDIR_D)/.created $(CMD) $(INPUT_LIBS_D)
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
	@for i in `cat $(BIOSCMD_LIBLIST)`; do \
    libname=`basename $$i`; \
    mkdir $(OBJDIR_D)/$$libname; \
    cd $(OBJDIR_D)/$$libname; \
    $(AR) -xq $$i; \
    if [ "$$?" -ne "0" ]; then \
      cp $$i ./; \
    else \
      for j in `ls`; do \
        mv $$j $$libname.$$j; \
      done; \
    fi; \
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

$(OUTPUT_LIB_R): $(OBJDIR_R)/.created $(CMD) $(INPUT_LIBS_R)
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
	@for i in `cat $(BIOSCMD_LIBLIST)`; do \
    libname=`basename $$i`; \
    mkdir $(OBJDIR_R)/$$libname; \
    cd $(OBJDIR_R)/$$libname; \
    $(AR) -x $$i; \
    if [ "$$?" -ne "0" ]; then \
      cp $$i ./; \
    else \
      for j in `ls`; do \
        mv $$j $$libname.$$j; \
      done; \
    fi; \
    $(AR) -aq $@ *.*; \
    cd ..; \
    rm -Rf $(OBJDIR_R)/$$libname; \
  done;
	@echo "complete!"

  
#   ----------------------------------------------------------------------------
#   Build Posix layer library
#   ----------------------------------------------------------------------------
$(POSIX_LIBS_R): $(OBJDIR_R)/.created $(BIOSOPT)
	@make -C $(POSIXPATH) release

$(POSIX_LIBS_D): $(OBJDIR_D)/.created $(BIOSOPT)
	@make -C $(POSIXPATH) debug


#   ----------------------------------------------------------------------------
#   Build BIOS auto-generated source files
#   ----------------------------------------------------------------------------
.PHONY: bios bios_clean

.PRECIOUS: $(BIOSCMD)

bios: $(CMD)

$(CMD) : $(BIOSCMD)
	@perl $(C6RUN_INSTALL_DIR)/build/perl/linker_cmd_prep.pl $(BIOSCMD)
	@perl $(C6RUN_INSTALL_DIR)/build/perl/lib_path_update.pl $(BIOSCMD_LIBLIST) $(BIOS_INSTALL_DIR)/packages/ti/bios/lib 
	@perl $(C6RUN_INSTALL_DIR)/build/perl/lib_path_update.pl $(BIOSCMD_LIBLIST) $(BIOS_INSTALL_DIR)/packages/ti/rtdx/lib/c6000 
	@perl $(C6RUN_INSTALL_DIR)/build/perl/lib_path_update.pl $(BIOSCMD_LIBLIST) $(BIOS_INSTALL_DIR)/packages/ti/psl/lib
	@mv $(BIOSCMD_FINAL) $(CMD)

$(BIOSCMD) $(BIOSSRC) : $(BIOSTCI) $(BIOSTCF)
	@echo TCONF: Generating DSP/BIOS files...
	@cp $(BIOSTCI) ./platform.tci
	@$(XDC_INSTALL_DIR)/tconf $(TCONFFLAGS) $(BIOSTCF) 0
	@rm -Rf ./platform.tci

bios_clean:
  # Remove all Bios5 temp files
	@rm -f *.cdb
	@rm -f *cfg.*
	@rm -f *cfg_c.*
	@rm -Rf $(CMD)


#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean libs_clean
clean: libs_clean bios_clean

libs_clean::
	@echo -n "Removing object file directories..."
	@rm -Rf $(OBJDIR_D) $(OBJDIR_R)
	@echo "complete!"
	@echo -n "Removing output libraries..."
	@rm -Rf $(OUTPUT_LIB_R) $(OUTPUT_LIB_D)
	@echo "complete!"  
	@$(MAKE) -C posix clean
   
vpath %.c $(PLATFORMDIR)
