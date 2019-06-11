#!/bin/bash

# This script installs Recalbox onto the device passed as parameter.
# It is useful to boot Recalbox on a "live" support (USB thumb drive, SD card)
# and install it onto an internal hard drive.

### Global variables ###

bootPartition=$(/recalbox/scripts/recalbox-part.sh boot)
sourceDevice=$(/recalbox/scripts/recalbox-part.sh prefix ${bootPartition})

### Colours ###

c_reset='\033[0m'
c_bold_white='\033[1;1m'
c_red='\033[1;31m'
c_gray='\033[1;30m'
c_green='\033[1;32m'
c_yellow='\033[1;33m'

### Functions ###

function exitWithUsage() {
  echo -e "${c_bold_white}Usage${c_reset}: $(basename $0) [-f|--force] [-y|--assume-yes] [-l|--list-devices] [-m|--machine-output] [-h|--help] <target_device>

  This script installs the currently running Recalbox OS onto the device passed
  as parameter (usually an internal hard drive drive).

${c_bold_white}Options${c_reset}
  -l, --list-devices     list available storage devices
  -f, --force            skip safety checks
  -y, --assume-yes       skip user validation
  -m, --machine-output   output in machine-readable format
  -h, --help             display this message
"
  exit 64 # EX_USAGE
}

function askUserValidation() {
  while [[ ! ${areYouSure} =~ [yn] ]]; do
    echo -ne "Are you sure you want to continue? ${c_gray}(y/n)${c_reset} "
    read areYouSure
  done

  if [[ ${areYouSure} != "y" ]]; then
    echo "Exiting."
    exit 1
  fi
}

# printDeviceList
#   → prints available storage devices on the machine, either in machine or human format
#     based on `parted --list --machine` (see https://alioth-lists.debian.net/pipermail/parted-devel/2006-December/000573.html)
function printDeviceList() {
  local readonly partedOutput=$(parted --list --machine 2> /dev/null | sed -nE '/^\s*(BYT|CHS|CYL);$/{n;p}' | cut -d: -f1,2,7)
  if [[ -n ${optionMachineOutput} ]]; then
    echo "${partedOutput}"
  else
    echo -e "${c_bold_white}Available storage devices${c_reset}"
    echo "${partedOutput}" | sed 's|:|\t|g'
  fi
}

# printError <message>
#   → prints an error message, either in machine or human format
function printError() {
  local readonly message="$1"
  if [[ -n ${optionMachineOutput} ]]; then
    echo "${message}"
  else
    echo -e "${c_red}[error]${c_reset} ${message}"
    echo "Exiting."
  fi
  exit 1
}

# printStatus <message>
#   → prints a status message, only in human format
#     does not break line, expected to be followed by 'printOK'
function printStatus() {
  local readonly message="$1"
  if [[ -z ${optionMachineOutput} ]]; then
    echo -ne "${message}... "
  fi
}

# printOK
#   → prints a green "[OK]", only in human format
function printOK() {
  if [[ -z ${optionMachineOutput} ]]; then
    echo -e "${c_green}[OK]${c_reset}"
  fi
}

# printWarning <message>
#   → prints a warning message, only in human format
function printWarning() {
  local readonly message="$1"
  if [[ -z ${optionMachineOutput} ]]; then
    echo -e "${c_yellow}[warning]${c_reset} ${message}"
  fi
}

### Parameters parsing ###

while [[ -n $1 ]]; do
  case $1 in
    -*)
      case $1 in
        -f|--force)
          optionForce=1
          ;;
        -y|--assume-yes)
          optionAssumeYes=1
          ;;
        -l|--list-devices)
          optionList=1
          ;;
        -m|--machine-output)
          optionMachineOutput=1
          ;;
        -h|--help)
          exitWithUsage
          ;;
        *)
          printError "Unknown option '$1'."
          ;;
      esac
      ;;
    *)
      if [[ -z ${targetDevice} ]]; then
        targetDevice=$1
      else
        exitWithUsage
      fi
      ;;
  esac
  shift
done

if [[ -n ${optionList} ]]; then
  printDeviceList
  exit 0
fi

if [[ -z ${targetDevice} ]]; then
  exitWithUsage
fi

### Basic check ###

if [[ -z ${optionForce} ]]; then
  # Check source device
  if [[ ! -b ${sourceDevice} ]]; then
    printError "Source device '${sourceDevice}' is not a block device."
  fi

  # Check target device
  if [[ ! -b ${targetDevice} ]]; then
    printError "Target device '${targetDevice}' is not a block device."
  fi

  # Check that source and target devices are not the same
  if [[ ${sourceDevice} == ${targetDevice} ]]; then
    printError "Source and target devices cannot be the same."
  fi
fi

### Require user manual validation to process ###

if [[ -z ${optionAssumeYes} ]]; then
  printWarning "This will install the content of '${sourceDevice}' onto '${targetDevice}'. All data on '${targetDevice}' will be lost."
  askUserValidation
fi

### Un-mount target device partitions ###

printStatus "Un-mounting mounted partitions of '${targetDevice}'"
for partition in ${targetDevice}*; do
  if mount | grep -qE "^${partition}\s+"; then
    umount ${partition} || exit 1
  fi
done
printOK

### Raw-copy data from source device to target device ###

printStatus "Copying '${sourceDevice}' onto '${targetDevice}' (it might take a while)"
dd if=${sourceDevice} of=${targetDevice} bs=4M status=none
printOK

### Resize SHARE partition to fill all available space ###

printStatus "Resize SHARE to fill up all available space on '${targetDevice}'"
parted -s ${targetDevice} resizepart 3 100%  # First, resize the SHARE partition
e2fsck -f -y ${targetDevice}3                # Then, mark its filesystem as clean
resize2fs ${targetDevice}3                   # Finally, resize the filesystem
printOK
