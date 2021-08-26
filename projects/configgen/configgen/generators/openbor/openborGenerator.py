import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.generators.openbor.openborControllers import OpenborControllers
from configgen.settings.keyValueSettings import keyValueSettings


class OpenborGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.openborConfigOrigin)
        settings.loadFile(True)

        # Forced values
        settings.setOption("CompatibleVersion", "210760")
        settings.setOption("UseJoystick", "1")
        settings.setOption("SoftwareFilter", "1")
        settings.setOption("HardwareFilter", "1")
        settings.setOption("UseOpenGL", "1")
        settings.setOption("FullScreen", "1")

        # Configuration
        settings.setOption("PixelPerfect", "1" if system.IntegerScale else "0")
        settings.setOption("DebugInfo", "1" if system.ShowFPS else "0")

        # Pad configuration
        controllers = OpenborControllers(playersControllers)
        controllers.addControllers(settings)

        settings.changeSettingsFile(recalboxFiles.openborConfig)
        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
