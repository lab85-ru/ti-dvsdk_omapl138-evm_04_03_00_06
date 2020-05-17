#############################################################################
# Makefile                                                                  #
#                                                                           #
# Top-level make for C6Run project                                          #
#############################################################################
#
#
#############################################################################
#                                                                           #
#   Copyright (C) 2010 Texas Instruments Incorporated                       #
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

PLATFORMS=$(shell ls platforms)
C6RUN_DIR=$(shell pwd)

CONFIG_MAK_FILE=config.mak

# Conditional include of config makefile (gets currently configured platform)
-include $(C6RUN_DIR)/$(CONFIG_MAK_FILE)

# Required include of Rules.mak file
include $(C6RUN_DIR)/Rules.mak

# if the SDK_PATH variable is empty, assume the current working directory
ifeq ($(strip $(SDK_PATH)),)
SDK_PATH:=$(C6RUN_DIR)
endif

# Required settings for get_components target
LOCAL_SAVE_PATH=./downloads
WGET_FLAGS=--directory-prefix=$(LOCAL_SAVE_PATH)

# Define default values for install target
prefix ?= /usr/local
exec_prefix ?= $(prefix)
bindir ?= $(exec_prefix)/bin
sbindir ?= $(exec_prefix)/sbin
libexecdir ?= $(exec_prefix)/libexec
datarootdir ?= $(prefix)/share
includedir ?= $(prefix)/include
docdir ?= $(datarootdir)/doc/c6run
infodir ?= $(datarootdir)/info

libdir ?=  $(exec_prefix)/lib


#
ifdef DSP_REGION_BASE_ADDR
DSP_REGION_END_ADDR=$(shell perl -w -e 'printf "0x%08X", $(DSP_REGION_BASE_ADDR)+$(DSP_REGION_CMEM_SIZE)+$(DSP_REGION_CODE_SIZE);')
endif
#----------------------------------------------------------------------------------------------
# Build up the environment shell script, so we can import variables into shell script
#----------------------------------------------------------------------------------------------
ENV_SH_FILE=$(C6RUN_DIR)/c6run-environment.sh


.PHONY: all build get_components remove_components everything oe_build config create_c6run_env_script setup_dsp_memory_map dsp_libs gpp_libs modules tests examples linux uboot info check
all: oe_build

.PHONY: help
help:
		@echo "C6Run Make System Help"
		@echo "-------------------------"
		@echo
		@echo "make help"
		@echo "  See this message."
		@echo "make get_components"
		@echo "  Used to obtain and install required build components when using this tool"
		@echo "  in a stand alone development system (no SDK or OE)."
		@echo    
		@echo "make <platform>_config"
		@echo "  Used to configure the software for a particular platform."
		@echo "  Available platforms:"
		@for p in $(PLATFORMS); do \
    echo  "    $$p"; \
    done
		@echo
		@echo "make all (or just simply make)" 
		@echo "  Following configuration, use this to build back-end libraries."
		@echo "make everything" 
		@echo "  Following configuration, use this to build back-end libraries and the"
		@echo "  dependent components. For kernel modules to build correctly, the"
		@echo "  LINUXKERNEL_INSTALL_DIR variable must be set in platform.mak to point"
		@echo "  to a built kernel tree."
		@echo "make dsp_libs"
		@echo "  Optional make target to build only the DSP-side C6Run backend libraries."
		@echo "  'make all' will also perform this step"
		@echo "make gpp_libs"
		@echo "  Optional make target to build only the GPP-side C6Run backend libraries."
		@echo "  'make all' will also perform this step"
		@echo "make modules"
		@echo "  Optional make target to build the kernel modules for target platform"
		@echo "  The kernel must be built before building kernel modules."
		@echo "  'make everything' will also perform this step"
		@echo 
		@echo "make clean"
		@echo "  Following configuration, use this to clean build of back-end libraries."
		@echo "make distclean"
		@echo "  Use this to remove configuration and built components for current platform."
		@echo "make clobber"
		@echo "  Use this to clean C6Run back-end libraries, the dependent components and the"
		@echo "  current configuration."
		@echo 
		@echo "make DESTDIR=<install path> install"
		@echo "  Install the C6Run build tool (preconfigured for the current platform)"
		@echo "  into the specified DESTDIR path."
		@echo    
		@echo "make linux"
		@echo "  Optional make target to build the Linux kernel for the configured platform."
		@echo "  The kernel must be built before running 'make build' for kernel modules"
		@echo "  to be generated."
		@echo
		@echo "make examples"
		@echo "  Optional make target to build the example programs included in the C6Run"
		@echo "  package."
		@echo "make tests"
		@echo "  Optional make target to build the test programs included in the C6Run"
		@echo "  package."
		@echo "make INSTALL_DIR=<install path> install_programs"
		@echo "  Install the C6Run example and test application for into the"
		@echo "  specified INSTALL_DIR path."
		@echo 
		@echo "NOTE: You must at least run 'make <platform>_config' and 'make build' to"
		@echo "use the C6Run tools."

# Target to get and save the needed components for building
get_components: remove_components
    # Make a local directory to save the downloads
		@mkdir -m 777 -p $(LOCAL_SAVE_PATH)

    # Get and untar LPM to local path
		@if [ ! -e $(LOCAL_SAVE_PATH)/local_power_manager_linux_$(LPM_VERSION).tar.gz ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/lpm/$(LPM_VERSION)/exports/local_power_manager_linux_$(LPM_VERSION).tar.gz; \
    fi
		@echo -n "Unzipping local_power_manager_linux_${LPM_VERSION}..."
		@tar -C $(SDK_PATH) -xzf $(LOCAL_SAVE_PATH)/local_power_manager_linux_${LPM_VERSION}.tar.gz
		@echo "done!"

    # Get and untar DSPLink 1.65 to local path
		@if [ ! -e $(LOCAL_SAVE_PATH)/dsplink_linux_$(DSPLINK_VERSION).tar.gz ]; then \
      VERSION_WITH_PERIODS=`echo $(DSPLINK_VERSION) | tr _ .`; \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/DSPLink/1_65/$(DSPLINK_VERSION)/$$VERSION_WITH_PERIODS/dsplink_linux_$(DSPLINK_VERSION).tar.gz; \
    fi
		@echo -n "Unzipping dsplink_linux_$(DSPLINK_VERSION)..."
		@tar -C $(SDK_PATH) -xzf $(LOCAL_SAVE_PATH)/dsplink_linux_$(DSPLINK_VERSION).tar.gz
		@echo "done!"

    # Get and untar the linuxutils package to local path
		@if [ ! -e $(LOCAL_SAVE_PATH)/linuxutils_$(LINUXUTILS_VERSION).tar.gz ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/linuxutils/$(LINUXUTILS_VERSION)/exports/linuxutils_$(LINUXUTILS_VERSION).tar.gz; \
    fi
		@echo -n "Unzipping linuxutils_$(LINUXUTILS_VERSION)..."
		@tar -C $(SDK_PATH) -xzf $(LOCAL_SAVE_PATH)/linuxutils_$(LINUXUTILS_VERSION).tar.gz
		@echo "done!"

    # Get and install DSP/BIOS 5.4x
		@if [ ! -e $(LOCAL_SAVE_PATH)/bios_setuplinux_$(BIOS_VERSION).bin ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/bios/dspbios/$(BIOS_VERSION)/exports/bios_setuplinux_$(BIOS_VERSION).bin; \
    fi
		@echo -n "Installing bios_$(BIOS_VERSION)..."
		@chmod +x $(LOCAL_SAVE_PATH)/bios_setuplinux_$(BIOS_VERSION).bin
		@$(LOCAL_SAVE_PATH)/bios_setuplinux_$(BIOS_VERSION).bin --mode silent --prefix $(SDK_PATH)
		@echo "done!"

    # Get and install IPC 1.2x
		@if [ ! -e $(LOCAL_SAVE_PATH)/ipc_setuplinux_$(IPC_VERSION).bin ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/ipc/$(IPC_VERSION)/exports/ipc_setuplinux_$(IPC_VERSION).bin; \
    fi
		@echo -n "Installing ipc_$(IPC_VERSION)..."
		@chmod +x $(LOCAL_SAVE_PATH)/ipc_setuplinux_$(IPC_VERSION).bin
		@$(LOCAL_SAVE_PATH)/ipc_setuplinux_$(IPC_VERSION).bin --mode silent --prefix $(SDK_PATH)
		@echo "done!"
    
    # Get and install SysBios 6.3x
		@if [ ! -e $(LOCAL_SAVE_PATH)/bios_setuplinux_$(SYSBIOS_VERSION).bin ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/bios/sysbios/$(SYSBIOS_VERSION)/exports/bios_setuplinux_$(SYSBIOS_VERSION).bin; \
    fi
		@echo -n "Installing bios_$(SYSBIOS_VERSION)..."
		@chmod +x $(LOCAL_SAVE_PATH)/bios_setuplinux_$(SYSBIOS_VERSION).bin
		@$(LOCAL_SAVE_PATH)/bios_setuplinux_$(SYSBIOS_VERSION).bin --mode silent --prefix $(SDK_PATH)
		@echo "done!"
    
    # Get and untar the SysLink package
		@if [ ! -e $(LOCAL_SAVE_PATH)/syslink_$(SYSLINK_VERSION).tar.gz ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/syslink/$(SYSLINK_VERSION)/exports/syslink_$(SYSLINK_VERSION).tar.gz; \
    fi
		@echo -n "Unzipping syslink_$(SYSLINK_VERSION)..."
		@tar -C $(SDK_PATH) -xzf $(LOCAL_SAVE_PATH)/syslink_$(SYSLINK_VERSION).tar.gz
		@echo "done!"
    
    # Get and install XDCTools (needed by SysBios or DSP/Bios)
		@if [ ! -e $(LOCAL_SAVE_PATH)/xdctools_setuplinux_$(XDCTOOLS_VERSION).bin ]; then \
      wget $(WGET_FLAGS) http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/rtsc/$(XDCTOOLS_VERSION)/exports/xdctools_setuplinux_$(XDCTOOLS_VERSION).bin; \
    fi
		@echo -n "Installing xdctools_$(XDCTOOLS_VERSION)..."
		@chmod +x $(LOCAL_SAVE_PATH)/xdctools_setuplinux_$(XDCTOOLS_VERSION).bin
		@$(LOCAL_SAVE_PATH)/xdctools_setuplinux_$(XDCTOOLS_VERSION).bin --mode silent --prefix $(SDK_PATH)
		@echo "done!"

remove_components:
		@rm -Rf $(SDK_PATH)/local_power_manager_linux_$(LPM_VERSION)
		@rm -Rf $(SDK_PATH)/dsplink_linux_$(DSPLINK_VERSION)
		@rm -Rf $(SDK_PATH)/linuxutils_$(LINUXUTILS_VERSION)
		@rm -Rf $(SDK_PATH)/bios_$(BIOS_VERSION)
		@rm -Rf $(SDK_PATH)/ipc_$(IPC_VERSION)
		@rm -Rf $(SDK_PATH)/bios_$(SYSBIOS_VERSION)
		@rm -Rf $(SDK_PATH)/syslink_$(SYSLINK_VERSION)
		@rm -Rf $(SDK_PATH)/xdctools_$(XDCTOOLS_VERSION)
    

# Target for listing information about the dependent components.
info:	check
		@if [ -e $(CONFIG_MAK_FILE) ]; then \
      echo "Listing information about the DVSDK components:"; \
      if [ ! -e $(XDC_INSTALL_DIR) ]; then \
        echo "  ERROR: XDC tools not found in $(XDC_INSTALL_DIR), check Rules.mak."; \
        exit 1; \
      fi; \
      CHECKLIST="$(BIOS_INSTALL_DIR)/packages $(SYSBIOS_INSTALL_DIR)/packages $(CMEM_INSTALL_DIR)/packages $(LPM_INSTALL_DIR)/packages"; \
      for x in $${CHECKLIST}; do \
        echo "*******************************************************************************"; \
        echo "  Listing package information for $$x."; \
        echo ""; \
        XDC_PATH="$(XDC_INSTALL_DIR)" $(XDC_INSTALL_DIR)/xs xdc.tools.path -Pr $$x; \
      done; \
      echo "*******************************************************************************"; \
      if [ -e $(LINUXKERNEL_INSTALL_DIR) ]; then \
        echo "  Using Linux kernel from: $(LINUXKERNEL_INSTALL_DIR)"; \
      else \
        echo "  WARNING: Linux kernel not found at $(LINUXKERNEL_INSTALL_DIR)"; \
      fi; \
      echo ""; \
      echo "  *******************************************************************************"; \
      if [ -f $(CODEGEN_INSTALL_DIR)/bin/cl6x ]; then \
        echo "  TI C6000 codegen:"; \
        echo "    $(CODEGEN_INSTALL_DIR)/bin/cl6x"; \
        echo ""; \
        $(CODEGEN_INSTALL_DIR)/bin/cl6x -version; \
      else \
        echo "  WARNING: TI C6000 codegen not found!"; \
      fi; \
      echo ""; \
      echo "*******************************************************************************"; \
      if [ -f $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc ]; then \
        echo "  ARM GCC compiler:"; \
        echo "    $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc"; \
        echo ""; \
        $(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)gcc --version; \
      else \
        echo "  WARNING: ARM gcc tool chain not found!"; \
      fi; \
      echo ""; \
    else \
      echo "ERROR: No platform configured!."; \
      echo ""; \
      $(MAKE) -s --no-print-directory help; \
    fi;

# Target for checking that the Rules.mak file is set up properly.
check:
		@echo "Checking that dependent components in Rules.mak are found:"
		@CHECKLIST="$(BIOS_INSTALL_DIR) $(SYSBIOS_INSTALL_DIR) $(XDC_INSTALL_DIR) $(LINK_INSTALL_DIR) $(CMEM_INSTALL_DIR) $(LPM_INSTALL_DIR)"; \
    MISSING_CNT=0; \
    for x in $${CHECKLIST}; do \
      if [ ! -e $$x ]; then \
        echo "  WARNING: $$x doesn't exist"; \
        let MISSING_CNT=$$MISSING_CNT+1; \
      else \
        echo "  FOUND: $$x"; \
      fi \
    done; \
    if [ $${MISSING_CNT} -gt 0 ]; then \
      echo "  Your configuration has errors. Check the paths in Rules.mak"; \
      echo "  and then re-run the configuration steps."; \
      exit 1; \
    else \
      echo "  Done checking! All components were found."; \
    fi; \
    echo "";

# Building targets
everything: build
build:
		@if [ -e $(CONFIG_MAK_FILE) ]; then \
      echo "Building C6Run backend libraries."; \
      $(MAKE) -C $(C6RUN_DIR)/build all; \
      echo "Building C6Run backend libraries is now complete for the selected platform."; \
    else \
      echo "ERROR: No platform configured!."; \
      echo ""; \
      $(MAKE) -s --no-print-directory help; \
		fi

oe_build:
	@if [ -e $(CONFIG_MAK_FILE) ]; then \
    echo "Building C6Run backend libraries."; \
    $(MAKE) -C $(C6RUN_DIR)/build dsp gpp; \
    echo "Building C6Run backend libraries is now complete for the selected platform."; \
  else \
    echo "ERROR: No platform configured!."; \
    echo ""; \
    $(MAKE) -s --no-print-directory help; \
	fi

# Config targets
%_config:
		@if [ -e $(CONFIG_MAK_FILE) ]; then \
      echo -n "Cleaning old configuration before starting new one..."; \
      $(MAKE) -s --no-print-directory distclean >> /dev/null; \
      echo "done!"; \
    fi
		@$(MAKE) -s PLATFORM=$(@:%_config=%) config

config:
		@echo "Before performing this configuration step, you should have done the following:"    
		@echo "  Edited Rules.mak to set the correct location of dependencies."
		@echo "  Edited platforms/$(PLATFORM)/platform.mak to point to the correct location for the Linux kernel, etc."    
		@echo "Performing configuration steps for the $(PLATFORM) platform."
		# Check if this is a valid platform to configure
		@if [ ! -e platforms/$(PLATFORM)/platform.mak ]; then \
      echo "ERROR: $(PLATFORM) does not seem to be a valid platform!"; \
      exit 1; \
    fi
    # Create config.mak file
		@rm -Rf $(CONFIG_MAK_FILE)
		@echo "PLATFORM=$(PLATFORM)" > $(CONFIG_MAK_FILE)
		@echo "C6RUN_INSTALL_DIR=$(C6RUN_DIR)" >> $(CONFIG_MAK_FILE)
    # Create environment.sh based on contents in Rules.mak
		$(MAKE) -s --no-print-directory create_c6run_env_script
    # Create environment.sh based on contents in Rules.mak
		#$(MAKE) -s --no-print-directory setup_dsp_memory_map
		$(MAKE) setup_dsp_memory_map
ifeq ($(IPC),dsplink)
    # Do the Link config if it is needed (on OE platforms, it should
    # not be needed since Link config happened in a dependent recipe)
		#echo "hello"
		#$(MAKE) -s --no-print-directory -C $(C6RUN_DIR)/build -f dsplink.mak config
endif    
    # Display platform specific release notes
		@if [ -e platforms/$(PLATFORM)/notes.txt ]; then \
      echo; echo; \
      echo "IMPORTANT! Platform-specific release notes follow. Please read carefully."; \
      echo "-------------------------------------------------------------------------"; \
      echo; \
      cat platforms/$(PLATFORM)/notes.txt; \
		fi
    # Make cross-compiler/archiver scripts executable
		@chmod +x bin/c6runapp-cc
		@chmod +x bin/c6runlib-ar
		@chmod +x bin/c6runlib-cc
    
create_c6run_env_script:
		@echo "#!/bin/bash" > $(ENV_SH_FILE)
		@echo "# $(ENV_SH_FILE) environment script" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{CODEGEN_INSTALL_DIR='$(CODEGEN_INSTALL_DIR)'}" >> $(ENV_SH_FILE)
		@echo "export CODEGEN_INSTALL_DIR" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{C6RUN_TOOLCHAIN_PATH='$(C6RUN_INSTALL_DIR)'}" >> $(ENV_SH_FILE)
		@echo "export C6RUN_TOOLCHAIN_PATH" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{DSP_CFLAGS='$(DSP_CFLAGS)'}" >> $(ENV_SH_FILE)
		@echo "export DSP_CFLAGS" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{ARM_TOOLCHAIN_PATH='$(ARM_TOOLCHAIN_PATH)'}" >> $(ENV_SH_FILE)
		@echo "export ARM_TOOLCHAIN_PATH" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{ARM_TOOLCHAIN_PREFIX='$(ARM_TOOLCHAIN_PREFIX)'}" >> $(ENV_SH_FILE)
		@echo "export ARM_TOOLCHAIN_PREFIX" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{CFLAGS='$(CFLAGS)'}" >> $(ENV_SH_FILE)
		@echo "export CFLAGS" >> $(ENV_SH_FILE)
		@echo -n ": $$" >> $(ENV_SH_FILE)
		@echo "{PLATFORM_CFLAGS='$(PLATFORM_CFLAGS)'}" >> $(ENV_SH_FILE)
		@echo "export PLATFORM_CFLAGS" >> $(ENV_SH_FILE)
		@chmod +x $(ENV_SH_FILE)

setup_dsp_memory_map:
    # Update configuration makefile with memory map (so build step will have the correct ones)
ifdef IPC
		@echo "IPC=$(IPC)" >> $(CONFIG_MAK_FILE)
endif
ifdef DSPOS
		@echo "DSPOS=$(DSPOS)" >> $(CONFIG_MAK_FILE)
endif
ifdef GPPOS
		@echo "GPPOS=$(GPPOS)" >> $(CONFIG_MAK_FILE)
endif
ifdef SHAREDMEM
		@echo "SHAREDMEM=$(SHAREDMEM)" >> $(CONFIG_MAK_FILE)
endif
		@echo "DSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR)" >> $(CONFIG_MAK_FILE)
		@echo "DSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE)" >> $(CONFIG_MAK_FILE)
		@echo "DSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)" >> $(CONFIG_MAK_FILE)
    # Modify the platform's loadmodules.sh and platform.tci
		C6RUN_INSTALL_DIR=$(C6RUN_DIR) \
      PLATFORM=$(PLATFORM) \
      perl $(C6RUN_DIR)/build/perl/setup_loadmodules_scripts.pl $(IPC) $(DSP_REGION_BASE_ADDR) $(DSP_REGION_CMEM_SIZE) $(DSP_REGION_CODE_SIZE);
ifeq ($(IPC),dsplink)
		C6RUN_INSTALL_DIR=$(C6RUN_DIR) \
      PLATFORM=$(PLATFORM) \
      perl $(C6RUN_DIR)/build/perl/setup_bios5_tcf.pl $(DSP_REGION_BASE_ADDR) $(DSP_REGION_CMEM_SIZE) $(DSP_REGION_CODE_SIZE);
else
		C6RUN_INSTALL_DIR=$(C6RUN_DIR) \
      PLATFORM=$(PLATFORM) \
      perl $(C6RUN_DIR)/build/perl/setup_bios6_bld.pl $(DSP_REGION_BASE_ADDR) $(DSP_REGION_CMEM_SIZE) $(DSP_REGION_CODE_SIZE);
endif


linux:
		@$(MAKE) -C $(C6RUN_DIR)/build -f kernel.mak linux_clean linux

uboot:
		@$(MAKE) -C $(C6RUN_DIR)/build -f kernel.mak uboot_clean uboot

dsp_libs:
		@$(MAKE) -C $(C6RUN_DIR)/build dsp_clean dsp

gpp_libs:
		@$(MAKE) -C $(C6RUN_DIR)/build gpp_clean gpp
    
modules:
		@$(MAKE) -C $(C6RUN_DIR)/build modules_clean modules
    
tests:
		@ARM_TOOLCHAIN_PATH=$(ARM_TOOLCHAIN_PATH) \
    ARM_TOOLCHAIN_PREFIX=$(ARM_TOOLCHAIN_PREFIX) \
    C6RUN_TOOLCHAIN_PATH=$(C6RUN_TOOLCHAIN_PATH) \
    $(MAKE) -C $(C6RUN_DIR)/test clean all

examples:
		@ARM_TOOLCHAIN_PATH=$(ARM_TOOLCHAIN_PATH) \
    ARM_TOOLCHAIN_PREFIX=$(ARM_TOOLCHAIN_PREFIX) \
    C6RUN_TOOLCHAIN_PATH=$(C6RUN_TOOLCHAIN_PATH) \
    $(MAKE) -C $(C6RUN_DIR)/examples clean all


.PHONY : install install_programs
install:
		@if [ ! -e "$(DESTDIR)" ]; then \
      mkdir -p $(DESTDIR); \
    fi
		@cp -pPrf bin/*.*  $(DESTDIR)/$(bindir)
		@cp -pPrf include/*.*  $(DESTDIR)/$(includedir)
		@cp -pPrf lib/*.* $(DESTDIR)/$(libdir)
		@install -m 755 $(ENV_SH_FILE) $(DESTDIR)/

install_programs:
ifdef INSTALL_DIR
		@if [ ! -e "$(INSTALL_DIR)" ]; then \
      mkdir -p $(INSTALL_DIR); \
    fi
    # Install kernel modules (if we built them) and the module load/unload scripts   
		@$(MAKE) -s --no-print-directory -C build -f cmem.mak CMEM_INSTALL_DIR=$(CMEM_INSTALL_DIR) INSTALL_DIR=$(INSTALL_DIR) install_kernel_mod
ifeq ($(IPC),dsplink)    
		@$(MAKE) -s --no-print-directory -C build -f dsplink.mak LINK_INSTALL_DIR=$(LINK_INSTALL_DIR) INSTALL_DIR=$(INSTALL_DIR) install_kernel_mod
ifdef LPM_DSP_POWER_SOC
		@$(MAKE) -s --no-print-directory -C build -f lpm.mak LPM_INSTALL_DIR=$(LPM_INSTALL_DIR) INSTALL_DIR=$(INSTALL_DIR) install_kernel_mod
endif
else
		@$(MAKE) -s --no-print-directory -C build -f syslink.mak SYSLINK_INSTALL_DIR=$(SYSLINK_INSTALL_DIR) INSTALL_DIR=$(INSTALL_DIR) install_kernel_mod
endif
		@install -m 755 platforms/$(PLATFORM)/loadmodules.sh $(INSTALL_DIR)/
		@install -m 755 platforms/$(PLATFORM)/unloadmodules.sh $(INSTALL_DIR)/
    # Install the examples and tests, mirroring the source directory structure
		@list1="examples/c6runapp examples/c6runlib test/c6runapp test/c6runlib"; \
    for d in $${list1}; do \
      list2=`ls -1 $${d}`; \
      for i in $${list2}; do \
        if [ -d "$${d}/$${i}" ]; then \
          if [ -e "$${d}/$${i}/Makefile" ]; then \
            install -d $(INSTALL_DIR)/$${d}/$${i}; \
            FILECNT=`ls $${d}/$${i}/*_arm 2> /dev/null | wc -l`; \
            if [ "$${FILECNT}" != "0" ]; then \
              FILES="$${d}/$${i}/*_arm"; \
              for f in $${FILES}; do \
                cp -pPrf $${f} $(INSTALL_DIR)/$${d}/$${i}; \
              done \
            fi; \
            FILECNT=`ls $${d}/$${i}/*_dsp 2> /dev/null | wc -l`; \
            if [ "$${FILECNT}" != "0" ]; then \
              FILES="$${d}/$${i}/*_dsp"; \
              for f in $${FILES}; do \
                cp -pPrf $${f} $(INSTALL_DIR)/$${d}/$${i}; \
              done \
            fi \
          fi \
        fi \
      done \
    done
else
		@echo "You did not specify a valid INSTALL_DIR variable when running 'make install_programs'."
endif
    
# Clean targets
.PHONY: clean distclean clobber
clean:
		@if [ -e $(CONFIG_MAK_FILE) ]; then \
      $(MAKE) -C build gpp_clean dsp_clean; \
    else \
      echo "No platform configured! Nothing cleaned."; \
		fi
    
distclean: clean 
		@rm -Rf $(CONFIG_MAK_FILE)
		@rm -Rf $(ENV_SH_FILE)
    
clobber:
		@if [ -e $(CONFIG_MAK_FILE) ]; then \
      $(MAKE) -C build gpp_clean dsp_clean; \
    fi
		@$(MAKE) -C build -f dsplink.mak clean
		@$(MAKE) -C build -f cmem.mak clean
ifdef LPM_DSP_POWER_SOC
		@$(MAKE) -C build -f lpm.mak clean
endif
		@rm -Rf $(CONFIG_MAK_FILE)
		@rm -Rf $(ENV_SH_FILE)

