import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.generators.openbor.openborControllers import OpenborControllers
from configgen.settings.keyValueSettings import keyValueSettings


class OpenborGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.openborConfigOrigin)
        settings.loadFile(True)

        # Forced values
        settings.setInt("CompatibleVersion", 210760)
        settings.setInt("UseJoystick", 1)
        settings.setInt("SoftwareFilter", 1)
        settings.setInt("HardwareFilter", 1)
        settings.setInt("UseOpenGL", 1)
        settings.setInt("FullScreen", 1)

        # Configuration
        settings.setBool("PixelPerfect", system.IntegerScale)
        settings.setBool("DebugInfo", system.ShowFPS)

        # Pad configuration
        controllers = OpenborControllers(playersControllers)
        controllers.addControllers(settings)

        settings.changeSettingsFile(recalboxFiles.openborConfig)
        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
