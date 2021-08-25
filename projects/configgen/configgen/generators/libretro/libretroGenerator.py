#!/usr/bin/env python
import os.path

import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
import configgen.generators.libretro.libretroConfigurations as libretroConfigurations

from configgen.generators.Generator import Generator
from configgen.generators.libretro.libretroLightGuns import libretroLightGun


class LibretroGenerator(Generator):
    # Main entry of the module
    def config_upgrade(self, version):
        return libretroConfigurations.LibretroConfiguration.updateLibretroConfig(version)

    # Build command ligne arguments for Netplay
    @staticmethod
    def getNetplayArguments(system):
        # Netplay command holder
        result = []

        if 'netplaymode' in system.config and system.config['netplaymode'] in ('host', 'client'):
            if system.config['netplaymode'] == 'host':
                result.append("--host")
                if system.config['hash']:
                    result.extend(["--hash", system.config['hash']])
            elif system.config['netplaymode'] == 'client':
                if system.config['netplay_ip'] is not None:
                    result.extend(["--connect", system.config['netplay_ip']])
                else:
                    raise ValueError("You must specify n IP in client mode")
            else:
                raise ValueError("Netplay mode should be host or client")

            port = system.config.get('netplay_port', "55435")
            result.append("--port")
            result.append(port)
            nick = system.config.get('netplay_nickname', "Anonymous")
            result.append("--nick")
            result.append(nick)

        return result

    # Overlay management
    @staticmethod
    def processOverlays(system, romName, configs):
        # Overlays are applied only when we are not in wide core
        if system.config['core'] not in ["genesisplusgxwide"]:
            # User overlays
            userOverlayApplied = False
            overlayFile = "{}/{}/.overlay.cfg".format(recalboxFiles.OVERLAYS, system.name)
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
            overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.name, romName)
            if os.path.isfile(overlayFile):
                # Rom file overlay
                configs.append(overlayFile)
                userOverlayApplied = True
            else:
                overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.name, system.name)
                if os.path.isfile(overlayFile):
                    # System overlay
                    configs.append(overlayFile)
                    userOverlayApplied = True
            if not userOverlayApplied:
                # The recalbox overlays should be added only if
                # global.recalboxoverlays=1 or system.recalboxoverlays activated
                if 'recalboxoverlays' in system.config and system.config['recalboxoverlays'] == '1':
                    # ratio = we can activate when ratio is not 16/9 and 16/10
                    if 'ratio' not in system.config or system.config['ratio'] not in ["16/9", "16/10"]:
                        # screen resolution that can support overlays are over 1.5 ratio (as it is float > 1.51)
                        from configgen.utils.videoMode import getCurrentFramebufferRatio
                        if getCurrentFramebufferRatio() > 1.51:
                            defaultOverlayFile = "{}/{}/{}.cfg".format(recalboxFiles.RECALBOX_OVERLAYS, system.name, system.name)
                            if os.path.isfile(defaultOverlayFile):
                                configs.append(defaultOverlayFile)

    # Build appendable configurations files argument
    def getAppendConfigs(self, system, rom, externalOverrides):
        # Extra configs
        configs = []
        romName = os.path.basename(rom)

        # Custom configs - per core - DEPRECATED
        customCfg = "{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name)
        if os.path.isfile(customCfg):
            configs.append(customCfg)

        # Custom configs - per game - DEPRECATED
        customGameCfg = "{}/{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name, romName)
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
    def createConfigurationFile(system, playersControllers, rom, demo, nodefaultkeymap, recalboxSettings):
        # Setup system configuration
        configuration = libretroConfigurations.LibretroConfiguration(system, playersControllers, rom, demo, nodefaultkeymap, recalboxSettings)
        retroarchConfig, retroarchOverrides = configuration.createRetroarchConfiguration()
        coreConfig = configuration.createCoreConfiguration()
        commandArgs = configuration.getCommandLineArguments(retroarchConfig, coreConfig)

        # setup wiimotes lightgun configuration 
        lightgunConfig = libretroLightGun(system,rom,demo,retroarchConfig, coreConfig)
        lightgunConfig.createLightGunConfiguration()

        return configuration.getRetroarchConfigurationFileName(),\
               configuration.getRetroarchOverridesFileName(),\
               commandArgs

    # Configure retroarch and return a command
    def generate(self, system, playersControllers, recalboxSettings, args):
        configFileName = system.config.get("configfile", None)

        # Set recalbox default config file if no user defined one
        newConfigFileName, overrideFileName, commandArgs = self.createConfigurationFile(system, playersControllers, args.rom, args.demo, args.nodefaultkeymap, recalboxSettings)
        if configFileName is None:
            configFileName = newConfigFileName

        # Manage special scummvm roms
        # To keep compatibility with existing scummvm scraping systems as well as with the standalone core,
        # rom may contain the upper folder game_folder.scummvm
        # In such case we must look for the inner file.scummvm and use it instead
        rom = args.rom
        if system.config['core'] == 'scummvm':
            if os.path.isdir(args.rom):
                scummfiles = [fn for fn in os.listdir(args.rom) if fn.endswith('.scummvm')]
                if len(scummfiles) == 1:
                    rom = os.path.join(args.rom, scummfiles[0])

        # Retroarch core on the filesystem
        retroarchCore = recalboxFiles.retroarchCores + system.config['core'] + recalboxFiles.libretroExt

        # The command to run
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]
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
        commandArray.extend(self.buildExtraArguments(args.system, system.config['core']))

        # Optional arguments from es_systems.cfg
        if 'extra' in system.config and system.config['extra'] is not None:
             commandArray.extend(system.config['extra'].split(" "))

        # Optional arguments from recalbox.conf
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])

        # Sub-system roms (pc88)
        roms = self.buildRomArguments(rom, system.config['core'], args.verbose, args.demo)
        commandArray.extend(roms)

        return Command.Command(videomode=system.config['videomode'], array=commandArray)

    @staticmethod
    def buildExtraArguments(system, core): # type: (str, str) -> list
        if system == "neogeocd" and core == "fbneo":
            return ["--subsystem", "neocd"]

        return []

    @staticmethod
    def buildRomArguments(rom, core, verbose, demo): # type: (str, str, bool, bool) -> list
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
