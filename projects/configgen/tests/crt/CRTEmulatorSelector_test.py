from configgen.Emulator import Emulator
from configgen.crt.CRTEmulatorSelector import CRTEmulatorSelector

def test_given_a_240ptestsuite_game_then_change_to_the_corresponfing_system():
    gamesIn240PSuite = {
        "nes": (Emulator("240ptestsuite", "libretro", "nestopia"), "rom.nes"),
        "snes": (Emulator("240ptestsuite", "libretro", "snes9x"), "rom.sfc"),
        "segacd": (Emulator("240ptestsuite", "libretro", "genesisplusgx"), "rom.cue"),
        "pcengine": (Emulator("240ptestsuite", "libretro", "mednafen_supergrafx"), "rom.pce"),
        "gb": (Emulator("240ptestsuite", "libretro", "gambatte"), "rom.gb"),
        "gba": (Emulator("240ptestsuite", "libretro", "mgba"), "rom.gba"),
        "dreamcast": (Emulator("240ptestsuite", "libretro", "flycast"), "rom.cdi"),
        "megadrive": (Emulator("240ptestsuite", "libretro", "genesisplusgx"), "rom.bin"),
        "playstation": (Emulator("240ptestsuite", "libretro", "swanstation"), "rom.cue"),
    }
    for system in gamesIn240PSuite.items():
        exprected_system = system[0]
        emulator = system[1][0]
        rom = system[1][1]
        selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(emulator, rom)
        assert selected_emulator.Name == exprected_system
