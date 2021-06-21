import os
import xml.etree.ElementTree as ET
from typing import Union, List, Dict

from xml.dom import minidom
from configin import ConfigIn
from systemholder import SystemHolder


class SystemBuilder:

    def __init__(self, sourcePath: str, targetPath: str, root: str, mergefrom: str):
        self.__source = sourcePath
        self.__target = targetPath
        self.__mergefrom = mergefrom
        self.__config = ConfigIn(root)
        self.__root = root
        self.__arch = self.__config.Arch
        print("[ROMFS 2] Target Arch: {}".format(self.__arch))
        pass

    def execute(self):
        print("[ROMFS 2] Building systems to {}".format(self.__target))

        # Create final document w/ root node
        root = ET.Element("systemList")

        # Default command
        command = ET.Element("defaults", attrib={ "command": SystemHolder.DefaultCommand() })
        root.append(command)

        # Collect systems folders
        folderList: List[str] = []
        for folder in os.listdir(self.__source):
            subfolder = os.path.join(self.__source, folder)
            if os.path.isdir(subfolder) and not folder.startswith('.'):
                folderList.append(subfolder)
        print("[ROMFS 2] {} Systems collected".format(len(folderList)))

        # Sort
        folderList.sort()

        # Build
        finalFolderCount: int = 0
        guids: Dict[str, str] = {}
        for subfolder in folderList:
            print("[ROMFS 2] Building system from {}".format(os.path.basename(subfolder)))
            # Deserialize
            holder = SystemHolder(self.__arch, os.path.join(subfolder, "system.ini"), self.__config)
            if holder.CoreCount < 1: continue
            result = self.__process(subfolder, holder)
            if result is not None:
                if holder.Uuid in guids:
                    raise KeyError("[ROMFS 2] GUID of system {} already used in {}".format(subfolder, guids[holder.Uuid]))
                guids[holder.Uuid] = subfolder
                #self.__removeOlder(root, holder)
                root.append(result)
                finalFolderCount += 1
        print("[ROMFS 2] {} Systems built for {}".format(finalFolderCount, self.__arch))

        # Prettify
        xml: str = ET.tostring(root, "utf-8")
        xml = minidom.parseString(xml).toprettyxml(indent='  ').replace("\n\n", "\n")
        with open(self.__target, "w") as f:
            f.write(xml)

    @staticmethod
    def __removeOlder(root: ET.Element, holder: SystemHolder):
        target: Union[None, ET.Element] = None
        systems = root.findall("system")
        for system in systems:
            name = system.find("name").text
            if name == holder.Name:
                target = system
                break
        if target is not None:
            root.remove(target)

    @staticmethod
    def __process(folder: str, holder: SystemHolder) -> ET.Element:
        # Open system node
        system = ET.Element("system", attrib={
            "uuid": holder.Uuid,
            "name": holder.Name,
            "fullname": holder.FullName,
            "platforms": holder.Platform,
        })

        # Descriptor
        descriptor: Dict[str, str] = {
            "path": holder.RomFolder,
            "theme": holder.ThemeFolder,
            "extensions": holder.Extensions,
        }
        if not holder.IsDefaultCommand:
            descriptor["command"] = holder.Command
        if holder.IsPort:
            descriptor["port"] = "1"
            descriptor["readonly"] = "1" if holder.IsReadOnly else "0"
        ET.SubElement(system, "descriptor", attrib=descriptor)

        # Scraper
        ET.SubElement(system, "scraper", attrib={
            "screenscraper": str(holder.ScreenScraperId),
        })

        # Properties
        ET.SubElement(system, "properties", holder.Properties.serialize())

        # Record cores
        emulatorsNode = ET.SubElement(system, "emulatorList")
        for emulator in holder.Cores:
            emulatorNode = ET.SubElement(emulatorsNode, "emulator", attrib={ "name": emulator })
            for core in holder.Cores[emulator]:
                ET.SubElement(emulatorNode, "core", attrib=core.serialize())

        return system
