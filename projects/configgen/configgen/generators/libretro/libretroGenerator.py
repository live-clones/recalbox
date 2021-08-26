#!/usr/bin/env python
import os.path
from typing import List

import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
from configgen.Emulator import Emulator

from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.generators.libretro.libretroLightGuns import libretroLightGun
from configgen.settings.keyValueSettings import keyValueSettings


class LibretroGenerator(Generator):

    # Main entry of the module
    def config_upgrade(self, version: str) -> bool:
        return libretroConfigurations.LibretroConfiguration.updateLibretroConfig(version)

    # Build command ligne arguments for Netplay
    @staticmethod
    def getNetplayArguments(system: Emulator) -> List[str]:
        # Netplay command holder
        result = []

        if system.Netplay:
            if system.NetplayHostMode:
                result.append("--host")
                if system.HasHash:
                    result.extend(["--hash", system.Hash])
            else:
                if len(system.NetplayIP) != 0:
                    result.extend(["--connect", system.NetplayIP])
                else:
                    raise ValueError("You must specify n IP in client mode")

            result.append("--port")
            result.append(str(system.NetplayPort))
            result.append("--nick")
            result.append(system.NetplayUsername)

        return result

    # Overlay management
    @staticmethod
    def processOverlays(system: Emulator, romName: str, configs: List[str]):
        # Overlays are applied only when we are not in wide core
        if system.Core not in ["genesisplusgxwide"]:
            # User overlays
            userOverlayApplied = False
            overlayFile = "{}/{}/.overlay.cfg".format(recalboxFiles.OVERLAYS, system.Name)
            if os.path.isfile(overlayFile):
                # System global configuration
                configs.append(overlayFile)
                userOverlayApplied = True
            else:
                overlayFile = "{}/.overlay.cfg".format(recalboxFiles.OVERLAYS)
                if os.path.isfile(overlayFile):
                    # All system global configuration
                    configs.append(overlayFile)
                    userOverlayApplied = True
            overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.Name, romName)
            if os.path.isfile(overlayFile):
                # Rom file overlay
                configs.append(overlayFile)
                userOverlayApplied = True
            else:
                overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.Name, system.Name)
                if os.path.isfile(overlayFile):
                    # System overlay
                    configs.append(overlayFile)
                    userOverlayApplied = True
            if not userOverlayApplied:
                # The recalbox overlays should be added only if
                # global.recalboxoverlays=1 or system.recalboxoverlays activated
                if system.RecalboxOverlays:
                    # ratio = we can activate when ratio is not 16/9 and 16/10
                    if system.Ratio not in ["16/9", "16/10"]:
                        # screen resolution that can support overlays are over 1.5 ratio (as it is float > 1.51)
                        from configgen.utils.videoMode import getCurrentFramebufferRatio
                        if getCurrentFramebufferRatio() > 1.51:
                            defaultOverlayFile = "{}/{}/{}.cfg".format(recalboxFiles.RECALBOX_OVERLAYS, system.Name, system.Name)
                            if os.path.isfile(defaultOverlayFile):
                                configs.append(defaultOverlayFile)

    # Build appendable configurations files argument
    def getAppendConfigs(self, system: Emulator, rom: str, externalOverrides: str) -> List[str]:
        # Extra configs
        configs = []
        romName = os.path.basename(rom)

        # Custom configs - per core - DEPRECATED
        customCfg = "{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.Name)
        if os.path.isfile(customCfg):
            configs.append(customCfg)

        # Custom configs - per game - DEPRECATED
        customGameCfg = "{}/{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.Name, romName)
        if os.path.isfile(customGameCfg):
            configs.append(customGameCfg)

        # Process overlays
        self.processOverlays(system, romName, configs)

        # In-place override takes priority over all
        if os.path.isfile(externalOverrides):
            configs.append(externalOverrides)

        if not configs:
            return []

        return ["--appendconfig", "|".join(configs)]

    # Create configuration file
    @staticmethod
    def createConfigurationFile(system: Emulator, playersControllers: ControllerDictionary, rom: str, demo: bool, nodefaultkeymap: bool, recalboxSettings: keyValueSettings) -> (str, str, List[str]):
        # Setup system configuration
        configuration = libretroConfigurations.LibretroConfiguration(system, playersControllers, rom, demo, nodefaultkeymap, recalboxSettings)
        retroarchConfig, retroarchOverrides = configuration.createRetroarchConfiguration()
        coreConfig = configuration.createCoreConfiguration()
        commandArgs = configuration.getCommandLineArguments(retroarchConfig, coreConfig)

        # setup wiimotes lightgun configuration 
        lightgunConfig = libretroLightGun(system, rom, demo, retroarchConfig, coreConfig)
        lightgunConfig.createLightGunConfiguration()

        return configuration.getRetroarchConfigurationFileName(),\
               configuration.getRetroarchOverridesFileName(),\
               commandArgs

    # Configure retroarch and return a command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):

        # Set recalbox default config file if no user defined one
        newConfigFileName, overrideFileName, commandArgs = self.createConfigurationFile(system, playersControllers, args.rom, args.demo, args.nodefaultkeymap, recalboxSettings)
        configFileName = system.ConfigFile if system.HasConfigFile else newConfigFileName

        # Manage special scummvm roms
        # To keep compatibility with existing scummvm scraping systems as well as with the standalone core,
        # rom may contain the upper folder game_folder.scummvm
        # In such case we must look for the inner file.scummvm and use it instead
        rom = args.rom
        if system.Core == 'scummvm':
            if os.path.isdir(args.rom):
                scummfiles = [fn for fn in os.listdir(args.rom) if fn.endswith('.scummvm')]
                if len(scummfiles) == 1:
                    rom = os.path.join(args.rom, scummfiles[0])

        # Retroarch core on the filesystem
        retroarchCore = recalboxFiles.retroarchCores + system.Core + recalboxFiles.libretroExt

        # The command to run
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]
        # Verbose?
        if args.verbose:
            commandArray.extend(["--verbose"])
        # Core & config
        commandArray.extend(["-L", retroarchCore, "--config", configFileName])
        # Extra configs - pass in-place override last
        commandArray.extend(self.getAppendConfigs(system, rom, overrideFileName))
        # Netplay mode
        commandArray.extend(self.getNetplayArguments(system))
        # Converted command args
        commandArray.extend(commandArgs)
        # Extra system/core arguments
        commandArray.extend(self.buildExtraArguments(args.system, system.Core))

        # Optional arguments from recalbox.conf
        if system.HasArgs: commandArray.extend(system.Args)

        # Sub-system roms (pc88)
        roms = self.buildRomArguments(rom, system.Core, args.verbose, args.demo)
        commandArray.extend(roms)

        return Command.Command(videomode=system.VideoMode, array=commandArray)

    @staticmethod
    def buildExtraArguments(system: str, core: str) -> List[str]:
        if system == "neogeocd" and core == "fbneo":
            return ["--subsystem", "neocd"]

        return []

    @staticmethod
    def buildRomArguments(rom: str, core: str, verbose: bool, demo: bool) -> List[str]:
        # x68000
        if core == "px68k":
            # Open configuration
            x68kconfig = os.path.join(recalboxFiles.BIOS, "keropi/config")
            from configgen.settings.iniSettings import IniSettings
            settings = IniSettings(x68kconfig, False)
            # HDD or floppy disk?
            size = os.path.getsize(rom) >> 20
            if size >= 2:
                # HDD
                settings.setOption("WinX68k", "HDD0", rom)
            else:
                # Floppy
                from configgen.utils.diskCollector import DiskCollector
                collector = DiskCollector(rom, 2, verbose)
                settings.setOption("WinX68k", "FDD0", collector.disks[0])
                if collector.Count > 1:
                    settings.setOption("WinX68k", "FDD1", collector.disks[1])
            settings.saveFile()

        # quasi88 (Pc88) use retroarch subsystem's to load multiple content
        if core == "quasi88":
            from configgen.utils.diskCollector import DiskCollector
            collector = DiskCollector(rom, 6, verbose)
            if collector.Count > 1:
                roms = ["--subsystem", "pc88_{}_disk".format(collector.Count)]
                roms.extend(collector.Disks)
                return roms

        # Vic20 core with split cartridges
        if core == "vice_xvic":
            rom, romExt = os.path.splitext(rom)
            cartridges = \
            {
                ".20": "-cart2",
                ".40": "-cart4",
                ".60": "-cart6",
                ".70": "-cart6",
                ".a0": "-cartA",
                ".b0": "-cartB",
            }
            if romExt in cartridges:
                carts = []
                for ext in cartridges:
                    if os.path.exists(rom + ext):
                        carts.append(cartridges[ext])
                        carts.append('"' + rom + ext + '"')
                return ['xvic {}'.format(' '.join(carts))]
            else:
                rom = rom + romExt

        # Demo mode: take the first disk, always
        if demo:
            from configgen.utils.diskCollector import DiskCollector
            collector = DiskCollector(rom, 6, verbose)
            if collector.Count > 1:
                return [collector.Disks[0]]

        return [rom]
