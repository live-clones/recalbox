from configgen.Emulator import Emulator
from configgen.utils.recallog import recallog


class CRTEmulatorSelector:

    def chooseEmulatorAndCoreFor(self, emulator: Emulator, rom: str) -> Emulator:
        selected_emulator = emulator
        if emulator.Name == "snes":
            selected_emulator = Emulator(emulator.Name, "libretro", "snes9x", emulator.VideoMode, emulator.Ratio)
        if emulator.Name == "nes":
            selected_emulator = Emulator(emulator.Name, "libretro", "nestopia", emulator.VideoMode, emulator.Ratio)
        if emulator.Name == "atari2600":
            selected_emulator = Emulator(emulator.Name, "libretro", "stella", emulator.VideoMode, emulator.Ratio)
        if emulator.Name == "sega32x":
            selected_emulator = Emulator(emulator.Name, "libretro", "picodrive", emulator.VideoMode, emulator.Ratio)
        if emulator.Name in ["mastersystem", "segacd", "sg1000", "megadrive", "gamegear"]:
            selected_emulator = Emulator(emulator.Name, "libretro", "genesisplusgx", emulator.VideoMode, emulator.Ratio)
        if emulator.Name in ["wswan", "wswanc"]:
            selected_emulator = Emulator(emulator.Name, "libretro", "mednafen_wswan", emulator.VideoMode,
                                         emulator.Ratio)
        if emulator.Name in ["amiga600", "amiga1200", "amigacd32", "amigacdtv"]:
            selected_emulator = Emulator(emulator.Name, "libretro", "uae4arm", emulator.VideoMode,
                                         emulator.Ratio)
        if emulator.Name in ["n64", "64dd"]:
            selected_emulator = Emulator(emulator.Name, "libretro", "parallel_n64", emulator.VideoMode,
                                         emulator.Ratio)
        if emulator.Name == "psx":
            selected_emulator = Emulator(emulator.Name, "libretro", "swanstation", emulator.VideoMode, emulator.Ratio)
        if emulator.Name == "dos":
            selected_emulator = Emulator(emulator.Name, "libretro", "dosbox_pure", emulator.VideoMode, emulator.Ratio)

        if emulator.Name == "240ptestsuite":
            extensionToSystem = {
                ".nes": "nes",
                ".sfc": "snes",
                ".cue": "segacd",
                ".bin": "megadrive",
                ".gb": "gb",
                ".gba": "gba",
                ".cdi": "dreamcast",
                ".pce": "pcengine"
            }
            for supportedSystem in extensionToSystem.items():
                if rom.endswith(supportedSystem[0]):
                    selected_emulator = Emulator(supportedSystem[1], emulator.Emulator, emulator.Core,
                                                 emulator.VideoMode,
                                                 emulator.Ratio)

        recallog("Selected emulator for system {} : {} {} {}".format(emulator.Name, selected_emulator.Name,
                                                                     selected_emulator.Emulator,
                                                                     selected_emulator.Core), log_type="CRT")

        return selected_emulator
