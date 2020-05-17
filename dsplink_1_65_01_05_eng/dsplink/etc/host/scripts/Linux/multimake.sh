echo off
#   =========================================================
#   DSP/BIOS LINK Configuration file.
#
#   CAUTION! This is a generated file.
#            All changes will be lost.
#
#   This file was generated on MAR 30, 2017  20:12:16
#   =========================================================
echo on
echo off
#   =========================================================
#   Build DSP side binary for DA8XXGEM
#   =========================================================
export TI_DSPLINK_BUILD_DEFAULTDSP=0
export TI_DSPLINK_DSPDEVICE=DA8XXGEM
export TI_DSPLINK_DSPDEVICE_EXTERNAL=OMAPL138GEM
export TI_DSPLINK_DSPDISTRIBUTION=c674x_5.xx_linux
export TI_DSPLINK_DSPOS=DspBios
export TI_DSPLINK_DSPOSVERSION=5.XX
export TI_DSPLINK_PROCID=0
export TI_DSPLINK_DSPDEVICECLASS=C674X
export TI_DSPLINK_DSPTEMPPATH=
echo on
gmake -f Makefile $@
echo off
export TI_DSPLINK_BUILD_DEFAULTDSP=
echo on


