import os
import typing
from dataclasses import dataclass
from enum import Enum

from configgen.crt.CRTTypes import CRTSystem, CRTArcadeMode, CRTVideoStandard, CRTScreenType, CRTResolutionType
from configgen.crt.Mode import Mode
from configgen.recalboxFiles import crtFilesRootFolder, crtUserFilesRootFolder
from configgen.utils.recallog import recallog


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
