from typing import Dict

from configgen.controllers.controller import Controller, InputItem
from configgen.controllers.controller import ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings


class OpenborControllers:

    JOYSTICK_FIRST_VALUE = 600  # Keep in sync with openbor/engine/sdl/joystiks.c - #define	JOY_LIST_FIRST 600
    JOYSTICK_MAX_ITEM    = 64   # Keep in sync with openbor/engine/sdl/joystiks.c - #define JOY_MAX_INPUTS 64
    JOYSTICK_MAX         = 4    # Keep in sync with openbor/engine/sdl/joystiks.c - #define JOY_LIST_TOTAL 4

    # 4bits - bit to position
    HATBIT_TO_HATPOSITION: Dict[int, int] =\
    {
        1: 0,
        2: 1,
        4: 2,
        8: 3,
    }

    # Openbor Pad event index to recalbox item
    OPENBOR_TO_RECALBOX: Dict[str, int] =\
    {
        "MOVEUP"     : InputItem.ItemUp,
        "MOVEDOWN"   : InputItem.ItemDown,
        "MOVELEFT"   : InputItem.ItemLeft,
        "MOVERIGHT"  : InputItem.ItemRight,
        "ATTACK"     : InputItem.ItemA,
        "ATTACK2"    : InputItem.ItemB,
        "ATTACK3"    : InputItem.ItemX,
        "ATTACK4"    : InputItem.ItemY,
        "JUMP"       : InputItem.ItemL1,
        "SPECIAL"    : InputItem.ItemR1,
        "START"      : InputItem.ItemStart,
        "SCREENSHOT" : InputItem.ItemL2,
        "HOTKEY"     : InputItem.ItemHotkey,
    }

    def __init__(self, controllers: ControllerPerPlayer):
        self.controllers: ControllerPerPlayer = controllers

    # Returns the value to write in retroarch config file, depending on the type
    @staticmethod
    def getConfigValue(controller: Controller, item: InputItem):
        index = int(OpenborControllers.JOYSTICK_FIRST_VALUE) + 1 + controller.SdlIndex * OpenborControllers.JOYSTICK_MAX_ITEM
        if item.IsButton:       return index + item.Id
        if item.IsAxis:
            if item.Value < -1: return index + controller.ButtonCount + (item.Id * 2 + 0)
            else:               return index + controller.ButtonCount + (item.Id * 2 + 1)
        if item.IsHat:          return index + controller.ButtonCount + controller.AxisCount * 2 + \
                                       item.Id * 4 + OpenborControllers.HATBIT_TO_HATPOSITION[item.Value]
        if item.IsKey:          return item.Id

        raise TypeError

    def addController(self, controller: Controller, settings: keyValueSettings):
        for targetValue, item in self.OPENBOR_TO_RECALBOX.items():
            key = "JoystickButton-P{}-{}".format(controller.PlayerIndex - 1, targetValue)
            value = self.getConfigValue(controller, controller.Input(item)) if item in controller.AvailableInput else 0
            settings.setInt(key, value)

    def addControllers(self, settings: keyValueSettings):
        for player in range(0,4):
            for index, controller in self.controllers.items():
                if controller.PlayerIndex == player + 1:
                    self.addController(controller, settings)
