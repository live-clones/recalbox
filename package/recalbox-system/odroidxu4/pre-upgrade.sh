#!/bin/bash

if [ ! -d /boot/overlays ]; then
  mount -o remount,rw /boot
  mkdir /boot/overlays
fi

# Remove all .nvmem and .nvmem2 files from saves
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem2
