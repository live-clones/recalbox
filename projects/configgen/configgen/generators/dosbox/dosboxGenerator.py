#!/usr/bin/env python
import os.path
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class DosBoxGenerator(Generator):

    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:
        # Find rom path
        gameDir = args.rom
        batFile = gameDir + "/dosbox.bat"
        gameConfFile = gameDir + "/dosbox.cfg"

        commandArray = [recalboxFiles.recalboxBins[system.Emulator],
			"-userconf",
            "-fullscreen",
			"-exit", 
			"""{}""".format(batFile),
			"-c", """set ROOT={}""".format(gameDir),
			"-vkeybd", "/usr/share/dosbox"]

        if system.ShaderSet == 'scanlines':
            commandArray.append("-forcescaler")
            commandArray.append("scan3x")
        elif system.ShaderSet == 'retro':
            commandArray.append("-forcescaler")
            commandArray.append("rgb3x")

        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet and resolution.selfProcess:
            extraConf = "[sdl]\nfullscreen=true\nvsync=true\nfullresolution={}\n".format(resolution.string)
            extraConfPath = recalboxFiles.dosboxConfig + ".extra"
            with open(extraConfPath, "w") as f:
                f.write(extraConf)
            commandArray.append("-conf")
            commandArray.append(extraConfPath)

        commandArray.append("-conf")
        if os.path.isfile(gameConfFile):
            commandArray.append("""{}""".format(gameConfFile))
        else:
            commandArray.append("""{}""".format(recalboxFiles.dosboxConfig))

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so"})
