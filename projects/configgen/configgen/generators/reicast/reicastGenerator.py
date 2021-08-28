#!/usr/bin/env python
import shutil
import os.path
import glob
from configgen.Command import Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.generators.reicast.reicastControllers as reicastControllers

from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings


class ReicastGenerator(Generator):
    # Main entry of the module
    def config_upgrade(self, version):
        """
        Upgrade the user's configuration file with new values added to the
        system configuration file upgraded by S11Share:do_upgrade()

        Args:
            version (str): New Recalbox version

        Returns (bool):
            Returns True if this Generators sucessfully handled the upgrade.
        """
        # Copy the bios from share_init to share if it doesn't exis
        try:
            reicastBiosFileInit = recalboxFiles.BIOS_INIT + '/dc_nvmem.bin'
            reicastBiosFile = recalboxFiles.BIOS + '/dc_nvmem.bin'
            if os.path.isfile(reicastBiosFileInit) and not os.path.isfile(reicastBiosFile):
                shutil.copy2(reicastBiosFileInit, reicastBiosFile)

            # Copy the VMUs
            if not os.path.exists(recalboxFiles.SAVES + '/dreamcast/reicast/'):
                    os.makedirs(recalboxFiles.SAVES + '/dreamcast/reicast/')
            for srcFile in glob.glob(recalboxFiles.SAVES_INIT + '/dreamcast/reicast/vmu_save_*.bin'):
                destFile = recalboxFiles.SAVES + '/dreamcast/reicast/' + os.path.basename(srcFile)
                # Don't copy if the destination already exists
                if os.path.isfile(destFile): continue
                shutil.copy2(srcFile, destFile)

            print("ReicastGenerator 's configuration successfully upgraded")
            return True
        except Exception as e:
            print(e)
            print("ReicastGenerator 's configuration failed!")
            return False

    # Configure reicast and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:
        if not system.HasConfigFile:
            # Write emu.cfg to map joysticks, init with the default emu.cfg
            config = IniSettings(recalboxFiles.reicastConfigInit, True)
            config.loadFile(True)

            section = "input"
            # For each pad detected
            for index in playersControllers :
                controller = playersControllers[index]
                # Get the event number
                eventNum = controller.DevicePath.replace('/dev/input/event','')
                # Write its mapping file
                controllerConfigFile = reicastControllers.generateControllerConfig(controller)
                # set the evdev_device_id_X
                config.setString(section, 'evdev_device_id_{}'.format(controller.PlayerIndex), eventNum)
                # Set the evdev_mapping_X
                config.setString(section, 'evdev_mapping_{}'.format(controller.PlayerIndex), controllerConfigFile)

            config.saveFile()

        # the command to run  
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)
        # Here is the trick to make reicast find files :
        # emu.cfg is in $XDG_CONFIG_DIRS or $XDG_CONFIG_HOME. The latter is better
        # VMU will be in $XDG_DATA_HOME because it needs rw access -> /recalbox/share/saves/dreamcast
        # BIOS will be in $XDG_DATA_DIRS
        # controller cfg files are set with an absolute path, so no worry
        return Command(videomode=system.VideoMode, array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.reicastSaves, "XDG_DATA_DIRS":recalboxFiles.reicastBios})
