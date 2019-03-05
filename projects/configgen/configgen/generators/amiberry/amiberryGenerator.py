#!/usr/bin/env python
import glob
import os.path
import subprocess

import Command
import recalboxFiles
from generators.Generator import Generator
from generators.amiberry.amiberryConfig import ConfigGenerator
from generators.amiberry.amiberryGlobalConfig import AmiberryGlobalConfig
from generators.amiberry.amiberryKickstarts import KickstartManager
from generators.amiberry.amiberryRomType import RomType
from generators.amiberry.amiberrySubSystems import SubSystems
from settings.keyValueSettings import keyValueSettings


class AmiberryGenerator(Generator):

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
    def getRP9Arguments(rom, system, _):
        if system in SubSystems.COMPUTERS:
            # Package are self-configured so just return the package AS the config file
            # Add -G to force amiberry to run immediately. Otherwise the GUI is shown for some reasons
            return ["-config=" + rom, "-G"]
        raise Exception("PACKAGE not allowed on non-computer devices")

    # Generate ADF Arguments
    @staticmethod
    def getADFArguments(rom, system, configFile):
        # Set disk #1
        disks = [rom]
        _, ext = os.path.splitext(rom)

        # Seek for next disks
        for first in AmiberryGenerator.MultiDiscMap.keys():
            pos = rom.find(first)
            if pos > 0:
                nextDiskPattern = AmiberryGenerator.MultiDiscMap[first]
                for i in range(3):
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

        configFile.SetFloppies(system, disks)
        return []

    # Generate WHDL Arguments
    @staticmethod
    def getWHDLArguments(rom, system, configFile):
        del rom, configFile
        if system in SubSystems.COMPUTERS:
            # Prepare final save folder
            sourceSaveFolder = os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/save-data/Savegames")
            finalSaveFolder = os.path.join(recalboxFiles.SAVES, system, "whdl")
            subprocess.check_output(["mkdir", "-p", finalSaveFolder])

            # Copy whdl structure
            if os.path.islink(sourceSaveFolder):
                os.remove(sourceSaveFolder)  # Remove soft link
            subprocess.check_output(["cp", "-r", "/usr/share/amiberry/whdboot", recalboxFiles.amiberryMountPoint])

            # Delete save-game and redirect to finalSaveFolder
            os.removedirs(sourceSaveFolder)  # Remove true directory
            #subprocess.check_output(["rm", "-rf", sourceSaveFolder])
            subprocess.check_output(["ln", "-s", finalSaveFolder, sourceSaveFolder])

            # Create symlinks
            KickstartManager.GenerateWHDSymLinks(os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/save-data/Kickstarts"))

            # Do not use ["-autowhdload=" + rom] cause it requires a special game configuration
            return []
        raise Exception("WHDL not allowed on non-computer devices")

    # Generate raw WHD config
    @staticmethod
    def AddWHDLVolumes(settingFiles, rom):
        volumes = \
        [
            "rw,DH0:DH0:{},10".format(os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/boot-data.zip")),
            "rw,DH1:games:{},0".format(rom),
            "rw,DH2:saves:{},0".format(os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/save-data/")),
        ]
        with open(settingFiles, "a") as sf:
            index = 0
            for volume in volumes:
                sf.write("filesystem2=" + volume + '\n')
                sf.write("uaehf{}=dir,".format(index) + volume + '\n')
                index += 1

    # Generate CDROM Arguments
    @staticmethod
    def getCDROMArguments(rom, system, configFile):
        # Set CD image
        configFile.SetCD(system, rom)
        return []

    # Generate HDFS Arguments
    @staticmethod
    def getHDFSArguments(rom, system, configFile):
        if system in SubSystems.COMPUTERS:
            # mount an amiga volume DH0 to the mount point given by the rom argument
            configFile.SetHDFS(system, rom)
            return []
        raise Exception("HDFS not allowed on non-computer devices")

    # Generate HDF Arguments
    @staticmethod
    def getHDFArguments(rom, system, configFile):
        if system in SubSystems.COMPUTERS:
            # mount an amiga volume DH0 using the filename given by the rom argument
            configFile.SetHDF(system, rom)
            return []
        raise Exception("HDF not allowed on non-computer devices")

    # Unknown rom processing
    @staticmethod
    def unknownRomType(rom, system, configFile):
        # Force quit
        raise Exception("Unknown rom type: {}".format(rom))

    # Get keyboard layout
    @staticmethod
    def GetKeyboardLayout():
        conf = keyValueSettings(recalboxFiles.recalboxConf)
        conf.loadFile(True)
        # Try to obtain from keyboard layout, then from system language, then fallback to us
        kl = conf.getOption("system.kblayout", conf.getOption("system.language", "us")[-2:]).lower()
        return kl

    # Main entry of the module
    # Return command
    def generate(self, system, rom, playersControllers):
        # Get rom type and associated configuration file if any
        rom, romType, romHasUAE = RomType.Identify(rom)

        # Get subsystem - Force A1200 if WHDL detected (https://github.com/midwan/amiberry/issues/417)
        subSystem = SubSystems.A1200 if romType == RomType.WHDL else system.name

        # Generate global config file
        globalOverride = os.path.join(os.path.dirname(rom), ".amiberry.conf")
        globalConfig = AmiberryGlobalConfig(globalOverride,
                                            os.path.join(recalboxFiles.amiberryMountPoint, "conf/amiberry.conf"))
        globalConfig.createGlobalSettings()

        # Build default command
        settingsFullPath = os.path.join(recalboxFiles.amiberryMountPoint, "conf/uaeconfig.uae")
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]

        # Prepare configuration file
        romFile, _ = os.path.splitext(rom)
        userSettings = romFile + ".uae" if romHasUAE else None
        overridenSettings = os.path.join(os.path.dirname(rom), ".uaeconfig.uae")
        configFile = ConfigGenerator(settingsFullPath)

        # Load default settings
        configFile.SetDefaultPath(subSystem)
        configFile.SetUI(AmiberryGenerator.GetKeyboardLayout())
        configFile.SetInput(subSystem)
        configFile.SetJoystick(subSystem, playersControllers)
        configFile.SetCPU(subSystem)
        configFile.SetChipset(subSystem)
        configFile.SetMemory(subSystem)
        configFile.SetGraphics(subSystem)
        configFile.SetSound(subSystem)
        configFile.SetNetwork(romType in [RomType.WHDL, RomType.HDDFS, RomType.HDF])
        configFile.SetFloppies(subSystem, [])
        configFile.SetCD(subSystem, None)
        configFile.SetKickstarts(subSystem, romType)

        # Load overriden settings of current system
        if os.path.exists(overridenSettings):
            print("LOAD SYSTEM SETTINGS! " + userSettings)
            configFile.loadConfigFile(overridenSettings)
        # Load user settings
        if userSettings is not None:
            if os.path.exists(userSettings):
                print("LOAD USER SETTINGS! " + userSettings)
                configFile.loadConfigFile(userSettings)

        # Get arguments (and write configuration) regarding the rom type
        switcher = \
        {
            RomType.UNKNOWN: AmiberryGenerator.unknownRomType,
            RomType.DISK: AmiberryGenerator.getADFArguments,
            RomType.WHDL: AmiberryGenerator.getWHDLArguments,
            RomType.CDROM: AmiberryGenerator.getCDROMArguments,
            RomType.HDDFS: AmiberryGenerator.getHDFSArguments,
            RomType.HDF: AmiberryGenerator.getHDFArguments,
            RomType.PACKAGE: AmiberryGenerator.getRP9Arguments,
        }
        # Get the function from switcher dictionary
        func = switcher.get(romType)
        # Execute the function and get arguments
        commandArray.extend(func(rom, subSystem, configFile))

        # Add uae config, if no-one has been set before
        configExists = len([command for command in commandArray if "-config=" in command]) > 0
        if not configExists:
            # Insert the configuration at position #1, right after the amiberry executable
            # Amiberry ignore the config file after "rom" files
            commandArray.insert(1, "-config=" + settingsFullPath)

        # Save configuration
        configFile.saveConfigFile()

        # Generate specoal WHDL raw config
        if romType == RomType.WHDL:
            AmiberryGenerator.AddWHDLVolumes(settingsFullPath, rom)

        return Command.Command(videomode=system.config['videomode'], array=commandArray,
                               env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so",
                                    "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libGLESv2.so"},
                               cwdPath=recalboxFiles.amiberryMountPoint, postExec=None)
