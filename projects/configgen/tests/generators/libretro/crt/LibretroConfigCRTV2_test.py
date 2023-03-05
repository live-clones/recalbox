from typing import Optional, Dict
from unittest.mock import mock_open

import pytest

from configgen.Emulator import Emulator, ExtraArguments
from configgen.crt.CRTConfigParser import CRTConfigParser
from configgen.crt.CRTModeGenerator import CRTModeGenerator
from configgen.crt.CRTMonitorRanges import CRTMonitorRanges
from configgen.crt.CRTTypes import CRTSystemMode, CRTArcadeMode, CRTScreenType
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.generators.libretro.crt.LibretroConfigCRT import LibretroConfigCRT
from configgen.settings.keyValueSettings import keyValueSettings
from tests.Givens import givenThoseFiles

MODES_TXT = "/recalbox/share/system/configs/crt/modes.txt"
SYSTEMS_TXT = "/recalbox/share/system/configs/crt/systems.txt"
ARCADE_TXT = "/recalbox/share/system/configs/crt/arcade_games.txt"
ARCADEV2_TXT = "/recalbox/share/system/configs/crt/arcade_games_v2.txt"


def configureForCrt(emulator: Emulator, crtvideostandard="auto", crtresolutiontype="progressive", crtscreentype="15kHz",
                    crtadaptor="recalboxrgbdual", crtregion="auto", crtscanlines=False, crtsuperrez="original",
                    crt_verticaloffset_p320x240=0, rotation=0):
    emulator.configure(keyValueSettings(""),
                       ExtraArguments("", "", "", "", "", "", "", "", crtvideostandard, crtresolutiontype,
                                      crtscreentype,
                                      crtadaptor, crtregion,
                                      crtscanlines, crtsuperrez=crtsuperrez,
                                      crt_verticaloffset_p320x240=crt_verticaloffset_p320x240,
                                      rotation=rotation))
    return emulator


@pytest.fixture
def system_fbneo():
    return Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='fbneo')


@pytest.fixture
def system_mame():
    return Emulator(name='mame', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='mame2015')


def givenDefaultsModesAndSystems(mocker):
    givenThoseFiles(mocker, {
        SYSTEMS_TXT: "",
        MODES_TXT: "default:pal:288@50,1920 1 78 192 210 240 1 3 3 16 0 0 0 50 0 37730000 1,50\ndefault:ntsc:240@60,1920 1 78 192 210 240 1 3 3 16 0 0 0 60 0 37730000 1,50\n"
    })


def test_given_mslug5_on_fbneo_then_return_a_pixel_perfect_mode_in_60hz(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,59.185606,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original")
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    assert config_lines["video_refresh_rate_ntsc"] == '"59.185606"'
    assert config_lines["custom_viewport_width_ntsc"] == 304
    assert config_lines["custom_viewport_height_ntsc"] == 224


def test_given_mslug5_on_fbneo_then_return_an_integer_scaled_config_with_custom_ratio(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,59.185606,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original")
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    assert config_lines["video_scale_integer"] == 'true'
    assert config_lines["aspect_ratio_index"] == '23'


def test_given_mslug5_on_fbneo_then_return_a_mode_with_offset(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,60,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original", crt_verticaloffset_p320x240=1)

    mode = CRTModeGenerator().generate(304, 224, 60, CRTMonitorRanges[CRTScreenType.k15], False)
    mode.v_back_porch += 1
    mode.v_front_porch -= 1
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    assert config_lines["video_scale_integer"] == 'true'
    assert config_lines["aspect_ratio_index"] == '23'
    assert config_lines["crt_switch_timings_ntsc"] == '"{}"'.format(mode.timings())


def test_given_mslug5_on_fbneo_and_15kHExt_range_then_return_a_pixel_perfect_ext_mode(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,59.185606,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original", crtscreentype="15kHzExt")
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    mode = CRTModeGenerator().generate(304, 224, 59.185606, CRTMonitorRanges[CRTScreenType.k15Ext], False)

    assert config_lines["video_refresh_rate_ntsc"] == '"59.185606"'
    assert config_lines["custom_viewport_width_ntsc"] == 304
    assert config_lines["custom_viewport_height_ntsc"] == 224
    assert config_lines["crt_switch_timings_ntsc"] == '"{}"'.format(mode.timings())


def test_given_mslug5_on_fbneo_and_x2_superrez_then_return_a_double_sized_rez(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,59.185606,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="x2")
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    mode = CRTModeGenerator().generate(608, 224, 59.185606, CRTMonitorRanges[CRTScreenType.k15], False)

    assert config_lines["video_refresh_rate_ntsc"] == '"59.185606"'
    assert config_lines["custom_viewport_width_ntsc"] == 608
    assert config_lines["custom_viewport_height_ntsc"] == 224
    assert config_lines["crt_switch_timings_ntsc"] == '"{}"'.format(mode.timings())


def test_given_vertical_game_on_fbneo_then_return_an_adapted_config(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "espgal,fbneo,448,224,59.170000,V\n",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original")
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "espgal.zip")

    assert config_lines["video_refresh_rate_ntsc"] == '"59.17"'
    assert config_lines["aspect_ratio_index"] == "25"
    assert config_lines["video_smooth"] == '"true"'
    assert config_lines["video_scale_integer"] == '"false"'


def test_given_vertical_game_on_fbneo_and_rotate_then_return_a_rotated_config(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "espgal,fbneo,448,224,59.170000,V\n",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original", rotation=1)
    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "espgal.zip")

    assert config_lines["video_refresh_rate_ntsc"] == '"59.17"'
    assert config_lines["aspect_ratio_index"] == "23"
    assert config_lines["video_smooth"] == '"false"'
    assert config_lines["custom_viewport_width_ntsc"] == 448
    assert config_lines["custom_viewport_height_ntsc"] == 224
    assert config_lines["video_allow_rotate"] == '"true"'
    assert config_lines["video_rotation"] == 1

def test_given_mslug5_on_fbneo_with_upsidedown_rotation_then_return_rotated(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,60,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original", rotation=2)

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    assert config_lines["video_rotation"] == 2

def test_given_mslug5_on_fbneo_with_left_rotation_then_return_rotated_left(mocker, system_fbneo):
    givenThoseFiles(mocker, {
        ARCADEV2_TXT: "mslug5,fbneo,304,224,60,H",
    })

    emulator = configureForCrt(system_fbneo, crtsuperrez="original", rotation=1)

    config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter(), True).createConfigFor(emulator,
                                                                                                  "mslug5.zip")
    assert config_lines["aspect_ratio_index"] == "23"
    assert config_lines["video_smooth"] == '"true"'
    assert config_lines["video_scale_integer"] == '"false"'
    assert config_lines["video_rotation"] == 1
    assert config_lines["custom_viewport_width_ntsc"] == 168
    assert config_lines["custom_viewport_height_ntsc"] == 224
    assert config_lines["custom_viewport_x_ntsc"] == 0
    assert config_lines["custom_viewport_y_ntsc"] == 15


# def test_given_mslug5_on_mame_then_return_a_pixel_perfect_mode_in_60hz(mocker, system_fbneo):
#     givenThoseFiles(mocker, {
#         ARCADEV2_TXT: "mslug5,320,224,59.185606,mame2010,H",
#     })
#
#     emulator = configureForCrt(system_fbneo, crtsuperrez="original")
#     config_lines = LibretroConfigCRT(CRTConfigParser(), CRTModeOffsetter()).createConfigFor(emulator, "mslug5.zip")
#     assert config_lines["video_refresh_rate_ntsc"] == '"59.185606"'
#     assert config_lines["custom_viewport_width_ntsc"] == 320
#     assert config_lines["custom_viewport_height_ntsc"] == 240
#
#
