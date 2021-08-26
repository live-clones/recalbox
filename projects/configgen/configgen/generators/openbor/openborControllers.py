from configgen.controllersConfig import Controller, Input
from configgen.controllersConfig import ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings


class OpenborControllers:

    JOYSTICK_FIRST_VALUE = 600  # Keep in sync with openbor/engine/sdl/joystiks.c - #define	JOY_LIST_FIRST 600
    JOYSTICK_MAX_ITEM    = 64   # Keep in sync with openbor/engine/sdl/joystiks.c - #define JOY_MAX_INPUTS 64
    JOYSTICK_MAX         = 4    # Keep in sync with openbor/engine/sdl/joystiks.c - #define JOY_LIST_TOTAL 4

    # 4bits - bit to position
    HATBIT_TO_HATPOSITION =\
    {
        1: 0,
        2: 1,
        4: 2,
        8: 3,
    }

    # Openbor Pad event index to recalbox item
    OPENBOR_TO_RECALBOX =\
    {
        "MOVEUP"     : "up",
        "MOVEDOWN"   : "down",
        "MOVELEFT"   : "left",
        "MOVERIGHT"  : "right",
        "ATTACK"     : "a",
        "ATTACK2"    : "b",
        "ATTACK3"    : "x",
        "ATTACK4"    : "y",
        "JUMP"       : "l1",
        "SPECIAL"    : "r1",
        "START"      : "start",
        "SCREENSHOT" : "l2",
        "HOTKEY"     : "hotkey",
    }

    def __init__(self, controllers: ControllerDictionary):
        self.controllers: ControllerDictionary = controllers

    # Returns the value to write in retroarch config file, depending on the type
    @staticmethod
    def getConfigValue(controller: Controller, inp: Input):
        index = int(OpenborControllers.JOYSTICK_FIRST_VALUE) + 1 + (int(controller.index) * OpenborControllers.JOYSTICK_MAX_ITEM)
        iid = int(inp.id)
        if inp.type == 'button':
            return index + int(iid)
        if inp.type == 'axis':
            if inp.value == '-1':
                return index + int(controller.nbbuttons) + (iid * 2 + 0)
            else:
                return index + int(controller.nbbuttons) + (iid * 2 + 1)
        if inp.type == 'hat':
            return index + int(controller.nbbuttons) + int(controller.nbaxes) * 2 + \
                   iid * 4 + OpenborControllers.HATBIT_TO_HATPOSITION[int(inp.value)]
        if inp.type == 'key':
            return iid

        raise TypeError

    def addController(self, controller: Controller, settings: keyValueSettings):
        for o, r in self.OPENBOR_TO_RECALBOX.items():
            value = 0
            key = "JoystickButton-P{}-{}".format(int(controller.player) - 1, o)
            if r in controller.inputs:
                value = self.getConfigValue(controller, controller.inputs[r])
            settings.setOption(key, value)

    def addControllers(self, settings: keyValueSettings):
        for player in range(0,4):
            for index, controller in self.controllers.items():
                if int(controller.player) == player + 1:
                    self.addController(controller, settings)
