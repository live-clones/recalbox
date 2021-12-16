from typing import List, Dict

from configgen import recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import ControllerPerPlayer
from configgen.controllers.inputItem import InputItem
from configgen.generators.Generator import Generator
from configgen.Command import Command
from configgen.settings.keyValueSettings import keyValueSettings


class DuckstationGenerator(Generator):

    SETTINGS_FILE:str = recalboxFiles.HOME + "/.config/duckstation/settings.ini"
    DATABASE_FILE:str = recalboxFiles.HOME + "/.config/duckstation/gamecontrollerdb.txt"

    SDL_CONTROLLER_BUTTON_A: int = 0
    SDL_CONTROLLER_BUTTON_B: int = 1
    SDL_CONTROLLER_BUTTON_X: int = 2
    SDL_CONTROLLER_BUTTON_Y: int = 3
    SDL_CONTROLLER_BUTTON_BACK: int = 4
    SDL_CONTROLLER_BUTTON_GUIDE: int = 5
    SDL_CONTROLLER_BUTTON_START: int = 6
    SDL_CONTROLLER_BUTTON_LEFTSTICK: int = 7
    SDL_CONTROLLER_BUTTON_RIGHTSTICK: int = 8
    SDL_CONTROLLER_BUTTON_LEFTSHOULDER: int = 9
    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: int = 10
    SDL_CONTROLLER_BUTTON_DPAD_UP: int = 11
    SDL_CONTROLLER_BUTTON_DPAD_DOWN: int = 12
    SDL_CONTROLLER_BUTTON_DPAD_LEFT: int = 13
    SDL_CONTROLLER_BUTTON_DPAD_RIGHT: int = 14

    SDL_CONTROLLER_AXIS_LEFTX: int = 0
    SDL_CONTROLLER_AXIS_LEFTY: int = 1
    SDL_CONTROLLER_AXIS_RIGHTX: int = 2
    SDL_CONTROLLER_AXIS_RIGHTY: int = 3
    SDL_CONTROLLER_AXIS_TRIGGERLEFT: int = 4
    SDL_CONTROLLER_AXIS_TRIGGERRIGHT: int = 5

    __HatToDirections: Dict[int, str] = \
    {
        1: "Up",
        2: "Right",
        4: "Down",
        8: "Left"
    }

    @staticmethod
    def __BuildAxis(inp: int, sign: int = 0) -> str:
        return "{}Axis{}".format(('+' if sign > 0 else ('-' if sign < 0 else '')), inp)

    @staticmethod
    def __BuildButton(inp: int) -> str:
        return "Button{}".format(inp)

    def __BuildInput(self, inp: InputItem, signless: bool = False) -> str:
        if inp.IsButton: return "Button{}".format(inp.Id)
        if inp.IsAxis  : return "{}Axis{}".format(('' if signless else ('+' if int(inp.Value) >= 0 else '-')), inp.Id)
        if inp.IsHat   : return "Hat{} {}".format(inp.Id, self.__HatToDirections[int(inp.Value)])
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
        configFile = IniSettings(self.SETTINGS_FILE, True)
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
        for index in range(8):
            duckController: str = "Controller{}".format(index+1) # duckstation pad index
            if index+1 in playersControllers:
              controller = playersControllers[index+1]
              cs: str = "Controller{}/".format(controller.SdlIndex)                   # physical (sdl) pad index
              # Digital or analog?
              configFile.setString(duckController, "Type", "AnalogController" if controller.HasJoy1Left else "DigitalController")
              configFile.setBool(duckController, "AnalogDPadInDigitalMode", controller.HasJoy1Left)
              # Digital part
              if controller.HasUp      : configFile.setString(duckController, "ButtonUp"      , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_DPAD_UP))
              if controller.HasRight   : configFile.setString(duckController, "ButtonRight"   , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
              if controller.HasDown    : configFile.setString(duckController, "ButtonDown"    , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_DPAD_DOWN))
              if controller.HasLeft    : configFile.setString(duckController, "ButtonLeft"    , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_DPAD_LEFT))
              if controller.HasSelect  : configFile.setString(duckController, "ButtonSelect"  , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_BACK))
              if controller.HasStart   : configFile.setString(duckController, "ButtonStart"   , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_START))
              if controller.HasB       : configFile.setString(duckController, "ButtonCross"   , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_A))
              if controller.HasA       : configFile.setString(duckController, "ButtonCircle"  , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_B))
              if controller.HasX       : configFile.setString(duckController, "ButtonTriangle", cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_Y))
              if controller.HasY       : configFile.setString(duckController, "ButtonSquare"  , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_X))
              if controller.HasL1      : configFile.setString(duckController, "ButtonL1"      , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
              if controller.HasR1      : configFile.setString(duckController, "ButtonR1"      , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
              if controller.HasL2      : configFile.setString(duckController, "ButtonL2"      , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_TRIGGERLEFT, 1))
              if controller.HasR2      : configFile.setString(duckController, "ButtonR2"      , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 1))
              # Analog part
              if controller.HasJoy1Left: configFile.setString(duckController, "AxisLeftX"     , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_LEFTX, 0))
              if controller.HasJoy1Up  : configFile.setString(duckController, "AxisLeftY"     , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_LEFTY, 0))
              if controller.HasJoy2Left: configFile.setString(duckController, "AxisRightX"    , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_RIGHTX, 0))
              if controller.HasJoy2Up  : configFile.setString(duckController, "AxisRightY"    , cs + self.__BuildAxis(self.SDL_CONTROLLER_AXIS_RIGHTY, 0))
              # Hotkey for player one?
              if controller.PlayerIndex == 1:
                  if controller.HasHotkey: configFile.setString(duckController, "ButtonHotkey"  , cs + self.__BuildButton(self.SDL_CONTROLLER_BUTTON_GUIDE))
            else:
              configFile.setString(duckController, "Type", "None")

        # Save config
        configFile.saveFile()

        # Generale game controller database
        with open(self.DATABASE_FILE, "w") as f:
            for controller in playersControllers.values():
                f.write(controller.generateSDLGameDBLine() + '\n')

        # Command line arguments
        commandArray: List[str] = [recalboxFiles.recalboxBins[system.Emulator]]

        # Add extra arguments
        if system.HasArgs: commandArray.extend(system.Args)

        # finally add rom
        commandArray.extend([args.rom])

        return Command(videomode=system.VideoMode, array=commandArray)
