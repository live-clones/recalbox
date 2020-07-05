#!/usr/bin/env python
import Command
import recalboxFiles
from generators.Generator import Generator
import os.path
import daphneControllers


class DaphneGenerator(Generator):
    # Main entry of the module
    # Configure daphne and return a command
    def generate(self, system, playersControllers, recalboxSettings, args):
        if not system.config['configfile']:
            daphneControllers.generateControllerConfig(system, playersControllers)

        romName = os.path.splitext(os.path.basename(args.rom))[0]
        frameFile = args.rom + "/" + romName + ".txt"
        commandsFile = args.rom + "/" + romName + ".commands"
        # the command to run  
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
            romName, "vldp",
            "-framefile", frameFile,
            "-fullscreen",
            "-useoverlaysb", "2",
            "-datadir", recalboxFiles.daphneDatadir,
            "-homedir", recalboxFiles.daphneHomedir]

        from utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            commandArray.extend(["-x", str(resolution.width), "-y", str(resolution.height)])

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        if os.path.isfile(commandsFile):
            commandArray.extend(open(commandsFile,'r').read().split())
        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"})
