import typing
from pathlib import Path

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTConfigParser, CRTArcadeMode, CRTVideoStandard, CRTScreenType, \
    CRTResolutionType, CRTSystem
from configgen.crt.CRTModeGenerator import CRTModeGenerator
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.crt.CRTMonitorRanges import CRTMonitorRanges
from configgen.crt.CRTTypes import CRTArcadeGameV2
from configgen.crt.Mode import Mode
from configgen.utils.Rotation import Rotation
from configgen.utils.recallog import recallog


class LibretroConfigCRT:

    def __init__(self, crt_config_parser: CRTConfigParser, crt_mode_processor: CRTModeOffsetter, v2: bool = False):
        self.crt_config_parser = crt_config_parser
        self.crt_mode_processor = crt_mode_processor
        self.v2 = v2

    arcade_cores: [str] = ["fbneo", "mame2003", "mame2010", "mame2003_plus", "mame2015"]

    def createConfigForMode(self, region: str, mode: Mode, viewport_width: int, viewport_height: int, rotation: int) -> \
            typing.Dict[
                str, typing.Any]:
        config: typing.Dict[str, typing.Any] = {}
        if region == "all":
            config["crt_switch_timings_pal"] = '"' + mode.timings() + '"'
            config["crt_switch_timings_ntsc"] = '"' + mode.timings() + '"'
            config["video_refresh_rate_pal"] = '"{}"'.format(mode.emulator_refresh)
            config["video_refresh_rate_ntsc"] = '"{}"'.format(mode.emulator_refresh)
        else:
            config["crt_switch_timings_" + region] = '"' + mode.timings() + '"'
            config["video_refresh_rate_" + region] = '"{}"'.format(mode.emulator_refresh)

        config["aspect_ratio_index"] = '23'
        extension = ["_ntsc", "_pal"] if region == "all" else ["_" + region]

        if rotation == 1:
            config["video_smooth"] = '"true"'
            if viewport_height + viewport_width == 0:
                # Change the ratio to 1920 core only if no viewport info is given
                config["aspect_ratio_index"] = '25'
            if mode.width / mode.height < 1.5:
                # Change the ratio to core as we are 4/3 like mode (not super res)
                config["aspect_ratio_index"] = '22'
        else:
            for region in extension:
                config["custom_viewport_width" + region] = viewport_width if viewport_width > 0 else mode.width
                config[
                    "custom_viewport_height" + region] = viewport_height if viewport_height > 0 else mode.height
                config["custom_viewport_x" + region] = (mode.width - viewport_width) // 2 if viewport_width > 0 else 0
                config["custom_viewport_y" + region] = (
                                                               mode.height - viewport_height) // 2 if viewport_height > 0 else 0
                # For arcade, the viewport info by region seems not selected sometimes on retroarch so we set default values
                config["custom_viewport_x"] = (mode.width - viewport_width) // 2 if viewport_width > 0 else 0
                config["custom_viewport_y"] = (mode.height - viewport_height) // 2 if viewport_height > 0 else 0

        return config

    def select_width(self, system: Emulator, mode_viewport_width: int, mode: Mode, rotation: int = 0):
        if rotation:
            if mode_viewport_width > 0:
                return mode_viewport_width
            else:
                return 0
        if mode_viewport_width > 0:
            return mode_viewport_width
        return 1840 + (system.CRTViewportWidth(mode.extractCRTResolution()) * 6)

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

    def get_best_dimensions_for_arcade(self, system: Emulator, game: str, core: str):
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
                    # up to 256 px we double the height (rtype)
                    if height <= 256:
                        height = height * 2
                if system.CRTResolutionType == CRTResolutionType.DoubleFreq:
                    if height > 240:
                        height = 240
            return width, height
        if system.CRTScreenType == CRTScreenType.k31 and system.CRTResolutionType == CRTResolutionType.Progressive:
            return 640, 0
        return 0, 0

    def get_best_dimensions_for_default(self, system: Emulator, game: str = "", core: str = "") -> (int, int):
        width = 0
        height = 0
        for region in [CRTVideoStandard.ALL, CRTVideoStandard.NTSC, CRTVideoStandard.PAL]:
            if game != "" and core in self.arcade_cores:
                return self.get_best_dimensions_for_arcade(system, game, core)
            else:
                system_config: CRTSystem = self.crt_config_parser.findSystem(system.Name, system.Core, region,
                                                                             CRTScreenType.k15,
                                                                             CRTResolutionType.Progressive)
            if system_config is not None:
                width = system_config.viewport_width
                height = system_config.viewport_height
                if height == 0:
                    # No height found in systems height we should use mode height if we found it
                    mode = self.crt_config_parser.loadMode(system_config.mode_id)
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

    def __configureArcadeGameV2(self, system: Emulator, core: str, game_name: str,
                                config: typing.Dict[str, str | float | int]) -> typing.Dict[str, str | float | int]:
        game_config: CRTArcadeGameV2 = self.crt_config_parser.findArcadeGameV2(game_name, core)
        mode: Mode = CRTModeGenerator().generate(game_config.width * system.CRTSuperrez.multiplier(), game_config.height, game_config.vfreq,
                                                 CRTMonitorRanges[system.CRTScreenType], False)
        mode = self.crt_mode_processor.processMode(mode, system)

        config["crt_switch_timings_ntsc"] = '"' + mode.timings() + '"'
        config["video_refresh_rate_ntsc"] = '"{}"'.format(mode.emulator_refresh)
        config["custom_viewport_width_ntsc"] = mode.width
        config["custom_viewport_height_ntsc"] = mode.height
        config["video_scale_integer"] = "true"
        config["aspect_ratio_index"] = "23"
        if system.Rotation == Rotation.upsidedown:
            config["video_rotation"] = int(Rotation.upsidedown)
        if game_config.vertical:
            if system.Rotation == Rotation.none or system.Rotation == Rotation.upsidedown:
                config["aspect_ratio_index"] = "25"
                config["video_smooth"] = '"true"'
                config["video_scale_integer"] = '"false"'
            else:
                config["aspect_ratio_index"] = "23"
                config["video_rotation"] = int(system.Rotation)
        elif system.Rotation == Rotation.left or system.Rotation == Rotation.right:
            config["aspect_ratio_index"] = "23"
            config["video_smooth"] = '"true"'
            config["video_scale_integer"] = '"false"'
            config["video_rotation"] = int(system.Rotation)
            width: int = game_config.height*3/4*system.CRTSuperrez.multiplier()
            config["custom_viewport_width_ntsc"] = width
            config["custom_viewport_height_ntsc"] = game_config.height
            config["custom_viewport_x_ntsc"] = (mode.width - width) // 2


        return config

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
                                         "video_fullscreen": '"true"',
                                         "video_fullscreen_x": '',
                                         "video_fullscreen_y": '',
                                         "menu_driver": '"rgui"',
                                         "video_shader_enable": '"false"',
                                         "video_vsync": '"true"',
                                         "video_black_frame_insertion": '"0"',
                                         "audio_driver": '"pulse"',
                                         "video_smooth": '"false"',
                                         "video_allow_rotate": '"true"',
                                         "video_rotation": "0"
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
        if system.Core == "swanstation":
            config.update({"audio_driver": '"alsathread"'})

        config_core: str = core
        if config_core == "mame2003_plus":
            config_core = "mame2003"
        if config_core == "mame2015":
            config_core = "mame2010"

        # Arcade
        if core in self.arcade_cores and system.Name != "neogeocd":
            if self.v2:
                # Debrayage en V2
                return self.__configureArcadeGameV2(system, config_core, game_name, config)
            else:
                game_config: CRTArcadeMode = self.crt_config_parser.findArcadeGame(game_name, config_core)
                rotation = game_config[4] if game_config is not None else 0

                if system.CRTScreenType == CRTScreenType.k31:
                    defaultMode: str = self.get_default_mode_name_for_config(CRTScreenType.k31, system.CRTVideoStandard,
                                                                             system.CRTResolutionType)
                    width, height = self.get_best_dimensions_for_default(system, game_name, config_core)

                    for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                        config.update(
                            self.createConfigForMode(region,
                                                     self.crt_mode_processor.processMode(
                                                         self.crt_config_parser.loadMode(defaultMode),
                                                         system),
                                                     width,
                                                     height,
                                                     rotation)
                        )
                    recallog("Setting 31kHz mode {} for arcade game".format(system.CRTResolutionType), log_type="CRT")
                    default = False
                else:
                    # Arcade 15kHz
                    if system.CRTVideoStandard == CRTVideoStandard.AUTO or system.CRTVideoStandard == CRTVideoStandard.NTSC:
                        if game_config is not None:
                            rotation = game_config[4]
                            mode_id = game_config[1]
                            mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id), system)
                            recallog(
                                "Setting CRT mode for arcade game {} running with {} : {} {}".format(game_name, core,
                                                                                                     mode_id,
                                                                                                     "vertical" if rotation == 1 else ""),
                                log_type="CRT")

                            config.update(
                                self.createConfigForMode("all", mode,
                                                         self.select_width(system, game_config[2], mode, rotation),
                                                         game_config[3], rotation))
                            default = False
                        else:
                            recallog("Game configuration not found for {}".format(game_name), log_type="CRT")
        else:
            # Consoles
            if system.CRTVideoStandard == CRTVideoStandard.AUTO:
                recallog("AUTO region selected", log_type="CRT")
                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC, CRTVideoStandard.ALL]:
                    system_config: CRTSystem = self.crt_config_parser.findSystem(system.Name, system.Core, region,
                                                                                 system.CRTScreenType,
                                                                                 system.CRTResolutionType)
                    if system_config is not None:
                        mode_id = system_config.mode_id
                        mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id),
                                                                   system)
                        recallog(
                            "Setting {} mode for system {} running with {} : {}".format(region, system.Name, core,
                                                                                        mode_id),
                            log_type="CRT")
                        config.update(
                            self.createConfigForMode(region, mode,
                                                     self.select_width(system, system_config.viewport_width, mode),
                                                     system_config.viewport_height, 0))
                        default = False
                    else:
                        recallog(
                            "System config not found : {} {} {} {}".format(system.Name, region, system.CRTScreenType,
                                                                           system.CRTResolutionType), log_type="CRT")
            else:
                recallog("Forced region {}".format(system.CRTVideoStandard), log_type="CRT")
                system_config: CRTSystem = self.crt_config_parser.findSystem(system.Name, system.Core,
                                                                             system.CRTVideoStandard,
                                                                             system.CRTScreenType,
                                                                             system.CRTResolutionType)
                if system_config is not None:
                    mode_id = system_config.mode_id
                    mode = self.crt_mode_processor.processMode(self.crt_config_parser.loadMode(mode_id),
                                                               system)
                    for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                        recallog(
                            "Setting {} mode for system {} running with {} : {}".format(region, system.Name, core,
                                                                                        mode_id),
                            log_type="CRT")
                        config.update(
                            self.createConfigForMode(region, mode,
                                                     self.select_width(system, system_config.viewport_width, mode),
                                                     system_config.viewport_height, 0))
                    default = False
        if default:
            recallog("Setting CRT default modes for {} on {}".format(game_name, system.Name), log_type="CRT")

            if system.CRTVideoStandard == CRTVideoStandard.AUTO:
                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                    defaultMode: str = self.get_default_mode_name_for_config(system.CRTScreenType, region,
                                                                             system.CRTResolutionType)

                    width, height = self.get_best_dimensions_for_default(system, game_name, config_core)
                    config.update(
                        self.createConfigForMode(region, self.crt_mode_processor.processMode(
                            self.crt_config_parser.loadMode(defaultMode), system),
                                                 width, height, 0))
                    recallog("Setting mode {} for {} with width {}".format(defaultMode, region, width), log_type="CRT")

            else:
                # This mode will be forced for all roms.
                defaultMode: str = self.get_default_mode_name_for_config(system.CRTScreenType, system.CRTVideoStandard,
                                                                         system.CRTResolutionType)
                width, height = self.get_best_dimensions_for_default(system, game_name, config_core)

                for region in [CRTVideoStandard.PAL, CRTVideoStandard.NTSC]:
                    config.update(
                        self.createConfigForMode(region, self.crt_mode_processor.processMode(
                            self.crt_config_parser.loadMode(defaultMode), system),
                                                 width, height, 0))
                    recallog("Setting mode {} for {} with width {}".format(defaultMode, region, width), log_type="CRT")
        if system.Name in ["wswan", "wswanc"]:
            config["video_vsync"] = '"false"'
        return config
