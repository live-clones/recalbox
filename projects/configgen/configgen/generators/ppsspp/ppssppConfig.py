#!/usr/bin/env python
import sys
import os
import recalboxFiles
from settings.unixSettings import UnixSettings

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

ppssppSettings = UnixSettings(recalboxFiles.ppssppConfig, separator=' ')

# return true if the option is considered enabled (for boolean options)
def enabled(key, dictio):
    return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')


# return true if the option is considered defined
def defined(key, dictio):
    return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0
    

def writePPSSPPConfig(system):
    writePPSSPPConfigToFile(createPPSSPPConfig(system))

def createPPSSPPConfig(system):
    ppssppConfig = dict()
    recalboxConfig = system.config
    # Display FPS
    if enabled('showFPS', recalboxConfig):
        ppssppConfig['ShowFPSCounter'] = '3' # 1 for Speed%, 2 for FPS, 3 for both
    else:
        ppssppConfig['ShowFPSCounter'] = '0'

    return ppssppConfig


def writePPSSPPConfigToFile(config):
    for setting in config:
        ppssppSettings.save(setting, config[setting])
