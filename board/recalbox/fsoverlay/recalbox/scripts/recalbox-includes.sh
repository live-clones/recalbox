#!/bin/bash

#
## This is a kind of header file that can be used to declare variables and functions
## That could turn useful in any recalbox script
#

#
## Variables
#

_RBX=/recalbox
_SHAREINIT=$_RBX/share_init
_SHARE=$_RBX/share

#
## Functions
#
INIT_SCRIPT=$(basename "$0")

function shouldUpdate {
  rbxVersion=$_RBX/recalbox.version
  curVersion=$_SHARE/system/logs/lastrecalbox.conf.update
  diff -qN "$curVersion" "$rbxVersion" 2>/dev/null && return 1
  return 0
}
# Checks if $1 exists in the array passed for $2
function containsElement {
  # local e
  # for e in "${@:2}"; do [[ "$e" == "$1" ]] && return 0; done
  [[ "${@:2}" =~ "$1" ]] && return 0
  return 1
}

function doRecalboxUpgrades {
  if ! shouldUpdate;then
    recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -e "No need to upgrade configuration files" && return 0
  fi
  doRbxConfUpgrade
  upgradeConfiggen
  upgradeTheme
}

# Upgrade the recalbox.conf if necessary
function doRbxConfUpgrade {
  # Update recalbox.conf
  rbxVersion=$_RBX/recalbox.version
  curVersion=$_SHARE/system/logs/lastrecalbox.conf.update

  # Check if an update is necessary
  if ! shouldUpdate;then
    recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -e "recalbox.conf already up-to-date" && return 0
  fi
  cfgIn=$_SHAREINIT/system/recalbox.conf
  cfgOut=$_SHARE/system/recalbox.conf
  forced=(controllers.ps3.driver) # Used as a regex, need to escape .
  savefile=${cfgOut}-pre-$(cat $rbxVersion | sed "s+[/ :]++g")
  tmpFile=/tmp/recalbox.conf

  recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -e "recalbox.conf to $(cat $rbxVersion)"
  cp $cfgIn $tmpFile || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -e "ERROR : Couldn't copy $cfgIn to $tmpFile" ; return 1 ; }

  while read -r line ; do
    name=`echo $line | cut -d '=' -f 1`
    value=`echo $line | cut -d '=' -f 2-`
    # echo "$name => $value"
    # Don't update forced values
    if containsElement $name $forced ; then
      recallog -s "${INIT_SCRIPT}" -t "UPGRADE" "FORCING : $name=$value"
      continue
    fi

    # Check if the property exists or has to be added
    if grep -qE "^[;]?$name=" $cfgIn; then
      recallog -s "${INIT_SCRIPT}" -t "UPGRADE" "ADDING user defined to $cfgOut : $name=$value"
      sed -i s$'\001'"^[;]\?$name=.*"$'\001'"$name=$value"$'\001' $tmpFile || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE-ERROR" "Couldn't replace $name=$value in $tmpFile" ; return 1 ; }
    else
      recallog -s "${INIT_SCRIPT}" -t "UPGRADE" "ADDING custom property to $cfgOut : $name=$value"
      echo "$name=$value" >> $tmpFile || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE-ERROR" "Couldn't write $name=$value in $tmpFile" ; return 1 ; }
    fi

  done < <(grep -E "^[[:alnum:]\-]+\.[[:alnum:].\-]+=[[:print:]]+$" $cfgOut)

  cp $cfgOut $savefile || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE-ERROR" -e "Couldn't backup $cfgOut to $savefile" ; return 1 ; }
  rm -f $cfgOut
  mv $tmpFile $cfgOut || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE-ERROR" -e "Couldn't apply the new recalbox.conf" ; return 1 ; }
  cp "$rbxVersion" "$curVersion" || { recallog -s "${INIT_SCRIPT}" -t "UPGRADE-ERROR" -e "Couldn't set the new recalbox.conf version" ; return 1 ; }
  recallog -s "${INIT_SCRIPT}" -t "UPGRADE" "UPGRADE done !"
}

function upgradeConfiggen {
  NEW_VERSION=$(sed -rn "s/^\s*([0-9a-zA-Z.]*)\s*.*$/\1/p" /recalbox/recalbox.version)
  python -c "import sys; sys.path.append('/usr/lib/python3.9/site-packages/configgen'); from emulatorlauncher import config_upgrade; config_upgrade('$NEW_VERSION')"
}

function upgradeTheme {
  /recalbox/scripts/recalbox-themes.sh
}

function upgradeRetroarchCoreNames {
  OUTPUT=/recalbox/system/resources/retroarch.corenames
  [[ "$1" == "-f" ]] && OUTPUT="$2"
  rm "$OUTPUT" 2>/dev/null

  for file in /usr/lib/libretro/*.so ; do
    rbxsystem=`basename $file | sed -E "s%(.*)_libretro.so%\1%"`
    coreinfos=`/usr/bin/nanoarch $file 2>/dev/null`
    coreName=`echo $coreinfos | cut -d ';' -f 1`
    coreVersion=`echo $coreinfos | cut -d ';' -f 2`
    echo "$coreName;$rbxsystem;$coreVersion" >> "$OUTPUT"
  done
}

function fuseC2 {
  BL1="/boot/bl1.bin.hardkernel"
  UBOOT="/boot/u-boot.bin"

  # Make sure the required files exist
  if [[ ! -f $BL1 || ! -f $UBOOT ]] ; then
    recallog "ERROR: couldn't update U-Boot as some files were missing, aborting"
    return 1
  fi

  # Find the device corresponding to the card
  bootDev=`/recalbox/scripts/recalbox-part.sh boot`
  diskDevice=`/recalbox/scripts/recalbox-part.sh prefix "$bootDev"`

  if ! echo "$diskDevice" | grep -q "^/dev/" ; then
    recallog "ERROR: could not find the device of /boot, aborting"
    return 1
  fi

  # We're all set !
  recallog "Updating U-Boot ..."
  dd if=$BL1   of="$diskDevice" conv=fsync,notrunc bs=1   count=442 && \
  dd if=$BL1   of="$diskDevice" conv=fsync,notrunc bs=512 skip=1 seek=1 && \
  dd if=$UBOOT of="$diskDevice" conv=fsync,notrunc bs=512 seek=97 && \
  recallog "SUCCESS: U-boot has been updated" && return 0

  # if we're here, something failed in the dd
  recallog "ERROR: couldn't update U-Boot"
  return 1
}
