from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class FrotzGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept across emulator launches
        """

        from configgen.settings.iniSettings import IniSettings
        settings = IniSettings(recalboxFiles.frotzConfig, True)
        settings.loadFile(True)\
                .defineBool('on', 'no')

        # List of available colors:
        # 0: black, 1: red, 2: green, 3: yellow,4: blue,5: magenta, 6: cyan,
        # 7: white, 8: light grey, 9: medium grey, 10: dark grey

        # Set foreground color
        settings.setInt("Display", "Foreground", 7)

        # Set background color
        settings.setInt("Display", "Background", 10)

        # Set more prompt
        settings.setInt("Display", "Show More Prompts", 1)

        # Set gamma
        settings.setInt("Display", "Gamma", 2.2)

        # Set scaling X
        settings.setInt("Display", "Infocom V6 Scaling X", 2)

        # Set scaling Y
        settings.setInt("Display", "Infocom V6 Scaling Y", 2)

        # Set width
        settings.setInt("Window", "AcWidth", 640)

        # Set height
        settings.setInt("Window", "AcHeight", 480)

        # Set interpreter type
        # 0: default (Amiga), 1: DECSYSTEM-20, 2: Apple IIe, 3: Macintosh, 4: Amiga, 5: Atari ST,
        # 6: MSDOS, 7: Commodore 128, 8: Commodore 64, 9: Apple IIc, 10: Apple IIGS, 11: Tandy
        settings.setInt("Interpreter", "Number", 4)

        # Set error reporting
        # 0: never, 1: once, 2, always, 3: fatal
        settings.setInt("Interpreter", "Error Reporting", 1)

        # Set ignore errors
        settings.setInt("Interpreter", "Ignore Errors", 0)

        # Set expand abbreviations
        settings.setInt("Interpreter", "Expand Abbreviations", 0)

        # Set Tandy bit
        settings.setInt("Interpreter", "Tandy Bit", 0)

        # Set wrap script lines at 80 characters
        settings.setInt("Interpreter", "Wrap Script Lines", 1)

        # Set antialias for font
        settings.setInt("Fonts", "antialias", 0)

        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "-C", recalboxFiles.frotzConfig, "-F",
                        args.rom
                        ]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
