from typing import List, Dict

from configgen import recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.controllers.inputItem import InputItem
from configgen.generators.Generator import Generator
from configgen.Command import Command
from configgen.settings.keyValueSettings import keyValueSettings


class DuckstationGenerator(Generator):

    __ConfigurationFile: str = recalboxFiles.HOME + "/.config/duckstation/settings.ini"

    __HatToDirections: Dict[int, str] = \
    {
        1: "Up",
        2: "Right",
        4: "Down,",
        8: "Left"
    }

    def __BuildInput(self, inp: InputItem, signless: bool = False) -> str:
        if inp.IsButton: return "Button{}".format(inp.Id)
        if inp.IsAxis  : return "{}Axis{}".format(('' if signless else ('+' if int(inp.Value) >= 0 else '-')), inp.Id)
        if inp.IsHat   : return "Axis{} {}".format(inp.Id, self.__HatToDirections[int(inp.Value)])
        return "Unknown"

    def generate(self, system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings, args) -> Command:

        # Make save dirs
        import os
        memcards  : str = os.path.join(recalboxFiles.SAVES, "psx/duckstation/memcards")
        savestates: str = os.path.join(recalboxFiles.SAVES, "psx/duckstation/savestates")
        if not os.path.exists(memcards)  : os.makedirs(name=memcards  , exist_ok=True)
        if not os.path.exists(savestates): os.makedirs(name=savestates, exist_ok=True)

        # Config file
        from configgen.settings.iniSettings import IniSettings
        configFile = IniSettings(self.__ConfigurationFile, True)
        configFile.defineBool("true", "false") \
                  .loadFile(True)

        # Display
        from configgen.utils.architecture import Architecture
        arch: Architecture = Architecture()
        configFile.setBool("Display", "VSync", True)
        configFile.setInt("Display", "Rotate", 3 if arch.isGoa2 or arch.isGoa3 else 0)
        configFile.setBool("Display", "Fullscreen", True)
        configFile.setBool("Display", "IntegerScaling", system.IntegerScale)
        configFile.setBool("Display", "LinearFiltering", system.Smooth)
        configFile.setBool("Display", "ShowFPS", system.ShowFPS)

        # Gamelist - Delete recorded folder or the emulator may take a very long time to run
        configFile.clearRegion("GameList")

        # Log level
        configFile.setString("Logging", "LogLevel", "Info" if args.verbose else "Error")

        # Controllers
        for index in playersControllers:
            controller = playersControllers[index]
            cs: str = "Controller{}/".format(controller.PlayerIndex - 1)
            duckController: str = "Controller{}".format(controller.PlayerIndex)
            # Digital or analog?
            configFile.setString(duckController, "Type", "AnalogController" if controller.HasJoy1Left else "DigitalController")
            configFile.setBool(duckController, "AnalogDPadInDigitalMode", controller.HasJoy1Left)
            # Digital part
            if controller.HasUp      : configFile.setString(duckController, "ButtonUp"      , cs + self.__BuildInput(controller.Up))
            if controller.HasRight   : configFile.setString(duckController, "ButtonRight"   , cs + self.__BuildInput(controller.Right))
            if controller.HasDown    : configFile.setString(duckController, "ButtonDown"    , cs + self.__BuildInput(controller.Down))
            if controller.HasLeft    : configFile.setString(duckController, "ButtonLeft"    , cs + self.__BuildInput(controller.Left))
            if controller.HasSelect  : configFile.setString(duckController, "ButtonSelect"  , cs + self.__BuildInput(controller.Select))
            if controller.HasStart   : configFile.setString(duckController, "ButtonStart"   , cs + self.__BuildInput(controller.Start))
            if controller.HasB       : configFile.setString(duckController, "ButtonCross"   , cs + self.__BuildInput(controller.B))
            if controller.HasA       : configFile.setString(duckController, "ButtonCircle"  , cs + self.__BuildInput(controller.A))
            if controller.HasX       : configFile.setString(duckController, "ButtonTriangle", cs + self.__BuildInput(controller.X))
            if controller.HasY       : configFile.setString(duckController, "ButtonSquare"  , cs + self.__BuildInput(controller.Y))
            if controller.HasL1      : configFile.setString(duckController, "ButtonL1"      , cs + self.__BuildInput(controller.L1))
            if controller.HasR1      : configFile.setString(duckController, "ButtonR1"      , cs + self.__BuildInput(controller.R1))
            if controller.HasL2      : configFile.setString(duckController, "ButtonL2"      , cs + self.__BuildInput(controller.L2))
            if controller.HasR2      : configFile.setString(duckController, "ButtonR2"      , cs + self.__BuildInput(controller.R2))
            # Analog part
            if controller.HasJoy1Left: configFile.setString(duckController, "AxisLeftX"     , cs + self.__BuildInput(controller.Joy1Left, True))
            if controller.HasJoy1Up  : configFile.setString(duckController, "AxisLeftY"     , cs + self.__BuildInput(controller.Joy1Up  , True))
            if controller.HasJoy2Left: configFile.setString(duckController, "AxisRightX"    , cs + self.__BuildInput(controller.Joy2Left, True))
            if controller.HasJoy2Up  : configFile.setString(duckController, "AxisRightY"    , cs + self.__BuildInput(controller.Joy2Up  , True))
            # Hotkey for player one?
            if controller.PlayerIndex == 1:
                if controller.HasHotkey: configFile.setString(duckController, "ButtonHotkey"  , cs + self.__BuildInput(controller.Hotkey))

        # Save config
        configFile.saveFile()
        configFile.changeSettingsFile("/recalbox/share/system/duckstation.cfg")
        configFile.saveFile()

        # Command line arguments
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.Emulator]]

        # Add extra arguments
        if system.HasArgs: commandArray.extend(system.Args)

        # finally add rom
        commandArray.extend([args.rom])

        return Command(videomode=system.VideoMode, array=commandArray)
