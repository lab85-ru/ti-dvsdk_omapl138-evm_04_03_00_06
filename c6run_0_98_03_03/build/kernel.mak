#############################################################################
# Makefile                                                                  #
#                                                                           #
# Builds the Linux kernel modules for DSPLink and CMEM (and dependencies)   #
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


#   ----------------------------------------------------------------------------
#   Include the top-level Rules.mak file
#   ----------------------------------------------------------------------------
include ../config.mak
include ../Rules.mak


#   ----------------------------------------------------------------------------
#   Platform directory definition
#   ----------------------------------------------------------------------------
PLATFORMDIR := $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)


#   ----------------------------------------------------------------------------
#   DSPLink directory definition
#   ----------------------------------------------------------------------------
DSPLINK := $(LINK_INSTALL_DIR)/dsplink


#   ----------------------------------------------------------------------------
#   Include LPM target if needed
#   ----------------------------------------------------------------------------
ifneq ($(LPM_DSP_POWER_SOC),)
  LPM_TARGET=lpm_ko
endif

#   ----------------------------------------------------------------------------
#   Compiler and Linker procedure
#   From this point and on changes are very unlikely.
#   ----------------------------------------------------------------------------
.PHONY:	all clean uboot linux 

# The default build target.
all: linux


#   ----------------------------------------------------------------------------
#   Build the uboot.
#   ----------------------------------------------------------------------------
uboot : $(PLATFORMDIR)/u-boot.bin

$(PLATFORMDIR)/u-boot.bin : $(UBOOT_INSTALL_DIR)/u-boot.bin
	cp $(UBOOT_INSTALL_DIR)/u-boot.bin $(PLATFORMDIR)/u-boot.bin
  
$(UBOOT_INSTALL_DIR)/u-boot.bin :
	$(MAKE) -C $(UBOOT_INSTALL_DIR) CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) $(UBOOT_CONFIG)
	$(MAKE) -C $(UBOOT_INSTALL_DIR) CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX)
  

#   ----------------------------------------------------------------------------
#   Build the Linux kernel. Also, an explicit cleanup target is defined.
#   ----------------------------------------------------------------------------
linux : $(PLATFORMDIR)/uImage

$(PLATFORMDIR)/uImage : $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage
	cp $(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage $(PLATFORMDIR)/

$(LINUXKERNEL_INSTALL_DIR)/arch/arm/boot/uImage : uboot
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) $(LINUXKERNEL_CONFIG)
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) PATH=$(UBOOT_INSTALL_DIR)/tools:$(PATH) uImage
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) modules  

linux_install_modules : linux
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) ARCH=arm CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) INSTALL_MOD_PATH=$(INSTALL_MOD_PATH) modules_install

#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean uboot_clean linux_clean
clean: uboot_clean linux_clean
  
uboot_clean:
	rm -Rf $(PLATFORMDIR)/u-boot.bin
	$(MAKE) -C $(UBOOT_INSTALL_DIR) CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) distclean

linux_clean :
	rm -Rf $(PLATFORMDIR)/uImage
	$(MAKE) -C $(LINUXKERNEL_INSTALL_DIR) CROSS_COMPILE=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) clean
