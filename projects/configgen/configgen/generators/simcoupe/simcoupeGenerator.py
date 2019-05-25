import Command
from generators.Generator import Generator
from settings.keyValueSettings import keyValueSettings
import recalboxFiles


class SimCoupeGenerator(Generator):

    def generate(self, system, rom, playersControllers, demo, recalboxSettings):

        '''
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        '''

        settings = keyValueSettings('/recalbox/share/system/.simcoupe/SimCoupe.cfg')
        settings.loadFile(True)

        settings.setOption("FilterGui", "1")                              # Nice looking GUI
        settings.setOption("InPath", recalboxFiles.ROMS + "/samcoupe")    # default input path
        settings.setOption("OutPath", recalboxFiles.SAVES + "/samcoupe")  # default output path
        settings.setOption("Disk1", rom)                                  # rom
        settings.setOption("Fullscreen", "Yes")                           # Fullscreen
        settings.setOption("FirstRun", "No")                              # no first-launch nag-screen #1
        settings.setOption("CfgVersion", "4")                             # no first launch nag-screen #2

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

        commandArray = [recalboxFiles.recalboxBins[system.config['emulator']]]

        if 'args' in system.config and system.config['args'] is not None:
            commandArray.extend(system.config['args'])

        return Command.Command(videomode=system.config['videomode'], array=commandArray)
