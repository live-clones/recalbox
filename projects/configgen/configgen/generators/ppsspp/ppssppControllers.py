#!/usr/bin/env python
# -*- coding: utf-8 -*-
from configparser import ConfigParser
import configgen.recalboxFiles as recalboxFiles

# This configgen is based on PPSSPP 1.2.2. Therefore, all code/github references are valid at this version, and may not be valid with later updates

# PPSSPP internal "NKCodes" https://github.com/hrydgard/ppsspp/blob/master/ext/native/inp/keycodes.h#L198
# Will later be used to convert SDL inp ids
from configgen.controllersConfig import Controller

NKCODE_BUTTON_1 = 188
NKCODE_BUTTON_2 = 189
NKCODE_BUTTON_3 = 190
NKCODE_BUTTON_4 = 191
NKCODE_BUTTON_5 = 192
NKCODE_BUTTON_6 = 193
NKCODE_BUTTON_7 = 194
NKCODE_BUTTON_8 = 195
NKCODE_BUTTON_9 = 196
NKCODE_BUTTON_10 = 197
NKCODE_BUTTON_11 = 198
NKCODE_BUTTON_12 = 199
NKCODE_BUTTON_13 = 200
NKCODE_BUTTON_14 = 201
NKCODE_BUTTON_15 = 202
NKCODE_BUTTON_16 = 203
NKCODE_BACK = 4
JOYSTICK_AXIS_X = 0
JOYSTICK_AXIS_Y = 1
JOYSTICK_AXIS_HAT_X = 15
JOYSTICK_AXIS_HAT_Y = 16
JOYSTICK_AXIS_Z = 11
JOYSTICK_AXIS_RZ = 14
JOYSTICK_AXIS_LTRIGGER = 17
JOYSTICK_AXIS_RTRIGGER = 18
NKCODE_DPAD_UP = 19
NKCODE_DPAD_DOWN = 20
NKCODE_DPAD_LEFT = 21
NKCODE_DPAD_RIGHT = 22

# PPSSPP defined an offset for axis, see https://github.com/hrydgard/ppsspp/blob/eaeddc6c23cf86514f45199659ecc7396c91a3c0/Common/KeyMap.cpp#L694
AXIS_BIND_NKCODE_START = 4000

# From https://github.com/hrydgard/ppsspp/blob/master/ext/native/inp/input_state.h#L26
DEVICE_ID_PAD_0 = 10
DEVICE_ID_PAD_1 = 11
DEVICE_ID_PAD_2 = 12
DEVICE_ID_PAD_3 = 13
DEVICE_ID_PAD_4 = 14
sdlIndexToIdPad = {
    0: DEVICE_ID_PAD_0,
    1: DEVICE_ID_PAD_1,
    2: DEVICE_ID_PAD_2,
    3: DEVICE_ID_PAD_3,
    4: DEVICE_ID_PAD_4
}
# SDL 2.0.4 inp ids conversion table to NKCodes
# See https://hg.libsdl.org/SDL/file/e12c38730512/include/SDL_gamecontroller.h#l262
# See https://github.com/hrydgard/ppsspp/blob/master/SDL/SDLJoystick.h#L91
sdlNameToNKCode = {
        "b" : NKCODE_BUTTON_2, # A
        "a" : NKCODE_BUTTON_3, # B
        "y" : NKCODE_BUTTON_4, # X
        "x" : NKCODE_BUTTON_1, # Y
        "select" : NKCODE_BUTTON_9, # SELECT/BACK
        "hotkey" : NKCODE_BACK, # GUIDE
        "start" : NKCODE_BUTTON_10, # START
        # "7" : NKCODE_BUTTON_?, # L3, unsued
        # "8" : NKCODE_BUTTON_?, # R3, unsued
        "l1" : NKCODE_BUTTON_6, # L
        "r1" : NKCODE_BUTTON_5, # R
        "up" : NKCODE_DPAD_UP,
        "down" : NKCODE_DPAD_DOWN,
        "left" : NKCODE_DPAD_LEFT,
        "right" : NKCODE_DPAD_RIGHT
}

SDLHatMap = {
        "up" : NKCODE_DPAD_UP,
        "down" : NKCODE_DPAD_DOWN,
        "left" : NKCODE_DPAD_LEFT,
        "right" : NKCODE_DPAD_RIGHT
}

SDLJoyAxisMap = {
        "0" : JOYSTICK_AXIS_X,
        "1" : JOYSTICK_AXIS_Y,
        "2" : JOYSTICK_AXIS_Z,
        "3" : JOYSTICK_AXIS_RZ,
        "4" : JOYSTICK_AXIS_LTRIGGER,
        "5" : JOYSTICK_AXIS_RTRIGGER
}

ppssppMapping =  { 'a' :             {'button': 'Circle'},
                   'b' :             {'button': 'Cross'},
                   'x' :             {'button': 'Triangle'},
                   'y' :             {'button': 'Square'},
                   'start' :         {'button': 'Start'},
                   'select' :        {'button': 'Select'},
                   'hotkey' :        {'button': 'Pause'},
                   'l1' :            {'button': 'L'},
                   'r1' :            {'button': 'R'},
                   'joystick1left' : {'axis': 'An.Left'},
                   'joystick1up' :   {'axis': 'An.Up'},
                   'joystick2left' : {'axis': 'RightAn.Left'},
                   'joystick2up' :   {'axis': 'RightAn.Up'},
                   # The DPAD can be an axis (for gpio sticks for example) or a hat
                   'up' :            {'hat': 'Up',    'axis': 'Up',    'button': 'Up'},
                   'down' :          {'hat': 'Down',  'axis': 'Down',  'button': 'Down'},
                   'left' :          {'hat': 'Left',  'axis': 'Left',  'button': 'Left'},
                   'right' :         {'hat': 'Right', 'axis': 'Right', 'button': 'Right'},
                   # Need to add pseudo inputs as PPSSPP doesn't manually invert axises, and these are not referenced in es_input.cfg
                   'joystick1right' :{'axis': 'An.Right'},
                   'joystick1down' : {'axis': 'An.Down'},
                   'joystick2right' :{'axis': 'RightAn.Right'},
                   'joystick2down' : {'axis': 'RightAn.Down'}
}


# Create the controller configuration file
# returns its name
def generateControllerConfig(controller: Controller):
    # Set config file name
    configFileName = recalboxFiles.ppssppControlsIni
    Config = ConfigParser()
    Config.optionxform = str
    # We need to read the default file as PPSSPP needs the keyboard defs ine the controlls.ini file otherwise the GYUI won't repond
    Config.read(recalboxFiles.ppssppControlsInit)
    # As we start with the default ini file, no need to create the section
    section = "ControlMapping"

    # Parse controller inputs
    for index in controller.inputs:
        inp = controller.inputs[index]
        if inp.name not in ppssppMapping or inp.type not in ppssppMapping[inp.name]:
            continue

        var = ppssppMapping[inp.name][inp.type]

        #code = inp.code
        deviceIdPad = sdlIndexToIdPad[int(controller.index)]
        if inp.type == 'button':
            pspcode = sdlNameToNKCode[inp.name]
            val = "{}-{}".format( deviceIdPad, pspcode )
            val = optionValue(Config, section, var, val)
            Config.set(section, var, val)

        elif inp.type == 'axis':
            # Get the axis code
            nkAxisId = SDLJoyAxisMap[inp.id]
            # Apply the magic axis formula
            pspcode = axisToCode(nkAxisId, int(inp.value))
            val = "{}-{}".format( deviceIdPad, pspcode )
            val = optionValue(Config, section, var, val)
            print("Adding {} to {}".format(var, val))
            Config.set(section, var, val)
            
            # Skip the rest if it's an axis dpad
            if inp.name in [ 'up', 'down', 'left', 'right' ] : continue
            # Also need to do the opposite direction manually. The inp.id is the same as up/left, but the direction is opposite
            if inp.name == 'joystick1up':
                var = ppssppMapping['joystick1down'][inp.type]
            elif inp.name == 'joystick1left':
                var = ppssppMapping['joystick1right'][inp.type]
            elif inp.name == 'joystick2up':
                var = ppssppMapping['joystick2down'][inp.type]
            elif inp.name == 'joystick2left':
                var = ppssppMapping['joystick2right'][inp.type]
                
            pspcode = axisToCode(nkAxisId, -int(inp.value))
            val = "{}-{}".format( deviceIdPad, pspcode )
            val = optionValue(Config, section, var, val)
            Config.set(section, var, val)
        
        elif inp.type == 'hat' and inp.name in SDLHatMap:
            var = ppssppMapping[inp.name][inp.type]
            pspcode = SDLHatMap[inp.name]
            val = "{}-{}".format( deviceIdPad, pspcode )
            val = optionValue(Config, section, var, val)
            Config.set(section, var, val)
        
    cfgfile = open(configFileName,'w+')
    Config.write(cfgfile)
    cfgfile.close()
    return configFileName


# Simple rewrite of https://github.com/hrydgard/ppsspp/blob/eaeddc6c23cf86514f45199659ecc7396c91a3c0/Common/KeyMap.cpp#L747
def axisToCode(axisId: int, direction: int) :
    if direction < 0:
        direction = 1
    else:
        direction = 0
    return AXIS_BIND_NKCODE_START + axisId * 2 + direction

# determine if the option already exists or not
def optionValue(config: ConfigParser, section: str, option: str, value: str):
    if config.has_option(section, option):
        return "{},{}".format(config.get(section, option), value)
    else:
        return value

