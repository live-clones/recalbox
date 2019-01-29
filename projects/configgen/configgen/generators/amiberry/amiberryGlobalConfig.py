#!/usr/bin/env python
import recalboxFiles
from settings.keyValueSettings import keyValueSettings


class AmiberryGlobalConfig:

    def __init__(self, userGlobalSettingsFile, finalGlobalSettingsFile):
        self.globalSettingsFile = finalGlobalSettingsFile
        self.userSettingsFile = userGlobalSettingsFile

    def createGlobalSettings(self):
        # Load user settings first
        settings = keyValueSettings(self.userSettingsFile)
        settings.loadFile(True)

        # Set default settings if they do not exists
        settings.setDefaultOption("read_config_descriptions", "yes")
        settings.setDefaultOption("write_logfile", "yes")
        settings.setDefaultOption("scanlines_by_default", "no")
        settings.setDefaultOption("speedup_cycles_jit_pal", "10000")
        settings.setDefaultOption("speedup_cycles_jit_ntsc", "6667")
        settings.setDefaultOption("speedup_cycles_nonjit", "256")
        settings.setDefaultOption("speedup_timelimit_jit", "-5000")
        settings.setDefaultOption("speedup_timelimit_nonjit", "-5000")
        settings.setDefaultOption("speedup_timelimit_jit_turbo", "0")
        settings.setDefaultOption("speedup_timelimit_nonjit_turbo", "0")

        # Forced values
        settings.setOption("Quickstart", "1")
        settings.setOption("path", recalboxFiles.amiberryMountPoint)
        settings.setOption("config_path", recalboxFiles.amiberryMountPoint + "/conf")
        settings.setOption("controllers_path", recalboxFiles.amiberryMountPoint + "/conf")
        settings.setOption("retroarch_config", recalboxFiles.amiberryMountPoint + "/conf/retroarch.cfg")
        settings.setOption("rom_path", recalboxFiles.BIOS + '/')

        # Save file
        settings.changeSettingsFile(self.globalSettingsFile)
        settings.saveFile()

        # Hack the settings to add non-unique key/value tupples
        roms =[
            ("CD32 KS ROM v3.1 rev 40.60 (512k)", "/recalbox/share/bios/cd32.rom", "512"),
            ("CD32 extended ROM rev 40.60 (512k)", "/recalbox/share/bios/cd32ext.rom", "1024"),
            ("CDTV extended ROM v2.07 (256k)", "/recalbox/share/bios/cdtvext.rom", "2048"),
            ("KS ROM v1.3 (A500,A1000,A2000) rev 34.5 (256k) [315093-02]", "/recalbox/share/bios/kick13.rom", "256"),
            ("KS ROM v2.05 (A600HD) rev 37.300 (512k) [391304-01]", "/recalbox/share/bios/kick20.rom", "256"),
            ("KS ROM v3.1 (A1200) rev 40.68 (512k) [391773-01/391774-01]", "/recalbox/share/bios/kick31.rom", "256"),
            ("Freezer: HRTMon v2.37 (built-in)", ":HRTMon", "524291"),
            ("AROS KS ROM (built-in) (1024k)", ":AROS", "256"),
        ]
        with open(self.globalSettingsFile, "a") as sf:
            sf.write("ROMs=" + str(len(roms)) + '\n')
            for t in roms:
                name, path, itype = t
                sf.write("ROMName=" + name + '\n')
                sf.write("ROMPath=" + path + '\n')
                sf.write("ROMType=" + itype + '\n')
