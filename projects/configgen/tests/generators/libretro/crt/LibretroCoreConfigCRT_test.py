import pytest

from configgen.Emulator import Emulator
from configgen.generators.libretro.crt.LibretroCoreConfigCRT import LibretroCoreConfigCRT, VideoStandardToRegionMap
from tests.generators.libretro.crt.LibretroConfigCRT_test import configureForCrt


def test_given_snes_stella_sufami_system_should_create_core_config():
    core_configurator = LibretroCoreConfigCRT()
    sneses = [Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
              Emulator(name='satellaview', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
              Emulator(name='sufami', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x')]
    for snes in sneses:
        assert core_configurator.createConfigFor(snes)["snes9x_overscan"] == '"disabled"'
        assert core_configurator.createConfigFor(snes)["bsnes_ppu_show_overscan"] == '"ON"'

def test_given_nes_system_should_create_core_config():
    core_configurator = LibretroCoreConfigCRT()
    neses = [Emulator(name='nes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='nestopia'),
             Emulator(name='fds', videoMode='1920x1080', ratio='auto', emulator='libretro', core='nestopia')]
    for nes in neses:
        assert core_configurator.createConfigFor(nes)["nestopia_overscan_h"] == '"disabled"'
        assert core_configurator.createConfigFor(nes)["nestopia_overscan_v"] == '"disabled"'
        assert core_configurator.createConfigFor(nes)["fceumm_overscan_h"] == '"disabled"'
        assert core_configurator.createConfigFor(nes)["fceumm_overscan_v"] == '"disabled"'


def segaEmulatorWithGenesis(name):
    return Emulator(name=name, videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx')


@pytest.fixture
def system_dreamcast():
    return Emulator(name='dreamcast', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='flycast')


def test_given_sega_systems_should_create_core_config_with_overscan():
    core_configurator = LibretroCoreConfigCRT()
    sega_system = segaEmulatorWithGenesis('megadrive')
    assert core_configurator.createConfigFor(sega_system)["genesis_plus_gx_overscan"] == '"top/bottom"'
    sega_system = segaEmulatorWithGenesis('sg1000')
    assert core_configurator.createConfigFor(sega_system)["genesis_plus_gx_overscan"] == '"top/bottom"'
    sega_system = segaEmulatorWithGenesis('mastersystem')
    assert core_configurator.createConfigFor(sega_system)["genesis_plus_gx_overscan"] == '"top/bottom"'
    sega_system = segaEmulatorWithGenesis('segacd')
    assert core_configurator.createConfigFor(sega_system)["genesis_plus_gx_overscan"] == '"top/bottom"'
    sega_system = segaEmulatorWithGenesis('gamegear')
    assert core_configurator.createConfigFor(sega_system)["genesis_plus_gx_overscan"] == '"disabled"'


def test_given_sega32x_system_should_create_core_config_with_overscan():
    core_configurator = LibretroCoreConfigCRT()
    sega_system = Emulator(name='sega32x', videoMode='1920x1080', ratio='auto', emulator='libretro', core='picodrive')
    assert core_configurator.createConfigFor(sega_system)["picodrive_overscan"] == '"disabled"'


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
    ]

    for emulator in emulators:
        assert core_configurator.createConfigFor(emulator) == {"uae4arm_resolution": '"320x256"',
                                                               "uae4arm_leds_on_screen": '"off"',
                                                               "puae_zoom_mode": '"disabled"',
                                                               }
def test_given_amiga_cd32_systems_should_create_core_config_for_amiga():
    core_configurator = LibretroCoreConfigCRT()
    emulators = [
        Emulator(name='amigacd32', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm'),
        Emulator(name='amigacdtv', videoMode='1920x1080', ratio='auto', emulator='libretro', core='uae4arm')
    ]

    for emulator in emulators:
        assert core_configurator.createConfigFor(emulator) == {"uae4arm_resolution": '"320x240"',
                                                               "uae4arm_leds_on_screen": '"off"',
                                                               }
def test_given_n64_systems_should_create_core_config_for_parallel_resolution():
    core_configurator = LibretroCoreConfigCRT()
    emulator = Emulator(name='n64', videoMode='1920x1080', ratio='auto', emulator='libretro', core='parallel_n64')
    assert core_configurator.createConfigFor(emulator) == {"parallel-n64-screensize": '"320x240"'}
    emulator = Emulator(name='64dd', videoMode='1920x1080', ratio='auto', emulator='libretro', core='parallel_n64')
    assert core_configurator.createConfigFor(emulator) == {"parallel-n64-screensize": '"320x240"'}


def test_given_psx_systems_should_create_core_config():
    psx = Emulator(name='psx', videoMode='1920x1080', ratio='auto', emulator='libretro', core='swanstation')
    assert LibretroCoreConfigCRT().createConfigFor(psx)["duckstation_Display.AspectRatio"] == '"Auto"'
    assert LibretroCoreConfigCRT().createConfigFor(psx)["duckstation_Display.CropMode"] == '"None"'


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
    assert LibretroCoreConfigCRT().createConfigFor(gb) == {"gambatte_gb_colorization": '"internal"'}


def test_given_sega480i_systems_should_create_config_with_resolution_and_VGA():
    core_configurator = LibretroCoreConfigCRT()
    emulators = [
        Emulator(name='naomi', videoMode='1920x1080', ratio='auto', emulator='libretro', core='flycast'),
        Emulator(name='naomigd', videoMode='1920x1080', ratio='auto', emulator='libretro', core='flycast'),
        Emulator(name='dreamcast', videoMode='1920x1080', ratio='auto', emulator='libretro', core='flycast'),
        Emulator(name='atomiswave', videoMode='1920x1080', ratio='auto', emulator='libretro', core='flycast'),
    ]

    for emulator in emulators:
        assert core_configurator.createConfigFor(emulator) == {"reicast_internal_resolution": '"320x240"',
                                                               "reicast_cable_type": '"TV (RGB)"'}


def test_given_dreamcast_should_select_core_resolution_480_from_mode_on15khz_interlaced(mocker, system_dreamcast):
    core_configurator = LibretroCoreConfigCRT()

    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="interlaced", crtvideostandard="auto",
                                crtscreentype="15kHz")
    assert core_configurator.createConfigFor(dreamcast) == {"reicast_internal_resolution": '"640x480"',
                                                            "reicast_cable_type": '"TV (RGB)"'}


def test_given_dreamcast_should_select_core_resolution_240_from_mode_on15khz(mocker, system_dreamcast):
    core_configurator = LibretroCoreConfigCRT()

    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="progressive", crtvideostandard="auto",
                                crtscreentype="15kHz")
    assert core_configurator.createConfigFor(dreamcast) == {"reicast_internal_resolution": '"320x240"',
                                                            "reicast_cable_type": '"TV (RGB)"'}


def test_given_dreamcast_should_select_core_resolution_from_mode_on31khz(mocker, system_dreamcast):
    core_configurator = LibretroCoreConfigCRT()

    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="progressive", crtvideostandard="auto",
                                crtscreentype="31kHz")
    assert core_configurator.createConfigFor(dreamcast) == {"reicast_internal_resolution": '"640x480"',
                                                            "reicast_cable_type": '"TV (RGB)"'}


def test_given_dreamcast_should_select_core_resolution_from_mode_on31khz_doublefreq(mocker, system_dreamcast):
    core_configurator = LibretroCoreConfigCRT()

    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="doublefreq", crtvideostandard="auto",
                                crtscreentype="31kHz")
    assert core_configurator.createConfigFor(dreamcast) == {"reicast_internal_resolution": '"320x240"',
                                                            "reicast_cable_type": '"TV (RGB)"'}


def test_given_multiregion_emulator_should_add_the_core_region_when_forced(mocker):
    core_configurator = LibretroCoreConfigCRT()

    for region in ["auto", "ntsc", "pal"]:
        for core in VideoStandardToRegionMap.keys():
            emulator = configureForCrt(
                Emulator("whatever", videoMode='1920x1080', ratio='auto', emulator='libretro', core=core),
                crtvideostandard=region)
            assert core_configurator.createConfigFor(emulator)[VideoStandardToRegionMap[core]["prop_name"]] == \
                   VideoStandardToRegionMap[core]["values"][region]


def test_given_megadrive_and_jp_region_should_return_the_jp_region():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="ntsc", crtregion="jp", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'ntsc-j'

def test_given_megadrive_and_us_region_should_return_the_us_region():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="ntsc", crtregion="us", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'ntsc-u'

def test_given_megadrive_and_eu_region_should_return_the_eu_region():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="pal", crtregion="eu", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'pal'

def test_given_megadrive_and_eu_region_should_return_the_eu_region_even_if_mode_is_ntsc():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="ntsc", crtregion="eu", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'pal'

def test_given_megadrive_auto_standard_and_auto_region_should_return_the_auto_region():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtregion="auto", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'auto'

def test_given_megadrive_force_ntsc_and_auto_region_should_return_the_auto_region():
    megadrive = configureForCrt(
        Emulator(name='megadrive', videoMode='1920x1080', ratio='auto', emulator='libretro', core='genesisplusgx'),
        crtresolutiontype="progressive", crtvideostandard="ntsc", crtregion="auto", crtscreentype="15kHz")
    assert LibretroCoreConfigCRT().createConfigFor(megadrive)["genesis_plus_gx_region_detect"] == 'ntsc-u'

def test_given_quake2_should_set_resolution_the_resolution_to_240p_in15khz():
    quake2 = configureForCrt(
        Emulator(name='quake2', videoMode='1920x1080', ratio='auto', emulator='libretro', core='vitaquake2'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtregion="auto", crtscreentype="15kHz")

    assert LibretroCoreConfigCRT().createConfigFor(quake2)["vitaquakeii_resolution"] == '"320x240"'

def test_given_quake2_should_set_resolution_the_resolution_to_480p_in31khz():
    quake2 = configureForCrt(
        Emulator(name='quake2', videoMode='1920x1080', ratio='auto', emulator='libretro', core='vitaquake2'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtregion="auto", crtscreentype="31kHz")

    assert LibretroCoreConfigCRT().createConfigFor(quake2)["vitaquakeii_resolution"] == '"640x480"'

def test_given_quake2_should_set_resolution_the_resolution_to_240p_in31_double_freq():
    quake2 = configureForCrt(
        Emulator(name='quake2', videoMode='1920x1080', ratio='auto', emulator='libretro', core='vitaquake2'),
        crtresolutiontype="doublefreq", crtvideostandard="auto", crtregion="auto", crtscreentype="31kHz")

    assert LibretroCoreConfigCRT().createConfigFor(quake2)["vitaquakeii_resolution"] == '"320x240"'

def test_given_atarist_should_set_the_overscan_options():
    atarist = configureForCrt(
        Emulator(name='atarist', videoMode='1920x1080', ratio='auto', emulator='libretro', core='hatari'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtregion="auto", crtscreentype="15kHz")
    config = LibretroCoreConfigCRT().createConfigFor(atarist)
    assert config["hatari_video_crop_overscan"] == '"true"'
    assert config["hatari_video_hires"] == '"false"'
