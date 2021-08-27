import os
from typing import List

import configgen.Command as Command
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

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args):
        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launches
        """

        # Load config file
        settings = keyValueSettings(recalboxFiles.gsplusConfig, True)
        settings.loadFile(True)

        # Try to guess the best drive
        size = os.path.getsize(args.rom) >> 10
        if size == 140: # Probably Apple II software on 5.25" disk
            slot = 6
            disks = 2
        elif size == 800: # Most likely an Apple IIGS 3.5" disk
            slot = 5
            disks = 2
        else: # Apple IIGS hard disk image or unknown size, let the emulator decide!
            slot = 7
            disks = 32

        # Seek multidisk games
        disks = GSplusGenerator.SeekMultiDisks(args.rom, disks)
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
                   "-mem", "14680064",
                   "-slot", str(slot)]

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

        return Command.Command(videomode=system.VideoMode, array=commandArray)
