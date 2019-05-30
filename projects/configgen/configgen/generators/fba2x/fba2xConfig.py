#!/usr/bin/env python
import sys
import os
import recalboxFiles
from settings.unixSettings import UnixSettings

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


fbaSettings = UnixSettings(recalboxFiles.fbaCustom)

# return true if the option is considered enabled (for boolean options)
def enabled(key, dictio):
    return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')


# return true if the option is considered defined
def defined(key, dictio):
    return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0


ratioIndexes = {'16/9': '0', '4/3': '1'}


def writeFBAConfig(system):
    writeFBAConfigToFile(createFBAConfig(system))


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
