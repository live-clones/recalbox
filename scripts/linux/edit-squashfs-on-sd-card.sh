#!/bin/bash

# This script is a tool to easily make modifications inside a SquashFS file
# on a SD card where Recalbox is installed.
# It is particularly useful when trying to add some debugging to script shells
# without having to recompile and flash a Recalbox image every time.
#
# How it works:
#   1. it displays the list of SD-card partitions and prompts the user which one
#      contains the SquashFS file to edit
#   2. it mounts this partition in a temporary directory
#   3. it mounts the SquashFS file read-only (SquashFS cannot be mounted read-write)
#   4. it setups an overlayfs (to be able to change the read-only filesystem in an upper layer)
#   5. it spawns a shell where you can edit whatever you want in the "mounted" SquashFS
#   6. when closing the shell, the changes are saved back to the SquashFS file on the SD-card
#      (unless you exit the shell in error, which discards all your modifications)

set -e # exit on error

# Check some mandatory dependencies
if [[ $(id -u) != 0 ]]; then
  echo "This script requires root access to mount partitions. Please run it as root or using \`sudo\`."
  exit 1
fi
if ! which mksquashfs > /dev/null; then
  echo "Error: \`mksquashfs\` is missing. Please install it and try again."
  exit 1
fi
if ! modinfo overlay &> /dev/null; then
  echo "Error: OverlayFS kernel module (named \`overlay\`) is missing. Please load it and try again."
  exit 1
fi

# Display SD card partition table and ask which one contains the SquashFS
# file to edit (its path inside the partition must `boot/recalbox`)
echo -e "Here is a list of partitions on your SD card(s):\n"
lsblk -noheadings -o KNAME,FSTYPE,SIZE,LABEL /dev/mmcblk* | sort | uniq | sed 's|^|/dev/|'
echo
while [[ ! -b ${containerPartition} ]]; do
  read -p "Which SD-card partition contains the SquashFS file you want to edit? " containerPartition
done

# Create temporary files and directories, and declare some variables
mnt=$(mktemp -d)
mkdir -p ${mnt}/{container,squashfs,upper,temp,merged}
oldSquashfsFile="${mnt}/container/boot/recalbox"
newSquashfsFile="${mnt}/new.squashfs"

# Mount SD-card partition that contains the SquashFS file
mount ${containerPartition} ${mnt}/container

if [[ ! -f ${oldSquashfsFile} ]]; then
  echo "Error: SquashFS file not found on this SD-card partition."
  umount ${mnt}/container
  exit 1
fi

# Mount SquashFS file (read-only) and OverlayFS
mount -t squashfs -o loop ${oldSquashfsFile} ${mnt}/squashfs
mount -t overlay -o lowerdir=${mnt}/squashfs,upperdir=${mnt}/upper,workdir=${mnt}/temp overlay ${mnt}/merged

# Spawn a shell for user to make changes
echo -e "\033[0;33mYou are now virtually inside the SquashFS file.
When you close this shell (Ctrl+D or \`exit\`), the SquashFS file will be re-generated with your changes on your SD card.
To quit without saving your changes, use \`exit 1\`.\033[0m"
if (cd ${mnt}/merged; bash); then
  # Create new SquashFS file
  mksquashfs ${mnt}/merged ${newSquashfsFile} -noappend
fi

# Unmount OverlayFS and SquashFS
umount ${mnt}/{merged,squashfs}

# Replace old SquashFS with the newly-generated one on the SD-card (if it was created)
if [[ -f ${newSquashfsFile} ]]; then
  mv ${newSquashfsFile} ${oldSquashfsFile}
fi

# Unmount SD-card partition
umount ${mnt}/container

# Cleanup temp directories
rm -rf ${mnt}
