#!/usr/bin/env python
import os
import sys

import recalboxFiles
from settings.keyValueSettings import keyValueSettings
from libretroRetroarch import LibretroRetroarch
from libretroCores import LibretroCores
from libretroControllers import LibretroControllers

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))


# Libretro configuration
class LibretroConfiguration:

    # constructor
    def __init__(self, system, controllers, rom):
        # Rom folder
        romFolder = os.path.dirname(rom)
        # Default files
        self.retroarchCustomOriginFile = recalboxFiles.retroarchCustomOrigin
        self.retroarchCustomFile = recalboxFiles.retroarchCustom
        self.retroarchCustomOverrideFile = os.path.join(romFolder, ".retroarch.cfg")
        self.retroarchCoreCustomFile = recalboxFiles.retroarchCoreCustom
        self.retroarchCoreCustomOverrideFile = os.path.join(romFolder, ".core.cfg")
        # Default settings
        self.retroarchSettings = keyValueSettings(None, True)
        self.coreSettings = keyValueSettings(None, True)
        self.system = system
        self.controllers = controllers

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
        # Override with custom settings
        if self.retroarchCustomFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomFile)
            retroarchConfig.loadFile()
        # Override with folder settings
        if self.retroarchCustomOverrideFile is not None:
            retroarchConfig.changeSettingsFile(self.retroarchCustomOverrideFile)
            retroarchConfig.loadFile()

        return retroarchConfig

    def createRetroarchConfiguration(self):
        # Load settings
        retroarchConfig = self.loadRetroarchConfigurations()

        # Configure options
        retroarch = LibretroRetroarch(self.system, retroarchConfig, self.controllers)
        retroarch.fillRetroarchConfiguration()
        # Configure controllers
        controllers = LibretroControllers(self.system, retroarchConfig, self.controllers)
        controllers.fillControllersConfiguration()

        # Save settings
        retroarchConfig.changeSettingsFile(self.retroarchCustomFile)
        retroarchConfig.saveFile()

        # Return file for testing purpose only
        return retroarchConfig

    def createCoreConfiguration(self):
        coreConfig = self.coreSettings

        # Load existing core file
        if self.retroarchCoreCustomFile is not None:
            coreConfig.changeSettingsFile(self.retroarchCoreCustomFile)
            coreConfig.loadFile(True)
        # Override with folder settings
        if self.retroarchCoreCustomOverrideFile is not None:
            coreConfig.changeSettingsFile(self.retroarchCoreCustomOverrideFile)
            coreConfig.loadFile(True)

        # Configure
        cores = LibretroCores(self.system, coreConfig, self.controllers)
        cores.fillCoresConfiguration()

        # Save settings
        coreConfig.changeSettingsFile(self.retroarchCoreCustomFile)
        coreConfig.saveFile()

        # Return file for testing purpose only
        return coreConfig

    def getRetroarchConfigurationFileName(self):
        return self.retroarchSettings.getSettingsFile()

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
