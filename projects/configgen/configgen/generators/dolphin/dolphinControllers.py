#!/usr/bin/env python
# -*- coding: utf-8 -*-

import recalboxFiles
import ConfigParser

hotkeysCombo = {
    "b":      "Toggle Pause",
    "pageup": "Take Screenshot",
    "start":  "Exit",
    "a":      "Reset",
    "y":      "Save to selected slot",
    "x":      "Load from selected slot",
    "r2":     "Stop",
    "up":     "Select State Slot 1",
    "down":   "Select State Slot 2",
    "left":   "Decrease Emulation Speed",
    "right":  "Increase Emulation Speed"
}

# Create the controller configuration file
def generateControllerConfig(system, playersControllers):
    generateHotkeys(playersControllers)
    if system.name == "wii":
        if 'emulatedwiimotes' in system.config and system.config['emulatedwiimotes'] == '1':
            generateControllerConfig_emulatedwiimotes(playersControllers, system)
        else:
            generateControllerConfig_realwiimotes("WiimoteNew.ini", "Wiimote")
    elif system.name == "gamecube":
        generateControllerConfig_gamecube(playersControllers, system)
    else:
        raise ValueError("Invalid system name : '" + system.name + "'")

def generateControllerConfig_emulatedwiimotes(playersControllers, system):
    wiiMapping = {
        'a':           'Buttons/2',         'b':             'Buttons/A',
        'x':           'Buttons/1',         'y':             'Buttons/B',
        'pageup':      'Nunchuk/Buttons/Z', 'pagedown':      'Nunchuk/Buttons/C',
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
    generateControllerConfig_any(playersControllers, "WiimoteNew.ini", "Wiimote", wiiMapping, wiiReverseAxes, system)

def generateControllerConfig_gamecube(playersControllers, system):
    gamecubeMapping = {
        'a':      'Buttons/X',  'b':        'Buttons/A',
        'x':      'Buttons/Y',  'y':        'Buttons/B',
        'r2':     'Buttons/Z',  'start':    'Buttons/Start',
        'pageup': 'Triggers/L', 'pagedown': 'Triggers/R',
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
    generateControllerConfig_any(playersControllers, "GCPadNew.ini", "GCPad", gamecubeMapping, gamecubeReverseAxes, system)

def generateControllerConfig_realwiimotes(filename, anyDefKey):
    configFileName = "{}/{}".format(recalboxFiles.dolphinConfig, filename)
    f = open(configFileName, "w")
    nplayer = 1
    while nplayer <= 4:
        f.write("[" + anyDefKey + str(nplayer) + "]" + "\n")
        f.write("Source = 2\n")
        nplayer += 1
    f.write
    f.close()

def generateControllerConfig_any(playersControllers, filename, anyDefKey, anyMapping, anyReverseAxes, system):
    configFileName = "{}/{}".format(recalboxFiles.dolphinConfig, filename)
    f = open(configFileName, "w")
    nplayer = 1
    nsamepad = 0

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

        if system.name == "wii":
            f.write("Extension = Nunchuk" + "\n")
            f.write("IR/Center = 15.000000000000000" + "\n")
            f.write("IR/Height = 85.000000000000000" + "\n")
            f.write("Nunchuk/Stick/Dead Zone = 25.000000000000000" + "\n")
        elif system.name == "gamecube":
            f.write("Main Stick/Dead Zone = 25.000000000000000" + "\n")
            f.write("C-Stick/Dead Zone = 25.000000000000000" + "\n")

        for x in pad.inputs:
            input = pad.inputs[x]

            keyname = None
            if input.name in anyMapping:
                keyname = anyMapping[input.name]
            #else:
            #    f.write("# undefined key: name="+input.name+", type="+input.type+", id="+str(input.id)+", value="+str(input.value)+"\n")

            # write the configuration for this key
            if keyname is not None:
                write_key(f, keyname, input.type, input.id, input.value, pad.nbaxes, False)
            # write the 2nd part
            if input.name in { "joystick1up", "joystick1left", "joystick2up", "joystick2left"} and keyname is not None:
                write_key(f, anyReverseAxes[keyname], input.type, input.id, input.value, pad.nbaxes, True)

        nplayer += 1
    f.write
    f.close()

def write_key(f, keyname, input_type, input_id, input_value, input_global_id, reverse):
    f.write(keyname + " = `")
    if input_type == "button":
        f.write("Button " + str(input_id))
    elif input_type == "hat":
        if input_value == "1" or input_value == "4": # up or down
            f.write("Axis " + str(int(input_global_id)+1))
        else:
            f.write("Axis " + str(input_global_id))
        if input_value == "1" or input_value == "8": # up or left
            f.write("-")
        else:
            f.write("+")
    elif input_type == "axis":
        if (reverse and input_value == "-1") or (not reverse and input_value == "1"):
            f.write("Axis " + str(input_id) + "+")
        else:
            f.write("Axis " + str(input_id) + "-")
    f.write("`\n")


def generateHotkeys(playersControllers):
    player1 = None
    HK = None
    iniValues = dict()

    # Find player 1
    for idx, playerController in playersControllers.iteritems():
        if playerController.player == "1":
            print "P1 found"
            player1 = playerController
            break

    if player1 is None:
        print "no P1"
        raise ValueError("Couldn't find Player 1 input config")

    # Read its inputs, get the hotkey
    for idx, input in player1.inputs.iteritems():
        if input.name == "hotkey":
            HK = input.id
            break

    if HK is None:
        print "no HK"
        raise ValueError("Couldn't find Player 1 hotkey")

    # Now generate the hotkeys
    for idx, inputobj in player1.inputs.iteritems():
        if inputobj.name in hotkeysCombo:
            propertyName = "Keys/{}".format(hotkeysCombo[inputobj.name])
            print propertyName
            propertyValue = "`Button {}` & `Button {}`".format(HK, inputobj.id)
            iniValues[propertyName] = propertyValue
    iniValues["Device"] = '"evdev/0/{}"'.format(player1.realName)
    # Prepare the ini write
    iniSections = {"Hotkeys1": iniValues }
    writeIniFile(recalboxFiles.dolphinHKeys, iniSections)


def writeIniFile(filename, sectionsAndValues):
    # filename: file to write
    # sectionsAndValues: a dict indexed on sections on the ini. Each section has a dict of propertyName: propertyValue

    Config = ConfigParser.ConfigParser()
    # To prevent ConfigParser from converting to lower case
    Config.optionxform = str


    # Write dynamic config
    for section, values in sectionsAndValues.iteritems():
        Config.add_section(section)
        for propertyName, propertyValue in values.iteritems():
            Config.set(section, propertyName, propertyValue)

    # Open file
    cfgfile = open(filename,'w')
    Config.write(cfgfile)
    cfgfile.close()
