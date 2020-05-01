#!/bin/bash
###################################
# Authors: Ian57 (Recalbox) 2020  #
# Thanksx to LittleBallup         #
# for the initial work            #
###################################
#      Useful Variables           #
###################################
# Start/Stop secondMiniTFT support
NAME="miniTFTSupport.sh"

#some useful system constants
fbDevice="/dev/fb1"
FFMPEG="/usr/bin/ffmpeg"
scriptPID="$(cat /var/run/miniTFT.pid)"
systemsetting="recalbox_settings"
tempoShort="25000"
tempoLong="1000000"

#file wich contains path to boot/shutdown/system videos (in /recalbox/share/system to be easily editable by user)
functionFile="/recalbox/share/system/configs/minitftscreen/miniTFTVideosCfg.sh"

#get configuration from recalbox.conf
tftEnabled="$(${systemsetting} -command load -key system.secondminitft.enabled)"
recallog "Reading system.secondminitft.enabled => ${tftEnabled}"
tftResolution="$(${systemsetting} -command load -key system.secondminitft.resolution)"
recallog "Reading system.secondminitft.resolution => ${tftResolution}"
tftType="$(${systemsetting} -command load -key system.secondminitft.type)"    
recallog "Reading system.secondminitft.type => ${tftType}"
tftBacklightControl="$(${systemsetting} -command load -key system.secondminitft.backlightcontrol)"

if [[ "${tftBacklightControl}" ]] ; then 
    recallog "Reading system.secondminitft.backlightcontrol => ${tftBacklightControl}"
else
    recallog "Reading system.secondminitft.backlightcontrol => none (No backlight control)"
fi

#needed to use scaling with ffmpeg
case "${tftResolution}" in
    240p)    tftFullResolution="320x240"  ;;
    320p)    tftFullResolution="480x320"  ;;
esac
recallog "Full Resolution => ${tftFullResolution}"

#get image processing option fot fbv
imgStretchEnabled="$(${systemsetting} -command load -key system.secondminitft.imageStretchEnabled)"
recallog "Reading system.secondminitft.imageStretchEnabled => ${imgStretchEnabled}"
imgEnlargeEnabled="$(${systemsetting} -command load -key system.secondminitft.imageEnlargeEnabled)"
recallog "Reading system.secondminitft.imageEnlargeEnabled => ${imgEnlargeEnabled}"
imgAlphaEnabled="$(${systemsetting} -command load -key system.secondminitft.imageAlphaEnabled)"
recallog "Reading  system.secondminitft.imageAlphaEnabled => ${imgAlphaEnabled}"
imgIgnoreAspectEnabled="$(${systemsetting} -command load -key system.secondminitft.imageIgnoreAspectEnabled)"
recallog "Reading system.secondminitft.imageIgnoreAspectEnabled => ${imgIgnoreAspectEnabled}"


if [[ "${imgStretchEnabled}" == "1" ]] ; then
    imgStretch="--stretch"
else
    imgStretch=""
fi
if [[ "${imgEnlargeEnabled}" == "1" ]] ; then
    imgEnlarge="--enlarge"
else
    imgEnlarge=""
fi
if [[ "${imgAlphaEnabled}" == "1" ]] ; then
    imgAlpha="--alpha"
else
    imgAlpha=""
fi
if [[ "${imgIgnoreAspectEnabled}" == "1" ]] ; then
    imgIgnoreAspect="--ignore-aspect"
else
    imgIgnoreAspect=""
fi
recallog "fbv options : ${imgStretch} ${imgEnlarge} ${imgIgnoreAspect} ${imgAlpha}"

#get Videos function and variables
source "${functionFile}"

do_start() {
    recallog "... Starting : system.secondminitft.enabled"
    #test fot OGST support
    case "${tftType}" in
        ogst)
            recallog "Init OGST Case Screen..."
            fbDevice="/dev/fb_"
            if [ -c "${fbDevice}" ]; then
                recallog "Init OGST : /dev/fb1 already present..."
            else
                # init TFT screen OGST
                recallog "Init OGST : removing modules ..."
                modprobe -r fbtft_device
                modprobe -r fb_ili9340
                modprobe -r fbtft
                modprobe -r spidev
                modprobe -r spi_s3c64xx
                sleep 1
                recallog "Init OGST : installing modules with better options ..."
                modprobe spi_s3c64xx force32b=1
                modprobe spidev
                modprobe fbtft_device name=hktft9340 busnum=1 rotate=270 force32b=1
                sleep 1
                #rename fb1 to fb_ (trick to ovoid conflics with some emulators like reicast...)
                mv /dev/fb1 /dev/fb_
                
                #fbDevice="/dev/fb_"
            fi
            ;;
        overlay)
            recallog "TFT screen configured through overlays in /boot/config.txt. Be sure to have your second TFT working"
            ;;
        default)
            recallog "Not OGST Case Screen, be sure to have your second TFT configured thanks to overlays"
            ;;
    esac
    # test if ${fbDevice} is present to be able to display stuff
    if [ -c "${fbDevice}" ]; then
        echo "Second Screen detected."
        
        if [[ "${tftBacklightControl}" ]] ; then 
            #~ #backlight control...  pwm  on GPIO "${tftBacklightControl}"
            gpio -g mode "${tftBacklightControl}" pwm 
            gpio -g pwm "${tftBacklightControl}" 1023
            WAKEUP=1        
        fi
        
        #display start video
        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
        #to save time intro video is backgrounded and killable with any action
        pkill -P "${scriptPID}" &> /dev/null
        "${FFMPEG}" -re -i "${LogoFolder}"/"${tftResolution}"/"${DefaultStartLogo}" -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null
        
        #initialize
        LTIME=""
        LTIME2=""
        LSIZE=""
        # loop forever
        while true
        do
        
            #With MQTT Mosquitto
            #problem with stat, nee to check the modify field only (13th), if not, loop is exécuted twice (modify and access)
            #eventES=$(mosquitto_sub -h 127.0.0.1 -p 1883 -q 0 -t Recalbox/EmulationStation/Event -C 1)
            
            ATIME=$(stat -t  /tmp/es_state.inf | awk '{print $13}')
            ATIME2=$(stat -t  /tmp/es_state.inf | awk '{print $14}')
            ASIZE=$(wc /tmp/es_state.inf | awk '{print $3}')
        
            if [[ "${ATIME}" != "${LTIME}" ]]  || [[ "${ASIZE}" != "${LSIZE}" ]] || [[ "${ATIME2}" != "${LTIME2}" ]] ; then
                LTIME="${ATIME}"
                LTIME2="${ATIME2}"
                LSIZE="${ASIZE}"
                        
                #getAction
                TFTAction="$(sed -n 's/^Action=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                # action in TFTAction : systembrowsing, gamelistbrowsing, rungame, rundemo, endgame, enddemo
                # Used variables TFTGameImage, TFTVideoPath, TFTGame, TFTESSystem, TFTESState
                case "${TFTAction}" in
                    sleep)
                        case "${tftType}" in
                            ogst)
                                if [[ "$(cat /sys/class/backlight/*/bl_power})"=="0" ]] && [[ "${WAKE}"=="1" ]] ; then
                                    backLight=$(echo 1 | tee /sys/class/backlight/*/bl_power)
                                    WAKE="1"
                                fi
                                ;;
                            overlay)
                                if [[ "${tftBacklightControl}" ]] ; then 
                                    if [[ "$(gpio -g read ${tftBacklightControl})"=="1" ]] && [[ "${WAKE}"=="1" ]] ; then
                                        for i in {1023..64}; do gpio -g pwm "${tftBacklightControl}" $i; done
                                        WAKE="0"
                                    fi
                                fi
                                ;;
                        esac    
                        ;;
                    wakeup)
                        pkill -P "${scriptPID}" &> /dev/null
                        #delete screen on wakeup
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        case "${tftType}" in
                            ogst)
                                if [[ "$(cat /sys/class/backlight/*/bl_power})"=="1" ]] && [[ "${WAKE}"=="0" ]] ; then
                                    backLight=$(echo 0 | tee /sys/class/backlight/*/bl_power)
                                    WAKE="1"
                                fi
                                ;;
                            overlay)
                                if [[ "${tftBacklightControl}" ]] ; then 
                                    if [[ "$(gpio -g read ${tftBacklightControl})"=="0" ]] && [[ "${WAKE}"=="0" ]] && [[ "${tftBacklightControl}" ]] ; then
                                        for i in {64..1023}; do gpio -g pwm "${tftBacklightControl}" $i; done
                                        WAKE="1"
                                    fi
                                fi
                                ;;
                        esac
                        ;;
                    systembrowsing)     
                        #get system
                        TFTESSystem="$(sed -n 's/^SystemId=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        #select system video
                        selectSystemVideo;
                        previousTFTGameBrowse=""
                        previousTFTGameRun=""
                        if [[ "$(pgrep -P ${scriptPID})" ]]; then
                            pkill -P "${scriptPID}" &> /dev/null
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                        fi
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                        "${FFMPEG}" -re -stream_loop -1 -i "${LogoFolder}"/"${tftResolution}"/"${LogoFile}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                        ;;
                    gamelistbrowsing|endgame|enddemo)
                        TFTGameImage="$(sed -n 's/^ImagePath=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        TFTVideoPath="$(sed -n 's/^VideoPath=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        TFTGame="$(sed -n 's/^Game=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        if [[ "${TFTGame}" != "${previousTFTGameBrowse}" ]]; then
                            previousTFTGameBrowse="${TFTGame}"
                            previousTFTESSystem=""
							if [[ "$(pgrep -P ${scriptPID})" ]]; then
								pkill -P "${scriptPID}" &> /dev/null
                                dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                                usleep "${tempoShort}"
                            fi
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                            if [[ -z "${TFTGameImage}" ]] ; then
                                TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                            fi
                            if [[ -z "${TFTVideoPath}" ]]; then 
								FRAMEBUFFER="${fbDevice}" fbv "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}" &> /dev/null &
                            else
								FRAMEBUFFER="${fbDevice}" fbv "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}" &> /dev/null &
                                usleep "${tempoLong}" #display image for a bit of time 
                                "${FFMPEG}" -re -stream_loop -1 -i "${TFTVideoPath}" -s "${tftFullResolution}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                            fi
                        fi
                        ;;
                    rungame|rundemo)
                        #get system
                        TFTESSystem="$(sed -n 's/^SystemId=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        #select system video
                        selectSystemVideo;
                        TFTGameImage="$(sed -n 's/^ImagePath=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        TFTVideoPath="$(sed -n 's/^VideoPath=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                        TFTGame="$(sed -n 's/^Game=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"

                        if [[ "${TFTGame}" != "${previousTFTGameRun}" ]]; then
                            previousTFTGameRun="${TFTGame}"
                            previousTFTESSystem=""
	                        if [[ "$(pgrep -P ${scriptPID})" ]]; then
	                            pkill -P "${scriptPID}" &> /dev/null
                                dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                                usleep "${tempoShort}"
                            fi
                            if [[ -z "${TFTGameImage}" ]] ; then
                                TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                            fi
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                            "${FFMPEG}" -re -i "${LogoFolder}"/"${tftResolution}"/"${LogoFile}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                            FRAMEBUFFER="${fbDevice}" fbv "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}" &> /dev/null
                        fi
                        ;;
                esac                
            fi
        done
    else
        echo "Small Second Screen not detected. Please install/configure it to get /dev/fb1 device"
    fi    
}

###################################
#      Main                       #
###################################

tftEnabled="$(${systemsetting}  -command load -key system.secondMiniTFT.enabled)"
if [ "$tftEnabled" != "0" ];then
    recallog "Starting $NAME"
    do_start
else
    recallog "Will not start $NAME : system.secondMiniTFT.enabled is set to 0"
fi
