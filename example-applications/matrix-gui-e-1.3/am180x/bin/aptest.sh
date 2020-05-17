#!/bin/sh
#check if the system is already running in station mode.
#if it does, revert to ap mode
StationRunning=`ps | grep -c wpa_supplicant`
if [ $StationRunning -eq 2 ]; then 
	killall wpa_supplicant
fi
#start ap mode
ApRunning=`ps | grep -c hostapd`
if [ $ApRunning -eq 1 ]; then
	echo 1 > /proc/sys/net/ipv4/ip_forward
	ifconfig wlan0 192.168.0.1
	hostapd -B /etc/hostapd.conf -P /var/run/hostapd.pid
	udhcpd /etc/udhcpd.conf
	iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
fi

