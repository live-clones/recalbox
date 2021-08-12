from configgen.Emulator import Emulator
from configgen.crt.CRTEmulatorSelector import CRTEmulatorSelector


def test_given_a_snes_game_then_return_libretro_snes9x():
    anySnes = Emulator("snes", "pisnes", "")
    emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(anySnes, "anyrom")
    assert emulator.Emulator == "libretro"
    assert emulator.Core == "snes9x"


def test_given_a_sega_game_then_return_libretro_genesisplusgx():
    for emulator in [Emulator("mastersystem", "", ""), Emulator("segacd", "", ""), Emulator("sg1000", "", ""),
                     Emulator("megadrive", "", ""), Emulator("gamegear", "", "")]:
        selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(emulator, "anyrom")
        assert selected_emulator.Emulator == "libretro"
        assert selected_emulator.Core == "genesisplusgx"


def test_given_a_sega32x_game_then_return_libretro_picodrive():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("sega32x", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "picodrive"


def test_given_a_nes_game_then_return_libretro_nestopia():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("nes", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "nestopia"


def test_given_a_atari2600_game_then_return_libretro_stella():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("atari2600", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "stella"


def test_given_a_wswan_game_then_return_libretro_mednafen():
    for emulator in [Emulator("wswan", "", ""), Emulator("wswanc", "", "")]:
        selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(emulator, "anyrom")
        assert selected_emulator.Emulator == "libretro"
        assert selected_emulator.Core == "mednafen_wswan"


def test_given_a_amiga_game_then_return_libretro_uae4arm():
    for emulator in [Emulator("amiga600", "", ""), Emulator("amiga1200", "", ""), Emulator("amigacd32", "", ""),
                     Emulator("amigacdtv", "", "")]:
        selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(emulator, "anyrom")
        assert selected_emulator.Emulator == "libretro"
        assert selected_emulator.Core == "uae4arm"


def test_given_a_n64_game_then_return_libretro_parallel_n64():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("n64", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "parallel_n64"
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("64dd", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "parallel_n64"


def test_given_a_psx_game_then_return_libretro_swanstation():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("psx", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "swanstation"


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
    }
    for system in gamesIn240PSuite.items():
        exprected_system = system[0]
        emulator = system[1][0]
        rom = system[1][1]
        selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(emulator, rom)
        assert selected_emulator.Name == exprected_system


def test_given_a_dos_game_then_return_libretro_dosboxpure():
    selected_emulator = CRTEmulatorSelector().chooseEmulatorAndCoreFor(Emulator("dos", "", ""), "anyrom")
    assert selected_emulator.Emulator == "libretro"
    assert selected_emulator.Core == "dosbox_pure"
