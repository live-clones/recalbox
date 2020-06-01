import Command
import recalboxFiles
from generators.Generator import Generator
from generators.solarus.solarusControllers import SolarusControllers


class SolarusGenerator(Generator):

    IS_TRUE = ("1", "true")

    def isEnabled(self, system, key):
        recalbox = system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    def generate(self, system, rom, playersControllers, demo, nodefaultkeymap, recalboxSettings):

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        # Default args
        args = ["-fullscreen=yes", "-cursor-visible=no"]

        # Filtering?
        if self.isEnabled(system, "smooth"):
            args.append("-bilinear-filtering")

        # Pad configuration
        controllers = SolarusControllers(playersControllers)
        args.append("-quit-combo={}+{}".format(controllers.HotKey(), controllers.Start()))

        # Rom
        args.append(rom)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        commandArray.extend(args)

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
