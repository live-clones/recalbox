from typing import Optional, Dict
from unittest.mock import mock_open

import pytest

from configgen.Emulator import Emulator, ExtraArguments
from configgen.crt.CRTConfigParser import CRTConfigParser
from configgen.crt.CRTTypes import CRTSystemMode, CRTArcadeMode
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.generators.libretro.crt.LibretroConfigCRT import LibretroConfigCRT
from configgen.settings.keyValueSettings import keyValueSettings
from tests.Givens import givenThoseFiles

MODES_TXT = "/recalbox/share/system/configs/crt/modes.txt"
SYSTEMS_TXT = "/recalbox/share/system/configs/crt/systems.txt"
ARCADE_TXT = "/recalbox/share/system/configs/crt/arcade_games.txt"


def configureForCrt(emulator: Emulator, crtvideostandard="auto", crtresolutiontype="progressive", crtscreentype="15kHz",
                    crtadaptor="recalboxrgbdual", crtregion="auto", crtscanlines=False):
    emulator.configure(keyValueSettings(""),
                       ExtraArguments("", "", "", "", "", "", "", "", crtvideostandard, crtresolutiontype,
                                      crtscreentype,
                                      crtadaptor, crtregion,
                                      crtscanlines))
    return emulator


@pytest.fixture
def system_snes():
    return Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='snes9x')


@pytest.fixture
def system_fbneo():
    return Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='fbneo')


@pytest.fixture
def system_mastersystem():
    return Emulator(name='mastersystem', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='picodrive')


@pytest.fixture
def system_dreamcast():
    return Emulator(name='dreamcast', videoMode='1920x1080', ratio='auto', emulator='libretro', core='flycast')


def givenDefaultsModesAndSystems(mocker):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default:pal:288@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\ndefault:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,50\n"
    })


def test_given_any_system_should_avoid_fullscreen_dimensions(mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,0,0",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["video_fullscreen"] == '"true"'
    assert config_lines["video_fullscreen_x"] == ''
    assert config_lines["video_fullscreen_y"] == ''


def test_given_any_config_should_create_default_config_with_no_graphical_filters(mocker):
    givenDefaultsModesAndSystems(mocker)
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
        "/recalbox/share/roms/wswanc/arkbl2.zip")
    assert libretro_config["video_shader_enable"] == '"false"'


def test_given_snes_system_and_a_single_mode_should_create_mode_in_configuration(mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,0,0",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"60"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60"'


def test_given_snes_system_and_a_two_modes_pal_ntsc_should_create_two_mode_in_configuration(mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,pal,15kHz,progressive,snes:pal:240@50p,0,0\nsnes,snes9x,ntsc,15kHz,progressive,snes:ntsc:224@60p,0,0",
        MODES_TXT: "snes:ntsc:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60.1\nsnes:pal:240@50p,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50.1"})
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"50.1"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60.1"'


def test_missing_system_creates_default_config(mocker, system_snes):
    givenThoseFiles(mocker, {
        MODES_TXT: "default:pal:288@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\ndefault:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"50"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60"'


def test_missing_mode_throw(mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,0,0",
        MODES_TXT: "nes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    with pytest.raises(Exception):
        LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes), "Mario.smc"


def test_given_overscan_feature_creates_viewport_all_region_config(mocker, system_snes: Emulator):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,1820,200",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] == "23"
    assert libretro_config["custom_viewport_width_pal"] == 1820
    assert libretro_config["custom_viewport_width_ntsc"] == 1820
    assert libretro_config["custom_viewport_height_pal"] == 200
    assert libretro_config["custom_viewport_height_ntsc"] == 200
    assert libretro_config["custom_viewport_x_pal"] == 50
    assert libretro_config["custom_viewport_x_ntsc"] == 50
    assert libretro_config["custom_viewport_y_pal"] == 12
    assert libretro_config["custom_viewport_y_ntsc"] == 12


def test_given_overscan_feature_creates_viewport_ntsc_region_config(mocker, system_snes: Emulator):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,ntsc,15kHz,progressive,snes:224@60p,1830,200",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] == "23"
    assert libretro_config["custom_viewport_width_ntsc"] == 1830
    assert libretro_config["custom_viewport_height_ntsc"] == 200
    assert libretro_config["custom_viewport_x_ntsc"] == 45
    assert libretro_config["custom_viewport_y_ntsc"] == 12


def test_given_overscan_horizontal_creates_viewport_config(mocker, system_snes: Emulator):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,1840,0",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] == "23"
    assert libretro_config["custom_viewport_width_ntsc"] == 1840
    assert libretro_config["custom_viewport_width_pal"] == 1840
    assert libretro_config["custom_viewport_height_ntsc"] == 224
    assert libretro_config["custom_viewport_height_pal"] == 224
    assert libretro_config["custom_viewport_x_ntsc"] == 40
    assert libretro_config["custom_viewport_x_pal"] == 40
    assert libretro_config["custom_viewport_y_ntsc"] == 0
    assert libretro_config["custom_viewport_y_pal"] == 0


def test_given_no_viewport_config_returns_viewport_values_from_mode(mocker, system_snes: Emulator):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,all,15kHz,progressive,snes:224@60p,0,0",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})
    snes = configureForCrt(system_snes)
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        snes, "Mario.smc")

    assert libretro_config["aspect_ratio_index"] == "23"
    assert libretro_config["custom_viewport_width"] == ""
    assert libretro_config["custom_viewport_width_ntsc"] == 1840
    assert libretro_config["custom_viewport_width_pal"] == 1840
    assert libretro_config["custom_viewport_height_ntsc"] == 224
    assert libretro_config["custom_viewport_height_pal"] == 224
    assert libretro_config["custom_viewport_x"] == 40
    assert libretro_config["custom_viewport_x_ntsc"] == 40
    assert libretro_config["custom_viewport_x_pal"] == 40
    assert libretro_config["custom_viewport_y"] == 0
    assert libretro_config["custom_viewport_y_ntsc"] == 0
    assert libretro_config["custom_viewport_y_pal"] == 0


def test_given_fbneo_game_returns_game_mode(mocker, system_fbneo: Emulator):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,1920,0,0",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_neogeocd_game_returns_neogeo_mode(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "mslug5,fbneo,whatever,1920,0,0",
        SYSTEMS_TXT: "neogeocd,fbneo,all,15kHz,progressive,neogeocdmode,0,0",
        MODES_TXT: "neogeocdmode,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    neogeocd = Emulator(name='neogeocd', videoMode='1920x1080', ratio='auto', emulator='libretro',
                        core='fbneo')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        neogeocd, "/recalbox/share/roms/neogeocd/mslug5.iso")
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2003_game_returns_game_mode(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,1920,0,0\narkbl2,mame2003,arcade:240@60.000000,1920,0,0",
        MODES_TXT: "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\narcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"})

    system_fbneo = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2003')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2003plus_game_returns_game_mode(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,0\narkbl2,mame2003,arcade:240@60.000000,0,0,0",
        MODES_TXT: "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\narcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })

    system_fbneo = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2003_plus')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2010_game_returns_game_mode(mocker, system_fbneo: Emulator):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,0\narkbl2,mame2010,arcade:240@60.000000,0,0,0",
        MODES_TXT: "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\narcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })

    system_fbneo = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2010')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_any_system_returns_overscan_active(mocker, system_snes: Emulator):
    givenDefaultsModesAndSystems(mocker)
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert libretro_config["video_crop_overscan"] == '"false"'


def test_given_a_vertical_game_and_no_viewport_info_returns_core_1920_ratio(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "25"


def test_given_a_vertical_game_and_viewport_info_returns_custom_ratio(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,1840,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "23"


def test_given_a_vertical_game_and_system_wide_viewport_info_core_1920_ratio(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "25"


def test_given_a_vertical_game_returns_bilinear_filtering(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["video_smooth"] == '"true"'


def test_given_a_mode_with_refresh_rate_then_set_refresh_rate_option_in_retroarch(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,0\narkbl2,mame2010,arcade:240@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,59.899"
    })
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["video_refresh_rate_pal"] == '"59.899"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"59.899"'


def test_given_wonderswan_systems_should_create_config_with_vsync_deactivated(mocker):
    givenDefaultsModesAndSystems(mocker)

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), ).createConfigFor(
        Emulator(name='wswanc', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mednafen_wswan'),
        "/recalbox/share/roms/wswanc/arkbl2.zip")
    assert libretro_config["video_vsync"] == '"false"'


SEGA_CONFIGURATION = {
    SYSTEMS_TXT: "dreamcast,flycast,pal,15kHz,progressive,palmode,0,0\ndreamcast,flycast,ntsc,15kHz,progressive,ntscmode,0,0\ndreamcast,flycast,pal,15kHz,interlaced,palimode,0,0\ndreamcast,flycast,ntsc,15kHz,interlaced,ntscimode,0,0\ndreamcast,flycast,all,31kHz,progressive,480pmode,640,0\ndreamcast,flycast,all,31kHz,doublefreq,240on31,0,0",
    MODES_TXT: "palmode,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50.5\nntscmode,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60.6\npalimode,1920 1 80 184 312 576 1 18 6 21 0 0 0 50 1 39312000 1,50\nntscimode,1920 1 80 184 312 480 1 18 6 21 0 0 0 60 1 39312000 1,60\n480pmode,1920 1 80 184 312 480 1 18 6 21 0 0 0 60 0 99999999 1,60\n240on31,1920 1 80 184 312 240 1 18 6 21 0 0 0 60 0 66666666 1,60"
}


def test_given_sega_systems_when_15khz_and_progressive_selected_should_create_config_with_240p_mode(mocker,
                                                                                                    system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="progressive")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_sega_systems_when_15khz_and_interlaced_selected_should_create_config_with_480i_mode(mocker,
                                                                                                   system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="interlaced")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 576 1 18 6 21 0 0 0 50 1 39312000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 480 1 18 6 21 0 0 0 60 1 39312000 1"'


def test_given_sega_systems_when_31khz_selected_should_create_config_with_480p_mode(mocker, system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 480 1 18 6 21 0 0 0 60 0 99999999 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 480 1 18 6 21 0 0 0 60 0 99999999 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640


def test_given_sega_systems_when_31khz_and_doublefreq_selected_should_create_config_with_240p_mode(mocker,
                                                                                                   system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="31kHz", crtresolutiontype="doublefreq")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 240 1 18 6 21 0 0 0 60 0 66666666 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 240 1 18 6 21 0 0 0 60 0 66666666 1"'


def test_given_15kHz_and_force50hz_selected_should_create_config_with_pal_mode(mocker, system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="15kHz", crtresolutiontype="progressive",
                                crtvideostandard="pal")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50.5"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"50.5"'


def test_given_15kHz_and_force60hz_selected_should_create_config_with_ntsc_mode(mocker, system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="15kHz", crtresolutiontype="progressive",
                                crtvideostandard="ntsc")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60.6"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60.6"'


def test_given_sega_systems_when_15khz_and_interlaced_and_force_50hz_should_create_config_with_480i_pal_mode(mocker,
                                                                                                             system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtresolutiontype="interlaced", crtvideostandard="pal")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 576 1 18 6 21 0 0 0 50 1 39312000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 576 1 18 6 21 0 0 0 50 1 39312000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"50"'


def test_given_any_systems_when_31kHz_with_no_mode_found_should_default_to_31kHz_mode(mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="interlaced", crtvideostandard="pal", crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 480
    assert libretro_config["custom_viewport_height_pal"] == 480


def test_given_any_systems_when_31kHz_and_doublefreq_with_no_mode_found_should_default_to_31kHz_doublefreq_mode(mocker,
                                                                                                                system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="doublefreq", crtvideostandard="pal", crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["video_black_frame_insertion"] == '"1"'
    assert libretro_config["custom_viewport_width_ntsc"] == 1920
    assert libretro_config["custom_viewport_width_pal"] == 1920
    assert libretro_config["custom_viewport_height_ntsc"] == 240
    assert libretro_config["custom_viewport_height_pal"] == 240


def test_given_any_systems_when_15kHz_and_auto_region_with_no_mode_found_should_select_15kHz_default_mode(mocker,
                                                                                                          system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default:ntsc:240@60,1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1,60\ndefault:pal:288@50,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1,50"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="interlaced", crtvideostandard="auto", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'


def test_given_any_systems_when_15kHz_and_forced_region_with_no_mode_found_should_select_15kHz_default_mode(mocker,
                                                                                                            system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default:ntsc:240@60,1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1,60\ndefault:pal:288@50,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1,50"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="interlaced", crtvideostandard="pal", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"50"'


def test_given_a_arcade_game_and_31kHz_screen_and_doublefreq_then_return_default_doublefreq_mode(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 80 184 312 224 1 10 3 25 0 0 0 59 0 39000000 1,59.637405\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })

    mamecrt = configureForCrt(system_fbneo, crtresolutiontype="doublefreq", crtvideostandard="auto",
                              crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        mamecrt, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["video_black_frame_insertion"] == '"1"'


def test_given_a_nes_game_should_return_viewport_info(mocker):
    # BUG where the viewport height was empty
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "nes,nestopia,ntsc,15kHz,progressive,snes:nes:ntsc:240@60.0988,0,0\nnes,nestopia,pal,15kHz,progressive,snes:nes:pal:288@50.01,0,240",
        MODES_TXT: "snes:nes:ntsc:240@60.0988,1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 39001717 1,60.0988\nsnes:nes:pal:288@50.01,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39070212 1,50.01"})

    nes_crt = configureForCrt(Emulator("nes", "libretro", "nestopia", "", "auto"), crtresolutiontype="progressive",
                              crtvideostandard="auto", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        nes_crt, "/recalbox/share/roms/nes/nes.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39070212 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 39001717 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50.01"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60.0988"'

    assert libretro_config["custom_viewport_width_ntsc"] == 1840
    assert libretro_config["custom_viewport_height_ntsc"] == 240
    assert libretro_config["custom_viewport_x_ntsc"] == 40
    assert libretro_config["custom_viewport_y_ntsc"] == 0

    assert libretro_config["custom_viewport_width_pal"] == 1840
    assert libretro_config["custom_viewport_height_pal"] == 240
    assert libretro_config["custom_viewport_x_pal"] == 40
    assert libretro_config["custom_viewport_y_pal"] == 24


def test_given_15kHz_and_force50hz_selected_and_no_pal_config_should_create_config_with_default_pal_mode_but_best_viewport_size_selected_from_ntsc(
        mocker):
    givenThoseFiles(mocker, {
        MODES_TXT: "gamegear:all:224@59.92274,1920 1 80 184 312 224 1 10 3 24 0 0 0 59 0 39037029 1,59.92274\ndefault:pal:288@50,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1,50",
        SYSTEMS_TXT: "gamegear,picodrive,all,15kHz,progressive,standard:ntsc:224@60,960,144"
    })
    gamegear = configureForCrt(
        Emulator(name='gamegear', videoMode='1920x1080', ratio='auto', emulator='libretro', core='picodrive'),
        crtscreentype="15kHz", crtresolutiontype="progressive", crtvideostandard="pal")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(gamegear, "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["custom_viewport_width_pal"] == 960
    assert libretro_config["custom_viewport_height_pal"] == 144


def test_given_31kHz_should_create_config_with_default_mode_but_best_viewport_size_selected_from_ntsc(
        mocker):
    givenThoseFiles(mocker, {
        MODES_TXT: "1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60",
        SYSTEMS_TXT: "gamegear,picodrive,all,15kHz,progressive,standard:ntsc:224@60,960,144"
    })
    gamegear = configureForCrt(
        Emulator(name='gamegear', videoMode='1920x1080', ratio='auto', emulator='libretro', core='picodrive'),
        crtscreentype="31kHz", crtresolutiontype="progressive", crtvideostandard="ntsc")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(gamegear, "rom.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 320
    assert libretro_config["custom_viewport_height_ntsc"] == 288


def test_given_31kHz_and_double_freq_should_create_config_with_default_mode_but_best_viewport_size_selected_from_ntsc(
        mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,ntsc,15kHz,progressive,snes:nes:ntsc:240@60.0988,0,239",
        MODES_TXT: "default@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="doublefreq", crtvideostandard="pal",
                           crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["video_black_frame_insertion"] == '"1"'
    assert libretro_config["custom_viewport_width_ntsc"] == 1920
    assert libretro_config["custom_viewport_width_pal"] == 1920
    assert libretro_config["custom_viewport_height_ntsc"] == 239
    assert libretro_config["custom_viewport_height_pal"] == 239


def test_given_31kHz_should_create_config_with_default_mode_but_best_viewport_size_selected_from_mode(mocker,
                                                                                                      system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,ntsc,15kHz,progressive,snes:nes:ntsc:240@60.0988,0,0",
        MODES_TXT: "snes:nes:ntsc:240@60.0988,1920 1 80 184 312 239 1 1 3 16 0 0 0 60 0 39001717 1,60.0988\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="progressive", crtvideostandard="auto",
                           crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 478
    assert libretro_config["custom_viewport_height_pal"] == 478


def test_given_31kHz_and_double_freq_should_create_config_with_default_mode_but_best_viewport_size_selected_from_mode(
        mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,ntsc,15kHz,progressive,snes:nes:ntsc:240@60.0988,0,0",
        MODES_TXT: "snes:nes:ntsc:240@60.0988,1920 1 80 184 312 239 1 1 3 16 0 0 0 60 0 39001717 1,60.0988\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60"
    })
    snes = configureForCrt(system_snes, crtresolutiontype="doublefreq", crtvideostandard="auto",
                           crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/snes/Mario.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 1920
    assert libretro_config["custom_viewport_width_pal"] == 1920
    assert libretro_config["custom_viewport_height_ntsc"] == 239
    assert libretro_config["custom_viewport_height_pal"] == 239


def test_given_31kHz_and_should_create_neogeo_config_with_default_mode_but_best_viewport_size_selected_from_mode(mocker,
                                                                                                                 system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "neogeo,all,15kHz,progressive,neogeo:all:224@59.185606,0,0",
        ARCADE_TXT: "dino,fbneo,arcade:224@59.637405,0,0,0",
        MODES_TXT: "arcade:224@59.637405,1920 1 80 184 312 224 1 10 3 25 0 0 0 59 0 39000000 1,59.637405\nneogeo:all:224@59.185606,1920 1 80 184 312 224 1 12 3 26 0 0 0 59 0 39147727 1,59.185606\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    snes = configureForCrt(
        Emulator(name='neogeo', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fbneo'),
        crtresolutiontype="progressive", crtvideostandard="auto",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(snes,
                                                                                               "/recalbox/share/roms/neogeo/dino.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 448
    assert libretro_config["custom_viewport_height_pal"] == 448


def test_given_31kHz_and_arcade_game_should_create_arcade_config_with_default_mode_but_best_viewport_size_selected_from_mode(
        mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "dino,fbneo,arcade:224@59.637405,0,0,0",
        MODES_TXT: "arcade:224@59.637405,1920 1 80 184 312 224 1 10 3 25 0 0 0 59 0 39000000 1,59.637405\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    emulator = configureForCrt(
        Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fbneo'),
        crtresolutiontype="progressive", crtvideostandard="auto",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/fbneo/dino.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 448
    assert libretro_config["custom_viewport_height_pal"] == 448


#
# test_given_default_should_take_in_account_viewport_width
# Issue with metal gear solid pal
def test_given_psx_when_starting_a_game_then_use_alsamixer(
        mocker, system_snes):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "psx,swanstation,ntsc,15kHz,progressive,psx@60.0988,0,0",
        MODES_TXT: "psx@60.0988,1920 1 80 184 312 239 1 1 3 16 0 0 0 60 0 39001717 1,60.0988"
    })

    psx = configureForCrt(
        Emulator(name='psx', videoMode='1920x1080', ratio='auto', emulator='libretro', core='swanstation'),
        crtresolutiontype="progressive", crtvideostandard="auto",
        crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(psx,
                                                                                               "/recalbox/share/roms/psx/Mario.zip")

    assert libretro_config["audio_driver"] == '"alsathread"'


def test_given_31kHz_and_scanlines_on_should_create_scanlines_config(mocker, system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="31kHz", crtresolutiontype="progressive",
                                crtvideostandard="all", crtscanlines=True)

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["video_shader_enable"] == '"true"'
    assert libretro_config["video_shader_dir"] == '"/recalbox/share/shaders/"'
    assert libretro_config["video_shader"] == '/recalbox/share/shaders/rrgbd-scanlines.glslp'


def test_given_31kHz_and_scanlines_on_should_not_create_scanlines_config_when_double_freq(mocker, system_dreamcast):
    givenThoseFiles(mocker, SEGA_CONFIGURATION)
    dreamcast = configureForCrt(system_dreamcast, crtscreentype="31kHz", crtresolutiontype="doublefreq",
                                crtvideostandard="all", crtscanlines=True)

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(dreamcast,
                                                                                               "rom.zip")

    assert libretro_config["video_shader_enable"] == '"false"'


# Bug outrunner having 960px height
def test_given_weird_mode_game_is_started_in_480_it_does_not_double_the_height(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "paperboy,mame2010,arcade:384@60.096154,0,0,0",
        MODES_TXT: "arcade:384@60.096154,1920 1 80 184 312 384 1 2 6 33 0 0 0 60 1 39233551 1,60.34\narcade:224@59.637405,1920 1 80 184 312 224 1 10 3 25 0 0 0 59 0 39000000 1,59.637405\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    emulator = configureForCrt(
        Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2010'),
        crtresolutiontype="progressive", crtvideostandard="ntsc",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/fbneo/paperboy.zip")

    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 384
    assert libretro_config["custom_viewport_height_pal"] == 384

    emulator = configureForCrt(
        Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2010'),
        crtresolutiontype="doublefreq", crtvideostandard="ntsc",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/fbneo/paperboy.zip")

    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 1920
    assert libretro_config["custom_viewport_width_pal"] == 1920
    assert libretro_config["custom_viewport_height_ntsc"] == 240
    assert libretro_config["custom_viewport_height_pal"] == 240


# Bug toki having 240 px height on 31khz
def test_given_a_240p_game_then_let_height_to_be_480p_on_31khz(mocker, system_dreamcast):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "toki,mame2003,arcade:240@60.000000,0,0,0",
        MODES_TXT: "arcade:240@60.000000,1920 1 80 184 312 240 1 2 3 16 0 0 0 60 0 39087360 1,60.000000\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    emulator = configureForCrt(
        Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2003'),
        crtresolutiontype="progressive", crtvideostandard="ntsc",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/fbneo/toki.zip")
    assert libretro_config["custom_viewport_height_ntsc"] == 480


def test_given_a_vertical_game_in_31khz_returns_vertical_configuration_with_ratio_and_smooth(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,1",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    emulator = configureForCrt(
        Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fbneo'),
        crtresolutiontype="progressive", crtvideostandard="ntsc",
        crtscreentype="31kHz")
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        emulator, "/recalbox/share/roms/mame/arkbl2.zip")

    assert libretro_config["aspect_ratio_index"] == "22"
    assert libretro_config["video_smooth"] == '"true"'


def test_given_mame2015_game_returns_mame2010_game_mode(mocker, system_fbneo: Emulator):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,0,0,0\narkbl2,mame2010,arcade:240@60.000000,0,0,0",
        MODES_TXT: "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\narcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60"
    })

    system_fbneo = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2015')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_fbneo, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_15kHz_and_no_mode_found_should_create_default_config_for_arcade(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "anygame,fbneo,arcade:224@60.000000,1920,0,0",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60\ndefault:pal:288@50,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1,50\ndefault:ntsc:240@60,1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1,60"})

    emulator = configureForCrt(system_fbneo, crtresolutiontype="progressive", crtvideostandard="auto",
                               crtscreentype="15kHz")
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        emulator, "arkbl2.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'


def test_given_15kHz_and_force50hz_selected_should_create_config_with_pal_mode_for_arcade(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "arkbl2,fbneo,arcade:224@60.000000,1920,0,0",
        MODES_TXT: "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60\ndefault:pal:288@50,1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1,50\ndefault:ntsc:240@60,1920 1 80 184 312 240 1 1 3 16 0 0 0 60 0 38937600 1,60"})

    emulator = configureForCrt(system_fbneo, crtresolutiontype="progressive", crtvideostandard="pal",
                               crtscreentype="15kHz")
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        emulator, "arkbl2.zip")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 80 184 312 288 1 4 3 18 0 0 0 50 0 39062400 1"'
    assert libretro_config["video_refresh_rate_pal"] == '"50"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"50"'


def test_given_31kHz_and_system_width_should_ignore_system_width(mocker):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "psx,ntsc,15kHz,progressive,psx@60.0988,2560,0",
        MODES_TXT: "default@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\npsx2560:ntsc:240@59.826,2560 1 104 248 416 240 1 2 3 17 0 0 0 59 0 52164443 1,59.826"})

    emulator = configureForCrt(
        Emulator(name='playstation', videoMode='1920x1080', ratio='auto', emulator='libretro', core='swansation'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtscreentype="31kHz")
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        emulator, "Die Hard Trilogy.iso")

    assert libretro_config["crt_switch_timings_pal"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 480
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_pal"] == 480


def test_given_31kHz_and_doublefreq_system_width_should_ignore_system_width(mocker):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "psx,ntsc,15kHz,progressive,psx2560:ntsc:240@59.826,2560,0",
        MODES_TXT: "default@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\npsx2560:ntsc:240@59.826,2560 1 104 248 416 240 1 2 3 17 0 0 0 59 0 52164443 1,59.826"})

    emulator = configureForCrt(
        Emulator(name='playstation', videoMode='1920x1080', ratio='auto', emulator='libretro', core='swansation'),
        crtresolutiontype="doublefreq", crtvideostandard="auto", crtscreentype="31kHz")
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        emulator, "Die Hard Trilogy.iso")

    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1"'
    assert libretro_config["custom_viewport_width_ntsc"] == 1920
    assert libretro_config["custom_viewport_height_ntsc"] == 240
    assert libretro_config["custom_viewport_width_pal"] == 1920
    assert libretro_config["custom_viewport_height_pal"] == 240


# Rtype in 31khz 480p should be 512px height
def test_given_rtype_with_256px_height_in_480_the_height_is_doubled(mocker):
    givenThoseFiles(mocker, {
        ARCADE_TXT: "rtype,fbneo,arcade:256@55.017606,0,0,0",
        MODES_TXT: "arcade:256@55.017606,1920 1 80 184 312 256 1 5 3 20 0 0 0 55 0 39000000 1,55.017606\n1920@31KHz-double:all:240@120,1920 1 8 32 40 240 1 4 3 15 0 0 0 60 0 6288000 1,60\ndefault@31kHz:all:480@60,640 1 24 96 48 480 1 11 2 32 0 0 0 60 0 25452000 1,60"
    })
    emulator = configureForCrt(
        Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro', core='fbneo'),
        crtresolutiontype="progressive", crtvideostandard="ntsc",
        crtscreentype="31kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/fbneo/rtype.zip")

    assert libretro_config["video_refresh_rate_pal"] == '"60"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"60"'
    assert libretro_config["custom_viewport_width_ntsc"] == 640
    assert libretro_config["custom_viewport_width_pal"] == 640
    assert libretro_config["custom_viewport_height_ntsc"] == 512
    assert libretro_config["custom_viewport_height_pal"] == 512
    assert libretro_config["custom_viewport_y_ntsc"] == -16
    assert libretro_config["custom_viewport_y_pal"] == -16
    assert libretro_config["custom_viewport_y"] == -16


def test_given_core_in_systems_then_use_system(mocker):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "snes,snes9x,pal,15kHz,progressive,snes:224@60p,0,0\nsnes,snes9x,ntsc,15kHz,progressive,snes:240@60p,0,0",
        MODES_TXT: "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60\nsnes:240@60p,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730001 1,60"})
    emulator = configureForCrt(
        Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
        crtresolutiontype="progressive", crtvideostandard="ntsc", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/snes/SMK.zip")

    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730001 1"'
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730001 1"'

    emulator = configureForCrt(
        Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
        crtresolutiontype="progressive", crtvideostandard="pal", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/snes/SMK.zip")

    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'

    emulator = configureForCrt(
        Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
        crtresolutiontype="progressive", crtvideostandard="auto", crtscreentype="15kHz")

    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator,
                                                                                               "/recalbox/share/roms/snes/SMK.zip")

    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730001 1"'
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
