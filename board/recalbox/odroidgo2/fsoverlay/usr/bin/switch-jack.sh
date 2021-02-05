#!/bin/bash

device='/dev/input/by-path/platform-rk817-sound-event'

extract='*0'
insert='*1'

evtest "$device" | while read -r line; do
    case $line in
        ($extract)    amixer cset numid=1,iface=MIXER,name='Playback Path' SPK ;;
        ($insert)     amixer cset numid=1,iface=MIXER,name='Playback Path' HP ;;
    esac
done

