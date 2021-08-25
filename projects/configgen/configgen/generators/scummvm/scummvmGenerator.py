#!/usr/bin/env python
import os.path
import glob
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.controllersConfig import Controller
from configgen.generators.Generator import Generator, ControllerDictionary
from settings.keyValueSettings import keyValueSettings


class ScummVMGenerator(Generator):

    # return true if the option is considered defined
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def enabled(key, dictio):
        return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')

    # Main entry of the module
    # Return scummvm command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        # Create a temporary gamecontrollerdb.txt file with controllers mapping
        Controller.generateSDLGameDBAllControllers(playersControllers, "/tmp/gamecontrollerdb.txt")

        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            #system.config['configfile'] = recalboxFiles.mupenCustom
            pass

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

        smooth = "--filtering" if self.enabled('smooth', system.config) else "--no-filtering"
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
                        "--fullscreen",
                        "--subtitles",
                        "--joystick=0",
                        smooth,
                        "--extrapath=/usr/share/scummvm",
                        "--savepath="+recalboxFiles.scummvmSaves,
                        "--path=""{}""".format(romPath)]
        if self.defined('shaderset', system.config) and system.config['shaderset'] == 'scanlines':
            commandArray.append("--gfx-mode=DotMatrix")

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        commandArray.append("""{}""".format(romName))

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER":"/usr/lib/libEGL.so", "SDL_RENDER_VSYNC":"1"})
