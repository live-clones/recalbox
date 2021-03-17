#!/bin/bash

RA_CORE_OPTIONS=/recalbox/share/system/configs/retroarch/cores/retroarch-core-options.cfg

# remove old es_input.cfg as the joypad name has changed upstrem
rm /recalbox/share/system/.emulationstation/es_input.cfg

# disable visibility of pre-installed games to avoid double display of those games
grep -Eq '^\s*global.hidepreinstalledgames=' /recalbox/share/system/recalbox.conf || \
  echo -e '\nglobal.hidepreinstalledgames=1' >>/recalbox/share/system/recalbox.conf

# change retroarchcustom.cfg.odroidgo2 to fix shaders and core freeze
sed -i -E 's/^(\s*video_driver\s*=).*/\1 "gl"/' /recalbox/share/system/configs/retroarch/retroarchcustom.cfg.odroidgo2

# enable Vulkan GPU render (#1563)
sed -i -E 's/^\s*(duckstation_GPU.Renderer\s*=\s*).*/\1"Vulkan"/' "$RA_CORE_OPTIONS"

# change lr-mupen64plus-nx core config
sed -i -E '/^(mupen64plus-43screensize|mupen64plus-EnableCopyColorToRDRAM|mupen64plus-ThreadedRenderer)/d' "$RA_CORE_OPTIONS"
cat >>"$RA_CORE_OPTIONS" <<EOF
mupen64plus-43screensize = "320x240"
mupen64plus-EnableCopyColorToRDRAM = "Off"
mupen64plus-ThreadedRenderer = "True"
EOF
