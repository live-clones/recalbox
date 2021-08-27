#!/usr/bin/env python
from typing import Dict

from configgen.Emulator import Emulator

# return true if the option is considered enabled (for boolean options)
from configgen.settings.iniSettings import IniSettings

sectionGraphics = '[Graphics]'

def writeFBAConfig(system: Emulator, fbaSettings: IniSettings):
    config: Dict[str, int] = createFBAConfig(system)
    for setting in config:
        fbaSettings.setInt(sectionGraphics, setting, config[setting])

# take a system, and returns a dict of retroarch.cfg compatible parameters
def createFBAConfig(system: Emulator) -> Dict[str, int]:
    ratioIndexes = { '16/9': 0, '4/3': 1 }
    fbaConfig: Dict[str, int] = dict()
    fbaConfig['DisplaySmoothStretch'] = 1 if system.Smooth else 0
    fbaConfig['MaintainAspectRatio'] = ratioIndexes[system.Ratio] if system.Ratio in ratioIndexes else 1
    fbaConfig['DisplayEffect'] = 1 if system.ShaderSet == 'scanlines' else 0

    return fbaConfig

