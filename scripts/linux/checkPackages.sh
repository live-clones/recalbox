#!/bin/bash

# Getting prepared for the external tree
[[ -z $BUILDROOT_DIR && -d buildroot ]] && BUILDROOT_DIR="./buildroot" || BUILDROOT_DIR=.
[[ -z $RECALBOX_DIR ]] && RECALBOX_DIR=.

# EMULATORS is compared to each package's repo
EMULATORS="advancemame amiberry citra dolphin-emu dosbox gsplus hypseus libretro-81 libretro-2048 libretro-atari800 libretro-beetle-lynx libretro-beetle-ngp libretro-beetle-pce-fast libretro-beetle-pcfx libretro-beetle-psx libretro-beetle-psx-hw libretro-beetle-saturn libretro-beetle-supergrafx libretro-beetle-vb libretro-beetle-wswan libretro-bluemsx libretro-boom3 libretro-cannonball libretro-cap32 libretro-craft libretro-crawl libretro-cheats libretro-citra libretro-crocods libretro-desmume libretro-dinothawr libretro-duckstation libretro-easyrpg libretro-ecwolf libretro-fbneo libretro-fceumm libretro-fceunext libretro-flycast libretro-fmsx libretro-freechaf libretro-freeintv libretro-fuse libretro-gambatte libretro-gearsystem libretro-genesisplusgx libretro-gpsp libretro-gw libretro-handy libretro-hatari libretro-imageviewer libretro-kronos libretro-lutro libretro-mame2000 libretro-mame2003 libretro-mame2003-plus libretro-mame2010 libretro-mame2015 libretro-mame2016 libretro-melonds libretro-mesen libretro-mesen-s libretro-meteor libretro-mgba libretro-minivmac libretro-mrboom libretro-mu libretro-mupen64plus libretro-mupen64plus-nx libretro-nestopia libretro-np2kai libretro-nxengine libretro-o2em libretro-opera libretro-parallel-n64 libretro-pcsx_rearmed libretro-picodrive libretro-pokemini libretro-prboom libretro-prosystem libretro-px68k libretro-quasi88 libretro-quicknes libretro-race libretro-reminiscence libretro-sameboy libretro-scummvm libretro-snes9x libretro-snes9x2002 libretro-snes9x2005 libretro-snes9x2010 libretro-stella libretro-stella2014 libretro-tgbdual libretro-theodore libretro-tic80 libretro-thepowdertoy libretro-tyrquake libretro-uae libretro-uzem libretro-vecx libretro-vice libretro-virtualjaguar libretro-vitaquake2 libretro-vitaquake3 libretro-vitavoyager libretro-xmil libretro-xrick libretro-yabasanshiro libretro-yabause linapple-pie moonlight-embedded mupen64plus-audio-sdl mupen64plus-core mupen64plus-gles2n64 mupen64plus-gles2rice mupen64plus-gliden64 mupen64plus-input-sdl mupen64plus-omx mupen64plus-rsphle mupen64plus-uiconsole mupen64plus-video-glide64mk2 mupen64plus-video-rice openbor oricutron pifba ppsspp reicast residualvm retroarch scummvm simcoupe solarus-recalbox vice"

# PACKAGES is compared to each package's repo
PACKAGES="rpi-firmware rpi-userland odroid-scripts odroid-mali"

# List Recalbox defconfigs
DEFCONFIGS=`ls configs/recalbox-* | cut -d "-" -f 2 | sed s+_defconfig++g`


#
# Get Github API Token
#
function getAPIToken () {
  [[ ! -f scripts/linux/.githubAPIToken && -z $GHUser && -z $GHKey ]] && return
  if [[ ! -z $GHUser && -z $GHKey || -z $GHUser && ! -z $GHKey ]] ; then
    echo "When specifying user or token from the command line, both must be set. Exiting ..." >&2
    exit 2
  fi
  [[ -f scripts/linux/.githubAPIToken && -z $GHUser && -z $GHKey ]] && source scripts/linux/.githubAPIToken
  CURLPARMS="-u ${GHUser}:${GHKey}"
}

#
# Build a github URL from a package _SITE
#
function getGitHubApiUrlFromPackage() {
  package=$1
  makeFile=`[[ -f "$BUILDROOT_DIR/package/$package/$package.mk" ]] && echo "$BUILDROOT_DIR/package/$package/$package.mk" || echo "$RECALBOX_DIR/package/$package/$package.mk"`
  elements=`grep "_SITE = " "$makeFile" | grep github  | grep -v "^#" | cut -d "=" -f 2 | cut -d "," -f 2,3`
  # Case when it's not a call github(...) but a git://github.com/...
  if echo "$elements" | grep -qE "github\.com" ; then
    # Just get the URL past github.com/ and convert / to , so that it's backward compatible with a call github method
    elements=`echo "$elements" | sed "s+^\(.*github.com\/\)++g" | sed "s+\/+,+g"`
  fi
  owner=`echo "$elements" | cut -d "," -f 1`
  project=`echo "$elements" | cut -d "," -f 2 | sed "s+\.git$++g"`
  [[ ! -z $owner && ! -z $project ]] && echo "https://api.github.com/repos/$owner/$project"
}

#
# Get emulator version in buildroot
#
function getCommitIdFromPackage () {
  package=$1
  makeFile=`[[ -f "$BUILDROOT_DIR/package/$package/$package.mk" ]] && echo "$BUILDROOT_DIR/package/$package/$package.mk" || echo "$RECALBOX_DIR/package/$package/$package.mk"`
  commitid=`grep "_VERSION = " "$makeFile"  | grep -v "^#" | cut -d " " -f 3`
  echo $commitid
}


#
# Get package latest commit id
#
function getCommitIdFromGitHub () {
  package=$1
  url=`getGitHubApiUrlFromPackage "$package"`
  [[ -z $url ]] && exit 1
  #commitid=`curl ${CURLPARMS} -s "$url"/commits | grep sha | head -n 1 |cut -d '"' -f 4`
  commitid=`export PYTHONIOENCODING=utf8 ; curl ${CURLPARMS} -s "$url"/commits | python -c "import sys, json; print json.load(sys.stdin)[0]['sha']" 2>/dev/null`
  
  if [[ ! -z $commitid ]] ; then
    echo $commitid
  else
    stopAndQuitIfHttp403 "$url/commits"
  fi
}

#
#
#
function stopAndQuitIfHttp403 () {
  url="$1"
  httpStatus=`curl -sw "%{http_code}\n" ${CURLPARMS} -s "$url"/commits | tail -n 1`
  [[ $httpStatus == "403" ]] && echo " ERROR: API calls limit reached, can't go further" >&2 && kill -- -$$ 2>/dev/null && exit 1
}

#
#
#
function isValidSHA1 () {
  echo $1 | grep -qiE "^[a-f0-9]{40}$"
}

#
# Get package latest release
#
function getLatestRelease () {
  package=$1
  url=`getGitHubApiUrlFromPackage "$package"`
  [[ -z $url ]] && exit 1
  tag=`export PYTHONIOENCODING=utf8 ; curl ${CURLPARMS} -s "$url"/releases/latest | python -c "import sys, json; print json.load(sys.stdin)['tag_name']" 2>/dev/null`
  if [[ ! -z $tag ]] ; then
    echo $tag
  else
    stopAndQuitIfHttp403 "$url/releases/latest"
  fi
}


#
# Gets the BR2_* package variable name in Buildroot
#
function getBR2ConfigVar () {
  package=$1
  configFile=`[[ -f $BUILDROOT_DIR/package/"$package"/Config.in ]] && echo "$BUILDROOT_DIR/package/"$package"/Config.in" || echo "$RECALBOX_DIR/package/"$package"/Config.in"`
  BR2var=`head -n 1 "$configFile" | grep "^config BR2_" | sed "s+^config ++g"`
  echo $BR2var
}

#
# show if the $1 (package) is compiled in $2 (arch)
#
function isPackageInArch () {
  package="$1"
  arch="$2"
  brvar=`getBR2ConfigVar "${package}"`
  grep -q "^${brvar}=y" "$RECALBOX_DIR/configs/recalbox-${arch}_defconfig" && echo -en "\tx" || echo -en "\t"
}

#
# Update a package ($1) from commit ($2) to commitid ($3)
# 
function updatePackageCommitId () {
  package="$1"
  oldId="$2"
  newId="$3"
  brPackageName=`echo $package | tr /a-z/ /A-Z/ | sed s+-+_+g`
  echo "UPDATING: $1 $oldId -> $newId" >&2
  sed -i "s/\(${brPackageName}_VERSION\s\?=\s\?\)${oldId}/\1${newId}/" "$RECALBOX_DIR"/package/"${package}"/"${package}".mk
}

function getKernelHeaderFromDefconfig() {
  grep "BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_" "$1" | sed "s+BR2.*_\([[:digit:]]\{1,2\}\)_\([[:digit:]]\{1,2\}\).*+\1.\2+g"
}

function getKernelCustomRepo () {
  grep "BR2_LINUX_KERNEL_CUSTOM_REPO_VERSION" "$1" | cut -d "=" -f 2- | tr -d '"'
}

function getKernelCustomValue () {
  grep "BR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE" "$1" | cut -d "=" -f 2- | tr -d '"'
}


function updateDefconfig () {
  file="$1"
  searchPattern="$2"
  replacePattern="$3"
  
  sed -i "s+.*${searchPattern}.*+${replacePattern}+g" "$file"
}
#
# Compare defconfigs
#
function comparePiDefconfig() {
  piVersion=$1 #0 2 or 3
  willUpdate="$2"
  skippedDefconfig="$3"
  [[ $piVersion == 1 ]] && piBRVersion=0 || piBRVersion=$piVersion
  
  if [[ $willUpdate == 1 && $skippedDefconfig != *"recalbox-rpi${piVersion}_defconfig"* ]] && \
	  [[ ! -z $selectedPackages && $selectedPackages == *"recalbox-rpi${piVersion}_defconfig"* || -z $selectedPackages ]]
  then
    brKernelHeaders=`grep "BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_" "$BUILDROOT_DIR/configs/raspberrypi${piBRVersion}_defconfig"`
    updateDefconfig "$RECALBOX_DIR/configs/recalbox-rpi${piVersion}_defconfig" "BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_" "$brKernelHeaders"
    brKernelCommitId=`grep "BR2_LINUX_KERNEL_CUSTOM_REPO_VERSION" "$BUILDROOT_DIR/configs/raspberrypi${piBRVersion}_defconfig"`
    updateDefconfig "$RECALBOX_DIR/configs/recalbox-rpi${piVersion}_defconfig" "BR2_LINUX_KERNEL_CUSTOM_REPO_VERSION" "$brKernelCommitId"
  fi
  
  #~ Get current kernel branch
  rbxkernelBranch=`getKernelHeaderFromDefconfig "$RECALBOX_DIR/configs/recalbox-rpi${piVersion}_defconfig"`
  brkernelBranch=`getKernelHeaderFromDefconfig "$BUILDROOT_DIR/configs/raspberrypi${piBRVersion}_defconfig"`
  rbxKernelCommitId=`getKernelCustomRepo "$RECALBOX_DIR/configs/recalbox-rpi${piVersion}_defconfig"`
  brKernelCommitId=`getKernelCustomRepo "$BUILDROOT_DIR/configs/raspberrypi${piBRVersion}_defconfig"`
  updateKernelBranch=`[[ $rbxKernelBranch == $brKernelBranch ]] && echo "Up-to-date" || echo "Bump !"`
  updateKernelCommitId=`[[ $rbxKernelCommitId == $brKernelCommitId ]] && echo "Up-to-date" || echo "Bump !"`
  
  echo -e "\e[7m Pi$piVersion Defconfig \e[27m"
  header="\e[1mPackage\t   Recalbox version\t   Buildroot version\t   Update ?\e[0m"
  echo -e "$header
Kernel branch\t${rbxkernelBranch}\t${brkernelBranch}\t${updateKernelBranch}
Kernel Commit Id\t${rbxKernelCommitId:0:7}\t${brKernelCommitId:0:7}\t${updateKernelCommitId}" | column -ten -s $'\t'
}

#~ PC
#~ BR2_PACKAGE_HOST_LINUX_HEADERS_CUSTOM_4_11=y
#~ BR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE="4.11.5"
function comparePcDefconfig() {
  x86Arch="$1"
  willUpdate="$2"
  skippedDefconfig="$3"
  X86_LATEST_KERNEL=`grep -E "default .* if BR2_LINUX_KERNEL_LATEST_VERSION" "$BUILDROOT_DIR/linux/Config.in" | sed 's/[^"]*"\([^"]*\).*/\1/'`

  # Can't be done yet as the package is poorly written
  #~ if [[ $willUpdate == 1 && $skippedDefconfig != *"recalbox-rpi${piVersion}_defconfig"* ]] ; then
  #~ fi
  rbxkernelBranch=`getKernelHeaderFromDefconfig "$RECALBOX_DIR/configs/recalbox-${x86Arch}_defconfig"`
  brkernelBranch=`getKernelHeaderFromDefconfig "$BUILDROOT_DIR/configs/pc_x86_64_efi_defconfig"`
  rbxKernelCustomValue=`getKernelCustomValue "$RECALBOX_DIR/configs/recalbox-${x86Arch}_defconfig"`
  brKernelCustomValue=`getKernelCustomValue "$BUILDROOT_DIR/configs/pc_x86_64_efi_defconfig"`
  updateKernelBranch=`[[ $rbxKernelBranch == $brKernelBranch ]] && echo "Up-to-date" || echo "Bump !"`
  updateKernelCommitId=`[[ $rbxKernelCustomValue == $brKernelCustomValue ]] && echo "Up-to-date" || echo "Bump !"`
  
  echo -e "\e[7m $x86Arch Defconfig \e[27m"
  header="\e[1mPackage\t   Recalbox version\t   Buildroot version\t   Update ?\t   Max\e[0m"
  echo -e "$header
Kernel branch\t${rbxkernelBranch}\t${brkernelBranch}\t${updateKernelBranch}
Kernel Version\t${rbxKernelCustomValue}\t${brKernelCustomValue}\t${updateKernelCommitId}\t${X86_LATEST_KERNEL}" | column -ten -s $'\t'
}

#
# Pi specific informations
#
function getPiUpstreamInfo () {
  # Kernel details
  currentCommit=`curl -s https://raw.githubusercontent.com/Hexxeh/rpi-firmware/stable/git_hash`
  currentKernelVersion=`curl -s "https://raw.githubusercontent.com/raspberrypi/linux/${currentCommit}/Makefile" | grep -E "^(VERSION|PATCHLEVEL|SUBLEVEL) = " | grep -Eo "[0-9]*$" | tr -s '\n' '.' | sed 's/\.$/\n/'`

  # Corresponding firmware
  # We'll look for commits at raspberrypi/firmware that occured AFTER the commit date
  #commitDate=`curl "https://api.github.com/repos/raspberrypi/linux/commits/${currentCommit}" | python -c "import sys, json; print json.load(sys.stdin)['commit']['author']['date']"`
  # Afterall, use the search API and pray the 1st result is the good one
  currentFWCommitId=`curl ${CURLPARMS} -sH "Accept: application/vnd.github.cloak-preview" "https://api.github.com/search/commits?q=repo:raspberrypi/firmware+${currentKernelVersion}" | python -c "import sys, json; print json.load(sys.stdin)['items'][0]['sha']"`

  # Display the result
  echo -e "\e[7m Pi Upstream infos \e[27m"
  echo "Kernel stable version: $currentKernelVersion ($currentCommit)"
  echo "Corresponding firmware: $currentFWCommitId (make sure it's right)"
}

#
# Get package infos + upstream infos and compare
#
function taskEmulators () {
  emu=$1
  willUpdate="$2"
  skippedPackages="$3"
  # Make sure the package exists
  [[ ! -f package/"$emu"/"$emu".mk ]] 
  currentId=`getCommitIdFromPackage "$emu"`
  if isValidSHA1 "$currentId" ; then
    remoteId=`getCommitIdFromGitHub "$emu"`
  else
    remoteId=`getLatestRelease "$emu"`
  fi

  # Update the package if update is set and the package is not skipped
  [[ $willUpdate == 1 ]] && \
	  [[ $skippedPackages != *"$emu"* ]] && \
	  [[ ! -z $selectedPackages && $selectedPackages == *"$emu"* || -z $selectedPackages ]] && \
	  updatePackageCommitId "$emu" "$currentId" "$remoteId"
  currentId=`getCommitIdFromPackage "$emu"`

  str="$emu\t${currentId:0:7}\t${remoteId:0:7}"
  if [[ -z $remoteId ]] ; then 
    str="${str}\tDead repo ?"
  elif [[ $currentId != $remoteId ]]; then
    str="${str}\tBump !"
  else
    str="${str}\tUp-to-date"
  fi
  
  # Add the compilation for arch flag
  for arch in $DEFCONFIGS ; do
    str="${str}`isPackageInArch $emu $arch`"
  done
  echo -e "$str"
}

function taskPackages () {
  emu=$1
  willUpdate="$2"
  skippedPackages="$3"
  # Make sure the package exists
  [[ ! -f package/"$emu"/"$emu".mk ]]
  currentId=`getCommitIdFromPackage "$emu"`
  if isValidSHA1 "$currentId" ; then
    remoteId=`getCommitIdFromGitHub "$emu"`
  else
    remoteId=`getLatestRelease "$emu"`
  fi

  # Update the package if update is set and the package is not skipped
  [[ $willUpdate == 1 ]] && \
	  [[ $skippedPackages != *"$emu"* ]] && \
	  [[ ! -z $selectedPackages && $selectedPackages == *"$emu"* || -z $selectedPackages ]] && \
	  updatePackageCommitId "$emu" "$currentId" "$remoteId"
  currentId=`getCommitIdFromPackage "$emu"`

  str="$emu\t${currentId:0:7}\t${remoteId:0:7}"
  if [[ -z $remoteId ]] ; then
    str="${str}\tDead repo ?"
  elif [[ $currentId != $remoteId ]]; then
    str="${str}\tShould update"
  else
    str="${str}\tUp-to-date"
  fi

  echo -e "$str"
}

function mainEmulators () {
  willUpdate="$1"
  skippedPackages="$2"
  for emu in $EMULATORS ; do
    taskEmulators "$emu" "$willUpdate" "$skippedPackages" "$selectedPackages" &
  done >> /tmp/result
  wait
  # We need \t + 3 spaces behind to compensate the non printable caracters for bold
  header="\e[1mEmulator / Package\t   Recalbox version\t   Most recent version\t   Update ?\t   `echo $DEFCONFIGS | sed "s+ +\t   +g"`\e[0m\n"
  ( echo -e "$header" ; ( cat /tmp/result | sort ) ) | column -ten -s $'\t'
  rm /tmp/result
}

function mainPackages () {
  willUpdate="$1"
  skippedPackages="$2"
  for emu in $PACKAGES ; do
    taskPackages "$emu" "$willUpdate" "$skippedPackages" &
  done >> /tmp/result
  wait

  # We need \t + 3 spaces behind to compensate the non printable caracters for bold
  header="\e[1mPackage\t   Buildroot version\t   Upstream version\t   Update ?\e[0m\n"
  ( echo -e "$header" ; ( cat /tmp/result | sort ) ) | column -ten -s $'\t'
  rm /tmp/result
}

function main() {
  mainEmulators "$willUpdate" "$skippedPackages" "$selectedPackages"
  echo
  mainPackages "$willUpdate" "$skippedPackages" "$selectedPackages"
  echo
  getPiUpstreamInfo
  comparePiDefconfig 1 "$willUpdate" "$skippedPackages" "$selectedPackages"
  comparePiDefconfig 2 "$willUpdate" "$skippedPackages" "$selectedPackages"
  comparePiDefconfig 3 "$willUpdate" "$skippedPackages" "$selectedPackages"
  comparePcDefconfig x86 "$willUpdate" "$skippedPackages" "$selectedPackages"
  comparePcDefconfig x86_64 "$willUpdate" "$skippedPackages" "$selectedPackages"
}

# Usage
function usage() {
  echo "Usage :"
  echo "-h, --help                     Print this help"
  echo "-u, --update                   Update packages"
  echo "-s, --skip-package <package>   Exclude <package> from update. Can be used multiple times"
  echo "-p, --package-select <package> Select only <package>. Can be used multiple times"
  echo "-U, --user <user>              Specify github user for API access"
  echo "-t, --token <token>            Specify github user token for API access"
  echo
  echo "To query the github API you need to set some github user + token through the command line"
  echo "You can also specify github user and token by creating scripts/linux/.githubAPIToken with the following content:"
  echo "GHUser=username"
  echo "GHKey=xxxxxxxxxxxxxxxxxxxxxxxxxxxx"

}

#default values
willUpdate=0
skippedPackages=""
selectedPackages=""
# parse arguments
while [[ $# -gt 0 ]]
do
    key="$1"
    case $key in
	-h | --help)
	    usage
	    exit
	    ;;
	-u | --update)
            echo "PARAMS: will update packages" >&2
            willUpdate=1
            ;;
        -p | --package-select)
            echo "PARAMS: add $2 to selected packages for update" >&2
            selectedPackages="$selectedPackages $2"
	    shift
	    ;;
        -s | --skip-package)
            echo "PARAMS: skipping package $2" >&2
            skippedPackages="$skippedPackages $2"
	    shift
	    ;;
	-U | --user)
            echo "PARAMS: github user set to $2" >&2
            GHUser="$2"
	    shift
	    ;;
	-t | --token)
            echo "PARAMS: github token set to $2" >&2
            GHKey="$2"
	    shift
	    ;;
	*)
	    echo "Invalid option: $2. Exiting ..."
	    exit 1
            ;;
    esac
    shift
done

getAPIToken

main
