#------------------------------------------------------------------------------
# Version number for default dependent components
#   1) If using the 'make get_components' command, edit these variables BEFORE 
#      doing the command.
#   2) If trying to use C6Run in a TI SDK environment, edit these variables to
#      match the SDK installed versions.
#------------------------------------------------------------------------------

# Components used for DSPLink & Bios5 based platforms
DSPLINK_VERSION=1_65_00_03
LPM_VERSION=1_24_02_09
BIOS_VERSION=5_41_10_36

# Components used for SysLink & Bios6 based platforms
SYSLINK_VERSION=2_00_02_80
SYSBIOS_VERSION=6_32_01_38
IPC_VERSION=1_23_03_31

# Common components used for all platforms
XDCTOOLS_VERSION=3_22_01_21
LINUXUTILS_VERSION=3_21_00_04


#-----------------------------------------------------------------------------------------------
# "SDK" path
#   1) If using the make get_components command, point it to the where you want 
#      all the components to be installed/unpacked.
#      
#   2) If trying to use C6Run in a TI SDK environment, then point this variable 
#      to the top-level SDK directory, which should contain the directories of 
#      of the dependent components.
#
#   3) In Angstrom SDK install, the SDK_PATH variable should come from the sourced
#      environment-setup.  Make sure it is sourced before configuring/building 
#      the C6Run components.
#-----------------------------------------------------------------------------------------------
SDK_PATH                ?= $(C6RUN_INSTALL_DIR)


#------------------------------------------------------------------------------------------------
# Global path setup for DSP and ARM compiler tools
#------------------------------------------------------------------------------------------------
CODEGEN_INSTALL_DIR     ?= $(HOME)/toolchains/TI_CGT_C6000_7.3.0
ARM_TOOLCHAIN_PATH      ?= $(HOME)/toolchains/arm-2009q1
ARM_TOOLCHAIN_PREFIX    ?= arm-none-linux-gnueabi-


#------------------------------------------------------------------------------------------------
# Global path setup for dependent components
#   Generally you should not need to modify these, and instead should modify the
#   SDK_PATH variable above.
#------------------------------------------------------------------------------------------------

# Components used for DSPLink & Bios5 based platforms
LINK_INSTALL_DIR        ?= $(SDK_PATH)/dsplink_linux_$(DSPLINK_VERSION)
LPM_INSTALL_DIR         ?= $(SDK_PATH)/local_power_manager_linux_$(LPM_VERSION)
BIOS_INSTALL_DIR        ?= $(SDK_PATH)/bios_$(BIOS_VERSION)

# Components used for SysLink & Bios6 based platforms
SYSLINK_INSTALL_DIR     ?= $(SDK_PATH)/syslink_$(SYSLINK_VERSION)
SYSBIOS_INSTALL_DIR     ?= $(SDK_PATH)/bios_$(SYSBIOS_VERSION)
IPC_INSTALL_DIR         ?= $(SDK_PATH)/ipc_$(IPC_VERSION)

# Common components used for all platforms
XDC_INSTALL_DIR         ?= $(SDK_PATH)/xdctools_$(XDCTOOLS_VERSION)
CMEM_INSTALL_DIR        ?= $(SDK_PATH)/linuxutils_$(LINUXUTILS_VERSION)



#------------------------------------------------------------------------------------------------
# Default IPC is DSPLINK (could be SYSLINK)
#------------------------------------------------------------------------------------------------
IPC                     ?= dsplink


#------------------------------------------------------------------------------------------------
# Get Platform specific paths and variables (if PLATFORM is defined)
#------------------------------------------------------------------------------------------------
ifdef PLATFORM
-include $(C6RUN_INSTALL_DIR)/platforms/$(PLATFORM)/platform.mak
endif


#------------------------------------------------------------------------------------------------
# Check to specify the default directory separator for path construction
#------------------------------------------------------------------------------------------------
ifneq ("$(COMSPEC)", "")
  ifeq ($(shell echo $${OSTYPE}), cygwin)
    DIRSEP ?=/
  else
    DIRSEP ?=\\
  endif
else
  ifeq ($(shell uname),Linux)
    DIRSEP ?= /
  endif
endif

