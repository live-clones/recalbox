#!/usr/bin/env python
from typing import Dict

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.unixSettings import UnixSettings

fbaSettings = UnixSettings(recalboxFiles.fbaCustom)

# return true if the option is considered enabled (for boolean options)
def enabled(key: str, dictio: Dict[str, str]):
    return key in dictio and dictio[key] in ('1', 'true')

# return true if the option is considered defined
def defined(key: str, dictio: Dict[str, str]):
    return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

def writeFBAConfigToFile(config: Dict[str, str]):
    for setting in config:
        fbaSettings.save(setting, config[setting])

def writeFBAConfig(system: Emulator):
    writeFBAConfigToFile(createFBAConfig(system))

# take a system, and returns a dict of retroarch.cfg compatible parameters
def createFBAConfig(system: Emulator) -> Dict[str, str]:
    ratioIndexes = { '16/9': '0', '4/3': '1' }
    fbaConfig = dict()
    fbaConfig['DisplaySmoothStretch'] = '1' if system.Smooth else '0'
    fbaConfig['MaintainAspectRatio'] = ratioIndexes[system.Ratio] if system.Ratio in ratioIndexes else '1'
    fbaConfig['DisplayEffect'] = '1' if system.ShaderSet == 'scanlines' else '0'

    return fbaConfig

