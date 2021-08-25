#!/usr/bin/env python
import os.path
import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from settings.keyValueSettings import keyValueSettings


class DosBoxGenerator(Generator):

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # Main entry of the module
    # Return command
    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args):
        # Find rom path
        gameDir = args.rom
        batFile = gameDir + "/dosbox.bat"
        gameConfFile = gameDir + "/dosbox.cfg"

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']],
			"-userconf",
            "-fullscreen",
			"-exit", 
			"""{}""".format(batFile),
			"-c", """set ROOT={}""".format(gameDir),
			"-vkeybd", "/usr/share/dosbox"]

        if self.defined('shaderset', system.config):
            if system.config['shaderset'] == 'scanlines':
                commandArray.append("-forcescaler")
                commandArray.append("scan3x")
            elif system.config['shaderset'] == 'retro':
                commandArray.append("-forcescaler")
                commandArray.append("rgb3x")

        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(system.config['videomode'])
        if resolution.isSet and resolution.selfProcess:
            extraConf = "[sdl]\nfullscreen=true\nvsync=true\nfullresolution={}\n".format(resolution.string)
            extraConfPath = recalboxFiles.dosboxConfig + ".extra"
            with open(extraConfPath, "w") as f:
                f.write(extraConf)
            commandArray.append("-conf")
            commandArray.append(extraConfPath)

        if os.path.isfile(gameConfFile):
            commandArray.append("-conf")
            commandArray.append("""{}""".format(gameConfFile))
        else:
            commandArray.append("-conf")
            commandArray.append("""{}""".format(recalboxFiles.dosboxConfig))
        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])
        return Command.Command(videomode='default', array=commandArray, env={"SDL_VIDEO_GL_DRIVER":"/usr/lib/libGLESv2.so"})
