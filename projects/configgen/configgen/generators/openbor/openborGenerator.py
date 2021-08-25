import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.generators.openbor.openborControllers import OpenborControllers
from configgen.settings.keyValueSettings import keyValueSettings


class OpenborGenerator(Generator):

    IS_TRUE = ("1", "true")

    def isEnabled(self, system, key):
        recalbox = system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):

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
        settings.setOption("PixelPerfect", "1" if self.isEnabled(system, "integerscale") else "0")
        settings.setOption("DebugInfo", "1" if self.isEnabled(system, "showFPS") else "0")

        # Pad configuration
        controllers = OpenborControllers(playersControllers)
        controllers.addControllers(settings)

        settings.changeSettingsFile(recalboxFiles.openborConfig)
        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], args.rom]

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
