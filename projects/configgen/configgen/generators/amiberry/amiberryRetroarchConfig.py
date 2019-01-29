#!/usr/bin/env python
import os

import recalboxFiles
from settings.keyValueSettings import keyValueSettings

'''
This file mimic the retroarch config, using a separate configuration file for every controller
'''
class AmiberryRetroarchConfig:

    controlsAmiberryToRecalbox = \
    {
        "input_up_btn"           : "up",
        "input_down_btn"         : "down",
        "input_left_btn"         : "left",
        "input_right_btn"        : "right",
        "input_a_btn"            : "a",
        "input_b_btn"            : "b",
        "input_x_btn"            : "x",
        "input_y_btn"            : "y",
        "input_l_btn"            : "pageup",
        "input_r_btn"            : "pagedown",
        "input_l2_btn"           : "l2",
        "input_r2_btn"           : "r2",
        "input_l3_btn"           : "l3",
        "input_r3_btn"           : "r3",
        "input_start_btn"        : "start",
        "input_select_btn"       : "select",
        "input_enable_hotkey_btn": "hotkey",
        "input_menu_toggle_btn"  : "b",
        "input_reset_btn"        : "x",
        "input_exit_emulator_btn": "start",
    }

    joysticksAmiberryToRecalbox = \
    {
        "input_l_y_minus_axis": ( "joystick1up"  , "-"),
        "input_l_y_plus_axis" : ( "joystick1up"  , "+"),
        "input_l_x_minus_axis": ( "joystick1left", "-"),
        "input_l_x_plus_axis" : ( "joystick1left", "+"),
        "input_r_y_minus_axis": ( "joystick2up"  , "-"),
        "input_r_y_plus_axis" : ( "joystick2up"  , "+"),
        "input_r_x_minus_axis": ( "joystick2left", "-"),
        "input_r_x_plus_axis" : ( "joystick2left", "+"),
    }

    HatsRecalboxToAmiberry = \
    {
        '1': 'up',
        '2': 'right',
        '4': 'down',
        '8': 'left'
    }

    def __init__(self, controller):
        self.controller = controller
        self.settings = keyValueSettings(os.path.join(recalboxFiles.amiberryMountPoint, "conf", controller.realName + ".cfg"), True)

    @staticmethod
    def getInputValue(recalboxInput):
        if recalboxInput.type == "button" or recalboxInput.type == "key":
            return recalboxInput.id
        if recalboxInput.type == 'axis':
            return ("-" if int(recalboxInput.value) < 0 else "+") + str(recalboxInput.id)
        if recalboxInput.type == 'hat':
            return "h" + recalboxInput.id + AmiberryRetroarchConfig.HatsRecalboxToAmiberry[recalboxInput.value]

    def generateConfiguration(self):
        # generate all controls but joysticks
        for amiberryKey in AmiberryRetroarchConfig.controlsAmiberryToRecalbox:
            recalboxControlKey = AmiberryRetroarchConfig.controlsAmiberryToRecalbox[amiberryKey]
            if recalboxControlKey in self.controller.inputs:
                recalboxInput = self.controller.inputs[recalboxControlKey]
                self.settings.setOption(amiberryKey, '"' + AmiberryRetroarchConfig.getInputValue(recalboxInput) + '"')
        # generate joysticks
        for amiberryKey in AmiberryRetroarchConfig.joysticksAmiberryToRecalbox:
            recalboxControlKey, axisSign = AmiberryRetroarchConfig.joysticksAmiberryToRecalbox[amiberryKey]
            if recalboxControlKey in self.controller.inputs:
                recalboxInput = self.controller.inputs[recalboxControlKey]
                self.settings.setOption(amiberryKey, '"' + axisSign + str(recalboxInput.id) +'"')

    def saveConfigurationFile(self):
        self.settings.saveFile()
