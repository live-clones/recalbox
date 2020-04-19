#!/bin/bash

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
