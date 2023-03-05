import os
import typing
from dataclasses import dataclass
from enum import Enum

from configgen.crt.CRTTypes import CRTSystem, CRTArcadeMode, CRTVideoStandard, CRTScreenType, CRTResolutionType, \
    CRTArcadeGameV2
from configgen.crt.Mode import Mode
from configgen.recalboxFiles import crtFilesRootFolder, crtUserFilesRootFolder
from configgen.utils.recallog import recallog


class CRTConfigParser:
    v2: bool = False

    def __init__(self, V2: bool = False):
        self.v2 = V2

    def findSystem(self, system: str, core: str, region: CRTVideoStandard, screenType: CRTScreenType,
                   rezType: CRTResolutionType) -> typing.Optional[CRTSystem]:
        for line in self.__find_lines_begining_with_in_configs(
                "{},{},{},{},{}".format(system, core, region, screenType, rezType), "systems.txt"):
            return self.__parseSystem(line)

    def findArcadeGame(self, game: str, emulator: str) -> CRTArcadeMode:
        for line in self.__find_lines_begining_with_in_configs("{},{}".format(game, emulator), "arcade_games.txt"):
            return self.__parseArcadeGame(line)

    def findArcadeGameV2(self, game: str, emulator: str) -> CRTArcadeGameV2:
        for line in self.__find_lines_begining_with_in_configs("{},{}".format(game, emulator), "arcade_games_v2.txt"):
            return self.__parseArcadeGameV2(line)

    def loadMode(self, mode_id: str) -> Mode:
        mode: typing.Optional[Mode] = None
        modes = self.__find_lines_begining_with_in_configs(mode_id, "modes.txt")
        if len(modes) >= 1:
            mode = self.__parseModeLine(modes[0])
        if mode is None:
            recallog("Mode not found : {}".format(mode_id), log_type="CRT")
            raise Exception("Mode not found")
        return mode

    def __parseModeLine(self, mode_line: str) -> typing.Optional[Mode]:
        if len(mode_line) == 0 or mode_line.startswith("#"):
            return None
        id_and_mode = mode_line.strip().split(",")
        if len(id_and_mode) != 3:
            recallog("Malformed mode file on line: {}".format(id_and_mode), log_type="CRT")
            raise Exception('Malformed mode file')
        return Mode(id_and_mode[1], id_and_mode[2])

    def __parseSystem(self, line: str) -> CRTSystem:
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

    def __parseArcadeGame(self, line: str) -> CRTArcadeMode:
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

    def __parseArcadeGameV2(self, line: str) -> CRTArcadeGameV2:
        line = line.rstrip()
        infos = line.split(",")
        if len(infos) != 6:
            recallog("Malformed arcade game file", log_type="CRT")
            raise Exception('Malformed arcade game file')

        return CRTArcadeGameV2(infos[0],
                               infos[1],
                               int(infos[2]),
                               int(infos[3]),
                               float(infos[4]),
                               infos[5] == "V")

    def __find_line_begining_with(self, string, fp):
        for line in fp:
            if line.startswith(string):
                yield line

    def __find_lines_begining_with_in_configs(self, element_id: str, config_file: str) -> [str]:
        lines: [str] = []
        user_file = os.path.join(crtUserFilesRootFolder, config_file)
        system_file = os.path.join(crtFilesRootFolder, config_file)
        try:
            with open(user_file, "r") as fp:
                for line in self.__find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        except Exception:
            pass
        try:
            with open(system_file, "r") as fp:
                for line in self.__find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        except Exception:
            pass
        return lines
