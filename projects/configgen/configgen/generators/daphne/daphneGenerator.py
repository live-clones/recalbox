#!/usr/bin/env python
import Command
import recalboxFiles
from generators.Generator import Generator
import os.path
import daphneControllers

class DaphneGenerator(Generator):
    # Main entry of the module
    # Configure daphne and return a command
    def generate(self, system, rom, controllers):
        if not system.config['configfile']:
            daphneControllers.generateControllerConfig(system, controllers)

        romName = os.path.splitext(os.path.basename(rom))[0]
        frameFile = rom + "/" + romName + ".txt"
        commandsFile = rom + "/" + romName + ".commands"
        # the command to run  
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
            romName, "vldp",
            "-framefile", frameFile,
            "-fullscreen",
            "-nolog",
            "-homedir", recalboxFiles.daphneHomedir]
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        if os.path.isfile(commandsFile):
            commandArray.extend(open(commandsFile,'r').read().splitlines());
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libGLESv2.so"})
