# Component and OS selection
IPC = dsplink
SHAREDMEM = cmem
DSPOS = dspbios5
GPPOS = linux

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = davinci_dm6446_defconfig
LINUXKERNEL_INSTALL_DIR     = $(HOME)/workdir/dm644X/kernel
UBOOT_CONFIG                = davinci_evm_config
UBOOT_INSTALL_DIR           = $(HOME)/workdir/dm644X/u-boot

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0x86000000
DSP_REGION_CMEM_SIZE        ?= 0x01000000
DSP_REGION_CODE_SIZE        ?= 0x01000000

PLATFORM_CFLAGS             = -DDSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR) -DDSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE) -DDSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)

# Required definitions for DSPLINK/Bios5 builds
DSPLINK_PLATFORM            = DAVINCI
DSPLINK_DSP_PROCESSOR       = DM6446GEM
DSPLINK_GPP_OS              = DM6446LSP
DSPLINK_DSP_OS              = DSPBIOS5XX
DSPLINK_CFG_OPTS            =--nodsp=1 --DspTskMode=1

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv64+

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -mcpu=arm926ej-s -mlittle-endian -mfloat-abi=soft
