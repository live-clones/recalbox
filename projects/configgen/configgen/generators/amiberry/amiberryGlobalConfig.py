#!/usr/bin/env python
import os

import configgen.recalboxFiles as recalboxFiles
from configgen.generators.amiberry.amiberryKickstarts import KickstartManager
from configgen.settings.keyValueSettings import keyValueSettings


class AmiberryGlobalConfig:

    def __init__(self, userGlobalSettingsFile, finalGlobalSettingsFile):
        self.globalSettingsFile = finalGlobalSettingsFile
        self.userSettingsFile = userGlobalSettingsFile

    def createGlobalSettings(self, verbose: bool, scanline: bool):
        # Load user settings first
        settings = keyValueSettings(self.userSettingsFile)
        settings.loadFile(True).defineBool('yes', 'no')

        # Set default settings if they do not exists
        settings.setDefaultBool("read_config_descriptions", True)
        settings.setDefaultBool("write_logfile", verbose)
        settings.setDefaultBool("scanlines_by_default", scanline)
        settings.setDefaultInt("speedup_cycles_jit_pal", 10000)
        settings.setDefaultInt("speedup_cycles_jit_ntsc", 6667)
        settings.setDefaultInt("speedup_cycles_nonjit", 256)
        settings.setDefaultInt("speedup_timelimit_jit", -5000)
        settings.setDefaultInt("speedup_timelimit_nonjit", -5000)
        settings.setDefaultInt("speedup_timelimit_jit_turbo", 0)
        settings.setDefaultInt("speedup_timelimit_nonjit_turbo", 0)
        settings.setDefaultInt("default_horizontal_centering", 1)
        settings.setDefaultInt("default_vertical_centering", 1)
        settings.setDefaultInt("default_correct_aspect_ratio", 1)
        settings.setDefaultInt("default_frame_skip", 1)
        settings.setDefaultInt("default_fullscreen", 1)
        settings.setDefaultInt("default_scaling_method", -1)

        # Forced values
        settings.setInt("Quickstart", 1)
        settings.setString("path", recalboxFiles.amiberryMountPoint)
        settings.setString("config_path", recalboxFiles.amiberryMountPoint + "/conf")
        settings.setString("controllers_path", recalboxFiles.amiberryMountPoint + "/conf")
        settings.setString("retroarch_config", recalboxFiles.retroarchCustom)
        settings.setString("rom_path", recalboxFiles.BIOS + '/')

        # Save file
        settings.changeSettingsFile(self.globalSettingsFile)
        settings.saveFile()

        # Hack the settings to add non-unique key/value tupples
        with open(self.globalSettingsFile, "a") as sf:
            sf.write("ROMs=" + str(len(KickstartManager.BIOS_LIST)) + '\n')
            for rom in KickstartManager.BIOS_LIST:
                itype, name = KickstartManager.BIOS_LIST[rom]
                path = os.path.join(recalboxFiles.BIOS, rom + ".rom")
                sf.write("ROMName=" + name + '\n')
                sf.write("ROMPath=" + path + '\n')
                sf.write("ROMType=" + str(itype) + '\n')
