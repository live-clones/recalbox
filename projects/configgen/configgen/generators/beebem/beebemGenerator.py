import os

from configgen.controllers.inputItem import InputItem
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.generators.Generator import Generator, ControllerPerPlayer
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.Command import Command


class BeebemGenerator(Generator):

    __BEEBEM_CONFIGFILE = os.path.join(recalboxFiles.HOME, ".config/beebem/beebem.cfg")

    def __BuildITem(self, into: keyValueSettings, name: str, base: str, item: InputItem):
        # Type
        if item.IsButton: into.setString(base + "type." + name, "button")
        elif item.IsAxis: into.setString(base + "type." + name, "axis")
        elif item.IsHat : into.setString(base + "type." + name, "hat")
        # Id
        into.setInt(base + name, item.Id)

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        settings = keyValueSettings(self.__BEEBEM_CONFIGFILE, False)
        settings.defineBool('1', '0') \
                .loadFile(True)

        # Default sound on
        settings.setDefaultBool("sound.enable", True)
        settings.setDefaultBool("sound.chip.enable", True)
        settings.setDefaultBool("sound.relay.enable", True)
        settings.setDefaultBool("sound.tape.enable", True)
        settings.setDefaultBool("sound.disk.enable", True)
        settings.setDefaultBool("sound.partsamples", True)
        settings.setDefaultBool("sound.music5000.enabled", True)

        # Default RGB monitor
        settings.setDefaultString("display.monitortype", "RGB")

        # default enabled devices
        settings.setDefaultBool("devices.amxmouse.enable", True)
        settings.setDefaultBool("devices.swramboard.enable", True)

        # Fast tape by default
        settings.setDefaultBool("devices.tape.fast", True)

        # Disconnect Tube - use override to activate
        settings.setString("machine.tube.type", "none")

        # Rendering
        settings.setBool("display.fullscreen", True)
        settings.setBool("display.leds.show", True)
        settings.setBool("display.smooth", system.Smooth)
        settings.setBool("display.showfps", system.ShowFPS)

        # Disk protection on by default
        settings.setBool("disk.writeprotected", True)

        # Default devices - Enable it using overrides
        settings.setBool("devices.scsidrive.enable", False)
        settings.setBool("devices.idedrive.enable", False)
        settings.setBool("devices.printer.enable", False)

        # Try identify machine folder
        defaultMachine: str = "B" # BBC Model B by default
        from typing import Dict
        machines: Dict[str, str] = \
        {
            "bbcbplus": "BPlus",
            "bbc-bplus": "BPlus",
            "modelbplus": "BPlus",
            "model-bplus": "BPlus",
            "bbcmodelbplus": "BPlus",
            "bbc-model-bplus": "BPlus",
            "bbcmicrointegrab": "IntegraB",
            "bbcmicro-integra-b": "IntegraB",
            "integrab": "IntegraB",
            "integra-b": "IntegraB",
            "bbcmaster": "Master128",
            "bbc-master": "Master128",
            "bbcmaster128": "Master128",
            "bbc-master-128": "Master128",
            "master128": "Master128",
            "master-128": "Master128",
        }
        lrom: str = args.rom.lower()
        for key in machines:
            if key in lrom:
                defaultMachine = machines[key]
                break
        settings.setString("machine.model", defaultMachine)

        # controller settings
        for player, controller in playersControllers.items():
            base: str = "joystick.{}.".format(player - 1)
            settings.setInt(base + "sdlindex", controller.SdlIndex)
            if controller.HasJoy1Left: settings.setInt(base + "xaxis", controller.Joy1Left.Id)
            if controller.HasJoy1Up  : settings.setInt(base + "yaxis", controller.Joy1Up.Id)
            if controller.HasHotkey  : self.__BuildITem(settings, "hotkey", base, controller.Hotkey)
            if controller.HasSelect  : self.__BuildITem(settings, "select", base, controller.Select)
            if controller.HasStart   : self.__BuildITem(settings, "start" , base, controller.Start)
            if controller.HasA       : self.__BuildITem(settings, "a"     , base, controller.A)
            if controller.HasB       : self.__BuildITem(settings, "b"     , base, controller.B)
            if controller.HasX       : self.__BuildITem(settings, "x"     , base, controller.X)
            if controller.HasY       : self.__BuildITem(settings, "y"     , base, controller.Y)
            if controller.HasL1      : self.__BuildITem(settings, "l1"    , base, controller.L1)
            if controller.HasR1      : self.__BuildITem(settings, "r1"    , base, controller.R1)
            if controller.HasL2      : self.__BuildITem(settings, "l2"    , base, controller.L2)
            if controller.HasR2      : self.__BuildITem(settings, "r2"    , base, controller.R2)
            if controller.HasL3      : self.__BuildITem(settings, "l3"    , base, controller.L3)
            if controller.HasR3      : self.__BuildITem(settings, "r3"    , base, controller.R3)

        settings.saveFile()

        commandArray = [recalboxFiles.recalboxBins[system.Emulator], args.rom]
        if system.HasArgs: commandArray.extend(system.Args)

        return Command(videomode=system.VideoMode, array=commandArray)
