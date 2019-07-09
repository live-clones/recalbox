import os
import Command
import recalboxFiles
from generators.Generator import Generator


class PcsxGenerator(Generator):

    RECALBOX_TO_PCSX_BUTTON_MAPPING = \
    {
        "a": "CROSS",
        "b": "CIRCLE",
        "x": "SQUARE",
        "y": "TRIANGLE",
        "pageup": "L1",
        "pagedown": "R1",
        "l2": "L2",
        "r2": "R2",
        "start": "START",
        "select": "SELECT",
    }


    @staticmethod
    def Loadconfiguration():
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
    def SaveConfiguration(config):
        # Force path creation
        configPath = os.path.dirname(recalboxFiles.pcsxConfigFile)
        if not os.path.exists(configPath):
            os.makedirs(configPath)

        # Save configuration back
        with open(recalboxFiles.pcsxConfigFile, "w+") as lines:
            for line in config:
                lines.write(line + '\n')


    @staticmethod
    def ButtonChar(button):
        return '\\' + hex(int(button) + 0xA0)[1:]


    def generate(self, system, rom, playersControllers, demo, recalboxSettings):

        config = PcsxGenerator.Loadconfiguration()

        # controller settings
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player in ('1', '2'):
                player = controller.player
                config.append("binddev = sdl:{}".format(controller.realName))
                config.append("bind up =  player{} UP".format(player))
                config.append("bind down =  player{} DOWN".format(player))
                config.append("bind left =  player{} LEFT".format(player))
                config.append("bind right =  player{} RIGHT".format(player))
                for k,v in self.RECALBOX_TO_PCSX_BUTTON_MAPPING.iteritems():
                    if k in controller.inputs:
                        config.append("bind {} =  player{} {}".format(PcsxGenerator.ButtonChar(controller.inputs[k].id), player, v))

        PcsxGenerator.SaveConfiguration(config)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-cdfile", rom]

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
