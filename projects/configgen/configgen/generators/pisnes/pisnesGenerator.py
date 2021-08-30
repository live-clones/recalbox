from typing import Dict

from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.inputItem import InputItem
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings


class PisnesGenerator(Generator):

    RECALBOX_TO_PBCS_BUTTON_MAPPING: Dict[int, str] = \
    {
        InputItem.ItemUp    : "UP",
        InputItem.ItemDown  : "DOWN",
        InputItem.ItemLeft  : "LEFT",
        InputItem.ItemRight : "RIGHT",
        InputItem.ItemA     : "A",
        InputItem.ItemB     : "B",
        InputItem.ItemX     : "X",
        InputItem.ItemY     : "Y",
        InputItem.ItemL1    : "L",
        InputItem.ItemR1    : "R",
        InputItem.ItemStart : "START",
        InputItem.ItemSelect: "SELECT",
    }

    SECTION_PATH = "Path"
    SECTION_PAD  = "Joystick"
    SECTION_GFX  = "Graphics"

    @staticmethod
    def Loadconfiguration() -> IniSettings:
        # Load configuration
        config = IniSettings(recalboxFiles.pisnesConfigFile, False)
        config.loadFile(True)

        return config

    @staticmethod
    def SaveConfiguration(config):
        # Force path creation
        import os
        configPath = os.path.dirname(recalboxFiles.pisnesConfigFile)
        if not os.path.exists(configPath):
            os.makedirs(configPath)

        # Save configuration back
        config.saveFile()

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        config = PisnesGenerator.Loadconfiguration()

        # Path settings
        config.setString(self.SECTION_PATH, "RomFolder", recalboxFiles.ROMS + "/snes")
        config.setString(self.SECTION_PATH, "SaveStateFolder", recalboxFiles.SAVES + "/snes")

        # Full screen settings
        config.setInt(self.SECTION_GFX, "MaintainAspectRatio", 1 if system.IntegerScale else 0)

        # controller settings
        # Axis & Hats are hard-coded, so just set buttons
        for index in playersControllers:
            controller = playersControllers[index]
            player = controller.PlayerIndex
            if player in (1, 2):
                for item in controller.AvailableInput:
                    if item.Item in self.RECALBOX_TO_PBCS_BUTTON_MAPPING:
                        value: str = self.RECALBOX_TO_PBCS_BUTTON_MAPPING[item.Item]
                        if item.IsButton: # Axis/Hat won't work :(
                            config.setInt(self.SECTION_PAD, "{}_{}".format(value, player), item.Id)

        PisnesGenerator.SaveConfiguration(config)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]
        if system.HasArgs: commandArray.extend(system.Args)
        commandArray.append(args.rom)

        return Command(videomode=system.VideoMode, array=commandArray)
