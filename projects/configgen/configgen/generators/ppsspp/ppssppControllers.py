#!/usr/bin/env python
from typing import Dict

import configgen.recalboxFiles as recalboxFiles

# This configgen is based on PPSSPP 1.2.2. Therefore, all code/github references are valid at this version, and may not be valid with later updates

# PPSSPP internal "NKCodes" https://github.com/hrydgard/ppsspp/blob/master/ext/native/inp/keycodes.h#L198
# Will later be used to convert SDL inp ids
from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import Controller
from configgen.settings.iniSettings import IniSettings

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

sdlIndexToIdPad: Dict[int, int] = \
{
    0: DEVICE_ID_PAD_0,
    1: DEVICE_ID_PAD_1,
    2: DEVICE_ID_PAD_2,
    3: DEVICE_ID_PAD_3,
    4: DEVICE_ID_PAD_4
}
# SDL 2.0.4 inp ids conversion table to NKCodes
# See https://hg.libsdl.org/SDL/file/e12c38730512/include/SDL_gamecontroller.h#l262
# See https://github.com/hrydgard/ppsspp/blob/master/SDL/SDLJoystick.h#L91
sdlNameToNKCode: Dict[int, int] = \
{
    InputItem.ItemB      : NKCODE_BUTTON_2, # A
    InputItem.ItemA      : NKCODE_BUTTON_3, # B
    InputItem.ItemY      : NKCODE_BUTTON_4, # X
    InputItem.ItemX      : NKCODE_BUTTON_1, # Y
    InputItem.ItemSelect : NKCODE_BUTTON_9, # SELECT/BACK
    InputItem.ItemHotkey : NKCODE_BACK, # GUIDE
    InputItem.ItemStart  : NKCODE_BUTTON_10, # START
    # "7" : NKCODE_BUTTON_?, # L3, unsued
    # "8" : NKCODE_BUTTON_?, # R3, unsued
    InputItem.ItemL1     : NKCODE_BUTTON_6, # L
    InputItem.ItemR1     : NKCODE_BUTTON_5, # R
    InputItem.ItemUp     : NKCODE_DPAD_UP,
    InputItem.ItemDown   : NKCODE_DPAD_DOWN,
    InputItem.ItemLeft   : NKCODE_DPAD_LEFT,
    InputItem.ItemRight  : NKCODE_DPAD_RIGHT
}

SDLHatMap: Dict[int, int] = \
{
    InputItem.ItemUp    : NKCODE_DPAD_UP,
    InputItem.ItemDown  : NKCODE_DPAD_DOWN,
    InputItem.ItemLeft  : NKCODE_DPAD_LEFT,
    InputItem.ItemRight : NKCODE_DPAD_RIGHT
}

SDLJoyAxisMap: Dict[int, int] = \
{
    0 : JOYSTICK_AXIS_X,
    1 : JOYSTICK_AXIS_Y,
    2 : JOYSTICK_AXIS_Z,
    3 : JOYSTICK_AXIS_RZ,
    4 : JOYSTICK_AXIS_LTRIGGER,
    5 : JOYSTICK_AXIS_RTRIGGER
}

ppssppMapping: Dict[int, Dict[int, str]] = \
{
    InputItem.ItemA :        { InputItem.TypeButton: 'Circle' },
    InputItem.ItemB :        { InputItem.TypeButton: 'Cross' },
    InputItem.ItemX :        { InputItem.TypeButton: 'Triangle' },
    InputItem.ItemY :        { InputItem.TypeButton: 'Square' },
    InputItem.ItemStart :    { InputItem.TypeButton: 'Start' },
    InputItem.ItemSelect :   { InputItem.TypeButton: 'Select' },
    InputItem.ItemHotkey :   { InputItem.TypeButton: 'Pause' },
    InputItem.ItemL1 :       { InputItem.TypeButton: 'L' },
    InputItem.ItemR1 :       { InputItem.TypeButton: 'R' },
    InputItem.ItemJoy1Left : { InputItem.TypeAxis  : 'An.Left' },
    InputItem.ItemJoy1Up :   { InputItem.TypeAxis  : 'An.Up' },
    InputItem.ItemJoy2Left : { InputItem.TypeAxis  : 'RightAn.Left' },
    InputItem.ItemJoy2Up :   { InputItem.TypeAxis  : 'RightAn.Up' },
    # The DPAD can be an axis (for gpio sticks for example) or a hat
    InputItem.ItemUp :       { InputItem.TypeHat: 'Up',    InputItem.TypeAxis: 'Up',    InputItem.TypeButton: 'Up' },
    InputItem.ItemDown :     { InputItem.TypeHat: 'Down',  InputItem.TypeAxis: 'Down',  InputItem.TypeButton: 'Down' },
    InputItem.ItemLeft :     { InputItem.TypeHat: 'Left',  InputItem.TypeAxis: 'Left',  InputItem.TypeButton: 'Left' },
    InputItem.ItemRight :    { InputItem.TypeHat: 'Right', InputItem.TypeAxis: 'Right', InputItem.TypeButton: 'Right' },
    # Need to add pseudo inputs as PPSSPP doesn't manually invert axises, and these are not referenced in es_input.cfg
    InputItem.ItemJoy1Right :{ InputItem.TypeAxis: 'An.Right' },
    InputItem.ItemJoy1Down : { InputItem.TypeAxis: 'An.Down' },
    InputItem.ItemJoy2Right :{ InputItem.TypeAxis: 'RightAn.Right' },
    InputItem.ItemJoy2Down : { InputItem.TypeAxis: 'RightAn.Down' }
}


# Create the controller configuration file
# returns its name
def generateControllerConfig(controller: Controller):
    # Set config file name
    configFileName = recalboxFiles.ppssppControlsIni
    configFile = IniSettings(configFileName, True)
    configFile.loadFile(True)
    # As we start with the default ini file, no need to create the section
    section = "ControlMapping"

    # Parse controller inputs
    for item in controller.AvailableInput:
        if item.Item not in ppssppMapping or item.Type not in ppssppMapping[item.Item]:
            continue

        var: str = ppssppMapping[item.Item][item.Type]

        deviceIdPad = sdlIndexToIdPad[int(controller.SdlIndex)]
        if item.IsButton:
            pspcode: int = sdlNameToNKCode[item.Item]
            val: str = optionValue(configFile, section, var, "{}-{}".format(deviceIdPad, pspcode))
            configFile.setString(section, var, val)

        elif item.IsAxis:
            nkAxisId = SDLJoyAxisMap[item.Id] # Get the axis code
            pspcode: int = axisToCode(nkAxisId, item.Value) # Apply the magic axis formula
            val: str = optionValue(configFile, section, var, "{}-{}".format(deviceIdPad, pspcode))
            configFile.setString(section, var, val)

        elif item.IsHat and item.Item in SDLHatMap:
            var: str = ppssppMapping[item.Item][item.Type]
            pspcode: int = SDLHatMap[item.Item]
            val: str = "{}-{}".format( deviceIdPad, pspcode )
            val: str = optionValue(configFile, section, var, val)
            configFile.setString(section, var, val)
        
    configFile.saveFile()

    return configFileName


# Simple rewrite of https://github.com/hrydgard/ppsspp/blob/eaeddc6c23cf86514f45199659ecc7396c91a3c0/Common/KeyMap.cpp#L747
def axisToCode(axisId: int, direction: int) :
    if direction < 0:
        direction = 1
    else:
        direction = 0
    return AXIS_BIND_NKCODE_START + axisId * 2 + direction

# determine if the option already exists or not
def optionValue(config: IniSettings, section: str, option: str, value: str) -> str:
    if config.hasOption(section, option):
        return "{},{}".format(config.getString(section, option, ""), value)
    else:
        return value

