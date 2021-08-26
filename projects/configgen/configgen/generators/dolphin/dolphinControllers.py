#!/usr/bin/env python
# -*- coding: utf-8 -*-
from typing import Dict, IO

import configgen.recalboxFiles as recalboxFiles
from configparser import ConfigParser

from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings

hotkeysCombo = {
    "b":      "Stop", ## to unify the unique combination of Recalbox ;) 
#    "b":      "Toggle Pause",
    "l1":     "Take Screenshot",
    "start":  "Exit",
    "a":      "Reset",
    "y":      "Save to selected slot",
    "x":      "Load from selected slot",
#    "r2":     "Stop",
    "r2":     "Toggle Pause",
    "up":     "Select State Slot 1",
    "down":   "Select State Slot 2",
    "left":   "Decrease Emulation Speed",
    "right":  "Increase Emulation Speed"
}

# Create the controller configuration file
def generateControllerConfig(system: Emulator, playersControllers: ControllerDictionary, recalboxSettings: keyValueSettings):
    generateHotkeys(playersControllers)
    if system.Name == "wii":
        realWiimotes: bool = recalboxSettings.getOption("global.realwiimotes", recalboxSettings.getOption(system.Name + ".realwiimotes", "0")) in ('1', 'true')
        if realWiimotes:
            generateControllerConfigEmulatedwiimotes(playersControllers, system)
        else:
            generateControllerConfigRealwiimotes("WiimoteNew.ini", "Wiimote")
    elif system.Name == "gamecube":
            generateControllerConfigGamecube(playersControllers, system)
    else:
        raise ValueError("Invalid system name : '" + system.Name + "'")

def generateControllerConfigEmulatedwiimotes(playersControllers: ControllerDictionary, system: Emulator):
    wiiMapping = {
        'a':           'Buttons/2',         'b':             'Buttons/A',
        'x':           'Buttons/1',         'y':             'Buttons/B',
        'l1':          'Nunchuk/Buttons/Z', 'r1':            'Nunchuk/Buttons/C',
        'select':      'Buttons/-',         'start':         'Buttons/+',
        'r2':          'Shake/Z',
        'joystick1up': 'Nunchuk/Stick/Up',  'joystick1left': 'Nunchuk/Stick/Left',
        'joystick2up': 'IR/Up',             'joystick2left': 'IR/Left',
        'up': 'D-Pad/Up', 'down': 'D-Pad/Down', 'left': 'D-Pad/Left', 'right': 'D-Pad/Right'
    }
    wiiReverseAxes = {
        'D-Pad/Up':   'D-Pad/Down',
        'D-Pad/Left': 'D-Pad/Right',
        'IR/Up':      'IR/Down',
        'IR/Left':    'IR/Right',
        'Swing/Up':   'Swing/Down',
        'Swing/Left': 'Swing/Right',
        'Nunchuk/Stick/Up': 'Nunchuk/Stick/Down',
        'Nunchuk/Stick/Left': 'Nunchuk/Stick/Right'
    }
    generateControllerConfigAny(playersControllers, "WiimoteNew.ini", "Wiimote", wiiMapping, wiiReverseAxes, system)

def generateControllerConfigGamecube(playersControllers: ControllerDictionary, system: Emulator):
    gamecubeMapping = {
        'a':      'Buttons/X',  'b':        'Buttons/A',
        'x':      'Buttons/Y',  'y':        'Buttons/B',
        'r2':     'Buttons/Z',  'start':    'Buttons/Start',
        'l1':     'Triggers/L', 'r1':       'Triggers/R',
        'up': 'D-Pad/Up', 'down': 'D-Pad/Down', 'left': 'D-Pad/Left', 'right': 'D-Pad/Right',
        'joystick1up': 'Main Stick/Up', 'joystick1left': 'Main Stick/Left',
        'joystick2up': 'C-Stick/Up',    'joystick2left': 'C-Stick/Left'
    }
    gamecubeReverseAxes = {
        'D-Pad/Up':        'D-Pad/Down',
        'D-Pad/Left':      'D-Pad/Right',
        'Main Stick/Up':   'Main Stick/Down',
        'Main Stick/Left': 'Main Stick/Right',
        'C-Stick/Up':      'C-Stick/Down',
        'C-Stick/Left':    'C-Stick/Right'
    }
    generateControllerConfigAny(playersControllers, "GCPadNew.ini", "GCPad", gamecubeMapping, gamecubeReverseAxes, system)

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

def generateControllerConfigAny(playersControllers: ControllerDictionary, filename: str, anyDefKey: str, anyMapping: Dict[str, str], anyReverseAxes: Dict[str, str], system: Emulator):
    configFileName = "{}/{}".format(recalboxFiles.dolphinConfig, filename)
    f = open(configFileName, "w")
    nplayer = 1

    # in case of two pads having the same name, dolphin wants a number to handle this
    double_pads = dict()

    for playercontroller in playersControllers:
        # handle x pads having the same name
        pad = playersControllers[playercontroller]
        if pad.configName in double_pads:
            nsamepad = double_pads[pad.configName]
        else:
            nsamepad = 0
        double_pads[pad.configName] = nsamepad+1

        f.write("[" + anyDefKey + str(nplayer) + "]" + "\n")
        f.write('Device = "evdev/' + str(nsamepad) + '/' + pad.configName + '"\n')

        if system.Name == "wii":
            f.write("Extension = Nunchuk" + "\n")
            f.write("IR/Center = 15.000000000000000" + "\n")
            f.write("IR/Height = 85.000000000000000" + "\n")
            f.write("Nunchuk/Stick/Dead Zone = 25.000000000000000" + "\n")
        elif system.Name == "gamecube":
            f.write("Main Stick/Dead Zone = 25.000000000000000" + "\n")
            f.write("C-Stick/Dead Zone = 25.000000000000000" + "\n")

        for x in pad.inputs:
            inp = pad.inputs[x]

            keyname = None
            if inp.name in anyMapping:
                keyname = anyMapping[inp.name]
            #else:
            #    f.write("# undefined key: name="+input.name+", type="+input.type+", id="+str(input.id)+", value="+str(input.value)+"\n")

            # write the configuration for this key
            if keyname is not None:
                writeKey(f, keyname, inp.type, inp.id, inp.value, pad.nbaxes, False)
            # write the 2nd part
            if inp.name in { "joystick1up", "joystick1left", "joystick2up", "joystick2left"} and keyname is not None:
                writeKey(f, anyReverseAxes[keyname], inp.type, inp.id, inp.value, pad.nbaxes, True)

        nplayer += 1
    f.close()

def writeKey(f: IO, keyname: str, inputType: str, inputId: str, inputValue: str, inputGlobalId: str, reverse: bool):
    f.write(keyname + " = `")
    if inputType == "button":
        f.write("Button " + str(inputId))
    elif inputType == "hat":
        if inputValue == "1" or inputValue == "4": # up or down
            f.write("Axis " + str(int(inputGlobalId) + 1))
        else:
            f.write("Axis " + str(inputGlobalId))
        if inputValue == "1" or inputValue == "8": # up or left
            f.write("-")
        else:
            f.write("+")
    elif inputType == "axis":
        if (reverse and inputValue == "-1") or (not reverse and inputValue == "1"):
            f.write("Axis " + str(inputId) + "+")
        else:
            f.write("Axis " + str(inputId) + "-")
    f.write("`\n")


def generateHotkeys(playersControllers: ControllerDictionary):
    player1 = None
    HK = None
    iniValues = dict()

    # Find player 1
    for idx, playerController in playersControllers.items():
        if playerController.player == "1":
            print("P1 found")
            player1 = playerController
            break

    if player1 is None:
        print("no P1")
        raise ValueError("Couldn't find Player 1 input config")

    # Read its inputs, get the hotkey
    for idx, inp in player1.inputs.items():
        if inp.name == "hotkey":
            HK = inp.id
            break

    if HK is None:
        print("no HK")
        raise ValueError("Couldn't find Player 1 hotkey")

    # Now generate the hotkeys
    for idx, inputobj in player1.inputs.items():
        if inputobj.name in hotkeysCombo:
            propertyName = "Keys/{}".format(hotkeysCombo[inputobj.name])
            print(propertyName)
            propertyValue = "`Button {}` & `Button {}`".format(HK, inputobj.id)
            iniValues[propertyName] = propertyValue
    iniValues["Device"] = '"evdev/0/{}"'.format(player1.realName)
    # Prepare the ini write
    iniSections = {"Hotkeys1": iniValues}
    writeIniFile(recalboxFiles.dolphinHKeys, iniSections)


def writeIniFile(filename: str, sectionsAndValues: Dict[str, Dict[str, str]]):
    # filename: file to write
    # sectionsAndValues: a dict indexed on sections on the ini. Each section has a dict of propertyName: propertyValue

    Config = ConfigParser()
    # To prevent ConfigParser from converting to lower case
    Config.optionxform = str

    # Write dynamic config
    for section, values in sectionsAndValues.items():
        Config.add_section(section)
        for propertyName, propertyValue in values.items():
            Config.set(section, propertyName, propertyValue)

    # Open file
    cfgfile = open(filename, 'w')
    Config.write(cfgfile)
    cfgfile.close()
