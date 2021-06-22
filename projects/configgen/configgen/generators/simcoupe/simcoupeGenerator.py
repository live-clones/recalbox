import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.generators.Generator import Generator
from configgen.settings.keyValueSettings import keyValueSettings


class SimCoupeGenerator(Generator):

    # return true if the option is considered defined
    @staticmethod
    def defined(key, dictio):
        return key in dictio and isinstance(dictio[key], str) and len(dictio[key]) > 0

    # return true if the option is considered enabled (for boolean options)
    @staticmethod
    def enabled(key, dictio):
        return key in dictio and (dictio[key] == '1' or dictio[key] == 'true')

    def generate(self, system, playersControllers, recalboxSettings, args):

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.simcoupeConfig)
        settings.loadFile(True)

        settings.setOption("FilterGUI", "1")                                    # Nice looking GUI
        settings.setOption("InPath", recalboxFiles.ROMS + "/samcoupe")          # default input path
        settings.setOption("OutPath", recalboxFiles.SAVES + "/samcoupe")        # default output path
        settings.setOption("Disk1", args.rom)                                   # rom
        settings.setOption("Fullscreen", "Yes")                                 # Fullscreen
        settings.setOption("FirstRun", "No")                                    # no first-launch nag-screen #1
        settings.setOption("CfgVersion", "4")                                   # no first launch nag-screen #2
        smooth = "Yes" if self.enabled('smooth', system.config) else "No"
        settings.setOption("Filter", smooth)                                    # no first launch nag-screen #2

        settings.setOption("scanlines", "no")
        if self.defined('shaderset', system.config) and system.config['shaderset'] == 'scanlines':
            settings.setOption("Scanlines", "Yes")

        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                settings.setOption("JoyDev1", controller.configName)
                settings.setOption("JoyHKey1", controller.inputs["hotkey"].id)
                settings.setOption("JoyStart1", controller.inputs["start"].id)
                settings.setOption("JoyA1", controller.inputs["a"].id)
                settings.setOption("JoyB1", controller.inputs["b"].id)
                settings.setOption("JoyX1", controller.inputs["x"].id)
                settings.setOption("JoyY1", controller.inputs["y"].id)
            if controller.player == "2":
                settings.setOption("JoyDev2", controller.configName)
                settings.setOption("JoyHKey2", controller.inputs["hotkey"].id)
                settings.setOption("JoyStart2", controller.inputs["start"].id)
                settings.setOption("JoyA2", controller.inputs["a"].id)
                settings.setOption("JoyB2", controller.inputs["b"].id)
                settings.setOption("JoyX2", controller.inputs["x"].id)
                settings.setOption("JoyY2", controller.inputs["y"].id)

        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']], args.rom]

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
