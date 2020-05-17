# --COPYRIGHT--,BSD
#  Copyright (c) $(CPYYEAR), Texas Instruments Incorporated
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
#

ROOT_DIR = .
include Rules.make

#==============================================================================
# The prefix to be added before the GNU compiler tools
#==============================================================================
CSTOOL_PREFIX	?= $(CSTOOL_DIR)/bin/arm-arago-linux-gnueabi-

#==============================================================================
# Set up platform dependent variables.
#==============================================================================
ifeq ($(PLATFORM), omapl138)
LINUXKERNEL_CONFIG=da850_omapl138_defconfig
endif

.PHONY: dsp gpp clean audio_soc_linux audio_soc_linux_clean

#==============================================================================
# The default build target.
#==============================================================================
all: gpp dsp

#==============================================================================
# Clean up the targets built by 'make all'.
#==============================================================================
clean:
	$(MAKE) -C gpp clean
	$(MAKE) -C dsp clean

#==============================================================================
# Target to build GPP and DSP application
#==============================================================================
gpp:
	$(MAKE) -C gpp

dsp:
	$(MAKE) -C dsp

#==============================================================================
# Build the Linux kernel for the Audio SOC example.  
#==============================================================================
FIND_CONFIG = $(shell find $(LINUXKERNEL_INSTALL_DIR) -name '.config')
FIND_UIMAGE = $(shell find $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/ -name 'uImage')

audio_soc_linux:
ifeq ($(FIND_CONFIG), $(LINUXKERNEL_INSTALL_DIR)/.config)
	@echo "Saving existing .config file to .config.org file"
	@mv $(LINUXKERNEL_INSTALL_DIR)/.config $(LINUXKERNEL_INSTALL_DIR)/.config.org
endif
ifeq ($(FIND_UIMAGE), $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage)
	@echo "Saving existing uImage file to uImage.org file"
	@mv $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage.org
endif
	@echo "Configure the kernel to default configuration for $(PLATFORM)"
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(CSTOOL_PREFIX) $(LINUXKERNEL_CONFIG)
	@echo "Removing the Linux Sound Card support (ALSA) support in the kernel"
	@cat $(LINUXKERNEL_INSTALL_DIR)/.config | sed -e 's/CONFIG_SOUND=y/# CONFIG_SOUND is not set/' > $(LINUXKERNEL_INSTALL_DIR)/.config_audioSoc
	@cp -f $(LINUXKERNEL_INSTALL_DIR)/.config_audioSoc $(LINUXKERNEL_INSTALL_DIR)/.config
	@echo "Re-building kernel uImage for $(PLATFORM)"
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(CSTOOL_PREFIX) uImage
	@mv $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage_audioSoc
	@rm $(LINUXKERNEL_INSTALL_DIR)/.config
	@echo
	@echo "Your kernel image for the audioSOC application can be found at $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage_audioSoc"

audio_soc_linux_clean:
	@rm -f $(LINUXKERNEL_INSTALL_DIR)/.config_audioSoc
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(MVTOOL_PREFIX) distclean


