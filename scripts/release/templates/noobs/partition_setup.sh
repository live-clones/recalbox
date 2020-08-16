#!/bin/sh

set -ex

[ -z "${part1}" ] && echo "Error: missing environment variable \${part1}" 1>&2 && exit 1
[ -z "${part2}" ] && echo "Error: missing environment variable \${part2}" 1>&2 && exit 1
[ -z "${part3}" ] && echo "Error: missing environment variable \${part3}" 1>&2 && exit 1

bootPartition="${part1}" # already mounted by NOOBS as `/mnt2`, best name ever
sharePartition="${part2}"
overlayPartition="${part3}"

# Since NOOBS does not create partition like our downloadable image, we have to
# modify our kernel parameters to use a different partition to boot from. This
# is defined in `/cmdline.txt` (from the `rpi-firmware` package in Buildroot,
# that we patch in `custom`) and we set it to the value of the first partition,
# as given by NOOBS as ${part1}.
sed "/mnt2/cmdline.txt" -i -e "s|dev=/dev/[^ ]*|dev=${bootPartition}|"
