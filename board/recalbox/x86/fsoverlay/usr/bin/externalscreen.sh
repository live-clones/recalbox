#!/bin/bash

export DISPLAY=:0

xrandrOutput=$(xrandr)

OUTPUT_MAXRES=$(echo "${xrandrOutput}" | awk '$1!~"[0-9]*x[0-9]*i?" {if (MAXRES!="") {print OUTPUT ";" MAXRES ";" FALLBACKRES;} OUTPUT=$1; MAXRES=""; FALLBACKRES=""; NBRESO=1} $1~"[0-9]*x[0-9]*i?" {if (NBRESO==1) {MAXRES=$1; NBRESO++;} HEIGHT=$1; gsub(/^[0-9]+x/,"",HEIGHT); if ((FALLBACKRES=="") && (HEIGHT+0<=1080)) {FALLBACKRES=$1;}} END {if (MAXRES!="") {print OUTPUT ";" MAXRES ";" FALLBACKRES;}}')
if [ -z "$OUTPUT_MAXRES" ]; then
  echo "No screen connected..."
  exit
else
  echo "Connected screen(s) / Max resolution / Fallback resolution lower than FullHD:"
  for screen in ${OUTPUT_MAXRES[*]}; do
    echo "  ${screen}"
  done
fi

#Prefered output screen and force resolution from recalbox.conf
PREFERED=$(recalbox_settings -command load -key system.externalscreen.prefered)
FORCE_RES=$(recalbox_settings -command load -key system.externalscreen.forceresolution)
FORCE_FREQ=$(recalbox_settings -command load -key system.externalscreen.forcefrequency)

if [ -n "$PREFERED" ]; then
  PREFERED_CONNECTED=$(echo "$OUTPUT_MAXRES" | grep "$PREFERED" | awk 'BEGIN {FS=";"} {print $1}')
fi
if [[ -n "$PREFERED" ]] && [[ -n "$PREFERED_CONNECTED" ]]; then
  SEL_OUTPUT="$PREFERED"
else
  SEL_OUTPUT=$(echo "$OUTPUT_MAXRES" | awk 'BEGIN {FS=";"} NR==1 {print $1}')
fi
if [ -n "$PREFERED" ]; then
  if [ -n "$PREFERED_CONNECTED" ]; then
    echo "Prefered screen $PREFERED"
  else
    echo "Prefered screen $PREFERED disconnected, fallback to first connected screen $SEL_OUTPUT"
  fi
fi
  
if [ -n "$FORCE_RES" ]; then
  FORCE_RES_EXISTS=$(echo "${xrandrOutput}" | awk -v SEL_OUTPUT="$SEL_OUTPUT" -v FORCE_RES="$FORCE_RES" '$1!~"[0-9]*x[0-9]*i?" {OUTPUT=$1; RESO=1;} ($1~"[0-9]*x[0-9]*i?") && (OUTPUT==SEL_OUTPUT) && ($1==FORCE_RES) {print $1; RESO++;}')
  if [ -n "$FORCE_RES_EXISTS" ]; then
    FORCE_FREQ_OPTION=$(echo "${xrandrOutput}" | awk -v SEL_OUTPUT="$SEL_OUTPUT" -v FORCE_RES="$FORCE_RES" -v FORCE_FREQ="$FORCE_FREQ" '$1!~"[0-9]*x[0-9]*i?" {OUTPUT=$1; RESO=1;} ($1~"[0-9]*x[0-w9]*i?") && (OUTPUT==SEL_OUTPUT) && ($1==FORCE_RES) {for (i==2; i<=NF; i++) { gsub("*","",$i); if ($i==FORCE_FREQ) print " --refresh " $i;}  RESO++;}')
    if [ -n "$FORCE_FREQ_OPTION" ]; then
      echo "Force selected output $SEL_OUTPUT to $FORCE_RES at frequence $FORCE_FREQ, disable others"
    else
      echo "Force selected output $SEL_OUTPUT to $FORCE_RES, disable others"
    fi
    XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" -v FORCE_RES="$FORCE_RES" -v FORCE_FREQ_OPTION="$FORCE_FREQ_OPTION" 'BEGIN {FS=";"; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --mode " FORCE_RES FORCE_FREQ_OPTION;} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}')
  else
    SEL_OUTPUT_VERT_PIX=$(echo "$OUTPUT_MAXRES" | grep "$SEL_OUTPUT" | sed 's/^.*x\([0-9]*\);.*$/\1/')
    if [ "$SEL_OUTPUT_VERT_PIX" -gt "1080" ]; then
      echo "Force resolution inexistant for selected output $SEL_OUTPUT at fallback resolution, disable other outputs"
      XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS=";"; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --mode " $3;} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}')
    else
      echo "Force resolution inexistant for selected output $SEL_OUTPUT, fallback to auto, disable other outputs"
      XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS=";"; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --auto";} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}')
    fi
  fi
else
  SEL_OUTPUT_VERT_PIX=$(echo "$OUTPUT_MAXRES" | grep "$SEL_OUTPUT" | sed 's/^.*x\([0-9]*\);.*$/\1/')
  if [ "$SEL_OUTPUT_VERT_PIX" -gt "1080" ]; then
    echo "Force selected output $SEL_OUTPUT limited to fallback resolution, disable others"
    XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS=";"; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --mode " $3;} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}') 
  else
    echo "Force selected output $SEL_OUTPUT, disable others"
    XRANDR_CMD=$(echo "$OUTPUT_MAXRES" | awk -v SEL_OUTPUT="$SEL_OUTPUT" 'BEGIN {FS=";"; printf "xrandr"} $1==SEL_OUTPUT {printf " --output " $1 " --auto";} $1!=SEL_OUTPUT {printf " --output " $1 " --off";} END {printf "\n";}') 
  fi
fi

echo "$XRANDR_CMD"
$($XRANDR_CMD)
