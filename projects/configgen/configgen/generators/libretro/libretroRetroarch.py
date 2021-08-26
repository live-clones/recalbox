import platform
import os
import configgen.recalboxFiles as recalboxFiles
from configgen.Emulator import Emulator
from configgen.controllersConfig import ControllerDictionary
from configgen.settings.keyValueSettings import keyValueSettings


class LibretroRetroarch:

    TRUE = "true"
    FALSE = "false"

    IS_TRUE = ("1", "true")

    # Warning: The values in the array must be exactly at the same index than
    # https://github.com/libretro/RetroArch/blob/master/retroarch.c#L3042
    RATIO_INDEXES = \
    [
        "4/3",
        "16/9",
        "16/10",
        "16/15",
        "21/9",
        "1/1",
        "2/1",
        "3/2",
        "3/4",
        "4/1",
        "9/16",
        "5/4",
        "6/5",
        "7/9",
        "8/3",
        "8/7",
        "19/12",
        "19/14",
        "30/17",
        "32/9",
        "config",
        "squarepixel",
        "coreprovided",
        "custom",
    ]

    # Define systems not compatible with rewind option
    SYSTEM_WITH_NO_REWIND = \
    (
        'virtualboy',
        'sega32x',
        'segacd',
        'psx',
        'fba_libretro',
        'vectrex',
        'zxspectrum',
        'mame',
        'n64',
        '3do',
        'jaguar',
        'intellivision',
        'channelf',
        'neogeocd',
        'dreamcast',
        'atomiswave',
        'naomi',
        'ngpc',
        'pcfx',
        'dos',
    )

    # Define the libretro device type corresponding to the libretro cores, when needed.
    CORE_TO_PLAYER1_DEVICE = \
    {
        'cap32': '513',
        '81': '257',
        'fuse': '513',
        'atari800': '513'
    }
    CORE_TO_PLAYER2_DEVICE = \
    {
        'fuse': '513',
        'atari800': '513'
    }

    # Netplay modes
    NETPLAY_MODES = ('host', 'client')

    # Define systems compatible with retroachievements
    SYSTEM_COMPATIBLE_RETROACHIEVEMENTS = \
    (
        'snes',
        'nes',
        'gba',
        'gb',
        'gbc',
        'megadrive',
        'pcengine',
        'atari2600',
        'fba_libretro',
        'neogeo',
        'virtualboy',
        'ngp',
        'ngpc',
        'lynx',
        'mastersystem',
        'gamegear',
        'n64',
        'satellaview',
        'sufami',
        'neogeocd',
        'colecovision',
        'atari7800'
    )

    # AI Service language indexes - Keep in sync with
    # https://github.com/libretro/RetroArch/blob/master/translation_defines.h
    aiLanguages =\
    [
        "AUTO",
        "EN",
        "ES",
        "FR",
        "IT",
        "DE",
        "JP",
        "NL",
        "CS",
        "DA",
        "SV",
        "HR",
        "KO",
        "ZH_CN",
        "ZH_TW",
        "CA",
        "BG",
        "BN",
        "EU",
        "AZ",
        "AR",
        "SQ",
        "AF",
        "EO",
        "ET",
        "TL",
        "FI",
        "GL",
        "KA",
        "EL",
        "GU",
        "HT",
        "IW",
        "HI",
        "HU",
        "IS",
        "ID",
        "GA",
        "KN",
        "LA",
        "LV",
        "LT",
        "MK",
        "MS",
        "MT",
        "NO",
        "FA",
        "PL",
        "PT",
        "RO",
        "RU",
        "SR",
        "SK",
        "SL",
        "SW",
        "TA",
        "TE",
        "TH",
        "TR",
        "UK",
        "UR",
        "VI",
        "CY",
        "YI"
    ]

    def languageIndex(self, language: str) -> str:
        try:
            return str(self.aiLanguages.index(language))
        except ValueError:
            return "0"

    def __init__(self, system: Emulator, settings: keyValueSettings, controllers: ControllerDictionary, demo: bool, recalboxSettings: keyValueSettings):
        self.recalboxSettings: keyValueSettings = recalboxSettings
        self.system: Emulator = system
        self.settings: keyValueSettings = settings
        self.controllers: ControllerDictionary = controllers
        self.demo: bool = demo

    # Is nVidia driver on?
    @staticmethod
    def hasnVidiaDriver() -> bool:
        arch = platform.machine()
        if arch == "x86_64":
            if os.path.exists("/etc/modprobe.d/blacklist.conf"):
               for line in open("/etc/modprobe.d/blacklist.conf"):
                   if "blacklist nouveau" in line:
                       return True
        return False

    # Fill retroarch configuration
    def fillRetroarchConfiguration(self):
        settings = self.settings

        # Screen resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(self.system.VideoMode)
        settings.setOption("video_fullscreen", self.TRUE)
        if resolution.isSet and resolution.selfProcess:
            settings.setOption("video_fullscreen_x", str(resolution.width))
            settings.setOption("video_fullscreen_y", str(resolution.height))
        else:
            settings.removeOption("video_fullscreen_x")
            settings.removeOption("video_fullscreen_y")

        # AI System
        aiFrom = self.system.TranslateFrom.upper()
        aiTo = self.system.TranslateTo.upper()
        if len(aiTo) > 2:
            if not aiTo.startswith("ZH") and not aiTo.startswith("AUTO"):  # Keep chinese region
                aiTo = aiTo[:2]
        aiKey = self.system.TranslateAPIKey
        aiUrl = self.system.TranslateURL
        settings.setOption("ai_service_enable", self.TRUE if self.system.Translate and aiKey != "" else self.FALSE)
        settings.setDefaultOption("ai_service_mode", "0")
        settings.setOption("ai_service_source_lang", self.languageIndex(aiFrom))
        settings.setOption("ai_service_target_lang", self.languageIndex(aiTo))
        if len(aiKey) == 0: aiKey = "RECALBOX"
        if len(aiUrl) == 0: aiUrl = "https://ztranslate.net/service?api_key={}".format(aiKey)
        settings.setOption("ai_service_url", aiUrl)

        # Threaded video
        settings.setOption("video_threaded", self.FALSE)

        # Enable RetroArch option "quit_press_twice"
        settings.setOption("quit_press_twice", self.TRUE if self.system.QuitTwice else self.FALSE)

        # Control new RA 1.7.7 key: do not allow upscaling higher than x4
        rguiUpscaling = settings.getOption("rgui_internal_upscale_level", "1").strip('"')
        if rguiUpscaling.isdigit():
            if int(rguiUpscaling) > 4:
                settings.setOption("rgui_internal_upscale_level", "4")
        # Allow extended ASCII
        settings.setOption("rgui_extended_ascii", self.TRUE)

        # Smoothing?
        settings.setOption("video_smooth", self.TRUE if self.system.Smooth else self.FALSE)

        # Shaders?
        settings.setOption("video_shader_enable", self.TRUE if self.system.HasShaderFile else self.FALSE)
        settings.setOption("video_shader_dir", os.path.dirname(self.system.ShaderFile) if self.system.HasShaderFile else recalboxFiles.shadersRoot)
        if self.system.HasShaderFile:
            settings.setOption("video_shader", os.path.basename(self.system.ShaderFile))

        # Screen ratio
        if self.system.Ratio in self.RATIO_INDEXES:
            settings.setOption("aspect_ratio_index", self.RATIO_INDEXES.index(self.system.Ratio))
            settings.setOption("video_aspect_ratio_auto", self.FALSE)
        elif self.system.Ratio == "none":
            # Do not fix aspect ratio. Let the user play with RA
            settings.setOption("video_aspect_ratio_auto", self.FALSE)
        else:
            # default: auto
            settings.setOption("video_aspect_ratio_auto", self.TRUE)
            settings.removeOption("aspect_ratio_index")

        # Rewind enabled?
        hasRewind = self.system.Rewind and self.system.Name not in self.SYSTEM_WITH_NO_REWIND
        settings.setOption("rewind_enable", self.TRUE if hasRewind else self.FALSE)

        # Auto-save (not in demo mode! not in netplay mode!)
        hasAutoSave = self.system.AutoSave and not self.demo and not self.system.Netplay
        settings.setOption("savestate_auto_save", self.TRUE if hasAutoSave else self.FALSE)
        settings.setOption("savestate_auto_load", self.TRUE if hasAutoSave else self.FALSE)

        # Save folders
        settings.setOption("savestate_directory", recalboxFiles.savesDir + self.system.Name)
        settings.setOption("savefile_directory", recalboxFiles.savesDir + self.system.Name)

        # Extend Controller configurations
        for i in range(0, 11):
            settings.setOption("input_libretro_device_p{}".format(i), "1")
        core = self.system.Core
        # Generic
        if core in self.CORE_TO_PLAYER1_DEVICE:
          settings.setOption("input_libretro_device_p1", self.CORE_TO_PLAYER1_DEVICE[core])
        if core in self.CORE_TO_PLAYER2_DEVICE:
          settings.setOption("input_libretro_device_p2", self.CORE_TO_PLAYER2_DEVICE[core])
        # Sens9x2010 case
        if len(self.controllers) > 2 and self.system.Core == 'snes9x2010':
            settings.setOption("input_libretro_device_p2", "257")
        # Fuse case - Force keyboard as P3
        if self.system.Core == 'fuse':
            settings.setOption("input_libretro_device_p3", "259")
        # 81 case - Force keyboard as P2
        if self.system.Core == '81':
            settings.setOption("input_libretro_device_p2", "259")

        # Retroachievements (not in demo mode!)
        hasRetroAch = self.system.Retroachievements and not self.demo
        settings.setOption("cheevos_enable", self.TRUE if hasRetroAch else self.FALSE)
        settings.setOption("cheevos_hardcore_mode_enable", self.FALSE)
        if hasRetroAch and self.system.Name in self.SYSTEM_COMPATIBLE_RETROACHIEVEMENTS:
            settings.setOption("cheevos_username", self.system.RetroachievementsUsername)
            settings.setOption("cheevos_password", self.system.RetroachievementsPassword)
            settings.setOption("cheevos_hardcore_mode_enable", self.TRUE if self.system.RetroachievementsHardcore else self.FALSE)

        # Integer scale
        settings.setOption("video_scale_integer", self.TRUE if self.system.IntegerScale else self.FALSE)

        # Display FPS
        settings.setOption("fps_show", self.TRUE if self.system.ShowFPS else self.FALSE)
        settings.setOption("fps_update_interval", "256")

        # Netplay management
        settings.removeOption("netplay_password")
        settings.removeOption("netplay_spectate_password")
        settings.removeOption("netplay_start_as_spectator")
        if self.system.Netplay:
            settings.setOption("netplay_use_mitm_server", self.FALSE)
            if self.system.NetplayHostMode:
                if self.system.HasNetplayMITM:
                    settings.setOption("netplay_use_mitm_server", self.TRUE)
                    settings.setOption("netplay_mitm_server", self.system.NetplayMITM)
            else:
                settings.setOption("netplay_start_as_spectator", self.TRUE if self.system.NetplayViewerOnly else self.FALSE)
            # Netplay passwords
            settings.setOption("netplay_password", '"' + self.system.NetplayPlayerPassword + '"')
            settings.setOption("netplay_spectate_password", '"' + self.system.NetplayViewerPassword + '"')
