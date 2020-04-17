#!/bin/bash

UPDATEARCH=`cat /recalbox/recalbox.arch`
UPDATEFILE="recalbox-${UPDATEARCH}.img.xz"

FILES_TO_UPGRADE="${UPDATEFILE} ${UPDATEFILE}.sha1"
FILES_TO_CHECK="${UPDATEFILE}"
ADDITIONAL_PARAMETERS="?source=recalbox"

while [[ $# -gt 1 ]]
do
key="$1"
case $key in
    --upgrade-dir)
    UPGRADE_DIR="$2"
    shift
    ;;
    --upgrade-url)
    UPGRADE_URL="$2"
    shift
    ;;
    --arch)
    ARCH="$2"
    shift
    ;;
    *)
    ;;
esac
shift
done

if [[ -z ${UPGRADE_DIR} || -z ${UPGRADE_URL} || -z ${ARCH} ]]; then
    echo -e "Usage:\n$0 --upgrade-dir /path/to/tmp/dir --upgrade-url https://url-to-use.com --arch [rpi1|rpi2...]" && exit 1
fi

function calcDownloadedSize() {
    size=0
    for dlFile in ${FILES_TO_UPGRADE} ; do
        # Skip unexisting files
        [[ ! -f "${UPGRADE_DIR}/${dlFile}" ]] && continue
	    fileSize=$(stat ${UPGRADE_DIR}/${dlFile} | grep "Size:" | tr -s ' ' | cut -d ' ' -f 3)
        size=$((size + fileSize))
    done
    echo "${size}"
}

# Show the progression while download
function cyclicProgression() {
    totalSize="$1"
    while true ; do
        sizeDownloaded=$(calcDownloadedSize)
        if [ $sizeDownloaded -gt 0 ];then
            MBDownloaded=$(( sizeDownloaded / 1024 / 1024 ))
            MBTotal=$(( totalSize / 1024 / 1024 ))
            echoES "DOWNLOADED: ${MBDownloaded} / ${MBTotal} MB ($(( 100 * sizeDownloaded / totalSize ))%)"
        fi
        [[ ${sizeDownloaded} -ge $1 ]] && break
        sleep 1
    done
}

source /recalbox/scripts/upgrade/recalbox-upgrade.inc.sh

echoerr "------------ Will process to the recalbox upgrade from ${UPGRADE_URL} ------------"
# Create download directory
if ! mkdir -p "${UPGRADE_DIR}"
then
    echoerr -e "Unable to create upgrade directory"
    exit 1
fi

clean

# Check sizes from header
size="0"
for file in ${FILES_TO_UPGRADE}; do
  FILE_URL="${UPGRADE_URL}/${RECALBOX_URL}/${ARCH}/${file}"
  headers=$(curl -sfI "${FILE_URL}${ADDITIONAL_PARAMETERS}")
  if [[ "$?" != "0" ]];then
    echoerr "Unable to get headers for ${FILE_URL}"
    exit 2
  fi
  filesize=$(echo "${headers}" | grep "Content-Length: " | grep -Eo '[0-9]+')
  if [ $? -ne 0 ];then
    echoerr "Unable to get size from headers ${FILE_URL}"
    exit 3
  fi
  size=$((size + filesize))
done
if [[ "${size}" == "0" ]];then
  echoerr "Download size = 0"
  exit 4
fi

sizeInBytes="${size}"
echoerr "Need ${sizeInBytes}"
size=$((size / 1024))
echoerr "Needed size for upgrade : ${size}kb"

# Getting free space on UPGRADE_DIR
freespace=`df -k "${UPGRADE_DIR}" | tail -1 | awk '{print $4}'`
if [ $? -ne 0 ];then
  echoerr "Unable to get freespace for ${UPGRADE_DIR}"
  exit 5
fi
diff=$((freespace - size))
if [[ "$diff" -lt "0" ]]; then
  echoerr "Not enough space on ${UPGRADE_DIR} to download the update"
  exit 6
fi
echoerr "Will download ${size}kb of files in ${UPGRADE_DIR} where ${freespace}kb is available. Free disk space after operation : ${diff}kb"


# Start checking download progression
cyclicProgression "$sizeInBytes" &
progressionPid=$!
for file in ${FILES_TO_UPGRADE}; do
  FILE_URL="${UPGRADE_URL}/${RECALBOX_URL}/${ARCH}/${file}"
  if ! curl -fs "${FILE_URL}${ADDITIONAL_PARAMETERS}" -o "${UPGRADE_DIR}/${file}";then
    echoerr "Unable to download file ${FILE_URL}"
    cleanBeforeExit 7
  fi
  echoerr "${FILE_URL} downloaded"
done

kill -9 "${progressionPid}" > /dev/null 2>&1

# Verify checksums
for file in $FILES_TO_CHECK; do
  echoES "VERIFYING: $file CHECKSUM"
  computedSum=$(sha1sum "${UPGRADE_DIR}/${file}")
  [ $? -ne 0 ] && echoerr "Unable to calculate sha1sum for ${file}" && cleanBeforeExit 8
  computedSum=$(echo $computedSum | cut -d ' ' -f 1)
  buildSum=$(cat "${UPGRADE_DIR}/${file}.sha1")
  [ $? -ne 0 ] && echoerr "Unable to load sha1sum for ${file} (${file}.sha1)" && cleanBeforeExit 8
  buildSum=$(echo $buildSum | cut -d ' ' -f 1)
  if [[ $computedSum != $buildSum ]]; then
    echoerr "Checksums differ for ${file}. Aborting upgrade !"
    cleanBeforeExit 8
  fi
  echoerr "${file} checksum verified"
done

( exec /recalbox/scripts/upgrade/recalbox-squashfs-upgrade.sh )
if [ $? -ne 0 ];then
  echoerr "Failed to start upgrade"
  exit 6
fi

touch "${UPGRADE_DIR}/okforupgrade.go"
echoES "THE SYSTEM WILL NOW REBOOT"
exit 0

