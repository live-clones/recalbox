#!/usr/bin/env python
import Command
import mupenConfig
import mupenControllers
import recalboxFiles
from generators.Generator import Generator


class MupenGenerator(Generator):
    # Main entry of the module
    # Configure mupen and return a command
    def generate(self, system, playersControllers, recalboxSettings, args):
        # Settings recalbox default config file if no user defined one
        if not system.config['configfile']:
            # Using recalbox config file
            system.config['configfile'] = recalboxFiles.mupenCustom
            # Write configuration file
            mupenConfig.writeMupenConfig(system, playersControllers, args.rom)
            #  Write controllers configuration files
            mupenControllers.writeControllersConfig(playersControllers)

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], 
	    "--corelib", "/usr/lib/libmupen64plus.so.2.0.0",
	    "--gfx", "/usr/lib/mupen64plus/mupen64plus-video-{}.so".format(system.config['core']),
	    "--configdir", recalboxFiles.mupenConf,
	    "--datadir", recalboxFiles.mupenConf,
        "--sshortdir", recalboxFiles.SCREENSHOTS]

        # Screen resolution
        from utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            commandArray.extend(["--fullscreen", "--resolution", resolution.string])

        # Verbose
        if args.verbose:
            commandArray.append("--verbose")

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        commandArray.append(args.rom)

        return Command.Command(videomode=system.config['videomode'], array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so", "SDL_VIDEO_EGL_DRIVER":"/usr/lib/libGLESv2.so"})
