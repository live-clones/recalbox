#!/bin/bash

source /recalbox/scripts/recalbox-utils.sh
IMAGE_PATH=$(getInstallUpgradeImagePath)

# Don't play animation when a special case is installing!
if [[ -f /tmp/.install.png ]] ; then
  exit 0
fi

# Stop
if [ "$1" = "stop" ]; then
  if [[ -f /tmp/first-boot-video ]] ; then
    rm -f /tmp/first-boot-video
    killall mpv
  fi
  if [[ -f /tmp/first-boot-slides ]] ; then
    rm -f /tmp/first-boot-slides
    while pkill --signal 0 fbv2; do sleep 1; done
  fi
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
playVideo() {
  local conn
  if [[ ! -f /tmp/first-boot-video ]] ; then
    touch /tmp/first-boot-video
    if [[ "$machineArch" =~ x86 ]]; then
      # on x86 and x86_64, find connected monitors and play video on all of them
      findConnectedConnectors | while read -r conn; do
        mpv --loop --really-quiet --fps 5 --no-correct-pts -ao=alsa --vo=drm --drm-connector=$conn /recalbox/system/resources/creating-share.mp4 &
      done
    else
      # on rpi, odroidxu4, simply play
      mpv --loop --really-quiet --fps 5 --no-correct-pts /recalbox/system/resources/creating-share.mp4 &
    fi
  fi  
}

# playSlides
#    play all the 8 slides
playSlides() {
  if [[ ! -f /tmp/first-boot-slides ]] ; then
    touch /tmp/first-boot-slides
    fbv2 -i -k -e -s 150 \
      "$IMAGE_PATH/install-1.png" \
      "$IMAGE_PATH/install-2.png" \
      "$IMAGE_PATH/install-3.png" \
      "$IMAGE_PATH/install-4.png" \
      "$IMAGE_PATH/install-5.png" \
      "$IMAGE_PATH/install-6.png" \
      "$IMAGE_PATH/install-7.png" \
      "$IMAGE_PATH/install-8.png" &
  fi  
}

# Start
fbdevHeight=$(cut -d, -f2 /sys/class/graphics/fb0/virtual_size)
if [ "${fbdevHeight}" -le 320 ] ; then
  playVideo
else  
  playSlides
fi
exit 0
