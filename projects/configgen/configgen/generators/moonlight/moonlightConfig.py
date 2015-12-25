#!/usr/bin/env python
import sys
import os
import recalboxFiles
from settings.unixSettings import UnixSettings

#
# Besoin de :
#   - voir s'il existe un fichier de conf pour le jeu
## Hostname or IP-address of host to connect to
## By default host is autodiscovered using mDNS
#address = 1.2.3.4

## Video streaming configuration
#width = 1280
#height = 720
#fps = 60

## Bitrate depends by default on resolution and fps
## Set to -1 to enable default
## 20Mbps (20000) for 1080p (60 fps)
## 10Mbps (10000) for 1080p or 60 fps
## 5Mbps (5000) for lower resolution or fps
#bitrate = -1

## Size of network packets should be lower than MTU
#packetsize = 1024

## Default started application on host
#app = Steam

## Default used mapping for streaming
## Searched for in $XDG_DATA_DIRS/moonlight or /usr/share/moonlight and /usr/local/share/moonlight
## Mapping can also be user overrided in $XDG_CONFIG_DIR/moonlight or ~/.config/moonlight or current directory
#mapping = mappings/default.conf

## Enable selected input devices
## By default all available input devices should be used
## Only evdev devices /dev/input/event* are allowed
## To use a different mapping then default another mapping should be declared above the input
#input = /dev/input/event1

## Let GFE change graphical game settings for optimal performance and quality
#sops = true

## Play audio on host instead of streaming to client
#localaudio = false

## Select audio device to play sound on
#audio = sysdefault

## Select the audio and video decoder to use
## default - autodetect
## omx - hardware video decoder for Raspberry Pi
## imx - hardware video decoder for i.MX6 devices
## sdl - software decoder
## fake - save to file (only available in debug builds)
#platform = default

## Directory to store encryption keys
## By default keys are stored in $XDG_CACHE_DIR/moonlight or ~/.cache/moonlight
#keydir = /dir/to/keys

## Load additional configuration files
#config = /path/to/config






# take a system, and returns a dict of retroarch.cfg compatible parameters
def createFBAConfig(system):
    fbaConfig = dict()
    recalboxConfig = system.config
    if enabled('smooth', recalboxConfig):
        fbaConfig['DisplaySmoothStretch'] = '1'
    else:
        fbaConfig['DisplaySmoothStretch'] = '0'

    if defined('ratio', recalboxConfig) and recalboxConfig['ratio'] in ratioIndexes:
        fbaConfig['MaintainAspectRatio'] = ratioIndexes[recalboxConfig['ratio']]
    else:
        fbaConfig['MaintainAspectRatio'] = '1'

    if defined('shaders', recalboxConfig) and recalboxConfig['shaders'] == 'scanlines':
        fbaConfig['DisplayEffect'] = '1'
    else :
        fbaConfig['DisplayEffect'] = '0'

    return fbaConfig


def writeFBAConfigToFile(config):
    for setting in config:
        fbaSettings.save(setting, config[setting])
