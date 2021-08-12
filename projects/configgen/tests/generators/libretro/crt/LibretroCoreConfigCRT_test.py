import pytest

from configgen.Emulator import Emulator
from configgen.generators.libretro.crt.LibretroCoreConfigCRT import LibretroCoreConfigCRT


def test_given_snes_stella_sufami_system_should_create_core_config():
    core_configurator = LibretroCoreConfigCRT()
    sneses = [Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
              Emulator(name='satellaview', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
              Emulator(name='sufami', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x')]
    for snes in sneses:
        assert core_configurator.createConfigFor(snes) == {"snes9x_overscan": '"disabled"'}


def test_given_nes_system_should_create_core_config():
    core_configurator = LibretroCoreConfigCRT()
    neses = [Emulator(name='nes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='nestopia'),
             Emulator(name='fds', videoMode='1920x1080', ratio='auto', emulator='libretro', core='nestopia')]
    for nes in neses:
        assert core_configurator.createConfigFor(nes) == {"nestopia_overscan_h": '"disabled"',
                                                          "nestopia_overscan_v": '"disabled"'}


def segaEmulator(name):
    return Emulator(name=name, videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx')


def test_given_sega_systems_should_create_core_config_with_overscan():
    core_configurator = LibretroCoreConfigCRT()
    sega_system = segaEmulator('megadrive')
    assert core_configurator.createConfigFor(sega_system) == {"genesis_plus_gx_overscan": '"full"'}
    sega_system = segaEmulator('sg1000')
    assert core_configurator.createConfigFor(sega_system) == {"genesis_plus_gx_overscan": '"full"'}
    sega_system = segaEmulator('mastersystem')
    assert core_configurator.createConfigFor(sega_system) == {"genesis_plus_gx_overscan": '"full"'}
    sega_system = segaEmulator('segacd')
    assert core_configurator.createConfigFor(sega_system) == {"genesis_plus_gx_overscan": '"full"'}
    sega_system = segaEmulator('gamegear')
    assert core_configurator.createConfigFor(sega_system) == {"genesis_plus_gx_overscan": '"disabled"'}


def test_given_sega32x_system_should_create_core_config_with_overscan():
    core_configurator = LibretroCoreConfigCRT()
    sega_system = Emulator(name='sega32x', videoMode='1920x1080', ratio='auto', emulator='libretro', core='picodrive')
    assert core_configurator.createConfigFor(sega_system) == {"picodrive_overscan": '"disabled"'}


def test_given_atari2600_systems_should_create_core_config_with_overscan():
    core_configurator = LibretroCoreConfigCRT()
    atari2600 = Emulator(name='atari2600', videoMode='1920x1080', ratio='auto', emulator='libretro', core='whatever')
    assert core_configurator.createConfigFor(atari2600) == {"stella_console": '"auto"',
                                                            "stella_crop_hoverscan": '"disabled"'}


def test_given_amiga_systems_should_create_core_config_for_amiga():
    core_configurator = LibretroCoreConfigCRT()
    emulators = [
        Emulator(name='amiga600', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm'),
        Emulator(name='amiga1200', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm'),
        Emulator(name='amigacd32', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm'),
        Emulator(name='amigacdtv', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm')
    ]

    for emulator in emulators:
        assert core_configurator.createConfigFor(emulator) == {"uae4arm_resolution": '"320x240"',
                                                               "uae4arm_leds_on_screen": '"off"'}


def test_given_n64_systems_should_create_core_config_for_parallel_resolution():
    core_configurator = LibretroCoreConfigCRT()
    emulator = Emulator(name='n64', videoMode='1920x1080', ratio='auto', emulator='libretro', core='parallel_n64')
    assert core_configurator.createConfigFor(emulator) == {"parallel-n64-screensize": '"320x240"'}
    emulator = Emulator(name='64dd', videoMode='1920x1080', ratio='auto', emulator='libretro', core='parallel_n64')
    assert core_configurator.createConfigFor(emulator) == {"parallel-n64-screensize": '"320x240"'}


def test_given_psx_systems_should_create_core_config():
    psx = Emulator(name='psx', videoMode='1920x1080', ratio='auto', emulator='libretro', core='swanstation')
    assert LibretroCoreConfigCRT().createConfigFor(psx) == {
        "duckstation_Display.AspectRatio": '"Auto"',
        "duckstation_Display.CropMode": '"None"',
        "duckstation_Console.Region": '"Auto"'
    }


def test_given_gba_systems_should_create_core_config_with_gba_model():
    gba = Emulator(name='gba', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mgba')
    assert LibretroCoreConfigCRT().createConfigFor(gba) == {"mgba_gb_model": '"Game Boy Advance"'}


def test_given_o2em_systems_should_create_core_config_with_crop_overscan():
    o2em = Emulator(name='o2em', videoMode='1920x1080', ratio='auto', emulator='libretro', core='whatever')
    assert LibretroCoreConfigCRT().createConfigFor(o2em) == {"o2em_crop_overscan": '"enabled"'}


def test_given_msx_systems_should_create_core_config_with_overscan_disabled():
    core_configurator = LibretroCoreConfigCRT()
    emulators = [
        Emulator(name='msx', videoMode='1920x1080', ratio='auto', emulator='libretro', core='bluemsx'),
        Emulator(name='msx1', videoMode='1920x1080', ratio='auto', emulator='libretro', core='bluemsx'),
        Emulator(name='msx2', videoMode='1920x1080', ratio='auto', emulator='libretro', core='bluemsx'),
        Emulator(name='msxturbor', videoMode='1920x1080', ratio='auto', emulator='libretro', core='bluemsx')
    ]

    for emulator in emulators:
        assert core_configurator.createConfigFor(emulator) == {"bluemsx_overscan": '"disabled"'}


def test_given_c64_system_should_create_core_config_with_c64config():
    c64 = Emulator(name='c64', videoMode='1920x1080', ratio='auto', emulator='libretro', core='whatever')
    assert LibretroCoreConfigCRT().createConfigFor(c64) == {"vice_zoom_mode": '"disabled"',
                                                             "vice_zoom_mode_crop": '"4:3"',
                                                             "vice_c64_model": '"C64 PAL auto"'}


def test_given_gameboy_system_should_create_core_config_with_color():
    gb = Emulator(name='gb', videoMode='1920x1080', ratio='auto', emulator='libretro', core='gambatte')
    assert LibretroCoreConfigCRT().createConfigFor(gb) == {"gambatte_gb_colorization": '"auto"'}
