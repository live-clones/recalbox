#!/usr/bin/env python
import Command
import recalboxFiles
from controllersConfig import Controller
from generators.Generator import Generator
from generators.residualvm.residualvmGenerator import ResidualVMGenerator 
import os.path
import glob


class ScummVMGenerator(Generator):
    # Main entry of the module
    # Return scummvm command
    def generate(self, system, rom, playersControllers, demo, recalboxSettings):
        # Create a temporary gamecontrollerdb.txt file with controllers mapping
        Controller.generateSDLGameDBAllControllers(playersControllers, "/tmp/gamecontrollerdb.txt")
        # If ResidualVM extension, redirect to ResidualVM emulator
        if rom.endswith(".residualvm"):
            system.config['emulator'] = 'residualvm'
            return ResidualVMGenerator().generate(system, rom, playersControllers, demo, recalboxSettings)

        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            #system.config['configfile'] = recalboxFiles.mupenCustom
            pass
        
        # Find rom path
        if os.path.isdir(rom):
          # rom is a directory: must contains a <game name>.scummvm file
          romPath = rom
          romFile = glob.glob(romPath + "/*.scummvm")[0]
          romName = os.path.splitext(os.path.basename(romFile))[0]
        else:
          # rom is a file: split in directory and file name
          romPath = os.path.dirname(rom)
          romName = os.path.splitext(os.path.basename(rom))[0]
        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], 
                        "--fullscreen",
                        "--joystick=0", 
                        "--screenshotpath="+recalboxFiles.screenshotsDir, 
                        "--extrapath=/usr/share/scummvm",
                        "--savepath="+recalboxFiles.scummvmSaves,
                        "--path=""{}""".format(romPath)]
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append("""{}""".format(romName))

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so"})
