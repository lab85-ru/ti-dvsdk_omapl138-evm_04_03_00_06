#!/bin/sh

CERTFILE=/home/root/matrixcert.pem


OPENSSL=/usr/bin/openssl

echo -e "\nDisplay Certificate Information"





if [ ! -r $CERTFILE ]
then
	echo "Certificate does not exist.  Generating new certificate before starting server..."
	/usr/bin/openssl_gen_cert.sh
fi


#$OPENSSL x509 -text -in $CERTFILE | more

echo
echo "Summary of Certificate Information:"
echo
$OPENSSL x509 -text -in $CERTFILE | egrep 'Certificate|Data|Version|Serial Number|Signature|Issuer|Subject'

