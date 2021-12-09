import typing
from pathlib import Path

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTConfigParser, CRTConfiguration
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class LibretroConfigCRT:

    def __init__(self, crt_config_parser: CRTConfigParser, crt_mode_processor: CRTModeOffsetter, h_offset: int = 0,
                 v_offset: int = 0, viewport_width: int = 0):
        self.crt_config_parser = crt_config_parser
        self.crt_mode_processor = crt_mode_processor
        self.h_offset = h_offset
        self.v_offset = v_offset
        self.viewport_width = viewport_width

    def createConfigForMode(self, region: str, mode: Mode, viewport_width: int, viewport_height: int, rotation: int) -> \
            typing.Dict[
                str, str]:
        config: typing.Dict[str, str] = {}
        if region == "all":
            config["crt_switch_timings_pal"] = '"' + mode.timings() + '"'
            config["crt_switch_timings_ntsc"] = '"' + mode.timings() + '"'
            config["video_refresh_rate_pal"] = '"{}"'.format(mode.refresh_rate)
            config["video_refresh_rate_ntsc"] = '"{}"'.format(mode.refresh_rate)
        else:
            config["crt_switch_timings_" + region] = '"' + mode.timings() + '"'
            config["video_refresh_rate_" + region] = '"{}"'.format(mode.refresh_rate)

        if viewport_height > 0 or viewport_width > 0:
            config["aspect_ratio_index"] = '23'
            extension = ["_ntsc", "_pal"] if region == "all" else ["_" + region]
            for region in extension:
                config["custom_viewport_width" + region] = viewport_width if viewport_width > 0 else mode.width
                config[
                    "custom_viewport_height" + region] = viewport_height if viewport_height > 0 else mode.height
                config["custom_viewport_x" + region] = (
                                                               mode.width - viewport_width) // 2 if viewport_width > 0 else 0
                config["custom_viewport_y" + region] = (
                                                               mode.height - viewport_height) // 2 if viewport_height > 0 else 0
        else:
            if rotation == 1:
                config["aspect_ratio_index"] = '25'
                config["video_smooth"] = '"true"'
        return config

    def select_width(self, mode_viewport_width: int, rotation: int = 0):
        if rotation:
            if mode_viewport_width > 0:
                return mode_viewport_width
            else:
                return 0
        if mode_viewport_width > 0:
            return mode_viewport_width
        return self.viewport_width

    def createConfigFor(self, system: Emulator, rom: str) -> typing.Dict[str, str]:
        config: typing.Dict[str, str] = {"aspect_ratio_index": "24",
                                         "video_aspect_ratio_auto": '"false"',
                                         "crt_switch_timings_pal": "",
                                         "crt_switch_timings_ntsc": "",
                                         "video_refresh_rate_pal": '"60"',
                                         "video_refresh_rate_ntsc": '"50"',
                                         "crt_switch_timings": "",
                                         "custom_viewport_width": "",
                                         "custom_viewport_width_ntsc": "",
                                         "custom_viewport_width_pal": "",
                                         "custom_viewport_height": "",
                                         "custom_viewport_height_ntsc": "",
                                         "custom_viewport_height_pal": "",
                                         "custom_viewport_x": "",
                                         "custom_viewport_x_ntsc": "",
                                         "custom_viewport_x_pal": "",
                                         "custom_viewport_y": "",
                                         "custom_viewport_y_ntsc": "",
                                         "custom_viewport_y_pal": "",
                                         "video_crop_overscan": '"false"',
                                         "menu_driver": '"rgui"',
                                         "video_shader_enable": '"false"',
                                         "video_vsync": '"true"'
                                         }
        arcade_cores: [str] = ["fbneo", "mame2003", "mame2010", "mame2003_plus"]
        core: str = system.Core
        default: bool = False
        game_name: str = Path(rom).stem

        if core in arcade_cores and system.Name != "neogeocd":
            config_core: str = core
            if config_core == "mame2003_plus":
                config_core = "mame2003"
            game_config: CRTConfiguration = self.crt_config_parser.loadArcadeGame(game_name)
            if len(game_config) > 0:
                if config_core in game_config:
                    mode_id = game_config[config_core][0]
                    mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id), self.h_offset,
                                                               self.v_offset)
                    rotation = game_config[config_core][3]
                    recallog(
                        "Setting CRT mode for arcade game {} running with {} : {} {}".format(game_name, core, mode_id,
                                                                                             "vertical" if rotation == 1 else ""),
                        log_type="CRT")

                    config.update(
                        self.createConfigForMode("all", mode, self.select_width(game_config[config_core][1],rotation),
                                                 game_config[config_core][2], rotation))
                else:
                    recallog("Core configuration {} not found for game {}".format(core, game_name), log_type="CRT")
                    default = True
            else:
                recallog("Game configuration not found for {}".format(game_name), log_type="CRT")
                default = True
        else:
            system_config = self.crt_config_parser.loadSystem(system.Name)
            if len(system_config) > 0:
                for region_mode in system_config.items():
                    print(region_mode)
                    region = region_mode[0]
                    mode_id = region_mode[1][0]
                    mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id), self.h_offset,
                                                               self.v_offset)
                    recallog(
                        "Setting {} mode for system {} running with {} : {}".format(region, system.Name, core, mode_id),
                        log_type="CRT")
                    config.update(
                        self.createConfigForMode(region, mode, self.select_width(region_mode[1][1]),
                                                 region_mode[1][2], 0))
            else:
                recallog("System configuration not found for {}".format(system.Name), log_type="CRT")
                default = True
        if default:
            recallog("Setting CRT default modes for {} on {}".format(game_name, system.Name), log_type="CRT")
            config.update(
                self.createConfigForMode("pal", self.crt_mode_processor.processMode(
                    self.crt_config_parser.loadMode("default:pal:240@50"), self.h_offset, self.v_offset),
                                         self.viewport_width, 0, 0))
            config.update(
                self.createConfigForMode("ntsc", self.crt_mode_processor.processMode(
                    self.crt_config_parser.loadMode("default:ntsc:240@60"), self.h_offset, self.v_offset),
                                         self.viewport_width, 0, 0))
        if system.Name in ["wswan", "wswanc"]:
            config["video_vsync"] = '"false"'
        return config
