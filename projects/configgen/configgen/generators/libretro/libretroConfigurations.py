#!/usr/bin/env python

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllers.controller import ControllerPerPlayer


# Libretro configuration
class LibretroConfiguration:

    # constructor
    def __init__(self, system: Emulator, controllers: ControllerPerPlayer, rom: str, demo: bool, nodefaultkeymap: bool, recalboxOptions: keyValueSettings):
        # Default files
        self.retroarchCustomOriginFile = recalboxFiles.retroarchCustomOrigin
        from configgen.utils.architecture import Architecture
        self.retroarchCustomArchFile = recalboxFiles.retroarchCustom + '.' + Architecture().Architecture
        self.retroarchCustomForCoreFile = recalboxFiles.retroarchCustom + '.' + system.Core
        self.retroarchCustomArchForCoreFile = recalboxFiles.retroarchCustom + '.' + Architecture().Architecture + '.' + system.Core
        self.retroarchCustomFile = recalboxFiles.retroarchCustom
        from configgen.settings.configOverriding import buildOverrideChain
        self.retroarchCustomOverrideChain = buildOverrideChain(rom, ".retroarch.cfg")
        self.retroarchCustomOverrideFile = recalboxFiles.retroarchCustom + ".overrides.cfg"
        self.retroarchCoreCustomFile = recalboxFiles.retroarchCoreCustom
        self.retroarchCoreCustomOverrideChain = buildOverrideChain(rom, ".core.cfg")
        # Default settings
        self.recalboxOptions = recalboxOptions
        self.retroarchSettings = keyValueSettings("", True)
        self.retroarchOverrides = keyValueSettings("", True)
        self.coreSettings = keyValueSettings("", True)
        self.system: Emulator = system
        self.controllers: ControllerPerPlayer = controllers
        self.demo: bool = demo
        self.nodefaultkeymap: bool = nodefaultkeymap
        self.rom = rom

    # Config file overriding
    def overrideLibretroConfigurationFiles(self, customOrigin: str, custom: str):
        self.retroarchCustomOriginFile = customOrigin
        self.retroarchCustomFile = custom

    def overrideCoreConfigurationFiles(self, coreCustom: str):
        self.retroarchCoreCustomFile = coreCustom

    def loadRetroarchConfigurations(self) -> keyValueSettings:
        retroarchConfig = self.retroarchSettings
        retroarchConfig.clear() \
                       .defineBool('true', 'false')

        # Load template config file
        if self.retroarchCustomOriginFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomOriginFile)
            retroarchConfig.loadFile()
        # Load config file
        if self.retroarchCustomFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomFile)
            retroarchConfig.loadFile()

        return retroarchConfig

    def loadRetroarchOverrides(self) -> keyValueSettings:
        retroarchOverrides = self.retroarchOverrides
        retroarchOverrides.clear()

        # Load core-specific config file
        import os
        if os.path.exists(self.retroarchCustomForCoreFile) is not None:
            retroarchOverrides.changeSettingsFile(self.retroarchCustomForCoreFile)
            retroarchOverrides.loadFile()

        # Load arch-specific config file
        if os.path.exists(self.retroarchCustomArchFile) is not None:
            retroarchOverrides.changeSettingsFile(self.retroarchCustomArchFile)
            retroarchOverrides.loadFile()

        # Load arch-specific & core-specific config file
        if os.path.exists(self.retroarchCustomArchForCoreFile) is not None:
            retroarchOverrides.changeSettingsFile(self.retroarchCustomArchForCoreFile)
            retroarchOverrides.loadFile()

        # Override with folder/rom settings
        for customFile in self.retroarchCustomOverrideChain:
            retroarchOverrides.changeSettingsFile(customFile)
            retroarchOverrides.loadFile()

        return retroarchOverrides

    @staticmethod
    def getCommandLineArguments(retroarchConfig: keyValueSettings, _):
        # No result yet
        result = []

        # Starting from Retroarch 1.7.8, the shaders are now managed by a new automatic overall/core/game system
        # and the old video_shader key is no longer used.
        # The we have to convert the old key into a new overriding command line argument.
        # That way the users can use both system:
        # - Internal Retroarch configuration/override
        # - Recalbox configuration and overrides
        if retroarchConfig.hasOption("video_shader"):
            shader = retroarchConfig.getString("video_shader", "")
            if len(shader) != 0:
                result.append("--set-shader")
                result.append(shader)

        return result

    def createRetroarchConfiguration(self) -> (keyValueSettings, keyValueSettings):
        # Load settings
        retroarchConfig = self.loadRetroarchConfigurations()

        # Configure options
        from configgen.generators.libretro.libretroRetroarch import LibretroRetroarch
        retroarch = LibretroRetroarch(self.system, retroarchConfig, self.controllers, self.demo, self.recalboxOptions)
        retroarch.fillRetroarchConfiguration()
        # Configure controllers
        from configgen.generators.libretro.libretroControllers import LibretroControllers
        controllers = LibretroControllers(self.system, self.recalboxOptions, retroarchConfig, self.controllers, self.nodefaultkeymap)
        controllers.fillControllersConfiguration()

        # Save settings
        retroarchConfig.changeSettingsFile(self.retroarchCustomFile)
        retroarchConfig.saveFile()

        # load overrides
        retroarchOverrides = self.loadRetroarchOverrides()

        # Save overrides
        retroarchOverrides.changeSettingsFile(self.retroarchCustomOverrideFile)
        retroarchOverrides.saveFile()

        # Return file for testing purpose only
        return retroarchConfig, retroarchOverrides

    def createCoreConfiguration(self) -> keyValueSettings:
        coreConfig = self.coreSettings

        # Load existing core file
        if self.retroarchCoreCustomFile is not None:
            coreConfig.changeSettingsFile(self.retroarchCoreCustomFile)
            coreConfig.loadFile(True)

        # Configure
        from configgen.generators.libretro.libretroCores import LibretroCores
        cores = LibretroCores(self.system, coreConfig, self.controllers, self.rom)
        cores.fillCoresConfiguration()

        # Override with folder/rom settings
        for coreFile in self.retroarchCoreCustomOverrideChain:
            coreConfig.changeSettingsFile(coreFile)
            coreConfig.loadFile(False)

        # Save settings
        coreConfig.changeSettingsFile(self.retroarchCoreCustomFile)
        coreConfig.saveFile()

        # Return file for testing purpose only
        return coreConfig

    def getRetroarchConfigurationFileName(self) -> str:
        return self.retroarchSettings.SettingsFile

    def getRetroarchOverridesFileName(self) -> str:
        return self.retroarchOverrides.SettingsFile

    @staticmethod
    def updateLibretroConfig(version: str):
        returnValue = True
        returnValue = LibretroConfiguration.removeLibretroConfigUnwantedOptions(version) and returnValue
        returnValue = LibretroConfiguration.updateLibretroConfigCustom(version) and returnValue
        return returnValue

    @staticmethod
    def updateLibretroConfigCustom(_):
        # Version is unused so far, but who knows, one day
        try:
            # Load new initial settings
            initialSettings = keyValueSettings(recalboxFiles.retroarchInitCustomOrigin, True)
            initialSettings.loadFile(True)
            # Load template settings and override new initial settings
            initialSettings.changeSettingsFile(recalboxFiles.retroarchCustomOrigin)
            initialSettings.loadFile(False)
            initialSettings.saveFile()
            # Load custom settings and override previous settings
            initialSettings.changeSettingsFile(recalboxFiles.retroarchCustom)
            initialSettings.loadFile(False)
            initialSettings.saveFile()

            print("LibretroConfig 's configuration successfully upgraded")
            return True
        except IOError:
            print("Libretro update failed !")
            return False

    @staticmethod
    def removeLibretroConfigUnwantedOptions(_):
        unwantedOptions = ['extraction_directory']

        try:
            # Load template settings
            templateSettings = keyValueSettings(recalboxFiles.retroarchCustomOrigin, True)
            templateSettings.loadFile(False)
            # Load custom settings
            customSettings = keyValueSettings(recalboxFiles.retroarchCustom, True)
            customSettings.loadFile(False)

            for option in unwantedOptions:
                templateSettings.removeOption(option)
                customSettings.removeOption(option)

            templateSettings.saveFile()
            customSettings.saveFile()

            print("LibretroConfig's configuration successfully cleaned up")
            return True
        except IOError:
            print("Libretro's clean up failed!")
            return False
