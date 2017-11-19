#!/usr/bin/env python
import Command
import recalboxFiles
from generators.Generator import Generator
import dolphinControllers
import shutil
import os.path
import ConfigParser
from settings.unixSettings import UnixSettings

class DolphinGenerator(Generator):
    def generate(self, system, rom, playersControllers):
        dolphinControllers.generateControllerConfig(system, playersControllers)

        if not system.config['configfile']:
            dolphinSettings = UnixSettings(recalboxFiles.dolphinIni, separator=' ')
            dolphinGFX = UnixSettings(recalboxFiles.dolphinGFX, separator=' ')
            #Draw or not FPS
            showFPS = "True" if system.config['showFPS'] == 'true' else "False"
            dolphinGFX.save("ShowFPS", showFPS)

            # Disable analytics
            dolphinSettings.save("PermissionAsked", "True")
            dolphinSettings.save("Enabled", "True")
            dolphinSettings.save("AutoHideCursor", "True")

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], "-e", rom]
        if 'args' in system.config and system.config['args'] is not None:
             commandArray.extend(system.config['args'])
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.CONF, "XDG_DATA_HOME":recalboxFiles.SAVES})
