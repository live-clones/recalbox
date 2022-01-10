#!/bin/bash

# Remove old dats files from bios
rm -rf /recalbox/share/bios/dc/*.dat.zip
rm -rf /recalbox/share/bios/mame/*.dat.zip

# Remove all .nvmem and .nvmem2 files from saves
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem
rm -rf /recalbox/share/saves/atomiswave/reicast/*.nvmem2

# Remove shaders files
rm -rf /recalbox/share/shaders/crt-pi.glslp