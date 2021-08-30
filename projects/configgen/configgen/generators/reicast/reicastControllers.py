#!/usr/bin/env python
from typing import Dict

import configgen.recalboxFiles as recalboxFiles
from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import Controller

reicastMapping: Dict[int, Dict[int, str]] = \
{
    InputItem.ItemA:        { InputItem.TypeButton: 'btn_b'},
    InputItem.ItemB:        { InputItem.TypeButton: 'btn_a'},
    InputItem.ItemX:        { InputItem.TypeButton: 'btn_y'},
    InputItem.ItemY:        { InputItem.TypeButton: 'btn_x'},
    InputItem.ItemStart:    { InputItem.TypeButton: 'btn_start'},
    InputItem.ItemHotkey:   { InputItem.TypeButton: 'btn_escape'},
    InputItem.ItemL1:       { InputItem.TypeAxis  : 'axis_trigger_left',  InputItem.TypeButton: 'btn_trigger_left'},
    InputItem.ItemR1:       { InputItem.TypeAxis  : 'axis_trigger_right', InputItem.TypeButton: 'btn_trigger_right'},
    InputItem.ItemJoy1Left: { InputItem.TypeAxis  : 'axis_x'},
    InputItem.ItemJoy1Up:   { InputItem.TypeAxis  : 'axis_y'},
    # The DPAD can be an axis (for gpio sticks for example) or a hat
    InputItem.ItemLeft:     { InputItem.TypeHat   : 'axis_dpad1_x', InputItem.TypeAxis  : 'axis_x', InputItem.TypeButton: 'btn_dpad1_left'},
    InputItem.ItemUp:       { InputItem.TypeHat   : 'axis_dpad1_y', InputItem.TypeAxis  : 'axis_y', InputItem.TypeButton: 'btn_dpad1_up'},
    InputItem.ItemRight:    { InputItem.TypeButton: 'btn_dpad1_right'},
    InputItem.ItemDown:     { InputItem.TypeButton: 'btn_dpad1_down'},
    # We are only interested in L2/R2 if they are axis, to have real dreamcasttriggers
    InputItem.ItemR2:       { InputItem.TypeAxis  : 'axis_trigger_right'},
    InputItem.ItemL2:       { InputItem.TypeAxis  : 'axis_trigger_left'}
}

sections: Dict[str, str] = \
{
    'mapping_name':                'emulator',
    'btn_escape':                  'emulator',
    'btn_a':                       'dreamcast',
    'btn_b':                       'dreamcast',
    'btn_c':                       'dreamcast',
    'btn_d':                       'dreamcast',
    'btn_z':                       'dreamcast',
    'btn_x':                       'dreamcast',
    'btn_y':                       'dreamcast',
    'btn_start':                   'dreamcast',
    'axis_x':                      'dreamcast',
    'axis_y':                      'dreamcast',
    'axis_trigger_left':           'dreamcast',
    'axis_trigger_right':          'dreamcast',
    'btn_dpad1_left':              'dreamcast',
    'btn_dpad1_right':             'dreamcast',
    'btn_dpad1_up':                'dreamcast',
    'btn_dpad1_down':              'dreamcast',
    'btn_dpad2_left':              'dreamcast',
    'btn_dpad2_right':             'dreamcast',
    'btn_dpad2_up':                'dreamcast',
    'btn_dpad2_down':              'dreamcast',
    'axis_dpad1_x':                'compat',
    'axis_dpad1_y':                'compat',
    'btn_trigger_left':            'compat',
    'btn_trigger_right':           'compat',
    'axis_dpad2_x':                'compat',
    'axis_dpad2_y':                'compat',
    'axis_x_inverted':             'compat',
    'axis_y_inverted':             'compat',
    'axis_trigger_left_inverted':  'compat',
    'axis_trigger_right_inverted': 'compat'
}


# Create the controller configuration file
# returns its name
def generateControllerConfig(controller: Controller):
    # Set config file name
    configFileName = "{}/controllerP{}.cfg".format(recalboxFiles.reicastCustom, controller.PlayerIndex)
    from configgen.settings.iniSettings import IniSettings
    configFile = IniSettings(configFileName, True)
    configFile.loadFile(True)

    # Add controller name
    configFile.setString('emulator', "mapping_name", controller.DeviceName)

    # Parse controller inputs
    for item in controller.AvailableInput:
        if item.Item not in reicastMapping: continue
        if item.Type not in reicastMapping[item.Item]: continue
        var: str = reicastMapping[item.Item][item.Type]
        section: str = "error"
        if var in sections:
           section = sections[var]

        # Sadly, we don't get the right axis code for Y hats. So, dirty hack time
        code = item.Code
        if item.Type == InputItem.TypeHat and item.Item == InputItem.ItemUp:
            code = code + 1

        configFile.setInt(section, var, code)

    configFile.saveFile()

    return configFileName
