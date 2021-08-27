#!/usr/bin/env python
import os
from typing import Dict
from xml.dom import minidom
from configgen.controllers.controller import InputItem, Controller, ControllerPerPlayer
from configgen.settings.iniSettings import IniSettings
import configgen.recalboxFiles as recalboxFiles

# Must read :
# http://mupen64plus.org/wiki/index.php?title=Mupen64Plus_Plugin_Parameters

# Mupen doesn't like to have 2 buttons mapped for N64 pad entry. That's why r2 is commented for now. 1 axis and 1 button is ok
mupenHatToAxis = { 1: 'Up', 2: 'Right', 4: 'Down', 8: 'Left'}
mupenDoubleAxis = {0: 'X Axis', 1: 'Y Axis'}

def getMupenMappingFile() -> str:
    if os.path.exists(recalboxFiles.mupenMappingUser):
        return recalboxFiles.mupenMappingUser
    else:
        return recalboxFiles.mupenMappingSystem

def getMupenMapping() -> Dict[str, str]:
    dom = minidom.parse(getMupenMappingFile())
    dictio: Dict[str, str] = {}
    for inputs in dom.getElementsByTagName('inputList'):
        for inp in inputs.childNodes:
            if inp.attributes:
                if inp.attributes['name']:
                    if inp.attributes['value']:
                        dictio[inp.attributes['name'].value] = inp.attributes['value'].value
    return dictio

# Write a configuration for a specified controller
def writeControllersConfig(controllers: ControllerPerPlayer):
    # Do not load previous file
    padConfig = IniSettings(recalboxFiles.mupenInput)
    padConfig.loadFile(True) \
             .defineBool('True', 'False')

    for controller in controllers:
        player = controllers[controller]
        # Dynamic controller bindings
        config = defineControllerKeys(player)
        # Write to file
        writeToIni(player, config, padConfig)

    padConfig.saveFile()

def defineControllerKeys(controller: Controller) -> Dict[str, str]:
    mupenmapping: Dict[str, str] = getMupenMapping()

    # config holds the final pad configuration in the mupen style
    # ex: config['DPad U'] = "button(1)"
    config: Dict[str, str] = {'AnalogDeadzone': mupenmapping['AnalogDeadzone']}

    for item in controller.AvailableInput:
        if item.Name in mupenmapping and len(mupenmapping[item.Name]) != 0:
            value = setControllerLine(mupenmapping, item, mupenmapping[item.Name])
            # Handle multiple inputs for a single N64 Pad inp
            if mupenmapping[item.Name] not in config: config[mupenmapping[item.Name]] = value
            else:                                     config[mupenmapping[item.Name]] += ' ' + value

    # Big dirty hack : handle when the pad has no analog sticks. Only Start A, B L and R survive from the previous configuration
    if "X Axis" not in config and "Y Axis" not in config:
        # remap Z Trig
        config['Z Trig'] = setControllerLine(mupenmapping, controller.X, "Z Trig")
        # remove C Button U and R
        if 'C Button U' in config: del config['C Button U']
        if 'C Button R' in config: del config['C Button R']
        # remove DPad
        if 'DPad U' in config: del config['DPad U']
        if 'DPad D' in config: del config['DPad D']
        if 'DPad L' in config: del config['DPad L']
        if 'DPad R' in config: del config['DPad R']
        # Remap up/down/left/right to  X and Y Axis
        if controller.Left.IsHat:
            config['X Axis'] = "hat({} {} {})".format(controller.Left.Id, mupenHatToAxis[controller.Left.Value], mupenHatToAxis[controller.Right.Value])
            config['Y Axis'] = "hat({} {} {})".format(controller.Up.Id, mupenHatToAxis[controller.Up.Value], mupenHatToAxis[controller.Down.Value])
        elif controller.Left.IsAxis:
            config['X Axis'] = setControllerLine(mupenmapping, controller.Left, "X Axis")
            config['Y Axis'] = setControllerLine(mupenmapping, controller.Up, "Y Axis")
        elif controller.Left.IsButton:
            config['X Axis'] = "button({},{})".format(controller.Left.Id, controller.Right.Id)
            config['Y Axis'] = "button({},{})".format(controller.Up.Id, controller.Down.Id)
    return config


def setControllerLine(_, item: InputItem, mupenSettingName: str) -> str:
    value = ''
    if item.IsButton:
        value = "button({})".format(item.Id)
    elif item.IsHat:
        value = "hat({} {})".format(item.Id, mupenHatToAxis[item.Value])
    elif item.IsAxis:
        # Generic case for joystick1up and joystick1left
        if mupenSettingName in mupenDoubleAxis.values():
            # X axis : value = -1 for left, +1 for right
            # Y axis : value = -1 for up, +1 for down
            if item.Value < 0: value = "axis({}-,{}+)".format(item.Id, item.Id)
            else:              value = "axis({}+,{}-)".format(item.Id, item.Id)
        else:
            if item.Value > 0: value = "axis({}+)".format(item.Id)
            else:              value = "axis({}-)".format(item.Id)
    return value


def writeToIni(controller: Controller, config: Dict[str, str], padConfig: IniSettings):
    section = controller.DeviceName

    # Write static config
    padConfig.setBool(section, 'plugged', True)
    padConfig.setInt(section, 'plugin', 2)
    padConfig.setString(section, 'AnalogDeadzone', config['AnalogDeadzone'])
    padConfig.setString(section, 'AnalogPeak', "32768,32768")
    padConfig.setString(section, 'Mempak switch', "")
    padConfig.setString(section, 'Rumblepak switch', "")
    padConfig.setBool(section, 'mouse', False)
    # Config.set(section, 'name', controller.RealName)
    # Config.set(section, 'device', controller.index)

    # Write dynamic config
    for inputName in sorted(config):
        padConfig.setString(section, inputName, config[inputName])
