#!/usr/bin/env python
import os

import configgen.recalboxFiles as recalboxFiles
from xml.etree import ElementTree as ET

from configgen.Emulator import Emulator
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


def getControllerName(device: str):
    parts: [str] = device.split("/")
    index: str = parts[len(parts) - 1]
    sysFile = "/sys/class/input/{}/device/name".format(index)
    if os.path.isfile(sysFile):
        with open(sysFile, "r") as f:
            return f.readline().rstrip("\n")
    return ""

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

        from xml.etree import ElementTree as ET
        buttonmap = ET.Element("buttonmap")
        controllerName = getControllerName(controller.DevicePath)
        if controllerName == "":
            controllerName = controller.DeviceName
        device = ET.SubElement(buttonmap, "device", name=controllerName, provider="linux", buttoncount=str(nbButtons), axiscount=str(nbAxis))
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


def writeKodiAdvancedSettingsConfig(architecture: str, resolution: str):
    """special configuration depending on architecture"""
    """rpi4:"""
    """<setting id="videoplayer.useprimedecoder">true</setting>"""
    """<setting id="videoplayer.useprimedecoderforhw" default="true">true</setting>"""
    """<setting id="videoplayer.useprimerenderer">0</setting>"""
    print(f"Setting advanced kodi settings with resolution {resolution} for arch {architecture}")

    force_configuration = {
        "rpi4_64": {
            "videoplayer": {
                "useprimedecoder": "true",
                "useprimedecoderforhw": "true",
                "useprimerenderer": "0",

            },
            "videoscreen": {
                "screenmode": resolution,
            }
        },
        "rpi3": {
            "videoplayer": {
                "useprimedecoder": "true",
                "useprimedecoderforhw": "true",
                "useprimerenderer": "0",
            },
            "videoscreen": {
                "screenmode": resolution,
            }
        },
        "rpizero2": {
            "videoplayer": {
                "useprimedecoder": "true",
                "useprimedecoderforhw": "true",
                "useprimerenderer": "0",
            },
            "videoscreen": {
                "screenmode": resolution,
            }
        },
        "default": {
            "videoscreen": {
                "screenmode": resolution,
            }
        }
    }
    config = force_configuration["default"]
    if architecture in force_configuration:
        config = force_configuration[architecture]
    try:
        kodiAdvancedSettingsSetter(config)
    except KeyError:
        print(f"no gui settings for {architecture}")


def kodiAdvancedSettingsSetter(wanted_configuration):
    try:
        tree = ET.parse(recalboxFiles.kodiAdvancedSettings)
        advanced_settings = tree.getroot()
    except Exception as err:
        print(f"can't parse guisettings.xml: {err}")
        advanced_settings = ET.fromstring('<advancedsettings></advancedsettings>')

    for category, category_items in wanted_configuration.items():
        category_setting = advanced_settings.find(f"./{category}")
        if category_setting is None:
            category_setting = ET.SubElement(advanced_settings, category)

        for setting_name, setting_value in category_items.items():
            setting = category_setting.find(f"./{setting_name}")
            if setting is None:
                print(f"add setting {setting_name} in {category}")
                setting = ET.SubElement(category_setting, setting_name)
            setting.text = setting_value

    try:
        tree.write(recalboxFiles.kodiAdvancedSettings)
    except:
        with open(recalboxFiles.kodiAdvancedSettings, "wb") as fh:
            fh.write(ET.tostring(advanced_settings, encoding="UTF-8"))
