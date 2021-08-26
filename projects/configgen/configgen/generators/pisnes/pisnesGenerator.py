import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings


class PisnesGenerator(Generator):

    RECALBOX_TO_PBCS_BUTTON_MAPPING = \
    {
        "up": "UP",
        "down": "DOWN",
        "left": "LEFT",
        "right": "RIGHT",
        "a": "A",
        "b": "B",
        "x": "X",
        "y": "Y",
        "l1": "L",
        "r1": "R",
        "start": "START",
        "select": "SELECT",
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

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        config = PisnesGenerator.Loadconfiguration()

        # Path settings
        config.setOption(self.SECTION_PATH, "RomFolder", recalboxFiles.ROMS + "/snes")
        config.setOption(self.SECTION_PATH, "SaveStateFolder", recalboxFiles.SAVES + "/snes")

        # Full screen settings
        config.setOption(self.SECTION_GFX, "MaintainAspectRatio", '1' if system.IntegerScale else '0')

        # controller settings
        # Axis & Hats are hard-coded, so just set buttons
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player in ('1', '2'):
                player = controller.player
                for k,v in self.RECALBOX_TO_PBCS_BUTTON_MAPPING.items():
                    if k in controller.inputs:
                        inp = controller.inputs[k]
                        if inp.type == "button":
                            config.setOption(self.SECTION_PAD, "{}_{}".format(v, player), str(inp.id))

        PisnesGenerator.SaveConfiguration(config)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
