#!/usr/bin/env python
from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class Fba2xGenerator(Generator):
    # Main entry of the module
    # Configure fba and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Settings recalbox default config file if no user defined one
        configFile = system.ConfigFile
        if not system.HasConfigFile:
            # Using recalbox config file
            configFile = recalboxFiles.fbaCustom
            from configgen.settings.iniSettings import IniSettings
            fbaSettings = IniSettings(recalboxFiles.fbaCustomOrigin, False)
            fbaSettings.loadFile(True)
            fbaSettings.changeSettingsFile(configFile)

            #  Write controllers configuration files
            from configgen.generators.fba2x import fba2xControllers
            fba2xControllers.writeControllersConfig(system, args.rom, playersControllers, fbaSettings)
            # Write configuration to retroarchcustom.cfg
            from configgen.generators.fba2x import fba2xConfig
            fba2xConfig.writeFBAConfig(system, fbaSettings)

            # Save config
            fbaSettings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], "--configfile", configFile, '--logfile', recalboxFiles.logdir + "/fba2x.log"]
        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)

        return Command(videomode=system.VideoMode, array=commandArray)
