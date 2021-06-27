#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.linapple.linappleGenerator as linappleGenerator
from configgen.generators.linapple.linappleGenerator import LinappleGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllersConfig as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    #linappleGenerator.recalboxFiles.linappleConfig = 'tests/tmp/linapple.conf'
    return LinappleGenerator('tests/resources/linapple', 'tests/tmp/linapple')


@pytest.fixture
def system():
    return Emulator(name='apple2', videomode='1920x1080', ratio='auto', smooth='1', emulator='linapple')


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


def test_simple_generate_apple_ii(emulator, system, mocker, controller_configuration):
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test'))
    print
    assert command.array == ['/usr/bin/linapple']
