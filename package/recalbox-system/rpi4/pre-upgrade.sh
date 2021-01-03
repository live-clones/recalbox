#!/bin/bash

mount -o remount,rw /boot

sed -i -E '/^# enable fake-kms overlay/d' /boot/config.txt
sed -i -E '/^# possible alternatives are/d' /boot/config.txt
sed -i -E '/^# \* vc4-kms-v3d/d' /boot/config.txt
sed -i -E '/^# \* vc4-kms-v3d-pi4/d' /boot/config.txt
sed -i -E '/dtoverlay=vc4-f?kms-v3d/d' /boot/config.txt
sed -i -E '/^# allow only one framebuffer \(the one marked 0, closest to the power input\)/d' /boot/config.txt
sed -i -E '/max_framebuffers=/d' /boot/config.txt
sed -i -E '/^# disable 4k 60hz - enabling can cause random freezes/d' /boot/config.txt
sed -i -E '/hdmi_enable_4kp60/d' /boot/config.txt
# remove empty double lines
sed -i -E '$!N;/^\n$/{$q;D;};P;D;' /boot/config.txt

mount -o remount,ro /boot
