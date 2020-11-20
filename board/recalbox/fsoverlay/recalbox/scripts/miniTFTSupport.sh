#!/bin/bash
###################################
# Authors: Ian57 (Recalbox) 2020  #
# Thanks to LittleBallup          #
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
tftResolution="$(${systemsetting} -command load -key system.secondminitft.resolution)"
recallog "Reading system.secondminitft.resolution => ${tftResolution}"
tftType="$(${systemsetting} -command load -key system.secondminitft.type)"
recallog "Reading system.secondminitft.type => ${tftType}"
tftBacklightControl="$(${systemsetting} -command load -key system.secondminitft.backlightcontrol)"
#recallog "Reading system.secondminitft.backlightcontrol  => ${tftBacklightControl}"
if [[ "${tftBacklightControl}" ]] ; then
    recallog "Reading system.secondminitft.backlightcontrol => ${tftBacklightControl}"
else
    recallog "Reading system.secondminitft.backlightcontrol => none (No backlight control)"
fi

tftSleepEnabled="$(${systemsetting} -command load -key system.secondminitft.sleepenabled)"
#recallog "Reading system.secondminitft.sleepenabled  => ${tftSleepEnabled}"
if [[ "${tftSleepEnabled}" == "1" ]] ; then
    recallog "Reading system.secondminitft.sleepenabled  => ${tftSleepEnabled} Sleep mode ON"
else
    recallog "Reading system.secondminitft.sleepenabled  => ${tftSleepEnabled} Sleep mode OFF"
fi

#system.secondminitft.usemarquee
tftUseMarquee="$(${systemsetting} -command load -key system.secondminitft.usemarquee)"
#recallog "Reading system.secondminitft.usemarquee  => ${tftUseMarquee}"
if [[ "${tftSleepEnabled}" == "1" ]] ; then
    recallog "Reading system.secondminitft.usemarquee  => ${tftUseMarquee} Use Marquee Image"
else
    recallog "Reading system.secondminitft.usemarquee  => ${tftUseMarquee} Do Not Use Marquee Image"
fi



#needed to use scaling with ffmpeg
case "${tftResolution}" in
    240p)    tftFullResolution=("-vf" "scale=320:-1,setdar=4/3,pad=width=320:height=240:x=0:y=15:color=black")
             tftScreenResolution="320x240"  ;;
    320p)    tftFullResolution=("-s" "480x320")
             tftScreenResolution="480x320"  ;;
esac
recallog "Full Resolution => ${tftFullResolution}"

#force to 320p to keep only 1 videos directory
#ffmpg will be used to resize
tftResolution=320p

#get image processing option fot fbv
imgStretchEnabled="$(${systemsetting} -command load -key system.secondminitft.imagestretchenabled)"
recallog "Reading system.secondminitft.imagestretchenabled => ${imgStretchEnabled}"
imgEnlargeEnabled="$(${systemsetting} -command load -key system.secondminitft.imageenlargeenabled)"
recallog "Reading system.secondminitft.imageenlargeenabled => ${imgEnlargeEnabled}"
imgAlphaEnabled="$(${systemsetting} -command load -key system.secondminitft.imagealphaenabled)"
recallog "Reading  system.secondminitft.imagealphaenabled => ${imgAlphaEnabled}"
imgIgnoreAspectEnabled="$(${systemsetting} -command load -key system.secondminitft.imageignoreaspectenabled)"
recallog "Reading system.secondminitft.imageignoreaspectenabled => ${imgIgnoreAspectEnabled}"


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
recallog "fbv2 options : ${imgStretch} ${imgEnlarge} ${imgIgnoreAspect} ${imgAlpha}"

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

        #Only if you are using a sounh hat. On rpi pwm cannot be used simultlaneously with sound
        #because sound is generated by PWM !
        if [[ "${tftBacklightControl}" ]] ; then
            #~ #backlight control...  pwm  on GPIO "${tftBacklightControl}"
            gpio -g mode "${tftBacklightControl}" pwm
            gpio -g pwm "${tftBacklightControl}" 1023
        else
            echo 0 | tee /sys/class/backlight/*/bl_power
        fi

        WAKE=1

        #display start video
        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
        #to save time intro video is backgrounded and killable with any action
        pkill -P "${scriptPID}" &> /dev/null

        "${FFMPEG}" -re -i "${LogoFolder}"/"${tftResolution}"/"${DefaultStartLogo}" "${tftFullResolution[@]}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null

        #get event from ES and senf it to fifo file
        if [ -f "/tmp/mqtt" ]; then
          rm /tmp/mqtt
        fi
        #create fifo file to receive event
        mkfifo /tmp/mqtt
        #run mqtt brocker
        mosquitto_sub -h 127.0.0.1 -p 1883 -q 0 -t Recalbox/EmulationStation/Event > /tmp/mqtt &
        # loop forever
        while true
        do
            #read fifo to get actions
            read -r TFTAction < /tmp/mqtt
            #recallog "TFTAction (read)  => ${TFTAction}"

            #getAction additionnal read to catch wakeup action
            esStateFile=$(</tmp/es_state.inf)
            TFTAction="$(sed -n 's/^Action=\([^\r]\+\)\r\?$/\1/p' <<< ${esStateFile})"
            #TFTAction="$(sed -n 's/^Action=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
            #recallog "TFTAction (sed) => ${TFTAction}"

            # action in TFTAction : systembrowsing, gamelistbrowsing, rungame, rundemo, endgame, enddemo
            # Used variables TFTGameImage, TFTVideoPath, TFTGame, TFTESSystem, TFTESState
            case "${TFTAction}" in
                sleep)
                    if [[ "${tftSleepEnabled}" == "1" ]] ; then
                    case "${tftType}" in
                        ogst)
                            if [[ "$(cat /sys/class/backlight/*/bl_power})"=="0" ]] && [[ "${WAKE}"=="1" ]] ; then
                                backLight=$(echo 1 | tee /sys/class/backlight/*/bl_power)
                                WAKE="0"
                            fi
                            ;;
                        overlay)
                            if [[ "${tftBacklightControl}" ]] ; then
                                if [[ "$(gpio -g read ${tftBacklightControl})"=="1" ]] && [[ "${WAKE}"=="1" ]] ; then
                                    for i in {1023..64}; do gpio -g pwm "${tftBacklightControl}" $i; done
                                    WAKE="0"
                                fi
                            else
                                if [[ "$(cat /sys/class/backlight/*/bl_power})"=="0" ]] && [[ "${WAKE}"=="1" ]] ; then
                                    backLight=$(echo 1 | tee /sys/class/backlight/*/bl_power)
                                    WAKE="0"
                                fi
                            fi
                            ;;
                    esac
                    fi
                    ;;
                wakeup)
                    #pkill -P "${scriptPID}" &> /dev/null
                    pkill ffmpeg &> /dev/null
                    #delete screen on wakeup
                    dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                    usleep "${tempoShort}"
                    if [[ "${tftSleepEnabled}" == "1" ]] ; then
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
                            else
                                if [[ "$(cat /sys/class/backlight/*/bl_power})"=="1" ]] && [[ "${WAKE}"=="0" ]] ; then
                                    backLight=$(echo 0 | tee /sys/class/backlight/*/bl_power)
                                    WAKE="1"
                                fi
                            fi
                            ;;
                    esac
                    fi
                    #recallog "Case wakeup LastTFTAction => ${LastTFTAction}"
                    case "${LastTFTAction}" in
                        systembrowsing)
                            TFTESSystem="${PreviousTFTESSystem}"
                            #recallog "PreviousTFTESSystem => ${PreviousTFTESSystem}"
                            #select system video
                            selectSystemVideo;
                            if [[ "$(pgrep -P ${scriptPID})" ]]; then
                                #pkill -P "${scriptPID}" &> /dev/null
                                pkill ffmpeg &> /dev/null
                                dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                                usleep "${tempoShort}"
                            fi
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                            "${FFMPEG}" -re -stream_loop "${Loop}" -i "${LogoFolder}"/"${tftResolution}"/"${LogoFile}"  "${tftFullResolution[@]}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                            ;;
                        gamelistbrowsing)
                            TFTGame="${previousTFTGameBrowse}"
                            TFTGameImage="${previousTFTGameImage}"
                            TFTVideoPath="${previousTFTVideoPath}"
                            #recallog "previousTFTGameBrowse => ${previousTFTGameBrowse}"
                            #recallog "previousTFTGameImage => ${previousTFTGameImage}"
                            #recallog "previousTFTVideoPath => ${previousTFTVideoPath}"
                            if [[ "$(pgrep -P ${scriptPID})" ]]; then
                                #pkill -P "${scriptPID}" &> /dev/null
                                pkill ffmpeg &> /dev/null
                                dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                                usleep "${tempoShort}"
                            fi
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                            if [[ -z "${TFTGameImage}" ]] ; then
                                TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                            fi
                            if [[ -z "${TFTVideoPath}" ]]; then
                                FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                            else
                                                                FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                                usleep "${tempoLong}" #display image for a bit of time
                                #compute display settings for scraped video
                                widthVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=width -of csv=s=x:p=0  ${TFTVideoPath})
                                #echo ${widthVideo}
                                heightVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=height,width -of csv=s=x:p=0  ${TFTVideoPath})
                                #echo ${heightVideo}
                                "${FFMPEG}" -re -stream_loop -1 -i "${TFTVideoPath}" -s "${tftScreenResolution}" -aspect 4:3 -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                            fi
                            ;;
                    esac
                    ;;
                systembrowsing|runkodi)
                    #store es_state.inf in a variable
                    esStateFile=$(</tmp/es_state.inf)
                    #get system
                    #TFTESSystem="$(sed -n 's/^SystemId=\([^\r]\+\)\r\?$/\1/p' /tmp/es_state.inf)"
                    TFTESSystem="$(sed -n 's/^SystemId=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    #recallog "TFTESSystem => ${TFTESSystem}"
                    PreviousTFTESSystem="${TFTESSystem}"
                    #recallog "PreviousTFTESSystem => ${PreviousTFTESSystem}"
                    #select system video
                    selectSystemVideo;
                    previousTFTGameBrowse=""
                    previousTFTGameRun=""
                    previousTFTGameImage=""
                    previousTFTVideoPath=""

                    if [[ "$(pgrep -P ${scriptPID})" ]]; then
                        #pkill -P "${scriptPID}" &> /dev/null
                        pkill ffmpeg &> /dev/null
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                    fi
                    dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                    usleep "${tempoShort}"
                    "${FFMPEG}" -re -stream_loop "${Loop}" -i "${LogoFolder}"/"${tftResolution}"/"${LogoFile}"  "${tftFullResolution[@]}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                    LastTFTAction=${TFTAction}
                    ;;
                gamelistbrowsing)
                    #store es_state.inf in a variable
                    esStateFile=$(</tmp/es_state.inf)
                    TFTGameImage="$(sed -n 's/^ImagePath=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    TFTVideoPath="$(sed -n 's/^VideoPath=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    TFTGame="$(sed -n 's/^Game=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    if [[ "${TFTGame}" != "${previousTFTGameBrowse}" ]]; then
                        previousTFTGameBrowse="${TFTGame}"
                        previousTFTGameImage="${TFTGameImage}"
                        previousTFTVideoPath="${TFTVideoPath}"
                        if [[ "$(pgrep -P ${scriptPID})" ]]; then
                            #pkill -P "${scriptPID}" &> /dev/null
                            pkill ffmpeg &> /dev/null
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                        fi
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                        if [[ -z "${TFTGameImage}" ]] ; then
                            TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                        fi
                        if [[ -z "${TFTVideoPath}" ]]; then
                            FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                        else
                            FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                            usleep "${tempoLong}" #display image for a bit of time
                            #compute display settings for scraped video
                            widthVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=width -of csv=s=x:p=0  ${TFTVideoPath})
                            #echo ${widthVideo}
                            heightVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=height,width -of csv=s=x:p=0  ${TFTVideoPath})
                            #echo ${heightVideo}
                            "${FFMPEG}" -re -stream_loop -1 -i "${TFTVideoPath}" -s "${tftScreenResolution}" -aspect 4:3 -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                        fi
                    fi
                    LastTFTAction=${TFTAction}
                    ;;
                endgame|enddemo)
                    TFTGame="${previousTFTGameBrowse}"
                    TFTGameImage="${previousTFTGameImage}"
                    TFTVideoPath="${previousTFTVideoPath}"
                    #recallog "previousTFTGameBrowse => ${previousTFTGameBrowse}"
                    #recallog "previousTFTGameImage => ${previousTFTGameImage}"
                    #recallog "previousTFTVideoPath => ${previousTFTVideoPath}"
                    if [[ "$(pgrep -P ${scriptPID})" ]]; then
                        #pkill -P "${scriptPID}" &> /dev/null
                        pkill ffmpeg &> /dev/null
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                    fi
                    dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                    usleep "${tempoShort}"
                    if [[ -z "${TFTGameImage}" ]] ; then
                        TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                    fi
                    if [[ -z "${TFTVideoPath}" ]]; then
                        FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                    else
                        FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                        usleep "${tempoLong}" #display image for a bit of time
                        #compute display settings for scraped video
                        widthVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=width -of csv=s=x:p=0  ${TFTVideoPath})
                        #echo ${widthVideo}
                        heightVideo=$(ffprobe -v error -select_streams v:0 -show_entries stream=height,width -of csv=s=x:p=0  ${TFTVideoPath})
                        #echo ${heightVideo}
                        "${FFMPEG}" -re -stream_loop -1 -i "${TFTVideoPath}" -s "${tftScreenResolution}" -aspect 4:3 -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null &
                    fi
                    ;;
                rungame|rundemo)
                    #store es_state.inf in a variable
                    esStateFile=$(</tmp/es_state.inf)
                    #get system
                    TFTESSystem="$(sed -n 's/^SystemId=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    #select system video
                    selectSystemVideo;
                    TFTGameImage="$(sed -n 's/^ImagePath=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    TFTGameMarquee="${TFTGameImage/images/marquees}"
                    TFTVideoPath="$(sed -n 's/^VideoPath=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    TFTGame="$(sed -n 's/^Game=\([^\r]\+\)\r\?$/\1/p'  <<< ${esStateFile})"
                    if [[ "${TFTGame}" != "${previousTFTGameRun}" ]]; then
                        previousTFTGameRun="${TFTGame}"
                        if [[ "$(pgrep -P ${scriptPID})" ]]; then
                            #pkill -P "${scriptPID}" &> /dev/null
                            pkill ffmpeg &> /dev/null
                            dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                            usleep "${tempoShort}"
                        fi
                        if [[ "${tftUseMarquee}" == "1" ]] && [[ -f "${TFTGameMarquee}" ]] ; then
                             TFTGameImage=${TFTGameMarquee}
                        else
                            if [[ -z "${TFTGameImage}" ]] ; then
                                TFTGameImage="${LogoFolder}"/"${tftResolution}"/"${DefaultImage}"
                            fi
                        fi
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                        timeout 10 "${FFMPEG}" -re -i "${LogoFolder}"/"${tftResolution}"/"${LogoFile}" "${tftFullResolution[@]}" -c:v rawvideo -pix_fmt rgb565le -f fbdev "${fbDevice}" &> /dev/null
                        dd if=/dev/zero of="${fbDevice}" &> /dev/null   #clear before draw
                        usleep "${tempoShort}"
                        FRAMEBUFFER="${fbDevice}" fbv2 -i "${imgStretch}" "${imgIgnoreAspect}" "${imgEnlarge}" "${imgAlpha}" --delay 1 "${TFTGameImage}"
                    fi
                ;;
            esac
        done
        mosquitto_sub -U Recalbox/EmulationStation/Event
        mosquitto_sub -E
    else
        echo "Small Second Screen not detected. Please install/configure it to get /dev/fb1 device"
    fi
}

###################################
#      Main                       #
###################################

do_start
