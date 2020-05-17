#!/bin/sh

CERTFILE=/home/root/matrixcert.pem
CERTSUBJ='/C=US/ST=Texas/L=Dallas/O=Texas Instruments/OU=ARM MPU/CN=Sitara User/emailAddress=SitaraUser@ti.com'


OPENSSL=/usr/bin/openssl

echo -e "\nGenerating Self Signed Certificate"

if [ -s $CERTFILE ]
then
	echo "Removing existing certificate file"
	rm $CERTFILE
fi

lsmod | grep ocf_omap3_cryptok >/dev/null
if [ `echo $?` = "0" ]
then
	rmmod ocf_omap3_cryptok
fi




if [ ! -r $CERTFILE ]
then
	echo "Creating certificate (${CERTFILE})"
	$OPENSSL req -x509 -nodes -days 365 -subj '/C=US/ST=Texas/L=Dallas/O=Texas Instruments/OU=ARM MPU/CN=Sitara User/emailAddress=SitaraUser@ti.com' -newkey rsa:1024 -keyout $CERTFILE -out $CERTFILE
else
	echo -e "\n## Certificate already exists."
	echo -e "## Delete ${CERTFILE} first and then run this script again to create a fresh certificate.\n"
	echo -e "## Or run the Certificate Info routine to view the existing certificate.\n"
	exit 1
fi

cat $CERTFILE

