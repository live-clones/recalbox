import os
import xml.etree.ElementTree as ET
from typing import Union

from xml.dom import minidom
from configin import ConfigIn
from systemholder import SystemHolder


class SystemBuilder:

    def __init__(self, sourcePath: str, targetPath: str, root: str):
        self.__source = sourcePath
        self.__target = targetPath
        self.__config = ConfigIn(root)
        self.__root = root
        self.__arch = self.__config.Arch
        print("[ROMFS 2] Target Arch: {}".format(self.__arch))
        pass

    def execute(self):
        print("[ROMFS 2] Building systems to {}".format(self.__target))

        # Create final document w/ root node
        root = ET.Element("systemList")

        # Run though systems folders
        for folder in os.listdir(self.__source):
            subfolder = os.path.join(self.__source, folder)
            if os.path.isdir(subfolder) and not folder.startswith('.'):
                print("[ROMFS 2] Building system from {}".format(folder))
                result = self.__process(subfolder)
                if result is not None:
                    root.append(result)

        # Pretiffy
        xml: str = ET.tostring(root, "utf-8")
        xml = minidom.parseString(xml).toprettyxml(indent='  ')
        with open(self.__target, "w") as f:
            f.write(xml)

    def __process(self, folder: str) -> Union[ET.Element, None]:
        # Deserialize
        holder = SystemHolder(self.__arch, os.path.join(folder, "system.ini"), self.__config)
        if holder.CoreCount < 1:
            return None

        # Open system node
        system = ET.Element("system")

        # System part
        ET.SubElement(system, "name").text = holder.Name
        ET.SubElement(system, "fullname").text = holder.FullName
        ET.SubElement(system, "platform").text = holder.Platform
        ET.SubElement(system, "theme").text = holder.ThemeFolder
        ET.SubElement(system, "path").text = holder.RomFolder
        ET.SubElement(system, "screenscraper").text = str(holder.ScreenScraperId)
        ET.SubElement(system, "command").text = holder.Command
        ET.SubElement(system, "extensions").text = holder.Extensions

        # Properties
        ET.SubElement(system, "properties", holder.Properties.serialize())

        # Record cores
        emulatorsNode = ET.SubElement(system, "emulators")
        for emulator in holder.Cores:
            emulatorNode = ET.SubElement(emulatorsNode, "emulator", attrib={ "name": emulator })
            coresNode = ET.SubElement(emulatorNode, "cores")
            for core in holder.Cores[emulator]:
                ET.SubElement(coresNode, "core", attrib=core.serialize()).text = core.core

        return system
