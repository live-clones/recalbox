Ww#!/bin/bash
recalboxupdateurl="http://archive.recalbox.com/updates/v1.0"
systemsetting="recalbox_settings"
recalboxFiles="boot.tar.xz root.tar.xz boot.tar.xz.sha1 root.tar.xz.sha1 root.list"

arch=$(cat /recalbox/recalbox.arch)
updatetype="`$systemsetting  -command load -key updates.type`"
upgradeDir="/recalbox/share/system/upgrade"
updateformat=`grep "^boot=" /boot/recalbox-boot.conf | cut -d "=" -f 2`

function calcDownloadedSize() {
    size=0
    for dlFile in ${filesToDownload} ; do
	# Skip unexisting files
        [[ ! -f "${upgradeDir}/${dlFile}" ]] && continue
	fileSize=`stat ${upgradeDir}/${dlFile} | grep "Size:" | tr -s ' ' | cut -d ' ' -f 3`
	size=$(($size + $fileSize))
    done
    echo $size
}

function cyclicProgression() {
    totalSize="$1"
    while true ; do
        sizeDownloaded=`calcDownloadedSize`
	MBDownloaded=$(( $sizeDownloaded / 1024 / 1024 ))
	MBTotal=$(( $totalSize / 1024 / 1024 ))
	echo -ne "\e[1A$MBDownloaded / $MBTotal MB ($(( 100 * $sizeDownloaded / $totalSize ))%)\n"
        [[ $sizeDownloaded -ge $1 ]] && break
	sleep 1
    done
}

function cleanBeforeExit {
  rm -rf "${upgradeDir}"/*
  exit $1
}

case "${updateformat}" in
  squashfs)
    filesForSize="recalbox.squashfs.xz boot.tar.xz"
    filesToDownload="boot.tar.xz recalbox.squashfs.xz boot.tar.xz.sha1 recalbox.squashfs.xz.sha1 recalbox.squashfs.size"
    filesToChecksum="boot.tar.xz recalbox.squashfs.xz"
    ;;
  *)
    filesForSize="root.tar.xz boot.tar.xz"
    filesToDownload="boot.tar.xz root.tar.xz boot.tar.xz.sha1 root.tar.xz.sha1 root.list"
    filesToChecksum="boot.tar.xz root.tar.xz"
    ;;
esac

if [[ "${updatetype}" == "beta" ]]
then
    # force a default value in case the value is removed or miswritten
    updatetype="stable"
fi

recallog "------------ Will process to a ${updatetype} upgrade ------------"
# Create download directory
if ! mkdir -p "${upgradeDir}"
then
    recallog -e "Unable to create upgrade directory"
    exit 1
fi

# Check sizes from header
size="0"
for file in $filesForSize; do
  url="${recalboxupdateurl}/${updatetype}/${arch}/${file}"
  headers=`curl -sfI ${url}`
  if [ $? -ne 0 ];then
    recallog -e "Unable to get headers for ${url}"
    exit 2
  fi
  filesize=`echo "$headers" | grep "Content-Length: " | grep -Eo '[0-9]+'`
  if [ $? -ne 0 ];then
    recallog -e "Unable to get size from headers ${url}"
    exit 3
  fi
  size=$(($size + filesize))
done
if [[ "$size" == "0" ]];then
  recallog -e "Download size = 0"
  exit 4
fi

sizeInBytes="${size}"
echo "Need $sizeInBytes"
size=$((size / 1024))
recallog "Needed size for upgrade : ${size}kb"

# Getting free space on share
freespace=`df -k /recalbox/share | tail -1 | awk '{print $4}'`
if [ $? -ne 0 ];then
  recallog -e "Unable to get freespace for /recalbox/share"
  exit 5
fi
diff=$((freespace - size))
if [[ "$diff" -lt "0" ]]; then
  recallog -e "Not enough space on /recalbox/share to download the update"
  exit 6
fi
recallog "Will download ${size}kb of files in ${upgradeDir} where ${freespace}kb is available. Free disk space after operation : ${diff}kb"

# Downloading files

# Start checking download progression
cyclicProgression "$sizeInBytes" &
progressionPid=$!
for file in $filesToDownload; do
  url="${recalboxupdateurl}/${updatetype}/${arch}/${file}"
  if ! curl -fs "${url}" -o "${upgradeDir}/${file}";then
    recallog -e "Unable to download file ${url}"
    kill -9 $progressionPid > /dev/null 2>&1
    cleanBeforeExit 7
  fi
  recallog "${url} downloaded"
done

# Verify checksums
for file in $filesToChecksum; do
  computedSum=`sha1sum /recalbox/share/system/upgrade/${file} | cut -d ' ' -f 1`
  buildSum=`cat /recalbox/share/system/upgrade/${file}.sha1 | cut -d ' ' -f 1`
  if [[ $computedSum != $buildSum ]]; then
    recallog -e "Checksums differ for ${file}. Aborting upgrade !"
    kill -9 $progressionPid > /dev/null 2>&1
    cleanBeforeExit 8
  fi
done

recallog -e "All files downloaded and checked, ready for upgrade on next reboot"
exit 0
