#!/usr/bin/env python
import os.path
import glob
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import Controller, ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class ScummVMGenerator(Generator):

    # Main entry of the module
    # Return scummvm command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:
        # Create a temporary gamecontrollerdb.txt file with controllers mapping
        Controller.GenerateSDLGameDatabase(playersControllers, "/tmp/gamecontrollerdb.txt")

        # Find rom path
        if os.path.isdir(args.rom):
          # rom is a directory: must contains a <game name>.scummvm/.residualvm file
          romPath = args.rom
          romExt = os.path.splitext(romPath)[1]
          romFile = glob.glob(romPath + "/*" + romExt)[0]
          romName = os.path.splitext(os.path.basename(romFile))[0]
        else:
          # rom is a file: split in directory and file name
          romPath = os.path.dirname(args.rom)
          romName = os.path.splitext(os.path.basename(args.rom))[0]

        smooth = "--filtering" if system.Smooth else "--no-filtering"
        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "--fullscreen",
                        "--subtitles",
                        "--joystick=0",
                        smooth,
                        "--extrapath=/usr/share/scummvm",
                        "--savepath=" + recalboxFiles.scummvmSaves,
                        "--path=""{}""".format(romPath)]
        if system.ShaderSet == 'scanlines':
            commandArray.append("--gfx-mode=DotMatrix")

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append("""{}""".format(romName))

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER":"/usr/lib/libEGL.so", "SDL_RENDER_VSYNC":"1"})
