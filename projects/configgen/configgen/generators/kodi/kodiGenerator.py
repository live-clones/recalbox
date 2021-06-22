#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.generators.Generator import Generator
import configgen.generators.kodi.kodiConfig as kodiConfig


class KodiGenerator(Generator):
    # Main entry of the module
    # Configure kodi inputs and return the command to run
    def generate(self, system, playersControllers, recalboxSettings, args):
        if not system.config['configfile']:
            kodiConfig.writeKodiControllersConfig(playersControllers)
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
