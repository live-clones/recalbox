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

machineArch=$(arch)

# findConnectedConnectors
#   returns a list of connected connectors (using mpv)
findConnectedConnectors() {
  grep -l ^connected /sys/class/drm/card*/status | sed 's/.*card\([0-9]\+\)-\([^\/]\+\).*/\1.\2/'
}

# playVideo
#   select all screen and run playVideoOnOneScreen for each connected screen
# intput:
# * filename to play
playVideo() {
  local conn
  # Already playing?
  if [[ -f /tmp/newshare ]] ; then
    exit 0
  fi
  if [[ "$machineArch" =~ x86 ]]; then
    # on x86 and x86_64, find connected monitors and play video on all of them
    findConnectedConnectors | while read -r conn; do
      mpv --loop --really-quiet --fps 5 --no-correct-pts -ao=alsa --vo=drm --drm-connector=$conn /recalbox/system/resources/creating-share.mp4 &
    done
  else
    # on rpi, odroidxu4, simply play
    mpv --loop --really-quiet --fps 5 --no-correct-pts /recalbox/system/resources/creating-share.mp4 &
  fi
}

# Start
touch /tmp/newshare
playVideo
exit 0
