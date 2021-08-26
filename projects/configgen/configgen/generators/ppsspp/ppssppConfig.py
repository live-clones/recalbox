#!/usr/bin/env python
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.unixSettings import UnixSettings

ppssppSettings = UnixSettings(recalboxFiles.ppssppConfig, separator=' ')

# return true if the option is considered enabled (for boolean options)
def enabled(key, dictio):
    return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')


# return true if the option is considered defined
def defined(key, dictio):
    return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0


def writePPSSPPConfig(system):
    writePPSSPPConfigToFile(createPPSSPPConfig(system))


def createPPSSPPConfig(system: Emulator):
    ppssppConfig = dict()
    # Display FPS
    ppssppConfig['ShowFPSCounter'] = '3' if  system.ShowFPS else '0' # 1 for Speed%, 2 for FPS, 3 for both

    return ppssppConfig


def writePPSSPPConfigToFile(config):
    for setting in config:
        ppssppSettings.save(setting, config[setting])
