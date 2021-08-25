#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
import configgen.generators.kodi.kodiConfig as kodiConfig
from settings.keyValueSettings import keyValueSettings


class KodiGenerator(Generator):
    # Main entry of the module
    # Configure kodi inputs and return the command to run
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        if not system.config['configfile']:
            kodiConfig.writeKodiControllersConfig(playersControllers)
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
