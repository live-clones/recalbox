#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
import configgen.generators.advancemame.advMameControllers as advMameControllers

from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class AdvMameGenerator(Generator):

    # Main entry of the module
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:
        import os.path
        romName = os.path.basename(os.path.splitext(args.rom)[0])
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        configFile: str = system.ConfigFile if system.HasConfigFile else recalboxFiles.advancemameConfig
        if not system.HasConfigFile:
            advMameControllers.writeConfig(system, playersControllers, args)

        if system.HasArgs: commandArray.extend(system.Args)

        if args.verbose:
            commandArray.append("-log")
        commandArray.extend(['-cfg', configFile])
        commandArray.append(romName)

        return Command.Command(videomode='default', array=commandArray, env={"TERM": "linux"})
