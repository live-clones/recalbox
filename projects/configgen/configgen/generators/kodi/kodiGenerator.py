#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.generators.kodi.kodiConfig as kodiConfig
from configgen.settings.keyValueSettings import keyValueSettings


class KodiGenerator(Generator):
    # Main entry of the module
    # Configure kodi inputs and return the command to run
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            kodiConfig.writeKodiControllersConfig(playersControllers)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        return Command.Command(videomode=system.VideoMode, array=commandArray)
