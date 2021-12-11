import typing

from configgen.Emulator import Emulator
from configgen.utils.recallog import recallog


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

        log = "Forcing core configuration: "
        for config in lines.items():
            log = "{} {}={}".format(log, config[0], config[1])
        recallog(log, log_type="CRT")
        return lines
