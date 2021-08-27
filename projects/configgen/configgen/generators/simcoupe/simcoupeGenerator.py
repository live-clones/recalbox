import configgen.Command as Command
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.generators.Generator import Generator, ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class SimCoupeGenerator(Generator):

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxSettings: keyValueSettings, args) -> Command:

        """
        Load, override keys and save back emulator's configuration file
        This way, any modification is kept accross emulator launches
        """

        settings = keyValueSettings(recalboxFiles.simcoupeConfig)
        settings.loadFile(True)

        settings.setInt("filtergui", 1)                                  # Nice looking GUI
        settings.setString("inpath", recalboxFiles.ROMS + "/samcoupe")   # default input path
        settings.setString("outpath", recalboxFiles.SAVES + "/samcoupe") # default output path
        settings.setString("disk1", args.rom)                            # rom
        settings.setString("fullscreen", "Yes")                          # Fullscreen
        settings.setString("firstrun", "No")                             # no first-launch nag-screen #1
        settings.setInt("cfgversion", 4)                                 # no first launch nag-screen #2
        smooth = "yes" if system.Smooth else "no"
        settings.setString("filter", smooth)                             # Bilinear filtering

        settings.setString("scanlines", "no")
        if system.ShaderSet == 'scanlines':
            settings.setString("scanlines", "yes")

        for index in playersControllers:
            controller = playersControllers[index]
            if controller.PlayerIndex == 1:
                settings.setString("joydev1", controller.DeviceName)
                if controller.HasHotkey: settings.setInt("joyhkey1",  controller.Hotkey.Id)
                if controller.HasStart:  settings.setInt("joystart1", controller.Start.Id)
                if controller.HasA:      settings.setInt("joya1",     controller.A.Id)
                if controller.HasB:      settings.setInt("joyb1",     controller.B.Id)
                if controller.HasX:      settings.setInt("joyx1",     controller.X.Id)
                if controller.HasY:      settings.setInt("joyy1",     controller.Y.Id)
            if controller.PlayerIndex == 2:
                settings.setString("joydev2", controller.DeviceName)
                if controller.HasHotkey: settings.setInt("joyhkey2",  controller.Hotkey.Id)
                if controller.HasStart:  settings.setInt("joystart2", controller.Start.Id)
                if controller.HasA:      settings.setInt("joya2",     controller.A.Id)
                if controller.HasB:      settings.setInt("joyb2",     controller.B.Id)
                if controller.HasX:      settings.setInt("joyx2",     controller.X.Id)
                if controller.HasY:      settings.setInt("joyy2",     controller.Y.Id)

        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]

        if system.HasArgs: commandArray.extend(system.Args)

        return Command.Command(videomode=system.VideoMode, array=commandArray)
