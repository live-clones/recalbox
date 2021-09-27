#!/usr/bin/env python
from __future__ import  annotations

from typing import Dict


class IniSettings:
    def __init__(self, settingsFile, extraSpaces = False):
        self.settingsFile = settingsFile
        self.regions: Dict[str, Dict[str, str]] = dict()
        self.extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.
        self.__true = '1'
        self.__false = '0'

    def __getitem__(self, section: str, item: str):
        if section in self.regions:
            region = self.regions[section]
            if item in region:
                return region[item]
        raise KeyError

    def defineBool(self, true: str, false: str) -> IniSettings:
        self.__true = true
        self.__false = false
        return self

    def clear(self) -> IniSettings:
        self.regions.clear()
        return self

    def clearRegion(self, section: str) -> IniSettings:
        if section in self.regions:
            self.regions[section].clear()
        return self

    def changeSettingsFile(self, newfilename: str) -> IniSettings:
        self.settingsFile = newfilename
        return self

    def getSettingsFile(self):
        return self.settingsFile

    def getString(self, section: str, item: str, default: str) -> str:
        if section in self.regions:
            region = self.regions[section]
            if item in region:
                return region[item]
        return default

    def getInt(self, section: str, item: str, default: int) -> int:
        if section in self.regions:
            region = self.regions[section]
            if item in region:
                return int(region[item].strip('"'))
        return default

    def getBool(self, section: str, item: str, default: bool) -> bool:
        if section in self.regions:
            region = self.regions[section]
            if item in region:
                return region[item].strip('"') == self.__true
        return default

    def hasRegion(self, section: str) -> bool:
        return section in self.regions

    def hasOption(self, section:str, item: str) -> bool:
        if section in self.regions:
            if item in self.regions[section]:
                return True
        return False

    def setString(self, section, item: str, value: str) -> IniSettings:
        if section not in self.regions:
            self.regions[section] = {}
        region = self.regions[section]
        region[item] = value
        return self

    def setInt(self, section, item: str, value: int) -> IniSettings:
        if section not in self.regions:
            self.regions[section] = {}
        region = self.regions[section]
        region[item] = str(value)
        return self

    def setBool(self, section, item: str, value: bool) -> IniSettings:
        if section not in self.regions:
            self.regions[section] = {}
        region = self.regions[section]
        region[item] = self.__true if value else self.__false
        return self

    def removeOption(self, section: str, item: str) -> IniSettings:
        if section in self.regions:
            self.regions[section].pop(item, None)
        return self

    def removeOptionStartingWith(self, section: str, pattern: str) -> IniSettings:
        if section in self.regions:
            region = self.regions[section]
            keysToRemove = []
            for key in region.keys():
                if key.startswith(pattern):
                    keysToRemove.append(key)
            for key in keysToRemove:
                region.pop(key, None)
        return self

    def saveFile(self) -> IniSettings:
        import os
        folder = os.path.dirname(self.settingsFile)
        if not os.path.exists(folder):
            os.makedirs(folder)
        firstSection = True
        with open(self.settingsFile, 'wb+') as sf:
            for section in sorted(self.regions.keys()):
                if firstSection: firstSection = False
                else: sf.write('\n'.encode("utf-8"))
                sf.write(('[' + str(section) + ']\n').encode("utf-8"))
                region = self.regions[section]
                if self.extraSpaces:
                    for key in sorted(region.keys()):
                        sf.write((key + " = " + str(region[key]) + '\n').encode("utf-8"))
                else:
                    for key in sorted(region.keys()):
                        sf.write((key + "=" + str(region[key]) + '\n').encode("utf-8"))
        return self

    def loadFile(self, clear = False) -> IniSettings:
        import re
        try:
            if clear:
                self.regions.clear()
            import os
            if os.path.exists(self.settingsFile):
                with open(self.settingsFile) as lines:
                    lastSection = "default"
                    for line in lines:
                        m = re.match(r'^([^#;].*)\s?=\s?(.+)$', line)
                        if m:
                            key = m.group(1).strip()
                            value = m.group(2).strip()
                            self.setString(lastSection, key, value)
                            #print("Value: -{}:{}={}-".format(lastSection, key, value))
                        else:
                            m = re.match(r'^\[(.*)\]', line)
                            if m:
                                lastSection = m.group(1).strip()
                                #print("Section: -{}-".format(lastSection))
        finally:
            return self
