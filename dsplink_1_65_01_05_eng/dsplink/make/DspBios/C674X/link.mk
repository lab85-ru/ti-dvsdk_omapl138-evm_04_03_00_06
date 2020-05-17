#   ============================================================================
#   @file   link.mk
#
#   @path   $(DSPLINK)/make/DspBios/C674X/
#
#   @desc   This makefile contains rules to link sources.
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


ifndef LINK_MK

define LINK_MK
endef


include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)systools.mk
include $(MAKEROOT)$(DIRSEP)gendefs.mk
include $(MAKEROOT)$(DIRSEP)$(BUILDOS)$(DIRSEP)osdefs.mk
include $(MAKEROOT)$(DIRSEP)rulebase.mk

-include $(BUILDOS)$(DIRSEP)$(COMP_DETAILS)
-include $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(COMP_SOURCES)
-include $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)$(DIRSEP)$(COMP_SOURCES)
ifneq ("$(TI_DSPLINK_DSPDEVICE_EXTERNAL)", "")
-include $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE_EXTERNAL)$(DIRSEP)$(COMP_SOURCES)
else
-include $(COMP_PATH)$(DIRSEP)$(BUILDOS)$(DIRSEP)$(BUILDOSVERSION)$(DIRSEP)$(TI_DSPLINK_DSPDEVICE)$(DIRSEP)$(COMP_SOURCES)
endif



#   ============================================================================
#   Clear environment variables
#   ============================================================================

INCLUDE     :=
LIB         :=


#   ============================================================================
#   Search path for standard libraries
#   ============================================================================

STD_LIB_PATH    := $(addprefix $(LD_SW_INC), $(OSLIB_GENERIC))
STD_LIB_PATH    += $(addprefix $(LD_SW_INC), $(OSLIB_PLATFORM))

ifneq ("$(OSLIB_VARIANT)", "")
STD_LIB_PATH    += $(addprefix $(LD_SW_INC), $(OSLIB_VARIANT))
endif


#   ============================================================================
#   Rules to build targets
#   ============================================================================

target_deb  := $(addsuffix .deb, $(COMP_TARGET))
target_rel  := $(addsuffix .rel, $(COMP_TARGET))


#   ============================================================================
#   @rule   trgdeb
#
#   @desc   Builds the target for DEBUG variant.
#   ============================================================================

ifeq ("$(COMP_TYPE)", "ARC")
trgdeb  : arcdeb
endif

ifeq ("$(COMP_TYPE)", "EXE")
trgdeb  : exedeb
endif


#   ============================================================================
#   @rule   trgrel
#
#   @desc   Builds the target for RELEASE variant.
#   ============================================================================

ifeq ("$(COMP_TYPE)", "ARC")
trgrel  : arcrel
endif

ifeq ("$(COMP_TYPE)", "EXE")
trgrel  : exerel
endif


#   ############################################################################
#   BUILD AN ARCHIVE
#   ############################################################################


ifeq ("$(COMP_TYPE)", "ARC")

#   ----------------------------------------------------------------------------
#   Name of the target archive
#   ----------------------------------------------------------------------------

ARCHIVE_DEB   = $(COMP_BINARY_DEB)$(DIRSEP)$(basename $(@))
ARCHIVE_REL   = $(COMP_BINARY_REL)$(DIRSEP)$(basename $(@))

#   ----------------------------------------------------------------------------
#   Append suitable path to each user defined library
#   ----------------------------------------------------------------------------

ifeq ("$(USR_LIBS)", "")

USR_LIBS_DEB    :=
USR_LIBS_REL    :=

DEPENDS_DEB     :=
DEPENDS_REL     :=

ARGS_ARCH_DEB   := $(COMP_OBJECT_DEB)
ARGS_ARCH_REL   := $(COMP_OBJECT_REL)

else

DEPENDS_DEB     := $(COMP_OBJECT_DEB)$(DIRSEP)DEPEND
DEPENDS_REL     := $(COMP_OBJECT_REL)$(DIRSEP)DEPEND

USR_LIBS_DEB    := $(addprefix $(BUILD_EXPORT_DEB)$(DIRSEP), $(USR_LIBS))
USR_LIBS_REL    := $(addprefix $(BUILD_EXPORT_REL)$(DIRSEP), $(USR_LIBS))

ARGS_ARCH_DEB   := $(COMP_OBJECT_DEB)     $(DEPENDS_DEB)
ARGS_ARCH_REL   := $(COMP_OBJECT_REL)     $(DEPENDS_REL)

endif

CMD_ARCHIVE     := $(ARCHIVER) $(STD_AR_FLAGS)


#   ----------------------------------------------------------------------------
#   @rule   $(USR_LIBS_DEB), mkdepend_deb
#
#   @desc   Gets the dependent libraries for the archive for DEBUG variant.
#   ----------------------------------------------------------------------------

.PHONY      : mkdepend_deb $(USR_LIBS_DEB)

mkdepend_deb :
	$(CMDHIDE)$(DISPLAY) Initialize DEPENDS directory...
	$(CMDECHO)$(RUNIF) "exist $(DEPENDS_DEB)" "$(RMTREE) $(DEPENDS_DEB)"
	$(CMDECHO)$(MKDIR) $(DEPENDS_DEB)

$(USR_LIBS_DEB): mkdepend_deb
	$(CMDECHO)$(DISPLAY) Getting dependency $@ ...
	$(CMDECHO)$(COPY) $@ $(DEPENDS_DEB)$(DIRSEP). > $(TRASH)
	$(CMDECHO)$(CHDIR) $(DEPENDS_DEB) && $(CMDECHO)$(ARCHIVER) $(EXT_AR_FLAGS) $@

#   ----------------------------------------------------------------------------
#   @rule   arcdeb
#
#   @desc   Builds the target archive for DEBUG variant.
#   ----------------------------------------------------------------------------

$(target_deb): $(USR_LIBS_DEB)
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(COPYFILES) "$(PROJ_INCLUDE)" "$(EXPORT_INC_PLATFORM)" > $(TRASH)
	$(CMDECHO)$(CMD_ARCHIVE) $(ARCHIVE_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.obj)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(ARCHIVE_DEB) $(BUILD_EXPORT_DEB)$(DIRSEP). > $(TRASH)

arcdeb  : $(target_deb)


#   ----------------------------------------------------------------------------
#   @rule   $(USR_LIBS_REL), mkdepend_rel
#
#   @desc   Gets the dependent libraries for the archive for RELEASE variant.
#   ----------------------------------------------------------------------------

.PHONY      : mkdepend_rel $(USR_LIBS_REL)

mkdepend_rel :
	$(CMDHIDE)$(DISPLAY) Initialize DEPENDS directory...
	$(CMDECHO)$(RUNIF) "exist $(DEPENDS_REL)" "$(RMTREE) $(DEPENDS_REL)"
	$(CMDECHO)$(MKDIR) $(DEPENDS_REL)

$(USR_LIBS_REL): mkdepend_rel
	$(CMDECHO)$(DISPLAY) Getting dependency $@ ...
	$(CMDECHO)$(COPY) $@ $(DEPENDS_REL)$(DIRSEP). > $(TRASH)
	$(CMDECHO)$(CHDIR) $(DEPENDS_REL) && $(CMDECHO)$(ARCHIVER) $(EXT_AR_FLAGS) $@

#   ----------------------------------------------------------------------------
#   @rule   arcrel
#
#   @desc   Builds the target archive for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel): $(USR_LIBS_REL)
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(COPYFILES) "$(PROJ_INCLUDE)" "$(EXPORT_INC_PLATFORM)" > $(TRASH)
	$(CMDECHO)$(CMD_ARCHIVE) $(ARCHIVE_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.obj)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(ARCHIVE_REL) $(BUILD_EXPORT_REL)$(DIRSEP).

arcrel  : $(target_rel)


endif   # ifeq ("$(COMP_TYPE)", "ARC")


#   ############################################################################
#   BUILD AN EXECUTABLE
#   ############################################################################


ifeq ("$(COMP_TYPE)", "EXE")
#   ----------------------------------------------------------------------------
#   Name of the target executable
#   ----------------------------------------------------------------------------
ifneq ("$(TI_DSPLINK_DSPBINPATH)", "")
EXECUTABLE_DEB   = $(TI_DSPLINK_DSPBINPATH)$(DIRSEP)$(basename $(@))
EXECUTABLE_REL   = $(TI_DSPLINK_DSPBINPATH)$(DIRSEP)$(basename $(@))
else
EXECUTABLE_DEB   = $(COMP_BINARY_DEB)$(DIRSEP)$(basename $(@))
EXECUTABLE_REL   = $(COMP_BINARY_REL)$(DIRSEP)$(basename $(@))
endif

#   ----------------------------------------------------------------------------
#   Append suitable path to each user defined library
#   ----------------------------------------------------------------------------

ifeq ("$(USR_LIBS)", "")

USR_LIBS_DEB  :=
USR_LIBS_REL  :=

else

DEPENDS_DEB   := $(COMP_OBJECT_DEB)$(DIRSEP)DEPEND
DEPENDS_REL   := $(COMP_OBJECT_REL)$(DIRSEP)DEPEND

USR_LIBS_DEB  := $(addprefix $(BUILD_EXPORT_DEB)$(DIRSEP), $(USR_LIBS))
USR_LIBS_REL  := $(addprefix $(BUILD_EXPORT_REL)$(DIRSEP), $(USR_LIBS))

ARGS_ARCH_DEB := $(COMP_OBJECT_DEB) $(DEPENDS_DEB)
ARGS_ARCH_REL := $(COMP_OBJECT_REL) $(DEPENDS_REL)

endif

MAP_FILE_DEB  := $(BUILD_EXPORT_DEB)$(DIRSEP)$(COMP_MAP_FILE)
MAP_FILE_REL  := $(BUILD_EXPORT_REL)$(DIRSEP)$(COMP_MAP_FILE)

CMD_LINK      := $(LINKER) $(STD_LIB_PATH) $(EXE_LD_FLAGS) $(STD_LD_FLAGS)


#   ----------------------------------------------------------------------------
#   @rule   exedeb
#
#   @desc   Builds the target executable for DEBUG variant.
#   ----------------------------------------------------------------------------

$(target_deb):
	$(CMDECHO)$(DISPLAY) Generating $(basename $(@))...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(EXECUTABLE_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.obj)) $(LD_SW_LIB)$(USR_LIBS_DEB) $(LD_SW_MAP)$(MAP_FILE_DEB) $(CMD_FILE) $(USR_LD_FLAGS)
	$(CMDECHO)$(COPY) $(EXECUTABLE_DEB) $(BUILD_EXPORT_DEB)$(DIRSEP).

exedeb  : $(target_deb)


#   ----------------------------------------------------------------------------
#   @rule   exerel
#
#   @desc   Builds the target executable for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel):
	$(CMDECHO)$(DISPLAY) Generating $(basename $(@))...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(EXECUTABLE_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.obj)) $(LD_SW_LIB)$(USR_LIBS_REL) $(LD_SW_MAP)$(MAP_FILE_REL) $(CMD_FILE) $(USR_LD_FLAGS)
	$(CMDECHO)$(COPY) $(EXECUTABLE_REL) $(BUILD_EXPORT_REL)$(DIRSEP).

exerel  : $(target_rel)


endif   # ifeq ("$(COMP_TYPE)", "EXE")

endif   # ifndef LINK_MK
