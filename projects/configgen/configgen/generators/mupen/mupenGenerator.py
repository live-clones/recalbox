#!/usr/bin/env python
from configgen.Command import Command
from configgen.Emulator import Emulator
import configgen.recalboxFiles as recalboxFiles
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.iniSettings import IniSettings
from configgen.settings.keyValueSettings import keyValueSettings


class MupenGenerator(Generator):
    # Main entry of the module
    # Configure mupen and return a command
    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Settings recalbox default config file if no user defined one
        if not system.HasConfigFile:
            # Load config
            mupenSettings = IniSettings(recalboxFiles.mupenCustom, True)
            mupenSettings.loadFile(True) \
                         .defineBool('True', 'False')
            # Write configuration file
            from configgen.generators.mupen import mupenConfig
            mupenConfig.writeMupenConfig(system, playersControllers, args.rom, mupenSettings)
            # Write controllers configuration files
            from configgen.generators.mupen import mupenControllers
            mupenControllers.writeControllersConfig(playersControllers)
            # Save config
            mupenSettings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
                        "--corelib", "/usr/lib/libmupen64plus.so.2.0.0",
                        "--gfx", "/usr/lib/mupen64plus/mupen64plus-video-{}.so".format(system.Core),
                        "--configdir", recalboxFiles.mupenConf,
                        "--datadir", recalboxFiles.mupenConf,
                        "--sshotdir", recalboxFiles.SCREENSHOTS]

        # Screen resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            commandArray.extend(["--fullscreen", "--resolution", resolution.string])

        # Verbose
        if args.verbose:
            commandArray.append("--verbose")

        if system.HasArgs: commandArray.extend(system.Args)

        commandArray.append(args.rom)

        return Command(videomode=system.VideoMode,
                               array=commandArray,
                               env={"SDL_VIDEO_GL_DRIVER": "/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER": "/usr/lib/libEGL.so"})
