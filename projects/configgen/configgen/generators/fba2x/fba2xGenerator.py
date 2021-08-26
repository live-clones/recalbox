#!/usr/bin/env python
import shutil
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.generators.fba2x import fba2xControllers
from configgen.generators.fba2x import fba2xConfig
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class Fba2xGenerator(Generator):
    # Main entry of the module
    # Configure fba and return a command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        # Settings recalbox default config file if no user defined one
        configFile = system.ConfigFile
        if not system.HasConfigFile:
            # Using recalbox config file
            configFile = recalboxFiles.fbaCustom
            # Copy original fba2x.cfg
            shutil.copyfile(recalboxFiles.fbaCustomOrigin, recalboxFiles.fbaCustom)
            #  Write controllers configuration files
            fba2xControllers.writeControllersConfig(system, args.rom, playersControllers)
            # Write configuration to retroarchcustom.cfg
            fba2xConfig.writeFBAConfig(system)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], "--configfile", configFile, '--logfile', recalboxFiles.logdir + "/fba2x.log"]
        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
