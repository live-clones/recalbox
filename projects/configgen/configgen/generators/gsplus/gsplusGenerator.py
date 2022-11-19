from typing import List

from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllers.controller import Controller, InputItem, ControllerPerPlayer


class GSplusGenerator(Generator):

    # Collect disks
    @staticmethod
    def SeekMultiDisks(rom: str, disks: int) -> List[str]:
        from configgen.utils.diskCollector import DiskCollector
        collector = DiskCollector(rom, disks, True)
        return collector.disks

    @staticmethod
    def addSpecialButtons(controller: Controller, to: List[str]):
        if controller.HasHotkey and controller.Hotkey.IsButton:
            to.append("-joy-bhk")
            to.append(str(controller.Hotkey.Id))
        if controller.HasStart and controller.Start.IsButton:
            to.append("-joy-bstart")
            to.append(str(controller.Start.Id))

    @staticmethod
    def addGeneralButtons(controller: Controller, to: List[str]):
        if controller.HasA and controller.A.IsButton:
            to.append("-joy-b0")
            to.append(str(controller.A.Id))
        if controller.HasB and controller.B.IsButton:
            to.append("-joy-b1")
            to.append(str(controller.B.Id))
        if controller.HasX and controller.X.IsButton:
            to.append("-joy-b2")
            to.append(str(controller.X.Id))
        if controller.HasY and controller.Y.IsButton:
            to.append("-joy-b3")
            to.append(str(controller.Y.Id))

    @staticmethod
    def addJoystickLeft(controller: Controller, to: List[str]):
        if controller.HasJoy1Left and controller.Joy1Left.IsAxis:
            to.append("-joy-x")
            to.append(str(controller.Joy1Left.Id))
        if controller.HasJoy1Up and controller.Joy1Up.IsAxis:
            to.append("-joy-y")
            to.append(str(controller.Joy1Up.Id))

    @staticmethod
    def addJoystickRight(controller: Controller, to: List[str]):
        if controller.HasJoy2Left and controller.Joy2Left.IsAxis:
            to.append("-joy-x2")
            to.append(str(controller.Joy2Left.Id))
        if controller.HasJoy2Up and controller.Joy2Up.IsAxis:
            to.append("-joy-y2")
            to.append(str(controller.Joy2Up.Id))

    @staticmethod
    def addDpadItem(option: str, item: InputItem, to: List[str]):
        if item.IsButton:
            to.append('-joy-' + option + '-button')
            to.append(str(item.Id))
        if item.IsHat:
            to.append('-joy-' + option + '-hat')
            to.append(str((item.Id << 4) + item.Value))
        if item.IsAxis:
            if option == 'up':
                to.append('-joy-y')
                to.append(str(item.Id))
            if option == 'left':
                to.append('-joy-x')
                to.append(str(item.Id))

    def addDpad(self, controller: Controller, to):
        self.addDpadItem('up', controller.Up, to)
        self.addDpadItem('down', controller.Down, to)
        self.addDpadItem('left', controller.Left, to)
        self.addDpadItem('right', controller.Right, to)
        pass

    @staticmethod
    def clearDisks(settings: keyValueSettings):
        for i in range(1, 3):
            settings.removeOption("s5d{}".format(i))
            settings.removeOption("s6d{}".format(i))
        for i in range(1, 33):
            settings.removeOption("s7d{}".format(i))

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args):
        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launches
        """

        slot: int = -1
        if ".gsplus" in args.rom:
            settings = keyValueSettings(args.rom, True)
            settings.loadFile(True)
            settings.changeSettingsFile(recalboxFiles.gsplusConfig)

            # Adjust BIOS path
            import os
            if settings.getString("g_cfg_rom_path", "") == "%1":
                settings.setString("g_cfg_rom_path", os.path.join(recalboxFiles.BIOS, "apple2gs1.rom"))
            elif settings.getString("g_cfg_rom_path", "") == "%3":
                settings.setString("g_cfg_rom_path", os.path.join(recalboxFiles.BIOS, "apple2gs3.rom"))

            # Adjust rom path
            folder = os.path.dirname(args.rom)
            for i in range(1, 33):
                if i < 3:
                    if settings.hasOption("s5d{}".format(i)) and settings.getString("s5d{}".format(i), "") != "":
                        settings.setString("s5d{}".format(i), os.path.join(folder, settings.getString("s5d{}".format(i), "")))
                    if settings.hasOption("s6d{}".format(i)) and settings.getString("s6d{}".format(i), "") != "":
                        settings.setString("s6d{}".format(i), os.path.join(folder, settings.getString("s6d{}".format(i), "")))
                if settings.hasOption("s7d{}".format(i)) and settings.getString("s7d{}".format(i), "") != "":
                    settings.setString("s7d{}".format(i), os.path.join(folder, settings.getString("s7d{}".format(i), "")))

        else:
            # Load config file
            settings = keyValueSettings(recalboxFiles.gsplusConfig, True)
            settings.loadFile(True)

            # Reset drive slots
            for i in range(1, 33):
                if i < 3:
                    if settings.hasOption("s5d{}".format(i)):
                        settings.setString("s5d{}".format(i), "")
                    if settings.hasOption("s6d{}".format(i)):
                        settings.setString("s6d{}".format(i), "")
                if settings.hasOption("s7d{}".format(i)):
                    settings.setString("s7d{}".format(i), "")

            # Reset bios
            import os
            bios = os.path.join(recalboxFiles.BIOS, "apple2gs1.rom")
            if os.path.exists(bios):
                settings.setString("g_cfg_rom_path", bios)
            else:
                bios = os.path.join(recalboxFiles.BIOS, "apple2gs3.rom")
                if os.path.exists(bios):
                    settings.setString("g_cfg_rom_path", bios)
                else:
                    settings.removeOption("g_cfg_rom_path")

            # Reset speed limit
            settings.removeOption("g_limit_speed")

            # Try to guess the best drive
            import os
            size = os.path.getsize(args.rom) >> 10
            if size == 140:     # Probably Apple II software on 5.25" disk
                slot: int = 6
                maxDisks: int = 2
            elif size == 800:   # Most likely an Apple IIGS 3.5" disk
                slot: int = 5
                maxDisks: int = 2
            else:               # Apple IIGS hard disk image or unknown size, let the emulator decide!
                slot: int = 7
                maxDisks: int = 32

            # Seek multidisk games
            disks = GSplusGenerator.SeekMultiDisks(args.rom, maxDisks)
            diskDrive = 1
            self.clearDisks(settings)
            for disk in disks:
                settings.setString("s{}d{}".format(slot, diskDrive), disk)
                diskDrive += 1

        # Save config file
        settings.saveFile()

        # Default options
        options = ["-fullscreen",
                   "-ssdir", recalboxFiles.SCREENSHOTS,
                   "-config", recalboxFiles.gsplusConfig,
                   "-mem", "14680064"]
        if slot >= 0:
            options.extend(["-slot", str(slot)])

        # Screen resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            options.append("-x")
            options.append(str(resolution.width))
            options.append("-y")
            options.append(str(resolution.height))

        if system.ShaderSet == 'scanlines':
            options.append("-scanline")
            options.append("40")

        # controller settings
        joystickOptions = []
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.PlayerIndex == 1:
                joystickOptions = ["-joy", str(controller.SdlIndex)]
                self.addGeneralButtons(controller, joystickOptions)
                self.addSpecialButtons(controller, joystickOptions)
                self.addJoystickLeft(controller, joystickOptions)
                self.addJoystickRight(controller, joystickOptions)
                self.addDpad(controller, joystickOptions)

        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]
        commandArray.extend(options)
        commandArray.extend(joystickOptions)

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
