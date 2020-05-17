# Component and OS selection
IPC = syslink
SHAREDMEM = cmem
DSPOS = sysbios6
GPPOS = linux

# Redefine the ARM compiler tools to use the Angstrom/OE ones
ARM_TOOLCHAIN_PATH          = $(HOME)/ti-sdk/beagleboard-05.02.00.00/linux-devkit
ARM_TOOLCHAIN_PREFIX        = arm-arago-linux-gnueabi-

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = omap3_beagle_defconfig
LINUXKERNEL_INSTALL_DIR     ?= $(HOME)/ti-sdk/beagleboard-05.02.00.00/psp/linux-2.6.39-r102h-arago1
UBOOT_CONFIG                = omap3_beagle_config
UBOOT_INSTALL_DIR           ?= $(HOME)/ti-sdk/beagleboard-05.02.00.00/psp/u-boot-2011.03+2011.06rc2+r1+gitr9623c158f6a5150a21c25026bfba79e7ff7912f5

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0x86300000
DSP_REGION_CMEM_SIZE        ?= 0x01000000
DSP_REGION_CODE_SIZE        ?= 0x00D00000

PLATFORM_CFLAGS             = -DDSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR) -DDSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE) -DDSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)

ifeq ($(IPC),dsplink)
  # Required definitions for DSPLINK/Bios5 builds

  # Required definitions for LPM builds
  LPM_DSP_POWER_SOC         = omap3530

  DSPLINK_PLATFORM          = OMAP3530
  DSPLINK_DSP_PROCESSOR     = OMAP3530
  DSPLINK_GPP_OS            = OMAPLSP
  DSPLINK_DSP_OS            = DSPBIOS5XX
  DSPLINK_CFG_OPTS          =--nodsp=1 --DspTskMode=1
  PLATFORM_CFLAGS           += -DLPM_REQUIRED
else
  # Required definitions for SysLink/SysBios6 builds
  SYSLINK_PLATFORM          = OMAP3530
  SYSLINK_LOADER            = ELF
endif

# Generic Platform Specific compiler flags
PLATFORM_CFLAGS             += -DDSP_HAS_MMU

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv64+ --abi=eabi

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfloat-abi=softfp
