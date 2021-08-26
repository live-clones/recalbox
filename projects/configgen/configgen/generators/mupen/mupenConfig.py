#!/usr/bin/env python

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.unixSettings import UnixSettings
from configgen.utils.videoMode import *
from configgen.utils.architecture import Architecture
from configgen.controllersConfig import Input
from configgen.controllersConfig import ControllerDictionary

mupenSettings = UnixSettings(recalboxFiles.mupenCustom, separator=' ')

GlideN64FBEmulation_whitelist = ["ocarina", "empire", "pokemon", "rayman", "donald", "diddy", "beetle", "tennis", "instinct", "gemini", "twins", "majora", "quake", "ridge"]
GLideN64LegacyBlending_blacklist = ["empire", "beetle", "donkey", "zelda", "bomberman", "party"]
GLideN64NativeResolution_blacklist = ["majora"]


def writeMupenConfig(system: Emulator, controllers: ControllerDictionary, rom: str):
    setPaths()
    writeHotKeyConfig(controllers)
    setIpl64DD(system)
    # ~ mupenSettings.save('Fullscreen', "True")
    # ~ mupenSettings.save('ScreenWidth', "")
    # ~ mupenSettings.save('ScreenHeight', "")

    #Draw or not FPS
    if system.ShowFPS:
        mupenSettings.save('ShowFPS', 'True')
        mupenSettings.save('show_fps', '4')
    else:
        mupenSettings.save('ShowFPS', 'False')
        mupenSettings.save('show_fps', '8')

    #Write GlideN64 config
    romName = os.path.basename(rom)

    #Crop resulted image.
    mupenSettings.save('CropMode', '1')
    #Bilinear filtering mode.
    mupenSettings.save('bilinearMode', '1')
    #Size of texture cache in megabytes.
    mupenSettings.save('CacheSize', '100')
    #Enable color buffer copy to RDRAM.
    mupenSettings.save('EnableCopyColorToRDRAM', '0')
    #Enable frame and|or depth buffer emulation.
    mupenSettings.save('EnableFBEmulation', 'False')
    #Do not use shaders to emulate N64 blending modes. Works faster on slow GPU. Can cause glitches.
    mupenSettings.save('EnableLegacyBlending', 'True')
    #Frame buffer size is the factor of N64 native resolution.
    mupenSettings.save('UseNativeResolutionFactor', '1')
    #Resolution
    from configgen.utils.resolutions import ResolutionParser
    resolution = ResolutionParser(system.VideoMode.strip())
    if resolution.isSet and resolution.selfProcess:
        mupenSettings.save('ScreenWidth', "{}".format(resolution.width))
        mupenSettings.save('ScreenHeight', "{}".format(resolution.height))

    # VerticalSync makes video smoother on odroidgo2 and odroidgo3
    # and force 16bpp color quality as H/W does not support 32bpp
    if Architecture.isGoa2 or Architecture.isGoa3:
        mupenSettings.save('VerticalSync', 'True')
    if Architecture.isGoa2 or Architecture.isGoa3 or Architecture.isPi4:
        mupenSettings.save('ColorQuality', '1')

    for n in GlideN64FBEmulation_whitelist:
        if n in romName.lower():
            mupenSettings.save('EnableFBEmulation', 'True')

    for n in GLideN64LegacyBlending_blacklist:
        if n in romName.lower():
            mupenSettings.save('EnableLegacyBlending', 'False')

    for n in GLideN64NativeResolution_blacklist:
        if n in romName.lower():
            mupenSettings.save('UseNativeResolutionFactor', '0')

def writeHotKeyConfig(controllers: ControllerDictionary):
    if '1' in controllers:
        if 'hotkey' in controllers['1'].inputs:
            if 'start' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Stop', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['start'])))
            if 'y' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Save State', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['y'])))
            if 'x' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Load State', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['x'])))
            if 'l1' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Screenshot', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['l1'])))
            if 'up' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Increment Slot', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['up'])))
            if 'right' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Fast Forward', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['right'])))
            if 'a' in controllers['1'].inputs:
                mupenSettings.save('Joy Mapping Reset', "\"J{}{}/{}\"".format(controllers['1'].index, createButtonCode(controllers['1'].inputs['hotkey']), createButtonCode(controllers['1'].inputs['a'])))


def createButtonCode(button: Input) -> str:
    if button.type == 'axis':
        if button.value == '-1':
            return 'A'+button.id+'-'
        else:
            return 'A'+button.id+'+'
    if button.type == 'button':
        return 'B'+button.id
    if button.type == 'hat':
        return 'H'+button.id+'V'+button.value


def setPaths():
    mupenSettings.save('ScreenshotPath', recalboxFiles.SCREENSHOTS)
    mupenSettings.save('SaveStatePath', recalboxFiles.mupenSaves)
    mupenSettings.save('SaveSRAMPath', recalboxFiles.mupenSaves)

def setIpl64DD(system: Emulator):
    #Set ipl only for n64
    if system.Name == "64dd":
        mupenSettings.save("IPL-ROM", "/recalbox/share/bios/64DD_IPL.bin")
    elif system.Name == "n64":
        mupenSettings.save("IPL-ROM", "")
