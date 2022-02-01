"""
Created on Mar 6, 2016

@author: Laurent Marchelli
"""

from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings


class LinappleGenerator(Generator):
    """
    Command line generator for linapple-pie emulator
    
    Ensure the user's configuration directory has all needed files to run
    linapple emulator and manage configuration file to tune emulator behaviour 
    with current hardware configuration.
    
    Args:
        pathInit (str):
            Full path name where default settings are stored.
            ('/recalbox/share_init/system/.linapple')
        
        pathUser (str):
            Full path name where user settings are stored.
            ('/recalbox/share/system/.linapple')
            
    """
    def __init__(self, pathInit, pathUser):
        self.pathInit = pathInit
        self.pathUser = pathUser
        self.resources = ['Master.dsk']
        self.filename = 'linapple.conf'

    def check_resources(self) -> bool:
        """
        Check system needed resources
        
        Returns (bool:
            Returns True if the check suceeded, False otherwise.
        """
        # Create user setting path, if it does not exists
        import os
        if not os.path.exists(self.pathUser):
            os.makedirs(self.pathUser)

        # Ensure system configuration file is available
        sys_conf = os.path.join(self.pathInit, self.filename)
        if not os.path.exists(sys_conf):
            return False

        # Ensure system resources are available
        for r in self.resources:
            sys_filename = os.path.join(self.pathInit, r)
            if not os.path.exists(sys_filename):
                return False
            usr_filename = os.path.join(self.pathUser, r)
            if not os.path.exists(usr_filename):
                import shutil
                shutil.copyfile(sys_filename, usr_filename)

        return True

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args):
        # Check resources
        if not self.check_resources():
            return

        if not system.HasConfigFile:
            # Load config file
            import os
            usr_conf = os.path.join(self.pathUser, self.filename)
            filename = usr_conf \
                if os.path.exists(usr_conf) \
                else os.path.join(self.pathInit, self.filename)
            from configgen.generators.linapple.linappleConfig import LinappleConfig
            config = LinappleConfig(filename=filename)
            config.joysticks(playersControllers)
            config.system(system, args.rom)
            # Save changes
            config.save(filename=usr_conf)

        commandArray = [ recalboxFiles.recalboxBins[system.Emulator] ]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)

    def config_upgrade(self, version: str):
        """
        Upgrade the user's configuration file with new values added to the
        system configuration file upgraded by S11Share:do_upgrade()
        
        Args: 
            version (str): New Recalbox version
            
        Returns (bool):
            Returns True if this Generators sucessfully handled the upgrade.
        """
        # Check resources
        if not self.check_resources(): 
            return False

        # Load system configuration file
        import os
        from configgen.generators.linapple.linappleConfig import LinappleConfig
        config = LinappleConfig(filename=os.path.join(self.pathInit, self.filename))

        # If an user's configuration file exists, upgrade it
        usr_conf = os.path.join(self.pathUser, self.filename)
        if os.path.exists(usr_conf):
            config_sys = config
            config = LinappleConfig(filename=usr_conf)
            for k,v in config_sys.settings.items():
                if k not in config.settings:
                    config.settings[k]=v

        # Save config file (original/updated) to user's directory
        config.save(filename=usr_conf)
        print("{} 's configuration successfully upgraded".format(self.__class__.__name__))
        return True
