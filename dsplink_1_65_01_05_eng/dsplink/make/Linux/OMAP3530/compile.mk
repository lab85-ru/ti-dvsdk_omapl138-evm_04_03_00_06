#   ============================================================================
#   @file   compile.mk
#
#   @path   $(DSPLINK)/make/Linux/OMAP3530/
#
#   @desc   This makefile contains rules to compile sources.
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


ifndef COMPILE_MK

define COMPILE_MK
endef


include $(MAKEROOT)/$(BUILDOS)/systools.mk
include $(MAKEROOT)/gendefs.mk
include $(MAKEROOT)/$(BUILDOS)/osdefs.mk
include $(MAKEROOT)/rulebase.mk

-include $(COMP_SOURCES)
-include ../$(COMP_DETAILS)
-include $(BUILDOS)/$(COMP_DETAILS)
-include $(BUILDOS)/$(TI_DSPLINK_GPPOSVERSION)/$(COMP_DETAILS)
-include $(COMP_DETAILS)
-include $(TI_DSPLINK_GPPOSVERSION)/$(COMP_DETAILS)


#   ============================================================================
#   Clear environment variables so the don't interfere in the build process.
#   ============================================================================

INCLUDE     :=
LIB         :=
C_OPTION    :=
C_DIR       :=
A_DIR       :=


#   ============================================================================
#   Standard include search path
#   ============================================================================

#   ----------------------------------------------------------------------------
#   Header files with global scope
#   ----------------------------------------------------------------------------
STD_INC_PATH    := $(CC_SW_INC)$(PROJ_INC_GENERIC)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_USR)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_BUILDOS)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_BUILDOSVER)

#   ----------------------------------------------------------------------------
#   Header files with component scope
#   ----------------------------------------------------------------------------
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_GENERIC)
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOS)
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOSVER)

#   ----------------------------------------------------------------------------
#   Header files from the target operating system
#   ----------------------------------------------------------------------------


ifeq ("$(COMP_MEMSPACE)", "USER")
	ifneq ("$(OSINC_TARGET)", "")
		STD_INC_PATH    += $(CC_SW_INC)$(OSINC_TARGET)
	endif
endif
STD_INC_PATH    += $(CC_SW_INC)$(BUILD_USER_INCLUDE)


ifeq ("$(COMP_MEMSPACE)", "USER")
	ifneq ("$(OSINC_TARGET)", "")
		STD_INC_PATH    += $(CC_SW_INC)$(OSINC_TARGET)
	endif
endif



STD_KRNL_INC_PATH    := $(CC_SW_INC)$(PROJ_KRNL_INC_GENERIC)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(PROJ_KRNL_INC_ARCH)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(PROJ_KRNL_INC_USR)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(PROJ_KRNL_INC_SYS)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(PROJ_KRNL_INC_BUILDOS)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(PROJ_KRNL_INC_BUILDOSVER)
STD_KRNL_INC_PATH    += $(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CC_SW_INC)$(PROJ_KRNL_INC_ARCH)$(DIRSEP)$(device))
ifeq ("$(BUILDOSVARIANT)", "")
STD_KRNL_INC_PATH    += $(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CC_SW_INC)$(PROJ_KRNL_INC_ARCH)$(DIRSEP)$(device)$(DIRSEP)$(BUILDOS))
else
STD_KRNL_INC_PATH    += $(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CC_SW_INC)$(PROJ_KRNL_INC_ARCH)$(DIRSEP)$(device)$(DIRSEP)$(BUILDOSVARIANT))
endif

#   ----------------------------------------------------------------------------
#   Header files with component scope
#   ----------------------------------------------------------------------------
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(COMP_INC_GENERIC)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOS)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOSVER)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(BUILD_INCLUDE)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(BUILD_USER_INCLUDE)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(BUILD_KRNL_INCLUDE)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(OSINC_GENERIC)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(OSINC_PLATFORM)

#   ============================================================================
#   Take the compiler flags based on the Component memory space.
#   ============================================================================
ifeq ("$(COMP_MEMSPACE)", "USER")
	STD_CC_FLAGS := $(STD_USER_FLAGS)
endif

ifeq ("$(COMP_MEMSPACE)", "KRNL")
ifeq ("$(TI_DSPLINK_GPPOSVERSION)", "2.6.10")
	STD_CC_FLAGS := $(STD_KRNL_FLAGS) -DKBUILD_MODNAME="dsplinkk"
endif
ifeq ("$(TI_DSPLINK_GPPOSVERSION)", "2.6.18")
	STD_CC_FLAGS := $(STD_KRNL_FLAGS) -D"KBUILD_STR(s)=\#s" -D"KBUILD_MODNAME=KBUILD_STR(dsplinkk)"
endif
endif

#   ============================================================================
#   Standard flags for the compiler
#   ============================================================================
ifeq ("$(COMP_TYPE)", "EXE")
STD_CC_FLAGS    += $(EXE_CC_FLAGS)
endif   # ifeq ("$(COMP_TYPE)", "EXE")

ifeq ("$(COMP_TYPE)", "DRV")
STD_CC_FLAGS    += $(DRV_CC_FLAGS)
endif   # ifeq ("$(COMP_TYPE)", "DRV")

ifeq ("$(COMP_TYPE)", "LIB")
STD_CC_FLAGS    += $(LIB_CC_FLAGS)
endif   # ifeq ("$(COMP_TYPE)", "LIB")


#   ============================================================================
#   Standard way of invoking the compiler
#   ============================================================================

COMPILER_DEB     = $(COMPILER) $(CC_SW_DEB) \
				   $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) -DDDSP_DEBUG \
				   -DKBUILD_BASENAME=$(basename $(basename $(notdir $@))) \
				   $(CC_SW_OBJ)$(COMP_OBJECT_DEB)/$(basename $(basename $(notdir $@))).o


COMPILER_REL     = $(COMPILER) \
				   $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				   -DKBUILD_BASENAME=$(basename $(basename $(notdir $@))) \
				   $(CC_SW_OBJ)$(COMP_OBJECT_REL)/$(basename $(basename $(notdir $@))).o

COMPILER_KRNL_DEB     = $(COMPILER) $(CC_SW_DEB) \
				   $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))\
				   $(STD_KRNL_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) -DDDSP_DEBUG \
				   -DKBUILD_BASENAME=$(basename $(basename $(notdir $@))) \
				   $(CC_SW_OBJ)$(COMP_OBJECT_DEB)/$(basename $(basename $(notdir $@))).o


COMPILER_KRNL_REL     = $(COMPILER) \
				   $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))\
				   $(STD_KRNL_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				   -DKBUILD_BASENAME=$(basename $(basename $(notdir $@))) \
				   $(CC_SW_OBJ)$(COMP_OBJECT_REL)/$(basename $(basename $(notdir $@))).o
#   ============================================================================
#   Get the C file containing configuration information.
#   ============================================================================

ifneq ("$(CONFIG)", "")
source_cfg := $(CONFIG)
else
source_cfg :=
endif

.PHONY: $(source_cfg)

$(source_cfg):
	$(CMDECHO)$(RUNIF) "exist $(BUILD_CONFIG)" ""  "$(CMDECHO)$(MKDIR) $(BUILD_CONFIG)"
	$(CMDECHO)$(DISPLAY) Fetching the configuration file $@ ...
	$(CMDECHO)$(COPY) $(CONFIG_DEVICES)$(DIRSEP)$@  $(BUILD_CONFIG)$(DIRSEP).

#   ============================================================================
#   Generate the build information used for compiling sources for this component
#   ============================================================================
bldinfodeb :
ifeq ("$(COMP_MEMSPACE)", "USER")
	$(CMDECHO) $(DISPLAY) $(STD_INC_PATH) $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL))     \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_includes.txt
endif
ifeq ("$(COMP_MEMSPACE)", "KRNL")
	$(CMDECHO) $(DISPLAY) $(STD_KRNL_INC_PATH)  $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL))  \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_includes.txt
endif
	$(CMDECHO) $(DISPLAY) $(CC_SW_DEB) $(STD_CC_FLAGS) $(USR_CC_FLAGS)  \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_flags.txt
	$(CMDECHO) $(DISPLAY) $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES)) $(STD_CC_DEFNS) $(USR_CC_DEFNS) -DDDSP_DEBUG  \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_defines.txt

bldinforel :

ifeq ("$(COMP_MEMSPACE)", "USER")
	$(CMDECHO) $(DISPLAY) $(STD_INC_PATH)  $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL))     \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_includes.txt
endif
ifeq ("$(COMP_MEMSPACE)", "KRNL")
	$(CMDECHO) $(DISPLAY) $(STD_KRNL_INC_PATH)  $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL))  \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_includes.txt
endif

	$(CMDECHO) $(DISPLAY) $(CC_SW_DEB) $(STD_CC_FLAGS) $(USR_CC_FLAGS)  \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_flags.txt
	$(CMDECHO) $(DISPLAY) $(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES)) $(STD_CC_DEFNS) $(USR_CC_DEFNS) -DDDSP_DEBUG  \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_defines.txt

#   ============================================================================
#   Rules to compile sources
#   ============================================================================

sources_deb := $(addsuffix .deb, $(SOURCES))
sources_rel := $(addsuffix .rel, $(SOURCES))

.PHONY: $(sources_deb) $(sources_rel)

#   ----------------------------------------------------------------------------
#   @rule   objdeb
#
#   @desc   Builds the object files for DEBUG variant.
#   ----------------------------------------------------------------------------

objdeb  : $(source_cfg) $(sources_deb)

$(sources_deb):
ifeq ("$(COMP_MEMSPACE)", "KRNL")
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_KRNL_DEB) $(basename $@)
else
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_DEB) $(basename $@)
endif

#   ----------------------------------------------------------------------------
#   @rule   objrel
#
#   @desc   Builds the object files for RELEASE variant.
#   ----------------------------------------------------------------------------

objrel  : $(source_cfg) $(sources_rel)

$(sources_rel):
ifeq ("$(COMP_MEMSPACE)", "KRNL")
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_KRNL_REL) $(basename $@)
else
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_REL) $(basename $@)
endif


endif   # ifndef COMPILE_MK
