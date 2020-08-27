#!/usr/bin/env python
import Command
import recalboxFiles
from controllersConfig import Controller
from generators.Generator import Generator
import os.path
import glob


class ResidualVMGenerator(Generator):
    # Main entry of the module
    # Return residualvm command
    def generate(self, system, playersControllers, recalboxSettings, args):
        # Create a temporary gamecontrollerdb.txt file with controllers mapping
        Controller.generateSDLGameDBAllControllers(playersControllers, "/tmp/gamecontrollerdb.txt")
        # Find rom path
        if os.path.isdir(args.rom):
          # rom is a directory: must contains a <game name>.residualvm file
          romPath = args.rom
          romFile = glob.glob(romPath + "/*.residualvm")[0]
          romName = os.path.splitext(os.path.basename(romFile))[0]
        else:
          # rom is a file: split in directory and file name
          romPath = os.path.dirname(args.rom)
          romName = os.path.splitext(os.path.basename(args.rom))[0]

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
                        "--fullscreen",
                        "--joystick=0",
                        "--extrapath=/usr/share/residualvm",
                        "--savepath="+recalboxFiles.residualvmSaves,
                        "--path=""{}""".format(romPath)]

        # Set renderer if not default
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append("""{}""".format(romName))

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so","SDL_VIDEO_EGL_DRIVER":"/usr/lib/libEGL.so"})

    # return true if the option is considered defined
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def enabled(key, dictio):
        return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')

