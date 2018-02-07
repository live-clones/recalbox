#!/bin/bash
mlExtension=".moonlight"

function isIp {
  [[ $1 =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]] && return 0
  return 1
}

function ipToAvahi {
  isIp $1 && avahi-resolve-address $1 | cut -f 2 && return 0
  return 1
}

function avahiHasLocal {
  return $(echo $1 | grep -q '.local$')
}

function avahiAddLocal {
  ! isIp $1 && ! avahiHasLocal $1 && echo "$1.local" && return 0
  return 1
}

function avahiToIp {
  hn=$1
  isIp $hn && return 1
  ! avahiHasLocal $hn && hn=$(avahiAddLocal $hn)
  avahi-resolve-host-name -4 $hn | cut -f 2 && return 0
}

function hostDisplay {
  echo "($1)"
}

function gameShortNameToFileName {
  echo "$1$(hostExtenstion $2)"
}

function hostExtenstion {
  echo "_$1$mlExtension"
}