#!/usr/bin/env python
import configgen.recalboxFiles as recalboxFiles

from configgen.controllers.controller import Controller, InputItem, ControllerPerPlayer

kodiMapping = {
    InputItem.ItemA :        { InputItem.TypeButton: 'b' },
    InputItem.ItemB :        { InputItem.TypeButton: 'a' },
    InputItem.ItemX :        { InputItem.TypeButton: 'y' },
    InputItem.ItemY :        { InputItem.TypeButton: 'x' },
    InputItem.ItemStart :    { InputItem.TypeButton: 'start' },
    InputItem.ItemSelect :   { InputItem.TypeButton: 'back' },
    InputItem.ItemHotkey :   { InputItem.TypeButton: 'guide' },
    InputItem.ItemL1 :       { InputItem.TypeButton: 'leftbumper' },
    InputItem.ItemR1 :       { InputItem.TypeButton: 'rightbumper' },
    InputItem.ItemL2 :       { InputItem.TypeButton: 'lefttrigger',  InputItem.TypeAxis: 'lefttrigger' },
    InputItem.ItemR2 :       { InputItem.TypeButton: 'righttrigger', InputItem.TypeAxis: 'righttrigger' },
    InputItem.ItemL3 :       { InputItem.TypeButton: 'leftthumb' },
    InputItem.ItemR3 :       { InputItem.TypeButton: 'rightthumb' },
    # The DPAD can be an axis (for gpio sticks for example) or a hat
    InputItem.ItemUp :       { InputItem.TypeHat: 'up',    InputItem.TypeAxis: 'up',    InputItem.TypeButton: 'up' },
    InputItem.ItemDown :     { InputItem.TypeHat: 'down',  InputItem.TypeAxis: 'down',  InputItem.TypeButton: 'down' },
    InputItem.ItemLeft :     { InputItem.TypeHat: 'left',  InputItem.TypeAxis: 'left',  InputItem.TypeButton: 'left' },
    InputItem.ItemRight :    { InputItem.TypeHat: 'right', InputItem.TypeAxis: 'right', InputItem.TypeButton: 'right' },
    # They are empty because it's pointless to set something. But we need them to make sure they can be mapped
    InputItem.ItemJoy1Up :   { },
    InputItem.ItemJoy1Left : { },
    InputItem.ItemJoy2Up :   { },
    InputItem.ItemJoy2Left : { },
}

def getFormattedAxis(inpt: InputItem, controller: Controller):
    direction = int(inpt.Value)
    primary = '-' if direction < 0 else '+'
    secondary = '+' if direction < 0 else '-'
    axis = str(controller.getAxisNumber(inpt))
    return primary + axis, secondary + axis

def writeKodiControllersConfig(controllers: ControllerPerPlayer):
    leftstick = rightstick = None
    import os
    if not os.path.isdir(recalboxFiles.kodiJoystick):
        os.makedirs(recalboxFiles.kodiJoystick)

    # For each controller
    for cidx, controller in controllers.items():
        # Determine XML file name
        nbButtons = controller.ButtonCount
        nbAxis = controller.getTotalAxisNumber()
        xmlFileName = recalboxFiles.kodiJoystick + "/{}_{}b_{}a.xml".format(controller.DeviceName.strip().replace(' ', '_').replace(':', '_'), nbButtons, nbAxis)
        print(xmlFileName)

        from xml.etree import ElementTree as ET
        buttonmap = ET.Element("buttonmap")
        device = ET.SubElement(buttonmap, "device", name=controller.DeviceName, provider="linux", buttoncount=str(nbButtons), axiscount=str(nbAxis))
        ET.SubElement(device, "configuration")
        controllerXml = ET.SubElement(device, "controller", id="game.controller.default")

        # Loop through controller inputs
        for item in controller.AvailableInput:
            # Skip unmapped inputs
            if not item.Item in kodiMapping: continue

            if item.IsButton:
                ET.SubElement(controllerXml, "feature", name=kodiMapping[item.Item][item.Type], button=str(item.Id))

            elif item.Type in (InputItem.TypeHat, InputItem.TypeAxis) and item.IsDPad:
                direction = "-" if item.Item in (InputItem.ItemUp, InputItem.ItemLeft) else "+"
                directionId = controller.getAxisNumber(item)
                ET.SubElement(controllerXml, "feature", name=kodiMapping[item.Item][item.Type], axis="{}{}".format(direction, directionId))

            # special case: sticks
            elif item.IsAxis:
                if item.IsJoy1Left or item.IsJoy1Up:
                    if leftstick is None:
                        leftstick = ET.SubElement(controllerXml, "feature", name="leftstick")
                    if item.IsJoy1Up:
                        upaxis, downaxis = getFormattedAxis(item, controller)
                        ET.SubElement(leftstick, "up", axis=upaxis)
                        ET.SubElement(leftstick, "down", axis=downaxis)
                    if item.IsJoy1Left:
                        leftaxis, rightaxis = getFormattedAxis(item, controller)
                        ET.SubElement(leftstick, "left", axis=leftaxis)
                        ET.SubElement(leftstick, "right", axis=rightaxis)

                elif item.IsJoy2Left or item.IsJoy2Up:
                    if rightstick is None:
                        rightstick = ET.SubElement(controllerXml, "feature", name="rightstick")
                    if item.IsJoy2Left:
                        leftaxis, rightaxis = getFormattedAxis(item, controller)
                        ET.SubElement(rightstick, "left", axis=leftaxis)
                        ET.SubElement(rightstick, "right", axis=rightaxis)
                    if item.IsJoy2Up:
                        upaxis, downaxis = getFormattedAxis(item, controller)
                        ET.SubElement(rightstick, "up", axis=upaxis)
                        ET.SubElement(rightstick, "down", axis=downaxis)
                else:
                    print("Unsupportted " + item.TypeName + " " + item.Name)
            else:
                print("Unsupportted " + item.TypeName + " " + item.Name)

        # Need to write this string to a file
        with open(xmlFileName, "w") as f:
            from xml.dom import minidom
            f.write(minidom.parseString(ET.tostring(buttonmap)).toprettyxml())
