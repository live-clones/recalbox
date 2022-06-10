#!/bin/bash
source /recalbox/scripts/recalbox-utils.sh

SKIPVIDEOSPLASHFLAG=/overlay/.configs/skipvideosplash

# Management functions

function mediaIgnoreAll {
    touch /tmp/.media.ignoreall
}

function mediaShouldIgnoreAll {
    test -f /tmp/.media.ignoreall
    return $?
}

function mediaShouldIgnoreSplash {
    test -f "${SKIPVIDEOSPLASHFLAG}" || mediaShouldIgnoreAll
    return $?
}

function mediaIsLowDef {
    # Low definiton devices
    test [ "$(cut -d, -f2 /sys/class/graphics/fb0/virtual_size)" -le 320 ] \
        && ! isRecalboxRGBDual
    return $?
}

# Mpv Queue (used in S05mediaservice)
function startMpvQueue {
    if [ ! -f /tmp/.mpvqueue ];then
        touch /tmp/.mpvqueue
        (
            while [ -f /tmp/.mpvqueue ];do
                if ! pgrep mpv; then
                    if [ -f /tmp/.mpvqueuevideos ] && [ $(wc -l < /tmp/.mpvqueuevideos) -gt 0 ]; then
                        # mpv is not running and we have videos to play
                        touch /tmp/.mpvqueuevideos.runned
                        rm /tmp/.mpvqueuevideos.fbv
                        video=$(head -n1 /tmp/.mpvqueuevideos)
                        cp /tmp/.mpvqueuevideos /tmp/.mpvqueuevideos.save
                        tail -n+2 /tmp/.mpvqueuevideos.save > /tmp/.mpvqueuevideos
                        eval "$video"
                    else
                        # mpv is not running we should display the background
                        if [ ! -f /tmp/.mpvqueuevideos.fbv ];then
                            touch /tmp/.mpvqueuevideos.fbv
                            if mediaIsLowDef; then
                                fbv2 -f -i /recalbox/system/resources/splash/240p/logo-version.png
                            else
                                fbv2 -f -i /recalbox/system/resources/splash/logo-version.png
                            fi
                        fi
                    fi
                fi
                sleep 1
            done
        )
    fi
}

function stopMpvQueue {
    if [ -f /tmp/.mpvqueuevideos.runned ];then
        killall mpv
    fi
    rm -f /tmp/.mpvqueue /tmp/.mpvqueuevideos.save /tmp/.mpvqueuevideos /tmp/.mpvqueuevideos.runned
}

function stopAllMedia {
    kilall mpv
}

function addNextVideo {
    newVideo="$@"
    echo "${newVideo}" >> /tmp/.mpvqueuevideos
}




# showIntroBackground
#   displays our intro background via the framebuffer
showIntroBackground() {
  if mediaIsLowDef ; then
    fbv2 -f -i /recalbox/system/resources/splash/240p/logo-version.png
  else
    fbv2 -f -i /recalbox/system/resources/splash/logo-version.png
  fi
  # launching ES just after fbv can cause some screen corruption
  sleep 1
}


# start mpv then show the background (aka pacman screen)
# $1 timeout
# $2 array of options for mpv
runInBackground() {
  local timeout="${1:-0}"; shift
  local options=("$@")

  if [ "$timeout" -gt 0 ]; then
    addNextVideo timeout "$timeout" mpv "${options[@]}"
  else
    addNextVideo mpv "${options[@]}"
  fi
}

# playVideoOnOneScreen
#   play a video on a specific display
# intput:
# * filename to play
# * optins regarding output screen (--ao=alsa  --vo=drm  --drm-connector=1.DVI-I-1)
playVideoOnOneScreen() {
  local options
  # shellcheck disable=SC2206
  options+=($3) # get space delimited options as is
  options+=(--ao=pulse)
  options+=("${1}")
  timeout="${2}"
  runInBackground "${timeout}" "${options[@]}" #>/dev/null 2>&1 &
}

# playVideo
#   select all screen and run playVideoOnOneScreen for each connected screen
# intput:
# * filename to play
mediaPlayVideo() {
    if mediaShouldIgnoreAll; then
        return
    fi

    local conn
    local resolution
    local drmmode=""
    local video="${1}"
    local timeout="${2}"
    local options="${3}"

    if [[ "$machineArch" =~ x86 ]]; then
        resolution="$($systemsetting -command load -key system.splash.resolution -source /boot/recalbox-backup.conf)"
        if [ "$resolution" != "" ] ; then drmmode="--drm-mode=$resolution" ; fi
        # on x86 and x86_64, find connected monitors and play video on all of them
        findConnectedConnectors |while read -r conn; do
            playVideoOnOneScreen "${video}" "${timeout}" "--vo=drm $drmmode --drm-connector=$conn $options"
        done
    elif isRecalboxRGBDual; then
        # On recalbox RGB Dual we will try to get 640x480i for videos 
        playVideoOnOneScreen "${video}" "${timeout}" "$(getCrtMpvOptions) $options"
    elif [[ "$ARCH" == "rpi1" ]]; then
        # on rpi1 2 and 3 we need to force output
        playVideoOnOneScreen "${video}" "${timeout}" "--hwdec=auto -vo=rpi $options"
    else
        # on other simply play
        playVideoOnOneScreen "${video}" "${timeout}" "$options"
    fi
}


# # # 
# Splash functions
mediaPlaySplash() {
    if mediaShouldIgnoreSplash; then
        return
    fi
    video="${1}"
    timeout="${2}"
    mediaPlayVideo "${video}" "${timeout}" "--fs --sub-file=/recalbox/system/resources/splash/recalboxintro.srt --really-quiet"
}

mediaPlayInstallMedia() {
    source /recalbox/scripts/recalbox-utils.sh
    IMAGE_PATH=$(getInstallUpgradeImagePath)

    # mpv has a bug having the second and last slide not displayed
    if isRecalboxRGBDual; then
        mediaPlayVideo /recalbox/system/resources/install/install-recalbox-rgb-dual.mp4
    else
        mediaPlayVideo "$IMAGE_PATH/install-1.png" \
        "$IMAGE_PATH/install-2.png" \
        "$IMAGE_PATH/install-2.png" \
        "$IMAGE_PATH/install-3.png" \
        "$IMAGE_PATH/install-4.png" \
        "$IMAGE_PATH/install-5.png" \
        "$IMAGE_PATH/install-6.png" \
        "$IMAGE_PATH/install-7.png" \
        "$IMAGE_PATH/install-8.png" \
        "$IMAGE_PATH/install-8.png" "" "--really-quiet --image-display-duration=8"
    fi
}