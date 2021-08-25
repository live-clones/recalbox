#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
import configgen.generators.advancemame.advMameControllers as advMameControllers
import os.path

from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from settings.keyValueSettings import keyValueSettings


class AdvMameGenerator(Generator):
    # Main entry of the module
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        romName = os.path.basename(os.path.splitext(args.rom)[0])
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]

        if not system.config['configfile']:
            # Using recalbox config file
            system.config['configfile'] = recalboxFiles.advancemameConfig
            advMameControllers.writeConfig(system, playersControllers, args)

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        if args.verbose:
            commandArray.append("-log")
        commandArray.extend(['-cfg', system.config['configfile']])
        commandArray.append(romName)

        return Command.Command(videomode='default', array=commandArray, env={"TERM": "linux"})
