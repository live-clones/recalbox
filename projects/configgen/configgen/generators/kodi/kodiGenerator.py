#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.generators.kodi import kodiResolution
from configgen.generators.kodi.kodiResolution import KodiResolution
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.utils.architecture import Architecture


class KodiGenerator(Generator):
    # Main entry of the module
    # Configure kodi inputs and return the command to run
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            import configgen.generators.kodi.kodiConfig as kodiConfig
            kodiConfig.writeKodiControllersConfig(playersControllers)
        print(f"Generate kodi configuration with resolution {system.VideoMode}")

        rez: str = KodiResolution().transform(system)
        kodiConfig.writeKodiAdvancedSettingsConfig(Architecture().Architecture, rez)

        import configgen.recalboxFiles as recalboxFiles
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        return Command(videomode=system.VideoMode, array=commandArray)
