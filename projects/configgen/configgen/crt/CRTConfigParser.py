import os
import typing
from dataclasses import dataclass
from enum import Enum

from configgen.crt.Mode import Mode
from configgen.recalboxFiles import crtFilesRootFolder, crtUserFilesRootFolder
from configgen.utils.recallog import recallog

CRTSystemMode = typing.Tuple[str, str, str, str, str, int, int]
CRTArcadeMode = typing.Tuple[str, str, int, int, int]


class CRTVideoStandard(str, Enum):
    PAL = "pal"
    NTSC = "ntsc"
    AUTO = "auto"
    ALL = "all"

    @staticmethod
    def fromString(value: str):
        if value == "pal":
            return CRTVideoStandard.PAL
        if value == "ntsc":
            return CRTVideoStandard.NTSC
        return CRTVideoStandard.AUTO


class CRTRegion(str, Enum):
    AUTO = "auto"
    EU = "eu"
    US = "us"
    JP = "jp"

    @staticmethod
    def fromString(value: str):
        if value == "eu":
            return CRTRegion.EU
        if value == "us":
            return CRTRegion.US
        if value == "jp":
            return CRTRegion.JP
        return CRTRegion.AUTO


class CRTScreenType(str, Enum):
    k15 = "15kHz"
    k31 = "31kHz"

    @staticmethod
    def fromString(value: str):
        if value == "31kHz":
            return CRTScreenType.k31
        return CRTScreenType.k15


class CRTResolutionType(str, Enum):
    Progressive = "progressive"
    Interlaced = "interlaced"
    DoubleFreq = "doublefreq"

    @classmethod
    def fromString(cls, value: str):
        for k, v in cls.__members__.items():
            if v == value:
                return v
        return CRTResolutionType.Progressive


@dataclass
class CRTSystem:
    core: str
    region: str
    display: str
    reztype: str
    mode_id: str
    viewport_width: int
    viewport_height: int


class CRTConfigParser:

    def parseModeLine(self, mode_line: str) -> typing.Optional[Mode]:
        if len(mode_line) == 0 or mode_line.startswith("#"):
            return None
        id_and_mode = mode_line.strip().split(",")
        if len(id_and_mode) != 3:
            recallog("Malformed mode file on line: {}".format(id_and_mode), log_type="CRT")
            raise Exception('Malformed mode file')
        return Mode(id_and_mode[1], id_and_mode[2])

    def parseSystem(self, line: str) -> CRTSystem:
        line = line.rstrip()
        id_and_region_and_modeid = line.split(",")
        if len(id_and_region_and_modeid) < 8:
            recallog("Malformed system file", log_type="CRT")
            raise Exception('Malformed system file')
        core = id_and_region_and_modeid[1]
        region = id_and_region_and_modeid[2]
        display = id_and_region_and_modeid[3]
        reztype = id_and_region_and_modeid[4]
        mode_id = id_and_region_and_modeid[5]
        viewport_width = int(id_and_region_and_modeid[6])
        viewport_height = int(id_and_region_and_modeid[7])

        return CRTSystem(core, region, display, reztype, mode_id, viewport_width, viewport_height)

    def parseArcadeGame(self, line: str) -> CRTArcadeMode:
        line = line.rstrip()
        id_and_region_and_modeid = line.split(",")
        if len(id_and_region_and_modeid) < 3:
            recallog("Malformed arcade game file", log_type="CRT")
            raise Exception('Malformed arcade game file')
        region = id_and_region_and_modeid[1]
        mode_id = id_and_region_and_modeid[2]
        viewport_width = int(id_and_region_and_modeid[3])
        viewport_height = int(id_and_region_and_modeid[4])
        rotation = int(id_and_region_and_modeid[5])

        return region, mode_id, viewport_width, viewport_height, rotation

    def loadMode(self, mode_id: str) -> Mode:
        mode: typing.Optional[Mode] = None
        modes = self.find_lines_begining_with_in_configs(mode_id, "modes.txt")
        if len(modes) >= 1:
            mode = self.parseModeLine(modes[0])
        if mode is None:
            recallog("Mode not found : {}".format(mode_id), log_type="CRT")
            raise Exception("Mode not found")
        return mode

    def findSystem(self, system: str, core: str, region: CRTVideoStandard, screenType: CRTScreenType,
                   rezType: CRTResolutionType) -> typing.Optional[CRTSystem]:
        for line in self.find_lines_begining_with_in_configs(
                "{},{},{},{},{}".format(system, core, region, screenType, rezType), "systems.txt"):
            return self.parseSystem(line)

    def findArcadeGame(self, game: str, emulator: str) -> CRTArcadeMode:
        for line in self.find_lines_begining_with_in_configs("{},{}".format(game, emulator), "arcade_games.txt"):
            return self.parseArcadeGame(line)

    def find_line_begining_with(self, string, fp):
        for line in fp:
            if line.startswith(string):
                yield line

    def find_lines_begining_with_in_configs(self, element_id: str, config_file: str) -> [str]:
        lines: [str] = []
        user_file = os.path.join(crtUserFilesRootFolder, config_file)
        system_file = os.path.join(crtFilesRootFolder, config_file)
        try:
            with open(user_file, "r") as fp:
                for line in self.find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        except Exception:
            pass
        try:
            with open(system_file, "r") as fp:
                for line in self.find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        except Exception:
            pass
        return lines
