#!/usr/bin/env python


class keyValueSettings:
    def __init__(self, settingsFile, extraSpaces = False):
        self.settingsFile = settingsFile
        self.readSettings = dict()
        self.writeSettings = dict()
        self.extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.

    def __getitem__(self, item):
        if item in self.readSettings:
            return self.readSettings[item]
        raise KeyError

    def clear(self):
        self.readSettings.clear()
        self.writeSettings.clear()

    def changeSettingsFile(self, newfilename):
        self.settingsFile = newfilename

    def getSettingsFile(self):
        return self.settingsFile

    def getOption(self, key, default):
        if key in self.readSettings:
            return self.readSettings[key]
        return default

    def hasOption(self, key):
        if key in self.readSettings:
            return True
        return False

    def setOption(self, key, value):
        value = str(value)
        self.readSettings[key] = value
        self.writeSettings[key] = value

    def setOptionOnMatching(self, regEx, value):
        import re
        value = str(value)
        for key in self.readSettings:
            m = re.match(regEx, key)
            if m:
                self.writeSettings[key] = value

    def setDefaultOption(self, key, defaultvalue):
        if key not in self.readSettings:
            defaultvalue = str(defaultvalue)
            self.readSettings[key] = defaultvalue
            self.writeSettings[key] = defaultvalue

    def removeOption(self, key):
        self.readSettings.pop(key, None)
        self.writeSettings.pop(key, None)

    def removeOptionStartingWith(self, pattern):
        keysToRemove = []
        for key in self.readSettings.keys():
            if key.startswith(pattern):
                keysToRemove.append(key)
        for key in keysToRemove:
            self.readSettings.pop(key, None)
        keysToRemove = []
        for key in self.writeSettings.keys():
            if key.startswith(pattern):
                keysToRemove.append(key)
        for key in keysToRemove:
            self.writeSettings.pop(key, None)

    def getOptionSubset(self, startWith):
        result = {}
        swl = len(startWith)
        for key in self.readSettings.keys():
            if key.startswith(startWith):
                result[key[swl:]] = self.readSettings[key]
        return result

    def saveFile(self):
        if not self.writeSettings:
            return

        # Create path
        import os
        folder = os.path.dirname(self.settingsFile)
        if not os.path.exists(folder):
            os.makedirs(folder)

        # Load & replace keys
        outputLines = []
        if os.path.exists(self.settingsFile):
            import re
            with open(self.settingsFile, 'r') as lines:
                for line in lines:
                    m = re.match(r'^[#;]?(.*?)\s?=.*?', line)
                    if m:
                        key = m.group(1).strip()
                        if key in self.writeSettings:
                            line = key
                            line += " = " if self.extraSpaces else "="
                            line += self.writeSettings[key] + '\n'
                            self.writeSettings.pop(key, None)
                    outputLines.append(line)
        # Add missing key/value
        if self.writeSettings:
            outputLines.append('\n')
        for key in self.writeSettings.keys():
            line = key
            line += " = " if self.extraSpaces else "="
            line += self.writeSettings[key] + '\n'
            outputLines.append(line)

        # Save
        with open(self.settingsFile, 'w+') as sf:
            for line in outputLines:
                sf.write(line)

    def loadFile(self, clear = False):
        import re
        try:
            if clear:
                self.readSettings.clear()
                self.writeSettings.clear()
            with open(self.settingsFile, 'r') as lines:
                for line in lines:
                    m = re.match(r'^([^#;].*?)\s?=\s?(.+)$', line)
                    if m:
                        key = m.group(1).strip()
                        value = m.group(2).strip()
                        self.readSettings[key] = value
        finally:
            return self.readSettings
