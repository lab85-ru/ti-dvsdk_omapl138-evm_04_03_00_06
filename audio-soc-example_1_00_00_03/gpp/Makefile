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

#############################################################################
# Makefile for audio SOC Arm-side example
#
# This makefile is used to build the platform specific audio SOC application
# The platform is specified in the Rules.make
#
# To build the application:
#       make
#
#############################################################################

#
# Include the system configuration file
#
ROOT_DIR = ..
include ../Rules.make

# The prefix to be added before the GNU compiler tools if not previously set
CSTOOL_DIR_BIN  ?= $(CSTOOL_DIR)/bin
CSTOOL_PREFIX   ?= $(CSTOOL_DIR)/bin/arm-arago-linux-gnueabi-

# Complier, linker and archiver for ARM cross Complier
CC	 	:= $(CSTOOL_PREFIX)gcc
LD		:= $(CSTOOL_PREFIX)ld
AR	 	:= $(CSTOOL_PREFIX)ar

ifeq ($(PLATFORM), omapl138)
    # Set board for platform selected
    BOARD := evmOMAPL138

    # Use defines required by DSP/BIOS Link, leveraging file created by Link's build
    DSPLINK_DEFINES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL138/DEBUG/API_defines.txt)

    # Use include paths required by DSP/BIOS Link, leveraging file in Link's build
    DSPLINK_INCLUDES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL138/DEBUG/API_includes.txt)

    # Set path to DSP/BIOS Link's ARM libraries
    DSPLINK_LIB_PATH = $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL138
endif

ifeq ($(PLATFORM), omapl137)

    # Set board for platform selected
    BOARD := evmOMAPL137

    # Use defines required by DSP/BIOS Link, leveraging file created by Link's build
    DSPLINK_DEFINES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL1XX/DEBUG/API_defines.txt)

    # Use include paths required by DSP/BIOS Link, leveraging file in Link's build
    DSPLINK_INCLUDES = $(shell cat $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL1XX/DEBUG/API_includes.txt)

    # Path to DSP/BIOS Link's ARM libraries
    DSPLINK_LIB_PATH = $(LINK_INSTALL_DIR)/dsplink/gpp/export/BIN/Linux/OMAPL1XX
endif


# Base compiler defines, includes and build flags 
BASE_CC_DEFINES = $(DSPLINK_DEFINES)

BASE_CC_INCLUDES = $(DSPLINK_INCLUDES)

BASE_CC_FLAGS  = -mlittle-endian	\
		 -march=armv5t		\
		 -mtune=arm9tdmi 	\
		 -msoft-float 		\
		 -Uarm 			\
		 -Wdeclaration-after-statement	\
		 -marm			\
		 -Wall			\
		 -Wstrict-prototypes	\
		 -Wno-trigraphs		\
		 -fno-strict-aliasing	\
		 -fno-common		\
		 -fno-omit-frame-pointer\
		 -mapcs			\
		 -mabi=aapcs-linux	\
		 -O

# Debug specific build options
DEB_CC_FLAGS  = $(BASE_CC_FLAGS) $(BASE_CC_DEFINES) $(BASE_CC_INCLUDES) -g

# Release specific build options
REL_CC_FLAGS  = $(BASE_CC_FLAGS) $(BASE_CC_DEFINES) $(BASE_CC_INCLUDES)

# Linker specific options
LDFLAGS       = -lpthread


all: debug release

.PHONY: debug release

debug:
	@mkdir -p $(BOARD)/Debug

	@echo "----- Building GPP Debug application for $(BOARD) -----"
	$(CC) $(DEB_CC_FLAGS) -o $(BOARD)/Debug/audioSoc_gpp audio_main.c \
	    $(LDFLAGS) \
	    $(DSPLINK_LIB_PATH)/DEBUG/dsplink.lib
	@echo "----- The build was successful -----"

release:
	@mkdir -p $(BOARD)/Release

	@echo "----- Building GPP Release application for $(BOARD) -----"
	$(CC) $(REL_CC_FLAGS) -o $(BOARD)/Release/audioSoc_gpp audio_main.c \
	    $(LDFLAGS) \
	    $(DSPLINK_LIB_PATH)/RELEASE/dsplink.lib
	@echo "----- The build was successful -----"

clean:
	@echo "----- Removing GPP binaries for $(BOARD) -----"
	@rm -rf $(BOARD)/Debug $(BOARD)/Release
