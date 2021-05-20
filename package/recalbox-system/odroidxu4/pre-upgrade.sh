#!/bin/bash

if [ ! -d /boot/overlays ]; then
  mount -o remount,rw /boot
  mkdir /boot/overlays
fi
