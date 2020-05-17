#   =========================================================
#   DSP/BIOS LINK Configuration file.
#
#   CAUTION! This is a generated file.
#            All changes will be lost.
#
#   This file was generated on MAR 30, 2017  20:12:16
#   =========================================================


#   =========================================================
#   When this file was created.
#   =========================================================
export  TI_DSPLINK_CFGDATE            := MAR 30, 2017  20:12:16


#   =========================================================
#   DSP/BIOS LINK GPP side root directory.
#   =========================================================
export TI_DSPLINK_GPPROOT             := ${DSPLINK}/gpp
export TI_DSPLINK_DSPROOT             := ${DSPLINK}/dsp


#   =========================================================
#   GPP OS for which DSP/BIOS LINK is being built.
#   =========================================================
export TI_DSPLINK_GPPOS               := Linux
export TI_DSPLINK_GPPOSPREFIX         := 
export TI_DSPLINK_GPPDEVICE           := ARM
export TI_DSPLINK_GPPOSVERSION        := 2.6.18
export TI_DSPLINK_GPPDISTRIBUTION     := omapl138_arm


#   =========================================================
#   Target platform for DSP/BIOS LINK.
#   =========================================================
export TI_DSPLINK_PLATFORM            := OMAPL138


#   =========================================================
#   DSPs for which DSP/BIOS LINK is being built.
#   =========================================================
export TI_DSPLINK_DSPDEVICES          := DA8XXGEM 


#   =========================================================
#   External DSPS name for which DSP/BIOS LINK is being built.
#   =========================================================
export TI_DSPLINK_DSPDEVICES_EXTERNAL := OMAPL138GEM 


#   =========================================================
#   DSP Config files.
#   =========================================================
export TI_DSPLINK_DSPCFGFILES        := CFG_OMAPL138GEM_SHMEM.c 


#   =========================================================
#   Processor Counts and IDs.
#   =========================================================
export TI_DSPLINK_MAX_PROCESSORS     := 2
export TI_DSPLINK_MAX_DSPS           := 1
export TI_DSPLINK_ID_GPP             := 1


#   =========================================================
#   Default DSP Device and its physical interfaces for DSP/BIOS LINK.
#   =========================================================
ifndef TI_DSPLINK_BUILD_DEFAULTDSP
export  TI_DSPLINK_DSPDEVICE         := DA8XXGEM
export  TI_DSPLINK_DSPDEVICE_EXTERNAL:= OMAPL138GEM
export  TI_DSPLINK_DSPDISTRIBUTION   := c674x_5.xx_linux
export  TI_DSPLINK_DSPOS             := DspBios
export  TI_DSPLINK_DSPOSVERSION      := 5.XX
export  TI_DSPLINK_PROCID            := 0
export  TI_DSPLINK_DSPDEVICECLASS    := C674X
export  TI_DSPLINK_DSPTEMPPATH       := 
endif


#   =========================================================
#   Target DSP Devices and their physical interfaces for DSP/BIOS LINK.
#   =========================================================
export TI_DSPLINK_DA8XXGEM_OS                  := DspBios
export TI_DSPLINK_DA8XXGEM_DISTRIBUTION        := c674x_5.xx_linux
export TI_DSPLINK_DA8XXGEM_PHYINTERFACE        := SHMEM
export TI_DSPLINK_DA8XXGEM_CLASS               := C674X
export TI_DSPLINK_DA8XXGEM_PROCID              := 0
export TI_DSPLINK_DA8XXGEM_MODE                := DSP_SWI_MODE


#   =========================================================
#   Compile time scalability options for DSP/BIOS LINK.
#   =========================================================
export TI_DSPLINK_USE_PROC            := 1
export TI_DSPLINK_USE_POOL            := 1
export TI_DSPLINK_USE_NOTIFY          := 1
export TI_DSPLINK_USE_MPCS            := 1
export TI_DSPLINK_USE_RINGIO          := 1
export TI_DSPLINK_USE_MPLIST          := 1
export TI_DSPLINK_USE_MSGQ            := 1
export TI_DSPLINK_USE_CHNL            := 1


#   =========================================================
#   Compile time physical link scalability options for device.
#   =========================================================
export TI_DSPLINK_USE_MSGQ_ZCPY_LINK  := 1
export TI_DSPLINK_USE_CHNL_ZCPY_LINK  := 1


#   =========================================================
#   Enable/ Disable Trace.
#   =========================================================
export TI_DSPLINK_TRACE  := 0


#   =========================================================
#   GPP INC PATH
#   =========================================================
export  TI_DSPLINK_GPP_INC_PATH := ${DSPLINK}/gpp/inc ${DSPLINK}/gpp/inc/usr ${DSPLINK}/gpp/inc/sys ${DSPLINK}/gpp/inc/sys/arch ${DSPLINK}/gpp/inc/sys/arch/DA8XXGEM ${DSPLINK}/gpp/inc/sys/arch/DA8XXGEM/Linux ${DSPLINK}/gpp/inc/sys/Linux ${DSPLINK}/gpp/inc/sys/Linux/2.6.18 ${DSPLINK}/gpp/src/gen ${DSPLINK}/gpp/src/gen/Linux ${DSPLINK}/gpp/src/gen/Linux/2.6.18 ${DSPLINK}/gpp/src/osal ${DSPLINK}/gpp/src/osal/Linux ${DSPLINK}/gpp/src/osal/Linux/2.6.18 ${DSPLINK}/gpp/src/pmgr ${DSPLINK}/gpp/src/pmgr/Linux ${DSPLINK}/gpp/src/pmgr/Linux/2.6.18 ${DSPLINK}/gpp/src/api ${DSPLINK}/gpp/src/api/Linux ${DSPLINK}/gpp/src/api/Linux/2.6.18 ${DSPLINK}/gpp/src/ldrv ${DSPLINK}/gpp/src/ldrv/Linux ${DSPLINK}/gpp/src/ldrv/Linux/2.6.18 ${DSPLINK}/gpp/src/ldrv/DRV ${DSPLINK}/gpp/src/ldrv/DATA ${DSPLINK}/gpp/src/ldrv/MPCS ${DSPLINK}/gpp/src/ldrv/MPLIST ${DSPLINK}/gpp/src/ldrv/MQT ${DSPLINK}/gpp/src/ldrv/IPS ${DSPLINK}/gpp/src/ldrv/SMM ${DSPLINK}/gpp/src/ldrv/RINGIO ${DSPLINK}/gpp/src/ldrv/POOLS



#   =========================================================
#   GPP DEFINES
#   =========================================================
export  TI_DSPLINK_GPP_DEFINES :=  OS_LINUX MAX_DSPS=1 MAX_PROCESSORS=2 ID_GPP=1 OMAPL138 PROC_COMPONENT POOL_COMPONENT NOTIFY_COMPONENT MPCS_COMPONENT RINGIO_COMPONENT MPLIST_COMPONENT MSGQ_COMPONENT MSGQ_ZCPY_LINK CHNL_COMPONENT CHNL_ZCPY_LINK ZCPY_LINK KFILE_DEFAULT  DA8XXGEM DA8XXGEM_PHYINTERFACE=SHMEM_INTERFACE GPP_SWI_MODE

#   =========================================================
#   DSP INC PATH
#   =========================================================
export  TI_DSPLINK_DSP0_INC_PATH := ${DSPLINK}/dsp/inc ${DSPLINK}/dsp/inc ${DSPLINK}/dsp/inc ${DSPLINK}/dsp/inc/C674X ${DSPLINK}/dsp/inc/DspBios ${DSPLINK}/dsp/inc/DspBios/DA8XXGEM ${DSPLINK}/dsp/inc/DspBios/5.XX ${DSPLINK}/dsp/inc/DspBios/5.XX/DA8XXGEM ${DSPLINK}/dsp/src ${DSPLINK}/dsp/src/base ${DSPLINK}/dsp/src/base/hal ${DSPLINK}/dsp/src/base/hal/DspBios ${DSPLINK}/dsp/src/base/hal/DspBios/5.XX ${DSPLINK}/dsp/src/base/hal/DspBios/DA8XXGEM ${DSPLINK}/dsp/src/base/hal/DspBios/5.XX/DA8XXGEM ${DSPLINK}/dsp/src/base/drv ${DSPLINK}/dsp/src/base/drv/DspBios ${DSPLINK}/dsp/src/base/drv/DspBios/5.XX ${DSPLINK}/dsp/src/base/ips ${DSPLINK}/dsp/src/base/ips/DspBios ${DSPLINK}/dsp/src/base/ips/DspBios/5.XX ${DSPLINK}/dsp/src/base/gen ${DSPLINK}/dsp/src/base/gen/DspBios ${DSPLINK}/dsp/src/base/gen/DspBios/5.XX ${DSPLINK}/dsp/src/data ${DSPLINK}/dsp/src/data/DspBios ${DSPLINK}/dsp/src/data/DspBios/5.XX ${DSPLINK}/dsp/src/mpcs ${DSPLINK}/dsp/src/mpcs/DspBios ${DSPLINK}/dsp/src/mpcs/DspBios/5.XX ${DSPLINK}/dsp/src/mplist ${DSPLINK}/dsp/src/mplist/DspBios ${DSPLINK}/dsp/src/mplist/DspBios/5.XX ${DSPLINK}/dsp/src/msg ${DSPLINK}/dsp/src/msg/DspBios ${DSPLINK}/dsp/src/msg/DspBios/5.XX ${DSPLINK}/dsp/src/notify ${DSPLINK}/dsp/src/notify/DspBios ${DSPLINK}/dsp/src/notify/DspBios/5.XX ${DSPLINK}/dsp/src/pools ${DSPLINK}/dsp/src/pools/DspBios ${DSPLINK}/dsp/src/pools/DspBios/5.XX ${DSPLINK}/dsp/src/ringio ${DSPLINK}/dsp/src/ringio/DspBios ${DSPLINK}/dsp/src/ringio/DspBios/5.XX



#   =========================================================
#   DSP SPECIFIC DEFINES
#   =========================================================
export  TI_DSPLINK_DSP0_DEFINES :=  PROCID=0 DA8XXGEM DA8XXGEM_INTERFACE=SHMEM_INTERFACE PHYINTERFACE=SHMEM_INTERFACE DSP_SWI_MODE

#   =========================================================
#   DSP COMMON DEFINES
#   =========================================================
export  TI_DSPLINK_DSP_COMMON_DEFINES :=  MAX_DSPS=1 MAX_PROCESSORS=2 ID_GPP=1 OMAPL138 PROC_COMPONENT POOL_COMPONENT NOTIFY_COMPONENT MPCS_COMPONENT RINGIO_COMPONENT MPLIST_COMPONENT MSGQ_COMPONENT MSGQ_ZCPY_LINK CHNL_COMPONENT CHNL_ZCPY_LINK ZCPY_LINK

