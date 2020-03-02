import os
import Command
import recalboxFiles
from generators.Generator import Generator
from settings.iniSettings import IniSettings


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
    def Loadconfiguration():
        # Load configuration
        config = IniSettings(recalboxFiles.pisnesConfigFile, False)
        config.loadFile(True)

        return config

    @staticmethod
    def SaveConfiguration(config):
        # Force path creation
        configPath = os.path.dirname(recalboxFiles.pisnesConfigFile)
        if not os.path.exists(configPath):
            os.makedirs(configPath)

        # Save configuration back
        config.saveFile()

    def generate(self, system, rom, playersControllers, demo, recalboxSettings):

        config = PisnesGenerator.Loadconfiguration()

        # Path settings
        config.setOption(self.SECTION_PATH, "RomFolder", recalboxFiles.ROMS + "/snes")
        config.setOption(self.SECTION_PATH, "SaveStateFolder", recalboxFiles.SAVES + "/snes")

        # Full screen settings
        key = "integerscale"
        intScale = key in system.config and system.config[key] in ('1', 'true')
        config.setOption(self.SECTION_GFX, "MaintainAspectRatio", '1' if intScale else '0')

        # controller settings
        # Axis & Hats are hard-coded, so just set buttons
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player in ('1', '2'):
                player = controller.player
                for k,v in self.RECALBOX_TO_PBCS_BUTTON_MAPPING.iteritems():
                    if k in controller.inputs:
                        inp = controller.inputs[k]
                        if inp.type == "button":
                            config.setOption(self.SECTION_PAD, "{}_{}".format(v, player), str(inp.id))

        PisnesGenerator.SaveConfiguration(config)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append(rom)

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
