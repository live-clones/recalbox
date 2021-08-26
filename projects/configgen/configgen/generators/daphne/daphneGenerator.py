#!/usr/bin/env python
import os.path
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
import configgen.generators.daphne.daphneControllers as daphneControllers
from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class DaphneGenerator(Generator):

    # Main entry of the module
    # Configure daphne and return a command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        if not system.HasConfigFile:
            daphneControllers.generateControllerConfig(system, playersControllers)

        romName = os.path.splitext(os.path.basename(args.rom))[0]
        frameFile = args.rom + "/" + romName + ".txt"
        commandsFile = args.rom + "/" + romName + ".commands"
        singeFile = args.rom + "/" + romName + ".singe"
        # the command to run
        if os.path.exists(singeFile):
            # for a singe game
            commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                "singe", "vldp",
                "-framefile", frameFile,
                "-fullscreen",
                "-script", singeFile,
                "-blend_sprites",
                "-datadir", recalboxFiles.daphneDatadir,
                "-homedir", recalboxFiles.daphneHomedir]
        else:
            # for a classical game
            commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                romName, "vldp",
                "-framefile", frameFile,
                "-fullscreen",
                "-useoverlaysb", "2",
                "-datadir", recalboxFiles.daphneDatadir,
                "-homedir", recalboxFiles.daphneHomedir]

        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            commandArray.extend(["-x", str(resolution.width), "-y", str(resolution.height)])

        if system.HasArgs: commandArray.extend(system.Args)

        if os.path.isfile(commandsFile):
            commandArray.extend(open(commandsFile,'r').read().split())
        return Command.Command(videomode=system.VideoMode, array=commandArray, env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"})
