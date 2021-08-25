#!/usr/bin/env python
import os.path
import subprocess

import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.generators.amiberry.amiberryConfig import ConfigGenerator
from configgen.generators.amiberry.amiberryGlobalConfig import AmiberryGlobalConfig
from configgen.generators.amiberry.amiberryKickstarts import KickstartManager
from configgen.generators.amiberry.amiberryRomType import RomType
from configgen.generators.amiberry.amiberrySubSystems import SubSystems
from settings.keyValueSettings import keyValueSettings


class AmiberryGenerator(Generator):

    # Generate RP9 Arguments
    @staticmethod
    def getRP9Arguments(rom, system, _):
        if system in SubSystems.COMPUTERS:
            # Package are self-configured so just return the package AS the config file
            # Add -G to force amiberry to run immediately. Otherwise the GUI is shown for some reasons
            return ["-config", rom, "-G"]
        raise Exception("PACKAGE not allowed on non-computer devices")

    # Generate ADF Arguments
    @staticmethod
    def getADFArguments(rom, system, configFile):
        from configgen.utils.diskCollector import DiskCollector
        collector = DiskCollector(rom, 4, True)
        configFile.SetFloppies(system, collector.disks)
        return []

    # Generate WHDL Arguments
    @staticmethod
    def getWHDLArguments(_, system, __):
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
    def GetKeyboardLayout(recalboxSettings):
        # Try to obtain from keyboard layout, then from system language, then fallback to us
        kl = recalboxSettings.getOption("system.kblayout", recalboxSettings.getOption("system.language", "us")[-2:]).lower()
        return kl

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        # Get rom type and associated configuration file if any
        rom, romType, romHasUAE = RomType.Identify(args.rom)

        # Get subsystem - Force A1200 with WHDL
        subSystem = SubSystems.A1200 if romType == RomType.WHDL else system.name
        needSlowCPU = system.name != subSystem

        # Generate global config file
        globalOverride = os.path.join(os.path.dirname(rom), ".amiberry.conf")
        globalConfig = AmiberryGlobalConfig(globalOverride,
                                            os.path.join(recalboxFiles.amiberryMountPoint, "conf/amiberry.conf"))
        scanline = self.defined('shaders', system.config) and system.config['shaders'] == 'scanlines'
        globalConfig.createGlobalSettings(args.verbose, scanline)

        # Build default command
        settingsFullPath = os.path.join(recalboxFiles.amiberryMountPoint, "conf/default.uae")
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]

        # Prepare configuration file
        romFile, _ = os.path.splitext(rom)
        userSettings = romFile + ".uae" if romHasUAE else None
        overridenSettings = os.path.join(os.path.dirname(rom), ".uaeconfig.uae")
        configFile = ConfigGenerator(settingsFullPath)

        # Load default settings
        configFile.SetDefaultPath(subSystem)
        configFile.SetUI(AmiberryGenerator.GetKeyboardLayout(recalboxSettings), system.config['showFPS'] == 'true')
        configFile.SetInput(subSystem)
        configFile.SetJoystick(subSystem, playersControllers)
        configFile.SetCPU(subSystem, needSlowCPU)
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
            configFile.loadConfigFile(overridenSettings)
        # Load user settings
        if userSettings is not None:
            if os.path.exists(userSettings):
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
        configExists = len([command for command in commandArray if "-config" in command]) > 0
        if not configExists:
            # Insert the configuration at position #1, right after the amiberry executable
            # Amiberry ignore the config file after "rom" files
            if romType == RomType.WHDL :
                commandArray.insert(1, rom)
                commandArray.insert(1, "-autoload")
            else :
                commandArray.insert(1, settingsFullPath)
                commandArray.insert(1, "-config")

        # Save configuration
        configFile.saveConfigFile()

        # Generate special WHDL raw config
        if romType == RomType.WHDL:
            AmiberryGenerator.AddWHDLVolumes(settingsFullPath, rom)

        return Command.Command(videomode=system.config['videomode'], array=commandArray,
                               env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so",
                                    "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"},
                               cwdPath=recalboxFiles.amiberryMountPoint, postExec=None)
