from unittest.mock import mock_open

import pytest

from configgen.Emulator import Emulator
from configgen.crt.CRTConfigParser import CRTConfigParser, CRTConfiguration
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.generators.libretro.crt.LibretroConfigCRT import LibretroConfigCRT


@pytest.fixture
def system_snes():
    return Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='snes9x2002')


@pytest.fixture
def system_mame():
    return Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='fbneo')


@pytest.fixture
def system_mastersystem():
    return Emulator(name='mastersystem', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='picodrive')


def givenThisModesFile(mocker, content):
    mocker.patch('builtins.open', mock_open(read_data=content))
    mocker.patch('pathlib.Path.exists', return_value=True)


def givenDefaultsModes(mocker):
    mocker.patch('builtins.open',
                 mock_open(read_data="default:pal:240@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\n"
                                     "default:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,50\n"))
    mocker.patch('pathlib.Path.exists', return_value=True)


def givenDefaultsSystems(mocker):
    mocker.patch('configgen.crt.CRTConfigParser.CRTConfigParser.loadSystem',
                 return_value={})
    mocker.patch('pathlib.Path.exists', return_value=True)


def givenThisSystemsContent(mocker, return_value: CRTConfiguration):
    mocker.patch('configgen.crt.CRTConfigParser.CRTConfigParser.loadSystem',
                 return_value=return_value)
    mocker.patch('pathlib.Path.exists', return_value=True)


def givenThisGameConfig(mocker, return_value: CRTConfiguration):
    mocker.patch('configgen.crt.CRTConfigParser.CRTConfigParser.loadArcadeGame',
                 return_value=return_value)
    mocker.patch('pathlib.Path.exists', return_value=True)


def test_given_any_config_should_create_default_config_with_no_graphical_filters(mocker):
    givenDefaultsModes(mocker)
    givenThisSystemsContent(mocker, {})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 0).createConfigFor(
        Emulator(name='snes', videoMode='1920x1080', ratio='auto', emulator='libretro', core='snes9x'),
        "/recalbox/share/roms/wswanc/arkbl2.zip")
    assert libretro_config["video_smooth"] == '"false"'
    assert libretro_config["video_shader_enable"] == '"false"'


def test_given_snes_system_and_a_single_mode_should_create_mode_in_configuration(mocker, system_snes):
    givenThisModesFile(mocker, "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("snes:224@60p", 0, 0, 0)})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"60"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60"'


def test_given_snes_system_and_a_two_modes_pal_ntsc_should_create_two_mode_in_configuration(mocker, system_snes):
    givenThisModesFile(mocker,
                       "snes:ntsc:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60.1\n"
                       "snes:pal:240@50p,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50.1")
    givenThisSystemsContent(mocker, {"pal": ("snes:pal:240@50p", 0, 0), "ntsc": ("snes:ntsc:224@60p", 0, 0, 0)})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"50.1"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60.1"'


def test_missing_system_creates_default_config(mocker, system_snes):
    givenThisModesFile(mocker, "default:pal:240@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\n"
                               "default:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert config_lines["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1"'
    assert config_lines["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert config_lines["video_refresh_rate_pal"] == '"50"'
    assert config_lines["video_refresh_rate_ntsc"] == '"60"'


def test_missing_mode_throw(mocker, system_snes):
    givenThisModesFile(mocker, "nes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("snes:224@60p", 0, 0, 0)})
    with pytest.raises(Exception):
        LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes), "Mario.smc"


def test_given_overscan_feature_creates_viewport_all_region_config(mocker, system_snes: Emulator):
    givenThisModesFile(mocker, "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("snes:224@60p", 1820, 200, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] is "23"
    assert libretro_config["custom_viewport_width_pal"] is 1820
    assert libretro_config["custom_viewport_width_ntsc"] is 1820
    assert libretro_config["custom_viewport_height_pal"] is 200
    assert libretro_config["custom_viewport_height_ntsc"] is 200
    assert libretro_config["custom_viewport_x_pal"] is 50
    assert libretro_config["custom_viewport_x_ntsc"] is 50
    assert libretro_config["custom_viewport_y_pal"] is 12
    assert libretro_config["custom_viewport_y_ntsc"] is 12


def test_given_overscan_feature_creates_viewport_ntsc_region_config(mocker, system_snes: Emulator):
    givenThisModesFile(mocker, "default:pal:240@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\n"
                               "default:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n"
                               "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"ntsc": ("snes:224@60p", 1820, 200, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] is "23"
    assert libretro_config["custom_viewport_width_ntsc"] is 1820
    assert libretro_config["custom_viewport_height_ntsc"] is 200
    assert libretro_config["custom_viewport_x_ntsc"] is 50
    assert libretro_config["custom_viewport_y_ntsc"] is 12


def test_given_overscan_horizontal_creates_viewport_config(mocker, system_snes: Emulator):
    givenThisModesFile(mocker, "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("snes:224@60p", 1820, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] is "23"
    assert libretro_config["custom_viewport_width_ntsc"] is 1820
    assert libretro_config["custom_viewport_width_pal"] is 1820
    assert libretro_config["custom_viewport_height_ntsc"] is 224
    assert libretro_config["custom_viewport_height_pal"] is 224
    assert libretro_config["custom_viewport_x_ntsc"] is 50
    assert libretro_config["custom_viewport_x_pal"] is 50
    assert libretro_config["custom_viewport_y_ntsc"] is 0
    assert libretro_config["custom_viewport_y_pal"] is 0


def test_given_no_viewport_config_returns_viewport_empty(mocker, system_snes: Emulator):
    givenThisModesFile(mocker, "snes:224@60p,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("snes:224@60p", 0, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_snes, "Mario.smc")
    assert libretro_config["aspect_ratio_index"] is "24"
    assert libretro_config["custom_viewport_width"] is ""
    assert libretro_config["custom_viewport_width_ntsc"] is ""
    assert libretro_config["custom_viewport_width_pal"] is ""
    assert libretro_config["custom_viewport_height_ntsc"] is ""
    assert libretro_config["custom_viewport_height_pal"] is ""
    assert libretro_config["custom_viewport_x"] is ""
    assert libretro_config["custom_viewport_x_ntsc"] is ""
    assert libretro_config["custom_viewport_x_pal"] is ""
    assert libretro_config["custom_viewport_y"] is ""
    assert libretro_config["custom_viewport_y_ntsc"] is ""
    assert libretro_config["custom_viewport_y_pal"] is ""


def test_given_fbneo_game_returns_game_mode(mocker, system_mame: Emulator):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 1920, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_neogeocd_game_returns_neogeo_mode(mocker, system_mame: Emulator):
    givenThisModesFile(mocker, "neogeocdmode,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {"all": ("neogeocdmode", 0, 0, 0)})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 1920, 0, 0)})
    neogeocd = Emulator(name='neogeocd', videoMode='1920x1080', ratio='auto', emulator='libretro',
                        core='fbneo')
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        neogeocd, "/recalbox/share/roms/neogeocd/mslug5.iso")
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2003_game_returns_game_mode(mocker):
    givenThisModesFile(mocker, "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n"
                               "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    system_mame = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2003')
    givenThisGameConfig(mocker,
                        {"fbneo": ("arcade:224@60.000000", 0, 0, 0), "mame2003": ("arcade:240@60.000000", 0, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2003plus_game_returns_game_mode(mocker):
    givenThisModesFile(mocker, "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n"
                               "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    system_mame = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2003_plus')
    givenThisGameConfig(mocker,
                        {"fbneo": ("arcade:224@60.000000", 0, 0, 0), "mame2003": ("arcade:240@60.000000", 0, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_mame2010plus_game_returns_game_mode(mocker, system_mame: Emulator):
    givenThisModesFile(mocker, "arcade:240@60.000000,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,60\n"
                               "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    system_mame = Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mame2010')
    givenThisGameConfig(mocker,
                        {"fbneo": ("arcade:224@60.000000", 0, 0, 0), "mame2010": ("arcade:240@60.000000", 0, 0, 0)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["crt_switch_timings_pal"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'
    assert libretro_config["crt_switch_timings_ntsc"] == '"1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1"'


def test_given_any_system_returns_overscan_active(mocker, system_snes: Emulator):
    givenDefaultsModes(mocker)
    givenDefaultsSystems(mocker)
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(system_snes, "Mario.smc")
    assert libretro_config["video_crop_overscan"] == '"false"'


def test_given_a_vertical_game_and_no_viewport_info_returns_core_1920_ratio(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 0, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "25"


def test_given_a_vertical_game_and_viewport_info_returns_custom_ratio(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 1840, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "23"


def test_given_a_vertical_game_and_system_wide_viewport_info_core_1920_ratio(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 0, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 1000).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["aspect_ratio_index"] == "25"


def test_given_a_vertical_game_returns_bilinear_filtering(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,60")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 0, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 1000).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["video_smooth"] == '"true"'


def test_given_a_mode_with_refresh_rate_then_set_refresh_rate_option_in_retroarch(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,59.899")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 0, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 1000).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["video_refresh_rate_pal"] == '"59.899"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"59.899"'


def test_given_a_mode_with_refresh_rate_then_set_refresh_rate_option_in_retroarch(mocker, system_mame):
    givenThisModesFile(mocker, "arcade:224@60.000000,1920 1 78 192 210 224 1 3 3 16 0 0 0 60 0 37730000 1,59.899")
    givenThisSystemsContent(mocker, {})
    givenThisGameConfig(mocker, {"fbneo": ("arcade:224@60.000000", 0, 0, 1)})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 1000).createConfigFor(
        system_mame, "/recalbox/share/roms/mame/arkbl2.zip")
    assert libretro_config["video_refresh_rate_pal"] == '"59.899"'
    assert libretro_config["video_refresh_rate_ntsc"] == '"59.899"'


def test_given_wonderswan_systems_should_create_config_with_vsync_deactivated(mocker):
    givenDefaultsModes(mocker)
    givenThisSystemsContent(mocker, {})
    libretro_config = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), 0, 0, 0).createConfigFor(
        Emulator(name='wswanc', videoMode='1920x1080', ratio='auto', emulator='libretro', core='mednafen_wswan'),
        "/recalbox/share/roms/wswanc/arkbl2.zip")
    assert libretro_config["video_vsync"] == '"false"'
