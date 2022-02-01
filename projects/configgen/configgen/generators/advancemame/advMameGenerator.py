#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings
import configgen.recalboxFiles as recalboxFiles


class AdvMameGenerator(Generator):

    # Main entry of the module
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        import os.path
        romName = os.path.basename(os.path.splitext(args.rom)[0])
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        configFile: str = system.ConfigFile if system.HasConfigFile else recalboxFiles.advancemameConfig
        if not system.HasConfigFile:
            import configgen.generators.advancemame.advMameControllers as advMameControllers
            advMameControllers.writeConfig(system, playersControllers, args)

        if system.HasArgs: commandArray.extend(system.Args)

        if args.verbose:
            commandArray.append("-log")
        commandArray.extend(['-cfg', configFile])
        commandArray.append(romName)

        return Command(videomode='default', array=commandArray, env={"TERM": "linux"})
