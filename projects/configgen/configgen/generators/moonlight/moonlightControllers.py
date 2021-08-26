#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os

import configgen.settings.unixSettings as unixSettings
import configgen.recalboxFiles as recalboxFiles
from configgen.controllersConfig import Controller
from configgen.controllersConfig import ControllerDictionary

mlMapping = {   'a' :             {'button': 'btn_east'},
                'b' :             {'button': 'btn_south'},
                'x' :             {'button': 'btn_north'},
                'y' :             {'button': 'btn_west'},
                'start' :         {'button': 'btn_start'},
                'select':         {'button': 'btn_select'},
                'hotkey' :        {'button': 'btn_mode'},
                'l1' :            {'button': 'btn_tl'},
                'r1' :            {'button': 'btn_tr'},
                'l2' :            {'button': 'btn_tl2', 'axis': 'abs_z'},
                'r2' :            {'button': 'btn_tr2', 'axis':  'abs_rz'},
                'l3' :            {'button': 'btn_thumbl'},
                'r3' :            {'button': 'btn_thumbr'},
                # The DPAD can be an axis (for gpio sticks for example) or a hat or even a button
                'up' :            {'hat': 'abs_dpad_y', 'axis': 'abs_y', 'button': 'btn_dpad_up'},
                'down' :          {'button': 'btn_dpad_down'},
                'left' :          {'hat': 'abs_dpad_x', 'axis': 'abs_x', 'button': 'btn_dpad_left'},
                'right' :         {'button': 'btn_dpad_right'},
                'joystick1left' : {'axis': 'abs_x'},
                'joystick1up' :   {'axis': 'abs_y'},
                'joystick2left' : {'axis': 'abs_rx'},
                'joystick2up' :   {'axis': 'abs_ry'}
}


# Returns an array
# Index = mapping configuration file for player X
# Value = device path associated to the index
def writeControllersConfig(_, __, controllers: ControllerDictionary):
    config = dict()
    for controller in controllers:
        playerConfig = generateControllerConfig(controller, controllers[controller])
        confFile = recalboxFiles.moonlightMapping[int(controller)]
        if os.path.isfile(confFile):
            os.remove(confFile)
        os.mknod(confFile)
        mappingFile = unixSettings.UnixSettings(confFile, ' ')
        mappingFile.save("# Device name", controllers[controller].realName)
        mappingFile.save("# SDL2 GUID  ", controllers[controller].guid)
        mappingFile.save("# Event path ", controllers[controller].dev)
        for inp in playerConfig:
            mappingFile.save(inp, playerConfig[inp])
            config[confFile] = controllers[controller].dev
    return config


# Create a configuration file for a given controller
# returns an array :
# Index = Moonlight configuration parameter
# Value = the code extracted from es_input.cfg corresponding to the index
# ex : ['btn_select'] = 296
def generateControllerConfig(_, controller: Controller):
    config = dict()
   
    for index in controller.inputs:
        inp = controller.inputs[index]
        if inp.name not in mlMapping:
            continue
        if inp.type not in mlMapping[inp.name]:
            continue
        var = mlMapping[inp.name][inp.type]
        # Hats ids are not right, Y=X+1
        code = inp.code
        if inp.type == 'hat':
            if inp.name == 'up':
                code = int(inp.code) + 1
            else:
                code = inp.code
        config[var] = code
    
    # Add unhandled params
    config['reverse_x']         = "false"
    config['reverse_y']         = "true"
    config['reverse_rx']        = "false"
    config['reverse_ry']        = "true"
    config['reverse_dpad_x']    = "false"
    config['reverse_dpad_y']    = "false"
    config['abs_deadzone']      = "4"
    return config
