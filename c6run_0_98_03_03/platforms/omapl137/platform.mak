# Component and OS selection
IPC = dsplink
SHAREDMEM = cmem
DSPOS = dspbios5
GPPOS = linux

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = da830_omapl137_defconfig
LINUXKERNEL_INSTALL_DIR     = $(HOME)/workdir/omap-l137/kernel
UBOOT_CONFIG                = da830_omapl137_evm_config
UBOOT_INSTALL_DIR           = $(HOME)/workdir/omap-l137/uboot

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0xC2000000
DSP_REGION_CMEM_SIZE        ?= 0x01000000
DSP_REGION_CODE_SIZE        ?= 0x01000000

PLATFORM_CFLAGS             = -DDSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR) -DDSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE) -DDSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)

ifeq ($(IPC),dsplink)
  # Required definitions for DSPLINK/Bios5 builds
  DSPLINK_PLATFORM            = OMAPL1XX
  DSPLINK_DSP_PROCESSOR       = OMAPL1XXGEM
  DSPLINK_GPP_OS              = ARM
  DSPLINK_DSP_OS              = DSPBIOS5XX
  DSPLINK_CFG_OPTS            =--nodsp=1 --DspTskMode=1
else
  # Required definitions for SysLink/SysBios6 builds
  SYSLINK_PLATFORM          = OMAPL1XX
  SYSLINK_LOADER            = COFF
endif

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv6740

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -mcpu=arm926ej-s -mlittle-endian -mfloat-abi=soft
