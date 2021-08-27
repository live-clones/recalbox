#!/usr/bin/env python
import os
from typing import Dict

import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllers.controller import InputItem, Controller

'''
This file mimic the retroarch config, using a separate configuration file for every controller
'''
class AmiberryRetroarchConfig:

    controlsAmiberryToRecalbox: Dict[str, int] = \
    {
        "input_up_btn"           : InputItem.ItemUp,
        "input_down_btn"         : InputItem.ItemDown,
        "input_left_btn"         : InputItem.ItemLeft,
        "input_right_btn"        : InputItem.ItemRight,
        "input_a_btn"            : InputItem.ItemA,
        "input_b_btn"            : InputItem.ItemB,
        "input_x_btn"            : InputItem.ItemX,
        "input_y_btn"            : InputItem.ItemY,
        "input_l_btn"            : InputItem.ItemL1,
        "input_r_btn"            : InputItem.ItemR1,
        "input_l2_btn"           : InputItem.ItemL2,
        "input_r2_btn"           : InputItem.ItemR2,
        "input_l3_btn"           : InputItem.ItemL3,
        "input_r3_btn"           : InputItem.ItemR3,
        "input_start_btn"        : InputItem.ItemStart,
        "input_select_btn"       : InputItem.ItemSelect,
        "input_enable_hotkey_btn": InputItem.ItemHotkey,
        "input_menu_toggle_btn"  : InputItem.ItemB,
        "input_reset_btn"        : InputItem.ItemX,
        "input_exit_emulator_btn": InputItem.ItemStart,
    }

    joysticksAmiberryToRecalbox: Dict[str, int] = \
    {
        "input_l_y_minus_axis": InputItem.ItemJoy1Up   ,
        "input_l_y_plus_axis" : InputItem.ItemJoy1Down ,
        "input_l_x_minus_axis": InputItem.ItemJoy1Left ,
        "input_l_x_plus_axis" : InputItem.ItemJoy1Right,
        "input_r_y_minus_axis": InputItem.ItemJoy2Up   ,
        "input_r_y_plus_axis" : InputItem.ItemJoy2Down ,
        "input_r_x_minus_axis": InputItem.ItemJoy2Left ,
        "input_r_x_plus_axis" : InputItem.ItemJoy2Right,
    }

    dpadToJostickAmiberry: Dict[str, str] = \
    {
        "input_l_y_minus_axis": "input_up_btn",
        "input_l_y_plus_axis" : "input_down_btn",
        "input_l_x_minus_axis": "input_left_btn",
        "input_l_x_plus_axis" : "input_right_btn",
    }

    HatsRecalboxToAmiberry: Dict[int, str] = \
    {
        1: 'up',
        2: 'right',
        4: 'down',
        8: 'left'
    }

    def __init__(self, controller: Controller):
        self.controller = controller
        self.settings = keyValueSettings(os.path.join(recalboxFiles.amiberryMountPoint, "conf", self.sanitizeFilename(controller.DeviceName) + ".cfg"), True)

    @staticmethod
    def sanitizeFilename(filename: str):
        for c in ('\\', '/', ':', '?', '"', '<', '>', '|'):
            filename = filename.replace(c, '')
        return filename

    @staticmethod
    def getInputValue(recalboxInput: InputItem) -> str:
        if recalboxInput.IsButton or recalboxInput.IsKey:
            return str(recalboxInput.Id)
        if recalboxInput.IsAxis:
            return ('-' if recalboxInput.Value < 0 else '+') + str(recalboxInput.Id)
        if recalboxInput.IsHat:
            return "h" + str(recalboxInput.Id) + AmiberryRetroarchConfig.HatsRecalboxToAmiberry[recalboxInput.Value]

    def generateConfiguration(self):
        # generate all controls but joysticks
        isAxis = False
        for key in AmiberryRetroarchConfig.controlsAmiberryToRecalbox:
            item = AmiberryRetroarchConfig.controlsAmiberryToRecalbox[key]
            if self.controller.HasInput(item):
                inputItem = self.controller.Input(item)
                isAxis = isAxis or inputItem.IsAxis
                self.settings.setString(key, '"' + AmiberryRetroarchConfig.getInputValue(inputItem) + '"')
            else:
                self.settings.setString(key, '"999"')

        # generate joysticks
        hasJoystick = False
        for key in AmiberryRetroarchConfig.joysticksAmiberryToRecalbox:
            item = AmiberryRetroarchConfig.joysticksAmiberryToRecalbox[key]
            if self.controller.HasInput(item):
                hasJoystick = True
                inputItem = self.controller.Input(item)
                self.settings.setString(key, '"' + ('-' if inputItem.Value < 0 else '+') + str(inputItem.Id) + '"')

        # When dpad is axis and there are no other joystick available, remap dpad to joystick
        if isAxis and not hasJoystick:
            for destination in AmiberryRetroarchConfig.dpadToJostickAmiberry:
                source = AmiberryRetroarchConfig.dpadToJostickAmiberry[destination]
                self.settings.setString(destination, self.settings.getString(source, ""))
                self.settings.setString(source, '"999"')

    def saveConfigurationFile(self):
        self.settings.saveFile()
