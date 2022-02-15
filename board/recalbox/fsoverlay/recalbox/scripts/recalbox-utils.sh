# utilities
# how to use this file:
# in your bash script:
#
# source /recalbox/scripts/recalbox-utils.sh
#

RECALBOX_ARCH_FILE="/recalbox/recalbox.arch"

# Return the current running arch (or board)
# sanitized
# returns 0 on success and the name of the board on stdout
# returns 1 on error and unknown on stdout
getArchName() {
  local ARCH
  if [ -f "$RECALBOX_ARCH_FILE" ]; then
    ARCH=$(head -n 1 "$RECALBOX_ARCH_FILE" | tr '[:upper:]' '[:lower:]')
    if [ -n "$ARCH" ]; then
      echo "$ARCH"
      return 0
    else
      echo "unknown"
      return 1
    fi
  else
    echo "unknown"
    return 1
  fi
}

# Test if the current board allows mounting external share (USB, not network)
# i.e sharedevice=ANYEXTERNAL or sharedevice=DEV fs-uuid from /boot/recalbox-boot.conf
# currently, odroidgo2 does not support mounting USB share as
# it becomes unavailable after power suspend
# return 0 if the current board support mounting an external share
# return 1 if the current board DOES NOT support mounting an external share
doesBoardSupportExternalShare() {
  if [ "$(getArchName)" = 'odroidgo2' ]; then
    return 1
  fi
  return 0
}

# Returns path to directory containing best resolution images
# for current screen resolution
getInstallUpgradeImagePath() {
  local MODEL
  if [ "$(getArchName)" = "odroidgo2" ]; then
    MODEL=$(tr '\0' '\n' </sys/firmware/devicetree/base/model)
    case $MODEL in
      *ODROID-GO2) echo "/recalbox/system/resources/320x480" ;;
      *ODROID-GO3) echo "/recalbox/system/resources/480x854" ;;
      *)           echo "/recalbox/system/resources" ;;
    esac
  else
    echo "/recalbox/system/resources"
  fi
}

# Get the best CRT MPV Options
getCrtMpvOptions() {
  local connector="1.VGA-1"
  local arch="$(getArchName)"
  if [[ "$arch" == "rpi3" ]] || [[ "$arch" == "rpizero2" ]]; then 
    echo ""
    return
  fi
  if mpv --drm-mode=help | grep -q "640x480"; then
    echo "--vo=drm --drm-connector=${connector} --drm-mode=$(mpv --drm-mode=help | grep 640x480 | awk '{print $2}' | cut -c '1')"
  fi
}

# findConnectedConnectors
#   returns a list of connected connectors (using mpv)
findConnectedConnectors() {
  grep -l ^connected /sys/class/drm/card*/status | sed 's/.*card\([0-9]\+\)-\([^\/]\+\).*/\1.\2/'
}


# Check if we are on Recalbox RGB Dual
isRecalboxRGBDual() {
  if grep -q "Recalbox RGB Dual" /sys/firmware/devicetree/base/hat/product;then
    return 0
  else
    return 1
  fi
}

# Get the best MPV Options
getMpvOptions() {
  if isRecalboxRGBDual; then
    getCrtMpvOptions
  else
    echo ""
  fi
}

# Returns true if the platform may need to force enable DRM connector.
# This is related to black screen issue
doesBoardNeedKMSEnabler() {
  case "$(getArchName)" in
    rpizero2|rpi3|rpi4|rpi4_64)
      return 0 ;;
    *)
      return 1 ;;
  esac
}
