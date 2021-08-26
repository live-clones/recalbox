#!/usr/bin/env python
# -*- coding: utf-8 -*-

from configparser import ConfigParser
import configgen.recalboxFiles as recalboxFiles
from configgen.controllersConfig import ControllerDictionary

keyboard_keys = {
    "KEY_UP":         "1073741906 0",
    "KEY_DOWN":       "1073741905 0",
    "KEY_LEFT":       "1073741904 0",
    "KEY_RIGHT":      "1073741903 0",
    "KEY_BUTTON1":    "1073742048 0",
    "KEY_BUTTON2":    "1073742050 0",
    "KEY_BUTTON3":    "32 0",
    "KEY_START1":     "49 0",
    "KEY_START2":     "50 0",
    "KEY_COIN1":      "53 54",
    "KEY_COIN2":      "54 53",
    "KEY_SKILL1":     "1073742049 0",
    "KEY_SKILL2":     "122 0",
    "KEY_SKILL3":     "120 0",
    "KEY_SERVICE":    "57 0",
    "KEY_TEST":       "1073741883 0",
    "KEY_RESET":      "48 0",
    "KEY_SCREENSHOT": "1073741893 0",
    "KEY_QUIT":       "27 0",
    "KEY_PAUSE":      "112 0",
    "KEY_CONSOLE":    "92 0",
    "KEY_TILT":       "116 0"
}
joystick_keys = {
    "KEY_UP":         "up",
    "KEY_DOWN":       "down",
    "KEY_LEFT":       "left",
    "KEY_RIGHT":      "right",
    "KEY_BUTTON1":    "b",
    "KEY_BUTTON2":    "a",
    "KEY_BUTTON3":    "y",
    "KEY_START1":     "start",
    "KEY_START2":     None,
    "KEY_COIN1":      "select",
    "KEY_COIN2":      None,
    "KEY_SKILL1":     None,
    "KEY_SKILL2":     None,
    "KEY_SKILL3":     None,
    "KEY_SERVICE":    None,
    "KEY_TEST":       None,
    "KEY_RESET":      None,
    "KEY_SCREENSHOT": None,
    "KEY_QUIT":       "hotkey",
    "KEY_PAUSE":      None,
    "KEY_CONSOLE":    None,
    "KEY_TILT":       None
}
joystick_axis = {
    "KEY_UP":         ["joystick1up","up"],
    "KEY_DOWN":       ["-joystick1up","down"],
    "KEY_LEFT":       ["joystick1left","left"],
    "KEY_RIGHT":      ["-joystick1left","right"],
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
def generateControllerConfig(_, controllers: ControllerDictionary):
    Config = ConfigParser()
    # To prevent ConfigParser from converting to lower case
    Config.optionxform = str

    # Format is KEY=keyboard1 keyboard2 joystick_btn joystick_axis
    # keyboard1 and keyboard2 are hardcoded values (see above)
    # joystick_btn is computed from controller configuration: hundreds=joystick index, units=button index (+1)
    # joystick_axis too: hundreds=joystick index, units=axis index (+1), sign=direction
    section = 'KEYBOARD'
    Config.add_section(section)

    for index in controllers:
        controller = controllers[index]
        # we only care about player 1
        if controller.player != "1":
            continue

        # dirty hack: if quit is same button than another key, force r1 instead
        input_quit = controller.inputs[joystick_keys["KEY_QUIT"]]
        for propertyName, propertyValue in joystick_keys.items():
            if propertyName != "KEY_QUIT" and propertyValue is not None:
                inp = controller.inputs[propertyValue]
                if inp.type == input_quit.type and inp.id == input_quit.id:
                    joystick_keys["KEY_QUIT"] = "r1"
                    break

        for propertyName, keyboardValue in keyboard_keys.items():
            # Map buttons
            joystickButtonValue = 0
            if joystick_keys[propertyName] is not None:
                joystickButtonName = joystick_keys[propertyName]
                if joystickButtonName in controller.inputs:
                    inp = controller.inputs[joystickButtonName]
                    if inp.type == 'button':
                        joystickButtonValue = (int(index)-1)*100 + int(inp.id) + 1

            # Map axis
            joystickAxisValue = 0
            if joystick_axis[propertyName] is not None:
                joystickAxisNames = joystick_axis[propertyName]
                for joystickAxisName in joystickAxisNames:
                    axis_dir = 1
                    if joystickAxisName.startswith("-"):
                       axis_dir = -1
                       joystickAxisName = joystickAxisName[1:]
                    if joystickAxisName in controller.inputs:
                        inp = controller.inputs[joystickAxisName]
                        if inp.type == 'axis':
                            joystickAxisValue = ((int(index)-1)*100 + int(inp.id) + 1) * int(inp.value) * axis_dir
                            break

            Config.set(section, propertyName, keyboardValue+" "+str(joystickButtonValue)+" "+str(joystickAxisValue))

    cfgFile = open(recalboxFiles.daphneInputIni, "w")
    Config.write(cfgFile)
    cfgFile.close()
