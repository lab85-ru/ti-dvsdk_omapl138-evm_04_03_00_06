#   ============================================================================
#   @file   gendefs.mk
#
#   @path   $(DSPLINK)/make/
#
#   @desc   This makefile defines generic macros used by MAKE system.
#
#   @ver    1.65.01.05_eng
#   ============================================================================
#   Copyright (C) 2002-2009, Texas Instruments Incorporated -
#   http://www.ti.com/
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#   
#   *  Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#   
#   *  Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#   
#   *  Neither the name of Texas Instruments Incorporated nor the names of
#      its contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#   
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#   PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#   OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#   ============================================================================


ifndef GENDEFS_MK

define GENDEFS_MK
endef


include $(MAKEROOT)$(DIRSEP)processor.mk

.SUFFIXES :
.SUFFIXES : .EXE .DLL .LIB .OBJ. ASM .C .CPP


#   ============================================================================
#   Define common directory names
#   ============================================================================

DIR_INCLUDE     := INCLUDE
DIR_INCLUDE_USER := USER
DIR_INCLUDE_KRNL := KRNL
DIR_EXPORT      := EXPORT


DIR_OBJECTS     := OBJ
DIR_TARGETS     := BIN
DIR_BIOS        := BIOS_CONFIG

DIR_DEBUG       := DEBUG
DIR_RELEASE     := RELEASE


#   ============================================================================
#   Define common file names
#   ============================================================================

COMP_DETAILS    := COMPONENT
COMP_SOURCES    := SOURCES
COMP_OBJECTS    := OBJECTS
COMP_DEPENDS    := DEPENDS


#   ============================================================================
#   Directory hierarchy for the MAKE system
#   ============================================================================

PROJ_MAKE           := $(DSPLINK)$(DIRSEP)make
PROJ_MAK_GENERIC    := $(PROJ_MAKE)
PROJ_MAK_BUILDOS    := $(PROJ_MAKE)$(DIRSEP)$(BUILDOS)


#   ============================================================================
#   Directory hierarchy for global header files
#
#   The changes in directory hierarchy (if any) must be reflected in the rules
#   for making the same.
#   ============================================================================

PROJ_INCLUDE        := $(DEVICEROOT)$(DIRSEP)inc

PROJ_INC_GENERIC    := $(PROJ_INCLUDE)
ifeq ("$(DEVICETYPE)", "GPP")
PROJ_INC_USR        := $(PROJ_INCLUDE)$(DIRSEP)usr
PROJ_INC_SYS        := $(PROJ_INCLUDE)$(DIRSEP)sys
PROJ_INC_ARCH       := $(PROJ_INC_SYS)$(DIRSEP)arch
PROJ_INC_BUILDOS    := $(PROJ_INCLUDE)$(DIRSEP)sys$(DIRSEP)$(BUILDOS)
PROJ_INC_BUILDOSVER := $(PROJ_INCLUDE)$(DIRSEP)sys$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)

# To build kernel side library
PROJ_KRNL_INCLUDE        := $(DEVICEROOT)$(DIRSEP)src$(DIRSEP)inc
PROJ_KRNL_INC_GENERIC    := $(PROJ_KRNL_INCLUDE)
PROJ_KRNL_INC_USR        := $(PROJ_KRNL_INCLUDE)$(DIRSEP)usr
PROJ_KRNL_INC_SYS        := $(PROJ_KRNL_INCLUDE)$(DIRSEP)sys
PROJ_KRNL_INC_ARCH       := $(PROJ_KRNL_INC_SYS)$(DIRSEP)arch
PROJ_KRNL_INC_BUILDOS    := $(PROJ_KRNL_INCLUDE)$(DIRSEP)sys$(DIRSEP)$(BUILDOS)
PROJ_KRNL_INC_BUILDOSVER := $(PROJ_KRNL_INCLUDE)$(DIRSEP)sys$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)
endif
ifeq ("$(DEVICETYPE)", "DSP")
PROJ_INC_BUILDOS         := $(PROJ_INCLUDE)$(DIRSEP)$(BUILDOS)
PROJ_INC_BUILDOSVER      := $(PROJ_INCLUDE)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
PROJ_INC_DEVICEOS        := $(PROJ_INCLUDE)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)
else
PROJ_INC_DEVICEOS        := $(PROJ_INCLUDE)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)
endif
PROJ_INC_DSPDEVICE       := $(PROJ_INCLUDE)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)
PROJ_INC_DSPDEVICECLASS  := $(PROJ_INCLUDE)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)
endif

#   ============================================================================
#   Directory hierarchy for component header files
#
#   The changes in directory hierarchy (if any) must be reflected in the rules
#   for making the same.
#
#   These definitions are based on COMP_PATH defined in file $(COMP_DETAILS).
#   Note that the actual values will be calculated on actual usage
#   ============================================================================
COMP_INC_GENERIC    = $(COMP_PATH)
COMP_INC_BUILDOS    = $(COMP_PATH)$(DIRSEP)$(BUILDOS)
COMP_INC_BUILDOSVER = $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)


#   ============================================================================
#   Directory hierarchy for exported headers & targets
#
#   The changes in directory heirarchy (if any) must be reflected in the rules
#   for making the directory structure.
#   ============================================================================

EXPORT_INC          := $(EXPORT_DIR)$(DIRSEP)$(DIR_INCLUDE)
EXPORT_INC_GENERIC  := $(EXPORT_INC)
EXPORT_INC_BUILDOS  := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)
EXPORT_INC_PLATFORM := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)
EXPORT_INC_PLATFORM_USR      := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)usr
EXPORT_INC_PLATFORM_SYS      := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)sys
EXPORT_INC_PLATFORM_SYS_ARCH := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)sys$(DIRSEP)arch
EXPORT_INC_PLATFORM_INTERNAL := $(EXPORT_INC)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)internal

EXPORT_BIN          := $(EXPORT_DIR)$(DIRSEP)$(DIR_TARGETS)
EXPORT_BIN_GENERIC  := $(EXPORT_BIN)
ifeq ("$(DEVICETYPE)", "GPP")
	ifneq ("$(TI_DSPLINK_GPPOSPREFIX)", "")
		EXPORT_BIN_BUILDOS  := $(EXPORT_BIN)$(DIRSEP)$(TI_DSPLINK_GPPOSPREFIX)$(BUILDOS)
		EXPORT_BIN_PLATFORM := $(EXPORT_BIN)$(DIRSEP)$(TI_DSPLINK_GPPOSPREFIX)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)
	else
		EXPORT_BIN_BUILDOS  := $(EXPORT_BIN)$(DIRSEP)$(BUILDOS)
		EXPORT_BIN_PLATFORM := $(EXPORT_BIN)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)
	endif
endif
ifeq ("$(DEVICETYPE)", "DSP")
	EXPORT_BIN_BUILDOS  := $(EXPORT_BIN)$(DIRSEP)$(BUILDOS)
	EXPORT_BIN_PLATFORM := $(EXPORT_BIN)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)
endif
ifeq ("$(DEVICETYPE)", "GPP")
	EXPORT_BIN_DEB      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(DIR_DEBUG)
	EXPORT_BIN_REL      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(DIR_RELEASE)
endif
ifeq ("$(DEVICETYPE)", "DSP")
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
	EXPORT_BIN_DEB      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_DEBUG)
	EXPORT_BIN_REL      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_RELEASE)
else
	EXPORT_BIN_DEB      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_DEBUG)
	EXPORT_BIN_REL      := $(EXPORT_BIN_PLATFORM)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_RELEASE)
endif
endif


#   ============================================================================
#   Directory hierarchy for sources in a component
#   ============================================================================

SOURCE_GENERIC        := $(COMP_PATH)
SOURCE_BUILDOS        := $(COMP_PATH)$(DIRSEP)$(BUILDOS)
SOURCE_BUILDOS_VER    := $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)
ifeq ("$(DEVICETYPE)", "DSP")
SOURCE_OS                 := $(COMP_PATH)$(DIRSEP)$(BUILDOS)
SOURCE_OS_DSPDEVICE       := $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
SOURCE_OS_VER_DSPDEVICE   := $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)
else
SOURCE_OS_VER_DSPDEVICE   := $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)
endif
endif

#   ============================================================================
#   Directory hierarchy for generated objects and binaries
#
#   The changes in directory heirarchy (if any) must be reflected in the rules
#   for making the directory structure.
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
BUILD_INCLUDE       := $(BUILD_DIR)$(DIRSEP)$(DIR_INCLUDE)
BUILD_USER_INCLUDE  := $(BUILD_DIR)$(DIRSEP)$(DIR_INCLUDE)$(DIRSEP)$(DIR_INCLUDE_USER)
BUILD_KRNL_INCLUDE  := $(BUILD_DIR)$(DIRSEP)$(DIR_INCLUDE)$(DIRSEP)$(DIR_INCLUDE_KRNL)
BUILD_EXPORT        := $(BUILD_DIR)$(DIRSEP)$(DIR_EXPORT)
BUILD_CONFIG        := $(DSPLINK)$(DIRSEP)config$(DIRSEP)BUILD
endif

ifeq ("$(DEVICETYPE)", "DSP")
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
BUILD_INCLUDE       := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_INCLUDE)
BUILD_EXPORT        := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_EXPORT)
else
BUILD_INCLUDE       := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_INCLUDE)
BUILD_EXPORT        := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(DIR_EXPORT)
endif
endif

BUILD_EXPORT_DEB    := $(BUILD_EXPORT)$(DIRSEP)$(DIR_DEBUG)
BUILD_EXPORT_REL    := $(BUILD_EXPORT)$(DIRSEP)$(DIR_RELEASE)

ifeq ("$(DEVICETYPE)", "GPP")
COMP_BUILD          := $(BUILD_DIR)$(DIRSEP)$(COMP_NAME)
endif

ifeq ("$(DEVICETYPE)", "DSP")
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
COMP_BUILD          := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(COMP_NAME)
else
COMP_BUILD          := $(BUILD_DIR)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)_$(TI_DSPLINK_PROCID)$(DIRSEP)$(COMP_NAME)
endif
endif

COMP_OBJECT         := $(COMP_BUILD)$(DIRSEP)$(DIR_OBJECTS)
COMP_BINARY         := $(COMP_BUILD)$(DIRSEP)$(DIR_TARGETS)
ifeq ("$(DEVICETYPE)", "DSP")
COMP_BIOS           := $(COMP_BUILD)$(DIRSEP)$(DIR_BIOS)
endif

COMP_OBJECT_DEB     := $(COMP_OBJECT)$(DIRSEP)$(DIR_DEBUG)
COMP_OBJECT_REL     := $(COMP_OBJECT)$(DIRSEP)$(DIR_RELEASE)

COMP_BINARY_DEB     := $(COMP_BINARY)$(DIRSEP)$(DIR_DEBUG)
COMP_BINARY_REL     := $(COMP_BINARY)$(DIRSEP)$(DIR_RELEASE)


#   ============================================================================
#   Level of print(s) for the banner (Used in banner.pl)
#   ============================================================================

BANNER_L1   := 1
BANNER_L2   := 2


#   ============================================================================
#   Specify the configuration default build configuration file.
#   ============================================================================

DEFAULT_BUILD_CONFIG  := $(MAKEROOT)$(DIRSEP)config$(DIRSEP)LINK_FULL


#   ============================================================================
#   Specify the source files to copy for static/dynamic configuration.
#   ============================================================================

DST_STATIC_API_C   := $(TARGETROOT)$(DIRSEP)src$(DIRSEP)api
TARGETDIR          :=
ifeq ("$(DEVICETYPE)", "GPP")
	TARGETDIR      := gpp
endif
ifeq ("$(DEVICETYPE)", "DSP")
	TARGETDIR      := dsp
endif


#   ============================================================================
#   Configuration specific defines to be supplied to the compiler.
#   ============================================================================
CFG_CC_DEFNS :=


endif   # ifndef GENDEFS_MK
