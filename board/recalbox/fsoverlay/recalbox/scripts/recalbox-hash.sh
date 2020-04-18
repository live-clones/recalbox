#!/bin/bash

function crc32ZIP() {
    crc=`unzip -vl "$1" | egrep -o " [0-9a-f]{8} " | tr -d ' '`
    #~ unzip -vl "$1" | tail -n 3 | head -n 1 | tr -s ' ' | cut -d ' ' -f 2
    #~ unzip -p "$1" | python -c 'import sys;import zlib;print("%08x"%(zlib.crc32(sys.stdin.read())%(1<<32)))'
    echo ${crc^^}
}

function crc327ZIP() {
    #~ crc=`7zr l -slt "$1" | grep CRC | sed "s/.*\([0-9A-F]\{8\}\)$/\1/g"`
    crc=`7zr l -slt "$1" | grep CRC | egrep -o " [0-9a-f]{8} " | tr -d ' '`
    echo ${crc^^}
}

function crc32() {
    filename=`basename "$1"`
    crc=`7zr h "$1" | grep -F "$filename" | cut -d ' ' -f 1`
    echo ${crc^^}
}

# $1 = /path/to/file.ext
function hashFile () {
    filename=`basename "$1"`
    extension="${filename##*.}"
    # Convert to lowercase
    extension="${extension,,}"
    
    case $extension in
	zip)
	    crc32ZIP "$1"
	    ;;
	7z)
	    crc327ZIP "$1"
	    ;;
	*)
	    crc32 "$1"
	    ;;
    esac
}

#system=$1
#rom=$2
# Don't hash handhelds, CD, arcade
forbiddenSystems='3do amigacd32 amigacdtv atomiswave daphne dreamcast fba fbneo gamecube gamegear gb gba gbc lynx mame n64 naomi naomigd neogeo neogeocd ngp ngpc palm pcenginecd pcfx pokemini psp psx saturn scummvm segacd wii wswan wswanc'
forbiddenExtensions='m3u'
pRoms=/recalbox/share/roms
hashFilePath=/recalbox/share/system/configs/hashes
testMode=

while [[ $# -ge 1 ]]
do
key="$1"
case $key in
    -s)
    system="$2"
    shift
    ;;
    -r)
    rom="$2"
    shift
    ;;
    -f)
    file="$2"
    shift
    ;;
    -t)
    testMode="Y"
    ;;
esac
shift
done

# 2 possible modes
# system only -> parse all roms
# system + rom subpath/romname -> just hash the rom

if [[ ! -z "$testMode" && ! -z "$system" ]] ; then
    [[ $forbiddenSystems = *"$system"* ]] && exit 1 || exit 0
fi

# Make sure the system is not a forbidden one, return an empy hash
if [[ ! -z "$system" && $forbiddenSystems = *"$system"* ]] ; then
    echo "$system is not allowed for hashing" >&2
    echo
    exit 1
fi

# Make sure the system folder exists
if [[ ! -z $system && ! -d "$pRoms/$system" ]] ; then
    echo "$system is not an existing system" >&2
    exit 1
fi

# If a rom is specified, make sure system was passed
if [[ ! -z $rom && -z $system ]] ; then
    echo "You can't hash a rom using -r without its system" >&2
    exit 1
fi

# If a rom is specified, make sure the rom exists
if [[ ! -z $rom && ! -f "$pRoms/$system/$rom" ]] ; then
    echo "$pRoms/$system/$rom does not an exist" >&2
    exit 1
fi

[[ ! -d $hashFilePath ]] && mkdir -p $hashFilePath
systemHashFile="$hashFilePath/$system.hashes"

# we're good to go !
# system+rom mode first, much shorter
if [[ ! -z "$rom" ]] ; then
    hashFile "$pRoms/$system/$rom"
    exit $?
fi

# Just hash a file
if [[ ! -z "$file" ]] ; then
    folder=`dirname "$file"`
    hashFile "$file"
    exit $?
fi

# Hash a whole system
# First, get the system extensions from the es_systems.cfg
knownExtensions=`xmllint -xpath "string(/systemList/system/extension[../name=\"$system\"])" /recalbox/share_init/system/.emulationstation/es_systems.cfg`

# This block works fine but makes it hard to parse in a for loop because of spaces in file names
#~ lsexts=`echo $knownExtensions | tr -d ' ' | tr -s "." "," | cut -c 2-`
#~ oldIFS="$IFS"
#~ for file in `eval "ls \"$pRoms/$system/\"*.{$lsexts} 2>/dev/null"` ; do echo $file ; done
#~ IFS="$oldIFS"
#

# Other method using for file in path/*.ext1 path/*.ext2
lsexts="$knownExtensions"
for ext in `echo $lsexts` ; do 
    find "$pRoms/$system" -name "*$ext" | while read file ; do
        romHash=`hashFile "$file"`
        echo $romHash `echo $file | sed "s+$pRoms/$system/++g"`
    done
done
