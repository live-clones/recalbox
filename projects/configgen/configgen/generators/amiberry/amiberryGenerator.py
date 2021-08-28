#!/usr/bin/env python
import os.path
import subprocess
from typing import List, Dict

from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.generators.amiberry.amiberryConfig import ConfigGenerator
from configgen.generators.amiberry.amiberryGlobalConfig import AmiberryGlobalConfig
from configgen.generators.amiberry.amiberryKickstarts import KickstartManager
from configgen.generators.amiberry.amiberryRomType import RomType
from configgen.generators.amiberry.amiberrySubSystems import SubSystems
from configgen.settings.keyValueSettings import keyValueSettings


class AmiberryGenerator(Generator):

    # Generate RP9 Arguments
    @staticmethod
    def getRP9Arguments(rom: str, system: SubSystems, _) -> List[str]:
        if system in SubSystems.COMPUTERS:
            # Package are self-configured so just return the package AS the config file
            # Add -G to force amiberry to run immediately. Otherwise the GUI is shown for some reasons
            return ["-config", rom, "-G"]
        raise Exception("PACKAGE not allowed on non-computer devices")

    # Generate ADF Arguments
    @staticmethod
    def getADFArguments(rom: str, system: SubSystems, configFile: ConfigGenerator) -> List[str]:
        from configgen.utils.diskCollector import DiskCollector
        collector = DiskCollector(rom, 4, True)
        configFile.SetFloppies(system, collector.disks)
        return []

    # Generate WHDL Arguments
    @staticmethod
    def getWHDLArguments(_, system: SubSystems, __) -> List[str]:
        if system in SubSystems.COMPUTERS:
            # Prepare final save folder
            sourceSaveFolder = os.path.join(recalboxFiles.amiberryMountPoint, "whdboot/save-data/Savegames")
            finalSaveFolder = os.path.join(recalboxFiles.SAVES, str(system), "whdl")
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
    def AddWHDLVolumes(settingFiles: str, rom: str):
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
    def getCDROMArguments(rom: str, system: SubSystems, configFile: ConfigGenerator) -> List[str]:
        # Set CD image
        configFile.SetCD(system, rom)
        return []

    # Generate HDFS Arguments
    @staticmethod
    def getHDDFSArguments(rom: str, system: SubSystems, configFile: ConfigGenerator) -> List[str]:
        if system in SubSystems.COMPUTERS:
            # mount an amiga volume DH0 to the mount point given by the rom argument
            configFile.SetHDDFS(system, rom)
            return []
        raise Exception("HDFS not allowed on non-computer devices")

    # Generate HDF Arguments
    @staticmethod
    def getHDFArguments(rom: str, system: SubSystems, configFile: ConfigGenerator) -> List[str]:
        if system in SubSystems.COMPUTERS:
            # mount an amiga volume DH0 using the filename given by the rom argument
            configFile.SetHDF(system, rom)
            return []
        raise Exception("HDF not allowed on non-computer devices")

    # Unknown rom processing
    @staticmethod
    def unknownRomType(rom: str, system: SubSystems, configFile: ConfigGenerator) -> List[str]:
        # Force quit
        raise Exception("Unknown rom type: {}".format(rom))

    # Get keyboard layout
    @staticmethod
    def GetKeyboardLayout(recalboxSettings: keyValueSettings):
        # Try to obtain from keyboard layout, then from system language, then fallback to us
        kl = recalboxSettings.getString("system.kblayout", recalboxSettings.getString("system.language", "us")[-2:]).lower()
        return kl

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def defined(key: str, dictio: Dict[str, str]):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:
        # Get rom type and associated configuration file if any
        rom, romType, romHasUAE = RomType.Identify(args.rom)

        # Get subsystem - Force A1200 with WHDL
        subSystem = SubSystems.A1200 if romType == RomType.WHDL else system.Name
        needSlowCPU = system.Name != subSystem

        # Generate global config file
        globalOverride = os.path.join(os.path.dirname(rom), ".amiberry.conf")
        globalConfig = AmiberryGlobalConfig(globalOverride,
                                            os.path.join(recalboxFiles.amiberryMountPoint, "conf/amiberry.conf"))
        globalConfig.createGlobalSettings(args.verbose, system.ShaderSet == 'scanlines')

        # Build default command
        settingsFullPath = os.path.join(recalboxFiles.amiberryMountPoint, "conf/default.uae")
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        # Prepare configuration file
        romFile, _ = os.path.splitext(rom)
        userSettings = romFile + ".uae" if romHasUAE else None
        overridenSettings = os.path.join(os.path.dirname(rom), ".uaeconfig.uae")
        configFile = ConfigGenerator(settingsFullPath)

        # Load default settings
        configFile.SetDefaultPath(subSystem)
        configFile.SetUI(AmiberryGenerator.GetKeyboardLayout(recalboxSettings), system.ShowFPS)
        configFile.SetInput(subSystem)
        configFile.SetJoystick(subSystem, playersControllers)
        configFile.SetCPU(subSystem, needSlowCPU)
        configFile.SetChipset(subSystem)
        configFile.SetMemory(subSystem)
        configFile.SetGraphics(subSystem)
        configFile.SetSound(subSystem)
        configFile.SetNetwork(romType in [RomType.WHDL, RomType.HDDFS, RomType.HDF])
        configFile.SetFloppies(subSystem, [])
        configFile.SetCD(subSystem, "")
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
            RomType.HDDFS: AmiberryGenerator.getHDDFSArguments,
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

        return Command(videomode=system.VideoMode, array=commandArray,
                               env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so",
                                    "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"},
                               cwdPath=recalboxFiles.amiberryMountPoint, postExec=None)
