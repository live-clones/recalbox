import os

from typing import Dict

from keyvaluesettings import keyValueSettings


class ConfigIn:

    def __init__(self, root: str):
        self.__config = keyValueSettings(os.path.join(root, "output/.config"), False)
        self.__config.loadFile(True)
        self.__arch: str = self.__LoadArch()
        pass

    def __LoadArch(self) -> str:
        lines: Dict[str, str] = self.__config.getOptionSubset("BR2_PACKAGE_RECALBOX_TARGET_")
        for line in lines:
            return line.lower()
        return "unknown"

    def isDefined(self, package: str) -> bool:
        return self.__config.getOption(package, 'n') == 'y'

    @property
    def Arch(self):
        return self.__arch