from typing import List, Dict, Set

from configin import ConfigIn
from inisettings import IniSettings


class SystemHolder:

    class SystemProperties:

        __systemTypes: Dict[str, int] = {
            "arcade": 0,
            "console": 1,
            "handheld": 2,
            "computer": 3,
            "virtual": 4,
            "engine": 5,
            "port": 6,
        }

        __deviceRequirement: Dict[str, int] = {
            "no": 0,
            "optional": 1,
            "recommended": 2,
            "mandatory": 3,
        }

        def __init__(self, systemtype: str, pad: str, keyboard: str, mouse: str, lightgun: str, releasedate: str, retroachievements: bool):
            if systemtype not in self.__systemTypes.keys(): raise TypeError("Invalid system type! {}".format(systemtype))
            if pad not in self.__deviceRequirement.keys(): raise TypeError("Invalid pad type! {}".format(pad))
            if keyboard not in self.__deviceRequirement.keys(): raise TypeError("Invalid keyboard type! {}".format(keyboard))
            if mouse not in self.__deviceRequirement.keys(): raise TypeError("Invalid mouse type! {}".format(mouse))
            if lightgun not in self.__deviceRequirement.keys(): raise TypeError("Invalid lightgun type! {}".format(lightgun))
            self.__type = systemtype
            self.__pad = pad
            self.__keyboard = keyboard
            self.__mouse = mouse
            self.__lightgun = lightgun
            self.__releasedate = releasedate
            self.__retroachievements: bool = retroachievements

        def serialize(self):
            return {
                "type": self.__type,
                "pad": self.__pad,
                "keyboard": self.__keyboard,
                "mouse": self.__mouse,
                "lightgun": self.__lightgun,
                "releasedate": self.__releasedate,
                "retroachievements": '1' if self.__retroachievements else '0'
            }

        @property
        def Type(self): return self.__type

        @property
        def TypeEnum(self): return self.__systemTypes[self.__type]

        @property
        def Pad(self): return self.__pad

        @property
        def PadEnum(self): return self.__deviceRequirement[self.__pad]

        @property
        def Keyboard(self): return self.__keyboard

        @property
        def KeyboardEnum(self): return self.__deviceRequirement[self.__keyboard]

        @property
        def Mouse(self): return self.__mouse

        @property
        def MouseEnum(self): return self.__deviceRequirement[self.__mouse]

        @property
        def LightGun(self): return self.__lightgun

        @property
        def ReleaseDate(self): return self.__releasedate

        @property
        def RetroAchievements(self) -> bool: return self.__retroachievements

    class Core:

        __CompatibilityValues: Dict[str, int] = {
            "unknown": 0,
            "high": 1,
            "good": 2,
            "average": 3,
            "low" : 4,
        }

        def __init__(self, package: str, priority: int, emulator: str, core: str, extensions: str, netplay: bool, compatibility: str, speed: str):
            self.__package: str = package
            self.__priority: int = priority
            self.__emulator: str = emulator
            self.__core: str = core
            self.__extensions: List[str] = extensions.split(' ')
            self.__netplay: bool = netplay
            if len(compatibility) == 0: compatibility = "unknown"
            if compatibility not in self.__CompatibilityValues.keys(): raise TypeError("Invalid compatibility! {}".format(compatibility))
            self.__compatibility = compatibility
            self.__speed = speed
            pass

        @property
        def priority(self) -> int:
            return self.__priority

        @property
        def package(self) -> str:
            return self.__package

        @property
        def emulator(self) -> str:
            return self.__emulator

        @property
        def core(self) -> str:
            return self.__core

        @property
        def extensions(self) -> List[str]:
            return self.__extensions

        @property
        def netplay(self) -> bool:
            return self.__netplay

        @property
        def compatibility(self) -> str:
            return self.__compatibility

        @property
        def speed(self) -> str:
            return self.__speed

        def serialize(self):
            return {
                "name": self.__core,
                "priority": str(self.__priority),
                "extensions": ' '.join(self.__extensions),
                "netplay": '1' if self.__netplay else '0',
                "compatibility": self.__compatibility,
                "speed": self.__speed,
            }

    __COMMAND_DEFAULT: str = "python /usr/bin/emulatorlauncher.pyc %CONTROLLERSCONFIG% -system %SYSTEM% -rom %ROM% -emulator %EMULATOR% -core %CORE% -ratio %RATIO% %NETPLAY%"

    def __init__(self, arch: str, systemIni: str, config: ConfigIn):
        self.__config = config
        self.__systemIni: str = systemIni
        self.__arch: str = arch
        self.__uuid: str = "uuid"
        self.__name: str = "Name"
        self.__fullname: str = "Fullname"
        self.__platforms: str = "Platform list"
        self.__theme: str = "Theme folder"
        self.__roms: str = "Roms folder"
        self.__screenscraperId: int = 0 # Screenscraper's system ID
        self.__command: str = "Command template"
        self.__extensions: str = "Global extension list"
        self.__docLinks: Dict[str, str] = {
          "fr": "FR Documentation link",
          "en": "EN Documentation link",
        }
        self.__port: bool = False
        self.__readOnly: bool = False
        self.__properties = SystemHolder.SystemProperties("virtual", "no", "no", "no", "no", "", False)
        self.__coreLists: Dict[str, List[SystemHolder.Core]] = dict()
        self.__coreCount: int = 0
        self.__deserialize()
        pass

    def DocLinks(self, language: str) -> str:
        return  self.__docLinks[language] if language in self.__docLinks else ""

    @property
    def Cores(self) -> Dict[str, List[Core]]: return self.__coreLists

    @property
    def CoreCount(self) -> int: return self.__coreCount

    @property
    def Properties(self) -> SystemProperties: return self.__properties

    @property
    def Uuid(self) -> str: return self.__uuid

    @property
    def Name(self) -> str: return self.__name

    @property
    def FullName(self) -> str: return self.__fullname

    @property
    def Platform(self) -> str: return self.__platforms

    @property
    def ThemeFolder(self) -> str: return self.__theme

    @property
    def RomFolder(self) -> str: return self.__roms

    @property
    def Command(self) -> str: return self.__command

    @property
    def Extensions(self) -> str: return self.__extensions

    @property
    def ScreenScraperId(self) -> int: return self.__screenscraperId

    @property
    def IsPort(self) -> bool: return self.__port

    @property
    def IsReadOnly(self) -> bool: return self.__readOnly

    @property
    def IsDefaultCommand(self) -> bool: return self.__command == self.__COMMAND_DEFAULT

    @staticmethod
    def DefaultCommand() -> str: return SystemHolder.__COMMAND_DEFAULT

    def __get(self, ini: IniSettings, section: str, key: str, default: str, required: bool) -> str:
        # Arch Section has highest priority
        archSection: str = section + "." + self.__arch
        if ini.hasSection(archSection):
            if ini.hasOption(archSection, key):
                return ini.getOption(archSection, key, default)

        # Then Arch Key
        archKey: str = key + "." + self.__arch
        if ini.hasOption(section, archKey):
            return ini.getOption(section, archKey, default)

        # Then regular section/key
        if ini.hasOption(section, key):
            return ini.getOption(section, key, default)

        if not required:
            return default
        raise KeyError("Missing {}:{} in {}".format(section, key, ini.getSettingsFile()))

    def __deserialize(self):
        # Load configuration
        desc = IniSettings(self.__systemIni, extraSpaces=True)
        desc.loadFile(True)

        # System part
        self.__uuid = self.__get(desc, "system", "uuid", "", True)
        self.__name = self.__get(desc, "system", "name", "", True)
        self.__fullname = self.__get(desc, "system", "fullname", "", True)
        self.__platforms = self.__get(desc, "system", "platforms", "", True)
        self.__theme = self.__get(desc, "system", "theme.folder", "", True)
        self.__roms = self.__get(desc, "system", "roms.folder", "", True)
        self.__screenscraperId = int(self.__get(desc, "system", "screenscraper.id", "0", True))
        self.__command = self.__get(desc, "system", "command", self.__COMMAND_DEFAULT, False)
        self.__docLinks["fr"] = self.__get(desc, "system", "doc.link.fr", "", False)
        self.__docLinks["en"] = self.__get(desc, "system", "doc.link.en", "", False)
        self.__port = self.__get(desc, "system", "port", "0", False) == '1'
        self.__readOnly = self.__get(desc, "system", "readonly", "0", False) == '1'

        # Create Cores
        extensions: Set[str] = set()
        for i in range(0, 10):
            coreSection = "core.{}".format(i)
            coreSectionArch = "{}.{}".format(coreSection, self.__arch)
            if desc.hasSection(coreSection) or desc.hasSection(coreSectionArch):
                # deserialize
                core = SystemHolder.Core(
                    package=self.__get(desc, coreSection, "package", "", True),
                    priority=int(self.__get(desc, coreSection, "priority", "", True)),
                    emulator=self.__get(desc, coreSection, "emulator", "", True),
                    core=self.__get(desc, coreSection, "core", "", True),
                    extensions=self.__get(desc, coreSection, "extensions", "", True),
                    netplay=(self.__get(desc, coreSection, "netplay", "", True) == '1'),
                    compatibility=self.__get(desc, coreSection, "compatibility", "", True),
                    speed=self.__get(desc, coreSection, "speed", "", True),
                )
                # Package defined?
                if self.__config.isDefined(core.package):
                    # count
                    self.__coreCount = self.__coreCount + 1
                    # record core
                    if core.emulator not in self.__coreLists: self.__coreLists[core.emulator] = [core]
                    else:                                     self.__coreLists[core.emulator].append(core)
                    # Add extensions to global list
                    for e in core.extensions:
                        extensions.add(e)

        # Record extensions
        self.__extensions = ' '.join(extensions)
        if "files:" in self.__extensions:
            self.__extensions = "files:" + self.__extensions.replace("files:", "")

        # Properties
        self.__properties = SystemHolder.SystemProperties(
            systemtype=self.__get(desc, "properties", "type", "", True),
            pad=self.__get(desc, "properties", "device.pad", "no", True),
            keyboard=self.__get(desc, "properties", "device.keyboard", "no", True),
            mouse=self.__get(desc, "properties", "device.mouse", "no", True),
            lightgun=self.__get(desc, "properties", "device.lightgun", "no", True),
            releasedate=self.__get(desc, "properties", "release.date", "", True),
            retroachievements=(self.__get(desc, "properties", "retroachievements", "", True) == '1'),
        )

