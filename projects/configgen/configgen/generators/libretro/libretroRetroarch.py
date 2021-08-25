import platform
import os
import configgen.recalboxFiles as recalboxFiles
from Emulator import Emulator


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

    def languageIndex(self, language):
        try:
            return str(self.aiLanguages.index(language))
        except ValueError:
            return "0"

    def __init__(self, system, settings, controllers, demo, recalboxSettings):
        self.recalboxSettings = recalboxSettings
        self.system = system
        self.settings = settings
        self.controllers = controllers
        self.demo = demo

    # Return true if the option is considered enabled (for boolean options)
    def isEnabled(self, key):
        recalbox = self.system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    # Return true if the option is considered defined
    def isDefined(self, key):
        recalbox = self.system.config
        return key in recalbox and (isinstance(recalbox[key], str) or isinstance(recalbox[key], str)) and len(recalbox[key]) > 0

    # Get an option fromp system config or from global config
    # TODO: should be in self.system.config
    def getOption(self, key, default):
        settings = self.recalboxSettings
        systemkey = "{}.{}".format(self.system.config, key)
        if settings.hasOption(systemkey):
            return settings.getOption(systemkey, default)
        globalkey = "{}.{}".format("global", key)
        return settings.getOption(globalkey, default)

    # Is nVidia driver on?
    @staticmethod
    def hasnVidiaDriver():
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
        recalbox = self.system.config

        # Screen resolution
        from configgen.utils.resolutions import ResolutionParser
        resolution = ResolutionParser(recalbox['videomode'])
        settings.setOption("video_fullscreen", self.TRUE)
        if resolution.isSet and resolution.selfProcess:
            settings.setOption("video_fullscreen_x", str(resolution.width))
            settings.setOption("video_fullscreen_y", str(resolution.height))
        else:
            settings.removeOption("video_fullscreen_x")
            settings.removeOption("video_fullscreen_y")

        # AI System
        aiOn = self.getOption("translate", "1")
        aiFrom = self.getOption("translate.from", "auto").upper()
        aiTo = self.getOption("translate.to", self.recalboxSettings.getOption("system.language", "auto"))
        aiTo = aiTo.upper()
        if len(aiTo) > 2:
            if not aiTo.startswith("ZH") and not aiTo.startswith("AUTO"):  # Keep chinese region
                aiTo = aiTo[:2]
        aiKey = self.getOption("translate.apikey", "")
        aiUrl = self.getOption("translate.url", "")
        settings.setOption("ai_service_enable", self.TRUE if aiOn and aiKey != "" else self.FALSE)
        settings.setDefaultOption("ai_service_mode", "0")
        settings.setOption("ai_service_source_lang", self.languageIndex(aiFrom))
        settings.setOption("ai_service_target_lang", self.languageIndex(aiTo))
        if aiUrl == "":
            if aiKey == "":
                aiUrl = "http://localhost:4404/"
            else:
                aiUrl = "http://ztranslate.net/service?api_key={}".format(aiKey)
        settings.setOption("ai_service_url", aiUrl)

        # Threaded video
        settings.setOption("video_threaded", self.FALSE)

        # Enable RetroArch option "quit_press_twice"
        quitPressTwice = self.getOption("quitpresstwice", "0")
        settings.setOption("quit_press_twice", self.TRUE if quitPressTwice == "1" else self.FALSE)

        # Control new RA 1.7.7 key: do not allow upscaling higher than x4
        rguiUpscaling = settings.getOption("rgui_internal_upscale_level", "1").strip('"')
        if rguiUpscaling.isdigit():
            if int(rguiUpscaling) > 4:
                settings.setOption("rgui_internal_upscale_level", "4")
        # Allow extended ASCII
        settings.setOption("rgui_extended_ascii", self.TRUE)

        # Smoothing?
        settings.setOption("video_smooth", self.TRUE if self.isEnabled("smooth") else self.FALSE)

        # Shaders?
        hasShaders = self.isDefined("shaders")
        settings.setOption("video_shader_enable", self.TRUE if hasShaders else self.FALSE)
        settings.setOption("video_shader_dir", os.path.dirname(recalbox["shaders"]) if hasShaders else recalboxFiles.shadersRoot)
        if hasShaders:
            settings.setOption("video_shader", os.path.basename(recalbox["shaders"]))

        # Screen ratio
        hasRatio = self.isDefined("ratio")
        if hasRatio:
            if recalbox["ratio"] in self.RATIO_INDEXES:
                settings.setOption("aspect_ratio_index", self.RATIO_INDEXES.index(recalbox['ratio']))
                settings.setOption("video_aspect_ratio_auto", self.FALSE)
            elif recalbox["ratio"] == "none":
                # Do not fix aspect ratio. Let the user play with RA
                settings.setOption("video_aspect_ratio_auto", self.FALSE)
            else:
                # default: auto
                settings.setOption("video_aspect_ratio_auto", self.TRUE)
                settings.removeOption("aspect_ratio_index")

        # Rewind enabled?
        hasRewind = self.isEnabled("rewind") and self.system.name not in self.SYSTEM_WITH_NO_REWIND
        settings.setOption("rewind_enable", self.TRUE if hasRewind else self.FALSE)

        # Auto-save (not in demo mode! not in netplay mode!)
        hasAutoSave = self.isEnabled("autosave") and not self.demo and not self.isDefined("netplaymode")
        settings.setOption("savestate_auto_save", self.TRUE if hasAutoSave else self.FALSE)
        settings.setOption("savestate_auto_load", self.TRUE if hasAutoSave else self.FALSE)

        # Save folders
        settings.setOption("savestate_directory", recalboxFiles.savesDir + self.system.name)
        settings.setOption("savefile_directory", recalboxFiles.savesDir + self.system.name)

        # Extend Controller configurations
        for i in range(0, 11):
            settings.setOption("input_libretro_device_p{}".format(i), "1")
        core = self.system.config['core']
        # Generic
        if core in self.CORE_TO_PLAYER1_DEVICE:
          settings.setOption("input_libretro_device_p1", self.CORE_TO_PLAYER1_DEVICE[core])
        if core in self.CORE_TO_PLAYER2_DEVICE:
          settings.setOption("input_libretro_device_p2", self.CORE_TO_PLAYER2_DEVICE[core])
        # Sens9x2010 case
        if len(self.controllers) > 2 and self.system.config['core'] == 'snes9x2010':
            settings.setOption("input_libretro_device_p2", "257")
        # Fuse case - Force keyboard as P3
        if self.system.config['core'] == 'fuse':
            settings.setOption("input_libretro_device_p3", "259")
        # 81 case - Force keyboard as P2
        if self.system.config['core'] == '81':
            settings.setOption("input_libretro_device_p2", "259")

        # Retroachievements (not in demo mode!)
        hasRetroAch = self.isEnabled("retroachievements") and not self.demo
        settings.setOption("cheevos_enable", self.TRUE if hasRetroAch else self.FALSE)
        settings.setOption("cheevos_hardcore_mode_enable", self.FALSE)
        if hasRetroAch and self.system.name in self.SYSTEM_COMPATIBLE_RETROACHIEVEMENTS:
            settings.setOption("cheevos_username", recalbox.get('retroachievements.username', ""))
            settings.setOption("cheevos_password", recalbox.get('retroachievements.password', ""))
            hardcore = self.isEnabled("retroachievements.hardcore")
            settings.setOption("cheevos_hardcore_mode_enable", self.TRUE if hardcore else self.FALSE)

        # Integer scale
        intScale = self.isEnabled("integerscale")
        settings.setOption("video_scale_integer", self.TRUE if intScale else self.FALSE)

        # Display FPS
        fps = self.isEnabled("showFPS")
        settings.setOption("fps_show", self.TRUE if fps else self.FALSE)
        settings.setOption("fps_update_interval", "256")

        # Display framecount
        framecount = self.isEnabled("framecount")
        settings.setOption("framecount_show", self.TRUE if framecount else self.FALSE)

        # Netplay management
        settings.removeOption("netplay_password")
        settings.removeOption("netplay_spectate_password")
        settings.removeOption("netplay_start_as_spectator")
        if self.isDefined("netplaymode"):
            mode = self.system.config["netplaymode"]
            if mode in self.NETPLAY_MODES:
                settings.setOption("netplay_use_mitm_server", self.FALSE)
                if mode == 'host':
                    mitm = recalbox.get("netplay.relay", None)
                    if mitm is not None:
                        settings.setOption("netplay_use_mitm_server", self.TRUE)
                        settings.setOption("netplay_mitm_server", mitm)
                if mode == 'client':
                    settings.setOption("netplay_start_as_spectator", self.TRUE if self.system.config["netplay_vieweronly"] else self.FALSE)
                # Netplay passwords
                settings.setOption("netplay_password", '"' + self.system.config["netplay_playerpassword"] + '"')
                settings.setOption("netplay_spectate_password", '"' + self.system.config["netplay_viewerpassword"] + '"')
