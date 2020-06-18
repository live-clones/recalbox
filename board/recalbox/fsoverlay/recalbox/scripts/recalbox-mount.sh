#!/bin/bash

# This script is a wrapper for `mount` that automtatically sets mount options for
# each filesystem type. It also performs an additional check after mount to ensure
# the filesystem is writeable (if not, it tries to repair it and mounts it again).

if [ $# -ne 4 ]; then
  echo "${0} <filesystemType> <readwriteRequired> <mountDevice> <mountPoint>" >&2
  exit 64
else
  filesystemType="$1"
  readwriteRequired="$2"
  mountDevice="$3"
  mountPoint="$4"
fi

loadDependencies() {
  case "${filesystemType}" in
    "exfat") modprobe fuse &> /dev/null ;;
  esac
}

mountOptions() {
  local defaultMountOptions="noatime"
  local windowsMask="fmask=0133,dmask=0022" # Correct access rights for NTFS/FAT/ExFAT (755 for directories, 644 for files)
                                            # (otherwise default would be 777 (umask=0))

  case "${filesystemType}" in
    "vfat") echo "${defaultMountOptions},${windowsMask},iocharset=utf8,flush" ;;
    "ntfs") echo "${defaultMountOptions},${windowsMask}" ;;
    "exfat") echo "${defaultMountOptions},${windowsMask},nonempty" ;;
    *) echo "${defaultMountOptions}"
  esac
}

performMount() {
  case "${filesystemType}" in
    "vfat") mount "${mountDevice}" "${mountPoint}" -o "$(mountOptions)" ;;
    "ntfs") mount.ntfs-3g "${mountDevice}" "${mountPoint}" -o "$(mountOptions)" ;;
    "exfat") mount.exfat "${mountDevice}" "${mountPoint}" -o "$(mountOptions)" ;;
    *) mount "${mountDevice}" "${mountPoint}" -o "$(mountOptions)" ;;
  esac || exit 1
}

exitSuccessfullyIfFilesystemIsWritable() {
  local testFile="${mountPoint}/recalbox.fsrw.test"

  if touch "${testFile}"; then
    rm "${testFile}"
    exit 0 # that ok ;-)
  fi
}

# Load FS-related required dependencies
loadDependencies

# Mount filesystem and check that the filesystem is writable
# (in some cases, even if asked read-write, it may be mounted read-only as a fallback if errors occured... usually with NTFS)
performMount && exitSuccessfullyIfFilesystemIsWritable

# ℹ️ At this point, the mounted filesystem is actually read-only, probably because it was un-cleanly un-mounted.

# Try to repair the filesystem
# (it doesn't work 100%... in the worst case, user will have to to plug their disk on a Windows environement to perform a native fsck)
umount "${mountPoint}" || exit 1
case "${filesystemType}" in
  "vfat") fsck.vfat -a "${mountDevice}" > /dev/tty0 ;; # output to terminal since it can take some time
  "ntfs") ntfsfix -d "${mountDevice}" ;;
  "exfat") fsck.exfat "${mountDevice}" > /dev/tty0 ;; # output to terminal since it can take some time
esac

# Try to mount filesystem and check if it's writeable, again
performMount && exitSuccessfullyIfFilesystemIsWritable

# ℹ️ At this point, even after repair, the filesystem couldn't be mounted read-write

# Discard the mount and exit with error if read-write was mandatory
if [ "${readwriteRequired}" = 1 ]; then
  umount "${mountPoint}"
  exit 1
fi

# Filesystem has been mounted read-only, it's probably unclean but it's acceptable
exit 0
