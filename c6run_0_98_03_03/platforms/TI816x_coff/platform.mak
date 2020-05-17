# Component and OS selection
IPC = syslink
SHAREDMEM = cmem
DSPOS = sysbios6
GPPOS = linux

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = ti8168_evm_defconfig
LINUXKERNEL_INSTALL_DIR     ?= $(HOME)/ti-ezsdk/c6a816x-evm_5_01_00_52/psp/linux-2.6.37-psp04.00.00.09
UBOOT_CONFIG                = ti8168_evm_config
UBOOT_INSTALL_DIR           ?= $(HOME)/ti-ezsdk/c6a816x-evm_5_01_00_52/psp/u-boot-2010.06-psp04.00.00.09

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0x96C00000
DSP_REGION_CMEM_SIZE        ?= 0x02000000
DSP_REGION_CODE_SIZE        ?= 0x01500000

PLATFORM_CFLAGS             = -DDSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR) -DDSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE) -DDSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)

ifeq ($(IPC),dsplink)
  # DSPLink not supported
else
  # Required definitions for SysLink/SysBios6 builds
  SYSLINK_PLATFORM          = TI81XX
  SYSLINK_VARIANT           = TI816X
  SYSLINK_LOADER            = COFF
endif

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv6740

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -march=armv7-a -mtune=cortex-a8 -mlittle-endian -mfloat-abi=softfp
