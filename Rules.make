# Define target platform.
PLATFORM=omapl138

# The installation directory of the SDK.
SDK_INSTALL_DIR=/opt/ti-dvsdk_omapl138-evm_04_03_00_06

# For backwards compatibility
DVEVM_INSTALL_DIR=$(SDK_INSTALL_DIR)

# Where DSP/BIOS is installed.
BIOS_INSTALL_DIR=$(SDK_INSTALL_DIR)/dspbios_5_41_03_17

# Where the DSPBIOS Utils package is installed.
BIOSUTILS_INSTALL_DIR=$(SDK_INSTALL_DIR)/biosutils_1_02_02

# Where the Codec Engine package is installed.
CE_INSTALL_DIR=$(SDK_INSTALL_DIR)/codec-engine_2_26_02_11

# Where the DSP Link package is installed.
LINK_INSTALL_DIR=$(SDK_INSTALL_DIR)/dsplink_1_65_01_05_eng

# Where the codecs are installed.
CODEC_INSTALL_DIR=$(SDK_INSTALL_DIR)/codecs-omapl138_4_02_00_00

# Where DMAI package is installed.
DMAI_INSTALL_DIR=$(SDK_INSTALL_DIR)/dmai_2_20_00_15

# Where the EDMA3 LLD package is installed.
EDMA3_LLD_INSTALL_DIR=$(SDK_INSTALL_DIR)/edma3lld_01_11_02_05
EDMA3LLD_INSTALL_DIR=$(EDMA3_LLD_INSTALL_DIR)

# Where the DSP-side BIOS PSP package is installed.
BIOSPSP_INSTALL_DIR=$(SDK_INSTALL_DIR)/biospsp_01_30_01

# Where the Framework Components package is installed.
FC_INSTALL_DIR=$(SDK_INSTALL_DIR)/framework-components_2_26_00_01

# Where the MFC Linux Utils package is installed.
LINUXUTILS_INSTALL_DIR=$(SDK_INSTALL_DIR)/linuxutils_2_26_02_05
CMEM_INSTALL_DIR=$(LINUXUTILS_INSTALL_DIR)

# Where the XDAIS package is installed.
XDAIS_INSTALL_DIR=$(SDK_INSTALL_DIR)/xdais_6_26_01_03

# Where the RTSC tools package is installed.
XDC_INSTALL_DIR=$(SDK_INSTALL_DIR)/xdctools_3_16_03_36

# Where the Code Gen is installed.
CODEGEN_INSTALL_DIR=$(SDK_INSTALL_DIR)/cgt6x_6_1_14

# Where the DM365mm module is installed.
DM365MM_MODULE_INSTALL_DIR=$(SDK_INSTALL_DIR)/dm365mm-module_01_00_03

# Where the PSP is installed.
PSP_INSTALL_DIR=$(SDK_INSTALL_DIR)/psp

# The directory that points to your kernel source directory.
LINUXKERNEL_INSTALL_DIR=$(SDK_INSTALL_DIR)/psp/linux-2.6.37-psp03.21.00.04.sdk

# Where the development headers and libs are installed.
LINUXLIBS_INSTALL_DIR=$(SDK_INSTALL_DIR)/linux-devkit/arm-arago-linux-gnueabi/usr

# Where c6accel package is installed.
C6ACCEL_INSTALL_DIR=$(SDK_INSTALL_DIR)/c6accel_1_01_00_07

# Where DSP Benchmark package is installed.
DSP_BENCHMARK_INSTALL_DIR=$(SDK_INSTALL_DIR)/example-applications/dsp-benchmark_*

# Where c6run package is installed.
C6RUN_INSTALL_DIR=$(SDK_INSTALL_DIR)/c6run_0_98_03_03

# The prefix to be added before the GNU compiler tools (optionally including # path), i.e. "arm_v5t_le-" or "/opt/bin/arm_v5t_le-".
CSTOOL_DIR=/opt/ti-dvsdk_omapl138-evm_04_03_00_06/linux-devkit/
CSTOOL_PREFIX=$(CSTOOL_DIR)/bin/arm-arago-linux-gnueabi-

MVTOOL_DIR=$(CSTOOL_DIR)
MVTOOL_PREFIX=$(CSTOOL_PREFIX)

# Where to copy the resulting executables
EXEC_DIR=/opt/ti-dvsdk_omapl138-evm_04_03_00_06/targetfs

