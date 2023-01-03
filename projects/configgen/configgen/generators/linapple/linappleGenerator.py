from typing import List

from configgen import recalboxFiles
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class LinappleGenerator(Generator):

    EMULATOR_BINARY = "/usr/bin/linapple"

    SETTINGS_TEMPLATE = "linapple.conf.origin"
    SETTINGS_FINAL = "linapple.conf"

    def __init__(self, pathInit, pathUser):
        self.pathInit = pathInit
        self.pathUser = pathUser

    @staticmethod
    def setGraphicConfig(system: Emulator, settings: keyValueSettings):
        settings.removeOption("Screen Width")
        settings.removeOption("Screen Height")
        if 'x' in system.VideoMode:
            width, height = system.VideoMode.split('x')
            settings.setInt("Screen Width", int(width))
            settings.setInt("Screen Height", int(height))
        settings.setInt("Fullscreen", 1)

    @staticmethod
    def setMediaConfig(settings: keyValueSettings, args):
        # Static folders
        import os
        romPath: str = os.path.join(recalboxFiles.ROMS, "apple2")
        settings.setString("FTP Local Dir", romPath)

        # Save states
        settings.setString("Save State Directory", os.path.join(recalboxFiles.SAVES, "apple2"))
        settings.setString("Save State Filename", os.path.join(recalboxFiles.SAVES, "apple2", os.path.basename(args.rom) + ".savestate"))

        # Disk/HDD images
        settings.setString("HDV Starting Directory", romPath)
        settings.setString("Slot 6 Directory", romPath)
        _, ext = os.path.splitext(args.rom)
        if ext.lower() == ".hdv":
            settings.setInt("Harddisk Enable", 1)
            from configgen.utils.diskCollector import DiskCollector
            collector = DiskCollector(args.rom, 2, True)
            settings.setString("Harddisk Image 1", collector.Disks[0])
            settings.setString("Harddisk Image 2", collector.Disks[1] if collector.Count > 1 else "")
            settings.setInt("Slot 6 Autoload", 0)
            settings.removeOption("Disk Image 1")
            settings.removeOption("Disk Image 2")
        else:
            settings.setInt("Slot 6 Autoload", 1)
            from configgen.utils.diskCollector import DiskCollector
            collector = DiskCollector(args.rom, 2, True)
            settings.setString("Disk Image 1", collector.Disks[0])
            settings.setString("Disk Image 2", collector.Disks[1] if collector.Count > 1 else "")
            settings.setInt("Harddisk Enable", 0)
            settings.removeOption("Harddisk Image 1")
            settings.removeOption("Harddisk Image 2")

    @staticmethod
    def setPrinterConfig(settings: keyValueSettings, args):
        import os
        settings.setString("Parallel Printer Filename", os.path.join(recalboxFiles.SAVES, "apple2", os.path.basename(args.rom) + ".printer.txt"))
        settings.setInt("Append to printer file", 1)

    @staticmethod
    def setGeneralConfig(system: Emulator, settings: keyValueSettings):
        settings.setInt("Boot at Startup", 1)
        settings.setInt("Show Leds", 1 if system.ShowFPS else 0)
        from configgen.utils.architecture import Architecture
        settings.setInt("Singlethreaded", 1 if Architecture().isPi0or1 else 0)

    @staticmethod
    def setJoystickConfig(playersControllers: ControllerPerPlayer, settings: keyValueSettings):
        # Enable both joystick
        settings.setInt("Joystick 0", 1)
        settings.setInt("Joystick 1", 1)

        for index in playersControllers:
            controller = playersControllers[index]
            if controller.PlayerIndex == 1:
                settings.setInt("Joy0Index", controller.SdlIndex)
                settings.setInt("Joy0Axis0", 0)
                settings.setInt("Joy0Axis1", 1)
                settings.setInt("Joy0up", controller.Up.Id if controller.HasUp and controller.Up.IsButton else -1)
                settings.setInt("Joy0down", controller.Down.Id if controller.HasDown and controller.Down.IsButton else -1)
                settings.setInt("Joy0left", controller.Left.Id if controller.HasLeft and controller.Left.IsButton else -1)
                settings.setInt("Joy0right", controller.Right.Id if controller.HasRight and controller.Right.IsButton else -1)
                settings.setInt("Joy0Button1", controller.B.Id if controller.HasB and controller.B.IsButton else 1)
                settings.setInt("Joy0Button2", controller.A.Id if controller.HasA and controller.A.IsButton else 0)
                settings.setInt("Joy0ButtonX", controller.X.Id if controller.HasX and controller.B.IsButton else -1)
                settings.setInt("Joy0ButtonY", controller.Y.Id if controller.HasY and controller.A.IsButton else -1)
                settings.setInt("Joy0ButtonL1", controller.L1.Id if controller.HasL1 and controller.L1.IsButton else -1)
                settings.setInt("Joy0ButtonR1", controller.R1.Id if controller.HasR1 and controller.R1.IsButton else -1)
                settings.setInt("JoyExitEnable", 0)
                if controller.HasStart and controller.HasHotkey and controller.Start.IsButton and controller.Hotkey.IsButton:
                    settings.setInt("JoyExitEnable", 1)
                    settings.setInt("JoyExitButtonHotKey", controller.Hotkey.Id)
                    settings.setInt("JoyExitButtonStart", controller.Start.Id)
            elif controller.PlayerIndex == 2:
                settings.setInt("Joy1Index", controller.SdlIndex)
                settings.setInt("Joy1Axis0", 0)
                settings.setInt("Joy1Axis1", 1)
                settings.setInt("Joy1up", controller.Up.Id if controller.HasUp and controller.Up.IsButton else -1)
                settings.setInt("Joy1down", controller.Down.Id if controller.HasDown and controller.Down.IsButton else -1)
                settings.setInt("Joy1left", controller.Left.Id if controller.HasLeft and controller.Left.IsButton else -1)
                settings.setInt("Joy1right", controller.Right.Id if controller.HasRight and controller.Right.IsButton else -1)
                settings.setInt("Joy1Button1", controller.A.Id if controller.HasA and controller.A.IsButton else 0)

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args):

        # Has user config?
        final = system.ConfigFile

        # Nope, create it
        if not system.HasConfigFile:
            # Load config file
            import os
            template = os.path.join(self.pathUser, LinappleGenerator.SETTINGS_TEMPLATE)
            if not os.path.exists(template):
                template = os.path.join(self.pathInit, LinappleGenerator.SETTINGS_TEMPLATE)
            settings: keyValueSettings = keyValueSettings(template, True)
            settings.loadFile(True)

            # Configure
            self.setGeneralConfig(system, settings)
            self.setPrinterConfig(settings, args)
            self.setGraphicConfig(system, settings)
            self.setJoystickConfig(playersControllers, settings)
            self.setMediaConfig(settings, args)

            # Load overrides
            from configgen.settings.configOverriding import buildOverrideChain
            overrides: List[str] = buildOverrideChain(args.rom, ".linapple.conf")
            for override in overrides:
                settings.changeSettingsFile(override)
                settings.loadFile()

            # Save configuration
            final: str = os.path.join(self.pathUser, LinappleGenerator.SETTINGS_FINAL)
            settings.changeSettingsFile(final)
            settings.saveFile()

        commandArray = [LinappleGenerator.EMULATOR_BINARY,
                        "-r", "128",  # Add paged memory
                        "--conf", final,
                        "-l" if args.verbose else ""]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
