import glob
import os

import Command
import recalboxFiles
from generators.Generator import Generator
from settings.keyValueSettings import keyValueSettings


class GSplusGenerator(Generator):

    MultiDiscMap = \
    {
        "Disc 1" : ["Disc 2" , "Disc 3" , "Disc 4" ],
        "Disk 1" : ["Disk 2" , "Disk 3" , "Disk 4" ],
        "Disc A" : ["Disc B" , "Disc C" , "Disc D" ],
        "Disk A" : ["Disk B" , "Disk C" , "Disk D" ],
        "disc 1" : ["disc 2" , "disc 3" , "disc 4" ],
        "disk 1" : ["disk 2" , "disk 3" , "disk 4" ],
        "disc A" : ["disc B" , "disc C" , "disc D" ],
        "disk A" : ["disk B" , "disk C" , "disk D" ],
        "Disc 01": ["Disc 02", "Disc 03", "Disc 04"],
        "Disk 01": ["Disk 02", "Disk 03", "Disk 04"],
        "disc 01": ["disc 02", "disc 03", "disc 04"],
        "disk 01": ["disk 02", "disk 03", "disk 04"],
    }

    # Generate ADF Arguments
    @staticmethod
    def SeekMultiDisks(rom):
        # Set disk #1
        disks = [rom]
        _, ext = os.path.splitext(rom)

        # Seek for next disks
        for first in GSplusGenerator.MultiDiscMap.keys():
            pos = rom.find(first)
            if pos > 0:
                nextDiskPattern = GSplusGenerator.MultiDiscMap[first]
                for i in range(10):
                    Found = False
                    nextDisk = rom[:pos] + nextDiskPattern[i] + rom[pos  +len(nextDiskPattern[i]):]
                    if os.path.exists(nextDisk):
                        disks.append(nextDisk)
                        Found = True
                    else:
                        # Try to seek for next disk with a different tailing text (TOSEC case)
                        nextDisk = rom[:pos] + nextDiskPattern[i] + "*" + ext
                        files = glob.glob(nextDisk)
                        if files is not None:
                            files.sort()  # Sort to get shortest name first
                            if len(files) > 0:
                                disks.append(files[0])
                                Found = True
                    if not Found:
                        break  # Needless to seek for next file

        return disks

    @staticmethod
    def addJoyItem(option, item, _type, inputs, to):
        # -> option: str, item: str, input:s: dict, to: list
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

    def generate(self, system, playersControllers, recalboxSettings, args):
        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launches
        """

        # Load config file
        settings = keyValueSettings(recalboxFiles.gsplusConfig, True)
        settings.loadFile(True)

        # Seek multidisk games
        disks = GSplusGenerator.SeekMultiDisks(args.rom)
        diskDrive = 1
        for disk in disks:
            settings.setOption("s7d{}".format(diskDrive), disk)
            diskDrive += 1

        # Save config file
        settings.saveFile()

        # Default options
        options = ["-fullscreen",
                   "-ssdir", recalboxFiles.SCREENSHOTS,
                   "-config", recalboxFiles.gsplusConfig]

        # Screen resolution
        from utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            options.append("-x")
            options.append(str(resolution.width))
            options.append("-y")
            options.append(str(resolution.height))

        if self.defined('shaders', system.config) and system.config['shaders'] == 'scanlines':
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
