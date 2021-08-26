#!/usr/bin/env python
import os

import configgen.recalboxFiles as recalboxFiles
from configgen.settings.keyValueSettings import keyValueSettings
from configgen.controllersConfig import Input

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
        "input_l_btn"            : "l1",
        "input_r_btn"            : "r1",
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

    dpadToJostickAmiberry = \
    {
        "input_l_y_minus_axis": "input_up_btn",
        "input_l_y_plus_axis" : "input_down_btn",
        "input_l_x_minus_axis": "input_left_btn",
        "input_l_x_plus_axis" : "input_right_btn",
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
        self.settings = keyValueSettings(os.path.join(recalboxFiles.amiberryMountPoint, "conf", self.sanitizeFilename(controller.realName) + ".cfg"), True)

    @staticmethod
    def sanitizeFilename(filename: str):
        for c in ('\\', '/', ':', '?', '"', '<', '>', '|'):
            filename = filename.replace(c, '')
        return filename

    @staticmethod
    def getInputValue(recalboxInput: Input):
        if recalboxInput.type in ('button', 'key'):
            return recalboxInput.id
        if recalboxInput.type == 'axis':
            return ("-" if int(recalboxInput.value) < 0 else "+") + str(recalboxInput.id)
        if recalboxInput.type == 'hat':
            return "h" + recalboxInput.id + AmiberryRetroarchConfig.HatsRecalboxToAmiberry[recalboxInput.value]

    def generateConfiguration(self):
        # generate all controls but joysticks
        isAxis = False
        for amiberryKey in AmiberryRetroarchConfig.controlsAmiberryToRecalbox:
            recalboxControlKey = AmiberryRetroarchConfig.controlsAmiberryToRecalbox[amiberryKey]
            if recalboxControlKey in self.controller.inputs:
                recalboxInput = self.controller.inputs[recalboxControlKey]
                if recalboxInput.type == 'axis':
                    isAxis = True
                self.settings.setOption(amiberryKey, '"' + AmiberryRetroarchConfig.getInputValue(recalboxInput) + '"')
            else:
                self.settings.setOption(amiberryKey, '"999"')

        # generate joysticks
        hasJoystick = False
        for amiberryKey in AmiberryRetroarchConfig.joysticksAmiberryToRecalbox:
            recalboxControlKey, axisSign = AmiberryRetroarchConfig.joysticksAmiberryToRecalbox[amiberryKey]
            if recalboxControlKey in self.controller.inputs:
                hasJoystick = True
                recalboxInput = self.controller.inputs[recalboxControlKey]
                self.settings.setOption(amiberryKey, '"' + axisSign + str(recalboxInput.id) + '"')

        # When dpad is axis and there are no other joystick available, remap dpad to joystick
        if isAxis and not hasJoystick:
            for destination in AmiberryRetroarchConfig.dpadToJostickAmiberry:
                source = AmiberryRetroarchConfig.dpadToJostickAmiberry[destination]
                self.settings.setOption(destination, self.settings.getOption(source, ""))
                self.settings.setOption(source, '"999"')

    def saveConfigurationFile(self):
        self.settings.saveFile()
