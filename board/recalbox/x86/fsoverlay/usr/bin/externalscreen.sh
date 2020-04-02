#!/bin/bash

export DISPLAY=:0

#Prefered output screen got from recalbox.conf
OUTPUT_PREFERED=$(grep '^system.selected.external_screen=' /recalbox/share/system/recalbox.conf | awk 'BEGIN {FS="="} {print $2}')

xrandrOutput=$(xrandr)

OUTPUT_RESOLUTION=$(echo "${xrandrOutput}" | grep " connected" | sed 's/ primary//;s/^\(.*\) connected \([0-9]*x[0-9]*\).*$/\1=\2/')
echo "Connected screen(s):"
for screen in ${OUTPUT_RESOLUTION[*]}; do
  echo "  ${screen}"
done

if [ -n "$OUTPUT_PREFERED" ]; then
  PREFERED_CONNECTED=$(echo "$xrandrOutput" | grep "$OUTPUT_PREFERED connected" | awk '{print $1}')
  if [ -n "$PREFERED_CONNECTED" ]; then
    echo "Prefered screen $OUTPUT_PREFERED"
  else
    echo "Prefered screen $OUTPUT_PREFERED disconnected"
  fi
fi

if [ -n "$OUTPUT_RESOLUTION" ]; then
  if [[ -n "$OUTPUT_PREFERED" ]] && [[ -n "$PREFERED_CONNECTED" ]]; then
    SELECTED_OUTPUT=$PREFERED_CONNECTED
  else
    SELECTED_OUTPUT=$(echo "$OUTPUT_RESOLUTION" | awk 'BEGIN {FS="="} NR==1 {print $1}')
  fi
  
  echo "Force selected output $SELECTED_OUTPUT to FHD if UHD or WQHD, disable others"
  XRANDR_CMD=$(echo "$OUTPUT_RESOLUTION" | awk -v SELECTED_OUTPUT="$SELECTED_OUTPUT" 'BEGIN {FS="="; printf "xrandr"} $1==SELECTED_OUTPUT {if (($2=="3840x2160")||($2=="2560x1440")) printf " --output " $1 " --mode 1920x1080"; else printf " --output " $1 " --auto";} $1!=SELECTED_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}') 
  echo "$XRANDR_CMD"
  $($XRANDR_CMD)
fi
