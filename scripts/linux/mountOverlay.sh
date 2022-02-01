#!/bin/bash

if [ -z "$1" ]; then
    echo "missing device" >&2 
    echo "please specify sdcard (/dev/mmcblk0) of usb key (/dev/sdb)" >&2
    exit 1
fi

DEVICE="$1"
if [[ "$DEVICE" =~ mmcblk ]]; then
    BOOTDEVICE="${DEVICE}p1"
else
    BOOTDEVICE="${DEVICE}1"
fi


mkdir -p /tmp/recalbox/lower /tmp/recalbox/upper /tmp/recalbox/work /tmp/recalbox/boot /tmp/recalbox/root
sudo mount "$BOOTDEVICE" /tmp/recalbox/boot || exit 1
sudo mount /tmp/recalbox/boot/boot/recalbox /tmp/recalbox/lower || exit 1
sudo mount -t overlay overlay -o rw,lowerdir=/tmp/recalbox/lower,upperdir=/tmp/recalbox/upper,workdir=/tmp/recalbox/work /tmp/recalbox/root || exit 1

echo "root file system is accessible from /tmp/recalbox/root"
echo "once finished:"
echo "umount /tmp/recalbox/root"
echo "umount /tmp/recalbox/lower"
echo "umount /tmp/recalbox/boot"
