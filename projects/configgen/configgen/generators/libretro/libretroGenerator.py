#!/usr/bin/env python
import Command
import libretroControllers
import recalboxFiles
import libretroConfig
import shutil
from generators.Generator import Generator
import os.path


class LibretroGenerator(Generator):
    # Main entry of the module
    def config_upgrade(self, version):
        '''
        Upgrade the user's configuration file with new values added to the
        system configuration file upgraded by S11Share:do_upgrade()
        
        Args: 
            version (str): New Recalbox version
            
        Returns (bool):
            Returns True if this Generators sucessfully handled the upgrade.
        '''
        return libretroConfig.updateLibretroConfig(version)
    
    
    # Configure retroarch and return a command
    def generate(self, system, rom, playersControllers):
        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            system.config['configfile'] = recalboxFiles.retroarchCustom
            # Create retroarchcustom.cfg if does not exists
            if not os.path.isfile(recalboxFiles.retroarchCustom):
                shutil.copyfile(recalboxFiles.retroarchCustomOrigin, recalboxFiles.retroarchCustom)
            #  Write controllers configuration files
            libretroControllers.writeControllersConfig(system, playersControllers)
            # Write configuration to retroarchcustom.cfg
            libretroConfig.writeLibretroConfig(system, playersControllers)

        # Retroarch core on the filesystem
        retroarchCore = recalboxFiles.retroarchCores + system.config['core'] + recalboxFiles.libretroExt
        romName = os.path.basename(rom)

        # the command to run
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-L", retroarchCore, "--config", system.config['configfile']]
        configToAppend = []
        
        # Custom configs - per core
        customCfg = "{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name)
        if os.path.isfile(customCfg):
            configToAppend.append(customCfg)
        
        # Custom configs - per game
        customGameCfg = "{}/{}/{}.cfg".format(recalboxFiles.retroarchRoot, system.name, romName)
        if os.path.isfile(customGameCfg):
            configToAppend.append(customGameCfg)
        
        # Overlay management
        overlayFile = "{}/{}/{}.cfg".format(recalboxFiles.OVERLAYS, system.name, romName)
        if os.path.isfile(overlayFile):
            configToAppend.append(overlayFile)
        
        # Generate the append
        if configToAppend:
            commandArray.extend(["--appendconfig", "|".join(configToAppend)])
            
         # Netplay mode
        if system.config['netplaymode'] is not None and system.config['netplaymode'] in ('host', 'client'):
            if system.config['netplaymode'] == 'host':
                commandArray.append("--host")
		if system.config['hash']:
		    commandArray.extend(["--hash", system.config['hash']])
	    elif system.config['netplaymode'] == 'client':
                if system.config['netplay_ip'] is not None:
                    commandArray.extend(["--connect", system.config['netplay_ip']])
		else:
		    raise ValueError("You must specify n IP in client mode")
	    else:
		raise ValueError("Netplay mode should be host or client")
	    port = system.config.get('netplay_port', "55435")
	    commandArray.extend(["--port", port])
	    nick = system.config['netplay_nickname'] if system.config['netplay_nickname'] else "Anonymous"
	    nick += "@RECALBOX"
	    commandArray.extend(["--nick", nick])

        # Optionnal arguments
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
             
        commandArray.append(rom)
        return Command.Command(videomode=system.config['videomode'], array=commandArray)
