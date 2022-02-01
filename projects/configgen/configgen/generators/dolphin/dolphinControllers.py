#!/usr/bin/env python
from typing import Dict, IO

import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllers.inputItem import InputItem
from configgen.controllers.controller import ControllerPerPlayer
from configgen.settings.keyValueSettings import keyValueSettings

hotkeysCombo: Dict[int, str] =\
{
    InputItem.ItemB:      "Stop", ## to unify the unique combination of Recalbox ;)
#    "b":      "Toggle Pause",
    InputItem.ItemL1:     "Take Screenshot",
    InputItem.ItemStart:  "Exit",
    InputItem.ItemA:      "Reset",
    InputItem.ItemY:      "Save to selected slot",
    InputItem.ItemX:      "Load from selected slot",
#    "r2":     "Stop",
    InputItem.ItemR2:     "Toggle Pause",
    InputItem.ItemUp:     "Select State Slot 1",
    InputItem.ItemDown:   "Select State Slot 2",
    InputItem.ItemLeft:   "Decrease Emulation Speed",
    InputItem.ItemRight:  "Increase Emulation Speed"
}

# Create the controller configuration file
def generateControllerConfig(system: Emulator, playersControllers: ControllerPerPlayer, recalboxOptions: keyValueSettings):
    generateHotkeys(playersControllers)
    if system.Name == "wii":
        realWiimotes: bool = recalboxOptions.getBool("global.realwiimotes", recalboxOptions.getBool("wii.realwiimotes", False))
        if realWiimotes:
            generateControllerConfigRealwiimotes("WiimoteNew.ini", "Wiimote")
        else:
            generateControllerConfigEmulatedwiimotes(playersControllers, system)
    elif system.Name == "gamecube":
            generateControllerConfigGamecube(playersControllers, system)
    else:
        raise ValueError("Invalid system name : '" + system.Name + "'")

def generateControllerConfigEmulatedwiimotes(playersControllers: ControllerPerPlayer, system: Emulator):

    wiiMapping: Dict[int, str] = \
    {
        InputItem.ItemA:           'Buttons/2',
        InputItem.ItemB:           'Buttons/A',
        InputItem.ItemX:           'Buttons/1',
        InputItem.ItemY:           'Buttons/B',
        InputItem.ItemL1:          'Nunchuk/Buttons/Z',
        InputItem.ItemR1:          'Nunchuk/Buttons/C',
        InputItem.ItemSelect:      'Buttons/-',
        InputItem.ItemStart:       'Buttons/+',
        InputItem.ItemR2:          'Shake/Z',
        InputItem.ItemJoy1Up:      'Nunchuk/Stick/Up',
        InputItem.ItemJoy1Left:    'Nunchuk/Stick/Left',
        InputItem.ItemJoy1Down:    'Nunchuk/Stick/Down',
        InputItem.ItemJoy1Right:   'Nunchuk/Stick/Right',
        InputItem.ItemJoy2Up:      'IR/Up',
        InputItem.ItemJoy2Left:    'IR/Left',
        InputItem.ItemJoy2Down:    'IR/Down',
        InputItem.ItemJoy2Right:   'IR/Right',
        InputItem.ItemUp:          'D-Pad/Up',
        InputItem.ItemDown:        'D-Pad/Down',
        InputItem.ItemLeft:        'D-Pad/Left',
        InputItem.ItemRight:       'D-Pad/Right'
    }

    generateControllerConfigAny(playersControllers, "WiimoteNew.ini", "Wiimote", wiiMapping, system)

def generateControllerConfigGamecube(playersControllers: ControllerPerPlayer, system: Emulator):

    gamecubeMapping: Dict[int, str] = \
    {
        InputItem.ItemA:        'Buttons/X',
        InputItem.ItemB:        'Buttons/A',
        InputItem.ItemX:        'Buttons/Y',
        InputItem.ItemY:        'Buttons/B',
        InputItem.ItemR2:       'Buttons/Z',
        InputItem.ItemStart:    'Buttons/Start',
        InputItem.ItemL1:       'Triggers/L',
        InputItem.ItemR1:       'Triggers/R',
        InputItem.ItemUp:       'D-Pad/Up',
        InputItem.ItemDown:     'D-Pad/Down',
        InputItem.ItemLeft:     'D-Pad/Left',
        InputItem.ItemRight:    'D-Pad/Right',
        InputItem.ItemJoy1Up:   'Main Stick/Up',
        InputItem.ItemJoy1Left: 'Main Stick/Left',
        InputItem.ItemJoy1Down: 'Main Stick/Down',
        InputItem.ItemJoy1Right:'Main Stick/Right',
        InputItem.ItemJoy2Up:   'C-Stick/Up',
        InputItem.ItemJoy2Left: 'C-Stick/Left',
        InputItem.ItemJoy2Down: 'C-Stick/Down',
        InputItem.ItemJoy2Right:'C-Stick/Right'
    }

    generateControllerConfigAny(playersControllers, "GCPadNew.ini", "GCPad", gamecubeMapping, system)

def generateControllerConfigRealwiimotes(filename: str, anyDefKey: str):
    configFileName = "{}/{}".format(recalboxFiles.dolphinConfig, filename)
    f = open(configFileName, "w")
    nplayer = 1
    while nplayer <= 4:
        f.write("[" + anyDefKey + str(nplayer) + "]" + "\n")
        f.write("Source = 2\n")
        nplayer += 1
    #f.write
    f.close()

def EvdevGetJoystickName(path: str) -> str:
    import fcntl
    with open(path) as fd:
        return fcntl.ioctl(fd, 0x80804506, bytes(128)).decode('utf-8').rstrip().replace('\x00', '')

def generateControllerConfigAny(playersControllers: ControllerPerPlayer, filename: str, anyDefKey: str, anyMapping: Dict[int, str], system: Emulator):
    configFileName = "{}/{}".format(recalboxFiles.dolphinConfig, filename)
    f = open(configFileName, "w")
    nplayer = 1

    # in case of two pads having the same name, dolphin wants a number to handle this
    double_pads = dict()

    for playercontroller in playersControllers:
        # handle x pads having the same name
        pad = playersControllers[playercontroller]
        if pad.DeviceName in double_pads:
            nsamepad = double_pads[pad.DeviceName]
        else:
            nsamepad = 0
        double_pads[pad.DeviceName] = nsamepad + 1

        f.write("[" + anyDefKey + str(nplayer) + "]" + "\n")
        f.write('Device = "evdev/' + str(nsamepad) + '/' + EvdevGetJoystickName(pad.DevicePath) + '"\n')

        if system.Name == "wii":
            f.write("Extension = Nunchuk" + "\n")
            f.write("IR/Center = 15.000000000000000" + "\n")
            f.write("IR/Height = 85.000000000000000" + "\n")
            f.write("Nunchuk/Stick/Dead Zone = 25.000000000000000" + "\n")
        elif system.Name == "gamecube":
            f.write("Main Stick/Dead Zone = 25.000000000000000" + "\n")
            f.write("C-Stick/Dead Zone = 25.000000000000000" + "\n")

        for inp in pad.AvailableInput:

            if inp.Item in anyMapping:
                keyname = anyMapping[inp.Item]
                # write the configuration for this key
                writeKey(f, keyname, inp, pad.AxisCount)
                # write the 2nd part
                #if inp.IsAnalogJoystick:
                #    writeKey(f, anyReverseAxes[keyname], inp, pad.AxisCount, True)

        nplayer += 1
    f.close()

def writeKey(f: IO, keyname: str, inputItem: InputItem, inputGlobalId: int):
    f.write(keyname + " = `")
    if inputItem.IsButton:
        f.write("Button " + str(inputItem.Id))
    elif inputItem.IsHat:
        f.write("Axis " + str(inputGlobalId + (1 if inputItem.Value in (1, 4) else 0)) + ('-' if inputItem.Value in (1, 8) else '+'))
    elif inputItem.IsAxis:
        f.write("Axis " + str(inputItem.Id) + ('+' if inputItem.Value > 0 else '-'))
    f.write("`\n")


def generateHotkeys(playersControllers: ControllerPerPlayer):
    player1 = None
    iniValues: Dict[str, str] = {}

    # Find player 1
    for idx, playerController in playersControllers.items():
        if playerController.PlayerIndex == 1:
            print("P1 found")
            player1 = playerController
            break

    if player1 is None:
        print("no P1")
        raise ValueError("Couldn't find Player 1 input config")

    # Read its inputs, get the hotkey
    HK: int = player1.Hotkey.Id if player1.HasHotkey else -1

    if HK < 0:
        print("no HK")
        raise ValueError("Couldn't find Player 1 hotkey")

    # Now generate the hotkeys
    for inputItem in player1.AvailableInput:
        if inputItem.Item in hotkeysCombo:
            propertyName = "Keys/{}".format(hotkeysCombo[inputItem.Item])
            print(propertyName)
            propertyValue = "`Button {}` & `Button {}`".format(HK, inputItem.Id)
            iniValues[propertyName] = propertyValue
    iniValues["Device"] = '"evdev/0/{}"'.format(player1.DeviceName)
    # Prepare the ini write
    iniSections = { "Hotkeys1": iniValues }
    writeIniFile(recalboxFiles.dolphinHKeys, iniSections)


def writeIniFile(filename: str, sectionsAndValues: Dict[str, Dict[str, str]]):
    # filename: file to write
    # sectionsAndValues: a dict indexed on sections on the ini. Each section has a dict of propertyName: propertyValue
    from configgen.settings.iniSettings import IniSettings
    config = IniSettings(filename, True)

    # Write dynamic config
    for section, values in sectionsAndValues.items():
        for propertyName, propertyValue in values.items():
            config.setString(section, propertyName, propertyValue)

    # Open file
    config.saveFile()
