#!/usr/bin/env python
from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings import keyValueSettings
import os


class MoonlightGenerator(Generator):

    # Main entry of the module
    # Configure moonlight and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:
        outputFile = recalboxFiles.moonlightCustom + '/gamecontrollerdb.txt'
        from configgen.controllers.controller import Controller
        Controller.GenerateSDLGameDatabase(playersControllers, outputFile)
        (gameName, argument) = self.getRealGameNameAndArgument(args.rom)
        if self.isEmbedded():
            commandArray = [recalboxFiles.recalboxBins[system.Emulator], 'stream', '-config', argument]
        else:
            commandArray = [recalboxFiles.recalboxBins[system.Emulator], 'stream', argument]

        if system.HasArgs:
            commandArray.extend(system.Args)

        if self.isEmbedded():
            commandArray.append('-app')
        commandArray.append(gameName)
        return Command(videomode='default', array=commandArray, env={"XDG_DATA_DIRS": recalboxFiles.CONF})

    def getRealGameNameAndArgument(self, rom: str) -> (str, str):
        # Rom's basename without extension
        import os
        romName = os.path.splitext(os.path.basename(rom))[0]
        # find the real game name
        f = open(recalboxFiles.moonlightGamelist, 'r')
        gfeGame = None

        for line in f:
            if self.isEmbedded():
                try:
                    gfeRom, gfeGame, confFile = line.rstrip().split(';')
                except ValueError:
                    gfeRom, gfeGame = line.split(';')
                    confFile = recalboxFiles.moonlightConfig
                if gfeRom == romName:
                    f.close()
                    return [gfeGame, confFile]
            else:
                gfeRom, gfeGame, confFile = line.rstrip().split(';')
                streamHost = self.getStreamHostFromConfig(confFile)
                if gfeRom == romName:
                    # return it
                    f.close()
                    return [gfeGame, streamHost]
        # If nothing is found (old gamelist file format ?)
        return gfeGame, recalboxFiles.moonlightConfig

    @staticmethod
    def getStreamHostFromConfig(configFile: str) -> str:
        import re
        with open(configFile, 'r') as fp:
            for line in fp:
                z = re.match(r"^address\s*=\s*(.*)", line)
                if z:
                    return z.group(1)
        raise "IP not found"

    @staticmethod
    def isEmbedded() -> bool:
        return not os.path.exists(recalboxFiles.moonlightIsQT)
