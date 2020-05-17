# Component and OS selection
IPC = dsplink
SHAREDMEM = cmem
DSPOS = dspbios5
GPPOS = linux

# Required definitions for kernel module builds
LINUXKERNEL_CONFIG          = davinci_dm646x_1ghz_defconfig
LINUXKERNEL_INSTALL_DIR     = $(HOME)/dvsdk_dm6467t/linux/git
UBOOT_CONFIG                = davinci_dm6467_evm_config
UBOOT_INSTALL_DIR           = $(HOME)/dvsdk_dm6467t/u-boot/git

# Default memory map variables (can be overridden)
DSP_REGION_BASE_ADDR        ?= 0x84C00000
DSP_REGION_CMEM_SIZE        ?= 0x06E00000
DSP_REGION_CODE_SIZE        ?= 0x04600000

PLATFORM_CFLAGS             = -DDSP_REGION_BASE_ADDR=$(DSP_REGION_BASE_ADDR) -DDSP_REGION_CMEM_SIZE=$(DSP_REGION_CMEM_SIZE) -DDSP_REGION_CODE_SIZE=$(DSP_REGION_CODE_SIZE)

# Required definitions for DSPLINK/Bios5 builds
DSPLINK_PLATFORM            = DAVINCIHD
DSPLINK_DSP_PROCESSOR       = DM6467GEM
DSPLINK_GPP_OS              = DM6467LSP
DSPLINK_DSP_OS              = DSPBIOS5XX
DSPLINK_CFG_OPTS            =--nodsp=1 --DspTskMode=1

# DSP Architecture Specific Compiler flags
DSP_CFLAGS                  = -mv64+

# GPP Architecture Specific Compiler flags (can be overridden)
CFLAGS                      ?= -mcpu=arm926ej-s -mlittle-endian -mfloat-abi=soft
