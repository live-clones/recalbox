#!/usr/bin/env python
import sys
import os
import recalboxFiles
import settings
from settings.unixSettings import UnixSettings

sys.path.append(
    os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

libretroSettings = UnixSettings(recalboxFiles.retroarchCustom, separator=' ')
coreSettings = UnixSettings(recalboxFiles.retroarchCoreCustom, separator=' ')


# return true if the option is considered enabled (for boolean options)
def enabled(key, dict):
    return key in dict and (dict[key] == '1' or dict[key] == 'true')


# return true if the option is considered defined
def defined(key, dict):
    return key in dict and isinstance(dict[key], str) and len(dict[key]) > 0


# Warning the values in the array must be exactly at the same index than
# https://github.com/libretro/RetroArch/blob/master/gfx/video_driver.c#L237
ratioIndexes = ["4/3", "16/9", "16/10", "16/15", "1/1", "2/1", "3/2", "3/4", "4/1", "4/4", "5/4", "6/5", "7/9", "8/3",
                "8/7", "19/12", "19/14", "30/17", "32/9", "config", "squarepixel"]


# Define the libretro device type corresponding to the libretro cores, when needed.
coreToP1Device = {'cap32': '513', '81': '257', 'fuse': '513', 'atari800': '513'};
coreToP2Device = {'fuse': '513', 'atari800': '513'};

# Define systems compatible with retroachievements
systemToRetroachievements = {'snes', 'nes', 'gba', 'gb', 'gbc', 'megadrive', 'pcengine', 'atari2600', 'fba_libretro', 'neogeo', 'virtualboy', 'ngp', 'ngpc', 'lynx', 'mastersystem', 'gamegear', 'n64', 'satellaview', 'sufami', 'neogeocd', 'colecovision', 'atari7800'};

# Define systems not compatible with rewind option
systemNoRewind = {'virtualboy', 'sega32x', 'segacd', 'psx', 'fba_libretro', 'vectrex', 'zxspectrum', 'mame', 'n64', '3do', 'jaguar', 'intellivision', 'channelf', 'neogeocd', 'dreamcast', 'atomiswave', 'naomi', 'ngpc', 'pcfx'};

# Netplay modes
systemNetplayModes = {'host', 'client'}

def writeLibretroConfig(system, controllers):
    writeLibretroConfigToFile(createLibretroConfig(system, controllers))


# take a system, and returns a dict of retroarch.cfg compatible parameters
def createLibretroConfig(system, controllers):
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

    if defined('ratio', recalboxConfig):
        if recalboxConfig['ratio'] in ratioIndexes:
            retroarchConfig['aspect_ratio_index'] = ratioIndexes.index(recalboxConfig['ratio'])
            retroarchConfig['video_aspect_ratio_auto'] = 'false'
        elif recalboxConfig['ratio'] == "custom":
            retroarchConfig['video_aspect_ratio_auto'] = 'false'
        else:
            retroarchConfig['video_aspect_ratio_auto'] = 'true'
            retroarchConfig['aspect_ratio_index'] = ''

    retroarchConfig['rewind_enable'] = 'false'

    if enabled('rewind', recalboxConfig):
        if(not system.name in systemNoRewind):
            retroarchConfig['rewind_enable'] = 'true'
    else:
        retroarchConfig['rewind_enable'] = 'false'

    if enabled('autosave', recalboxConfig):
        retroarchConfig['savestate_auto_save'] = 'true'
        retroarchConfig['savestate_auto_load'] = 'true'
    else:
        retroarchConfig['savestate_auto_save'] = 'false'
        retroarchConfig['savestate_auto_load'] = 'false'

    if defined('inputdriver', recalboxConfig):
        retroarchConfig['input_joypad_driver'] = recalboxConfig['inputdriver']
    else:
        retroarchConfig['input_joypad_driver'] = 'udev'

    retroarchConfig['savestate_directory'] = recalboxFiles.savesDir + system.name
    retroarchConfig['savefile_directory'] = recalboxFiles.savesDir + system.name

    retroarchConfig['input_libretro_device_p1'] = '1'
    retroarchConfig['input_libretro_device_p2'] = '1'

    if(system.config['core'] in coreToP1Device):
        retroarchConfig['input_libretro_device_p1'] = coreToP1Device[system.config['core']]

    if(system.config['core'] in coreToP2Device):
        retroarchConfig['input_libretro_device_p2'] = coreToP2Device[system.config['core']]

    if len(controllers) > 2 and system.config['core'] == 'snes9x_next':
        retroarchConfig['input_libretro_device_p2'] = '257'

    retroarchConfig['cheevos_enable'] = 'false'
    retroarchConfig['cheevos_hardcore_mode_enable'] = 'false'

    if enabled('retroachievements', recalboxConfig):
        if(system.name in systemToRetroachievements):
            retroarchConfig['cheevos_enable'] = 'true'
            retroarchConfig['cheevos_username'] = recalboxConfig.get('retroachievements.username', "")
            retroarchConfig['cheevos_password'] = recalboxConfig.get('retroachievements.password', "")
            if enabled('retroachievements.hardcore', recalboxConfig):
                retroarchConfig['cheevos_hardcore_mode_enable'] = 'true'
            else:
                retroarchConfig['cheevos_hardcore_mode_enable'] = 'false'
    else:
        retroarchConfig['cheevos_enable'] = 'false'

    if enabled('integerscale', recalboxConfig):
        retroarchConfig['video_scale_integer'] = 'true'
    else:
        retroarchConfig['video_scale_integer'] = 'false'

    if system.config['core'] == 'bluemsx':
        coreSettings.save('bluemsx_msxtype', '"Auto"')

    if system.name == 'atari5200':
        coreSettings.save('atari800_system', '"5200"')
    else:
        coreSettings.save('atari800_system', '"800XL (64K)"')

    # Netplay management
    if 'netplaymode' in system.config and system.config['netplaymode'] in systemNetplayModes:
        retroarchConfig['netplay_use_mitm_server']   = "false"
        if system.config['netplaymode'] == 'client':
	    pass
        elif system.config['netplaymode'] == 'host':
            mitm = recalboxConfig.get('netplay.relay', "")
            if mitm:
                retroarchConfig['netplay_use_mitm_server'] = "true"
                retroarchConfig['netplay_mitm_server'] = mitm

    # Display FPS
    if enabled('showFPS', recalboxConfig):
        retroarchConfig['fps_show'] = 'true'
    else:
        retroarchConfig['fps_show'] = 'false'
        
    return retroarchConfig


def writeLibretroConfigToFile(config):
    for setting in config:
        libretroSettings.save(setting, config[setting])


def updateLibretroConfig(version):
    returnValue = True
    returnValue = removeLibretroConfigUnwantedOptions(version) and returnValue
    returnValue = updateLibretroConfigCustom(version) and returnValue
    return returnValue


def updateLibretroConfigCustom(version):
    # Version is unsued so far, but who knows, one day
    try: 
        # Read files
        sourceSettings = UnixSettings(recalboxFiles.retroarchInitCustomOrigin, separator=' ')
        sourceConf = sourceSettings.loadFile()
        destFiles = [recalboxFiles.retroarchCustomOrigin, recalboxFiles.retroarchCustom]
        
        for file in destFiles:
            if not os.path.isfile(file) : continue
            destSettings = UnixSettings(file, separator=' ')
            destConf = destSettings.loadFile()
            missingKeys = {}
            # Compare missing keysoprint "Compare keys ..."
            for key, value in sourceConf.iteritems():
                if key not in destConf:
                    missingKeys[key] = value
            # Save
            print "Save {}".format(file)
            for key, value in missingKeys.iteritems():
                destSettings.save(key, value)
            missingKeys.clear()
            
        print("LibretroConfig 's configuration successfully upgraded")
        return True
    except:
        print "Libretro update failed !"
        return False

def removeLibretroConfigUnwantedOptions(version):
    unwantedOptions = ['extraction_directory']
    returnValue = True

    for option in unwantedOptions:
        returnValue = (os.system("sed -i \"/^{} =.*/d\" {}".format(option,recalboxFiles.retroarchCustomOrigin)) ==0 ) and returnValue
        returnValue = (os.system("sed -i \"/^{} =.*/d\" {}".format(option,recalboxFiles.retroarchCustom)) == 0) and returnValue

    return returnValue 
