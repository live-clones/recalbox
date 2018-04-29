#!/usr/bin/env python
# -*- coding: utf-8 -*-

import recalboxFiles
import ConfigParser

keyboard_keys = {
    "KEY_UP":         "273 114",
    "KEY_DOWN":       "274 102",
    "KEY_LEFT":       "276 100",
    "KEY_RIGHT":      "275 103",
    "KEY_BUTTON1":    "306 97",
    "KEY_BUTTON2":    "308 115",
    "KEY_BUTTON3":    "32 113",
    "KEY_START1":     "49 0",
    "KEY_START2":     "50 0",
    "KEY_COIN1":      "53 0",
    "KEY_COIN2":      "54 0",
    "KEY_SKILL1":     "304 119",
    "KEY_SKILL2":     "122 105",
    "KEY_SKILL3":     "120 107",
    "KEY_SERVICE":    "57 0",
    "KEY_TEST":       "283 0",
    "KEY_RESET":      "284 0",
    "KEY_SCREENSHOT": "293 0",
    "KEY_QUIT":       "27 113"
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
    "KEY_QUIT":       "hotkey"
}

# Create the controller configuration file
def generateControllerConfig(system, controllers):
    Config = ConfigParser.ConfigParser()
    # To prevent ConfigParser from converting to lower case
    Config.optionxform = str

    section = 'KEYBOARD'
    Config.add_section(section)


    for index in controllers:
        controller = controllers[index]
        # we only care about player 1
        if controller.player != "1":
            continue

        for propertyName, keyboardValue in keyboard_keys.iteritems():
            joystickValue = 0
            joystickButtonName = joystick_keys[propertyName]
            if joystickButtonName in controller.inputs:
                input = controller.inputs[joystickButtonName]
                if input.type == 'button':
                    joystickValue = int(input.id) + 1
            Config.set(section, propertyName, keyboardValue+" "+str(joystickValue))

    cfgFile = open(recalboxFiles.daphneInputIni, "w")
    Config.write(cfgFile)
    cfgFile.close()
