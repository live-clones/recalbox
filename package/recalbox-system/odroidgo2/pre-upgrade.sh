#!/bin/bash

# This part will fetch the boot.ini header
# and will replace in boot.ini that comes with the update.
# This is done as for uboot from 3/12/2020 looks for
#   odroidgoa-uboot-config
OLDBOOT=$(head -n 1 /boot/boot.ini)
NEWBOOT=$(head -n 1 /boot/update/boot.ini)

if [ "$NEWBOOT" != "$OLDBOOT" ]; then
  mount -o remount,rw /boot
  sed -i -E '1 s/^.*$/'$OLDBOOT'/' /boot/update/boot.ini
  mount -o remount,ro /boot
fi

# remove old es_input.cfg as the joypad name has changed upstrem
rm /recalbox/share/system/.emulationstation/es_input.cfg

# disable visibility of pre-installed games to avoid double display of those games
grep -Eq '^\s*global.hidepreinstalledgames=' /recalbox/share/system/recalbox.conf || \
  echo -e '\nglobal.hidepreinstalledgames=1' >>/recalbox/share/system/recalbox.conf

# change retroarchcustom.cfg.odroidgo2 to fix shaders and core freeze
sed -i -E 's/^(\s*video_driver\s*=).*/\1 "gl"/' /recalbox/share/system/configs/retroarch/retroarchcustom.cfg.odroidgo2
