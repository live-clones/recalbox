#!/usr/bin/env python
import shutil
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.fba2x import fba2xControllers
from configgen.generators.fba2x import fba2xConfig
from configgen.generators.Generator import Generator, ControllerDictionary
from settings.keyValueSettings import keyValueSettings


class Fba2xGenerator(Generator):
    # Main entry of the module
    # Configure fba and return a command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            system.config['configfile'] = recalboxFiles.fbaCustom
            # Copy original fba2x.cfg
            shutil.copyfile(recalboxFiles.fbaCustomOrigin, recalboxFiles.fbaCustom)
            #  Write controllers configuration files
            fba2xControllers.writeControllersConfig(system, args.rom, playersControllers)
            # Write configuration to retroarchcustom.cfg
            fba2xConfig.writeFBAConfig(system)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "--configfile", system.config['configfile'], '--logfile', recalboxFiles.logdir + "/fba2x.log"]
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append(args.rom)
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
