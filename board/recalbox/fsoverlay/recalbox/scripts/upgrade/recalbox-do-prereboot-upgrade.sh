#!/bin/bash

while [[ $# -gt 1 ]]
do
key="$1"
case $key in
    --upgrade-dir)
    UPGRADE_DIR="$2"
    shift
    ;;
    *)
    ;;
esac
shift
done

if [[ -z ${UPGRADE_DIR} ]]; then
  echo -e "Usage:\n$0 --upgrade-dir /path/to/tmp/dir" && exit 1
fi

source /recalbox/scripts/upgrade/recalbox-upgrade.inc.sh

FILES_EXTRACTED=/tmp/upgrade.files
rm "$FILES_EXTRACTED" 2>/dev/null

function calcDownloadedSize() {
    cat "$FILES_EXTRACTED" | wc -l
}

# Show the progression while download
function cyclicProgression() {
    totalFiles="$1"
    extractedFiles=0
    while true ; do
        extractedFiles=$(calcDownloadedSize)
        echoES "PREPARING: $(( 100 * extractedFiles / totalFiles ))%"
        [[ "$extractedFiles" -ge  "$totalFiles" ]] && break
        sleep 1
    done
    echoES "PLEASE WAIT..."
    #~ kill -9 "$2"
}

function doModulesUpdate() {
    # Great, time to extract /lib/modules, get rid of the top /lib/modules/ in the listing to have the right number of files to extract
    nbFiles=`grep "/lib/modules/..*" ${UPGRADE_DIR}/root.list | wc -l`

    mount -o remount, rw /
    cyclicProgression "$nbFiles" "$!" &
    ( cd / && \
      tar xvf "${UPGRADE_DIR}/root.tar.xz" --wildcards "./lib/modules/*" >> "$FILES_EXTRACTED" ) || { echoerr "Failed upgrading /lib/modules" && cleanBeforeExit 10 ; exit 10 ; }
    wait
    mount -o remount, ro /

    echoerr "/lib/modules successfully upgraded!"
    return 0
}

function doBootUpgrade() {
    # Upgrade /boot (can't be done partially because of dtb and such)
    mount -o remount, rw /boot
    BOOTFILES="cmdline.txt config.txt recalbox-boot.conf"
    INIT_SCRIPT=$(basename "$0")
    
    # backup files to keep
    for BOOTFILE in ${BOOTFILES} ; do
	[[ -e "/boot/${BOOTFILE}" ]] && cp "/boot/${BOOTFILE}" "/boot/${BOOTFILE}.upgrade" | recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -f preupgrade.log
    done
    
    echoES "UPGRADING: /BOOT..."
    (cd /boot && tar xf "${UPGRADE_DIR}/boot.tar.xz" --no-same-owner)
    [ $? -ne 0 ] && echoerr "Failed upgrading /boot" && cleanBeforeExit 10
    
    # restore backup files
    for BOOTFILE in ${BOOTFILES} ; do
	[[ -e "/boot/${BOOTFILE}.upgrade" ]] && mv "/boot/${BOOTFILE}.upgrade" "/boot/${BOOTFILE}"  | recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -f preupgrade.log
    done
    
    mount -o remount, ro /boot
    echoerr "/boot successfully upgraded!"
    return 0
}

doModulesUpdate
[ $? -ne 0 ] && echoerr "Aborting upgrade" && cleanBeforeExit 10

doBootUpgrade
[ $? -ne 0 ] && echoerr "Aborting upgrade" && cleanBeforeExit 10

exit 0
