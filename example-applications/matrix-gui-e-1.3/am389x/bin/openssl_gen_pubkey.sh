#!/bin/sh

CERTFILE=/home/root/matrixcert.pem
PUBKEY=/home/root/pubkey.pem


OPENSSL=/usr/bin/openssl

echo -e "\nGenerating Public Key from ${CERTFILE}"


if [ ! -r $CERTFILE ]
then
	echo "Certificate does not exist.  Generate certificate first before generating a public key"
	exit 1
else
	$OPENSSL rsa -in $CERTFILE -pubout > $PUBKEY
	echo -e "\nPublic Key written to ${PUBKEY}\n"
fi

cat $PUBKEY

