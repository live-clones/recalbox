#!/usr/bin/env python
import os.path

import Command
import libretroConfigurations
import recalboxFiles

from generators.Generator import Generator
import settings.recalboxSettings as recalSettings


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
    def getAppendConfigs(system, rom):
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

        if not configs:
            return []

        return ["--appendconfig", "|".join(configs)]

    # Create configuration file
    @staticmethod
    def createConfigurationFile(system, playersControllers, rom, demo, recalboxSettings):
        # Setup system configuration
        configuration = libretroConfigurations.LibretroConfiguration(system, playersControllers, rom, demo, recalboxSettings)
        configuration.createRetroarchConfiguration()
        configuration.createCoreConfiguration()

        return configuration.getRetroarchConfigurationFileName()

    # Configure retroarch and return a command
    def generate(self, system, rom, playersControllers, demo, recalboxSettings):
        configFileName = system.config.get("configfile", None)

        # Set recalbox default config file if no user defined one
        if configFileName is None:
            configFileName = self.createConfigurationFile(system, playersControllers, rom, demo, recalboxSettings)

        # Retroarch core on the filesystem
        retroarchCore = recalboxFiles.retroarchCores + system.config['core'] + recalboxFiles.libretroExt

        # The command to run
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-L", retroarchCore, "--config", configFileName]
        # Extra configs
        commandArray.extend(self.getAppendConfigs(system, rom))
        # Netplay mode
        commandArray.extend(self.getNetplayArguments(system))

        # Optionnal arguments from es_systems.cfg
        if 'extra' in system.config and system.config['extra'] is not None:
             commandArray.extend(system.config['extra'].split(" "))

        # Optionnal arguments from recalbox.conf
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
             
        commandArray.append(rom)
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
