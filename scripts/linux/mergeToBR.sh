#!/bin/bash

hashSource="custom/list.hash"
if [[ $1 == "-h" ]] ; then
  echo "
This script considers the recalbox repo as a buildroot external tree
It will list the $hashSource file (which is the output of md5sum ) and patch a buildroot treee with it
The buildroot tree path must be specified through the BUILDROOT_DIR variable.
Example of use : BUILDROOT_DIR=~/git/buildroot/ scripts/linux/mergeToBR.sh
"
  exit -1
fi

####
echo "1. Doing basic checks ..."
####

if [[ ! -f $hashSource ]] ; then
  echo "$hashSource is missing" 
  exit 1
fi

if [[ -z "$BUILDROOT_DIR" ]] ; then
  echo "BUILDROOT_DIR is not set"
  exit 2
fi


if [[ ! -d "$BUILDROOT_DIR" || ! -d "$BUILDROOT_DIR/package" ]] ; then
  echo "$BUILDROOT_DIR is not a valid buildroot dir"
  exit 3
fi

####
echo "2. Compare source and destination ..."
####

foundError=0
# Make sure source and dest files match
while read line ; do
  #echo $line
  hash=$(echo $line | cut -d ' ' -f 1)
  file=$(echo $line | cut -d ' ' -f 2)

  # Check if source and dest file exist
  [[ ! -f custom/$file ]] && echo "Error: custom/$file doesn't exist" >&2 && foundError=1
  [[ ! -f $BUILDROOT_DIR/$file ]] && echo "Error: custom/$file doesn't exist" >&2 && foundError=1

  # Check if the buildroot file matches
  if [[ $hash != `md5sum $BUILDROOT_DIR/$file | cut -d ' ' -f 1` ]] ; then
    if diff -qN "custom/$file" "$BUILDROOT_DIR/$file" >/dev/null; then
      echo "file already patched: $file" >&2
    else
      echo "File doesn't have the expected md5: $file" >&2
      foundError=1
    fi
  fi
done < "$hashSource"

# Check that every source file is listed in the hash file
for file in `find custom/ -type f | grep -v 'custom/list.hash'` ; do
  file=`echo $file | sed 's+^custom/++'`
  ! grep -q "$file$" $hashSource && echo "Error: $file is not listed in $hashSource" >&2 && foundError=1
done

if [[ $foundError == 1 ]] ; then
  echo "Some errors were found. Can't patch buildroot. Abotring ..." >&2
  echo "No actions have been done, $BUILDROOT_DIR is still neat and clean" >&2
  exit 4
fi

####
echo "3. All checks done, merging trees ..."
####

foundError=0
# Ready to serve !
while read line ; do
  #echo $line
  hash=$(echo $line | cut -d ' ' -f 1)
  file=$(echo $line | cut -d ' ' -f 2)
  #! cp "custom/$file" "$BUILDROOT_DIR/$file" && echo "Error: copying custom/$file to $BUILDROOT_DIR/$file failed" && foundError=1 || echo "Copied: $file"
  if  cp "custom/$file" "$BUILDROOT_DIR/$file" ; then
    echo "Copied: $file"
  else
    echo "Error: copying custom/$file to $BUILDROOT_DIR/$file failed"
    foundError=1 
  fi
done < "$hashSource"

echo "Task completed !"
[[ $foundError == 1 ]] && echo "But there were some errors. Please check and correct them"

exit $foundError
