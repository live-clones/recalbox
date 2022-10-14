from configgen.Emulator import Emulator
from configgen.utils.recallog import recallog


class CRTEmulatorSelector:

    def chooseEmulatorAndCoreFor(self, emulator: Emulator, rom: str) -> Emulator:
        selected_emulator = emulator

        if emulator.Name == "240ptestsuite":
            extensionToSystem = {
                ".nes": "nes",
                ".sfc": "snes",
                ".bin": "megadrive",
                ".gb": "gb",
                ".gba": "gba",
                ".cdi": "dreamcast",
                ".pce": "pcengine",
            }
            for supportedSystem in extensionToSystem.items():
                if rom.endswith(supportedSystem[0]):
                    selected_emulator = Emulator(supportedSystem[1], emulator.Emulator, emulator.Core,
                                                 emulator.VideoMode,
                                                 emulator.Ratio)
            if rom.endswith(".cue"):
                system = "playstation" if emulator.Core == "swanstation" else "segacd"
                selected_emulator = Emulator(system, emulator.Emulator, emulator.Core, emulator.VideoMode,
                                             emulator.Ratio)

        recallog("Selected emulator for system {} : {} {} {}".format(emulator.Name, selected_emulator.Name,
                                                                     selected_emulator.Emulator,
                                                                     selected_emulator.Core), log_type="CRT")

        return selected_emulator
