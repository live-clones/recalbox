#!/usr/bin/env python

class IniSettings:
    def __init__(self, settingsFile, extraSpaces = False):
        self.settingsFile = settingsFile
        self.regions = dict()
        self.extraSpaces = extraSpaces # Allow 'key = value' instead of 'key=value' on writing.

    def __getitem__(self, section, item):
        if isinstance(section, str) and isinstance(item, str):
            if section in self.regions:
                region = self.regions[section]
                if item in region:
                    return region[item]
            raise KeyError
        else:
            raise TypeError

    def clear(self):
        self.regions.clear()

    def clearRegion(self, section):
        if isinstance(section, str):
            if section in self.regions:
                self.regions[section].clear()
            else:
                raise KeyError
        else:
            raise TypeError

    def changeSettingsFile(self, newfilename):
        self.settingsFile = newfilename

    def getSettingsFile(self):
        return self.settingsFile

    def getOption(self, section, item, default):
        if section in self.regions:
            region = self.regions[section]
            if item in region:
                return region[item]
        return default

    def hasSection(self, section):
        if section in self.regions:
            return True
        return False

    def hasOption(self, section, item):
        if section in self.regions:
            if item in self.regions[section]:
                return True
        return False

    def setOption(self, section, item, value):
        if section not in self.regions:
            self.regions[section] = dict()
        region = self.regions[section]
        region[item] = value

    def setDefaultOption(self, section, item, defaultvalue):
        if section not in self.regions:
            self.regions[section] = dict()
        region = self.regions[section]
        if item not in region:
            region[item] = defaultvalue

    def removeOption(self, section, item):
        if section not in self.regions:
            return
        self.regions[section].pop(item, None)

    def removeOptionStartingWith(self, section, pattern):
        if section not in self.regions:
            return
        region = self.regions[section]
        keysToRemove = []
        for key in region.keys():
            if key.startswith(pattern):
                keysToRemove.append(key)
        for key in keysToRemove:
            region.pop(key, None)

    def saveFile(self):
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

    def loadFile(self, clear = False):
        import re
        try:
            if clear:
                self.regions.clear()
            import os
            if os.path.exists(self.settingsFile):
                with open(self.settingsFile) as lines:
                    lastSection = "default"
                    for line in lines:
                        m = re.match(r'^([^#;].*)\s?=\s?\"?(.*?)\"?$', line)
                        if m:
                            key = m.group(1).strip()
                            value = m.group(2).strip()
                            self.setOption(lastSection, key, value)
                            #print("Value: -{}:{}={}-".format(lastSection, key, value))
                        else:
                            m = re.match(r'^\[(.*)\]', line)
                            if m:
                                lastSection = m.group(1).strip()
                                #print("Section: -{}-".format(lastSection))
        finally:
            return self.regions
