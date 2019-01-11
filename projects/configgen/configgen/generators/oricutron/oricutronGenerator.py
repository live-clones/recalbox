import Command
from generators.Generator import Generator
import recalboxFiles


class OricutronGenerator(Generator):

    def generate(self, system, rom, playersControllers):

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
                        """{}""".format(rom)]

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
