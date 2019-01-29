#!/usr/bin/env python
import os
import re


class keyValueSettings:
    def __init__(self, settingsFile, extraSpaces = False):
        self.settingsFile = settingsFile
        self.settings = dict()
        self.extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.

    def changeSettingsFile(self, newfilename):
        self.settingsFile = newfilename

    def getSettingsFile(self):
        return self.settingsFile

    def getOption(self, key, default):
        if key in self.settings:
            return self.settings[key]
        return default

    def hasOption(self, key):
        if key in self.settings:
            return True
        return False

    def setOption(self, key, value):
        self.settings[key] = value

    def setDefaultOption(self, key, defaultvalue):
        if key not in self.settings:
            self.settings[key] = defaultvalue

    def saveFile(self):
        folder = os.path.dirname(self.settingsFile)
        if not os.path.exists(folder):
            os.makedirs(folder)
        with open(self.settingsFile, 'w+') as sf:
            if self.extraSpaces:
                for key in sorted(self.settings.iterkeys()):
                    sf.write(key + " = " + self.settings[key] + '\n')
            else:
                for key in sorted(self.settings.iterkeys()):
                    sf.write(key + "=" + self.settings[key] + '\n')

    def loadFile(self, clear):
        try:
            if clear:
                self.settings.clear()
            with open(self.settingsFile) as lines:
                for line in lines:
                    m = re.match(r'^(.*)\s?=\s?"?(.+)"?', line)
                    if m:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip()
                        self.settings[key] = value
        finally:
            return self.settings
