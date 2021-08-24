#!/usr/bin/env python
import os
import sys

import configgen.recalboxFiles as recalboxFiles
from configgen.settings.configOverriding import buildOverrideChain
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.generators.libretro.libretroRetroarch import LibretroRetroarch
from configgen.generators.libretro.libretroCores import LibretroCores
from configgen.generators.libretro.libretroControllers import LibretroControllers
from configgen.utils.architecture import Architecture

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


# Libretro configuration
class LibretroConfiguration:

    # constructor
    def __init__(self, system, controllers, rom, demo, nodefaultkeymap, recalboxSettings):
        # Default files
        self.retroarchCustomOriginFile = recalboxFiles.retroarchCustomOrigin
        self.retroarchCustomArchFile = recalboxFiles.retroarchCustom + '.' + Architecture().Architecture
        self.retroarchCustomForCoreFile = recalboxFiles.retroarchCustom + '.' + system.config["core"]
        self.retroarchCustomArchForCoreFile = recalboxFiles.retroarchCustom + '.' + Architecture().Architecture + '.' + system.config["core"]
        self.retroarchCustomFile = recalboxFiles.retroarchCustom
        self.retroarchCustomOverrideChain = buildOverrideChain(rom, ".retroarch.cfg")
        self.retroarchCustomOverrideFile = recalboxFiles.retroarchCustom + ".overrides.cfg"
        self.retroarchCoreCustomFile = recalboxFiles.retroarchCoreCustom
        self.retroarchCoreCustomOverrideChain = buildOverrideChain(rom, ".core.cfg")
        # Default settings
        self.recalboxSettings = recalboxSettings
        self.retroarchSettings = keyValueSettings(None, True)
        self.retroarchOverrides = keyValueSettings(None, True)
        self.coreSettings = keyValueSettings(None, True)
        self.system = system
        self.controllers = controllers
        self.demo = demo
        self.nodefaultkeymap = nodefaultkeymap
        self.rom = rom

    # Config file overriding
    def overrideLibretroConfigurationFiles(self, customOrigin, custom):
        self.retroarchCustomOriginFile = customOrigin
        self.retroarchCustomFile = custom

    def overrideCoreConfigurationFiles(self, coreCustom):
        self.retroarchCoreCustomFile = coreCustom

    def loadRetroarchConfigurations(self):
        retroarchConfig = self.retroarchSettings
        retroarchConfig.clear()

        # Load template config file
        if self.retroarchCustomOriginFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomOriginFile)
            retroarchConfig.loadFile()
        # Load config file
        if self.retroarchCustomFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomFile)
            retroarchConfig.loadFile()

        return retroarchConfig

    def loadRetroarchOverrides(self):
        retroarchOverrides = self.retroarchOverrides
        retroarchOverrides.clear()

        # Load core-specific config file
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

    def getCommandLineArguments(self, retroarchConfig, _):
        # No result yet
        result = []

        # Starting from Retroarch 1.7.8, the shaders are now managed by a new automatic overall/core/game system
        # and the old video_shader key is no longer used.
        # The we have to convert the old key into a new overriding command line argument.
        # That way the users can use both system:
        # - Internal Retroarch configuration/override
        # - Recalbox configuration and overrides
        if retroarchConfig.hasOption("video_shader"):
            shader = retroarchConfig.getOption("video_shader", None)
            if shader is not None:
                result.append("--set-shader")
                result.append(shader)

        return result

    def createRetroarchConfiguration(self):
        # Load settings
        retroarchConfig = self.loadRetroarchConfigurations()

        # Configure options
        retroarch = LibretroRetroarch(self.system, retroarchConfig, self.controllers, self.demo, self.recalboxSettings)
        retroarch.fillRetroarchConfiguration()
        # Configure controllers
        controllers = LibretroControllers(self.system, retroarchConfig, self.controllers, self.nodefaultkeymap)
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

    def createCoreConfiguration(self):
        coreConfig = self.coreSettings

        # Load existing core file
        if self.retroarchCoreCustomFile is not None:
            coreConfig.changeSettingsFile(self.retroarchCoreCustomFile)
            coreConfig.loadFile(True)

        # Configure
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

    def getRetroarchConfigurationFileName(self):
        return self.retroarchSettings.getSettingsFile()

    def getRetroarchOverridesFileName(self):
        return self.retroarchOverrides.getSettingsFile()

    @staticmethod
    def updateLibretroConfig(version):
        returnValue = True
        returnValue = LibretroConfiguration.removeLibretroConfigUnwantedOptions(version) and returnValue
        returnValue = LibretroConfiguration.updateLibretroConfigCustom(version) and returnValue
        return returnValue

    @staticmethod
    def updateLibretroConfigCustom(version):
        del version # unused
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
    def removeLibretroConfigUnwantedOptions(version):
        del version # unused
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
