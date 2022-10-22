#!/usr/bin/env python
from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings import keyValueSettings


class MoonlightGenerator(Generator):

    # Main entry of the module
    # Configure moonlight and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        outputFile = recalboxFiles.moonlightCustom + '/gamecontrollerdb.txt'
        from configgen.controllers.controller import Controller
        Controller.GenerateSDLGameDatabase(playersControllers, outputFile)
        (gameName, streamHost) = self.getRealGameNameAndConfigFile(args.rom)
        commandArray = [recalboxFiles.recalboxBins[system.Emulator], 'stream', streamHost]

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(gameName)
        return Command(videomode='default', array=commandArray, env={"XDG_DATA_DIRS": recalboxFiles.CONF})

    def getRealGameNameAndConfigFile(self, rom: str) -> (str, str, str):
        # Rom's basename without extension
        import os
        romName = os.path.splitext(os.path.basename(rom))[0]
        # find the real game name
        f = open(recalboxFiles.moonlightGamelist, 'r')
        gfeGame = None
        for line in f:
            gfeRom, gfeGame, confFile = line.rstrip().split(';')
            streamHost = self.getStreamHostFromConfig(confFile)
            if gfeRom == romName:
                # return it
                f.close()
                return [gfeGame, streamHost]

    @staticmethod
    def getStreamHostFromConfig(configFile: str) -> str:
        import re
        with open(configFile, 'r') as fp:
            for line in fp:
                z = re.match(r"^address\s*=\s*(.*)", line)
                if z:
                    return z.group(1)
        raise "IP not found"
