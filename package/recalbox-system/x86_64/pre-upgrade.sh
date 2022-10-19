#!/bin/bash

# Remove old dats files from bios
rm -rf /recalbox/share/bios/dc/*.dat.zip
rm -rf /recalbox/share/bios/mame/*.dat.zip

# Remove all .nvmem and .nvmem2 files from saves
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem2

# Remove shaders files
rm -rf /recalbox/share/shaders/crt-pi.glslp

## Move cfg config for cdi-2015
mkdir -p /recalbox/share/system/configs/cdi/cdi2015
mv /recalbox/share/saves/cdi/cdi2015/cfg /recalbox/share/system/configs/cdi2015/