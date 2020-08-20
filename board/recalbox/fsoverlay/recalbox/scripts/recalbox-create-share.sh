#!/bin/bash

# Don't play animation when a special case is installing!
if [[ -f /tmp/.install.png ]] ; then
	exit 0
fi

# Stop
if [ "$1" = "stop" ]; then
	killall mpv
	exit 0
fi

# Start
touch /tmp/newshare
mpv --loop --fs --really-quiet --fps 5 --no-correct-pts /recalbox/system/resources/creating-share.mp4 &
exit 0
