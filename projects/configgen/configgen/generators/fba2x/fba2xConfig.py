#!/usr/bin/env python
import sys
import os
import settings.fbasettings as fbasettings

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


# return true if the option is considered enabled (for boolean options)
def enabled(key, dict):
    return key in dict and (dict[key] == '1' or dict[key] == 'true')


# return true if the option is considered defined
def defined(key, dict):
    return key in dict and isinstance(dict[key], str) and len(dict[key]) > 0


ratioIndexes = {'16/9': '1', '4/3': '0'}

def writeLibretroConfig(system):
    writeLibretroConfigToFile(createLibretroConfig(system))


# take a system, and returns a dict of retroarch.cfg compatible parameters
def createLibretroConfig(system):
    retroarchConfig = dict()
    recalboxConfig = system.config
    if enabled('smooth', recalboxConfig):
        retroarchConfig['video_smooth'] = 'true'
    else:
        retroarchConfig['video_smooth'] = 'false'

    if defined('shaders', recalboxConfig):
        retroarchConfig['video_shader'] = recalboxConfig['shaders']
        retroarchConfig['video_shader_enable'] = 'true'
        retroarchConfig['video_smooth'] = 'false'
    else:
        retroarchConfig['video_shader_enable'] = 'false'

    if defined('ratio', recalboxConfig) and recalboxConfig['ratio'] in ratioIndexes:
        retroarchConfig['aspect_ratio_index'] = ratioIndexes[recalboxConfig['ratio']]
        retroarchConfig['video_aspect_ratio_auto'] = 'false'
    else:
        retroarchConfig['video_aspect_ratio_auto'] = 'true'

    if enabled('rewind', recalboxConfig):
        retroarchConfig['rewind_enable'] = 'true'
    else:
        retroarchConfig['rewind_enable'] = 'false'

    return retroarchConfig

def writeLibretroConfigToFile(config):
    for setting in config:
        libretroSettings.save(setting, config[setting])
