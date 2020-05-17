#   ============================================================================
#   @file   compile.mk
#
#   @path   $(DSPLINK)/make/DspBios/C674X/
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


include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)systools.mk
include $(MAKEROOT)$(DIRSEP)gendefs.mk
include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)osdefs.mk
include $(MAKEROOT)$(DIRSEP)rulebase.mk

-include $(COMP_SOURCES)
-include ..$(DIRSEP)$(COMP_DETAILS)
-include $(BUILDOS)$(DIRSEP)$(COMP_DETAILS)
-include $(COMP_DETAILS)


#   ============================================================================
#   Clear environment variables so they don't interfere in the build process.
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
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_BUILDOS)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_BUILDOSVER)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_DEVICEOS)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_DSPDEVICE)
STD_INC_PATH    += $(CC_SW_INC)$(PROJ_INC_DSPDEVICECLASS)

#   ----------------------------------------------------------------------------
#   Header files with component scope
#   ----------------------------------------------------------------------------
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_GENERIC)
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOS)
STD_INC_PATH    += $(CC_SW_INC)$(COMP_INC_BUILDOS)$(DIRSEP)$(DSPDEVICE)

#   ----------------------------------------------------------------------------
#   Header files from the target operating system
#   ----------------------------------------------------------------------------
STD_INC_PATH    += $(addprefix $(CC_SW_INC), $(OSINC_GENERIC))
STD_INC_PATH    += $(addprefix $(CC_SW_INC), $(OSINC_PLATFORM))

STD_INC_PATH    += $(CC_SW_INC)$(BUILD_INCLUDE)


#   ============================================================================
#   Standard flags for TCONF
#   ============================================================================
STD_TCF_FLAGS   += -Dconfig.importPath="$(BASE_PLATFORMS);$(BASE_BUILDOS);$(PROJ_INC_BUILDOS);$(PROJ_INC_DEVICEOS);$(PROJ_INC_BUILDOSVER);$(COMP_PATH);" -Dconfig.programName="$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)$(DIRSEP)$(notdir $(COMP_PATH))"
TCF_ARGS        := $(TI_DSPLINK_PROCID)

#   ============================================================================
#   XDC Path for xs
#   ============================================================================
STD_XDC_PATH    += "$(BASE_PLATFORMS);$(MAKEROOT)$(DIRSEP)$(BUILDOS);$(COMP_PATH);$(PROJ_INC_BUILDOS);$(PROJ_INC_BUILDOSVER);$(PROJ_INC_DEVICEOS)"

#   ============================================================================
#   Debug XDC Flags for xs
#   ============================================================================
STD_XS_DEBUG_FLAGS_DM6446    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_debug_dm6446.bld" \
                               -rdebug -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Release XDC Flags for xs
#   ============================================================================
STD_XS_RELEASE_FLAGS_DM6446    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_release_dm6446.bld" \
                                 -rwhole_program -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Debug XDC Flags for xs
#   ============================================================================
STD_XS_DEBUG_FLAGS_DA8XX    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_debug_da8xx.bld" \
                               -rdebug -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Release XDC Flags for xs
#   ============================================================================
STD_XS_RELEASE_FLAGS_DA8XX    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_release_da8xx.bld" \
                                 -rwhole_program -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Debug XDC Flags for xs
#   ============================================================================
STD_XS_DEBUG_FLAGS_DA850    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_debug_da850.bld" \
                               -rdebug -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Release XDC Flags for xs
#   ============================================================================
STD_XS_RELEASE_FLAGS_DA850    = -b"$(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)config_release_da850.bld" \
                                 -rwhole_program -o "$(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"

#   ============================================================================
#   Standard flags for the compiler
#   ============================================================================

ifeq ("$(COMP_TYPE)", "EXE")
STD_CC_FLAGS    += $(EXE_CC_FLAGS)
endif   # ifeq ("$(COMP_TYPE)", "EXE")

ifeq ("$(COMP_TYPE)", "ARC")
STD_CC_FLAGS    += $(ARC_CC_FLAGS)
endif   # ifeq ("$(COMP_TYPE)", "ARC")

#   ============================================================================
#   Standard way of invoking the compiler
#   ============================================================================

COMPILER_CFG_DEB  = $(COMPILER) $(CC_SW_DEB) \
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES))\
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				  -@"$(COMPILER_OPT)" \
				   $(CC_SW_OBJ)"$(COMP_OBJECT_DEB)"

COMPILER_CFG_REL     = $(COMPILER) $(CC_SW_REL) \
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES))\
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				  -@"$(COMPILER_OPT)" \
				   $(CC_SW_OBJ)"$(COMP_OBJECT_REL)"

COMPILER_DEB     = $(COMPILER) $(CC_SW_DEB) \
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES))\
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				   $(CC_SW_OBJ)"$(COMP_OBJECT_DEB)"


COMPILER_REL     = $(COMPILER) $(CC_SW_REL) \
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES))\
					$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES))\
				   $(STD_INC_PATH) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
				   $(STD_CC_DEFNS) $(USR_CC_DEFNS) \
				   $(CC_SW_OBJ)"$(COMP_OBJECT_REL)"


#   ============================================================================
#   Generate the DSP/BIOS configuration files
#   ============================================================================

source_tcf_deb := $(addsuffix .deb, $(TCF_FILE))
source_tcf_rel := $(addsuffix .rel, $(TCF_FILE))

.PHONY: $(source_tcf_deb) $(source_tcf_rel)

ifeq ("$(TI_DSPLINK_DSPOSVERSION)", "6.XX")
$(source_tcf_deb):
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "$(RMTREE)  $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" ""
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "" "$(CMDECHO)$(MKDIR) $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"
ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_DEBUG_FLAGS_DM6446) "$(basename $@)"
endif
ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_DEBUG_FLAGS_DA8XX) "$(basename $@)"
endif
ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_DEBUG_FLAGS_DA850) "$(basename $@)"
endif
$(source_tcf_rel):
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "$(RMTREE)  $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" ""
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "" "$(CMDECHO)$(MKDIR) $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"
ifeq ("$(TI_DSPLINK_PLATFORM)", "DAVINCI")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_RELEASE_FLAGS_DM6446) "$(basename $@)"
endif
ifeq ("$(TI_DSPLINK_PLATFORM)", "DA8XX")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_RELEASE_FLAGS_DA8XX) "$(basename $@)"
endif
ifeq ("$(TI_DSPLINK_PLATFORM)", "DA850")
	$(CMDECHO)$(XS) xdc.tools.configuro.ccs --xp=$(STD_XDC_PATH) $(STD_XS_RELEASE_FLAGS_DA850) "$(basename $@)"
endif
endif
ifeq ("$(TI_DSPLINK_DSPOSVERSION)", "5.XX")
$(source_tcf_deb):
	$(CMDECHO)$(DISPLAY) Generating DSP/BIOS configuration files ...
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "$(RMTREE)  $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" ""
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "" "$(CMDECHO)$(MKDIR) $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"
	$(CMDECHO)$(TCONF) $(STD_TCF_FLAGS) $(COMP_PATH)$(DIRSEP)$(TCF_FILE) $(TCF_ARGS)
$(source_tcf_rel):
	$(CMDECHO)$(DISPLAY) Generating DSP/BIOS configuration files ...
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "$(RMTREE)  $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" ""
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)" "" "$(CMDECHO)$(MKDIR) $(COMP_BUILD)$(DIRSEP)BIOS_$(BUILD_MODE)"
	$(CMDECHO)$(TCONF) $(STD_TCF_FLAGS) $(COMP_PATH)$(DIRSEP)$(TCF_FILE) $(TCF_ARGS)
endif

#   ============================================================================
#   Generate the build information used for compiling sources for this component
#   ============================================================================
bldinfodeb :
	$(CMDECHO) $(DISPLAY) $(STD_INC_PATH)  $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL)) \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_includes.txt
	$(CMDECHO) $(DISPLAY) $(CC_SW_DEB) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_flags.txt
	$(CMDECHO) $(DISPLAY) $(STD_CC_DEFNS) $(USR_CC_DEFNS) $(XDC_CC_FLAGS) -DDDSP_DEBUG \
			$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES)) \
			$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES)) \
			$(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)$(COMP_NAME)_defines.txt

bldinforel :
	$(CMDECHO) $(DISPLAY) $(STD_INC_PATH) $(addprefix $(CC_SW_INC), $(EXPORT_INC_PLATFORM_INTERNAL)) \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_includes.txt
	$(CMDECHO) $(DISPLAY) $(CC_SW_DEB) $(STD_CC_FLAGS) $(USR_CC_FLAGS) \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_flags.txt
	$(CMDECHO) $(DISPLAY) $(STD_CC_DEFNS) $(USR_CC_DEFNS) $(XDC_CC_FLAGS) -DDDSP_DEBUG \
			$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP_COMMON_DEFINES)) \
			$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_DSP$(TI_DSPLINK_PROCID)_DEFINES)) \
			$(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)$(COMP_NAME)_defines.txt


#   ============================================================================
#   Rules to compile sources
#   ============================================================================

sources_deb := $(addsuffix .deb, $(SOURCES))
sources_rel := $(addsuffix .rel, $(SOURCES))
sources_cfg_deb := $(addsuffix .deb, $(SOURCES_CFG))
sources_cfg_rel := $(addsuffix .rel, $(SOURCES_CFG))

.PHONY: $(sources_deb) $(sources_rel) $(sources_cfg_deb) $(sources_cfg_rel)


#   ----------------------------------------------------------------------------
#   @rule   objdeb
#
#   @desc   Builds the object files for DEBUG variant.
#   ----------------------------------------------------------------------------

objdeb  :  $(source_tcf_deb) $(sources_deb) $(sources_cfg_deb)

$(sources_deb):
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_DEB) $(basename $@)

#   ----------------------------------------------------------------------------
#   @rule   sources_cfg_deb
#
#   @desc   Builds the object files for DEBUG variant for xdc generated file
#           cfg.c.
#   ----------------------------------------------------------------------------
$(sources_cfg_deb):
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_CFG_DEB) $(basename $@)


#   ----------------------------------------------------------------------------
#   @rule   objrel
#
#   @desc   Builds the object files for RELEASE variant.
#   ----------------------------------------------------------------------------

objrel  :  $(source_tcf_rel) $(sources_rel) $(sources_cfg_rel)

$(sources_rel):
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_REL) $(basename $@)

#   ----------------------------------------------------------------------------
#   @rule   sources_cfg_rel
#
#   @desc   Builds the object files for RELEASE variant for xdc generated file
#           cfg.c.
#   ----------------------------------------------------------------------------
$(sources_cfg_rel):
	$(CMDECHO)$(DISPLAY) Compiling $(basename $(notdir $@))...
	$(CMDECHO)$(COMPILER_CFG_REL) $(basename $@)


endif   # ifndef COMPILE_MK
