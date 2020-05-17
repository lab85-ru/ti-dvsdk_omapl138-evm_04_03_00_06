#!/bin/sh

DATAFILE=/home/root/rnddata


OPENSSL=/usr/bin/openssl

echo -e "\nGenerate SHA1 Hash"

if [ ! -r $DATAFILE ]
then
	echo "Creating 10M random data file ($DATAFILE)"
	echo "Please Wait..."
	dd if=/dev/urandom of=$DATAFILE bs=1048576 count=10
fi


lsmod | grep ocf_omap3_cryptok >/dev/null
if [ `echo $?` = "0" ]
then
	rmmod ocf_omap3_cryptok
fi


$OPENSSL dgst -sha1 $DATAFILE 


