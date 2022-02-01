#!/usr/bin/env python
from typing import Dict

from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import Controller, ControllerPerPlayer
from configgen.settings.iniSettings import IniSettings

# Map an emulationstation button name to the corresponding fba2x name
fba4bnts: Dict[int, str] = \
{
    InputItem.ItemA: 'Y',
    InputItem.ItemB: 'X',
    InputItem.ItemX: 'B',
    InputItem.ItemY: 'A',
    InputItem.ItemL1: 'L',
    InputItem.ItemR1: 'R',
}

fba6bnts: Dict[int, str] = \
{
    InputItem.ItemA: 'L',
    InputItem.ItemB: 'Y',
    InputItem.ItemX: 'X',
    InputItem.ItemY: 'A',
    InputItem.ItemL1: 'B',
    InputItem.ItemR1: 'R',
}

sectionJoystick = 'Joystick'

def writeControllersConfig(_, rom: str, controllers: ControllerPerPlayer, fbaSettings: IniSettings):
    writeIndexes(controllers, fbaSettings)
    sixBtnConfig: bool = is6btn(rom)
    for controller in controllers:
        playerConfig: Dict[str, int] = generateControllerConfig(controller, controllers[controller], sixBtnConfig)
        for inp in playerConfig:
            fbaSettings.setInt(sectionJoystick, inp, playerConfig[inp])

# Create a configuration file for a given controller
def generateControllerConfig(player: int, controller: Controller, special6: bool) -> Dict[str, int]:
    config: Dict[str, int] = {}
    fbaBtns: Dict[int, str] = fba6bnts if special6 else fba4bnts

    # DPAD
    if controller.HasUp    : config['UP_{}'.format(player)] = controller.Up.Id
    if controller.HasDown  : config['DOWN_{}'.format(player)] = controller.Down.Id
    if controller.HasLeft  : config['LEFT_{}'.format(player)] = controller.Left.Id
    if controller.HasRight : config['RIGHT_{}'.format(player)] = controller.Right.Id

    # Axis
    if controller.HasJoy1Up   : config['JA_UD_{}'.format(player)] = controller.Joy1Up.Id
    if controller.HasJoy1Left : config['JA_LR_{}'.format(player)] = controller.Joy1Left.Id

    # Start & select
    if controller.HasStart : config['START_{}'.format(player)] = controller.Start.Id
    if controller.HasSelect: config['SELECT_{}'.format(player)] = controller.Select.Id

    # Hotcket & quit for player 1
    if player == 1:
        if controller.HasHotkey: config['HOTKEY'] = controller.Hotkey.Id
        if controller.HasStart : config['QUIT'] = controller.Start.Id

    # Buttons
    for item, value in fbaBtns.items():
        if controller.HasInput(item):
            inputItem: InputItem = controller.Input(item)
            if inputItem.IsButton:
                config['{}_{}'.format(value, player)] = inputItem.Id


    return config


def writeIndexes(controllers, fbaSettings: IniSettings):
    for player in range(1, 5):
        fbaSettings.setInt(sectionJoystick, 'SDLID_{}'.format(player), -1)
    for player in controllers:
        controller = controllers[player]
        fbaSettings.setInt(sectionJoystick, 'SDLID_{}'.format(player), controller.SdlIndex)


sixBtnGames = ['sfa', 'sfz', 'sf2', 'dstlk', 'hsf2', 'msh', 'mshvsf', 'mvsc', 'nwarr', 'ssf2', 'vsav', 'vhunt', 'xmvsf',
               'xmcota']

def is6btn(rom) -> bool:
    import os
    rom = os.path.basename(rom)
    for game in sixBtnGames:
        if game in rom:
            return True
    return False
