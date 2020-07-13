#!/usr/bin/env python
import os.path

import Command
import libretroConfigurations
import recalboxFiles

from generators.Generator import Generator


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

    # Build appendable configurations files argument
    @staticmethod
    def getAppendConfigs(system, rom, externalOverrides):
        # Extra configs
        configs = []
        romName = os.path.basename(rom)

        # Custom configs - per core
        customCfg = "{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name)
        if os.path.isfile(customCfg):
            configs.append(customCfg)

        # Custom configs - per game
        customGameCfg = "{}/{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name, romName)
        if os.path.isfile(customGameCfg):
            configs.append(customGameCfg)

        # Overlay management
        overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.name, romName)
        if os.path.isfile(overlayFile):
            configs.append(overlayFile)

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
    def buildRomArguments(rom, core, verbose, demo): # type: (str, str, bool, bool) -> list
        # Remove old x68k config - Should find a better way later
        # Loading multi disk should go this way
        if os.path.exists(recalboxFiles.BIOS + "/keropi/config"):
            try:
                os.remove(recalboxFiles.BIOS + "/keropi/config")
            except Exception as ex:
                print("Exception: {}".format(ex))

        # quasi88 (Pc88) use retroarch subsystem's to load multiple content
        if core == "quasi88":
            from utils.diskCollector import DiskCollector
            collector = DiskCollector(rom, 6, verbose)
            if collector.Count > 1:
                roms = ["--subsystem", "pc88_{}_disk".format(collector.Count)]
                roms.extend(collector.Disks)
                return roms

        # Demo mode: take the first disk, always
        if demo:
            from utils.diskCollector import DiskCollector
            collector = DiskCollector(rom, 6, verbose)
            if collector.Count > 1:
                return [collector.Disks[0]]

        return [rom]