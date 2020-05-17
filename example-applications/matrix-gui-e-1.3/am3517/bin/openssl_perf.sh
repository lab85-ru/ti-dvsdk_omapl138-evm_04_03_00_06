#!/bin/sh


CRYPTOMOD=/lib/modules/2.6.37/kernel/crypto/ocf/cryptodev.ko
OCFMOD=/lib/modules/2.6.37/crypto/ocf/ocf_omap3_cryptok.ko


OPENSSL=/usr/bin/openssl

lsmod | grep cryptosoft >/dev/null
if [ `echo $?` = "0" ]
then
	echo "Uninstalling cryptosoft module"
	rmmod cryptosoft
fi



lsmod | grep cryptodev >/dev/null
if [ `echo $?` = "1" ]
then
	if [ -r $CRYPTOMOD ]
	then
		echo "Installing cryptodev module"
		insmod $CRYPTOMOD
	fi
else
	echo "Cryptodev module is already installed"
fi



lsmod | grep ocf_omap3_cryptok >/dev/null
if [ `echo $?` = "1" ]
then
	if [ -r $OCFMOD ]
	then
		echo "Installing ocf_omap3_crypto module"
		insmod $OCFMOD ocf_omap3_crypto_dma=1
	fi
else
	echo "ocf_omap3_crypto module is already installed"
fi


if [ -r $OPENSSL ]
then
	$OPENSSL version
else
	echo "Unable to find OpenSSL"
	exit 1
fi

echo "################################"
echo "Running OpenSSL Speed tests.  "
echo "There are 7 tests and each takes 15 seconds..."
echo

TEMP=/home/root/temp

echo "Running aes-128-cbc test.  Please Wait..."
time -v $OPENSSL speed -evp aes-128-cbc -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running aes-192-cbc test.  Please Wait..."
time -v $OPENSSL speed -evp aes-192-cbc -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running aes-256-cbc test.  Please Wait..."
time -v $OPENSSL speed -evp aes-256-cbc -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running des-cbc test.  Please Wait..."
time -v $OPENSSL speed -evp des-cbc -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running des3 test.  Please Wait..."
time -v $OPENSSL speed -evp des3 -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running sha1 test.  Please Wait..."
time -v $OPENSSL speed -evp sha1 -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

echo "Running md5 test.  Please Wait..."
time -v $OPENSSL speed -evp md5 -engine cryptodev > $TEMP 2>&1
egrep 'Doing|User|System|Percent|Elapsed' $TEMP

rm $TEMP


