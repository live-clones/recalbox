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

    def generate(self, system, rom, playersControllers, demo, recalboxSettings):

        '''
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launches
        '''

        # Load config file
        settings = keyValueSettings(recalboxFiles.gsplusConfig, True)
        settings.loadFile(True)

        # Seek multidisk games
        disks = GSplusGenerator.SeekMultiDisks(rom)
        diskDrive = 1
        for disk in disks:
            settings.setOption("s7d{}".format(diskDrive), disk)

        # Save config file
        settings.saveFile()

        # Default options
        options = ["-fullscreen",
                   "-ssdir", recalboxFiles.SCREENSHOTS,
                   "-config", recalboxFiles.gsplusConfig]

        # controller settings
        joystickOptions = []
        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                joystickOptions = ["-joy", str(controller.index),
                                   "-joy-x", str(controller.inputs["joystick1left"].id),
                                   "-joy-y", str(controller.inputs["joystick1up"].id),
                                   "-joy-x2", str(controller.inputs["joystick2left"].id),
                                   "-joy-y2", str(controller.inputs["joystick2up"].id),
                                   "-joy-bhk", str(controller.inputs["hotkey"].id),
                                   "-joy-bstart", str(controller.inputs["start"].id),
                                   "-joy-b0", str(controller.inputs["a"].id),
                                   "-joy-b1", str(controller.inputs["b"].id),
                                   "-joy-b2", str(controller.inputs["x"].id),
                                   "-joy-b3", str(controller.inputs["y"].id)]

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
        commandArray.extend(options)
        commandArray.extend(joystickOptions)

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
