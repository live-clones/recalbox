#!/usr/bin/env python
from typing import List

import configgen.recalboxFiles as recalboxFiles
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class LibretroGenerator(Generator):

    # Main entry of the module
    def config_upgrade(self, version: str) -> bool:
        import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
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
    def processOverlays(system: Emulator, romName: str, configs: List[str], recalboxOptions: keyValueSettings):
        import os.path
        # If we are in crt mode, we only allow recalbox default 240p overlays
        if system.CRTEnabled:
            if system.RecalboxOverlays:
                crtOverlayFile = "{}/{}/{}.cfg".format(recalboxFiles.RECALBOX_240P_OVERLAYS, system.Name,
                                                           system.Name)
                if os.path.isfile(crtOverlayFile):
                    configs.append(crtOverlayFile)
        # Overlays are applied only when we are not in wide core
        else:
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
                                defaultOverlayFile = "{}/{}/{}.cfg".format(recalboxFiles.RECALBOX_OVERLAYS, system.Name,
                                                                           system.Name)
                                if os.path.isfile(defaultOverlayFile):
                                    configs.append(defaultOverlayFile)

    # Build appendable configurations files argument
    def getAppendConfigs(self, system: Emulator, rom: str, externalOverrides: str, recalboxOptions: keyValueSettings) -> List[str]:
        # Extra configs
        configs = []
        import os.path
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
        self.processOverlays(system, romName, configs, recalboxOptions)

        # In-place override takes priority over all
        if os.path.isfile(externalOverrides):
            configs.append(externalOverrides)

        if not configs:
            return []

        return ["--appendconfig", "|".join(configs)]

    # Create crt configuration
    @staticmethod
    def createCrtConfiguration(system: Emulator, rom: str, recalboxOptions: keyValueSettings,
                               retroarchConfig: keyValueSettings, coreConfig: keyValueSettings,
                               retroarchOverrides: keyValueSettings):
        # recalbox-crt-options.cfg options

        # Retroarch CRT configuration
        from configgen.generators.libretro.crt.LibretroConfigCRT import LibretroConfigCRT
        from configgen.crt.CRTConfigParser import CRTConfigParser
        from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
        libretro_crt_configurator = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter())
        for option in libretro_crt_configurator.createConfigFor(system, rom).items():
            retroarchConfig.setString(option[0], option[1])
        # Core configuration
        from configgen.generators.libretro.crt.LibretroCoreConfigCRT import LibretroCoreConfigCRT
        core_config = LibretroCoreConfigCRT().createConfigFor(system)
        for core_option in core_config.items():
            coreConfig.setString(core_option[0], core_option[1])

        retroarchConfig.saveFile()
        coreConfig.saveFile()

        # Most specific code ever (it's here because of *.retroarch.cfg in /recalbox/share_init/roms/vectrex)
        if system.Name == "vectrex":
            retroarchOverrides.setString("aspect_ratio_index", retroarchConfig.getString("aspect_ratio_index", "24"))
            retroarchOverrides.saveFile()

    # Create zerolag configuration
    @staticmethod
    def createZeroLagConfiguration(retroarchConfig: keyValueSettings, enabled: bool):
        defaults = {
            "video_max_swapchain_images": "3",
            "video_hard_sync": "false",
            "video_hard_sync_frames": "0",
            "video_frame_delay_auto": "false",
            "run_ahead_secondary_instance": "false",
            "run_ahead_enabled": "false",
            "run_ahead_frames": "1"
        }
        activated = {
            "video_max_swapchain_images": "1",
            "video_hard_sync": "true",
            "video_hard_sync_frames": "1",
            "video_frame_delay_auto": "true",
            "run_ahead_secondary_instance": "false",
            "run_ahead_enabled": "true",
            "run_ahead_frames": "1"
        }
        configToSet = activated if enabled else defaults
        for option in configToSet.items():
            retroarchConfig.setString(option[0], option[1])
        retroarchConfig.saveFile()

    # Create configuration file
    @staticmethod
    def createConfigurationFile(system: Emulator, playersControllers: ControllerPerPlayer, rom: str, demo: bool,
                                nodefaultkeymap: bool, recalboxOptions: keyValueSettings) -> (str, str, List[str]):
        # Setup system configuration
        import configgen.generators.libretro.libretroConfigurations as libretroConfigurations
        configuration = libretroConfigurations.LibretroConfiguration(system, playersControllers, rom, demo,
                                                                     nodefaultkeymap, recalboxOptions)
        retroarchConfig, retroarchOverrides = configuration.createRetroarchConfiguration()
        coreConfig = configuration.createCoreConfiguration()
        commandArgs = configuration.getCommandLineArguments(retroarchConfig, coreConfig)

        # setup wiimotes lightgun configuration
        from configgen.generators.libretro.libretroLightGuns import libretroLightGun
        lightgunConfig = libretroLightGun(system, rom, demo, retroarchConfig, coreConfig)
        lightgunConfig.createLightGunConfiguration()

        # crt config
        if system.CRTEnabled:
            LibretroGenerator.createCrtConfiguration(system, rom, recalboxOptions, retroarchConfig, coreConfig,
                                                     retroarchOverrides)
        # zerodelay config
        LibretroGenerator.createZeroLagConfiguration(retroarchConfig, system.ZeroLag)

        return configuration.getRetroarchConfigurationFileName(), \
               configuration.getRetroarchOverridesFileName(), \
               commandArgs

    # Configure retroarch and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings,
                 args):

        # Set recalbox default config file if no user defined one
        newConfigFileName, overrideFileName, commandArgs = self.createConfigurationFile(system, playersControllers,
                                                                                        args.rom, args.demo,
                                                                                        args.nodefaultkeymap,
                                                                                        recalboxOptions)
        configFileName = system.ConfigFile if system.HasConfigFile else newConfigFileName

        # Manage special scummvm roms
        # To keep compatibility with existing scummvm scraping systems as well as with the standalone core,
        # rom may contain the upper folder game_folder.scummvm
        # In such case we must look for the inner file.scummvm and use it instead
        rom = args.rom
        if system.Core == 'scummvm':
            import os.path
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
        commandArray.extend(self.getAppendConfigs(system, rom, overrideFileName, recalboxOptions))
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

        return Command(videomode=system.VideoMode, array=commandArray)

    @staticmethod
    def buildExtraArguments(system: str, core: str) -> List[str]:
        if system == "neogeocd" and core == "fbneo":
            return ["--subsystem", "neocd"]
        if system == "channelf" and core == "fbneo":
            return ["--subsystem", "chf"]

        return []

    @staticmethod
    def buildRomArguments(rom: str, core: str, verbose: bool, demo: bool) -> List[str]:
        # x68000
        if core == "px68k":
            # Open configuration
            import os.path
            x68kconfig = os.path.join(recalboxFiles.BIOS, "keropi/config")
            from configgen.settings.iniSettings import IniSettings
            settings = IniSettings(x68kconfig, False)
            # HDD or floppy disk?
            size = os.path.getsize(rom) >> 20
            if size >= 2:
                # HDD
                settings.setString("WinX68k", "HDD0", rom)
            else:
                # Floppy
                from configgen.utils.diskCollector import DiskCollector
                collector = DiskCollector(rom, 2, verbose)
                settings.setString("WinX68k", "FDD0", collector.disks[0])
                if collector.Count > 1:
                    settings.setString("WinX68k", "FDD1", collector.disks[1])
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
            import os.path
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
