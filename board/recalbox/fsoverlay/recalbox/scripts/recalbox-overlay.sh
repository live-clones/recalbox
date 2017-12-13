#!/bin/bash
mntRoot=/tmp/ofsrr
mntShareInternal=/tmp/ofssi

overlay_mount_real_partition() {
    partitionType="$1"
    mountPoint="$2"
    
    # Check if it's already mounted
    mount | grep -q "$mountPoint" && return 0

    case "$partitionType" in
        "system")
            partitionDevice=`/recalbox/scripts/recalbox-part.sh "$partitionType"`
            ;;
        "share_internal")
            partitionDevice=`/recalbox/scripts/recalbox-part.sh "$partitionType"`
            ;;
        *)
            return 1
            ;;
    esac

    [ -z "$partitionDevice" ] && return 1
    mkdir -p "$mountPoint" \
        && ( mount -r "$partitionDevice" "$mountPoint" 2>/dev/null && mount -o remount, rw "$mountPoint" || mount "$partitionDevice" "$mountPoint" ) \
        && echo $mountPoint \
        && return 0

    # This block is executed only if the previous commands failed
    umount "$mountPoint" 2>/dev/null
    return 1
}

overlay_umount_real_partition() {
    mountPoint="$1"
    mount | grep -q "$mountPoint" && umount "$mountPoint"
}

overlay_mount_real_root() {
    overlay_mount_real_partition "system" "$mntRoot"
}

overlay_umount_real_root() {
    overlay_umount_real_partition "$mntRoot"
}

overlay_mount_share_internal() {
    overlay_mount_real_partition "share_internal" "$mntShareInternal"
}

overlay_umount_share_internal() {
    overlay_umount_real_partition "$mntShareInternal"
}

overlay_reset_deleted_files () {
    #~ find ~/.overlay/upper/ -type c -exec rm -f {} \;
    return 1
}

overlay_reset_dupes() {
    # Remove files from the overlay that do exist on /
    return 1
}

overlay_reset() {
    #~ rm -rf $upperOverlay
    return 1
}

case "$1" in
    "mount_real_root")          overlay_mount_real_root ;;
    "umount_real_root")         overlay_umount_real_root ;;
    "mount_share_internal")     overlay_mount_share_internal ;;
    "umount_share_internal")    overlay_umount_share_internal ;;
    "reset_deleted_files")      overlay_reset_deleted_files ;;
    "reset")                    overlay_reset ;;
esac
