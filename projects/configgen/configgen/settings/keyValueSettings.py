#!/usr/bin/env python
from __future__ import  annotations

from typing import Dict


class keyValueSettings:
    def __init__(self, settingsFile: str, extraSpaces = False):
        self.__settingsFile: str = settingsFile
        self.__settings: Dict[str, str] = {} 
        self.__extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.
        self.__true = '1'
        self.__false = '0'

    def __getitem__(self, item: str):
        if item in self.__settings:
            return self.__settings[item]
        raise KeyError

    def defineBool(self, true: str, false: str) -> keyValueSettings:
        self.__true = true
        self.__false = false
        return self

    def clear(self) -> keyValueSettings:
        self.__settings.clear()
        return self

    def changeSettingsFile(self, newfilename) -> keyValueSettings:
        self.__settingsFile = newfilename
        return self

    @property
    def SettingsFile(self) -> str: return self.__settingsFile

    def getString(self, key: str, default: str) -> str:
        if key in self.__settings:
            return self.__settings[key]
        return default

    def getInt(self, key: str, default: int) -> int:
        if key in self.__settings:
            return int(self.__settings[key])
        return default

    def getBool(self, key: str, default: bool) -> bool:
        if key in self.__settings:
            return self.__settings[key] == self.__true
        return default

    def hasOption(self, key: str):
        if key in self.__settings:
            return True
        return False

    def setString(self, key: str, value: str) -> keyValueSettings:
        self.__settings[key] = value
        return self

    def setInt(self, key: str, value: int) -> keyValueSettings:
        self.__settings[key] = str(value)
        return self

    def setBool(self, key: str, value: bool) -> keyValueSettings:
        self.__settings[key] = self.__true if value else self.__false
        return self

    def setDefaultString(self, key: str, defaultvalue: str) -> keyValueSettings:
        if key not in self.__settings:
            self.__settings[key] = defaultvalue
        return self    

    def setDefaultInt(self, key: str, defaultvalue: int) -> keyValueSettings:
        if key not in self.__settings:
            self.__settings[key] = str(defaultvalue)
        return self    

    def setDefaultBool(self, key: str, defaultvalue: bool) -> keyValueSettings:
        if key not in self.__settings:
            self.__settings[key] = self.__true if defaultvalue else self.__false
        return self    

    def removeOption(self, key: str) -> keyValueSettings:
        self.__settings.pop(key, None)
        return self

    def removeOptionStartingWith(self, pattern) -> keyValueSettings:
        keysToRemove = []
        for key in self.__settings.keys():
            if key.startswith(pattern):
                keysToRemove.append(key)
        for key in keysToRemove:
            self.__settings.pop(key, None)
        return self    

    def getOptionSubset(self, startWith) -> Dict[str, str]:
        result: Dict[str, str] = {}
        swl = len(startWith)
        for key in self.__settings.keys():
            if key.startswith(startWith):
                result[key[swl:]] = self.__settings[key]
        return result

    def saveFile(self) -> keyValueSettings:
        import os
        folder = os.path.dirname(self.__settingsFile)
        if not os.path.exists(folder):
            os.makedirs(folder)
        with open(self.__settingsFile, 'wb+') as sf:
            if self.__extraSpaces:
                for key in sorted(self.__settings.keys()):
                    sf.write((key + " = " + str(self.__settings[key]) + '\n').encode("utf-8"))
            else:
                for key in sorted(self.__settings.keys()):
                    sf.write((key + "=" + str(self.__settings[key]) + '\n').encode("utf-8"))
        return self            

    def loadFile(self, clear=False) -> keyValueSettings:
        import re
        try:
            if clear:
                self.__settings.clear()
            with open(self.__settingsFile) as lines:
                for line in lines:
                    m = re.match(r'^([^#;].*?)\s?=\s?(.+)$', line)
                    if m:
                        key = m.group(1).strip()
                        value = m.group(2).strip()
                        self.__settings[key] = value
        finally:
            return self
