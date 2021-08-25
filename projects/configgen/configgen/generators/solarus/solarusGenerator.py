import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.generators.solarus.solarusControllers import SolarusControllers
from settings.keyValueSettings import keyValueSettings


class SolarusGenerator(Generator):

    IS_TRUE = ("1", "true")

    def isEnabled(self, system, key):
        recalbox = system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        # Default args
        arguments = ["-fullscreen=yes", "-cursor-visible=no"]

        # Filtering?
        if self.isEnabled(system, "smooth"):
            arguments.append("-bilinear-filtering")

        # Pad configuration
        controllers = SolarusControllers(playersControllers)
        arguments.append("-quit-combo={}+{}".format(controllers.HotKey(), controllers.Start()))

        # Rom
        arguments.append(args.rom)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        commandArray.extend(arguments)

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
