#!/usr/bin/env python
import os
from typing import Dict

from configgen.controllersConfig import Controller
from configgen.controllersConfig import ControllerDictionary
from configgen.settings.iniSettings import IniSettings

# Map an emulationstation button name to the corresponding fba2x name
fba4bnts = {'a': 'Y', 'b': 'X', 'x': 'B', 'y': 'A',
            'l1': 'L', 'r1': 'R',
            'start': 'START', 'select': 'SELECT'}
fba6bnts = {'a': 'L', 'b': 'Y', 'x': 'X', 'y': 'A',
            'l1': 'B', 'r1': 'R',
            'start': 'START', 'select': 'SELECT'}

# Map an emulationstation direction to the corresponding fba2x
fbadirs = {'up': 'UP', 'down': 'DOWN', 'left': 'LEFT', 'right': 'RIGHT'}
fbaaxis = {'joystick1up': 'JA_UD', 'joystick1left': 'JA_LR'}
fbaHatToAxis = {'1': 'UP', '2': 'LR', '4': 'UD', '8': 'LR'}

# Map buttons to the corresponding fba2x specials keys
fbaspecials = {'start': 'QUIT', 'hotkey': 'HOTKEY'}

sectionJoystick = '[Joystick]'

def writeControllersConfig(_, rom: str, controllers: ControllerDictionary, fbaSettings: IniSettings):
    writeIndexes(controllers, fbaSettings)
    sixBtnConfig: bool = is6btn(rom)
    for controller in controllers:
        playerConfig: Dict[str, str] = generateControllerConfig(controller, controllers[controller], sixBtnConfig)
        for inp in playerConfig:
            fbaSettings.setOption(sectionJoystick, inp, playerConfig[inp])

# Create a configuration file for a given controller
def generateControllerConfig(player: str, controller: Controller, special6: bool) -> Dict[str, str]:
    config = dict()
    fbaBtns = fba4bnts
    if special6:
        fbaBtns = fba6bnts

    for dirkey in fbadirs:
        dirvalue = fbadirs[dirkey]
        if dirkey in controller.inputs:
            inp = controller.inputs[dirkey]
            if inp.type == 'button':
                config['{}_{}'.format(dirvalue, player)] = inp.id

    for axis in fbaaxis:
        axisvalue = fbaaxis[axis]
        if axis in controller.inputs:
            inp = controller.inputs[axis]
            config['{}_{}'.format(axisvalue, player)] = inp.id

    for btnkey in fbaBtns:
        btnvalue = fbaBtns[btnkey]
        if btnkey in controller.inputs:
            inp = controller.inputs[btnkey]
            config['{}_{}'.format(btnvalue, player)] = inp.id

    if player == '1':
        for btnkey in fbaspecials:
            btnvalue = fbaspecials[btnkey]
            if btnkey in controller.inputs:
                inp = controller.inputs[btnkey]
                config['{}'.format(btnvalue)] = inp.id

    return config


def writeIndexes(controllers, fbaSettings: IniSettings):
    for player in range(1, 5):
        fbaSettings.setOption(sectionJoystick, 'SDLID_{}'.format(player), '-1')
    for player in controllers:
        controller = controllers[player]
        fbaSettings.setOption(sectionJoystick, 'SDLID_{}'.format(player), controller.index)


sixBtnGames = ['sfa', 'sfz', 'sf2', 'dstlk', 'hsf2', 'msh', 'mshvsf', 'mvsc', 'nwarr', 'ssf2', 'vsav', 'vhunt', 'xmvsf',
               'xmcota']

def is6btn(rom) -> bool:
    rom = os.path.basename(rom)
    for game in sixBtnGames:
        if game in rom:
            return True
    return False
