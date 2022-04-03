import typing
from pathlib import Path

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTConfigParser, CRTArcadeMode, CRTVideoStandard, CRTScreenType, \
    CRTResolutionType
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.Mode import Mode
from configgen.utils.recallog import recallog


class LibretroConfigCRT:

    def __init__(self, crt_config_parser: CRTConfigParser, crt_mode_processor: CRTModeOffsetter):
        self.crt_config_parser = crt_config_parser
        self.crt_mode_processor = crt_mode_processor

    arcade_cores: [str] = ["fbneo", "mame2003", "mame2010", "mame2003_plus"]

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

        config["aspect_ratio_index"] = '23'
        extension = ["_ntsc", "_pal"] if region == "all" else ["_" + region]

        if rotation == 1:
            config["video_smooth"] = '"true"'

        if rotation == 1 and (viewport_height + viewport_width == 0):
            # Change the ratio to 1920 core only if no viewport info is given
            config["aspect_ratio_index"] = '25'
        else:
            for region in extension:
                config["custom_viewport_width" + region] = viewport_width if viewport_width > 0 else mode.width
                config[
                    "custom_viewport_height" + region] = viewport_height if viewport_height > 0 else mode.height
                config["custom_viewport_x" + region] = (mode.width - viewport_width) // 2 if viewport_width > 0 else 0
                config["custom_viewport_y" + region] = (
                                                               mode.height - viewport_height) // 2 if viewport_height > 0 else 0

        return config

    def select_width(self, system: Emulator, mode_viewport_width: int, rotation: int = 0):
        if rotation:
            if mode_viewport_width > 0:
                return mode_viewport_width
            else:
                return 0
        if mode_viewport_width > 0:
            return mode_viewport_width
        return 1840 + (system.CRTViewportWidth * 6)

    def get_default_mode_name_for_config(self, screentype: CRTScreenType, region: CRTVideoStandard,
                                         resolutiontype: CRTResolutionType):
        if screentype == CRTScreenType.k31:
            if resolutiontype == CRTResolutionType.DoubleFreq:
                return "1920@31KHz-double:all:240@120"
            else:
                return "default@31kHz:all:480@60"
        else:
            if region == "ntsc":
                return "default:ntsc:240@60"
            else:
                return "default:pal:288@50"

    def get_best_dimensions_for_arcade(self, system: Emulator, game:str, core: str):
        width = 0
        height = 0
        arcade_config = self.crt_config_parser.findArcadeGame(game, core)
        if arcade_config is None:
            for otherCore in self.arcade_cores:
                arcade_config = self.crt_config_parser.findArcadeGame(game, otherCore)
                if arcade_config is not None:
                    break
        if arcade_config is not None:
            width = arcade_config[2]
            height = arcade_config[3]
            if height == 0:
                # No height found in systems height we should use mode height if we found it
                mode = self.crt_config_parser.loadMode(arcade_config[1])
                if mode is not None:
                    height = mode.height
            if system.CRTScreenType == CRTScreenType.k31:
                if system.CRTResolutionType == CRTResolutionType.Progressive:
                    width = 640 * width / 1920
                    if height * 2 < 480:
                        height = height * 2
                if system.CRTResolutionType == CRTResolutionType.DoubleFreq:
                    if height > 240:
                        height = 240
            return width, height
        if system.CRTScreenType == CRTScreenType.k31 and system.CRTResolutionType == CRTResolutionType.Progressive:
            return 640, 0
        return 0, 0


    def get_best_dimensions_for_default(self, system: Emulator, arcadeGame: str="", core: str="") -> (int, int):
        width = 0
        height = 0
        for region in [CRTVideoStandard.ALL, CRTVideoStandard.NTSC, CRTVideoStandard.PAL]:
            if arcadeGame != "" and core != "":
                system_config = self.crt_config_parser.findArcadeGame(arcadeGame, core)
            else:
                system_config = self.crt_config_parser.findSystem(system.Name, region, CRTScreenType.k15,
                                                              CRTResolutionType.Progressive)
            if system_config is not None:
                width = system_config[4]
                height = system_config[5]
                if height == 0:
                    # No height found in systems height we should use mode height if we found it
                    mode = self.crt_config_parser.loadMode(system_config[3])
                    if mode is not None:
                        height = mode.height
                if system.CRTScreenType == CRTScreenType.k31:
                    if system.CRTResolutionType == CRTResolutionType.Progressive:
                        width = 640 * width / 1920
                        height = height * 2
                return width, height
        # No default values found, force 640 for 31khz
        if system.CRTScreenType == CRTScreenType.k31 and system.CRTResolutionType == CRTResolutionType.Progressive:
            return 640, 0
        return 0, 0

    def createConfigFor(self, system: Emulator, rom: str) -> typing.Dict[str, str]:
        config: typing.Dict[str, str] = {"aspect_ratio_index": "23",
                                         "video_aspect_ratio_auto": '"false"',
                                         "crt_switch_timings_pal": "",
                                         "crt_switch_timings_ntsc": "",
                                         "video_refresh_rate_pal": '"50"',
                                         "video_refresh_rate_ntsc": '"60"',
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
                                         "video_vsync": '"true"',
                                         "video_black_frame_insertion": '"0"',
                                         "audio_driver": '"pulse"'
                                         }
        core: str = system.Core
        default: bool = True
        game_name: str = Path(rom).stem
        recallog(
            "Starting configuration for game {} on system {}, screentype {}, resolutiontype {}, region {}".format(
                game_name, system.Name, system.CRTScreenType, system.CRTResolutionType, system.CRTVideoStandard),
            log_type="CRT")

        if system.CRTScreenType == CRTScreenType.k31 and system.CRTScanlines and system.CRTResolutionType == CRTResolutionType.Progressive:
            config.update({"video_shader_enable": '"true"'})
            config.update({"video_shader_dir": '"/recalbox/share/shaders/"'})
            config.update({"video_shader": '/recalbox/share/shaders/rrgbd-scanlines.glslp'})
        if system.CRTResolutionType == CRTResolutionType.DoubleFreq:
            config.update({"video_black_frame_insertion": '"1"'})
        if system.Name == "psx":
            config.update({"audio_driver": '"alsathread"'})
        if core in self.arcade_cores and system.Name != "neogeocd":
            config_core: str = core
            if config_core == "mame2003_plus":
                config_core = "mame2003"
            if system.CRTScreenType == CRTScreenType.k31:
                defaultMode: str = self.get_default_mode_name_for_config(CRTScreenType.k31, system.CRTVideoStandard,
                                                                         system.CRTResolutionType)
                width, height = self.get_best_dimensions_for_arcade(system, game_name, config_core)

                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                    config.update(
                        self.createConfigForMode(region,
                                                 self.crt_mode_processor.processMode(
                                                     self.crt_config_parser.loadMode(defaultMode),
                                                     system.CRTHorizontalOffset,
                                                     system.CRTVerticalOffset),
                                                 width,
                                                 height,
                                                 0)
                    )
                recallog("Setting 31kHz mode {} for arcade game".format(system.CRTResolutionType), log_type="CRT")
                default = False
            else:
                game_config: CRTArcadeMode = self.crt_config_parser.findArcadeGame(game_name, config_core)
                if game_config is not None:
                    mode_id = game_config[1]
                    mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id),
                                                               system.CRTHorizontalOffset, system.CRTVerticalOffset)
                    rotation = game_config[4]
                    recallog(
                        "Setting CRT mode for arcade game {} running with {} : {} {}".format(game_name, core, mode_id,
                                                                                             "vertical" if rotation == 1 else ""),
                        log_type="CRT")

                    config.update(
                        self.createConfigForMode("all", mode, self.select_width(system, game_config[2], rotation),
                                                 game_config[3], rotation))
                    default = False
                else:
                    recallog("Game configuration not found for {}".format(game_name), log_type="CRT")
        else:
            if system.CRTVideoStandard == CRTVideoStandard.AUTO:
                recallog("AUTO region selected", log_type="CRT")
                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC, CRTVideoStandard.ALL]:
                    system_config = self.crt_config_parser.findSystem(system.Name, region, system.CRTScreenType,
                                                                      system.CRTResolutionType)
                    if system_config is not None:
                        mode_id = system_config[3]
                        mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id),
                                                                   system.CRTHorizontalOffset,
                                                                   system.CRTVerticalOffset)
                        recallog(
                            "Setting {} mode for system {} running with {} : {}".format(region, system.Name, core,
                                                                                        mode_id),
                            log_type="CRT")
                        config.update(
                            self.createConfigForMode(region, mode, self.select_width(system, system_config[4]),
                                                     system_config[5], 0))
                        default = False
                    else:
                        recallog(
                            "System config not found : {} {} {} {}".format(system.Name, region, system.CRTScreenType,
                                                                           system.CRTResolutionType), log_type="CRT")
            else:
                recallog("Forced region {}".format(system.CRTVideoStandard), log_type="CRT")
                system_config = self.crt_config_parser.findSystem(system.Name, system.CRTVideoStandard,
                                                                  system.CRTScreenType,
                                                                  system.CRTResolutionType)
                if system_config is not None:
                    mode_id = system_config[3]
                    mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id),
                                                               system.CRTHorizontalOffset, system.CRTVerticalOffset)
                    for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                        recallog(
                            "Setting {} mode for system {} running with {} : {}".format(region, system.Name, core,
                                                                                        mode_id),
                            log_type="CRT")
                        config.update(
                            self.createConfigForMode(region, mode, self.select_width(system, system_config[4]),
                                                     system_config[5], 0))
                    default = False
        if default:
            recallog("Setting CRT default modes for {} on {}".format(game_name, system.Name), log_type="CRT")

            if system.CRTVideoStandard == CRTVideoStandard.AUTO:
                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                    defaultMode: str = self.get_default_mode_name_for_config(system.CRTScreenType, region,
                                                                             system.CRTResolutionType)
                    width, height = self.get_best_dimensions_for_default(system)
                    config.update(
                        self.createConfigForMode(region, self.crt_mode_processor.processMode(
                            self.crt_config_parser.loadMode(defaultMode), system.CRTHorizontalOffset,
                            system.CRTVerticalOffset),
                                                 width, height, 0))
                    recallog("Setting mode {} for {} with width {}".format(defaultMode, region, width), log_type="CRT")

            else:
                # This mode will be forced for all roms.
                defaultMode: str = self.get_default_mode_name_for_config(system.CRTScreenType, system.CRTVideoStandard,
                                                                         system.CRTResolutionType)
                width, height = self.get_best_dimensions_for_default(system)

                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                    config.update(
                        self.createConfigForMode(region, self.crt_mode_processor.processMode(
                            self.crt_config_parser.loadMode(defaultMode), system.CRTHorizontalOffset,
                            system.CRTVerticalOffset),
                                                 width, height, 0))
                    recallog("Setting mode {} for {} with width {}".format(defaultMode, region, width), log_type="CRT")
        if system.Name in ["wswan", "wswanc"]:
            config["video_vsync"] = '"false"'
        return config
