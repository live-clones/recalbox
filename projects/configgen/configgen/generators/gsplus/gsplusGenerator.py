import os

import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings


class GSplusGenerator(Generator):

    # Generate ADF Arguments
    @staticmethod
    def SeekMultiDisks(rom, disks): # type: (str, int) -> list
        from configgen.utils.diskCollector import DiskCollector
        collector = DiskCollector(rom, disks, True)
        return collector.disks

    @staticmethod
    def addJoyItem(option, item, _type, inputs, to): # type: (str, str, str, input, list) -> None
        if item in inputs:
            if inputs[item].type == _type:
                to.append(option)
                to.append(str(inputs[item].id))

    def addSpecialButtons(self, controller, to):
        self.addJoyItem("-joy-bhk", "hotkey", 'button', controller.inputs, to)
        self.addJoyItem("-joy-bstart", "start", 'button', controller.inputs, to)

    def addGeneralButtons(self, controller, to):
        self.addJoyItem("-joy-b0", "a", 'button', controller.inputs, to)
        self.addJoyItem("-joy-b1", "b", 'button', controller.inputs, to)
        self.addJoyItem("-joy-b2", "x", 'button', controller.inputs, to)
        self.addJoyItem("-joy-b3", "y", 'button', controller.inputs, to)

    def addJoystickLeft(self, controller, to):
        self.addJoyItem("-joy-x", "joystick1left", 'axis', controller.inputs, to)
        self.addJoyItem("-joy-y", "joystick1up", 'axis', controller.inputs, to)

    def addJoystickRight(self, controller, to):
        self.addJoyItem("-joy-x2", "joystick2left", 'axis', controller.inputs, to)
        self.addJoyItem("-joy-y2", "joystick2up", 'axis', controller.inputs, to)

    @staticmethod
    def addDpadItem(option, item, inputs, to):
        if item in inputs:
            _input = inputs[item]
            if _input.type == 'button':
                to.append('-joy-' + option + '-button')
                to.append(str(_input.id))
            if _input.type == 'hat':
                to.append('-joy-' + option + '-hat')
                to.append(str((int(_input.id) << 4) + int(_input.value)))
            if _input.type == 'axis':
                if option == 'up':
                    to.append('-joy-y')
                    to.append(str(_input.id))
                if option == 'left':
                    to.append('-joy-x')
                    to.append(str(_input.id))

    def addDpad(self, controller, to):
        self.addDpadItem('up', 'up', controller.inputs, to)
        self.addDpadItem('down', 'down', controller.inputs, to)
        self.addDpadItem('left', 'left', controller.inputs, to)
        self.addDpadItem('right', 'right', controller.inputs, to)
        pass

    # return true if the option is considered defined
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    @staticmethod
    def clearDisks(settings):
        for i in range(1, 3):
            settings.removeOption("s5d{}".format(i))
            settings.removeOption("s6d{}".format(i))
        for i in range(1, 33):
            settings.removeOption("s7d{}".format(i))

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
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
            settings.setOption("s{}d{}".format(slot, diskDrive), disk)
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
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            options.append("-x")
            options.append(str(resolution.width))
            options.append("-y")
            options.append(str(resolution.height))

        if self.defined('shaderset', system.config) and system.config['shaderset'] == 'scanlines':
            options.append("-scanline")
            options.append("40")

        # controller settings
        joystickOptions = []
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                joystickOptions = ["-joy", str(controller.index)]
                self.addGeneralButtons(controller, joystickOptions)
                self.addSpecialButtons(controller, joystickOptions)
                self.addJoystickLeft(controller, joystickOptions)
                self.addJoystickRight(controller, joystickOptions)
                self.addDpad(controller, joystickOptions)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        commandArray.extend(options)
        commandArray.extend(joystickOptions)

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
