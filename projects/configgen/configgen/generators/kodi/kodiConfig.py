#!/usr/bin/env python

import controllersConfig as controllers
import signal
import os
import recalboxFiles
from xml.dom import minidom
from xml.etree import ElementTree as ET

kodiMapping = {
    'a' :             {'button': 'b'},
    'b' :             {'button': 'a'},
    'x' :             {'button': 'y'},
    'y' :             {'button': 'x'},
    'start' :         {'button': 'start'},
    'select' :        {'button': 'back'},
    'hotkey' :        {'button': 'guide'},
    'pageup' :        {'button': 'leftbumper'},
    'pagedown' :      {'button': 'rightbumper'},
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


def writeKodiControllersConfig(controllers):
    leftstick = rightstick = None
    if not os.path.isdir(recalboxFiles.kodiJoystick):
        os.makedirs(recalboxFiles.kodiJoystick)
    # For each controller
    for cidx, controllerObj in controllers.iteritems():
	# Determine XML file name
	nbButtons = controllerObj.nbbuttons
	nbAxis = controllerObj.getTotalAxisNumber()
	xmlFileName = recalboxFiles.kodiJoystick + "/{}_{}b_{}a.xml".format(controllerObj.realName.strip().replace(' ', '_'), nbButtons, nbAxis)
	print xmlFileName
	buttonmap = ET.Element("buttonmap")
	device = ET.SubElement(buttonmap, "device"
	    , name=controllerObj.realName
	    , provider="linux"
	    , buttoncount=str(nbButtons)
	    , axiscount=str(nbAxis)
	)
	ET.SubElement(device, "configuration")
	controller = ET.SubElement(device, "controller", id="game.controller.default")
	# Loop through controller inputs
	inputs = controllerObj.inputs
	for iidx, input in inputs.iteritems():
	    # Skip unmapped inputs
	    if not input.name in kodiMapping: continue
	    if input.type == 'button':
		ET.SubElement(controller, "feature"
		    , name=kodiMapping[input.name][input.type]
		    , button=str(input.id)
		)
	    elif input.type == "hat":
		if input.name in ['up', 'left']:
		    direction = "-"
		else:
		    direction = "+"
		directionId = controllerObj.getAxisNumber(input)
		ET.SubElement(controller, "feature"
		    , name=kodiMapping[input.name][input.type]
		    , axis="{}{}".format(direction, directionId)
		)
	    # special case: sticks
	    elif input.type == "axis":
                if input.name in ['joystick1left', 'joystick1up']:
                    if leftstick is None:
                        leftstick = ET.SubElement(controller, "feature", name="leftstick")
                    if input.name == 'joystick1up':
                        if input.value == "-1":
                            upaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                            downaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                        else:
                            upaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                            downaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                        ET.SubElement(leftstick, "up", axis=upaxis)
                        ET.SubElement(leftstick, "down", axis=downaxis)
                    if input.name == 'joystick1left':
                        if input.value == "-1":
                            leftaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                            rightaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                        else:
                            leftaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                            rightaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                        ET.SubElement(leftstick, "left", axis=leftaxis)
                        ET.SubElement(leftstick, "right", axis=rightaxis)
                elif input.name in ['joystick2left', 'joystick2up']:
                    if rightstick is None:
                        rightstick = ET.SubElement(controller, "feature", name="rightstick")
                    if input.name == 'joystick2up':
                        if input.value == "-1":
                            upaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                            downaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                        else:
                            upaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                            downaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                        ET.SubElement(rightstick, "up", axis=upaxis)
                        ET.SubElement(rightstick, "down", axis=downaxis)
                    if input.name == 'joystick2left':
                        if input.value == "-1":
                            leftaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                            rightaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                        else:
                            leftaxis = "{}{}".format('+', controllerObj.getAxisNumber(input))
                            rightaxis = "{}{}".format('-', controllerObj.getAxisNumber(input))
                        ET.SubElement(rightstick, "left", axis=leftaxis)
                        ET.SubElement(rightstick, "right", axis=rightaxis)
                else:
                    print "Unsupportted " + input.type + " " + input.name
            else:
                print "Unsupportted " + input.type + " " + input.name
	# Need to write this string to a file
        with open(xmlFileName, "w") as f:
            f.write(minidom.parseString(ET.tostring(buttonmap)).toprettyxml())
