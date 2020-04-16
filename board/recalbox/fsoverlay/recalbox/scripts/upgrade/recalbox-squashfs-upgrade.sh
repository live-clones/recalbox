#!/bin/bash

# Stop ES
/etc/init.d/S31emulationstation stop
dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1
fbv -f -i /recalbox/system/resources/offline-install-0.jpg &
sleep 1

# Uncompress img
fbv -f -i /recalbox/system/resources/offline-install-1.jpg &
sleep 1
if test -e "/recalbox/share/system/upgrade/recalbox-rpi3.img.xz"; then
  unxz /recalbox/share/system/upgrade/recalbox-rpi3.img.xz || exit 1
else
  exit 1
fi

# Mount
fbv -f -i /recalbox/system/resources/offline-install-2.jpg &
sleep 1
mount -o loop,offset=512 /recalbox/share/system/upgrade/recalbox-rpi3.img /mnt/ || exit 1

fbv -f -i /recalbox/system/resources/offline-install-3.jpg &
sleep 1
mount -o remount,rw /boot/ || exit 1

# Delete old backup
rm -rf /boot/boot/.old          

# Files copy
fbv -f -i /recalbox/system/resources/offline-install-4.jpg &
sleep 1
cp /mnt/boot/recalbox /boot/boot/recalbox.update || exit 1
fbv -f -i /recalbox/system/resources/offline-install-5.jpg &
sleep 1
cp /mnt/boot/linux /boot/boot/linux.update || exit 1
fbv -f -i /recalbox/system/resources/offline-install-6.jpg &
sleep 1
cp /mnt/boot/initrd.gz /boot/boot/initrd.gz.update || exit 1

# Umount
umount /mnt/

# Clean up
rm /recalbox/share/system/upgrade/recalbox-rpi3.img

# Reboot
fbv -f -i /recalbox/system/resources/offline-install-7.jpg &
sleep 1
reboot

