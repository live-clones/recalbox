#!/bin/bash

function do_update() {

  UPDATEFILE="rbx_no_upgrade"
  UPDATEFILE2="rbx_no_upgrade"

  for file in /recalbox/share/system/upgrade/recalbox-*.img.xz; do
    if [[ -f $file ]]; then
      UPDATEFILE=${file}
      UPDATEFILE2=${file::-3}
      break
    fi
  done

  # Stop ES
  /etc/init.d/S31emulationstation stop
  dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1
  fbv -f -i /recalbox/system/resources/offline-install-0.jpg &
  sleep 1

  # Uncompress img
  fbv -f -i /recalbox/system/resources/offline-install-1.jpg &
  sleep 1
  if test -e "${UPDATEFILE}"; then
    unxz "${UPDATEFILE}"
  else
    return 1
  fi

  # Mount
  fbv -f -i /recalbox/system/resources/offline-install-2.jpg &
  sleep 1
  losetup /dev/loop1 "${UPDATEFILE2}" -o 1048576
  RESULT=$?
  if [ $RESULT -ne 0 ]; then
    losetup /dev/loop1 "${UPDATEFILE2}" -o $((2048 * 512)) || return 1
  fi
  mount /dev/loop1 /mnt || return 1

  fbv -f -i /recalbox/system/resources/offline-install-3.jpg &
  sleep 1
  mount -o remount,rw /boot/ || return 1

  # Files copy
  fbv -f -i /recalbox/system/resources/offline-install-4.jpg &
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
  fbv -f -i /recalbox/system/resources/offline-install-5.jpg &
  sleep 1

  # Umount
  umount /mnt/ || return 1

  # Clean up
  rm "${UPDATEFILE2}" || return 1
  fbv -f -i /recalbox/system/resources/offline-install-6.jpg &
  sleep 1

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

