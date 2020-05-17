#!/bin/sh

default_display="`cat /sys/devices/platform/omapdss/manager0/display`"
if [ "$default_display" = "lcd" ]; then
	echo "This script enables DVI display at 480p resolution"
	echo 0 > /sys/devices/platform/omapdss/display0/enabled
	echo 0 > /sys/devices/platform/omapdss/overlay0/enabled
	echo "" > /sys/devices/platform/omapdss/overlay0/manager
	echo "720,480" > /sys/class/graphics/fb0/virtual_size
	fbset -fb /dev/fb0 -xres 720 -yres 480
	echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
	echo "dvi" > /sys/devices/platform/omapdss/manager0/display
	echo 1 > /sys/devices/platform/omapdss/display2/enabled
	# DVI out timing can be configured at boottime
	# for 480p, the boot argument is "omapfb.mode=dvi:720x480@60"
	# By default DVI display timings are configured at VGA resolution
	echo "27903,720/24/96/96,480/10/32/3" > /sys/devices/platform/omapdss/display2/timings
	echo 1 > /sys/devices/platform/omapdss/overlay0/enabled
	/etc/init.d/matrix-gui-e stop
	`/usr/bin/matrix-gui-dvi-e start`
else
	echo "DVI display already enabled"
fi

