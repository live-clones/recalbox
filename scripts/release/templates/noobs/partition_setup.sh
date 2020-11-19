#!/bin/sh

set -ex

[ -z "${part1}" ] && echo "Error: missing environment variable \${part1}" 1>&2 && exit 1
[ -z "${part2}" ] && echo "Error: missing environment variable \${part2}" 1>&2 && exit 1
[ -z "${part3}" ] && echo "Error: missing environment variable \${part3}" 1>&2 && exit 1

bootPartition="${part1}" # already mounted by NOOBS as `/mnt2` (best name ever!) when this script is executed
sharePartition="${part2}"
overlayPartition="${part3}"

# We can do any change on any partition here (only the first one is already mounted, though).
# For example, we used to modify `dev=` kernel parameter from `cmdline.txt` on `boot` partition
# (but we don't anymore since we now use `label=`).
