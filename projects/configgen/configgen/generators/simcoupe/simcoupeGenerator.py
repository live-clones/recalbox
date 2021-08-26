import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings


class SimCoupeGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launhes
        """

        settings = keyValueSettings(recalboxFiles.simcoupeConfig)
        settings.loadFile(True)

        settings.setOption("filtergui", "1")                                    # Nice looking GUI
        settings.setOption("inpath", recalboxFiles.ROMS + "/samcoupe")          # default input path
        settings.setOption("outpath", recalboxFiles.SAVES + "/samcoupe")        # default output path
        settings.setOption("disk1", args.rom)                                   # rom
        settings.setOption("fullscreen", "yes")                                 # Fullscreen
        settings.setOption("firstrun", "no")                                    # no first-launch nag-screen #1
        settings.setOption("cfgversion", "4")                                   # no first launch nag-screen #2
        smooth = "yes" if system.Smooth else "no"
        settings.setOption("filter", smooth)                                    # no first launch nag-screen #2

        settings.setOption("scanlines", "no")
        if system.ShaderSet == 'scanlines':
            settings.setOption("scanlines", "yes")

        for index in playersControllers:
            controller = playersControllers[index]
            if controller.player == "1":
                settings.setOption("joydev1", controller.configName)
                settings.setOption("joyhkey1", controller.inputs["hotkey"].id)
                settings.setOption("joystart1", controller.inputs["start"].id)
                settings.setOption("joya1", controller.inputs["a"].id)
                settings.setOption("joyb1", controller.inputs["b"].id)
                settings.setOption("joyx1", controller.inputs["x"].id)
                settings.setOption("joyy1", controller.inputs["y"].id)
            if controller.player == "2":
                settings.setOption("joydev2", controller.configName)
                settings.setOption("joyhkey2", controller.inputs["hotkey"].id)
                settings.setOption("joystart2", controller.inputs["start"].id)
                settings.setOption("joya2", controller.inputs["a"].id)
                settings.setOption("joyb2", controller.inputs["b"].id)
                settings.setOption("joyx2", controller.inputs["x"].id)
                settings.setOption("joyy2", controller.inputs["y"].id)

        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
