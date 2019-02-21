from configgen import recalboxFiles


class LibretroRetroarch:

    TRUE = "true"
    FALSE = "false"

    IS_TRUE = ("1", "true")

    # Warning: The values in the array must be exactly at the same index than
    # https://github.com/libretro/RetroArch/blob/master/gfx/video_driver.c#L234
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

    def __init__(self, system, settings, controllers):
        self.system = system
        self.settings = settings
        self.controllers = controllers

    # Return true if the option is considered enabled (for boolean options)
    def isEnabled(self, key):
        recalbox = self.system.config
        return key in recalbox and recalbox[key] in self.IS_TRUE

    # Return true if the option is considered defined
    def isDefined(self, key):
        recalbox = self.system.config
        return key in recalbox and isinstance(recalbox[key], str) and len(recalbox[key]) > 0

    # Fill retroarch configuration
    def fillRetroarchConfiguration(self):
        settings = self.settings
        recalbox = self.system.config

        # Smoothing?
        settings.setOption("video_smooth", self.TRUE if self.isEnabled("smooth") else self.FALSE)

        # Shaders?
        hasShaders = self.isDefined("shaders")
        settings.setOption("video_shader_enable", self.TRUE if hasShaders else self.FALSE)
        if hasShaders:
            settings.setOption("video_shader", recalbox["shaders"])
            settings.setOption("video_smooth", self.FALSE)

        # Screen ratio
        hasRatio = self.isDefined("ratio")
        if hasRatio:
            if recalbox["ratio"] in self.RATIO_INDEXES:
                settings.setOption("aspect_ratio_index", self.RATIO_INDEXES.index(recalbox['ratio']))
                settings.setOption("video_aspect_ratio_auto", self.FALSE)
            elif recalbox["ratio"] == "custom":
                settings.setOption("video_aspect_ratio_auto", self.FALSE)
            else:
                settings.setOption("video_aspect_ratio_auto", self.TRUE)
                settings.setOption("aspect_ratio_index", "")

        # Rewind enabled?
        hasRewind = self.isEnabled("rewind") and self.system.name not in self.SYSTEM_WITH_NO_REWIND
        settings.setOption("rewind_enable", self.TRUE if hasRewind else self.FALSE)

        # Auto-save
        hasAutoSave = self.isEnabled("autosave")
        settings.setOption("savestate_auto_save", self.TRUE if hasAutoSave else self.FALSE)
        settings.setOption("savestate_auto_load", self.TRUE if hasAutoSave else self.FALSE)

        # Save folders
        settings.setOption("savestate_directory", recalboxFiles.savesDir + self.system.name)
        settings.setOption("savefile_directory", recalboxFiles.savesDir + self.system.name)

        # Extend Controller configurations
        core = self.system.config['core']
        settings.setOption("input_libretro_device_p1", self.CORE_TO_PLAYER1_DEVICE[core] if core in self.CORE_TO_PLAYER1_DEVICE else "1")
        settings.setOption("input_libretro_device_p2", self.CORE_TO_PLAYER2_DEVICE[core] if core in self.CORE_TO_PLAYER2_DEVICE else "1")
        # Special cases
        if len(self.controllers) > 2 and self.system.config['core'] == 'snes9x_next':
            settings.setOption("input_libretro_device_p2", "257")

        # Retroachievements
        hasRetroAch = self.isEnabled("retroachievements")
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

        # Display framecount
        framecount = self.isEnabled("framecount")
        settings.setOption("framecount_show", self.TRUE if framecount else self.FALSE)

        # Netplay management
        if self.isDefined("netplaymode"):
            mode = self.system.config["netplaymode"]
            if mode in self.NETPLAY_MODES:
                settings.setOption("netplay_use_mitm_server", self.FALSE)
                if mode == 'host':
                    mitm = recalbox.get("netplay.relay", None)
                    if mitm is not None:
                        settings.setOption("netplay_use_mitm_server", self.TRUE)
                        settings.setOption("netplay_mitm_server", mitm)
