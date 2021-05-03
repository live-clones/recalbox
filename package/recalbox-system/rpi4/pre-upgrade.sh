#!/bin/bash

mount -o remount,rw /boot

if [ ! -f /boot/recalbox-user-config.txt ]; then
  cat >/boot/recalbox-user-config.txt <<EOF
# Change to your needs

# uncomment if you get no picture on HDMI for a default "safe" mode
#hdmi_safe=1

disable_overscan=1

# uncomment to force a specific HDMI mode (this will force VGA)
#hdmi_group=1
#hdmi_mode=1

# Sound output. Set to 0 or comment for autodetect, 1 for DVI, 2 to force HDMI.
#hdmi_drive=2

config_hdmi_boost=0

# force hdmi while the tv can take time before sending the signal on the hdmi output
hdmi_force_hotplug=1

# uncomment for composite PAL
#sdtv_mode=2

# uncomment for lirc-rpi
#dtoverlay=lirc-rpi

# uncomment if you have chinese TV display and display is garbled or slow
#hdmi_ignore_edid=0xa5000080
EOF
fi

# force hdmi audio output
# ES should select best port on start
sed -i \
    -e '/^\(audio.device=\).*/{s//\1alsa_card.1:hdmi-output-0/;:a;n;ba;q}' \
    -e '$aaudio.device=alsa_card.1:hdmi-output-0' /recalbox/share/system/recalbox.conf

# Force VSync on PPSSPP
# If file is not here, we let the user take care of it 
PPSSPPCONFIGILE="/recalbox/share/system/configs/ppsspp/PSP/SYSTEM/ppsspp.ini"
if [ -f "${PPSSPPCONFIGILE}" ]; then
  sed -i \
    -e 's/^VSyncInterval\s*=.*/VSyncInterval = True/' \
    "${PPSSPPCONFIGILE}"
fi
