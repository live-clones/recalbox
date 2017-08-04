#!/bin/bash

hashSource="custom/list.hash"
scriptName=$(basename $0 .sh)

if [[ $1 == "-h" ]] ; then
  echo "
This script considers the recalbox repo as a buildroot external tree
It will list the $hashSource file (which is the output of md5sum ) and patch a buildroot treee with it
The buildroot tree path must be specified through the BUILDROOT_DIR variable.
Example of use : BUILDROOT_DIR=~/git/buildroot/ scripts/linux/mergeToBR.sh
"
  exit -1
fi
declare -x foundError
####
echo -e "\e[7m>>> $scriptName 1. Doing basic checks ...\e[27m"
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
echo -e "\e[7m>>> $scriptName 2. Compare source and destination ...\e[27m"
####

foundError=0
# Make sure source and dest files match
while read -r line ; do
  # A hash made of * is to specify files that do not exist in buildroot and should be copied over
  hash=$(echo "$line" | tr -d '*' | cut -d ' ' -f 1)
  file=$(echo "$line" | cut -d ' ' -f 3)

  # If the hash is empty, make sure the dest file doesn't exist
  if [[ ! -z $hash ]] ; then
    # Check if source and dest file exist
    [[ ! -f custom/$file ]] && echo "Error: custom/$file doesn't exist" >&2 && foundError=1
    [[ ! -f $BUILDROOT_DIR/$file ]] && echo "Error: $BUILDROOT_DIR/$file doesn't exist" >&2 && foundError=1
  fi

  # Check if the buildroot file matches. We don't check the md5 dest file if the source has no md5 -> copy a new file from source to dest
  if [[ ! -z $hash && $hash != `md5sum $BUILDROOT_DIR/$file | cut -d ' ' -f 1` || -z $hash ]] ; then
    if diff -qN "custom/$file" "$BUILDROOT_DIR/$file" >/dev/null; then
      echo "file already patched: $BUILDROOT_DIR/$file" >&2
    else
      # For a pfile needing patch, we only patch if the dest file is EXACTLY the one expected
      [[ ! -z $hash ]] && echo "File doesn't have the expected md5: $file" >&2 && foundError=1
      # When adding a new file to buildroot, and the dest file is not the same as the original one, raise an error
      # The dest file can eventually exist if the script was already run before. But the dest file MUSTN'T be modified
      [[ -z $hash && -f $BUILDROOT_DIR/$file ]] && echo "File is not supposed to exist and differs from source: $BUILDROOT_DIR/$file" >&2 && foundError=1
    fi
  fi
done < <(grep -v '^#' $hashSource)

# Check that every source file is listed in the hash file
for file in `find custom/ -type f | grep -v 'custom/list.hash'` ; do
  file=`echo $file | sed 's+^custom/++'`
  ! grep -q "$file$" $hashSource && echo "Error: $file is not listed in $hashSource" >&2 && foundError=1
done

if [[ $foundError != 0 ]] ; then
  echo "Some errors were found. Can't patch buildroot. Abotring ..." >&2
  echo "No actions have been done, $BUILDROOT_DIR is still neat and clean" >&2
  exit 4
fi

####
echo -e "\e[7m>>> $scriptName 3. All checks done, merging trees ...\e[27m"
####

foundError=0
# Ready to serve !
grep -v '^#' $hashSource | while read line ; do
  #echo $line
  hash=$(echo "$line" | tr -d '*' | cut -d ' ' -f 1)
  file=$(echo "$line" | cut -d ' ' -f 3)
  #! cp "custom/$file" "$BUILDROOT_DIR/$file" && echo "Error: copying custom/$file to $BUILDROOT_DIR/$file failed" && foundError=1 || echo "Copied: $file"
  if  cp "custom/$file" "$BUILDROOT_DIR/$file" ; then
    echo "Copied: $file"
  else
    echo "Error: copying custom/$file to $BUILDROOT_DIR/$file failed"
    foundError=1 
  fi
done

echo -e "\e[7m>>> $scriptName 4. Task completed !\e[27m"
[[ $foundError == 1 ]] && echo "But there were some errors. Please check and correct them"

exit $foundError
