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
EOF
fi

# Remove old dats files from bios
rm -rf /recalbox/share/bios/dc/*.dat.zip
rm -rf /recalbox/share/bios/mame/*.dat.zip

# Remove all .nvmem and .nvmem2 files from saves
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem2

# Remove shaders files
rm -rf /recalbox/share/shaders/crt-pi.glslp