#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class KodiGenerator(Generator):
    # Main entry of the module
    # Configure kodi inputs and return the command to run
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            import configgen.generators.kodi.kodiConfig as kodiConfig
            kodiConfig.writeKodiControllersConfig(playersControllers)

        import configgen.recalboxFiles as recalboxFiles
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        return Command(videomode=system.VideoMode, array=commandArray)
