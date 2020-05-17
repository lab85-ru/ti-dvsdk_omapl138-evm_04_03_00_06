#!/bin/sh
#check if wpa_supplicant is running
alreadyInstalled=`ps | grep -c wpa_supplicant`
if [ $alreadyInstalled -eq 1 ]; then
wpa_supplicant -d -Dnl80211 -c/etc/wpa_supplicant.conf -iwlan0 -B
sleep 1
fi

if [ -e /var/volatile/run/wpa_supplicant/wlan0 ]; then
        $1 -p  /var/volatile/run/wpa_supplicant -i wlan0
else
        echo "wpa_supplicant not running"
fi

