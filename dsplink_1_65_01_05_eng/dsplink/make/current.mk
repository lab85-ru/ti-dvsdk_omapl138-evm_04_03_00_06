#   ============================================================================
#   @file   current.mk
#
#   @path   $(DSPLINK)/make/
#
#   @desc   This makefile builds a specific component.
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


ifndef CURRENT_MK

define CURRENT_MK
endef

include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)systools.mk
include $(MAKEROOT)$(DIRSEP)gendefs.mk
include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)osdefs.mk
include $(MAKEROOT)$(DIRSEP)rulebase.mk

ifeq ("$(COMP_NAME)", "")
%:force
	$(CMDHIDE)$(BANNER) $(BANNER_L2) NO FILES BUILD
force: ;
else

#   ============================================================================
#   Rules to create directory structure
#   ============================================================================

dir_build   := $(BUILD_DIR)

dir_comp    := $(COMP_BUILD)
dir_comp    += $(COMP_OBJECT)
dir_comp    += $(COMP_BINARY)


$(dir_build) :
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"

$(dir_comp) : $(dir_build)
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"


#   ----------------------------------------------------------------------------
#   @rule   dirinc
#
#   @desc   Creates the directories for storing header files
#           exported from a component
#   ----------------------------------------------------------------------------

dir_export_inc  := $(EXPORT_INC_GENERIC)
dir_export_inc  += $(EXPORT_INC_BUILDOS)
dir_export_inc  += $(EXPORT_INC_PLATFORM)

ifneq ("$(VARIANT)", "")
dir_export_inc  += $(EXPORT_INC_VARIANT)
endif

dir_build_inc   := $(BUILD_INCLUDE)


dirincbanner :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) DIRS INCLUDE

$(dir_export_inc) :
	$(CMDECHO)$(RUNIF) "exist $(@)" ""  "$(CMDECHO)$(MKDIR) $(@)"

$(dir_build_inc) : $(dir_build)
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"
ifeq ("$(DEVICETYPE)", "GPP")
ifneq ("$(COMP_MEMSPACE)", "")
	$(CMDECHO)$(RUNIF) "exist $(BUILD_USER_INCLUDE)" "" "$(CMDECHO)$(MKDIR) $(BUILD_USER_INCLUDE)"
	$(CMDECHO)$(RUNIF) "exist $(BUILD_KRNL_INCLUDE)" "" "$(CMDECHO)$(MKDIR) $(BUILD_KRNL_INCLUDE)"
endif
endif


dirinc  : dirincbanner $(dir_export_inc) $(dir_build_inc)


#   ----------------------------------------------------------------------------
#   @rule   dirdeb
#
#   @desc   Creates the directories for storing generated object and target
#           files for the DEBUG build variant.
#   ----------------------------------------------------------------------------

dir_comp_deb    := $(COMP_OBJECT_DEB)
dir_comp_deb    += $(COMP_BINARY_DEB)


dirdebbanner :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) DIRS DEBUG

$(dir_comp_deb) : $(dir_comp)
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"


dirdeb  : dirdebbanner $(dir_export_inc) $(dir_comp_deb)


#   ----------------------------------------------------------------------------
#   @rule   dirrel
#
#   @desc   Creates the directories for storing generated object and target
#           files for the RELEASE build variant.
#   ----------------------------------------------------------------------------

dir_comp_rel := $(COMP_OBJECT_REL)
dir_comp_rel += $(COMP_BINARY_REL)


dirrelbanner :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) DIRS RELEASE

$(dir_comp_rel) :  $(dir_comp)
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"


dirrel  : dirrelbanner $(dir_export_inc) $(dir_comp_rel)


#   ----------------------------------------------------------------------------
#   @rule   direxp
#
#   @desc   Creates the directories for storing object and target files
#           exported from a component.
#   ----------------------------------------------------------------------------

dir_export_bin  := $(EXPORT_BIN_GENERIC)
dir_export_bin  += $(EXPORT_BIN_BUILDOS)
dir_export_bin  += $(EXPORT_BIN_PLATFORM)

ifneq ("$(VARIANT)", "")
dir_export_bin  += $(EXPORT_BIN_VARIANT)
endif

dir_export_bin  += $(EXPORT_BIN_DEB)
dir_export_bin  += $(EXPORT_BIN_REL)


dir_build_exp   := $(BUILD_EXPORT)
dir_build_exp   += $(BUILD_EXPORT_DEB)
dir_build_exp   += $(BUILD_EXPORT_REL)


direxpbanner :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) DIRS EXPORT

$(dir_export_bin) :
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"

$(dir_build_exp) : $(dir_build)
	$(CMDECHO)$(RUNIF) "exist $(@)" "" "$(CMDECHO)$(MKDIR) $(@)"


direxp  : direxpbanner $(dir_export_bin) $(dir_build_exp)


#   ============================================================================
#   Rules to clean the generated objects and targets
#   ============================================================================

#   ----------------------------------------------------------------------------
#   @rule   cleandeb
#
#   @desc   Cleans the file generated during the build for DEBUG variant.
#   ----------------------------------------------------------------------------

cleandeb:
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) CLEAN DEBUG
	$(CMDECHO)$(RUNIF) "exist $(COMP_OBJECT_DEB)" "$(DELETE) $(COMP_OBJECT_DEB)$(DIRSEP)*.*" "" >$(TRASH)
	$(CMDECHO)$(RUNIF) "exist $(COMP_BINARY_DEB)" "$(DELETE) $(COMP_BINARY_DEB)$(DIRSEP)*.*" "" >$(TRASH)


#   ----------------------------------------------------------------------------
#   @rule   cleanrel
#
#   @desc   Cleans the file generated during the build for RELEASE variant.
#   ----------------------------------------------------------------------------

cleanrel:
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) CLEAN RELEASE
	$(CMDECHO)$(RUNIF) "exist $(COMP_OBJECT_REL)" "$(DELETE) $(COMP_OBJECT_REL)$(DIRSEP)*.*" "" >$(TRASH)
	$(CMDECHO)$(RUNIF) "exist $(COMP_BINARY_REL)" "$(DELETE) $(COMP_BINARY_REL)$(DIRSEP)*.*" "" >$(TRASH)


#   ----------------------------------------------------------------------------
#   @rule   clobber
#
#   @desc   Deletes the directories created during the build process.
#   ----------------------------------------------------------------------------

clobber :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) CLOBBER
	$(CMDECHO)$(RUNIF) "exist $(COMP_BUILD)" "$(RMTREE) $(COMP_BUILD)" ""


#   ============================================================================
#   Rules to export header files from a component
#   ============================================================================

#   ----------------------------------------------------------------------------
#   @rule   includes
#
#   @desc   Exports header files from a component
#   ----------------------------------------------------------------------------

.PHONY: $(EXP_HEADERS)


incbanner:
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) INCLUDES

$(EXP_HEADERS):
ifeq ("$(DEVICETYPE)", "GPP")
ifeq ("$(COMP_MEMSPACE)", "USER")
	$(CMDECHO)$(COPY) $(COMP_PATH)$(DIRSEP)$(@) $(BUILD_USER_INCLUDE)$(DIRSEP). >$(TRASH)
endif

ifeq ("$(COMP_MEMSPACE)", "KRNL")
	$(CMDECHO)$(COPY) $(COMP_PATH)$(DIRSEP)$(@) $(BUILD_KRNL_INCLUDE)$(DIRSEP). >$(TRASH)
endif

ifeq ("$(COMP_MEMSPACE)", "")
	$(CMDECHO)$(COPY) $(COMP_PATH)$(DIRSEP)$(@) $(BUILD_INCLUDE)$(DIRSEP). >$(TRASH)
endif
else
	$(CMDECHO)$(COPY) $(COMP_PATH)$(DIRSEP)$(@) $(BUILD_INCLUDE)$(DIRSEP). >$(TRASH)
endif
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_INTERNAL)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_INTERNAL)"
	
	$(CMDECHO)$(COPY) $(COMP_PATH)$(DIRSEP)$(@) $(EXPORT_INC_PLATFORM_INTERNAL)  >$(TRASH)


includes: incbanner dirinc $(EXP_HEADERS)


#   ============================================================================
#   Rules to build object files.
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
CMD_MAKEOBJ_GENERIC  = \
	$(RUNIF) "exist $(SOURCE_GENERIC)" \
	"$(MAKE) -C $(SOURCE_GENERIC) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""

CMD_MAKEOBJ_BUILDOS    = \
	$(RUNIF) "exist $(SOURCE_BUILDOS)" \
	"$(MAKE) -C $(SOURCE_BUILDOS) -f    $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""

CMD_MAKEOBJ_BUILDOS_VER  = \
	$(RUNIF) "exist $(SOURCE_BUILDOS_VER)" \
	"$(MAKE) -C $(SOURCE_BUILDOS_VER) -f    $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""
endif	

ifeq ("$(DEVICETYPE)", "DSP")
CMD_MAKEOBJ_GENERIC  = \
	$(RUNIF) "exist $(SOURCE_GENERIC)" \
	"$(MAKE) -C $(SOURCE_GENERIC) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""

CMD_MAKEOBJ_BUILDOS    = \
	$(RUNIF) "exist $(SOURCE_BUILDOS)" \
	"$(MAKE) -C $(SOURCE_BUILDOS) -f    $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""

CMD_MAKEOBJ_BUILDOS_VER  = \
	$(RUNIF) "exist $(SOURCE_BUILDOS_VER)" \
	"$(MAKE) -C $(SOURCE_BUILDOS_VER) -f    $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""

CMD_MAKEOBJ_OS = \
	$(RUNIF) "exist $(SOURCE_OS)" \
	"$(MAKE) -C $(SOURCE_OS) -f $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""
	
CMD_MAKEOBJ_OS_DSPDEVICE = \
	$(RUNIF) "exist $(SOURCE_OS_DSPDEVICE)" \
	"$(MAKE) -C $(SOURCE_OS_DSPDEVICE) -f $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""
	
CMD_MAKEOBJ_OS_VER_DSPDEVICE = \
	$(RUNIF) "exist $(SOURCE_OS_VER_DSPDEVICE)" \
	"$(MAKE) -C $(SOURCE_OS_VER_DSPDEVICE) -f $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk     \
	$@ VERBOSE=$(VERBOSE)" ""
endif	


#   ============================================================================
#   Rule to generate the build information for a component
#   ============================================================================
ifeq ("$(DEVICETYPE)", "GPP")
CMD_GEN_BLDINFO  = \
	$(MAKE) -C $(SOURCE_GENERIC) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)compile.mk \
	$@ VERBOSE=$(VERBOSE)
endif

ifeq ("$(DEVICETYPE)", "DSP")
CMD_GEN_BLDINFO  = \
	$(MAKE) -C $(SOURCE_GENERIC) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)compile.mk \
	$@ VERBOSE=$(VERBOSE)
endif

#   ----------------------------------------------------------------------------
#   @rule   objdeb
#
#   @desc   Builds the object files for DEBUG variant.
#   ----------------------------------------------------------------------------

objdeb  : dirdeb
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) OBJECT DEBUG 
	$(CMDECHO)$(CMD_MAKEOBJ_GENERIC)
	$(CMDECHO)$(CMD_MAKEOBJ_BUILDOS)
	$(CMDECHO)$(CMD_MAKEOBJ_BUILDOS_VER)
ifeq ("$(DEVICETYPE)", "DSP") 
	$(CMDECHO)$(CMD_MAKEOBJ_OS)
	$(CMDECHO)$(CMD_MAKEOBJ_OS_DSPDEVICE)
	$(CMDECHO)$(CMD_MAKEOBJ_OS_VER_DSPDEVICE)
endif

#   ----------------------------------------------------------------------------
#   @rule   objrel
#
#   @desc   Builds the object files for RELEASE variant.
#   ----------------------------------------------------------------------------

objrel  : dirrel
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) OBJECT RELEASE
	$(CMDECHO)$(CMD_MAKEOBJ_GENERIC)
	$(CMDECHO)$(CMD_MAKEOBJ_BUILDOS)
	$(CMDECHO)$(CMD_MAKEOBJ_BUILDOS_VER)
ifeq ("$(DEVICETYPE)", "DSP") 
	$(CMDECHO)$(CMD_MAKEOBJ_OS)
	$(CMDECHO)$(CMD_MAKEOBJ_OS_DSPDEVICE)
	$(CMDECHO)$(CMD_MAKEOBJ_OS_VER_DSPDEVICE)
endif

#   ----------------------------------------------------------------------------
#   @rule   bldinfodeb
#
#   @desc   Generate build information for the debug build.
#   ----------------------------------------------------------------------------

bldinfodeb  : dirdeb
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) BLDINFO DEBUG
	$(CMDECHO)$(CMD_GEN_BLDINFO)

#   ----------------------------------------------------------------------------
#   @rule   bldinforel
#
#   @desc   Generate build information for the debug build.
#   ----------------------------------------------------------------------------

bldinforel  : dirrel
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) BLDINFO RELEASE
	$(CMDECHO)$(CMD_GEN_BLDINFO)

#   ============================================================================
#   Rules to build target files.
#   ============================================================================

#   ----------------------------------------------------------------------------
#   @rule   trgdeb
#
#   @desc   Builds the target files for DEBUG variant.
#   ----------------------------------------------------------------------------

trgdeb  :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) TARGET DEBUG
ifeq ("$(DEVICETYPE)", "GPP")
	$(MAKE) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)link.mk $@ VERBOSE=$(VERBOSE)
endif
ifeq ("$(DEVICETYPE)", "DSP")
	$(MAKE) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)link.mk $@ VERBOSE=$(VERBOSE)
endif

#   ----------------------------------------------------------------------------
#   @rule   trgrel
#
#   @desc   Builds the target files for RELEASE variant.
#   ----------------------------------------------------------------------------

trgrel  :
	$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) TARGET RELEASE
ifeq ("$(DEVICETYPE)", "GPP")
	$(MAKE) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_PLATFORM)$(DIRSEP)link.mk $@ VERBOSE=$(VERBOSE)
endif
ifeq ("$(DEVICETYPE)", "DSP")
	$(MAKE) -f  $(PROJ_MAK_BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICECLASS)$(DIRSEP)link.mk $@ VERBOSE=$(VERBOSE)
endif

#   ============================================================================
#   Rules to export target files.
#   ============================================================================

export_deb  := $(addsuffix .deb, $(EXP_TARGETS))
export_rel  := $(addsuffix .rel, $(EXP_TARGETS))

#   ============================================================================
#   Variable to create dsplink archive
#   ============================================================================
ifeq ("$(COMP_TARGET)","dsplink")
	ARCHIVE_DEB  = $(subst lib,a,$(basename $(@)))
	ARCHIVE_REL  = $(subst lib,a,$(basename $(@)))
endif


.PHONY: $(export_deb) $(export_rel)


$(export_deb):
		$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) EXPORT DEBUG
		$(CMDECHO)$(COPY) $(BUILD_EXPORT_DEB)$(DIRSEP)$(basename $@)                  \
		$(EXPORT_BIN_DEB)$(DIRSEP). >$(TRASH)
ifeq ("$(DEVICETYPE)", "DSP")
ifeq ("$(COMP_TYPE)", "EXE")
		$(CMDECHO)$(RUNIF) "exist $(BUILD_EXPORT_DEB)$(DIRSEP)$(COMP_MAP_FILE)"       \
                "$(COPY)  $(BUILD_EXPORT_DEB)$(DIRSEP)$(COMP_MAP_FILE) $(EXPORT_BIN_DEB)$(DIRSEP). >$(TRASH)" ""
endif
endif

ifeq ("$(COMP_TARGET)","dsplink")
	$(CMDECHO)$(COPY) $(COMP_BINARY_DEB)/$(ARCHIVE_DEB)                      \
	$(EXPORT_BIN_DEB)$(DIRSEP). >$(TRASH)
endif

$(export_rel):
		$(CMDHIDE)$(BANNER) $(BANNER_L2) $(COMP_NAME) EXPORT RELEASE
		$(CMDECHO)$(COPY) $(BUILD_EXPORT_REL)$(DIRSEP)$(basename $@)                  \
		$(EXPORT_BIN_REL)$(DIRSEP). >$(TRASH)
ifeq ("$(DEVICETYPE)", "DSP")
ifeq ("$(COMP_TYPE)", "EXE")
		$(CMDECHO)$(RUNIF) "exist $(BUILD_EXPORT_REL)$(DIRSEP)$(COMP_MAP_FILE)"       \
                "$(COPY)  $(BUILD_EXPORT_REL)$(DIRSEP)$(COMP_MAP_FILE) $(EXPORT_BIN_REL)$(DIRSEP). >$(TRASH)" ""
endif
endif

ifeq ("$(COMP_TARGET)","dsplink")
	$(CMDECHO)$(COPY) $(COMP_BINARY_REL)/$(ARCHIVE_REL)                      \
	$(EXPORT_BIN_REL)$(DIRSEP). >$(TRASH)
endif


#   ----------------------------------------------------------------------------
#   @rule   expdeb
#
#   @desc   Builds the target files for DEBUG variant.
#   ----------------------------------------------------------------------------

expdeb  : $(export_deb)


#   ----------------------------------------------------------------------------
#   @rule   exprel
#
#   @desc   Builds the target files for RELEASE variant.
#   ----------------------------------------------------------------------------

exprel  : $(export_rel)

#   ----------------------------------------------------------------------------
#   @rule   dsplinkdir
#
#   @desc   Dump information regarding the current setting of DSPLINK
#   ----------------------------------------------------------------------------

dsplinkdir  :
	$(CMDECHO) $(DISPLAY) $(DSPLINK) $(SENDTO) $(EXPORT_BIN_DEB)$(DIRSEP)DSPLINK.txt
	$(CMDECHO) $(DISPLAY) $(DSPLINK) $(SENDTO) $(EXPORT_BIN_REL)$(DIRSEP)DSPLINK.txt

endif
endif   # ifndef CURRENT_MK
