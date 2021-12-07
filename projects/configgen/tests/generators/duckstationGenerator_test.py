#!/usr/bin/env python
import pytest
from configgen.Emulator import Emulator
import configgen.generators.duckstation.duckstationGenerator as duckstationGenerator
from configgen.generators.duckstation.duckstationGenerator import DuckstationGenerator
from configgen.settings.keyValueSettings import keyValueSettings

import configgen.controllers.controller as controllersConfig

from tests.generators.FakeArguments import Arguments


@pytest.fixture
def emulator():
    duckstationGenerator.recalboxFiles.duckstationConfig = 'tests/tmp/duckstation.ini'
    duckstationGenerator.recalboxFiles.SAVES = 'tests/tmp/savedir/'
    controllersConfig.esInputs = 'tests/resources/es_input.cfg'
    return DuckstationGenerator()


@pytest.fixture
def system():
    return Emulator(name='psx', videoMode='1920x1080', ratio='auto', emulator='duckstation', core='duckstation')


@pytest.fixture
def controller_configuration():
    return controllersConfig.Controller.LoadUserControllerConfigurations(
        p1index =-1, p1guid ="060000004c0500006802000000010000", p1name ="PLAYSTATION(R)3 Controller (00:48:E8:D1:63:25)", p1devicepath ="", p1nbaxes = -1, p1nbhats = -1, p1nbbuttons = -1,
        p2index =-1, p2guid ="05000000c82d00000161000000010000", p2name ="8Bitdo SN30 Pro", p2devicepath ="", p2nbaxes = -1, p2nbhats = -1, p2nbbuttons = -1
    )


def test_simple_generate_duckstation(emulator, system, mocker, controller_configuration):
#    mocker.patch("configgen.generators.duckstation.duckstationGenerator.DuckstationsxGenerator.EvdevGetJoystickName", return_value="Microsoft Xbox 360 controller")
    command = emulator.generate(system, controller_configuration, keyValueSettings("", False), Arguments('path/to/test.dsk'))
    assert command.videomode == '1920x1080'
    assert command.array == ['/usr/bin/duckstation', 'path/to/test.dsk']
