#!/bin/bash

# Don't play animation when a special case is installing!
if [[ -f /tmp/.install.png ]] ; then
	exit 0
fi

touch /tmp/newshare

dd if=/dev/zero of=/dev/fb0 > /dev/null 2>&1
FILES=/recalbox/system/resources/creating-share*.jpg
while true ; do
  for f in $FILES
  do
    fbv -f -i $f &
    sleep 0.1
  done
done
exit 0
