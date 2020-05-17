#   ============================================================================
#   @file   link.mk
#
#   @path   $(DSPLINK)/make/Linux/OMAPL1XX/
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


include $(MAKEROOT)/$(BUILDOS)/systools.mk
include $(MAKEROOT)/gendefs.mk
include $(MAKEROOT)/$(BUILDOS)/osdefs.mk
include $(MAKEROOT)/rulebase.mk

-include $(BUILDOS)/$(COMP_DETAILS)
-include $(BUILDOS)/$(TI_DSPLINK_GPPOSVERSION)/$(COMP_DETAILS)


#   ============================================================================
#   Clear environment variables
#   ============================================================================

INCLUDE     :=
LIB         :=


#   ============================================================================
#   Search path for standard libraries
#   ============================================================================

STD_LIB_PATH    := $(addprefix -L, $(OSLIB_GENERIC))
STD_LIB_PATH    += $(addprefix -L, $(OSLIB_PLATFORM))

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

ifeq ("$(COMP_TYPE)", "LIB")
trgdeb  : libdeb
endif

ifeq ("$(COMP_TYPE)", "DRV")
trgdeb  : drvdeb
endif

ifeq ("$(COMP_TYPE)", "EXE")
trgdeb  : exedeb
endif


#   ============================================================================
#   @rule   trgrel
#
#   @desc   Builds the target for RELEASE variant.
#   ============================================================================

ifeq ("$(COMP_TYPE)", "LIB")
trgrel  : librel
endif

ifeq ("$(COMP_TYPE)", "DRV")
trgrel  : drvrel
endif

ifeq ("$(COMP_TYPE)", "EXE")
trgrel  : exerel
endif


#   ############################################################################
#   BUILD AN ARCHIVE
#   ############################################################################


ifeq ("$(COMP_TYPE)", "LIB")

#   ----------------------------------------------------------------------------
#   Name of the target archive & Library
#   ----------------------------------------------------------------------------

ARCHIVE_DEB   = $(COMP_BINARY_DEB)/$(basename $(basename $(@))).a
ARCHIVE_REL   = $(COMP_BINARY_REL)/$(basename $(basename $(@))).a
LIB_DEB       = $(COMP_BINARY_DEB)/$(basename $(basename $(@))).o
LIB_REL       = $(COMP_BINARY_REL)/$(basename $(basename $(@))).o

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

depend_deb      :=
depend_rel      :=

else

DEPENDS_DEB     := $(COMP_OBJECT_DEB)/DEPEND
DEPENDS_REL     := $(COMP_OBJECT_REL)/DEPEND

USR_LIBS_DEB    := $(addprefix $(BUILD_EXPORT_DEB)/, $(USR_LIBS))
USR_LIBS_REL    := $(addprefix $(BUILD_EXPORT_REL)/, $(USR_LIBS))

ARGS_ARCH_DEB   := $(COMP_OBJECT_DEB) $(DEPENDS_DEB)
ARGS_ARCH_REL   := $(COMP_OBJECT_REL) $(DEPENDS_REL)

depend_deb      := $(addsuffix .dep, $(USR_LIBS_DEB))
depend_rel      := $(addsuffix .dep, $(USR_LIBS_REL))

endif

CMD_ARCHIVE     := $(ARCHIVER) $(STD_AR_FLAGS)
CMD_LINK        := $(LD) $(LD_SW_RELOC)

#   ----------------------------------------------------------------------------
#   @rule   libdeb
#
#   @desc   Builds the target library for DEBUG variant.
#   ----------------------------------------------------------------------------

$(target_deb):
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_USR)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_USR)"
	$(CMDECHO)$(COPYFILES) "$(PROJ_INC_USR)" "$(EXPORT_INC_PLATFORM_USR)" > $(TRASH)
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS)"

	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS_ARCH)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS_ARCH)"
	$(CMDECHO)$(COPYFILES) "$(PROJ_INC_ARCH)" "$(EXPORT_INC_PLATFORM_SYS_ARCH)" > $(TRASH)
	$(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)")
	$(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CMDECHO)$(COPYFILES) "$(PROJ_INC_ARCH)$(DIRSEP)$(device)" "$(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)" > $(TRASH))
	$(CMDECHO)$(DISPLAY) Generating library : $(LIB_DEB)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(LIB_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(LIB_DEB) $(BUILD_EXPORT_DEB)/.
	$(CMDECHO)$(DISPLAY) Generating library : $(basename $(basename $(LIB_DEB))).lib...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(basename $(basename $(LIB_DEB))).lib $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(basename $(basename $(LIB_DEB))).lib $(BUILD_EXPORT_DEB)/.
	$(CMDECHO)$(DISPLAY) Generating archive : $(ARCHIVE_DEB)...
	$(CMDECHO)$(CMD_ARCHIVE) $(ARCHIVE_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(ARCHIVE_DEB) $(BUILD_EXPORT_DEB)/.

libdeb  : $(target_deb)


#   ----------------------------------------------------------------------------
#   @rule   librel
#
#   @desc   Builds the target library for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel):
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_USR)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_USR)"
	$(CMDECHO)$(COPYFILES) "$(PROJ_INC_USR)" "$(EXPORT_INC_PLATFORM_USR)" > $(TRASH)
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS)"
	$(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS_ARCH)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS_ARCH)"
	$(CMDECHO)$(COPYFILES) "$(PROJ_INC_ARCH)" "$(EXPORT_INC_PLATFORM_SYS_ARCH)" > $(TRASH)
	$(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CMDECHO)$(RUNIF) "exist $(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)" "" "$(MKDIR) $(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)")
	$(foreach device, $(TI_DSPLINK_DSPDEVICES), $(CMDECHO)$(COPYFILES) "$(PROJ_INC_ARCH)$(DIRSEP)$(device)" "$(EXPORT_INC_PLATFORM_SYS_ARCH)$(DIRSEP)$(device)" > $(TRASH))
	$(CMDECHO)$(DISPLAY) Generating library : $(LIB_REL)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(LIB_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(LIB_REL) $(BUILD_EXPORT_REL)/.
	$(CMDECHO)$(DISPLAY) Generating library : $(basename $(basename $(LIB_REL))).lib...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(basename $(basename $(LIB_REL))).lib $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(basename $(basename $(LIB_REL))).lib $(BUILD_EXPORT_REL)/.
	$(CMDECHO)$(DISPLAY) Generating archive : $(ARCHIVE_REL)...
	$(CMDECHO)$(CMD_ARCHIVE) $(ARCHIVE_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(ARCHIVE_REL) $(BUILD_EXPORT_REL)/.

librel  : $(target_rel)


endif   # ifeq ("$(COMP_TYPE)", "LIB")


#   ############################################################################
#   BUILD A DRIVER MODULE
#   ############################################################################


ifeq ("$(COMP_TYPE)", "DRV")
#   ----------------------------------------------------------------------------
#   Name of the target driver module
#   ----------------------------------------------------------------------------
DRIVER_DEB   = $(COMP_BINARY_DEB)/$(basename $(@))
DRIVER_REL   = $(COMP_BINARY_REL)/$(basename $(@))

#   ----------------------------------------------------------------------------
#   Append suitable path to each user defined library
#   ----------------------------------------------------------------------------

ifeq ("$(USR_LIBS)", "")

USR_LIBS_DEB    :=
USR_LIBS_REL    :=

ARGS_ARCH_DEB   := $(COMP_OBJECT_DEB)
ARGS_ARCH_REL   := $(COMP_OBJECT_REL)

else

DEPENDS_DEB     := $(COMP_OBJECT_DEB)/DEPEND
DEPENDS_REL     := $(COMP_OBJECT_REL)/DEPEND

USR_LIBS_DEB    := $(addprefix $(BUILD_EXPORT_DEB)/, $(USR_LIBS))
USR_LIBS_REL    := $(addprefix $(BUILD_EXPORT_REL)/, $(USR_LIBS))

ARGS_ARCH_DEB   := $(COMP_OBJECT_DEB) $(DEPENDS_DEB)
ARGS_ARCH_REL   := $(COMP_OBJECT_REL) $(DEPENDS_REL)

endif

#   ============================================================================
#   Standard include search path
#   ============================================================================

#   ----------------------------------------------------------------------------
#   Header files with global scope
#   ----------------------------------------------------------------------------
STD_KRNL_INC_PATH    := $(CC_SW_INC)$(PROJ_KRNL_INC_GENERIC)
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


#   ----------------------------------------------------------------------------
#   Header files from the target operating system
#   ----------------------------------------------------------------------------
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(OSINC_GENERIC)
STD_KRNL_INC_PATH    += $(CC_SW_INC)$(BUILD_KRNL_INCLUDE)


#   ============================================================================
#   Standard flags for the compiler
#   ============================================================================
#   ============================================================================
#   Take the compiler flags based on the Component memory space.
#   ============================================================================
ifeq ("$(COMP_MEMSPACE)", "USER")
	STD_CC_FLAGS := $(STD_USER_FLAGS)
endif

ifeq ("$(COMP_MEMSPACE)", "KRNL")
ifeq ("$(TI_DSPLINK_GPPOSVERSION)", "2.6.10")
	STD_CC_FLAGS := $(STD_KRNL_FLAGS) -DKBUILD_MODNAME="dsplinkk" -D"KBUILD_BASENAME=$(basename $(basename $@))"
	STD_CC_FLAGS += $(STD_KRNL_INC_PATH)
endif
ifeq ("$(TI_DSPLINK_GPPOSVERSION)", "2.6.18")
	STD_CC_FLAGS := $(STD_KRNL_FLAGS) -D"KBUILD_STR(s)=\#s" -D"KBUILD_MODNAME=KBUILD_STR(dsplinkk)" -D"KBUILD_BASENAME=KBUILD_STR($(basename $(basename $@)))"
	STD_CC_FLAGS += $(STD_KRNL_INC_PATH)
endif
endif


CMD_LINK     := $(LD) $(DRV_LD_FLAGS) $(LD_SW_RELOC)

MOD_FILE_DEB     = $(basename $(basename $(DRIVER_DEB))).mod.c
MOD_FILE_DEBOBJ  = $(basename $(basename $(DRIVER_DEB))).mod.o
MOD_FILE_DEBKNL  = $(basename $(basename $(DRIVER_DEB))).ko

MOD_FILE_REL     = $(basename $(basename $(DRIVER_REL))).mod.c
MOD_FILE_RELOBJ  = $(basename $(basename $(DRIVER_REL))).mod.o
MOD_FILE_RELKNL  = $(basename $(basename $(DRIVER_REL))).ko



ifeq ("$(CMD_MODPOST)", "")
#   ----------------------------------------------------------------------------
#   @rule   drvdeb
#
#   @desc   Builds the target driver module for DEBUG variant.
#   ----------------------------------------------------------------------------

$(target_deb):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(CMD_LINK) $(LD_SW_RELOC) $(LD_SW_OUT)$(DRIVER_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(DRIVER_DEB) $(BUILD_EXPORT_DEB)/.

drvdeb  : $(target_deb)


#   ----------------------------------------------------------------------------
#   @rule   drvrel
#
#   @desc   Builds the target driver module for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_RELOC) $(LD_SW_OUT)$(DRIVER_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(DRIVER_REL) $(BUILD_EXPORT_REL)/.

drvrel  : $(target_rel)
else
#   ----------------------------------------------------------------------------
#   @rule   drvdeb
#
#   @desc   Builds the target driver module for DEBUG variant.
#   ----------------------------------------------------------------------------
drvdeb  : $(target_deb)

$(target_deb):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_RELOC) $(LD_SW_OUT)$(DRIVER_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)

	$(CMDECHO)$(DISPLAY) Post processing kernel module $(basename $(basename $@))...
	$(CMD_MODPOST) $(DRIVER_DEB)
	$(CMDECHO) echo "#include <linux/autoconf.h>" >$$.tmp
	$(CMDECHO) cat $(MOD_FILE_DEB) >> $$.tmp
	$(CMDECHO) cat $$.tmp > $(MOD_FILE_DEB)
	$(CMDECHO) rm -f $$.tmp
	$(CMDECHO)$(COMPILER) $(CC_SW_DEB) $(STD_CC_FLAGS) $(USR_CC_FLAGS) $(STD_CC_DEFNS) \
		$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))                           \
		 $(USR_CC_DEFNS) -DDDSP_DEBUG  -c -o $(MOD_FILE_DEBOBJ) $(MOD_FILE_DEB)
	$(CMDECHO)$(CMD_LINK) $(DRV_LD_FLAGS) $(LD_SW_RELOC) $(LD_SW_OUT)$(MOD_FILE_DEBKNL) $(DRIVER_DEB) $(MOD_FILE_DEBOBJ)
	$(CMDECHO)$(COPY) $(MOD_FILE_DEBKNL) $(BUILD_EXPORT_DEB)/.


#   ----------------------------------------------------------------------------
#   @rule   drvrel
#
#   @desc   Builds the target driver module for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_RELOC) $(LD_SW_OUT)$(DRIVER_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(DISPLAY) Post processing kernel module $(basename $(basename $@))...
	$(CMD_MODPOST) $(DRIVER_REL)
	$(CMDECHO) echo "#include <linux/autoconf.h>" >$$.tmp
	$(CMDECHO) cat $(MOD_FILE_REL) >> $$.tmp
	$(CMDECHO) cat $$.tmp > $(MOD_FILE_REL)
	$(CMDECHO) rm -f $$.tmp
	$(CMDECHO)$(COMPILER) $(CC_SW_REL) $(STD_CC_FLAGS) $(USR_CC_FLAGS) $(STD_CC_DEFNS) \
		$(addprefix $(CC_SW_DEF), $(TI_DSPLINK_GPP_DEFINES))                           \
		 $(USR_CC_DEFNS) -DDDSP_RELUG -c -o $(MOD_FILE_RELOBJ) $(MOD_FILE_REL)
	$(CMDECHO)$(CMD_LINK) $(DRV_LD_FLAGS) $(LD_SW_OUT)$(MOD_FILE_RELKNL) $(DRIVER_REL) $(MOD_FILE_RELOBJ)
	$(CMDECHO)$(COPY) $(MOD_FILE_RELKNL) $(BUILD_EXPORT_REL)/.

post_proc_rel:

drvrel  : $(target_rel) post_proc_rel
endif   # ifeq ("$(CMD_MODPOST)", "DRV")


endif   # ifeq ("$(COMP_TYPE)", "DRV")


#   ############################################################################
#   BUILD AN EXECUTABLE
#   ############################################################################


ifeq ("$(COMP_TYPE)", "EXE")
#   ----------------------------------------------------------------------------
#   Name of the target executable
#   ----------------------------------------------------------------------------
EXECUTABLE_DEB   = $(COMP_BINARY_DEB)/$(basename $(@))
EXECUTABLE_REL   = $(COMP_BINARY_REL)/$(basename $(@))

#   ----------------------------------------------------------------------------
#   Append suitable path to each user defined library
#   ----------------------------------------------------------------------------

ifeq ("$(USR_LIBS)", "")

USR_LIBS_DEB  :=
USR_LIBS_REL  :=

else

DEPENDS_DEB   := $(COMP_OBJECT_DEB)/DEPEND
DEPENDS_REL   := $(COMP_OBJECT_REL)/DEPEND

USR_LIBS_DEB  := $(addprefix $(BUILD_EXPORT_DEB)/, $(USR_LIBS))
USR_LIBS_REL  := $(addprefix $(BUILD_EXPORT_REL)/, $(USR_LIBS))

ARGS_ARCH_DEB := $(COMP_OBJECT_DEB) $(DEPENDS_DEB)
ARGS_ARCH_REL := $(COMP_OBJECT_REL) $(DEPENDS_REL)

endif

CMD_LINK        := $(LINKER) $(EXE_LD_FLAGS) $(STD_LD_FLAGS) $(USR_LD_FLAGS) $(STD_LIB_PATH)

#   ----------------------------------------------------------------------------
#   @rule   exedeb
#
#   @desc   Builds the target executable for DEBUG variant.
#   ----------------------------------------------------------------------------

$(target_deb):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(EXECUTABLE_DEB) $(foreach dir, $(ARGS_ARCH_DEB), $(wildcard $(dir)/*.o)) $(USR_LIBS_DEB)
	$(CMDECHO)$(COPY) $(EXECUTABLE_DEB) $(BUILD_EXPORT_DEB)/.

exedeb  : $(target_deb)


#   ----------------------------------------------------------------------------
#   @rule   exerel
#
#   @desc   Builds the target executable for RELEASE variant.
#   ----------------------------------------------------------------------------

$(target_rel):
	$(CMDECHO)$(DISPLAY) Generating $(basename $@)...
	$(CMDECHO)$(CMD_LINK) $(LD_SW_OUT)$(EXECUTABLE_REL) $(foreach dir, $(ARGS_ARCH_REL), $(wildcard $(dir)/*.o)) $(USR_LIBS_REL)
	$(CMDECHO)$(COPY) $(EXECUTABLE_REL) $(BUILD_EXPORT_REL)/.

exerel  : $(target_rel)

endif
endif   # ifndef LINK_MK
