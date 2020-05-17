#!/bin/sh

OGLESDIR=""
OGLESAPP="OGLESVase"

#Check if DISPLAY is set.  If so we assume we are running in X11
#and will call the X11 version of the demos if they exist.
if [ "$DISPLAY" == "" ]
then
    OGLESDIR="/usr/bin/SGX/demos/Raw"
else
    OGLESDIR="/usr/bin/SGX/demos/X11"
fi

if [ -e "$OGLESDIR/$OGLESAPP" ]
then
    cd $OGLESDIR
    ./$OGLESAPP $1 >/dev/null
    if [ "$?" != "0" ]
    then
        echo "Failed to run $OGLESDIR/$OGLESAPP"
        exit 1
    fi
else
    echo "Could not find $OGLESDIR/$OGLESAPP"
    exit 1
fi
