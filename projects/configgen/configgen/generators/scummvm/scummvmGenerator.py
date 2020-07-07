#!/usr/bin/env python
import Command
import recalboxFiles
from controllersConfig import Controller
from generators.Generator import Generator
from generators.residualvm.residualvmGenerator import ResidualVMGenerator 
import os.path
import glob


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
    def generate(self, system, playersControllers, recalboxSettings, args):
        # Create a temporary gamecontrollerdb.txt file with controllers mapping
        Controller.generateSDLGameDBAllControllers(playersControllers, "/tmp/gamecontrollerdb.txt")
        # If ResidualVM extension, redirect to ResidualVM emulator
        if args.rom.endswith(".residualvm"):
            system.config['emulator'] = 'residualvm'
            return ResidualVMGenerator().generate(system, playersControllers, recalboxSettings, args)

        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            #system.config['configfile'] = recalboxFiles.mupenCustom
            pass
        
        # Find rom path
        if os.path.isdir(args.rom):
          # rom is a directory: must contains a <game name>.scummvm file
          romPath = args.rom
          romFile = glob.glob(romPath + "/*.scummvm")[0]
          romName = os.path.splitext(os.path.basename(romFile))[0]
        else:
          # rom is a file: split in directory and file name
          romPath = os.path.dirname(args.rom)
          romName = os.path.splitext(os.path.basename(args.rom))[0]

        smooth = "--filtering" if self.enabled('smooth', system.config) else "--no-filtering"
        scanline = "--gfx-mode=DotMatrix" if self.defined('shaders', system.config) and system.config['shaders'] == 'scanlines' else ""
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
                        "--fullscreen",
                        "--joystick=0",
                        "--extrapath=/usr/share/scummvm",
                        "--savepath="+recalboxFiles.scummvmSaves,
                        "--subtitles"
                        "--path=""{}""".format(romPath),
                        smooth, scanline]
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append("""{}""".format(romName))

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so"})
