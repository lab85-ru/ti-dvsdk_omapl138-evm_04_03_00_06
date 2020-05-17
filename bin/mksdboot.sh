#! /bin/sh
# Script to create SD card for OMAPL138 plaform.
#
# Author: Brijesh Singh, Texas Instruments Inc.
#
# Licensed under terms of GPLv2

VERSION="0.4"

execute ()
{
    $* >/dev/null
    if [ $? -ne 0 ]; then
        echo
        echo "ERROR: executing $*"
        echo
        exit 1
    fi
}

version ()
{
  echo
  echo "`basename $1` version $VERSION"
  echo "Script to create bootable SD card for OMAP-L138 kits"
  echo

  exit 0
}

usage ()
{
  echo "
Usage: `basename $1` [options] <sdk_install_dir> <device>

Mandatory options:
  --device              SD block device node (e.g /dev/sdd)
  --sdk                 Where is sdk installed ?

Optional options:
  --version             Print version.
  --help                Print this help message.
"
  exit 1
}

# Process command line...
while [ $# -gt 0 ]; do
  case $1 in
    --help | -h)
      usage $0
      ;;
    --device) shift; device=$1; shift; ;;
    --sdk) shift; sdkdir=$1; shift; ;;
    --version) version $0;;
    *) copy="$copy $1"; shift; ;;
  esac
done

test -z $sdkdir && usage $0
test -z $device && usage $0

if [ ! -d $sdkdir ]; then
   echo "ERROR: $sdkdir does not exist"
   exit 1;
fi

if [ ! -f $sdkdir/filesystem/dvsdk-da850-omapl138-evm-rootfs.tar.gz ]; then
  echo "ERROR: failed to find rootfs $sdkdir/filesystem/dvsdk-da850-omapl138-evm-rootfs.tar.gz"
  exit 1;
fi
 
if [ ! -b $device ]; then
   echo "ERROR: $device is not a block device file"
   exit 1;
fi

echo "************************************************************"
echo "*         THIS WILL DELETE ALL THE DATA ON $device        *"
echo "*                                                          *"
echo "*         WARNING! Make sure your computer does not go     *"
echo "*                  in to idle mode while this script is    *"
echo "*                  running. The script will complete,      *"
echo "*                  but your SD card may be corrupted.      *"
echo "*                                                          *"
echo "*         Press <ENTER> to confirm....                     *"
echo "************************************************************"
read junk

for i in `ls -1 $device?`; do
 echo "unmounting device '$i'"
 umount $i 2>/dev/null
done

execute "dd if=/dev/zero of=$device bs=1024 count=1024"

# get the partition information.
total_size=`fdisk -l $device | grep Disk | awk '{print $5}'`
total_cyln=`echo $total_size/255/63/512 | bc`

# start from cylinder 20, this should give enough space for flashing utility
# to write u-boot binary. 
pc1_start=20
pc1_end=10

# recalculate number of cylinder for the first parition
if [ "$copy" != "" ]; then
 pc1_end=$(((($total_cyln - $pc1_end) / 4) * 3))
fi

# start of rootfs partition
pc2_start=$(($pc1_start + $pc1_end))

{
echo $pc1_start,$pc1_end,0x0B,-
echo $pc2_start,,-
} | sfdisk -D -H 255 -S 63 -C $total_cyln $device

if [ $? -ne 0 ]; then
    echo ERROR
    exit 1;
fi

echo "Formating ${device}1 ..."
execute "mkfs.vfat -F 32 -n "START_HERE" ${device}1"
echo "Formating ${device}2 ..."
execute "mke2fs -j -L "ROOTFS" ${device}2"

# creating boot.scr
execute "mkdir -p /tmp/sdk"
cat <<EOF >/tmp/sdk/boot.cmd
mmc rescan 0
setenv bootargs 'console=ttyS2,115200n8 root=/dev/mmcblk0p2 rw ip=off mem=32M@0xc0000000 mem=64M@0xc4000000 rootwait'
fatload mmc 0 c0700000 uImage
bootm c0700000
EOF

mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n 'Execute uImage' -d /tmp/sdk/boot.cmd /tmp/sdk/boot.scr

if [ $? -ne 0 ]; then
  echo "Failed to execute mkimage to create boot.scr"
  echo "Execute 'sudo apt-get install uboot-mkimage' to install the package"
  exit 1
fi

echo "Copying uImage/boot.scr on ${device}1"
execute "mkdir -p /tmp/sdk/$$"
execute "mount ${device}1 /tmp/sdk/$$"
execute "cp /tmp/sdk/boot.scr /tmp/sdk/$$/"
execute "cp /tmp/sdk/boot.cmd /tmp/sdk/$$/"
execute "cp $sdkdir/bin/README.boot.scr /tmp/sdk/$$/"
execute "cp $sdkdir/psp/prebuilt-images/uImage*.bin /tmp/sdk/$$/uImage"
execute "cp $sdkdir/bin/setup.htm /tmp/sdk/$$"
execute "mkdir -p /tmp/sdk/$$/c6748"
execute "cp $sdkdir/bin/c6748.htm /tmp/sdk/$$/c6748/"
execute "mkdir -p /tmp/sdk/$$/screen_shots"
execute "cp $sdkdir/bin/top_omapl138_evm.png /tmp/sdk/$$/screen_shots/"
execute "mkdir -p /tmp/sdk/$$/Guides"
execute "cp $sdkdir/docs/OMAP-L138*.pdf /tmp/sdk/$$/Guides/"
execute "cp $sdkdir/docs/OMAPL138_EVM_Quick_Start_Guide.pdf /tmp/sdk/$$/Guides/OMAP-L138_EVM_QSG.pdf"
execute "cp $sdkdir/docs/OMAPL138_EXP_Quick_Start_Guide.pdf /tmp/sdk/$$/Guides/OMAP-L138_EXP_QSG.pdf"
if [ "$copy" != "" ]; then
  echo "Copying additional file(s) on ${device}1"
  execute "cp -r $copy /tmp/sdk/$$"
fi
sync
execute "umount /tmp/sdk/$$"

echo "Executing uflash tool to write u-boot.bin"
$sdkdir/psp/board-utilities/images/utils/uflash -d ${device} -b ${sdkdir}/psp/prebuilt-images/u-boot-da850-omapl138-evm.bin -p omapl138 -vv

if [ $? -ne 0 ]; then
  echo "Failed to execute uflash"
  exit 1
fi

echo "Extracting filesystem on ${device}2 ..."
execute "mkdir -p /tmp/sdk/$$"
execute "mount ${device}2 /tmp/sdk/$$"
execute "tar zxf $sdkdir/filesystem/dvsdk-da850-omapl138-evm-rootfs.tar.gz -C /tmp/sdk/$$"
sync

# check if we need to create symbolic link for matrix 
echo -n "Creating matrix-gui-e symbolic link..."
if [ -f /tmp/sdk/$$/etc/init.d/matrix-gui-e ]; then
  if [ -h /tmp/sdk/$$/etc/rc3.d/*matrix* ]; then
    echo " (skipped) "
  else
    ln -s  ../init.d/matrix-gui-e /tmp/sdk/$$/etc/rc3.d/S99matrix-gui-e
    ln -s  ../init.d/matrix-gui-e /tmp/sdk/$$/etc/rc5.d/S99matrix-gui-e
    echo "done"
  fi
fi

sync
echo "unmounting ${device}2"
execute "umount /tmp/sdk/$$"

execute "rm -rf /tmp/sdk/$$"
echo "completed!"

