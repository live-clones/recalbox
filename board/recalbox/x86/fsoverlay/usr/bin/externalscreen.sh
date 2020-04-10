#!/bin/bash

export DISPLAY=:0

xrandrOutput=$(xrandr)

OUTPUT_MAXRES=$(echo "${xrandrOutput}" | awk '$1!~"[0-9]*x[0-9]*" {OUTPUT=$1; RESO=1;} $1~"[0-9]*x[0-9]*" && RESO==1 {print OUTPUT "=" $1; RESO++;}')
if [ -z "$OUTPUT_MAXRES" ]; then
  echo "No screen connected..."
  exit
else
  echo "Connected screen(s):"
  for screen in ${OUTPUT_MAXRES[*]}; do
    echo "  ${screen}"
  done
fi

#Prefered output screen and force resolution from recalbox.conf
PREFERED=$(grep '^system.external_screen.prefered=' /recalbox/share/system/recalbox.conf | awk 'BEGIN {FS="="} {print $2}')
FORCE_RES=$(grep '^system.external_screen.force_resolution=' /recalbox/share/system/recalbox.conf | awk 'BEGIN {FS="="} {print $2}')

if [ -n "$PREFERED" ]; then
  PREFERED_CONNECTED=$(echo "$OUTPUT_MAXRES" | grep "$PREFERED" | awk 'BEGIN {FS="="} {print $1}')
fi
if [[ -n "$PREFERED" ]] && [[ -n "$PREFERED_CONNECTED" ]]; then
  SEL_OUTPUT="$PREFERED"
else
  SEL_OUTPUT=$(echo "$OUTPUT_MAXRES" | awk 'BEGIN {FS="="} NR==1 {print $1}')
fi
if [ -n "$PREFERED" ]; then
  if [ -n "$PREFERED_CONNECTED" ]; then
    echo "Prefered screen $PREFERED"
  else
    echo "Prefered screen $PREFERED disconnected, fallback to first connected screen $SEL_OUTPUT"
  fi
fi
  
if [ -n "$FORCE_RES" ]; then
  FORCE_RES_EXISTS=$(echo "${xrandrOutput}" | awk -v SEL_OUTPUT="$SEL_OUTPUT" -v FORCE_RES="$FORCE_RES" '$1!~"[0-9]*x[0-9]*" {OUTPUT=$1; RESO=1;} ($1~"[0-9]*x[0-9]*") && (OUTPUT==SEL_OUTPUT) && ($1==FORCE_RES) {print $1; RESO++;}')
  if [ -n "$FORCE_RES_EXISTS" ]; then
    echo "Force selected output $SEL_OUTPUT to $FORCE_RES, disable others"
    XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" -v FORCE_RES="$FORCE_RES" 'BEGIN {FS="="; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --mode " FORCE_RES;} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}') 
  else
    echo "Force resolution inexistant for selected output $SEL_OUTPUT, fallback to auto, disable other outputs"
	XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS="="; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --auto";} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}')
  fi
else
  echo "Force selected output $SEL_OUTPUT, disable others"
  XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS="="; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --auto";} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}') 
fi
echo "$XRANDR_CMD"
$($XRANDR_CMD)
