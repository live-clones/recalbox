#!/bin/bash

if [ ! -d /boot/overlays ]; then
  mount -o remount,rw /boot
  mkdir /boot/overlays
fi

# Remove old dats files from bios
rm -rf /recalbox/share/bios/dc/*.dat.zip
rm -rf /recalbox/share/bios/mame/*.dat.zip