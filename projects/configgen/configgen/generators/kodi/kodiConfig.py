#!/usr/bin/env python

#import controllersConfig as controllers
import os
import configgen.recalboxFiles as recalboxFiles
from xml.dom import minidom
from xml.etree import ElementTree as ET

from configgen.controllersConfig import Controller, Input
from configgen.controllersConfig import ControllerDictionary

kodiMapping = {
    'a' :             {'button': 'b'},
    'b' :             {'button': 'a'},
    'x' :             {'button': 'y'},
    'y' :             {'button': 'x'},
    'start' :         {'button': 'start'},
    'select' :        {'button': 'back'},
    'hotkey' :        {'button': 'guide'},
    'l1' :            {'button': 'leftbumper'},
    'r1' :            {'button': 'rightbumper'},
    'l2' :            {'button': 'lefttrigger', 'axis': 'lefttrigger'},
    'r2' :            {'button': 'righttrigger', 'axis': 'righttrigger'},
    'l3' :            {'button': 'leftthumb'},
    'r3' :            {'button': 'rightthumb'},
    # The DPAD can be an axis (for gpio sticks for example) or a hat
    'up' :            {'hat': 'up',    'axis': 'up',    'button': 'up'},
    'down' :          {'hat': 'down',  'axis': 'down',  'button': 'down'},
    'left' :          {'hat': 'left',  'axis': 'left',  'button': 'left'},
    'right' :         {'hat': 'right', 'axis': 'right', 'button': 'right'},
    # They are empty because it's pointless to set something. But we need them to make sure they can be mapped
    'joystick1up' :   {},
    'joystick1left' : {},
    'joystick2up' :   {},
    'joystick2left' : {},
}

def getFormattedAxis(inpt: Input, controllerObj: Controller):
    direction = int(inpt.value)
    primary = '-' if direction < 0 else '+'
    secondary = '+' if direction < 0 else '-'
    axis = str(controllerObj.getAxisNumber(inpt))
    return primary + axis, secondary + axis

def writeKodiControllersConfig(controllers: ControllerDictionary):
    leftstick = rightstick = None
    if not os.path.isdir(recalboxFiles.kodiJoystick):
        os.makedirs(recalboxFiles.kodiJoystick)

    # For each controller
    for cidx, controllerObj in controllers.items():
        # Determine XML file name
        nbButtons = controllerObj.nbbuttons
        nbAxis = controllerObj.getTotalAxisNumber()
        xmlFileName = recalboxFiles.kodiJoystick + "/{}_{}b_{}a.xml".format(controllerObj.realName.strip().replace(' ', '_').replace(':', '_'), nbButtons, nbAxis)
        print(xmlFileName)

        buttonmap = ET.Element("buttonmap")
        device = ET.SubElement(buttonmap, "device", name=controllerObj.realName, provider="linux", buttoncount=str(nbButtons), axiscount=str(nbAxis))
        ET.SubElement(device, "configuration")
        controller = ET.SubElement(device, "controller", id="game.controller.default")

        # Loop through controller inputs
        inputs = controllerObj.inputs
        for iidx, inpt in inputs.items():
            # Skip unmapped inputs
            if not inpt.name in kodiMapping: continue

            if inpt.type == 'button':
                ET.SubElement(controller, "feature", name=kodiMapping[inpt.name][inpt.type], button=str(inpt.id))

            elif inpt.type in ("hat", "axis") and inpt.name in ('up', 'left', 'down', 'right'):
                direction = "-" if inpt.name in ('up', 'left') else "+"
                directionId = controllerObj.getAxisNumber(inpt)
                ET.SubElement(controller, "feature", name=kodiMapping[inpt.name][inpt.type], axis="{}{}".format(direction, directionId))

            # special case: sticks
            elif inpt.type == "axis":
                if inpt.name in ('joystick1left', 'joystick1up'):
                    if leftstick is None:
                        leftstick = ET.SubElement(controller, "feature", name="leftstick")
                    if inpt.name == 'joystick1up':
                        upaxis, downaxis = getFormattedAxis(inpt, controllerObj)
                        ET.SubElement(leftstick, "up", axis=upaxis)
                        ET.SubElement(leftstick, "down", axis=downaxis)
                    if inpt.name == 'joystick1left':
                        leftaxis, rightaxis = getFormattedAxis(inpt, controllerObj)
                        ET.SubElement(leftstick, "left", axis=leftaxis)
                        ET.SubElement(leftstick, "right", axis=rightaxis)

                elif inpt.name in ('joystick2left', 'joystick2up'):
                    if rightstick is None:
                        rightstick = ET.SubElement(controller, "feature", name="rightstick")
                    if inpt.name == 'joystick2up':
                        upaxis, downaxis = getFormattedAxis(inpt, controllerObj)
                        ET.SubElement(rightstick, "up", axis=upaxis)
                        ET.SubElement(rightstick, "down", axis=downaxis)
                    if inpt.name == 'joystick2left':
                        leftaxis, rightaxis = getFormattedAxis(inpt, controllerObj)
                        ET.SubElement(rightstick, "left", axis=leftaxis)
                        ET.SubElement(rightstick, "right", axis=rightaxis)
                else:
                    print("Unsupportted " + inpt.type + " " + inpt.name)
            else:
                print("Unsupportted " + inpt.type + " " + inpt.name)

        # Need to write this string to a file
        with open(xmlFileName, "w") as f:
            f.write(minidom.parseString(ET.tostring(buttonmap)).toprettyxml())
