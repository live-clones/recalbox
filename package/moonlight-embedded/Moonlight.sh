#!/bin/bash
SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`
source "${SCRIPTPATH}/moonlight.inc.sh"
source "${SCRIPTPATH}/generic.inc.sh"
#source ./moonlight.inc.sh
#source ./generic.inc.sh

mlGameList=$moonlightConf/gamelist.txt
mlRomDir=/recalbox/share/roms/moonlight

function mlFindGfeHosts {
  tmpOut=/tmp/moonlightHosts
  echo "Listing available GFE servers :"
  getNvServers > $tmpOut
  while read line
  do
    info=`getNvServerInfo $(echo $line | cut -d ';' -f 2)`
    parseServerInfo $info
    rm $info
  done < $tmpOut
  
  nbHosts=$(cat $tmpOut | wc -l)
  if [ "$nbHosts" -gt "0" ]; then
    echo "You can now run $0 pair <host>"
    echo "<host> can be empty (not recommended if you have several GFE hosts), an IP or a PC name"
  else
    echo "mlFindGfeHosts() : No GFE host was found" >&2
  fi
  rm $tmpOut
}

function mlPair {
  # Check $1 : IP or parameter or empty
  # and fill up internal variables
  if [ -z "$1" ]; then
    # YOLO mode ! the First host that answers is the winner !
    mlIp=""
    mlHost=""
    mlConf="$moonlightConf/moonlight.conf"
    mlKeydir="$moonlightConf/keydir"
  else
    isIp "$1" > /dev/null
    if [ $? = 0 ]; then
      # IP mode : means the user may want to play through WAN, so don't look for any hostname
      echo "IP mode"
      mlIp="$1"
      mlHost="$1"
      mlConf="$moonlightConf/moonlight-$mlIp.conf"
      mlKeydir="$moonlightConf/keydir-$mlIp"
    else
      #HOSTNAME mode
      echo "Hostname mode"
      mlHost="$1"
      mlIp=$(avahiToIp "$1")
      mlConf="$moonlightConf/moonlight-$mlHost.conf"
      mlKeydir="$moonlightConf/keydir-$mlHost"
    fi
  fi
        
  echo "$mlHost($mlIp) $mlConf | $mlKeydir"
  
  # remove existing keydir for the host
  rm -rf $mlKeydir
  
  # We're all set, time to pair ! Exit if failed
  moonlight pair -keydir $mlKeydir $mlIp 
  [ $? != 0 ] && { echo "ERROR mlPair() : could not pair. Exiting ... " ; exit 1 ; }
  
  if [ -z "$1" ]; then
    echo "YOLO MODE !!!"
  else
    # Output some cool stuff
    info=$(getNvServerInfo $mlIp)
    parseServerInfo $info
    rm $info
    # Create the moonlight conf
    rm $mlConf 2>/dev/null
    cp /recalbox/share_init/system/configs/moonlight/moonlight.conf $mlConf
    sed -i "s+.*address =+address = $mlIp+" $mlConf 
    sed -i "s+.*keydir =.*+keydir = $mlKeydir+" $mlConf 
  fi
}

function mlInit {
  # same as mlPair :
  # Check $1 : IP or parameter or empty
  # and fill up internal variables
  hostOrIP="$1"
  if [ -z "$hostOrIP" ]; then
    # YOLO mode ! the First host that answers is the winner !
    echo "YOLO Mode"
    mlIp=$($0 find | grep 'running GFE' | cut -d '(' -f 2 | cut -d ')' -f 1)
    [ -z $mlIp ] && { echo "ERROR mlInit() : no GFE host found, or $hostOrIp was not yet paired. Exiting ... " ; exit 1 ; }
    mlConf="$moonlightConf/moonlight.conf"
    mlKeydir="$moonlightConf/keydir"
  else
    # isIp "$hostOrIP" > /dev/null
    # if [ $? = 0 ]; then
    if isIp "$hostOrIP" ; then
      # IP mode : means the user may want to play through WAN, so don't look for any hostname
      echo "IP mode"
      mlIp="$hostOrIP"
      mlConf="$moonlightConf/moonlight-$mlIp.conf"
      mlKeydir="$moonlightConf/keydir-$mlIp"
    else
      #HOSTNAME mode
      echo "Hostname mode"
      mlIp=$(avahiToIp "$hostOrIP")
      mlConf="$moonlightConf/moonlight-$hostOrIP.conf"
      mlKeydir="$moonlightConf/keydir-$hostOrIP"
    fi
  fi
  
  # Let's make sure the required files exist
  if [[ ! -f $mlConf || ! -d $mlKeydir ]] ; then
    echo "$mlConf or $mlKeyDir doesn't exist. Have you added the right arguments to the script ?"
    exit 1
  fi
  # cleanup previous existing host with the samename
  mlClean "$hostOrIP"
  
  # Get games list, time to create roms and scraping data
  xmlGames=$(getNvList $mlIp $mlKeydir)
  [ ! -f $mlRomDir/gamelist.xml ] && echo -e "<?xml version=\"1.0\"?>\n<gameList />" > $mlRomDir/gamelist.xml
  while read line
  do
    gameId=$(echo $line | cut -d "$moonlightSeparator" -f 1)
    gameShortName=$(echo $line | cut -d "$moonlightSeparator" -f 2)
    gameLongName=$(echo $line | cut -d "$moonlightSeparator" -f 3)
    romFileName=$(gameShortNameToFileName $gameShortName $hostOrIP)
    [ ! -z "$hostOrIP" ] && romFileName="${hostOrIP}/${romFileName}"
    [ ! -z "$hostOrIP" -a ! -d "$mlRomDir/${hostOrIP}" ] && mkdir -p "$mlRomDir/${hostOrIP}"
    echo "Adding and scraping $gameLongName ..."
    # Create ROM
    touch $mlRomDir/$romFileName
    # Add its entry in the gamelist.txt
    romFileNoExt=$(basename "$romFileName" "$mlExtension")
    echo "$romFileNoExt$moonlightSeparator$gameLongName$moonlightSeparator$mlConf" >> $mlGameList
    # Add scraping info
    if [ -z "$hostOrIP" ]; then romDisplayName="$gameLongName" ; else romDisplayName="$gameLongName ($hostOrIP)" ; fi
    img=`mlScrape "$mlIp" "$mlKeydir" "$gameId" "$hostOrIP" "$gameShortName"`
    # Get missing data from TGDB
    players="`queryTGDB "$gameLongName" "players"`"
    rating="`queryTGDB "$gameLongName" "rating"`"
    releasedate="`queryTGDB "$gameLongName" "releasedate"`"
    developer="`queryTGDB "$gameLongName" "developer"`"
    publisher="`queryTGDB "$gameLongName" "publisher"`"
    genre="`queryTGDB "$gameLongName" "genre"`"
    desc="`queryTGDB "$gameLongName" "desc"`"
    xml ed --inplace -s "/gameList" -t elem -n "game" \
                     -i '/gameList/game[not(@id)]' -t 'attr' -n 'id' -v "$gameId"\
                     -s "/gameList/game[@id='$gameId']" -t elem -n "name" -v "$romDisplayName" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "path" -v "./$romFileName" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "players" -v "$players" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "rating" -v "$rating" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "releasedate" -v "$releasedate" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "developer" -v "$developer" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "publisher" -v "$publisher" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "genre" -v "$genre" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "desc" -v "$desc" \
                     -s "/gameList/game[@id='$gameId']" -t elem -n "image" -v "$img/$gameShortName.jpg" $mlRomDir/gamelist.xml
    
  done < <(parseAppList $xmlGames)
}

function mlScrape {
  host="$1"
  keydir="$2"
  gameId="$3"
  hostOrIp="$4"
  gameShortName="$5"
  
  if [[ -z $hostOrIp ]]; then
    longImgPath="${mlRomDir}/downloaded_images"
    shortImgPath='./downloaded_images'
  else
    longImgPath="${mlRomDir}/downloaded_images/${hostOrIp}"
    shortImgPath="./downloaded_images/${hostOrIp}"
  fi
  
  [ ! -d "$longImgPath" ] && mkdir -p $longImgPath
  
  # Ask GFE the game cover
  getNvImage "$host" "$keydir" "$gameId" "${longImgPath}/${gameShortName}.jpg"
  echo "$shortImgPath"
  # Too bad, No pic from GFE. Ask GDB
}

queryTGDB () {
  # $1 is the gamename
  # $2 is the data we need
  gamename="$1"
  element="$2"
  GDBURL="http://thegamesdb.net/api/GetGame.php?platform=pc&exactname="
  IMGPATH="${mlRomDir}/downloaded_images"

  # Test if $GDBURL is online, and stop if it's offline
  dbdns=$(echo $GDBURL | awk -F/ '{print $3}')
  ping -c 1 $dbdns > /dev/null 2>&1
  if [ $? -ne '0' ]
  then
    echo "$dbdns is not online. Can't scrape" >&2
    return 1
  fi
  
  # Get the real game name, not the moonlight link + prepare xml game data
  xmlfilename="/tmp/${1}.xml"
  
  # download XML game data from TheGamesDB.net only if it was not downloaded yet (TGDB is sluggish at times)
  [[ ! -f "$xmlfilename" ]] && wget "$GDBURL$gamename" -O "$xmlfilename" >/dev/null 2>&1

  case "$element" in
    rating)
      data=$(xml sel -t -v "Data/Game/Rating" "$xmlfilename" 2>/dev/null)
      ;;
    releasedate)
      data=$(xml sel -t -v "Data/Game/ReleaseDate" "$xmlfilename" 2>/dev/null | sed 's/^\([0-9]\{2\}\)\/\([0-9]\{2\}\)\/\([0-9]\{4\}\)/\3\1\2T0000/')
      ;;
    developer)
      data=$(xml sel -t -v "Data/Game/Developer" "$xmlfilename" 2>/dev/null)
      ;;
    publisher)
      data=$(xml sel -t -v "Data/Game/Publisher" "$xmlfilename" 2>/dev/null)
      ;;
    genre)
      data=$(xml sel -T -t -m "Data/Game/Genres/genre" -v 'text()' -i 'not(position()=last())' -o ' / ' "$xmlfilename" 2>/dev/null)
      ;;
    players)
      data=$(xml sel -t -v "Data/Game/Players" "$xmlfilename" 2>/dev/null)
      ;;
    desc)
      data="$(xml sel -t -v "Data/Game/Overview" "$xmlfilename" 2>/dev/null)"
      ;;
    image)
      extension=$(echo $imgurl | awk -F . '{print $NF}')
      img=$IMGPATH/${gamename}.${extension}
      imgurl=$(xml sel -t -v "Data/baseImgUrl" -v "Data/Game/Images/boxart[@side='front']/@thumb" "$xmlfilename" 2>/dev/null)
      wget $imgurl -O "$img" >/dev/null 2>&1
      ;;
    *)
      echo "$element can't be scrapped from TGDB" >&2
      return 1
  esac
  echo "$data"
}

function mlClean {
  # remove all existing data concerning a GFE host
  host="$1"
  fullmode="$2"
  
  if [ -z $host ]; then
    conf="$moonlightConf/moonlight.conf"
    keydir="$moonlightConf/keydir"
  else
    conf="$moonlightConf/moonlight-$host.conf"
    keydir="$moonlightConf/keydir-$host"
  fi
  
  # Remove keydir, moonlight.conf in fullmode only. Sometimes, we just to remove games and scraping data, not the conf and keydir
  if [ "$fullmode" == "fullmode" ]; then
    rm $conf 2>/dev/null
    rm -rf $keydir 2>/dev/null
  fi
  
  ls ${mlRomDir}/*_$host.moonlight 2>/dev/null | while read rom; do
    shortRom=$(basename "$rom")
    # Remove rom
    rm "$rom"  
    
    # remove gamelist.txt entries
    grep -v "${moonlightSeparator}$shortRom$" $mlGameList > $mlGameList
    
    # remove scraping data
    hostInGamelist="$shortRom"
    # echo $hostInGamelist
    xml ed --inplace -d "//game[path[contains(text(),'$hostInGamelist')]]" $mlRomDir/gamelist.xml

  done
}

#
# Main section
#

case $1 in
  find)
    mlFindGfeHosts
    ;;
  pair)
    shift
    mlPair "$1"
    ;;
  init)
    shift
    mlInit "$1"
    ;;
  list)
    moonlight list -keydir "${moonlightConf}/keydir" # Wrong, should handle an argument
    ;;
  clean)
    shift
    mlClean "$1" "fullmode"
    echo 'You can now pair again your recalbox with a PC'
    ;;
  *)
    echo "Unknown option $1" >&2
esac
