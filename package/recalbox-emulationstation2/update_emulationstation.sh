#!/bin/bash

if test "$1" != "start"
then
  exit 0
fi

ARCH=$(cat /recalbox/recalbox.arch)
ESUPDATESOURCEDIR=/recalbox/share/system
ESUPDATESOURCEFILE="$ESUPDATESOURCEDIR/emulationstation-recalbox-$ARCH"
ESUPDATEDESTINATIONDIR=/usr/bin
ESUPDATEDESTINATIONFILE="$ESUPDATEDESTINATIONDIR/emulationstation"
INIT_SCRIPT=$(basename "$0")

if [[ -f $ESUPDATESOURCEFILE ]]; then
  recallog -s "${INIT_SCRIPT}" -t "UPGRADE" -f upgrade.log "EmulationStation update found! moving ${ESUPDATESOURCEFILE} to ${ESUPDATEDESTINATIONFILE}"
  mount -o remount,rw /
  mv ${ESUPDATESOURCEFILE} ${ESUPDATEDESTINATIONFILE}
  chmod +x ${ESUPDATEDESTINATIONFILE}
  mount -o remount,ro /
fi
