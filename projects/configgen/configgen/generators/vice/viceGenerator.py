#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class ViceGenerator(Generator):
    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        import configgen.recalboxFiles as recalboxFiles
        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "-config", recalboxFiles.viceConfig,
                        "-autostart", args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so"})
