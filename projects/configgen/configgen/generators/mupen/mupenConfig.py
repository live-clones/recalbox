#!/usr/bin/env python

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.iniSettings import IniSettings
from configgen.utils.videoMode import *
from configgen.utils.architecture import Architecture
from configgen.controllers.controller import InputItem, ControllerPerPlayer, Controller

GlideN64FBEmulation_whitelist = ["ocarina", "empire", "pokemon", "rayman", "donald", "diddy", "beetle", "tennis", "instinct", "gemini", "twins", "majora", "quake", "ridge"]
GLideN64LegacyBlending_blacklist = ["empire", "beetle", "donkey", "zelda", "bomberman", "party"]
GLideN64NativeResolution_blacklist = ["majora"]

def writeMupenConfig(system: Emulator, controllers: ControllerPerPlayer, rom: str, mupenSettings: IniSettings):

    __setPaths(mupenSettings)
    __writeHotKeyConfig(controllers, mupenSettings)
    __setIpl64DD(system, mupenSettings)
    # ~ mupenSettings.save('Fullscreen', "True")
    # ~ mupenSettings.save('ScreenWidth', "")
    # ~ mupenSettings.save('ScreenHeight', "")

    # Draw or not FPS
    mupenSettings.setBool('Video-GLideN64', 'ShowFPS', system.ShowFPS) \
                 .setInt('Video-Glide64mk2', 'show_fps', 4 if system.ShowFPS else 8)

    # Write GlideN64 config
    romName = os.path.basename(rom)

    # Crop resulted image.
    mupenSettings.setInt('Video-Rice', 'CropMode', 1) # This option no longer exist!
    # Size of texture cache in megabytes.
    mupenSettings.setInt('Video-Rice', 'CacheSize', 100) # This option no longer exist!
    # Bilinear filtering mode.
    mupenSettings.setInt('Video-GLideN64', 'bilinearMode', 1)
    # Enable color buffer copy to RDRAM.
    mupenSettings.setInt('Video-GLideN64', 'EnableCopyColorToRDRAM', 0)
    # Enable frame and|or depth buffer emulation.
    mupenSettings.setBool('Video-GLideN64', 'EnableFBEmulation', False)
    # Do not use shaders to emulate N64 blending modes. Works faster on slow GPU. Can cause glitches.
    mupenSettings.setBool('Video-GLideN64', 'EnableLegacyBlending', True)
    # Frame buffer size is the factor of N64 native resolution.
    mupenSettings.setInt('Video-GLideN64', 'UseNativeResolutionFactor', 1)
    # Resolution
    from configgen.utils.resolutions import ResolutionParser
    resolution = ResolutionParser(system.VideoMode.strip())
    if resolution.isSet and resolution.selfProcess:
        mupenSettings.setString('Video-General', 'ScreenWidth', "{}".format(resolution.width))
        mupenSettings.setString('Video-General', 'ScreenHeight', "{}".format(resolution.height))

    # VerticalSync makes video smoother on odroidgo2 and odroidgo3
    # and force 16bpp color quality as H/W does not support 32bpp
    if Architecture.isGoa2 or Architecture.isGoa3:
        mupenSettings.setBool('Video-General', 'VerticalSync', True)
    if Architecture.isGoa2 or Architecture.isGoa3 or Architecture.isPi4:
        mupenSettings.setInt('Video-Rice', 'ColorQuality', 1)

    for n in GlideN64FBEmulation_whitelist:
        if n in romName.lower():
            mupenSettings.setBool('Video-GLideN64', 'EnableFBEmulation', True)

    for n in GLideN64LegacyBlending_blacklist:
        if n in romName.lower():
            mupenSettings.setBool('Video-GLideN64', 'EnableLegacyBlending', False)

    for n in GLideN64NativeResolution_blacklist:
        if n in romName.lower():
            mupenSettings.setInt('Video-GLideN64', 'UseNativeResolutionFactor', 0)

def __writeHotKeyConfig(controllers: ControllerPerPlayer, mupenSettings: IniSettings):
    section = "CoreEvents"
    if 1 in controllers:
        controller: Controller = controllers[1]
        if controller.HasHotkey:
            if controller.HasStart:
                mupenSettings.setString(section, 'Joy Mapping Stop', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.Start)))
            if controller.HasY:
                mupenSettings.setString(section, 'Joy Mapping Save State', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.Y)))
            if controller.HasX:
                mupenSettings.setString(section, 'Joy Mapping Load State', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.X)))
            if controller.HasL1:
                mupenSettings.setString(section, 'Joy Mapping Screenshot', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.L1)))
            if controller.HasUp:
                mupenSettings.setString(section, 'Joy Mapping Increment Slot', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.Up)))
            if controller.HasRight:
                mupenSettings.setString(section, 'Joy Mapping Fast Forward', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.Right)))
            if controller.HasA:
                mupenSettings.setString(section, 'Joy Mapping Reset', "\"J{}{}/{}\"".format(controller.SdlIndex, __createButtonCode(controller.Hotkey), __createButtonCode(controller.A)))


def __createButtonCode(button: InputItem) -> str:
    if button.IsAxis:   return 'A' + str(button.Id) + ('-' if button.Value < 0 else '+')
    if button.IsButton: return 'B' + str(button.Id)
    if button.IsHat:    return 'H' + str(button.Id) + 'V' + str(button.Value)


def __setPaths(mupenSettings: IniSettings):
    section = 'Core'
    mupenSettings.setString(section, 'ScreenshotPath', recalboxFiles.SCREENSHOTS)
    mupenSettings.setString(section, 'SaveStatePath', recalboxFiles.mupenSaves)
    mupenSettings.setString(section, 'SaveSRAMPath', recalboxFiles.mupenSaves)

def __setIpl64DD(system: Emulator, mupenSettings: IniSettings):
    #Set ipl only for n64
    section = '64DD'
    if system.Name == "64dd":
        mupenSettings.setString(section, "IPL-ROM", "/recalbox/share/bios/64DD_IPL.bin")
    elif system.Name == "n64":
        mupenSettings.setString(section, "IPL-ROM", "")
