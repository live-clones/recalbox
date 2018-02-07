#!/bin/bash
moonlightConf=/recalbox/share/system/configs/moonlight
moonlightSeparator=";"

#############################################
#
#
# Low level functions
#
#
#############################################

# find keydir correponding to the IP
function getUniqueId {
  ip=$1
  
  confFile=$(grep -l "$ip" $moonlightConf/moonlight*.conf | head -n 1)
  
  # YOLO mode : no IP is written anywhere, so take the default file which should be set
  [ -z "$confFile" ] && confFile=$moonlightConf/moonlight.conf
  
  keydir=$(grep keydir $confFile | cut -d '=' -f 2 | tr -s ' ')
  echo "$keydir/uniqueid.dat"
}

# Returns the GFE URL depending on the protocol, the host and the command
function getURL {
  protocol=$1
  host=$2
  keydir="$3"
  command=$4
  
  # Generate a uuid
  uuid=$(cat /proc/sys/kernel/random/uuid)
  
  # Set URL depending on HTTPS or HTTP
  if [ "$protocol" == "https" ]
  then
    # Get NV unique id from pairing. If not paired, no need to continue
    [ ! -s $keydir/uniqueid.dat ] && { echo "ERROR getURL(): your recalbox is not yet paired with a GeForce Now compatible PC" >&2 ; exit 1 ; }
    uniqueId=$(cat $keydir/uniqueid.dat)
    echo "$protocol://$host:47984/$command?unique_id=$uniqueId&uuid=$uuid"
    return 0
  elif [ "$protocol" == "http" ]
  then
    echo "$protocol://$host:47989/$command?uuid=$uuid"
    return 0
  else
    echo "ERROR : unknown protocol used" >&2
    exit 1
  fi
}

# Defines the options for curl (except -o )
function getCurlOptions {
  protocol=$1
  host=$2
  keydir="$3"
  command=$4
  other=$5
  
  curlOptions="--insecure --silent --show-error"
  [ $protocol = "https" ] && curlOptions="$curlOptions --cert-type PEM --cert $keydir/client.pem --key-type PEM --key $keydir/key.pem"
  
  nvUrl=$(getURL $protocol $nvServer "$keydir" $nvCmd)
  [ ! -z "$nvOthers" ] && nvUrl="$nvUrl&$nvOthers"

  echo $curlOptions "$nvUrl"
  return 0
}

# Runs the curl command, and may eventually download a file
function doCurl {
  protocol=$1
  host=$2
  keydir="$3"
  command=$4
  other=$5
  downloadFile="$6"

  if [ -z "$downloadFile" ]
  then
    curl $(getCurlOptions $protocol $nvServer "$keydir" $nvCmd $nvOthers)
  else
    curl $(getCurlOptions $protocol $nvServer "$keydir" $nvCmd "$nvOthers") -o "$downloadFile" >/dev/null

    # Translate XML to UTF8
    filename=$(basename "$downloadFile")
    extension="${filename##*.}"
    [ "$extension" == "xml" ] && sed -i 's/encoding="UTF-16"/encoding="UTF-8"/' "$downloadFile"
  fi
}

# Trigger a HTTPS GFE service
function doHTTPSCurl {
  nvServer=$1
  keydir=$2
  nvCmd=$3
  nvOthers=$4
  downloadFile=$5

  doCurl https $nvServer "$keydir" $nvCmd "$nvOthers" "$downloadFile"
}

# Trigger a HTTP GFE service
function doHTTPCurl {
  nvServer=$1
  nvCmd=$2
  nvOthers=$3
  downloadFile=$4

  doCurl http $nvServer "" $nvCmd "$nvOthers" "$downloadFile"
}


# Get Gamestream compatible PCs
function getNvServers {
  nvAvahiService=_nvstream._tcp

  avahi-browse -prt $nvAvahiService | grep '^=.*IPv4' | cut -d ';' -f 4,8
}

# Get the list of available games
function getNvList {
  nvServer=$1
  keydir="$2"
  file="/tmp/$nvServer-applist.xml"
  doHTTPSCurl $nvServer $keydir applist "" "$file"
  echo $file
}

# Download the image of the appid
function getNvImage {
  nvServer=$1
  keydir="$2"
  nvAppId=$3
  nvFile=$4

  doHTTPSCurl $nvServer "$keydir" appasset "appid=$nvAppId&AssetType=2&AssetIdx=0" "$nvFile"
}

function getNvServerInfo {
  nvServer=$1
  file="/tmp/$nvServer-serverinfo.xml"
  doHTTPCurl $nvServer serverinfo "" "$file"
  echo $file
}

function parseServerInfo {
  xmlFile=$1
  xmlCmd="xml sel -t -v"

  [ ! -s $xmlFile ] && { echo "ERROR parseServerInfo() : couldn't find $xmlFile"  >&2 ; exit 1 ; }

  gfeHost=$($xmlCmd "root/hostname" "$xmlFile")
  gfeIP=$($xmlCmd "root/LocalIP" "$xmlFile")
  gfeVersion=$($xmlCmd "root/GfeVersion" "$xmlFile")
  gfeGPU=$($xmlCmd "root/gputype" "$xmlFile")

  echo "GFE Host $gfeHost($gfeIP) $gfeGPU running GFE $gfeVersion"
}

function parseAppList {
  xmlFile=$1
  xmlCmd="xml sel -t -v"

  [ ! -s $xmlFile ] && { echo "ERROR parseAppList() : couldn't find $xmlFile" >&2 ; exit 1 ; }

  #xml sel -t -c "root/App[AppTitle = 'Diablo III']/ID" applist-192.168.111.35.xml  

  while read line
  do
    # echo "  ++ Parsing game $line"
    appId=$($xmlCmd "root/App[AppTitle = '$line']/ID" $xmlFile)
    appShortName=$($xmlCmd "root/App[AppTitle = '$line']/ShortName" $xmlFile)
    # echo "    ++ Id : $appId"
    # echo "    ++ Short name : $appShortName"
    echo "$appId$moonlightSeparator$appShortName$moonlightSeparator$line"
  done < <( $xmlCmd "root/App/AppTitle" $xmlFile ; echo )
}

