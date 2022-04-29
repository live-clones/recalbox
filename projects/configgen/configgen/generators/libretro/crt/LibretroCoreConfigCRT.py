import typing

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTScreenType, CRTResolutionType, CRTVideoStandard, CRTRegion
from configgen.utils.recallog import recallog

VideoStandardToRegionMap = {
    "genesisplusgx":
        {
            "prop_name": "genesis_plus_gx_region_detect",
            "values": {CRTVideoStandard.AUTO: "auto", CRTVideoStandard.NTSC: "ntsc-u", CRTVideoStandard.PAL: "pal"}
        },
    "snes9x":
        {
            "prop_name": "snes9x_region",
            "values": {CRTVideoStandard.AUTO: "auto", CRTVideoStandard.NTSC: "ntsc", CRTVideoStandard.PAL: "pal"}
        },
    "picodrive":
        {
            "prop_name": "nestopia_favored_system",
            "values": {CRTVideoStandard.AUTO: "Auto", CRTVideoStandard.NTSC: "US", CRTVideoStandard.PAL: "Europe"}
        },
    "nestopia":
        {
            "prop_name": "nestopia_favored_system",
            "values": {CRTVideoStandard.AUTO: "auto", CRTVideoStandard.NTSC: "ntsc", CRTVideoStandard.PAL: "pal"}
        },
    "stella":
        {
            "prop_name": "stella_console",
            "values": {CRTVideoStandard.AUTO: "auto", CRTVideoStandard.NTSC: "ntsc", CRTVideoStandard.PAL: "pal"}
        },
    "swanstation":
        {
            "prop_name": "duckstation_Console.Region",
            "values": {CRTVideoStandard.AUTO: "Auto", CRTVideoStandard.NTSC: "NTSC-U", CRTVideoStandard.PAL: "PAL"}
        },
    "o2em":
        {
            "prop_name": "duckstation_Console.Region",
            "values": {CRTVideoStandard.AUTO: "auto", CRTVideoStandard.NTSC: "NTSC", CRTVideoStandard.PAL: "PAL"}
        }
}

RegionToCoreRegionMap = {
    "genesisplusgx":
        {
            "prop_name": "genesis_plus_gx_region_detect",
            "values": {CRTRegion.AUTO: "auto", CRTRegion.JP: "ntsc-j", CRTRegion.US: "ntsc-u", CRTRegion.EU: "pal"}
        },
}


class LibretroCoreConfigCRT:

    def createConfigFor(self, system: Emulator) -> typing.Dict[str, str]:
        lines = dict()
        if system.Name in ["mastersystem", "segacd", "sg1000", "megadrive"]:
            lines["genesis_plus_gx_overscan"] = '"top/bottom"'
        if system.Name == "gamegear":
            lines["genesis_plus_gx_overscan"] = '"disabled"'
        if system.Name == "sega32x":
            lines["picodrive_overscan"] = '"disabled"'
        if system.Name in ["snes", "satellaview", "sufami"]:
            lines["snes9x_overscan"] = '"disabled"'
        if system.Name in ["nes", "fds"]:
            lines["nestopia_overscan_h"] = '"disabled"'
            lines["nestopia_overscan_v"] = '"disabled"'
        if system.Name == "atari2600":
            lines["stella_crop_hoverscan"] = '"disabled"'
            lines["stella_console"] = '"auto"'
        if system.Name in ["amiga600", "amiga1200", "amigacd32", "amigacdtv"]:
            lines["uae4arm_resolution"] = '"320x240"'
            lines["uae4arm_leds_on_screen"] = '"off"'
        if system.Name == "amiga600":
            lines["uae4arm_resolution"] = '"320x240"'
            lines["uae4arm_leds_on_screen"] = '"off"'
        if system.Name in ["n64", "64dd"]:
            lines["parallel-n64-screensize"] = '"320x240"'
        if system.Name == "psx":
            lines["duckstation_Display.AspectRatio"] = '"Auto"'
            lines["duckstation_Display.CropMode"] = '"None"'
            lines["duckstation_Console.Region"] = '"Auto"'
        if system.Name == "gba":
            lines["mgba_gb_model"] = '"Game Boy Advance"'
        if system.Name == "o2em":
            lines["o2em_crop_overscan"] = '"enabled"'
        if system.Name in ["msx", "msx1", "msx2", "msxturbor"]:
            lines["bluemsx_overscan"] = '"disabled"'
        if system.Name == "c64":
            lines["vice_zoom_mode"] = '"disabled"'
            lines["vice_zoom_mode_crop"] = '"4:3"'
            lines["vice_c64_model"] = '"C64 PAL auto"'
        if system.Name == "gb":
            lines["gambatte_gb_colorization"] = '"auto"'
        if system.Name in ["dreamcast", "naomi", "naomigd", "atomiswave"]:
            if system.CRTScreenType == CRTScreenType.k15:
                if system.CRTResolutionType == CRTResolutionType.Progressive:
                    rez = "320x240"
                else:
                    rez = "640x480"
            else:
                if system.CRTResolutionType == CRTResolutionType.DoubleFreq:
                    rez = "320x240"
                else:
                    rez = "640x480"
            lines["reicast_internal_resolution"] = '"{}"'.format(rez)
            lines["reicast_cable_type"] = '"TV (RGB)"'

        if system.Core in VideoStandardToRegionMap:
            if system.CRTVideoStandard in VideoStandardToRegionMap[system.Core]["values"]:
                lines[VideoStandardToRegionMap[system.Core]["prop_name"]] = \
                    VideoStandardToRegionMap[system.Core]["values"][system.CRTVideoStandard]
        if system.Name == "megadrive" and system.CRTRegion != CRTRegion.AUTO:
            if system.Core in RegionToCoreRegionMap:
                if system.CRTRegion in RegionToCoreRegionMap[system.Core]["values"]:
                    lines[RegionToCoreRegionMap[system.Core]["prop_name"]] = \
                        RegionToCoreRegionMap[system.Core]["values"][system.CRTRegion]
        if system.Name == "quake2":
            if system.CRTScreenType == CRTScreenType.k15:
                rez = "320x240"
            else:
                if system.CRTResolutionType == CRTResolutionType.DoubleFreq:
                    rez = "320x240"
                else:
                    rez = "640x480"
            lines["vitaquakeii_resolution"] = '"{}"'.format(rez)
        if system.Name == "atarist":
            lines["hatari_video_crop_overscan"] = '"true"'
            lines["hatari_video_hires"] = '"false"'


        log = "Forcing core configuration: "
        for config in lines.items():
            log = "{} {}={}".format(log, config[0], config[1]).replace('"', '')
        recallog(log, log_type="CRT")
        return lines
