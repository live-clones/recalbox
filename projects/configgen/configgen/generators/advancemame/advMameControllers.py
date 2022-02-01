#!/usr/bin/env python
from typing import Dict, Tuple

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.controller import InputItem, Controller, ControllerPerPlayer

advanceMapping: Dict[int, Tuple[str]] = \
{
    InputItem.ItemA        : ( 'p{}_button2', ),
    InputItem.ItemB        : ( 'p{}_button1', 'ui_select' ),
    InputItem.ItemX        : ( 'p{}_button4', ),
    InputItem.ItemY        : ( 'p{}_button3', ),
    InputItem.ItemStart    : ( 'start{}', ),
    InputItem.ItemSelect   : ( 'coin{}', ),
    #~ 'hotkey' :        '',
    InputItem.ItemL1       : ( 'p{}_button5', ),
    InputItem.ItemR1       : ( 'p{}_button6', ),
    InputItem.ItemUp       : ( 'p{}_up',    'ui_up' ),
    InputItem.ItemDown     : ( 'p{}_down',  'ui_down' ),
    InputItem.ItemLeft     : ( 'p{}_left',  'ui_left' ),
    InputItem.ItemRight    : ( 'p{}_right', 'ui_right' ),
    InputItem.ItemJoy1Up   : ( 'p{}_up',    'p{}_doubleleft_up',    'ui_up'    ),
    InputItem.ItemJoy1Down : ( 'p{}_down',  'p{}_doubleleft_down',  'ui_down'  ),
    InputItem.ItemJoy1Left : ( 'p{}_left',  'p{}_doubleleft_left',  'ui_left'  ),
    InputItem.ItemJoy1Right: ( 'p{}_right', 'p{}_doubleleft_right', 'ui_right' ),
    InputItem.ItemJoy2Up   : ( 'p{}_doubleright_up', ),
    InputItem.ItemJoy2Down : ( 'p{}_doubleright_down', ),
    InputItem.ItemJoy2Left : ( 'p{}_doubleright_left', ),
    InputItem.ItemJoy2Right: ( 'p{}_doubleright_right', ),
    InputItem.ItemL2       : ( 'p{}_button7', ),
    #~ 'r2' :            'p{}_button8',
    InputItem.ItemR2       : ( 'ui_configure', ),
    InputItem.ItemL3       : ( 'p{}_button9', ),
    InputItem.ItemR3       : ( 'p{}_button10', )
}

advanceCombo: Dict[int, str] =\
{
    InputItem.ItemA: 'ui_soft_reset',
    InputItem.ItemY: 'ui_save_state',
    InputItem.ItemX: 'ui_load_state',
    InputItem.ItemStart: 'ui_cancel',
    InputItem.ItemRight: 'ui_turbo',
    #~ 'r2' : 'ui_mode_pred', # Comment this one for now as it needs a "not" on the ui_configure
    InputItem.ItemL2: 'ui_mode_pred'
}


def writeConfig(system: Emulator, controllers: ControllerPerPlayer, args):
    finalConfig = getDefaultConfig()

    # Write rom path
    import os
    finalConfig["dir_rom"] = os.path.dirname(args.rom)
    finalConfig["dir_image"] = os.path.join(os.path.dirname(args.rom), "media/images")
    finalConfig["dir_snap"] = recalboxFiles.screenshotsDir

    # misc options
    finalConfig["display_vsync"] = "yes" if system.ShowFPS else "no"
    finalConfig["display_resize"] = "integer" if system.IntegerScale else "mixed"

    for controller in controllers.values():
        ctrlConfig: Dict[str, str] = getControllerConfig(controller)
        finalConfig: Dict[str, str] = intelligentExtend(finalConfig, ctrlConfig)

    with open(recalboxFiles.advancemameConfig, 'w') as f:
        for key in sorted(finalConfig):
            f.write("{} {}\n".format(key, finalConfig[key]))


def getDefaultConfig() -> Dict[str, str]:
    # Open the default file
    with open(recalboxFiles.advancemameConfigOrigin) as f:
        # read the values
        content = f.readlines()

    returnValue = {}
    # Reorder to a dict
    for line in content:
        # There may be some empty lines. Lazy method to handle errors on the split that would crash
        try:
            index, value = line.split(" ", 1)
            returnValue[index] = value.strip('\n')
            continue
        except ValueError:
            continue

    return returnValue


def getControllerConfig(controller: Controller) -> Dict[str, str]:
    returnValue = {}
    # Read the pad and configure
    for item in controller.AvailableInput:
        if item.Item in advanceMapping:
            for event in advanceMapping[item.Item]:
                index = "input_map[{}]".format(event.format(controller.PlayerIndex))
                value = generateButton(controller.NaturalIndex, item)
                returnValue = intelligentAppend(returnValue, index, value)

    # For player 1 only : add UI combos
    if controller.PlayerIndex == 1 and controller.HasHotkey:
        hk = controller.Hotkey
        for item, event in advanceCombo.items():
            if controller.HasInput(item):
                buttonInput = controller.Input(item)
                value = generateCombo(controller.NaturalIndex, buttonInput, hk)
                index = "input_map[{}]".format(event)
                returnValue = intelligentAppend(returnValue, index, value)

    return returnValue


def intelligentAppend(sourceDict: Dict[str, str], index: str, value: str) -> Dict[str, str]:
    if index in sourceDict:
        sourceDict[index] += " or {}".format(value)
    else:
        sourceDict[index] = value
    return sourceDict

def intelligentExtend(sourceDict: Dict[str, str], mergeDict: Dict[str, str]) -> Dict[str, str]:
    for index, value in mergeDict.items():
        sourceDict = intelligentAppend(sourceDict, index, value)
    return sourceDict

def generateButton(joyIndex: int, inputObject: InputItem) -> str:
    #~ http://www.advancemame.it/doc-advmame#8.9.6
    if inputObject.IsButton:
        return "joystick_button[{},{}]".format(joyIndex, inputObject.Id)
    elif inputObject.IsAxis:
        value = '1' if inputObject.Value < 0 else '0'
        # We suppose the CONTROL value is always 0 for analogue sticks
        return "joystick_digital[{},0,{},{}]".format(joyIndex, inputObject.Id, value)
    elif inputObject.IsHat:
        # We suppose the CONTROL value is always 1 for hats, as well as up = 1,1, down = 1,0, left = 0,1 and right = 0,0
        if inputObject.IsUp:
            return "joystick_digital[{},1,1,1]".format(joyIndex)
        elif inputObject.IsDown:
            return "joystick_digital[{},1,1,0]".format(joyIndex)
        elif inputObject.IsLeft:
            return "joystick_digital[{},1,0,1]".format(joyIndex)
        elif inputObject.IsRight:
            return "joystick_digital[{},1,0,0]".format(joyIndex)


def generateCombo(joyIndex: int, inputObject: InputItem, hkInputObject: InputItem) -> str:
    buttonKey = generateButton(joyIndex, inputObject)
    hotKey = generateButton(joyIndex, hkInputObject)
    return "{} {}".format(hotKey, buttonKey)
