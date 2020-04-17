#!/bin/bash

UPDATEARCH=`cat /recalbox/recalbox.arch`
UPDATEFILE="recalbox-${UPDATEARCH}.img.xz"
UPDATEFILE2="recalbox-${UPDATEARCH}.img"

function do_update() {

echo "go"
  # Stop ES
  /etc/init.d/S31emulationstation stop
  dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1
  fbv -f -i /recalbox/system/resources/offline-install-0.jpg &
  sleep 1

  # Uncompress img
  fbv -f -i /recalbox/system/resources/offline-install-1.jpg &
  sleep 1
  if test -e "/recalbox/share/system/upgrade/${UPDATEFILE}"; then
    unxz "/recalbox/share/system/upgrade/${UPDATEFILE}" || return 1
  else
    return 1
  fi

  # Mount
  fbv -f -i /recalbox/system/resources/offline-install-2.jpg &
  sleep 1
  mount -o loop,offset=512 "/recalbox/share/system/upgrade/${UPDATEFILE2}" /mnt/ || return 1

  fbv -f -i /recalbox/system/resources/offline-install-3.jpg &
  sleep 1
  mount -o remount,rw /boot/ || return 1

  # Files copy
  fbv -f -i /recalbox/system/resources/offline-install-4.jpg &
  sleep 1
  cp /mnt/boot/recalbox /boot/update/recalbox || return 1
  fbv -f -i /recalbox/system/resources/offline-install-5.jpg &
  sleep 1
  cp /mnt/boot/linux /boot/update/linux || return 1
  fbv -f -i /recalbox/system/resources/offline-install-6.jpg &
  sleep 1
  cp /mnt/boot/initrd.gz /boot/update/initrd.gz || return 1

  # Umount
  umount /mnt/ || return 1

  # Clean up
  rm "/recalbox/share/system/upgrade/${UPDATEFILE2}" || return 1

  return 0

}

if do_update; then
  # Reboot
  fbv -f -i /recalbox/system/resources/offline-install-7.jpg &
  sleep 1
  reboot
else
  fbv -f -i /recalbox/system/resources/offline-install-error.jpg &
  sleep 10
  reboot
fi

exit 0
