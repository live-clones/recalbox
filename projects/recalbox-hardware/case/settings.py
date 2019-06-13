#!/usr/bin/env python


class Settings:
    def __init__(self, settingsFile, extraSpaces = False):
        self.settingsFile = settingsFile
        self.settings = dict()
        self.extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.

    def __getitem__(self, item):
        if isinstance(item, str):
            if item in self.settings:
                return self.settings[item]
            raise KeyError
        else:
            raise TypeError

    def clear(self):
        self.settings.clear()

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

    def removeOption(self, key):
        self.settings.pop(key, None)

    def removeOptionStartingWith(self, pattern):
        keysToRemove = []
        for key in self.settings.iterkeys():
            if key.startswith(pattern):
                keysToRemove.append(key)
        for key in keysToRemove:
            self.settings.pop(key, None)

    def getOptionSubset(self, startWith):
        result = {}
        swl = len(startWith)
        for key in self.settings.iterkeys():
            if key.startswith(startWith):
                result[key[swl:]] = self.settings[key]
        return result

    def saveFile(self):
        os = __import__("os")
        folder = os.path.dirname(self.settingsFile)
        if not os.path.exists(folder):
            os.makedirs(folder)
        with open(self.settingsFile, 'w+') as sf:
            if self.extraSpaces:
                for key in sorted(self.settings.iterkeys()):
                    sf.write(key + " = " + str(self.settings[key]) + '\n')
            else:
                for key in sorted(self.settings.iterkeys()):
                    sf.write(key + "=" + str(self.settings[key]) + '\n')

    def loadFile(self, clear = False):
        try:
            if clear:
                self.settings.clear()
            with open(self.settingsFile) as lines:
                for line in lines:
                    if '=' in line:
                        key, value = line.split('=', 1)
                        if key is not None and value is not None:
                            self.settings[key.strip()] = value.strip()
        finally:
            return self.settings
