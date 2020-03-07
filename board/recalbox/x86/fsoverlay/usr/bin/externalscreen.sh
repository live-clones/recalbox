#!/bin/bash

export DISPLAY=:0

xrandrOutput=$(xrandr)

PRIMARY=$(echo "${xrandrOutput}" | grep "connected primary" | awk '{print $1}')
EXTERNAL=($(echo "${xrandrOutput}" | grep "connected" | grep -vE "(disconnected|connected primary)" | awk '{print $1}'))
##-4k tv need resize screen because 3840 x 2160 in 30fps with retroarch cores
UHDTV=($(echo "${xrandrOutput}" | grep "3840x2160" | grep "connected" | awk '{print $1}'))

echo "Primary screen:"
echo "  ${PRIMARY}"

echo "Other screens (${#EXTERNAL[*]}):"

for screen in ${EXTERNAL[*]}; do
echo "  ${screen}"
done

if [ -n "$UHDTV" ] && [ -n "$PRIMARY" ]; then
echo "4k tv found need resize on 1080p 60fps"
xrandr --output $UHDTV --mode "1920x1080"
fi

if [ -n "$EXTERNAL" ] && [ -z "$UHDTV" ]; then
echo "change to external screen"
xrandr --output $PRIMARY --off --output $EXTERNAL --auto
elif [ -n "$EXTERNAL" ] && [ -n "$UHDTV" ]; then
echo "4k tv found need resize on 1080p 60fps"
xrandr --output $PRIMARY --off --output $EXTERNAL --mode "1920x1080"
else
echo "no external screen found" 
xrandr --output $PRIMARY --auto
fi
