#!/bin/sh
rm /tftpboot/uImage
make linux_clean
make linux -j24 1>1.1 2>2.2
