#!/bin/bash

. board/recalbox/fsoverlay/etc/init.d/S06splash start

mkdir -p /tmp/video/rb /tmp/video/cu
for i in {01..30}; do touch /tmp/video/rb/video-recalbox-$i.mp4; done
for i in {01..30}; do touch /tmp/video/cu/video-custom-$i.mp4; done

originalVideoPath=/tmp/video/rb
customVideoPath=/tmp/video/cu

echo "TEST 1"
selectVideo
echo
echo "TEST 2"
findConnectedConnectors
echo
