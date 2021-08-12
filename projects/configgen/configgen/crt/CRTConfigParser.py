import os
import typing
from pathlib import Path

from configgen.crt.Mode import Mode
from configgen.recalboxFiles import crtFilesRootFolder, crtUserFilesRootFolder
from configgen.utils.recallog import recallog

CRTConfiguration = typing.Dict[str, typing.Tuple[str, int, int, int]]


class CRTConfigParser:

    def parseModeLine(self, mode_line: str) -> typing.Optional[Mode]:
        if len(mode_line) == 0 or mode_line.startswith("#"):
            return None
        id_and_mode = mode_line.strip().split(",")
        if len(id_and_mode) != 3:
            recallog("Malformed mode file on line: {}".format(id_and_mode), log_type="CRT")
            raise Exception('Malformed mode file')
        return Mode(id_and_mode[1], id_and_mode[2])

    def parseModeAndViewport(self, line: str, id: str) -> CRTConfiguration:
        line = line.rstrip()
        if line.startswith("{},".format(id)):
            id_and_region_and_modeid = line.split(",")
            if len(id_and_region_and_modeid) < 3:
                recallog("Malformed system file", log_type="CRT")
                raise Exception('Malformed system file')
            region = id_and_region_and_modeid[1]
            mode_id = id_and_region_and_modeid[2]
            viewport_width = 0
            if len(id_and_region_and_modeid) > 3:
                viewport_width = int(id_and_region_and_modeid[3])
            viewport_height = 0
            if len(id_and_region_and_modeid) > 4:
                viewport_height = int(id_and_region_and_modeid[4])
            rotation = 0
            if len(id_and_region_and_modeid) > 5:
                rotation = int(id_and_region_and_modeid[5])

            return {region: (mode_id, viewport_width, viewport_height, rotation)}
        return {}

    def loadMode(self, mode_id: str) -> Mode:
        mode: typing.Optional[Mode] = None
        for line in self.find_lines_begining_with_id_in_configs(mode_id, "modes.txt"):
            mode = self.parseModeLine(line)
        if mode is None:
            recallog("Mode not found : {}".format(mode_id), log_type="CRT")
            raise Exception("Mode not found")
        return mode

    def loadSystem(self, system: str) -> CRTConfiguration:
        config = dict()
        for line in self.find_lines_begining_with_id_in_configs(system, "systems.txt"):
            config.update(self.parseModeAndViewport(line, system))
        return config

    def loadArcadeGame(self, game: str) -> CRTConfiguration:
        config = dict()
        for line in self.find_lines_begining_with_id_in_configs(game, "arcade_games.txt"):
            config.update(self.parseModeAndViewport(line, game))
        return config

    def find_line_begining_with(self, string, fp):
        for line in fp:
            if line.startswith(string):
                yield line

    def find_lines_begining_with_id_in_configs(self, element_id: str, config_file: str) -> [str]:
        lines: [str] = []
        system_file = os.path.join(crtFilesRootFolder, config_file)
        if Path(system_file).exists():
            with open(system_file, "r") as fp:
                for line in self.find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        user_file = os.path.join(crtUserFilesRootFolder, config_file)
        if Path(user_file).exists():
            with open(os.path.join(crtUserFilesRootFolder, config_file), "r") as fp:
                for line in self.find_line_begining_with("{},".format(element_id), fp):
                    lines.append(line)
        return lines
