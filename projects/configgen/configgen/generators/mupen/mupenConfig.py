#!/usr/bin/env python

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.iniSettings import IniSettings
from configgen.utils.videoMode import *
from configgen.utils.architecture import Architecture
from configgen.controllersConfig import Input
from configgen.controllersConfig import ControllerDictionary

GlideN64FBEmulation_whitelist = ["ocarina", "empire", "pokemon", "rayman", "donald", "diddy", "beetle", "tennis", "instinct", "gemini", "twins", "majora", "quake", "ridge"]
GLideN64LegacyBlending_blacklist = ["empire", "beetle", "donkey", "zelda", "bomberman", "party"]
GLideN64NativeResolution_blacklist = ["majora"]

def writeMupenConfig(system: Emulator, controllers: ControllerDictionary, rom: str, mupenSettings: IniSettings):

    __setPaths(mupenSettings)
    __writeHotKeyConfig(controllers, mupenSettings)
    __setIpl64DD(system, mupenSettings)
    # ~ mupenSettings.save('Fullscreen', "True")
    # ~ mupenSettings.save('ScreenWidth', "")
    # ~ mupenSettings.save('ScreenHeight', "")

    # Draw or not FPS
    if system.ShowFPS:
        mupenSettings.setOption('Video-GLideN64', 'ShowFPS', 'True')
        mupenSettings.setOption('Video-Glide64mk2', 'show_fps', '4')
    else:
        mupenSettings.setOption('Video-GLideN64', 'ShowFPS', 'False')
        mupenSettings.setOption('Video-Glide64mk2', 'show_fps', '8')

    # Write GlideN64 config
    romName = os.path.basename(rom)

    # Crop resulted image.
    mupenSettings.setOption('Video-Rice', 'CropMode', '1') # This option no longer exist!
    # Size of texture cache in megabytes.
    mupenSettings.setOption('Video-Rice', 'CacheSize', '100') # This option no longer exist!
    # Bilinear filtering mode.
    mupenSettings.setOption('Video-GLideN64', 'bilinearMode', '1')
    # Enable color buffer copy to RDRAM.
    mupenSettings.setOption('Video-GLideN64', 'EnableCopyColorToRDRAM', '0')
    # Enable frame and|or depth buffer emulation.
    mupenSettings.setOption('Video-GLideN64', 'EnableFBEmulation', 'False')
    # Do not use shaders to emulate N64 blending modes. Works faster on slow GPU. Can cause glitches.
    mupenSettings.setOption('Video-GLideN64', 'EnableLegacyBlending', 'True')
    # Frame buffer size is the factor of N64 native resolution.
    mupenSettings.setOption('Video-GLideN64', 'UseNativeResolutionFactor', '1')
    # Resolution
    from configgen.utils.resolutions import ResolutionParser
    resolution = ResolutionParser(system.VideoMode.strip())
    if resolution.isSet and resolution.selfProcess:
        mupenSettings.setOption('Video-General', 'ScreenWidth', "{}".format(resolution.width))
        mupenSettings.setOption('Video-General', 'ScreenHeight', "{}".format(resolution.height))

    # VerticalSync makes video smoother on odroidgo2 and odroidgo3
    # and force 16bpp color quality as H/W does not support 32bpp
    if Architecture.isGoa2 or Architecture.isGoa3:
        mupenSettings.setOption('Video-General', 'VerticalSync', 'True')
    if Architecture.isGoa2 or Architecture.isGoa3 or Architecture.isPi4:
        mupenSettings.setOption('Video-Rice', 'ColorQuality', '1')

    for n in GlideN64FBEmulation_whitelist:
        if n in romName.lower():
            mupenSettings.setOption('Video-GLideN64', 'EnableFBEmulation', 'True')

    for n in GLideN64LegacyBlending_blacklist:
        if n in romName.lower():
            mupenSettings.setOption('Video-GLideN64', 'EnableLegacyBlending', 'False')

    for n in GLideN64NativeResolution_blacklist:
        if n in romName.lower():
            mupenSettings.setOption('Video-GLideN64', 'UseNativeResolutionFactor', '0')

def __writeHotKeyConfig(controllers: ControllerDictionary, mupenSettings: IniSettings):
    section = "CoreEvents"
    if '1' in controllers:
        if 'hotkey' in controllers['1'].inputs:
            if 'start' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Stop', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['start'])))
            if 'y' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Save State', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['y'])))
            if 'x' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Load State', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['x'])))
            if 'l1' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Screenshot', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['l1'])))
            if 'up' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Increment Slot', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['up'])))
            if 'right' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Fast Forward', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['right'])))
            if 'a' in controllers['1'].inputs:
                mupenSettings.setOption(section, 'Joy Mapping Reset', "\"J{}{}/{}\"".format(controllers['1'].index, __createButtonCode(controllers['1'].inputs['hotkey']), __createButtonCode(controllers['1'].inputs['a'])))


def __createButtonCode(button: Input) -> str:
    if button.type == 'axis':
        if button.value == '-1':
            return 'A'+button.id+'-'
        else:
            return 'A'+button.id+'+'
    if button.type == 'button':
        return 'B'+button.id
    if button.type == 'hat':
        return 'H'+button.id+'V'+button.value


def __setPaths(mupenSettings: IniSettings):
    section = 'Core'
    mupenSettings.setOption(section, 'ScreenshotPath', recalboxFiles.SCREENSHOTS)
    mupenSettings.setOption(section, 'SaveStatePath', recalboxFiles.mupenSaves)
    mupenSettings.setOption(section, 'SaveSRAMPath', recalboxFiles.mupenSaves)

def __setIpl64DD(system: Emulator, mupenSettings: IniSettings):
    #Set ipl only for n64
    section = '64DD'
    if system.Name == "64dd":
        mupenSettings.setOption(section, "IPL-ROM", "/recalbox/share/bios/64DD_IPL.bin")
    elif system.Name == "n64":
        mupenSettings.setOption(section, "IPL-ROM", "")
