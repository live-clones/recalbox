#!/usr/bin/env python
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.generators.ppsspp.ppssppConfig as ppssppConfig
import configgen.generators.ppsspp.ppssppControllers as ppssppControllers
from configgen.settings.keyValueSettings import keyValueSettings


class PPSSPPGenerator(Generator):
    # Main entry of the module
    # Configure ppsspp and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:

        if not system.HasConfigFile:
            ppssppConfig.writePPSSPPConfig(system)
            # For each pad detected
            for index in playersControllers:
                controller = playersControllers[index]
                # we only care about player 1
                if controller.PlayerIndex != 1:
                    continue
                ppssppControllers.generateControllerConfig(controller)
                cfgFile = open(recalboxFiles.ppssppControls, "w")
                cfgFile.write(controller.generateSDLGameDBLine())
                cfgFile.close()
                break

        # the command to run
        commandArray = [recalboxFiles.recalboxBins[system.Emulator]]

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)
        # The next line is a reminder on how to quit PPSSPP with just the HK
        #commandArray = [recalboxFiles.recalboxBins[system.Emulator], rom, "--escape-exit"]
        return Command.Command(videomode=system.VideoMode, array=commandArray, env={"XDG_CONFIG_HOME":recalboxFiles.ppssppSaves, "SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so", "PPSSPP_GAME_CONTROLLER_DB_PATH": recalboxFiles.ppssppControls}, delay=1)
