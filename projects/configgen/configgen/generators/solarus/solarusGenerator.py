from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.generators.solarus.solarusControllers import SolarusControllers
from configgen.settings.keyValueSettings import keyValueSettings


class SolarusGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        # Default args
        arguments = ["-fullscreen=yes", "-cursor-visible=no"]

        # Filtering?
        if system.Smooth:
            arguments.append("-bilinear-filtering")

        # Pad configuration
        controllers = SolarusControllers(playersControllers)
        arguments.append("-quit-combo={}+{}".format(controllers.HotKey(), controllers.Start()))

        # Rom
        arguments.append(args.rom)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]
        commandArray.extend(arguments)

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
