#!/bin/bash
SHARE=/recalbox/share
SAVELOG=/tmp/backup.log
LIST=""

# Cant | grep > file so let's cheat
function calcSavedFiles() {
    grep -E "^($list)" "$SAVELOG" | wc -l
}

# Show the progression while download
function cyclicProgression() {
    totalFiles="$1"
    extractedFiles=0
    while true ; do
        extractedFiles=$(calcSavedFiles)
	currentFile=`grep -E "^($list)" "$SAVELOG" | tail -n 1`
        echo -e "\e[1A\e[KSAVING: $(( 100 * extractedFiles / totalFiles ))% ($currentFile)"
        [[ "$extractedFiles" -ge  "$totalFiles" ]] && break
        sleep 0.1
    done
}

device="$1"
echo "VERIFYING: free space required..."
sizeToSave=`du -sk "$SHARE" | grep -oE "^[0-9]*"`
devId=`blkid | grep $device | cut -d ':' -f 1`
if [[ -z $devId ]] ; then
    echo -e "\e[1A\e[KCouldn't get the free size left on bacup device $device"
    exit 1
fi
freeSpace=`df -k | grep "$devId" | tr -s ' ' | cut -d ' ' -f 4`

if [[ $freeSpace -lt $sizeToSave ]] ; then
    echo -e "\e[1A\e[KVERIFYING: not enough space to save. Need at least ${sizeToSave}kB"
    exit 1
fi

# get the number of files to be saved
nbFiles=`find "$SHARE" | wc -l`

# list the parent directories to filter the sync script
list=""
for rep in `ls "$SHARE"` ; do
  list=`echo -n "${list}|$rep"`
done

# Remove heading |
LIST=${list:1}

cyclicProgression $nbFiles &
#Now launch the script
/recalbox/scripts/recalbox-sync.sh sync "$device" > "$SAVELOG" 

exit $?

