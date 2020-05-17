# Component and OS selection
IPC = syslink
SHAREDMEM = cmem
DSPOS = sysbios56
GPPOS = linux

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = omap3_evm_defconfig
LINUXKERNEL_INSTALL_DIR     ?= $(HOME)/ti-dvsdk/dm3730-evm_4_01_00_09/psp/linux-2.6.32-psp03.00.01.06
UBOOT_CONFIG                = omap3_evm_config
UBOOT_INSTALL_DIR           ?= $(HOME)/ti-dvsdk/dm3730-evm_4_01_00_09/psp/u-boot-2009.11-psp03.00.01.06

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0x83700000
DSP_REGION_CMEM_SIZE        ?= 0x02900000
DSP_REGION_CODE_SIZE        ?= 0x02000000

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
