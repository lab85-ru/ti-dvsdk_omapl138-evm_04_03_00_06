#############################################################################
# cmem.mak                                                                  #
#                                                                           #
# Targets to build cmem components from the LinuxUtils package              #
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
#   CMEM directory definition
#   ----------------------------------------------------------------------------
MOD_PATH := $(CMEM_INSTALL_DIR)/packages/ti/sdo/linuxutils/cmem/src/module
MOD_NAME := cmemk.ko


#   ----------------------------------------------------------------------------
#   Compiler and Linker procedure
#   ----------------------------------------------------------------------------
.PHONY:	all kernel_mod install_kernel_mod

# The default build target.
all: kernel_mod

#   ----------------------------------------------------------------------------
#   Build the DSPLink kernel module for the configured platform, and make sure the
#   kernel_binaries directory is kept in sync. 
#   ----------------------------------------------------------------------------
kernel_mod:
	@echo -n "Building CMEM kernel module..."
	@$(MAKE) -C $(MOD_PATH) \
    MVTOOL_PREFIX=$(ARM_TOOLCHAIN_PATH)/bin/$(ARM_TOOLCHAIN_PREFIX) \
    LINUXKERNEL_INSTALL_DIR=$(LINUXKERNEL_INSTALL_DIR) \
    EXEC_DIR=$(PLATFORMDIR) >> /dev/null
	@echo "complete!"

install_kernel_mod:
ifdef INSTALL_DIR
	@if [ -e $(MOD_PATH)/$(MOD_NAME) ]; then \
    echo "CMEM kernel module found. Copying to install directory..."; \
    install -m 755 $(MOD_PATH)/$(MOD_NAME) $(INSTALL_DIR)/; \
  fi
else
	@echo "You did not specify a valid INSTALL_DIR variable when running 'make install_kernel_mod'."
endif

#   ----------------------------------------------------------------------------
#   Clean Rule(s)
#   ----------------------------------------------------------------------------
.PHONY: clean kernel_mod_clean
clean: kernel_mod_clean

kernel_mod_clean:
	@echo -n "Cleaning up CMEM kernel module..."
	@rm -Rf $(PLATFORMDIR)/$(MOD_NAME)
	@$(MAKE) -C $(MOD_PATH) clean >> /dev/null
	@if [ -e $(MOD_PATH)/$(MOD_NAME) ]; then \
    rm -Rf $(MOD_PATH)/$(MOD_NAME); \
  fi
	@echo "complete!"
