# Component and OS selection
IPC = dsplink
SHAREDMEM = cmem
DSPOS = dspbios5
GPPOS = linux

# Redefine the ARM compiler tools to use the Angstrom/OE ones
ARM_TOOLCHAIN_PATH          = $(SDK_PATH)
ARM_TOOLCHAIN_PREFIX        = $(TARGET_SYS)-

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = omap3_beagle_defconfig
LINUXKERNEL_INSTALL_DIR     ?= $(HOME)/angstrom-setup-scripts/build/tmp-angstrom_2008_1/sysroots/beagleboard-angstrom-linux-gnueabi/kernel
UBOOT_CONFIG                = omap3_beagle_config
UBOOT_INSTALL_DIR           ?= $(HOME)/workdir/beagleboard/u-boot/u-boot-main

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
  SYSLINK_LOADER            = COFF
endif

# Generic Platform Specific compiler flags
PLATFORM_CFLAGS             += -DDSP_HAS_MMU

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv64+

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfloat-abi=softfp
