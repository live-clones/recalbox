#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class DaphneGenerator(Generator):

    # Main entry of the module
    # Configure daphne and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        if not system.HasConfigFile:
            import configgen.generators.daphne.daphneControllers as daphneControllers
            daphneControllers.generateControllerConfig(system, playersControllers)

        import os.path
        romName = os.path.splitext(os.path.basename(args.rom))[0]
        frameFile = args.rom + "/" + romName + ".txt"
        commandsFile = args.rom + "/" + romName + ".commands"
        singeFile = args.rom + "/" + romName + ".singe"
        # the command to run
        import configgen.recalboxFiles as recalboxFiles
        if os.path.exists(singeFile):
            # for a singe game
            commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                "singe", "vldp",
                "-framefile", frameFile,
                "-fullscreen",
                "-script", singeFile,
                "-retropath",
                "-grabmouse",
                "-manymouse",
                "-opengl",
                "-texturestream",
                "-datadir", recalboxFiles.daphneDatadir,
                "-homedir", recalboxFiles.daphneHomedir]

            # This block needs to have a way to detect dolphinbar out of libretro to be used
            # Enable lightgun if dolphin bar is connected, else enable mouse
            #lst = ['lbh-hd', 'johnnyrock-hd', 'drugwars-hd', 'johnnyrocknoir', 'spacepirates-hd', 'crimepatrol-hd', 'maddog2-hd', 'maddog-hd']
            #if romName in lst:
                #if dolphinBar:
                    #commandArray.extend(["-manymouse"])
                #else:
                    #commandArray.extend(["-grabmouse"])

            # Enable -oversize_overlay
            lst = ['lbh-hd', 'johnnyrock-hd', 'aurunmilla', 'drugwars-hd', 'johnnyrocknoir', 'spacepirates-hd', 'crimepatrol-hd', 'maddog2-hd', 'maddog-hd']
            if romName in lst:
                commandArray.extend(["-oversize_overlay"])

            # Enable -blend_sprites
            lst = ['lbh-hd', 'maddog', 'johnnyrock-hd', 'drugwars-hd', 'johnnyrocknoir', 'spacepirates-hd', 'maddog2', 'crimepatrol-hd', 'maddog2-hd', 'maddog-hd']
            if romName in lst:
                commandArray.extend(["-blend_sprites"])

            # Enable -nolinear_scale
            lst = ['lbh-hd', 'maddog', 'dltv', 'titanae', 'johnnyrock-hd', 'drugwars-hd', 'johnnyrocknoir', 'spacepirates-hd', 'chantze-hd', 'maddog2', 'hayate-hd', 'crimepatrol-hd', 'maddog2-hd', 'tron', 'suckerpunch', 'mononoke', 'maddog-hd']
            if romName in lst:
                commandArray.extend(["-nolinear_scale"])

            # Enable -bootsilent
            lst = ['dltv', 'titanae', 'tron', 'suckerpunch', 'mononoke']
            if romName in lst:
                commandArray.extend(["-bootsilent"])

            # Enable specific options for rollercoaster
            if romName == "rollercoaster":
                commandArray.extend(["-scanlines"])
                commandArray.extend(["-scanline_alpha 32"])
                commandArray.extend(["-blank_searches"])
                commandArray.extend(["-min_seek_delay 1000"])
                commandArray.extend(["-blank_blue"])

        else:
            # for a classical game
            commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                romName, "vldp",
                "-framefile", frameFile,
                "-fullscreen",
                "-useoverlaysb", "2",
                "-opengl",
                "-datadir", recalboxFiles.daphneDatadir,
                "-homedir", recalboxFiles.daphneHomedir]

        #from configgen.utils.architecture import Architecture
        #if Architecture().isX64:
        #    commandArray.extend(["-vulkan"])
        #else:
        #    commandArray.extend(["-opengl"])

        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            commandArray.extend(["-x", str(resolution.width), "-y", str(resolution.height)])

        if system.HasArgs: commandArray.extend(system.Args)

        if os.path.isfile(commandsFile):
            commandArray.extend(open(commandsFile,'r').read().split())
        return Command(videomode=system.VideoMode, array=commandArray, env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"})
