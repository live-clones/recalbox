#!/bin/bash

source /recalbox/scripts/recalbox-utils.sh
IMAGE_PATH=$(getInstallUpgradeImagePath)
INIT_SCRIPT=$(basename "$0")

do_update() {

  UPDATEFILE="rbx_no_upgrade"

  for file in /recalbox/share/system/upgrade/recalbox-*.img; do
    if [[ -f $file ]]; then
      UPDATEFILE=${file}
      break
    fi
  done

  # Stop ES
  echo "stopping emulationstation"
  /etc/init.d/S31emulationstation stop
  dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1
  fbv2 -k -i "$IMAGE_PATH/offline-install-1.jpg"
  sleep 1

  # Mount
  echo "mount image"
  fbv2 -k -i "$IMAGE_PATH/offline-install-2.jpg"
  sleep 1
  LBA=$(lba-finder "${UPDATEFILE}")
  echo "intiating a losetup for lba ${LBA} (offset $((LBA * 512))s)"
  LOOPFILE=$(losetup -f --show "${UPDATEFILE}" -o $((LBA * 512))) || return 2
  mount "$LOOPFILE" /mnt || return 3

  echo "remounting /boot R/W"
  fbv2 -k -i "$IMAGE_PATH/offline-install-3.jpg"
  sleep 1
  mount -o remount,rw /boot/ || return 4

  # Files copy
  echo "copying update boot files"
  fbv2 -k -i "$IMAGE_PATH/offline-install-4.jpg"
  if [ -f /mnt/boot.lst ]; then
    while read -r file; do
      echo "  processing $file"
      [[ ! -d /boot/update/$(dirname "$file") ]] && mkdir -p "/boot/update/$(dirname "$file")"
      cp "/mnt/$file" "/boot/update/$file" || return 5
    done < /mnt/boot.lst
    cp /mnt/boot.lst /boot/update/ || return 6
  else
    return 7
  fi
  fbv2 -k -i "$IMAGE_PATH/offline-install-5.jpg"
  sleep 1

  # Umount
  echo "unmount image"
  umount /mnt/ || return 8
  fbv2 -k -i "$IMAGE_PATH/offline-install-6.jpg"
  sleep 1

  return 0

}

log() {
  while read -r line; do
    /usr/bin/recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -f upgrade.log -e "$line"
  done
}

# redirect STDOUT and STDERR to recallog
exec &> >(log)
echo "starting upgrade"

do_update
RC=$?
if [ $RC -eq 0 ]; then
  if [ -f "/boot/update/pre-upgrade.sh" ]; then
    echo "running pre-upgrade script"
    bash /boot/update/pre-upgrade.sh
  fi
  # clear case setting
  echo "clear case setting"
  sed -i -E 's/^case=/;case=/' /boot/recalbox-boot.conf
  # Reboot
  echo "upgrade successfull, rebooting"
  fbv2 -k -i "$IMAGE_PATH/offline-install-7.jpg"
  sleep 1
else
  echo "upgrade failed return code $RC" >&2
  fbv2 -k -i "$IMAGE_PATH/offline-install-error.jpg"
  sleep 10
fi
sync
reboot
exit 0
