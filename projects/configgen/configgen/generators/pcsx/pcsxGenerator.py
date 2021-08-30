from typing import List, Dict

from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.inputItem import InputItem
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings import keyValueSettings


class PcsxGenerator(Generator):

    RECALBOX_TO_PCSX_BUTTON_MAPPING: Dict[int, str] = \
    {
        InputItem.ItemA: "CROSS",
        InputItem.ItemB: "CIRCLE",
        InputItem.ItemX: "SQUARE",
        InputItem.ItemY: "TRIANGLE",
        InputItem.ItemL1: "L1",
        InputItem.ItemR1: "R1",
        InputItem.ItemL2: "L2",
        InputItem.ItemR2: "R2",
        InputItem.ItemStart: "START",
        InputItem.ItemSelect: "SELECT",
    }

    @staticmethod
    def Loadconfiguration() -> List[str]:
        # Load configuration without 'bind...' lines
        config = []
        try:
            with open(recalboxFiles.pcsxConfigFile) as lines:
                for line in lines:
                    if not line.startswith("bind"):
                        config.append(line)
        except IOError:
            pass

        return config

    @staticmethod
    def SaveConfiguration(config: List[str]):
        # Force path creation
        import os
        configPath = os.path.dirname(recalboxFiles.pcsxConfigFile)
        if not os.path.exists(configPath):
            os.makedirs(configPath)

        # Save configuration back
        with open(recalboxFiles.pcsxConfigFile, "w+") as lines:
            for line in config:
                lines.write(line + '\n')

    @staticmethod
    def ButtonChar(button: int) -> str:
        return '\\' + hex(button + 0xA0)[1:]

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        config = PcsxGenerator.Loadconfiguration()

        # controller settings
        for index in playersControllers:
            controller = playersControllers[index]
            player = controller.PlayerIndex
            if player in (1, 2):
                config.append("binddev = sdl:{}".format(controller.DeviceName))
                config.append("bind up =  player{} UP".format(player))
                config.append("bind down =  player{} DOWN".format(player))
                config.append("bind left =  player{} LEFT".format(player))
                config.append("bind right =  player{} RIGHT".format(player))
                for item in controller.AvailableInput:
                    if item.Item in self.RECALBOX_TO_PCSX_BUTTON_MAPPING:
                        value: str = self.RECALBOX_TO_PCSX_BUTTON_MAPPING[item.Item]
                        config.append("bind {} =  player{} {}".format(PcsxGenerator.ButtonChar(item.Id), player, value))

        PcsxGenerator.SaveConfiguration(config)
        commandArray = [recalboxFiles.recalboxBins[system.Emulator], "-cdfile", args.rom]
        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
