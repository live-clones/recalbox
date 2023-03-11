import os
from typing import Optional, Dict
from unittest.mock import mock_open

import pytest

from configgen.Emulator import Emulator, ExtraArguments
import configgen.controllers.controller as controllersConfig
from configgen.crt.CRTConfigParser import CRTConfigParser
from configgen.crt.CRTTypes import CRTSystemMode, CRTArcadeMode
from configgen.crt.CRTModeOffsetter import CRTModeOffsetter
from configgen.generators.libretro.crt.LibretroConfigCRT import LibretroConfigCRT
from configgen.generators.libretro.libretroControllers import LibretroControllers
from configgen.generators.libretro.libretroGenerator import LibretroGenerator
from configgen.settings.keyValueSettings import keyValueSettings
from tests.Givens import givenThoseFiles


@pytest.fixture
def system_fbneo():
    return Emulator(name='fbneo', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='fbneo')
@pytest.fixture
def system_wswanc():
    return Emulator(name='wswanc', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='mednafen_wswan')


def configureForTate(system: Emulator, rotation=0, rotatecontrols=False):
    opts = keyValueSettings("")
    extraArgs = ExtraArguments('1920x1080', "", "", "", "", "", "", "", "", "", "", "", rotation=rotation,
                               rotatecontrols=rotatecontrols)
    system.configure(opts, extraArgs)


def test_given_rotation_0_in_cli_then_dont_rotate(mocker, system_fbneo):
    configureForTate(system_fbneo, rotation=0)
    config, core = LibretroGenerator().createTateModeConfiguration(system_fbneo)

    assert config["video_allow_rotate"] == '"true"'
    assert config["video_rotation"] == 0


def test_given_rotation_1_in_cli_then_rotate(mocker, system_fbneo):
    configureForTate(system_fbneo, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(system_fbneo)

    assert config["video_allow_rotate"] == '"true"'
    assert config["video_rotation"] == 1


def test_given_rotated_then_set4_3_ratio(mocker, system_fbneo):
    configureForTate(system_fbneo, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(system_fbneo)

    assert config["video_allow_rotate"] == '"true"'
    assert config["aspect_ratio_index"] == 0


def test_given_rotated_then_disable_overlays(mocker, system_fbneo):
    configureForTate(system_fbneo, rotation=1)
    config = []
    LibretroGenerator().processOverlays(system_fbneo, "anyrom.zip", config, keyValueSettings(""))
    assert config == []

def test_given_rotated_then_disable_integer_scale(mocker, system_fbneo):
    configureForTate(system_fbneo, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(system_fbneo)

    assert config["video_scale_integer"] == '"false"'


controllersConfig.esInputs = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../resources/es_input.cfg"))

@pytest.fixture
def controller_configuration():
    uuid = "060000004c0500006802000000010000"
    return controllersConfig.Controller.LoadUserControllerConfigurations(
        p1index=-1, p1guid=uuid, p1name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath="", p1nbaxes=-1,
        p1nbhats=-1, p1nbbuttons=-1,
        p2index=-1, p2guid=uuid, p2name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p2devicepath="", p2nbaxes=-1,
        p2nbhats=-1, p2nbbuttons=-1,
        p3index=-1, p3guid=uuid, p3name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p3devicepath="", p3nbaxes=-1,
        p3nbhats=-1, p3nbbuttons=-1,
        p4index=-1, p4guid=uuid, p4name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p4devicepath="", p4nbaxes=-1,
        p4nbhats=-1, p4nbbuttons=-1,
        p5index=-1, p5guid=uuid, p5name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p5devicepath="", p5nbaxes=-1,
        p5nbhats=-1, p5nbbuttons=-1,
        p6index=-1, p6guid=uuid, p6name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p6devicepath="", p6nbaxes=-1,
        p6nbhats=-1, p6nbbuttons=-1,
        p7index=-1, p7guid=uuid, p7name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p7devicepath="", p7nbaxes=-1,
        p7nbhats=-1, p7nbbuttons=-1,
        p8index=-1, p8guid=uuid, p8name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p8devicepath="", p8nbaxes=-1,
        p8nbhats=-1, p8nbbuttons=-1,
        p9index=-1, p9guid=uuid, p9name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p9devicepath="", p9nbaxes=-1,
        p9nbhats=-1, p9nbbuttons=-1,
        p10index=-1, p10guid=uuid, p10name="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p10devicepath="",
        p10nbaxes=-1, p10nbhats=-1, p80nbbuttons=-1
    )


def test_given_rotated_input_then_return_right_joystick_as_left(system_fbneo, controller_configuration):
    lrControllers = LibretroControllers(system_fbneo, keyValueSettings(""), keyValueSettings(""), controller_configuration, False)
    config = lrControllers.fillControllersConfiguration(True)
    assert config.getString('input_player1_l_x_minus_axis', "") == "+3"
    assert config.getString('input_player1_l_x_plus_axis', "") == "-3"
    assert config.getString('input_player1_l_y_minus_axis', "") == "-2"
    assert config.getString('input_player1_l_y_plus_axis', "") == "+2"


def test_given_tate_mode_wonderswan_game_then_configure_core_and_retroarch(system_wswanc, controller_configuration):
    configureForTate(system_wswanc, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(system_wswanc)

    assert config["video_allow_rotate"] == '"true"'
    assert config["video_rotation"] == 2
    assert core["wswan_rotate_keymap"] == '"enabled"'
    assert core["wswan_rotate_display"] == '"manual"'

def test_given_tate_mode_dreamcast_game_then_configure_retroarch_with_specific_rotation(controller_configuration):
    dreamcast = Emulator(name='dreamcast', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='flycast')
    configureForTate(dreamcast, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(dreamcast)

    assert config["video_allow_rotate"] == '"true"'
    assert config["video_rotation"] == 2
    assert config["input_player1_analog_dpad_mode"] == 3

def test_given_tate_mode_namoi_game_then_configure_retroarch(controller_configuration):
    dreamcast = Emulator(name='naomi', videoMode='1920x1080', ratio='auto', emulator='libretro',
                    core='flycast')
    configureForTate(dreamcast, rotation=1)
    config, core = LibretroGenerator().createTateModeConfiguration(dreamcast)

    assert config["video_allow_rotate"] == '"true"'
    assert config["video_rotation"] == 1
    assert config["input_player1_analog_dpad_mode"] == 3
