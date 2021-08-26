#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllersConfig import Controller
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.settings import keyValueSettings


class MoonlightGenerator(Generator):
    # Main entry of the module
    # Configure moonlight and return a command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        outputFile = recalboxFiles.moonlightCustom + '/gamecontrollerdb.txt'
        Controller.generateSDLGameDBAllControllers(playersControllers, outputFile)
        (gameName,confFile) = self.getRealGameNameAndConfigFile(args.rom)
        commandArray = [recalboxFiles.recalboxBins[system.Emulator], 'stream','-config',  confFile]

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append('-app')
        commandArray.append(gameName)
        return Command.Command(videomode='default', array=commandArray, env={"XDG_DATA_DIRS": recalboxFiles.CONF})

    @staticmethod
    def getRealGameNameAndConfigFile(rom: str) -> (str, str):
        # Rom's basename without extension
        import os
        romName = os.path.splitext(os.path.basename(rom))[0]
        # find the real game name
        f = open(recalboxFiles.moonlightGamelist, 'r')
        gfeGame = None
        for line in f:
            try:
                gfeRom, gfeGame, confFile = line.rstrip().split(';')
                #confFile = confFile.rstrip()
            except ValueError:
                gfeRom, gfeGame = line.split(';')
                confFile = recalboxFiles.moonlightConfig
            #If found
            if gfeRom == romName:
                # return it
                f.close()
                return [gfeGame, confFile]
        # If nothing is found (old gamelist file format ?)
        return gfeGame, recalboxFiles.moonlightConfig
