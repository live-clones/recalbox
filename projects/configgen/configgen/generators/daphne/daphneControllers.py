#!/usr/bin/env python
from typing import Dict, Optional, Tuple

import configgen.recalboxFiles as recalboxFiles
from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import ControllerPerPlayer

keyboard_keys: Dict[str, str] =\
{
    "KEY_UP":         "1073741906 0",
    "KEY_DOWN":       "1073741905 0",
    "KEY_LEFT":       "1073741904 0",
    "KEY_RIGHT":      "1073741903 0",
    "KEY_BUTTON1":    "1073742048 0",
    "KEY_BUTTON2":    "1073742050 0",
    "KEY_BUTTON3":    "32 0",
    "KEY_START1":     "49 13",
    "KEY_START2":     "50 0",
    "KEY_COIN1":      "53 27",
    "KEY_COIN2":      "54 53",
    "KEY_SKILL1":     "1073742049 0",
    "KEY_SKILL2":     "122 0",
    "KEY_SKILL3":     "120 0",
    "KEY_SERVICE":    "57 0",
    "KEY_TEST":       "1073741883 0",
    "KEY_RESET":      "48 0",
    "KEY_SCREENSHOT": "1073741893 0",
    "KEY_QUIT":       "1073742051 0",
    "KEY_PAUSE":      "112 0",
    "KEY_CONSOLE":    "92 0",
    "KEY_TILT":       "116 0"
}
joystick_keys: Dict[str, Optional[int]] = \
{
    "KEY_UP":         InputItem.ItemUp,
    "KEY_DOWN":       InputItem.ItemDown,
    "KEY_LEFT":       InputItem.ItemLeft,
    "KEY_RIGHT":      InputItem.ItemRight,
    "KEY_BUTTON1":    InputItem.ItemB,
    "KEY_BUTTON2":    InputItem.ItemA,
    "KEY_BUTTON3":    InputItem.ItemY,
    "KEY_START1":     InputItem.ItemStart,
    "KEY_START2":     None,
    "KEY_COIN1":      InputItem.ItemSelect,
    "KEY_COIN2":      None,
    "KEY_SKILL1":     None,
    "KEY_SKILL2":     None,
    "KEY_SKILL3":     None,
    "KEY_SERVICE":    None,
    "KEY_TEST":       None,
    "KEY_RESET":      None,
    "KEY_SCREENSHOT": None,
    "KEY_QUIT":       InputItem.ItemHotkey,
    "KEY_PAUSE":      None,
    "KEY_CONSOLE":    None,
    "KEY_TILT":       None
}
joystick_axis: Dict[str, Optional[Tuple[int]]] = {
    "KEY_UP":         ( InputItem.ItemJoy1Up   , InputItem.ItemUp    ),
    "KEY_DOWN":       ( InputItem.ItemJoy1Down , InputItem.ItemDown  ),
    "KEY_LEFT":       ( InputItem.ItemJoy1Left , InputItem.ItemLeft  ),
    "KEY_RIGHT":      ( InputItem.ItemJoy1Right, InputItem.ItemRight ),
    "KEY_BUTTON1":    None,
    "KEY_BUTTON2":    None,
    "KEY_BUTTON3":    None,
    "KEY_START1":     None,
    "KEY_START2":     None,
    "KEY_COIN1":      None,
    "KEY_COIN2":      None,
    "KEY_SKILL1":     None,
    "KEY_SKILL2":     None,
    "KEY_SKILL3":     None,
    "KEY_SERVICE":    None,
    "KEY_TEST":       None,
    "KEY_RESET":      None,
    "KEY_SCREENSHOT": None,
    "KEY_QUIT":       None,
    "KEY_PAUSE":      None,
    "KEY_CONSOLE":    None,
    "KEY_TILT":       None
}

# Create the controller configuration file
def generateControllerConfig(_, controllers: ControllerPerPlayer):
    from configgen.settings.iniSettings import IniSettings
    config = IniSettings(recalboxFiles.daphneInputIni, True)

    # Format is KEY=keyboard1 keyboard2 joystick_btn joystick_axis
    # keyboard1 and keyboard2 are hardcoded values (see above)
    # joystick_btn is computed from controller configuration: hundreds=joystick index, units=button index (+1)
    # joystick_axis too: hundreds=joystick index, units=axis index (+1), sign=direction
    section = 'KEYBOARD'

    for index in controllers:
        controller = controllers[index]
        # we only care about player 1
        if controller.PlayerIndex != 1:
            continue

        # dirty hack: if quit is same button than another key, force r1 instead
        quitItem = joystick_keys["KEY_QUIT"]
        inputQuit: Optional[InputItem] = controller.Input(quitItem) if controller.HasInput(quitItem) else None
        if inputQuit is None: joystick_keys["KEY_QUIT"] = InputItem.ItemR1
        else:
            for key, item in joystick_keys.items():
                if key != "KEY_QUIT" and item is not None:
                    inputItem = controller.Input(item)
                    if inputItem.Type == inputQuit.Type and inputItem.Id == inputQuit.Id:
                        joystick_keys["KEY_QUIT"] = InputItem.ItemR1
                        break

        for propertyName, keyboardValue in keyboard_keys.items():
            # Map buttons
            joystickButtonValue: int = 0
            item: Optional[int] = joystick_keys[propertyName]
            if item is not None:
                if controller.HasInput(item):
                    inputItem = controller.Input(item)
                    if inputItem.IsButton:
                        joystickButtonValue = (index - 1) * 100 + inputItem.Id + 1

            # Map axis
            joystickAxisValue = 0
            if joystick_axis[propertyName] is not None:
                joystickAxisNames = joystick_axis[propertyName]
                for joystickAxisName in joystickAxisNames:
                    if controller.HasInput(joystickAxisName):
                        inputItem: InputItem = controller.Input(joystickAxisName)
                        if inputItem.IsAxis:
                            joystickAxisValue = ((index - 1) * 100 + inputItem.Id + 1) * inputItem.Value
                            break

            config.setString(section, propertyName, keyboardValue+" "+str(joystickButtonValue)+" "+str(joystickAxisValue))

    config.saveFile()
