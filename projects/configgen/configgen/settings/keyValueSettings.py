#!/usr/bin/env python

import re


class keyValueSettings:
    def __init__(self, settingsFile):
        self.settingsFile = settingsFile
        self.settings = dict()

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

    def saveFile(self):
        with open(self.settingsFile, 'w+') as sf:
            for key in sorted(self.settings.iterkeys()):
                sf.write(key + '=' + self.settings[key] + '\n')

    def loadFile(self):
        try:
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
