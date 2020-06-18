#!/bin/bash

# the only partition we know when booting from linux is the root device on which linux booted.
# by convention, the boot partition is determine from the root partition mounted by the kernel at boot time (it's the previous partition on the same disk)
# by convention, the share partition is the next partition on the same disk
# it cannot work if the root is an nfs device or an initramfs device, which is not supported by recalbox

# Recalbox has 3 partitions
# 1. the boot partition (contains system in a squashfs file)
# 2. the share partition (user data, created and maximized on 1st boot)
# 3. the overlay partition (created on 1st boot at the end of the SD-card/disk)
#
# from the root device partition the partitions can be determined
# the root partition is not always /dev/mmcblk0p1, mainly in case you boot from an usb stick or a hard drive

print_usage() {
  echo "${0} <command> [command_parameter]" >&2
  echo "  where <command> must be one of:"
  echo "    boot                 output partition mounted as /boot" >&2
  echo "    share                output partition that should be mounted as /share" >&2
  echo "    overlay              output partition that should be used as OverlayFS" >&2
  echo "    share_internal       [deprecated: use \"share\"] output partition that should be mounted as /share" >&2
  echo "    prefix <partition>   output <partition> disk (without the partition number)" >&2
  echo "    number <partition>   output <partition> x number on the disk" >&2
  echo "    previous <partition> output partition before <partition> on the same disk" >&2
  echo "    next <partition>     output partition after given <partition> on the same disk" >&2
  echo "    type <partition>     output <partition> type (primary or logical)" >&2
  echo "    extended <device>    output extended partition (if there is one) on <device>"
  exit 64
}

# determine_boot_part
#   → outputs the partition mounted as /boot
determine_boot_part() {
  df -P | grep -E '^.*? /boot' | cut -d ' ' -f 1
}

# determine_share_part
#   → outputs the partition that should be mounted as /share
determine_share_part() {
  local bootPartition=$(determine_boot_part)
  local bootPartitionNumber=$(determine_part_number "${bootPartition}")
  local sharePartitionNumber=$(( ${bootPartitionNumber} + 1 ))

  echo "${bootPartition}" | sed -e "s|${bootPartitionNumber}$|${sharePartitionNumber}|"
}

# determine_overlay_part
#   → outputs the partition that should be use by OverlayFS
determine_overlay_part() {
  local bootPartition=$(determine_boot_part)
  local bootPartitionNumber=$(determine_part_number "${bootPartition}")
  local overlayPartitionNumber=$(( ${bootPartitionNumber} + 2 ))

  echo "${bootPartition}" | sed -e "s|${bootPartitionNumber}$|${overlayPartitionNumber}|"
}

# determine_part_prefix <partition>
#   → outputs disk the given partition is on
#     sometimes, it's `pX`, sometimes just an `X`: http://www.tldp.org/HOWTO/Partition-Mass-Storage-Definitions-Naming-HOWTO/x160.html
#     /dev/mmcblk0p3 => /dev/mmcblk0
#     /dev/sda1      => /dev/sda
determine_part_prefix() {
  local partition="${1}"

  echo "${partition}" | sed -r 's/p?[0-9]$//'
}

# determine_part_number <partition>
#   → outputs partition number of the given partition
#     /dev/mmcblk0p3 => 3
#     /dev/sda1      => 1
#     /dev/sda       => [error]
determine_part_number() {
  local partition="${1}"
  local regex="^.*[^0-9]\([0-9]\+\)$"

  if echo "${partition}" | grep -q "${regex}"; then
    echo "${partition}" | sed -n "s|${regex}|\1|p"
  else
    echo "Error: '${partition}' does not end with numbers (it's probably not a disk partition)." >&2
    return 1
  fi
}

# determine_previous_part <partition>
#   → outputs previous partition (in order)
#     /dev/mmcblk0p3 => /dev/mmcblk0p2
#     /dev/sda2      => /dev/sda1
#     /dev/sda1      => [error]
determine_previous_part() {
  local partition="${1}"
  local partitionNumber=$(determine_part_number "${partition}")

  if [[ ${partitionNumber} -gt 1 ]]; then
    previousPartitionNumber=$(( ${partitionNumber} - 1 ))
    echo "${partition}" | sed "s|${partitionNumber}$|${previousPartitionNumber}|"
  else
    echo "Error: '${partition}' is the first partition on the disk" >&2
    return 1
  fi
}

# determine_next_part <partition>
#   → outputs next partition (in order)
#     /dev/mmcblk0p3 => /dev/mmcblk0p4
#     /dev/sda2      => /dev/sda3
determine_next_part() {
  local partition="${1}"
  local partitionNumber=$(determine_part_number "${partition}")
  local nextPartitionNumber=$(( ${partitionNumber} + 1 ))

  echo "${partition}" | sed "s|${partitionNumber}$|${nextPartitionNumber}|"
}

# determine_part_type <partition>
#   → outputs `primary` if given partition is a primary partition, or `logical` otherwise
#     we use MBR partition table, so:
#       - partitions 1-4 are primary (eventually extended)
#       - partitions 5+ are logical
determine_part_type() {
  local partition="${1}"
  local partitionNumber=$(determine_part_number "${partition}")
  local device=$(determine_part_prefix "${partition}")

  if [[ "${partitionNumber}" -le 4 ]]; then
    if [[ $(determine_extended_part "${device}") == "${partition}" ]]; then
      echo "extended"
    else
      echo "primary"
    fi
  else
    echo "logical"
  fi
}

# determine_extended_part <device>
#  → outputs the extended partition of <device>, if there is one
#    otherwise outputs nothing
determine_extended_part() {
  local device="${1}"

  if [[ -b "${device}" ]]; then
    local fdiskOutputLine=$(fdisk -l "${device}" 2> /dev/null | grep 'Extended$')
    [[ -n "${fdiskOutputLine}" ]] \
      && echo "${fdiskOutputLine}" | cut -d' ' -f1 \
      || return 1
  else
    echo "Error: '${device}' is not a block device" >&2
    return 1
  fi
}

case "${1}" in
  "boot") determine_boot_part;;
  "share"|"share_internal") determine_share_part;;
  "overlay") determine_overlay_part;;
  "prefix") determine_part_prefix "$2";;
  "number") determine_part_number "$2";;
  "previous") determine_previous_part "$2";;
  "next") determine_next_part "$2";;
  "type") determine_part_type "$2";;
  "extended") determine_extended_part "$2";;
  *) print_usage;;
esac || exit 1

exit 0
