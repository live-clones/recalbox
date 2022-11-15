import os

from configgen.controllers.inputItem import InputItem
from configgen.settings import configOverriding
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.Command import Command
from configgen.utils.resolutions import ResolutionParser


class RB5000Generator(Generator):

    __RB5000_CONFIGFILE = os.path.join(recalboxFiles.HOME, ".config/rb5000/rb5000.cfg")
    __RB5000_PRINTERFILE = os.path.join(recalboxFiles.SAVES, "vg5000/printer.txt")

    def __BuildITem(self, into: keyValueSettings, name: str, base: str, item: InputItem):
        if item.IsButton:
            into.setInt(base + name, item.Id)

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        settings = keyValueSettings(self.__RB5000_CONFIGFILE, False)
        settings.defineBool('1', '0') \
                .loadFile(True)

        # Printer
        settings.setString("printer.filename", self.__RB5000_PRINTERFILE)

        # Tape
        settings.setBool("tape.autorun", True)
        settings.setBool("tape.writable", False)

        # Graphics
        resolution = ResolutionParser(system.VideoMode)
        if resolution.isSet:
            settings.setInt("renderer.width", resolution.width)
            settings.setInt("renderer.height", resolution.height)
        settings.setInt("renderer.smooth", system.Smooth)
        settings.setInt("renderer.integerscale", system.IntegerScale)

        # controller settings
        for player, controller in playersControllers.items():
            if 1 <= player <= 2:
                base: str = "joystick.{}.".format("left" if player == 1 else "right")
                settings.setInt(base + "index", controller.SdlIndex)
                if player == 1:
                    if controller.HasHotkey: self.__BuildITem(settings, "hotkey", base, controller.Hotkey)
                    if controller.HasStart : self.__BuildITem(settings, "start" , base, controller.Start)
                if controller.HasA       : self.__BuildITem(settings, "button1", base, controller.A)
                if controller.HasB       : self.__BuildITem(settings, "button2", base, controller.B)
                if controller.HasUp   : self.__BuildITem(settings, "up"   , base, controller.Up)
                if controller.HasDown : self.__BuildITem(settings, "down" , base, controller.Down)
                if controller.HasLeft : self.__BuildITem(settings, "left" , base, controller.Left)
                if controller.HasRight: self.__BuildITem(settings, "right", base, controller.Right)

        # Override chain
        overrides = configOverriding.buildOverrideChain(args.rom, ".vg5000.cfg")
        for override in overrides:
            settings.changeSettingsFile(override)
            settings.loadFile(False)

        os.makedirs(os.path.dirname(self.__RB5000_CONFIGFILE), exist_ok=True)
        settings.changeSettingsFile(self.__RB5000_CONFIGFILE)
        settings.saveFile()

        commandArray = [
                        recalboxFiles.recalboxBins[system.Emulator],  # Emulator
                        "-c", self.__RB5000_CONFIGFILE,
                        "-f",                                         # Forced fullscreen
                        "-t", args.rom                                # Tape file
                        ]
        if system.HasArgs:
            commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
