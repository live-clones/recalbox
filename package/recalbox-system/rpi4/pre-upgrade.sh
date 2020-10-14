#!/bin/bash

# fix issue recalbox#1356
# rpi4 freeze randomly when displaying bootsplash or in ES
# 
# enabling hdmi_enable_4kp60 on pi4 causes lots of malfunction
# so keep it disabled for now
if grep -q -E '^hdmi_enable_4kp60=1' /boot/config.txt; then
  /usr/bin/recallog -f upgrade.log -e "disabling hdmi_enable_4kp60"
  mount -o remount,rw /boot
  sed -i 's/^\s*\(hdmi_enable_4kp60\)=1/\1=0/' /boot/config.txt
  mount -o remount,ro /boot
fi
