#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
from configgen.generators.kodi.kodiGenerator import KodiGenerator
import configgen.generators.kodi.kodiConfig as kodiConfig
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    kodiConfig.recalboxFiles.kodiJoystick = 'tests/tmp/kodi-joystick/'
    return KodiGenerator()


@pytest.fixture
def system():
    return Emulator(name='kodi', videoMode='1920x1080', ratio='auto', emulator='kodi', core='kodi')


@pytest.fixture
def controller_configuration():
    uuid = "060000004c0500006802000000010000"
    return controllersConfig.Controller.loadControllerConfig("-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*",
                                                             "-1", uuid, "PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", "", "*", "*", "*")


def test_simple_generate(emulator, system, mocker, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/recalbox/scripts/kodilauncher.sh']
