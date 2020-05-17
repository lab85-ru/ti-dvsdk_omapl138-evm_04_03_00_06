# --COPYRIGHT--,BSD
#  Copyright (c) 2010, Texas Instruments Incorporated
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
# 
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# --/COPYRIGHT--

###############################################################################
# Makefile for audio SOC dsp-side example 
#
# This makefile is used to build the platform specific audio SOC application
# The platform is specified in the Rules.make
#
# To build the application:
#	make
#
###############################################################################

# Tell make where the various components (BIOS, RTDX etc) are installed
ROOT_DIR = ..
include ../Rules.make

###############################################################################

# The cl6x acts as the compiler, assembler, and linker for C6x architectures.  
# The "-z" option in LD674 specifies that all options following the "-z" will
# be linker options.
CC674 = $(CODEGEN_INSTALL_DIR)/bin/cl6x
AS674 = $(CODEGEN_INSTALL_DIR)/bin/cl6x
LD674 = $(CODEGEN_INSTALL_DIR)/bin/cl6x -z

# Base C674 compiler options
BASE_CC674OPTS = -g -mv6740 \
	-I$(BIOS_INSTALL_DIR)/packages/ti/bios/include \
	-I$(RTDX_ROOT)/include \
	-I$(CODEGEN_INSTALL_DIR)/include

# Base C674 assembler options
BASE_AS674OPTS = -mv6740 -I. -I$(BIOS_INSTALL_DIR)/packages/ti/bios/include 

# Base C674 linker options
BASE_LD674OPTS = -c -q \
	-I$(BIOS_INSTALL_DIR)/packages/ti/bios/lib \
	-I$(BIOS_INSTALL_DIR)/packages/ti/pmi/lib \
	-I$(BIOS_INSTALL_DIR)/packages/ti/pscl/lib \
	-I$(BIOS_INSTALL_DIR)/packages/ti/rtdx/lib/c6000 \
	-I$(CODEGEN_INSTALL_DIR)/lib

###############################################################################
# Check platform and set BOARD and DSPLINK variables

ifeq ($(PLATFORM), omapl138)

    # Define BOARD based on platform selection
    BOARD = evmOMAPL138

    # Set defines required by DSP/BIOS Link, leveraging file created by Link's build
    DSPLINK_DEFINES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/dsp/export/BIN/DspBios/OMAPL138/OMAPL138GEM_0/DEBUG/IPS_defines.txt)

    # Set include paths required by DSP/BIOS Link, leveraging file in Link's build
    DSPLINK_INCLUDES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/dsp/export/BIN/DspBios/OMAPL138/OMAPL138GEM_0/DEBUG/IPS_includes.txt)

endif

ifeq ($(PLATFORM), omapl137)

    # Define BOARD based on platform selection
    BOARD = evmOMAPL137

    # Set defines required by DSP/BIOS Link, leveraging file created by Link's build
    DSPLINK_DEFINES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/dsp/export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/DEBUG/IPS_defines.txt)

    # Set include paths required by DSP/BIOS Link, leveraging file in Link's build
    DSPLINK_INCLUDES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/dsp/export/BIN/DspBios/OMAPL1XX/OMAPL1XXGEM_0/DEBUG/IPS_includes.txt)

endif
###############################################################################

#   
#   Targets:
#
#   Define BOARD as the platform name used by TCONF
#   Define BOARD_DIR as the board directory for the platform being built
#   Define BIOS_ARCH as the architecture extension used by DSP/BIOS
#   Define ARCH as the architecture extension
#

build:
	@echo "----- Building DSP application for $(PLATFORM) -----"
	@mkdir -p $(BOARD)/Debug
	@$(MAKE) BOARD_DIR=$(BOARD) BIOS_ARCH=62 ARCH=674 $(BOARD)

clean:
	@echo "----- Cleaning DSP application for $(PLATFORM) -----"
	@$(MAKE) BOARD_DIR=$(BOARD) BIOS_ARCH=62 ARCH=674 cleanup 

#Export the following variable to the sub-makefiles
export PLATFORM BIOS_ARCH ARCH BOARD BOARD_DIR PROG CODEGEN_INSTALL_DIR LINK_INSTALL_DIR BIOSPSP_INSTALL_DIR EDMA3LLD_INSTALL_DIR XDC_INSTALL_DIR BIOS_INSTALL_DIR

#
#  Application name
#
PROG = audio

# Base project application directory
PROJECTS_DIR = .

#Application include search paths
APP_HDRS_DIR = -I. \
	-I$(PROJECTS_DIR)/$(BOARD_DIR) \
	-I$(BIOSPSP_INSTALL_DIR)/packages \
	-I$(EDMA3LLD_INSTALL_DIR)/packages \
	$(DSPLINK_INCLUDES)

# Application library search paths
APP_LIBS_DIR = -I$(PROJECTS_DIR)/$(BOARD_DIR) \
		-I$(BIOSPSP_INSTALL_DIR)/packages/ \
		-I$(EDMA3LLD_INSTALL_DIR)/packages/ \
		-I$(LINK_INSTALL_DIR)/dsplink

# Application linker command file and path
APP_CMD_FILE = $(PROJECTS_DIR)/$(BOARD_DIR)/audioDebug.cmd \
	$(PROJECTS_DIR)/$(BOARD_DIR)/$(PROG)cfg.cmd


#  Flags - Application platform specific compiler options here
CC$(ARCH)FLAGS = $(DSPLINK_DEFINES) \
	-fr$(PROJECTS_DIR)/$(BOARD_DIR)/Debug

AS$(ARCH)FLAGS = -fr$(PROJECTS_DIR)/$(BOARD_DIR)/Debug

LD$(ARCH)FLAGS = -I$(PROJECTS_DIR)/$(BOARD_DIR)/Debug  \
	-m$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)Soc_dsp.map \
	--xml_link_info=$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)Soc_dsp.xml

#List all the object files needed by the application 
OBJS = $(PROJECTS_DIR)/$(BOARD_DIR)/Debug/deviceDriverInit.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/msgqConfig.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/inputThread.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/outputThread.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/processThread.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/main.obj \
	$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)cfg.obj

#  Pattern rules for all sources to build Application
$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/%.obj : %.c
	$(CC$(ARCH)) $(APP_HDRS_DIR) $(BASE_CC$(ARCH)OPTS) $(CC$(ARCH)FLAGS) $<

$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/deviceDriverInit.obj : $(PROJECTS_DIR)/$(BOARD_DIR)/deviceDriverInit.c
	$(CC$(ARCH)) $(APP_HDRS_DIR) $(BASE_CC$(ARCH)OPTS) $(CC$(ARCH)FLAGS) $<

$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/main.obj : $(PROJECTS_DIR)/$(BOARD_DIR)/main.c
	$(CC$(ARCH)) $(APP_HDRS_DIR) $(BASE_CC$(ARCH)OPTS) $(CC$(ARCH)FLAGS) $<

$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)cfg.obj : $(PROJECTS_DIR)/$(BOARD_DIR)/$(PROG)cfg.s$(BIOS_ARCH)
	$(AS$(ARCH)) -i$(PROJECTS_DIR)/$(BOARD_DIR) $(BASE_AS$(ARCH)OPTS) $(AS$(ARCH)FLAGS) $<

# Spin off a make on board-specific dir to build config with tconf and
# put generated files in their projects/<board> dir
$(PROJECTS_DIR)/$(BOARD_DIR)/$(PROG)cfg.s$(BIOS_ARCH) : $(PROJECTS_DIR)/$(BOARD_DIR)/$(PROG).tcf
	$(MAKE) -C $(PROJECTS_DIR)/$(BOARD_DIR)

# Rule for the application
$(BOARD_DIR): $(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)Soc_dsp.out

# object files explicitly specified to set build dependency chain going
$(PROJECTS_DIR)/$(BOARD_DIR)/Debug/$(PROG)Soc_dsp.out: $(OBJS) 
	$(LD$(ARCH)) $(LD$(ARCH)FLAGS) $(BASE_LD$(ARCH)OPTS) $(APP_LIBS_DIR) -o $@ $^ $(APP_CMD_FILE)


# Clean up object files and generated configuration files
cleanup:
	@ echo "----- Removing DSP object files and binaries for $(BOARD) -----"
	@ rm -f $(PROJECTS_DIR)/$(BOARD_DIR)/Debug/*
	@$(MAKE) -C $(PROJECTS_DIR)/$(BOARD_DIR) clean

